// Created on: 2016-07-04
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

#include <BRepMesh_ModelPreProcessor.hxx>
#include <BRepMesh_Deflection.hxx>
#include <BRepMesh_ShapeTool.hxx>
#include <BRep_Tool.hxx>
#include <IMeshData_Model.hxx>
#include <IMeshData_Edge.hxx>
#include <IMeshData_Wire.hxx>
#include <IMeshData_PCurve.hxx>
#include <OSD_Parallel.hxx>
#include <BRepMesh_ConeRangeSplitter.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_ModelPreProcessor, IMeshTools_ModelAlgo)

namespace
{
  //! Checks consistency of triangulation stored in topological face.
  class TriangulationConsistency
  {
  public:
    //! Constructor
    TriangulationConsistency(const Handle(IMeshData_Model)& theModel,
                             const Standard_Boolean theAllowQualityDecrease)
      : myModel (theModel)
      , myAllowQualityDecrease (theAllowQualityDecrease)
    {
    }

    //! Main functor.
    void operator()(const Standard_Integer theFaceIndex) const
    {
      const IMeshData::IFaceHandle& aDFace = myModel->GetFace(theFaceIndex);
      if (aDFace->IsSet(IMeshData_Outdated))
      {
        return;
      }

      TopLoc_Location aLoc;
      const Handle(Poly_Triangulation)& aTriangulation =
        BRep_Tool::Triangulation(aDFace->GetFace(), aLoc);

      if (!aTriangulation.IsNull())
      {
        Standard_Boolean isTriangulationConsistent = 
          BRepMesh_Deflection::IsConsistent (aTriangulation->Deflection(),
                                             aDFace->GetDeflection(),
                                             myAllowQualityDecrease);

        if (isTriangulationConsistent)
        {
          // #25080: check that indices of links forming triangles are in range.
          const Standard_Integer aNodesNb = aTriangulation->NbNodes();
          const Poly_Array1OfTriangle& aTriangles = aTriangulation->Triangles();

          Standard_Integer i = aTriangles.Lower();
          for (; i <= aTriangles.Upper() && isTriangulationConsistent; ++i)
          {
            const Poly_Triangle& aTriangle = aTriangles(i);

            Standard_Integer aNode[3];
            aTriangle.Get(aNode[0], aNode[1], aNode[2]);
            for (Standard_Integer j = 0; j < 3 && isTriangulationConsistent; ++j)
            {
              isTriangulationConsistent = (aNode[j] >= 1 && aNode[j] <= aNodesNb);
            }
          }
        }

        if (isTriangulationConsistent)
        {
          aDFace->SetStatus(IMeshData_Reused);
          aDFace->SetDeflection(aTriangulation->Deflection());
        }
        else
        {
          aDFace->SetStatus(IMeshData_Outdated);
        }
      }
    }

  private:

    Handle(IMeshData_Model) myModel;
    Standard_Boolean myAllowQualityDecrease; //!< Flag used for consistency check
  };

  //! Adds additional points to seam edges on specific surfaces.
  class SeamEdgeAmplifier
  {
  public:
    //! Constructor
    SeamEdgeAmplifier(const Handle(IMeshData_Model)& theModel,
                      const IMeshTools_Parameters&   theParameters)
      : myModel (theModel)
      , myParameters (theParameters)
    {
    }

    //! Main functor.
    void operator()(const Standard_Integer theFaceIndex) const
    {
      const IMeshData::IFaceHandle& aDFace = myModel->GetFace(theFaceIndex);
      if (aDFace->GetSurface()->GetType() != GeomAbs_Cone)
      {
        return;
      }

      const IMeshData::IWireHandle& aDWire = aDFace->GetWire (0);
      for (Standard_Integer aEdgeIdx = 0; aEdgeIdx < aDWire->EdgesNb() - 1; ++aEdgeIdx)
      {
        const IMeshData::IEdgePtr& aDEdge = aDWire->GetEdge (aEdgeIdx);
        
        if (aDEdge->GetPCurve(aDFace.get(), TopAbs_FORWARD) != aDEdge->GetPCurve(aDFace.get(), TopAbs_REVERSED))
        {
          if (aDEdge->GetCurve()->ParametersNb() == 2)
          {
            if (splitEdge (aDEdge, Abs (getConeStep (aDFace))))
            {
              TopLoc_Location aLoc;
              const Handle (Poly_Triangulation)& aTriangulation =
                BRep_Tool::Triangulation (aDFace->GetFace (), aLoc);

              if (!aTriangulation.IsNull ())
              {
                aDFace->SetStatus (IMeshData_Outdated);
              }
            }
          }
          return;
        } 
      }
    }

  private:

    //! Returns step for splitting seam edge of a cone.
    Standard_Real getConeStep(const IMeshData::IFaceHandle& theDFace) const
    {
      BRepMesh_ConeRangeSplitter aSplitter;
      aSplitter.Reset (theDFace, myParameters);

      const IMeshData::IWireHandle& aDWire = theDFace->GetWire (0);
      for (Standard_Integer aEdgeIt = 0; aEdgeIt < aDWire->EdgesNb(); ++aEdgeIt)
      {
        const IMeshData::IEdgeHandle    aDEdge  = aDWire->GetEdge(aEdgeIt);
        const IMeshData::IPCurveHandle& aPCurve = aDEdge->GetPCurve(
          theDFace.get(), aDWire->GetEdgeOrientation(aEdgeIt));

        for (Standard_Integer aPointIt = 0; aPointIt < aPCurve->ParametersNb(); ++aPointIt)
        {
          const gp_Pnt2d& aPnt2d = aPCurve->GetPoint(aPointIt);
          aSplitter.AddPoint(aPnt2d);
        }
      }

      std::pair<Standard_Integer, Standard_Integer> aStepsNb;
      std::pair<Standard_Real, Standard_Real> aSteps = aSplitter.GetSplitSteps (myParameters, aStepsNb);
      return aSteps.second;
    } 

    //! Splits 3D and all pcurves accoring using the specified step.
    Standard_Boolean splitEdge(const IMeshData::IEdgePtr& theDEdge,
                               const Standard_Real        theDU) const
    {
      if (!splitCurve<gp_XYZ> (theDEdge->GetCurve (), theDU))
      {
        return Standard_False;
      }

      for (Standard_Integer aPCurveIdx = 0; aPCurveIdx < theDEdge->PCurvesNb(); ++aPCurveIdx)
      {
        splitCurve<gp_XY> (theDEdge->GetPCurve (aPCurveIdx), theDU);
      }

      return Standard_True;
    }

    //! Splits the given curve using the specified step.
    template<class PointType, class Curve>
    Standard_Boolean splitCurve(Curve& theCurve, const Standard_Real theDU) const
    {
      Standard_Boolean isUpdated = Standard_False;
      PointType aDir = theCurve->GetPoint(theCurve->ParametersNb() - 1).Coord() - theCurve->GetPoint(0).Coord();
      const Standard_Real aModulus = aDir.Modulus();
      if (aModulus < gp::Resolution())
      {
        return isUpdated;
      }
      aDir /= aModulus;

      const Standard_Real    aLastParam = theCurve->GetParameter(theCurve->ParametersNb() - 1);
      const Standard_Boolean isReversed = theCurve->GetParameter(0) > aLastParam;  
      for (Standard_Integer aPointIdx = 1; ; ++aPointIdx)
      {
        const Standard_Real aCurrParam = theCurve->GetParameter(0) + aPointIdx * theDU * (isReversed ? -1.0 : 1.0); 
        if (( isReversed &&  (aCurrParam < aLastParam)) ||
            (!isReversed && !(aCurrParam < aLastParam)))
        {
          break;
        }

        theCurve->InsertPoint(theCurve->ParametersNb() - 1,
          theCurve->GetPoint(0).Translated (aDir * aPointIdx * theDU),
          aCurrParam);

        isUpdated = Standard_True;
      }

      return isUpdated;
    }

  private:

    Handle(IMeshData_Model) myModel;
    IMeshTools_Parameters   myParameters;
  };
}

//=======================================================================
// Function: Constructor
// Purpose : 
//=======================================================================
BRepMesh_ModelPreProcessor::BRepMesh_ModelPreProcessor()
{
}

//=======================================================================
// Function: Destructor
// Purpose : 
//=======================================================================
BRepMesh_ModelPreProcessor::~BRepMesh_ModelPreProcessor()
{
}

//=======================================================================
// Function: Perform
// Purpose : 
//=======================================================================
Standard_Boolean BRepMesh_ModelPreProcessor::performInternal(
  const Handle(IMeshData_Model)& theModel,
  const IMeshTools_Parameters&   theParameters,
  const Message_ProgressRange&   theRange)
{
  (void )theRange;
  if (theModel.IsNull())
  {
    return Standard_False;
  }

  const Standard_Integer aFacesNb    = theModel->FacesNb();
  const Standard_Boolean isOneThread = !theParameters.InParallel;
  OSD_Parallel::For(0, aFacesNb, SeamEdgeAmplifier        (theModel, theParameters),                      isOneThread);
  OSD_Parallel::For(0, aFacesNb, TriangulationConsistency (theModel, theParameters.AllowQualityDecrease), isOneThread);

  // Clean edges and faces from outdated polygons.
  Handle(NCollection_IncAllocator) aTmpAlloc(new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE));
  NCollection_Map<IMeshData_Face*> aUsedFaces(1, aTmpAlloc);
  for (Standard_Integer aEdgeIt = 0; aEdgeIt < theModel->EdgesNb(); ++aEdgeIt)
  {
    const IMeshData::IEdgeHandle& aDEdge = theModel->GetEdge(aEdgeIt);
    if (aDEdge->IsFree())
    {
      if (aDEdge->IsSet(IMeshData_Outdated))
      {
        TopLoc_Location aLoc;
        BRep_Tool::Polygon3D(aDEdge->GetEdge(), aLoc);
        BRepMesh_ShapeTool::NullifyEdge(aDEdge->GetEdge(), aLoc);
      }

      continue;
    }
    
    for (Standard_Integer aPCurveIt = 0; aPCurveIt < aDEdge->PCurvesNb(); ++aPCurveIt)
    {
      // Find adjacent outdated face.
      const IMeshData::IFaceHandle aDFace = aDEdge->GetPCurve(aPCurveIt)->GetFace();
      if (!aUsedFaces.Contains(aDFace.get()))
      {
        aUsedFaces.Add(aDFace.get());
        if (aDFace->IsSet(IMeshData_Outdated))
        {
          TopLoc_Location aLoc;
          const Handle(Poly_Triangulation)& aTriangulation =
            BRep_Tool::Triangulation(aDFace->GetFace(), aLoc);

          // Clean all edges of oudated face.
          for (Standard_Integer aWireIt = 0; aWireIt < aDFace->WiresNb(); ++aWireIt)
          {
            const IMeshData::IWireHandle& aDWire = aDFace->GetWire(aWireIt);
            for (Standard_Integer aWireEdgeIt = 0; aWireEdgeIt < aDWire->EdgesNb(); ++aWireEdgeIt)
            {
              const IMeshData::IEdgeHandle aTmpDEdge = aDWire->GetEdge(aWireEdgeIt);
              BRepMesh_ShapeTool::NullifyEdge(aTmpDEdge->GetEdge(), aTriangulation, aLoc);
            }
          }

          BRepMesh_ShapeTool::NullifyFace(aDFace->GetFace());
        }
      }
    }
  }

  return Standard_True;
}

