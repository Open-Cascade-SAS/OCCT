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
#include <IMeshData_Model.hxx>
#include <IMeshData_Edge.hxx>
#include <IMeshData_Wire.hxx>
#include <IMeshData_PCurve.hxx>
#include <IMeshTools_Parameters.hxx>
#include <OSD_Parallel.hxx>
#include <BRepMesh_ConeRangeSplitter.hxx>
#include <Poly_TriangulationParameters.hxx>

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
      if (aDFace->IsSet(IMeshData_Outdated) ||
          aDFace->GetFace().IsNull())
      {
        return;
      }

      TopLoc_Location aLoc;
      const Handle(Poly_Triangulation)& aTriangulation =
        BRep_Tool::Triangulation(aDFace->GetFace(), aLoc);

      if (!aTriangulation.IsNull())
      {
        // If there is an info about initial parameters, use it due to deflection kept
        // by Poly_Triangulation is generally an estimation upon generated mesh and can
        // be either less or even greater than specified value.
        const Handle(Poly_TriangulationParameters)& aSourceParams = aTriangulation->Parameters();
        const Standard_Real aDeflection = (!aSourceParams.IsNull() && aSourceParams->HasDeflection()) ?
          aSourceParams->Deflection() : aTriangulation->Deflection();

        Standard_Boolean isTriangulationConsistent = 
          BRepMesh_Deflection::IsConsistent (aDeflection,
                                             aDFace->GetDeflection(),
                                             myAllowQualityDecrease);

        if (isTriangulationConsistent)
        {
          // #25080: check that indices of links forming triangles are in range.
          for (Standard_Integer i = 1; i <= aTriangulation->NbTriangles() && isTriangulationConsistent; ++i)
          {
            const Poly_Triangle aTriangle = aTriangulation->Triangle (i);

            Standard_Integer aNode[3];
            aTriangle.Get(aNode[0], aNode[1], aNode[2]);
            for (Standard_Integer j = 0; j < 3 && isTriangulationConsistent; ++j)
            {
              isTriangulationConsistent = (aNode[j] >= 1 && aNode[j] <= aTriangulation->NbNodes());
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
      if (aDFace->GetSurface()->GetType() != GeomAbs_Cone || aDFace->IsSet(IMeshData_Failure))
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
            if (splitEdge (aDEdge, aDFace, Abs (getConeStep (aDFace))))
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

    //! Splits 3D and all pcurves accordingly using the specified step.
    Standard_Boolean splitEdge(const IMeshData::IEdgePtr&    theDEdge,
                               const IMeshData::IFaceHandle& theDFace,
                               const Standard_Real           theDU) const
    {
      TopoDS_Edge aE = theDEdge->GetEdge();
      const TopoDS_Face& aF = theDFace->GetFace();

      Standard_Real aFParam, aLParam;

      Handle(Geom_Curve) aHC = BRep_Tool::Curve (aE, aFParam, aLParam);

      const IMeshData::IPCurveHandle& aIPC1 = theDEdge->GetPCurve(0);
      const IMeshData::IPCurveHandle& aIPC2 = theDEdge->GetPCurve(1);

      // Calculate the step by parameter of the curve.
      const gp_Pnt2d& aFPntOfIPC1 = aIPC1->GetPoint (0);
      const gp_Pnt2d& aLPntOfIPC1 = aIPC1->GetPoint (aIPC1->ParametersNb() - 1);
      const Standard_Real aMod = Abs (aFPntOfIPC1.Y() - aLPntOfIPC1.Y());

      if (aMod < gp::Resolution())
      {
        return Standard_False;
      }

      const Standard_Real aDT = Abs (aLParam - aFParam) / aMod * theDU;

      if (!splitCurve<gp_Pnt> (aHC, theDEdge->GetCurve(), aDT))
      {
        return Standard_False;
      }

      // Define two pcurves of the seam-edge.
      Handle(Geom2d_Curve) aPC1, aPC2;
      Standard_Real af, al;

      aE.Orientation (TopAbs_FORWARD);
      aPC1 = BRep_Tool::CurveOnSurface (aE, aF, af, al);

      aE.Orientation (TopAbs_REVERSED);
      aPC2 = BRep_Tool::CurveOnSurface (aE, aF, af, al);

      if (aPC1.IsNull() || aPC2.IsNull())
      {
        return Standard_False;
      }

      // Select the correct pcurve of the seam-edge.
      const gp_Pnt2d& aFPntOfPC1 = aPC1->Value (aPC1->FirstParameter());

      if (Abs (aLPntOfIPC1.X() - aFPntOfPC1.X()) > Precision::Confusion())
      {
        std::swap (aPC1, aPC2);
      }

      splitCurve<gp_Pnt2d> (aPC1, aIPC1, aDT);
      splitCurve<gp_Pnt2d> (aPC2, aIPC2, aDT);

      return Standard_True;
    }

    //! Splits the given curve using the specified step.
    template<class PointType, class GeomCurve, class Curve>
    Standard_Boolean splitCurve(GeomCurve&          theGeomCurve, 
                                Curve&              theCurve, 
                                const Standard_Real theDT) const
    {
      Standard_Boolean isUpdated = Standard_False;

      const Standard_Real   aFirstParam = theCurve->GetParameter (0);
      const Standard_Real    aLastParam = theCurve->GetParameter (theCurve->ParametersNb() - 1);
      const Standard_Boolean isReversed = aFirstParam > aLastParam;

      for (Standard_Integer aPointIdx = 1; ; ++aPointIdx)
      {
        const Standard_Real aCurrParam = aFirstParam + aPointIdx * theDT * (isReversed ? -1.0 : 1.0);
        if (( isReversed &&  (aCurrParam - aLastParam < Precision::PConfusion())) ||
            (!isReversed && !(aCurrParam - aLastParam < - Precision::PConfusion())))
        {
          break;
        }

        theCurve->InsertPoint (theCurve->ParametersNb() - 1,
          theGeomCurve->Value (aCurrParam),
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

