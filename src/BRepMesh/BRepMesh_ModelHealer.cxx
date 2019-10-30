// Created on: 2016-06-23
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <BRepMesh_ModelHealer.hxx>
#include <BRepMesh_Deflection.hxx>
#include <BRepMesh_FaceChecker.hxx>
#include <BRepMesh_EdgeDiscret.hxx>
#include <IMeshData_Face.hxx>
#include <IMeshData_Wire.hxx>
#include <IMeshData_Edge.hxx>
#include <IMeshData_PCurve.hxx>
#include <OSD_Parallel.hxx>
#include <TopExp.hxx>
#include <TopoDS_Vertex.hxx>

#ifdef DEBUG_HEALER
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Compound.hxx>
#endif

IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_ModelHealer, IMeshTools_ModelAlgo)

namespace
{
  //! Decreases deflection of the given edge and tries to update discretization.
  class EdgeAmplifier
  {
  public:
    //! Constructor.
    EdgeAmplifier(const IMeshTools_Parameters& theParameters)
      : myParameters(theParameters)
    {
    }

    //! Main operator.
    void operator()(const IMeshData::IEdgePtr& theDEdge) const
    {
      const IMeshData::IEdgeHandle aDEdge = theDEdge;

      Standard_Integer aPointsNb = aDEdge->GetCurve()->ParametersNb();

      aDEdge->Clear(Standard_True);
      aDEdge->SetDeflection(Max(aDEdge->GetDeflection() / 3., Precision::Confusion()));

      for (Standard_Integer aPCurveIt = 0; aPCurveIt < aDEdge->PCurvesNb(); ++aPCurveIt)
      {
        const IMeshData::IPCurveHandle& aPCurve = aDEdge->GetPCurve(aPCurveIt);
        const IMeshData::IFaceHandle    aDFace  = aPCurve->GetFace();

        // Check that outer wire contains 2 edges or less and add an additional point.
        const IMeshData::IWireHandle&   aDWire  = aDFace->GetWire(0);
        if (aDWire->EdgesNb() <= 2)
        {
          ++aPointsNb;
          break;
        }
      }

      const IMeshData::IPCurveHandle& aPCurve = aDEdge->GetPCurve(0);
      const IMeshData::IFaceHandle    aDFace = aPCurve->GetFace();
      Handle(IMeshTools_CurveTessellator) aTessellator =
        BRepMesh_EdgeDiscret::CreateEdgeTessellator(
          aDEdge, aPCurve->GetOrientation(), aDFace,
          myParameters, aPointsNb);

      BRepMesh_EdgeDiscret::Tessellate3d(aDEdge, aTessellator, Standard_False);
      BRepMesh_EdgeDiscret::Tessellate2d(aDEdge, Standard_False);
    }

  private:

    EdgeAmplifier (const EdgeAmplifier& theOther);

    void operator=(const EdgeAmplifier& theOther);

  private:
    const IMeshTools_Parameters& myParameters;
  };

  //! Returns True if some of two vertcies is same with reference one.
  Standard_Boolean isSameWithSomeOf(
    const TopoDS_Vertex& theRefVertex,
    const TopoDS_Vertex& theVertex1,
    const TopoDS_Vertex& theVertex2)
  {
    return (theRefVertex.IsSame(theVertex1) ||
            theRefVertex.IsSame(theVertex2));
  }

  //! Returns True if some of two vertcies is within tolerance of reference one.
  Standard_Boolean isInToleranceWithSomeOf(
    const gp_Pnt& theRefPoint,
    const gp_Pnt& thePoint1,
    const gp_Pnt& thePoint2,
    const Standard_Real theTol)
  {
    const Standard_Real aSqTol = theTol * theTol;
    return (theRefPoint.SquareDistance(thePoint1) < aSqTol ||
            theRefPoint.SquareDistance(thePoint2) < aSqTol);
  }
}

//=======================================================================
// Function: Constructor
// Purpose : 
//=======================================================================
BRepMesh_ModelHealer::BRepMesh_ModelHealer()
{
}

//=======================================================================
// Function: Destructor
// Purpose : 
//=======================================================================
BRepMesh_ModelHealer::~BRepMesh_ModelHealer()
{
}

//=======================================================================
// Function: Perform
// Purpose : 
//=======================================================================
Standard_Boolean BRepMesh_ModelHealer::performInternal(
  const Handle(IMeshData_Model)& theModel,
  const IMeshTools_Parameters&   theParameters,
  const Message_ProgressRange&   theRange)
{
  (void )theRange;
  myModel      = theModel;
  myParameters = theParameters;
  if (myModel.IsNull())
  {
    return Standard_False;
  }

  // MinSize is made as a constant. It is connected with
  // the fact that too rude discretisation can lead to 
  // self-intersecting polygon, which cannot be fixed.
  // As result the face will not be triangulated at all.
  // E.g. see "Test mesh standard_mesh C7", the face #17.
  myParameters.MinSize = Precision::Confusion();

  myFaceIntersectingEdges = new IMeshData::DMapOfIFacePtrsMapOfIEdgePtrs;
  for (Standard_Integer aFaceIt = 0; aFaceIt < myModel->FacesNb(); ++aFaceIt)
  {
    myFaceIntersectingEdges->Bind(myModel->GetFace(aFaceIt).get(), Handle(IMeshData::MapOfIEdgePtr)());
  }

  // TODO: Here we can process edges in order to remove close discrete points.
  OSD_Parallel::For(0, myModel->FacesNb(), *this, !isParallel());
  amplifyEdges();

  IMeshData::DMapOfIFacePtrsMapOfIEdgePtrs::Iterator aFaceIt(*myFaceIntersectingEdges);
  for (; aFaceIt.More(); aFaceIt.Next())
  {
    if (!aFaceIt.Value().IsNull())
    {
      const IMeshData::IFaceHandle aDFace = aFaceIt.Key();
      aDFace->SetStatus(IMeshData_SelfIntersectingWire);
      aDFace->SetStatus(IMeshData_Failure);
    }
  }

  myFaceIntersectingEdges.Nullify();
  myModel.Nullify(); // Do not hold link to model.
  return Standard_True;
}

//=======================================================================
// Function: amplifyEdges
// Purpose : 
//=======================================================================
void BRepMesh_ModelHealer::amplifyEdges()
{
  Handle(NCollection_IncAllocator) aTmpAlloc =
    new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE);

  Standard_Integer aAmpIt = 0;
  const Standard_Real aIterNb = 5;
  IMeshData::MapOfIEdgePtr aEdgesToUpdate(1, aTmpAlloc);
  EdgeAmplifier anEdgeAmplifier (myParameters);

  while (aAmpIt++ < aIterNb && popEdgesToUpdate(aEdgesToUpdate))
  {
    // Try to update discretization by decreasing deflection of problematic edges.
    OSD_Parallel::ForEach(aEdgesToUpdate.cbegin(), aEdgesToUpdate.cend(),
                          anEdgeAmplifier,
                          !(myParameters.InParallel && aEdgesToUpdate.Size() > 1),
                          aEdgesToUpdate.Size());

    IMeshData::MapOfIFacePtr aFacesToCheck(1, aTmpAlloc);
    IMeshData::MapOfIEdgePtr::Iterator aEdgeIt(aEdgesToUpdate);
    for (; aEdgeIt.More(); aEdgeIt.Next())
    {
      const IMeshData::IEdgeHandle aDEdge = aEdgeIt.Value();
      for (Standard_Integer aPCurveIt = 0; aPCurveIt < aDEdge->PCurvesNb(); ++aPCurveIt)
      {
        aFacesToCheck.Add(aDEdge->GetPCurve(aPCurveIt)->GetFace());
      }
    }

    OSD_Parallel::ForEach(aFacesToCheck.cbegin(), aFacesToCheck.cend(),
                          *this, !(myParameters.InParallel && aFacesToCheck.Size() > 1),
                          aFacesToCheck.Size());

    aEdgesToUpdate.Clear();
    aTmpAlloc->Reset(Standard_False);
  }
}

//=======================================================================
// Function: popEdgesToUpdate
// Purpose : 
//=======================================================================
Standard_Boolean BRepMesh_ModelHealer::popEdgesToUpdate(
  IMeshData::MapOfIEdgePtr& theEdgesToUpdate)
{
  IMeshData::DMapOfIFacePtrsMapOfIEdgePtrs::Iterator aFaceIt(*myFaceIntersectingEdges);
  for (; aFaceIt.More(); aFaceIt.Next())
  {
    Handle(IMeshData::MapOfIEdgePtr)& aIntersections = aFaceIt.ChangeValue();
    if (!aIntersections.IsNull())
    {
      theEdgesToUpdate.Unite(*aIntersections);
      aIntersections.Nullify();
    }
  }

  return !theEdgesToUpdate.IsEmpty();
}

//=======================================================================
// Function: process
// Purpose : 
//=======================================================================
void BRepMesh_ModelHealer::process(const IMeshData::IFaceHandle& theDFace) const
{
  try
  {
    OCC_CATCH_SIGNALS

    Handle(IMeshData::MapOfIEdgePtr)& aIntersections = myFaceIntersectingEdges->ChangeFind(theDFace.get());
    aIntersections.Nullify();
  
    fixFaceBoundaries(theDFace);
  
    if (!theDFace->IsSet(IMeshData_Failure))
    {
      BRepMesh_FaceChecker aChecker(theDFace, myParameters);
      if (!aChecker.Perform())
      {
#ifdef DEBUG_HEALER
        std::cout << "Failed : #" << aChecker.GetIntersectingEdges()->Size() << std::endl;
#endif
        aIntersections = aChecker.GetIntersectingEdges();
      }
      else
      {
        if (theDFace->WiresNb () == 1)
        {
          const IMeshData::IWireHandle& aDWire = theDFace->GetWire (0);

          if (aDWire->EdgesNb () == 2)
          {
            const IMeshData::IEdgePtr& aDEdge0 = aDWire->GetEdge (0);
            const IMeshData::IEdgePtr& aDEdge1 = aDWire->GetEdge (1);

            const IMeshData::IPCurveHandle& aPCurve0 = aDEdge0->GetPCurve (theDFace.get (), aDWire->GetEdgeOrientation (0));
            const IMeshData::IPCurveHandle& aPCurve1 = aDEdge1->GetPCurve (theDFace.get (), aDWire->GetEdgeOrientation (1));

            if (aPCurve0->ParametersNb () == 2 && aPCurve1->ParametersNb () == 2)
            {
              aIntersections = new IMeshData::MapOfIEdgePtr;
              // a kind of degenerated face - 1 wire, 2 edges and both edges are very small
              aIntersections->Add (aDEdge0);
              aIntersections->Add (aDEdge1);
            }
          }
        }
      }
    }
  }
  catch (Standard_Failure const&)
  {
    theDFace->SetStatus (IMeshData_Failure);
  }
}

//=======================================================================
// Function: fixFaceBoundaries
// Purpose : 
//=======================================================================
void BRepMesh_ModelHealer::fixFaceBoundaries(const IMeshData::IFaceHandle& theDFace) const
{
#ifdef DEBUG_HEALER
  TopoDS_Compound aComp;
  BRep_Builder aBuilder;
  aBuilder.MakeCompound(aComp);
#endif

  for (int aWireIt = 0; aWireIt < theDFace->WiresNb(); ++aWireIt)
  {
    const IMeshData::IWireHandle& aDWire = theDFace->GetWire(aWireIt);
    BRepMesh_Deflection::ComputeDeflection(aDWire, myParameters);
    for (int aEdgeIt = 0; aEdgeIt < aDWire->EdgesNb(); ++aEdgeIt)
    {
      const int aPrevEdgeIt = (aEdgeIt + aDWire->EdgesNb() - 1) % aDWire->EdgesNb();
      const int aNextEdgeIt = (aEdgeIt + 1) % aDWire->EdgesNb();

      const IMeshData::IEdgeHandle aPrevEdge = aDWire->GetEdge(aPrevEdgeIt);
      const IMeshData::IEdgeHandle aCurrEdge = aDWire->GetEdge(aEdgeIt);
      const IMeshData::IEdgeHandle aNextEdge = aDWire->GetEdge(aNextEdgeIt);

      Standard_Boolean isConnected = !getCommonVertex(aCurrEdge, aNextEdge).IsNull() &&
                                     !getCommonVertex(aPrevEdge, aCurrEdge).IsNull();

      if (isConnected)
      {
        const IMeshData::IPCurveHandle& aPrevPCurve =
          aPrevEdge->GetPCurve(theDFace.get(), aDWire->GetEdgeOrientation(aPrevEdgeIt));

        const IMeshData::IPCurveHandle& aCurrPCurve =
          aCurrEdge->GetPCurve(theDFace.get(), aDWire->GetEdgeOrientation(aEdgeIt));

        const IMeshData::IPCurveHandle& aNextPCurve =
          aNextEdge->GetPCurve(theDFace.get(), aDWire->GetEdgeOrientation(aNextEdgeIt));

        isConnected = connectClosestPoints(aPrevPCurve, aCurrPCurve, aNextPCurve);

#ifdef DEBUG_HEALER
        BRepBuilderAPI_MakePolygon aPoly;
        for (int i = 0; i < aCurrPCurve->ParametersNb(); ++i)
        {
          const gp_Pnt2d& aPnt = aCurrPCurve->GetPoint(i);
          aPoly.Add(gp_Pnt(aPnt.X(), aPnt.Y(), 0.));
        }

        if (aPoly.IsDone())
        {
          aBuilder.Add(aComp, aPoly.Shape());
        }
        TCollection_AsciiString aName("face_discr.brep");
        BRepTools::Write(aComp, aName.ToCString());
#endif
      }

      if (!isConnected || aCurrEdge->IsSet(IMeshData_Outdated))
      {
        // We have to clean face from triangulation.
        theDFace->SetStatus(IMeshData_Outdated);

        if (!isConnected)
        {
          // Just mark wire as open, but continue fixing other inconsistencies
          // in hope that this data could be suitable to build mesh somehow.
          aDWire->SetStatus(IMeshData_OpenWire);
        }
      }
    }
  }

#ifdef DEBUG_HEALER
  TCollection_AsciiString aName    ("face_discr.brep");
  TCollection_AsciiString aFaceName("face_geom.brep");
  BRepTools::Write(aComp, aName.ToCString());
  BRepTools::Write(theDFace->GetFace(), aFaceName.ToCString());
#endif

  BRepMesh_Deflection::ComputeDeflection(theDFace, myParameters);
}

//=======================================================================
// Function: hasCommonVertex
// Purpose : 
//=======================================================================
TopoDS_Vertex BRepMesh_ModelHealer::getCommonVertex(
  const IMeshData::IEdgeHandle& theEdge1,
  const IMeshData::IEdgeHandle& theEdge2) const
{
  TopoDS_Vertex aVertex1_1, aVertex1_2;
  TopExp::Vertices(theEdge1->GetEdge(), aVertex1_1, aVertex1_2);

  //Test bugs moddata_2 bug428.
  //  restore [locate_data_file OCC428.brep] rr
  //  explode rr f
  //  explode rr_91 w
  //  explode rr_91_2 e
  //  nbshapes rr_91_2_2
  //  # 0 vertices; 1 edge

  //This shape is invalid and can lead to exception in this code.

  if (aVertex1_1.IsNull() || aVertex1_2.IsNull())
    return TopoDS_Vertex();

  if (theEdge1->GetEdge().IsSame(theEdge2->GetEdge()))
  {
    return aVertex1_1.IsSame(aVertex1_2) ? aVertex1_1 : TopoDS_Vertex();
  }

  TopoDS_Vertex aVertex2_1, aVertex2_2;
  TopExp::Vertices(theEdge2->GetEdge(), aVertex2_1, aVertex2_2);

  if (aVertex2_1.IsNull() || aVertex2_2.IsNull())
    return TopoDS_Vertex();

  if (isSameWithSomeOf(aVertex1_1, aVertex2_1, aVertex2_2))
  {
    return aVertex1_1;
  }
  else if (isSameWithSomeOf(aVertex1_2, aVertex2_1, aVertex2_2))
  {
    return aVertex1_2;
  }

  const gp_Pnt        aPnt1_1 = BRep_Tool::Pnt(aVertex1_1);
  const gp_Pnt        aPnt1_2 = BRep_Tool::Pnt(aVertex1_2);
  const Standard_Real aTol1_1 = BRep_Tool::Tolerance(aVertex1_1);
  const Standard_Real aTol1_2 = BRep_Tool::Tolerance(aVertex1_2);

  const gp_Pnt        aPnt2_1 = BRep_Tool::Pnt(aVertex2_1);
  const gp_Pnt        aPnt2_2 = BRep_Tool::Pnt(aVertex2_2);
  const Standard_Real aTol2_1 = BRep_Tool::Tolerance(aVertex2_1);
  const Standard_Real aTol2_2 = BRep_Tool::Tolerance(aVertex2_2);

  if (isInToleranceWithSomeOf(aPnt1_1, aPnt2_1, aPnt2_2, aTol1_1 + Max(aTol2_1, aTol2_2)))
  {
    return aVertex1_1;
  }
  else if (isInToleranceWithSomeOf(aPnt1_2, aPnt2_1, aPnt2_2, aTol1_2 + Max(aTol2_1, aTol2_2)))
  {
    return aVertex1_2;
  }

  return TopoDS_Vertex();
}

//=======================================================================
// Function: connectClosestPoints
// Purpose : 
//=======================================================================
Standard_Boolean BRepMesh_ModelHealer::connectClosestPoints(
  const IMeshData::IPCurveHandle& thePrevDEdge,
  const IMeshData::IPCurveHandle& theCurrDEdge,
  const IMeshData::IPCurveHandle& theNextDEdge) const
{
  if (thePrevDEdge->IsInternal() ||
      theCurrDEdge->IsInternal() ||
      theNextDEdge->IsInternal())
  {
    return Standard_True;
  }

  gp_Pnt2d& aPrevFirstUV = thePrevDEdge->GetPoint(0);
  gp_Pnt2d& aPrevLastUV  = thePrevDEdge->GetPoint(thePrevDEdge->ParametersNb() - 1);

  if (thePrevDEdge == theCurrDEdge)
  {
    // Wire consists of a single edge.
    aPrevFirstUV = aPrevLastUV;
    return Standard_True;
  }

  gp_Pnt2d& aCurrFirstUV = theCurrDEdge->GetPoint(0);
  gp_Pnt2d& aCurrLastUV  = theCurrDEdge->GetPoint(theCurrDEdge->ParametersNb() - 1);

  gp_Pnt2d *aPrevUV = NULL, *aCurrPrevUV = NULL;
  const Standard_Real aPrevSqDist = closestPoints(aPrevFirstUV, aPrevLastUV,
                                                  aCurrFirstUV, aCurrLastUV,
                                                  aPrevUV, aCurrPrevUV);

  gp_Pnt2d *aNextUV = NULL, *aCurrNextUV = NULL;
  if (thePrevDEdge == theNextDEdge)
  {
    // Wire consists of two edges. Connect both ends.
    aNextUV     = (aPrevUV     == &aPrevFirstUV) ? &aPrevLastUV : &aPrevFirstUV;
    aCurrNextUV = (aCurrPrevUV == &aCurrFirstUV) ? &aCurrLastUV : &aCurrFirstUV;

    *aNextUV = *aCurrNextUV;
    *aPrevUV = *aCurrPrevUV;
    return Standard_True;
  }

  gp_Pnt2d& aNextFirstUV = theNextDEdge->GetPoint(0);
  gp_Pnt2d& aNextLastUV  = theNextDEdge->GetPoint(theNextDEdge->ParametersNb() - 1);

  const Standard_Real aNextSqDist = closestPoints(aNextFirstUV, aNextLastUV,
                                                  aCurrFirstUV, aCurrLastUV,
                                                  aNextUV, aCurrNextUV);

#ifdef DEBUG_HEALER
  std::cout << "PrevSqDist = " << aPrevSqDist << std::endl;
  std::cout << "NextSqDist = " << aNextSqDist << std::endl;
#endif

  // Connect closest points first. This can help to identify 
  // which ends should be connected in case of gap.
  if (aPrevSqDist - aNextSqDist > gp::Resolution())
  {
    adjustSamePoints(aCurrNextUV, aNextUV, aCurrPrevUV, aPrevUV, aCurrFirstUV, aCurrLastUV, aPrevFirstUV, aPrevLastUV);
  }
  else
  {
    adjustSamePoints(aCurrPrevUV, aPrevUV, aCurrNextUV, aNextUV, aCurrFirstUV, aCurrLastUV, aNextFirstUV, aNextLastUV);
  }

  return Standard_True;
}
