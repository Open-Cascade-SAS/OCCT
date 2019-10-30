// Created on: 2016-04-19
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

#include <BRepMesh_EdgeDiscret.hxx>
#include <BRepMesh_Deflection.hxx>
#include <IMeshData_Model.hxx>
#include <IMeshData_Face.hxx>
#include <IMeshData_PCurve.hxx>
#include <TopExp.hxx>
#include <BRepMesh_ShapeTool.hxx>
#include <BRepMesh_EdgeTessellationExtractor.hxx>
#include <IMeshData_ParametersListArrayAdaptor.hxx>
#include <BRepMesh_CurveTessellator.hxx>
#include <OSD_Parallel.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_EdgeDiscret, IMeshTools_ModelAlgo)

//=======================================================================
// Function: Constructor
// Purpose : 
//=======================================================================
BRepMesh_EdgeDiscret::BRepMesh_EdgeDiscret ()
{
}

//=======================================================================
// Function: Destructor
// Purpose : 
//=======================================================================
BRepMesh_EdgeDiscret::~BRepMesh_EdgeDiscret ()
{
}

//=======================================================================
// Function: CreateFreeEdgeTessellator
// Purpose : 
//=======================================================================
Handle(IMeshTools_CurveTessellator) BRepMesh_EdgeDiscret::CreateEdgeTessellator(
  const IMeshData::IEdgeHandle& theDEdge,
  const IMeshTools_Parameters&  theParameters,
  const Standard_Integer        theMinPointsNb)
{
  return new BRepMesh_CurveTessellator(theDEdge, theParameters, theMinPointsNb);
}

//=======================================================================
// Function: CreateEdgeTessellator
// Purpose : 
//=======================================================================
Handle(IMeshTools_CurveTessellator) BRepMesh_EdgeDiscret::CreateEdgeTessellator(
  const IMeshData::IEdgeHandle& theDEdge,
  const TopAbs_Orientation      theOrientation,
  const IMeshData::IFaceHandle& theDFace,
  const IMeshTools_Parameters&  theParameters,
  const Standard_Integer        theMinPointsNb)
{
  return theDEdge->GetSameParam() ? 
    new BRepMesh_CurveTessellator(theDEdge, theParameters, theMinPointsNb) :
    new BRepMesh_CurveTessellator(theDEdge, theOrientation, theDFace, theParameters, theMinPointsNb);
}

//=======================================================================
// Function: CreateEdgeTessellationExtractor
// Purpose : 
//=======================================================================
Handle(IMeshTools_CurveTessellator) BRepMesh_EdgeDiscret::CreateEdgeTessellationExtractor(
  const IMeshData::IEdgeHandle& theDEdge,
  const IMeshData::IFaceHandle& theDFace)
{
  return new BRepMesh_EdgeTessellationExtractor(theDEdge, theDFace);
}

//=======================================================================
// Function: Perform
// Purpose : 
//=======================================================================
Standard_Boolean BRepMesh_EdgeDiscret::performInternal (
  const Handle (IMeshData_Model)& theModel,
  const IMeshTools_Parameters&    theParameters,
  const Message_ProgressRange&    theRange)
{
  (void )theRange;
  myModel      = theModel;
  myParameters = theParameters;

  if (myModel.IsNull())
  {
    return Standard_False;
  }

  OSD_Parallel::For (0, myModel->EdgesNb (), *this, !myParameters.InParallel);

  myModel.Nullify(); // Do not hold link to model.
  return Standard_True;
}

//=======================================================================
// Function: process
// Purpose : 
//=======================================================================
void BRepMesh_EdgeDiscret::process (const Standard_Integer theEdgeIndex) const
{
  const IMeshData::IEdgeHandle& aDEdge = myModel->GetEdge (theEdgeIndex);
  try
  {
    OCC_CATCH_SIGNALS

    BRepMesh_Deflection::ComputeDeflection (aDEdge, myModel->GetMaxSize (), myParameters);
  
    Handle (IMeshTools_CurveTessellator) aEdgeTessellator;
    if (!aDEdge->IsFree ())
    {
      // Iterate over pcurves and check deflection on corresponding face.
      Standard_Real    aMinDeflection = RealLast ();
      Standard_Integer aMinPCurveIndex = -1;
      for (Standard_Integer aPCurveIt = 0; aPCurveIt < aDEdge->PCurvesNb (); ++aPCurveIt)
      {
        const IMeshData::IPCurveHandle& aPCurve = aDEdge->GetPCurve (aPCurveIt);
        const Standard_Real aTmpDeflection = checkExistingPolygonAndUpdateStatus(aDEdge, aPCurve);
        if (aTmpDeflection < aMinDeflection)
        {
          // Identify pcurve with the smallest deflection in order to
          // retrieve polygon that represents the most smooth discretization.
          aMinDeflection  = aTmpDeflection;
          aMinPCurveIndex = aPCurveIt;
        }
  
        BRepMesh_ShapeTool::CheckAndUpdateFlags (aDEdge, aPCurve);
      }
  
      if (aMinPCurveIndex != -1)
      {
        aDEdge->SetDeflection (aMinDeflection);
        const IMeshData::IFaceHandle aDFace = aDEdge->GetPCurve(aMinPCurveIndex)->GetFace();
        aEdgeTessellator = CreateEdgeTessellationExtractor(aDEdge, aDFace);
      }
      else
      {
        const IMeshData::IPCurveHandle& aPCurve = aDEdge->GetPCurve(0);
        const IMeshData::IFaceHandle    aDFace  = aPCurve->GetFace();
        aEdgeTessellator = BRepMesh_EdgeDiscret::CreateEdgeTessellator(
          aDEdge, aPCurve->GetOrientation(), aDFace, myParameters);
      }
    }
    else
    {
      TopLoc_Location aLoc;
      const Handle (Poly_Polygon3D)& aPoly3D = BRep_Tool::Polygon3D (aDEdge->GetEdge (), aLoc);
      if (!aPoly3D.IsNull ())
      {
        if (aPoly3D->HasParameters() &&
            BRepMesh_Deflection::IsConsistent (aPoly3D->Deflection(),
                                               aDEdge->GetDeflection(),
                                               myParameters.AllowQualityDecrease))
        {
          // Edge already has suitable 3d polygon.
          aDEdge->SetStatus(IMeshData_Reused);
          return;
        }
        else
        {
          aDEdge->SetStatus(IMeshData_Outdated);
        }
      }
  
      aEdgeTessellator = CreateEdgeTessellator(aDEdge, myParameters);
    }
  
    Tessellate3d (aDEdge, aEdgeTessellator, Standard_True);
    if (!aDEdge->IsFree())
    {
      Tessellate2d(aDEdge, Standard_True);
    }
  }
  catch (Standard_Failure const&)
  {
    aDEdge->SetStatus (IMeshData_Failure);
  }
}

//=======================================================================
// Function: checkExistingPolygonAndUpdateStatus
// Purpose : 
//=======================================================================
Standard_Real BRepMesh_EdgeDiscret::checkExistingPolygonAndUpdateStatus(
  const IMeshData::IEdgeHandle&   theDEdge,
  const IMeshData::IPCurveHandle& thePCurve) const
{
  const TopoDS_Edge& aEdge = theDEdge->GetEdge ();
  const TopoDS_Face& aFace = thePCurve->GetFace ()->GetFace ();

  TopLoc_Location aLoc;
  const Handle (Poly_Triangulation)& aFaceTriangulation =
    BRep_Tool::Triangulation (aFace, aLoc);

  Standard_Real aDeflection = RealLast ();
  if (aFaceTriangulation.IsNull())
  {
    return aDeflection;
  }

  const Handle (Poly_PolygonOnTriangulation)& aPolygon =
    BRep_Tool::PolygonOnTriangulation (aEdge, aFaceTriangulation, aLoc);

  if (!aPolygon.IsNull ())
  {
    Standard_Boolean isConsistent = aPolygon->HasParameters() &&
      BRepMesh_Deflection::IsConsistent (aPolygon->Deflection(),
                                         theDEdge->GetDeflection(),
                                         myParameters.AllowQualityDecrease);

    if (!isConsistent)
    {
      // Nullify edge data and mark discrete pcurve to 
      // notify necessity to mesh the entire face.
      theDEdge->SetStatus(IMeshData_Outdated);
    }
    else
    {
      aDeflection = aPolygon->Deflection();
    }
  }

  return aDeflection;
}

//=======================================================================
// Function: Tessellate3d
// Purpose : 
//=======================================================================
void BRepMesh_EdgeDiscret::Tessellate3d(
  const IMeshData::IEdgeHandle&               theDEdge,
  const Handle (IMeshTools_CurveTessellator)& theTessellator,
  const Standard_Boolean                      theUpdateEnds)
{
  // Create 3d polygon.
  const IMeshData::ICurveHandle& aCurve = theDEdge->GetCurve();

  const TopoDS_Edge& aEdge = theDEdge->GetEdge();
  TopoDS_Vertex aFirstVertex, aLastVertex;
  TopExp::Vertices(aEdge, aFirstVertex, aLastVertex);

  if(aFirstVertex.IsNull() || aLastVertex.IsNull())
    return;

  if (theUpdateEnds)
  {
    gp_Pnt aPoint;
    Standard_Real aParam;
    theTessellator->Value(1, aPoint, aParam);
    aCurve->AddPoint(BRep_Tool::Pnt(aFirstVertex), aParam);
  }

  if (!theDEdge->GetDegenerated())
  {
    for (Standard_Integer i = 2; i < theTessellator->PointsNb(); ++i)
    {
      gp_Pnt aPoint;
      Standard_Real aParam;
      if (!theTessellator->Value(i, aPoint, aParam))
        continue;

      if (theUpdateEnds)
      {
        aCurve->AddPoint(aPoint, aParam);
      }
      else
      {
        aCurve->InsertPoint(aCurve->ParametersNb() - 1, aPoint, aParam);
      }
    }
  }

  if (theUpdateEnds)
  {
    gp_Pnt aPoint;
    Standard_Real aParam;
    theTessellator->Value(theTessellator->PointsNb(), aPoint, aParam);
    aCurve->AddPoint(BRep_Tool::Pnt(aLastVertex), aParam);
  }
}

//=======================================================================
// Function: Tessellate2d
// Purpose : 
//=======================================================================
void BRepMesh_EdgeDiscret::Tessellate2d(
  const IMeshData::IEdgeHandle& theDEdge,
  const Standard_Boolean        theUpdateEnds)
{
  const IMeshData::ICurveHandle& aCurve = theDEdge->GetCurve();
  for (Standard_Integer aPCurveIt = 0; aPCurveIt < theDEdge->PCurvesNb(); ++aPCurveIt)
  {
    const IMeshData::IPCurveHandle& aPCurve = theDEdge->GetPCurve(aPCurveIt);
    const IMeshData::IFaceHandle    aDFace  = aPCurve->GetFace();
    IMeshData::ICurveArrayAdaptorHandle aCurveArray(new IMeshData::ICurveArrayAdaptor(aCurve));
    BRepMesh_EdgeParameterProvider<IMeshData::ICurveArrayAdaptorHandle> aProvider(
      theDEdge, aPCurve->GetOrientation(), aDFace, aCurveArray);

    const Handle(Adaptor2d_Curve2d)& aGeomPCurve = aProvider.GetPCurve();

    Standard_Integer aParamIdx, aParamNb;
    if (theUpdateEnds)
    {
      aParamIdx = 0;
      aParamNb  = aCurve->ParametersNb();
    }
    else
    {
      aParamIdx = 1;
      aParamNb  = aCurve->ParametersNb() - 1;
    }

    for (; aParamIdx < aParamNb; ++aParamIdx)
    {
      const Standard_Real aParam = aProvider.Parameter(aParamIdx, aCurve->GetPoint(aParamIdx));

      gp_Pnt2d aPoint2d;
      aGeomPCurve->D0(aParam, aPoint2d);
      if (theUpdateEnds)
      {
        aPCurve->AddPoint(aPoint2d, aParam);
      }
      else
      {
        aPCurve->InsertPoint(aPCurve->ParametersNb() - 1, aPoint2d, aParam);
      }
    }
  }
}
