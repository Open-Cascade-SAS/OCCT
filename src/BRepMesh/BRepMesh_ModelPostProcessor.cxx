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

#include <BRepMesh_ModelPostProcessor.hxx>

#include <BRepMesh_ShapeTool.hxx>
#include <IMeshData_Model.hxx>
#include <IMeshData_Edge.hxx>
#include <IMeshData_PCurve.hxx>
#include <IMeshTools_Parameters.hxx>
#include <OSD_Parallel.hxx>
#include <BRepLib.hxx>
#include <Poly_TriangulationParameters.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_ModelPostProcessor, IMeshTools_ModelAlgo)

namespace
{
  //! Commits 3D polygons and polygons on triangulations for corresponding edges.
  class PolygonCommitter
  {
  public:
    //! Constructor
    PolygonCommitter(const Handle(IMeshData_Model)& theModel)
      : myModel(theModel)
    {
    }

    //! Main functor.
    void operator()(const Standard_Integer theEdgeIndex) const
    {
      const IMeshData::IEdgeHandle& aDEdge = myModel->GetEdge(theEdgeIndex);
      if (aDEdge->GetCurve()->ParametersNb() == 0)
        return;

      if (aDEdge->IsFree())
      {
        if (!aDEdge->IsSet(IMeshData_Reused))
        {
          commitPolygon3D(aDEdge);
        }
      }
      else
      {
        commitPolygons(aDEdge);
      }
    }

  private:

    //! Commits 3d polygon to topological edge
    void commitPolygon3D(const IMeshData::IEdgeHandle& theDEdge) const
    {
      const IMeshData::ICurveHandle& aCurve = theDEdge->GetCurve();

      TColgp_Array1OfPnt   aNodes  (1, aCurve->ParametersNb());
      TColStd_Array1OfReal aUVNodes(1, aCurve->ParametersNb());
      for (Standard_Integer i = 1; i <= aCurve->ParametersNb(); ++i)
      {
        aNodes  (i) = aCurve->GetPoint    (i - 1);
        aUVNodes(i) = aCurve->GetParameter(i - 1);
      }

      Handle(Poly_Polygon3D) aPoly3D = new Poly_Polygon3D(aNodes, aUVNodes);
      aPoly3D->Deflection(theDEdge->GetDeflection());

      BRepMesh_ShapeTool::UpdateEdge(theDEdge->GetEdge(), aPoly3D);
    }

    //! Commits all polygons on triangulations correspondent to the given edge.
    void commitPolygons(const IMeshData::IEdgeHandle& theDEdge) const
    {
      // Collect pcurves associated with the given edge on the specific surface.
      IMeshData::IDMapOfIFacePtrsListOfIPCurves aMapOfPCurves;
      for (Standard_Integer aPCurveIt = 0; aPCurveIt < theDEdge->PCurvesNb(); ++aPCurveIt)
      {
        const IMeshData::IPCurveHandle& aPCurve   = theDEdge->GetPCurve(aPCurveIt);
        const IMeshData::IFacePtr&      aDFacePtr = aPCurve->GetFace();
        const IMeshData::IFaceHandle    aDFace    = aDFacePtr;
        if (aDFace->IsSet(IMeshData_Failure) ||
            aDFace->IsSet(IMeshData_Reused))
        {
          continue;
        }

        if (!aMapOfPCurves.Contains(aDFacePtr))
        {
          aMapOfPCurves.Add(aDFacePtr, IMeshData::ListOfIPCurves());
        }

        IMeshData::ListOfIPCurves& aPCurves = aMapOfPCurves.ChangeFromKey(aDFacePtr);
        aPCurves.Append(aPCurve);
      }

      // Commit polygons related to separate face.
      const TopoDS_Edge& aEdge = theDEdge->GetEdge();
      IMeshData::IDMapOfIFacePtrsListOfIPCurves::Iterator aPolygonIt(aMapOfPCurves);
      for (; aPolygonIt.More(); aPolygonIt.Next())
      {
        const TopoDS_Face& aFace = aPolygonIt.Key()->GetFace();

        TopLoc_Location aLoc;
        const Handle(Poly_Triangulation)& aTriangulation =
          BRep_Tool::Triangulation(aFace, aLoc);

        if (!aTriangulation.IsNull())
        {
          const IMeshData::ListOfIPCurves& aPCurves = aPolygonIt.Value();
          if (aPCurves.Size() == 2)
          {
            BRepMesh_ShapeTool::UpdateEdge(
              aEdge,
              collectPolygon(aPCurves.First(), theDEdge->GetDeflection()),
              collectPolygon(aPCurves.Last (), theDEdge->GetDeflection()),
              aTriangulation, aLoc);
          }
          else
          {
            BRepMesh_ShapeTool::UpdateEdge(
              aEdge,
              collectPolygon(aPCurves.First(), theDEdge->GetDeflection()),
              aTriangulation, aLoc);
          }
        }
      }
    }

    //! Collects polygonal data for the given pcurve
    Handle(Poly_PolygonOnTriangulation) collectPolygon(
      const IMeshData::IPCurveHandle& thePCurve,
      const Standard_Real             theDeflection) const
    {
      TColStd_Array1OfInteger aNodes (1, thePCurve->ParametersNb());
      TColStd_Array1OfReal    aParams(1, thePCurve->ParametersNb());
      for (Standard_Integer i = 1; i <= thePCurve->ParametersNb(); ++i)
      {
        aNodes (i) = thePCurve->GetIndex    (i - 1);
        aParams(i) = thePCurve->GetParameter(i - 1);
      }

      Handle(Poly_PolygonOnTriangulation) aPolygon = 
        new Poly_PolygonOnTriangulation(aNodes, aParams);

      aPolygon->Deflection(theDeflection);
      return aPolygon;
    }

  private:

    Handle(IMeshData_Model) myModel;
  };

  //! Estimates and updates deflection of triangulations for corresponding faces.
  class DeflectionEstimator
  {
  public:
    //! Constructor
    DeflectionEstimator (const Handle(IMeshData_Model)& theModel,
                         const IMeshTools_Parameters&   theParams)
      : myModel  (theModel)
      , myParams (new Poly_TriangulationParameters (
          theParams.Deflection, theParams.Angle, theParams.MinSize))
    {
    }

    //! Main functor.
    void operator()(const Standard_Integer theFaceIndex) const
    {
      const IMeshData::IFaceHandle& aDFace = myModel->GetFace (theFaceIndex);
      if (aDFace->IsSet (IMeshData_Failure) ||
          aDFace->IsSet (IMeshData_Reused))
      {
        return;
      }

      BRepLib::UpdateDeflection (aDFace->GetFace());

      TopLoc_Location aLoc;
      const Handle(Poly_Triangulation)& aTriangulation =
        BRep_Tool::Triangulation (aDFace->GetFace(), aLoc);
      
      if (!aTriangulation.IsNull())
      {
        aTriangulation->Parameters (myParams);
      }
    }

  private:

    Handle(IMeshData_Model)              myModel;
    Handle(Poly_TriangulationParameters) myParams;
  };
}

//=======================================================================
// Function: Constructor
// Purpose : 
//=======================================================================
BRepMesh_ModelPostProcessor::BRepMesh_ModelPostProcessor()
{
}

//=======================================================================
// Function: Destructor
// Purpose : 
//=======================================================================
BRepMesh_ModelPostProcessor::~BRepMesh_ModelPostProcessor()
{
}

//=======================================================================
// Function: Perform
// Purpose : 
//=======================================================================
Standard_Boolean BRepMesh_ModelPostProcessor::performInternal(
  const Handle(IMeshData_Model)& theModel,
  const IMeshTools_Parameters&   theParameters,
  const Message_ProgressRange&   theRange)
{
  (void )theRange;
  if (theModel.IsNull())
  {
    return Standard_False;
  }

  // TODO: Force single threaded solution due to data races on edges sharing the same TShape
  OSD_Parallel::For (0, theModel->EdgesNb(), PolygonCommitter (theModel), Standard_True/*!theParameters.InParallel*/);

  // Estimate deflection here due to BRepLib::EstimateDeflection requires
  // existence of both Poly_Triangulation and Poly_PolygonOnTriangulation.
  OSD_Parallel::For (0, theModel->FacesNb(), DeflectionEstimator (theModel, theParameters), !theParameters.InParallel);
  return Standard_True;
}
