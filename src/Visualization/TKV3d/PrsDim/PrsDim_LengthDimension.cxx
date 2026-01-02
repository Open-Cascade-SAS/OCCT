// Created on: 1996-12-05
// Created by: Arnaud BOUZY/Odile Olivier
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2013 OPEN CASCADE SAS
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

#include <PrsDim_LengthDimension.hxx>

#include <PrsDim.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <BRepTools.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <gce_MakeDir.hxx>
#include <gce_MakePln.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ExtremaSurfaceSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(PrsDim_LengthDimension, PrsDim_Dimension)

//=================================================================================================

PrsDim_LengthDimension::PrsDim_LengthDimension()
    : PrsDim_Dimension(PrsDim_KOD_LENGTH),
      myHasCustomDirection(false)
{
  SetFlyout(15.0);
}

//=======================================================================
// function : Constructor
// purpose  : Dimension between two faces
//=======================================================================
PrsDim_LengthDimension::PrsDim_LengthDimension(const TopoDS_Face& theFirstFace,
                                               const TopoDS_Face& theSecondFace)
    : PrsDim_Dimension(PrsDim_KOD_LENGTH),
      myHasCustomDirection(false)
{
  SetMeasuredGeometry(theFirstFace, theSecondFace);
  SetFlyout(15.0);
}

//=======================================================================
// function : Constructor
// purpose  : Dimension between two shape
//=======================================================================
PrsDim_LengthDimension::PrsDim_LengthDimension(const TopoDS_Face& theFace,
                                               const TopoDS_Edge& theEdge)
    : PrsDim_Dimension(PrsDim_KOD_LENGTH),
      myHasCustomDirection(false)
{
  SetMeasuredGeometry(theFace, theEdge);
  SetFlyout(15.0);
}

//=======================================================================
// function : Constructor
// purpose  : Dimension between two points
//=======================================================================
PrsDim_LengthDimension::PrsDim_LengthDimension(const gp_Pnt& theFirstPoint,
                                               const gp_Pnt& theSecondPoint,
                                               const gp_Pln& thePlane)
    : PrsDim_Dimension(PrsDim_KOD_LENGTH),
      myHasCustomDirection(false)
{
  SetMeasuredGeometry(theFirstPoint, theSecondPoint, thePlane);
  SetFlyout(15.0);
}

//=======================================================================
// function : Constructor
// purpose  : Dimension between two shape
//=======================================================================
PrsDim_LengthDimension::PrsDim_LengthDimension(const TopoDS_Shape& theFirstShape,
                                               const TopoDS_Shape& theSecondShape,
                                               const gp_Pln&       thePlane)
    : PrsDim_Dimension(PrsDim_KOD_LENGTH),
      myHasCustomDirection(false)
{
  SetCustomPlane(thePlane);
  SetMeasuredShapes(theFirstShape, theSecondShape);
  SetFlyout(15.0);
}

//=================================================================================================

PrsDim_LengthDimension::PrsDim_LengthDimension(const TopoDS_Edge& theEdge, const gp_Pln& thePlane)
    : PrsDim_Dimension(PrsDim_KOD_LENGTH),
      myHasCustomDirection(false)
{
  SetMeasuredGeometry(theEdge, thePlane);
  SetFlyout(15.0);
}

//=================================================================================================

void PrsDim_LengthDimension::SetMeasuredGeometry(const gp_Pnt& theFirstPoint,
                                                 const gp_Pnt& theSecondPoint,
                                                 const gp_Pln& thePlane)
{
  myFirstPoint   = theFirstPoint;
  mySecondPoint  = theSecondPoint;
  myFirstShape   = BRepLib_MakeVertex(myFirstPoint);
  mySecondShape  = BRepLib_MakeVertex(mySecondPoint);
  myGeometryType = GeometryType_Points;
  SetCustomPlane(thePlane);
  myIsGeometryValid = IsValidPoints(theFirstPoint, theSecondPoint);

  SetToUpdate();
}

//=================================================================================================

void PrsDim_LengthDimension::SetMeasuredGeometry(const TopoDS_Edge& theEdge, const gp_Pln& thePlane)
{
  myFirstShape   = theEdge;
  mySecondShape  = TopoDS_Shape();
  myGeometryType = GeometryType_Edge;
  SetCustomPlane(thePlane);
  myIsGeometryValid = InitOneShapePoints(myFirstShape);

  SetToUpdate();
}

//=================================================================================================

void PrsDim_LengthDimension::SetMeasuredGeometry(const TopoDS_Face& theFirstFace,
                                                 const TopoDS_Face& theSecondFace)
{
  SetMeasuredShapes(theFirstFace, theSecondFace);
}

//=================================================================================================

void PrsDim_LengthDimension::SetMeasuredGeometry(const TopoDS_Face& theFace,
                                                 const TopoDS_Edge& theEdge)
{
  SetMeasuredShapes(theFace, theEdge);
}

//=================================================================================================

void PrsDim_LengthDimension::SetMeasuredShapes(const TopoDS_Shape& theFirstShape,
                                               const TopoDS_Shape& theSecondShape)
{
  gp_Pln aComputedPlane;
  bool   isPlaneReturned = false;

  myFirstShape  = theFirstShape;
  mySecondShape = theSecondShape;
  myIsGeometryValid =
    InitTwoShapesPoints(myFirstShape, mySecondShape, aComputedPlane, isPlaneReturned);

  if (myIsGeometryValid && !myIsPlaneCustom)
  {
    if (isPlaneReturned)
    {
      myPlane = aComputedPlane;
    }
    else
    {
      myIsGeometryValid = false;
    }
  }

  SetToUpdate();
}

//=================================================================================================

bool PrsDim_LengthDimension::CheckPlane(const gp_Pln& thePlane) const
{
  bool anIsFaultyNormal =
    thePlane.Axis().Direction().IsParallel(gce_MakeDir(myFirstPoint, mySecondPoint),
                                           Precision::Angular());

  return !((!thePlane.Contains(myFirstPoint, Precision::Confusion())
       && !thePlane.Contains(mySecondPoint, Precision::Confusion()))
      || anIsFaultyNormal);
}

//=================================================================================================

gp_Pln PrsDim_LengthDimension::ComputePlane(const gp_Dir& theAttachDir) const
{
  if (!IsValidPoints(myFirstPoint, mySecondPoint))
  {
    return gp_Pln();
  }

  gp_Pnt      aThirdPoint(myFirstPoint.Translated(gp_Vec(theAttachDir)));
  gce_MakePln aPlaneConstrustor(myFirstPoint, mySecondPoint, aThirdPoint);
  return aPlaneConstrustor.Value();
}

//=================================================================================================

const TCollection_AsciiString& PrsDim_LengthDimension::GetModelUnits() const
{
  return myDrawer->DimLengthModelUnits();
}

//=================================================================================================

const TCollection_AsciiString& PrsDim_LengthDimension::GetDisplayUnits() const
{
  return myDrawer->DimLengthDisplayUnits();
}

//=================================================================================================

void PrsDim_LengthDimension::SetModelUnits(const TCollection_AsciiString& theUnits)
{
  myDrawer->SetDimLengthModelUnits(theUnits);
}

//=================================================================================================

void PrsDim_LengthDimension::SetDisplayUnits(const TCollection_AsciiString& theUnits)
{
  myDrawer->SetDimLengthDisplayUnits(theUnits);
}

//=================================================================================================

double PrsDim_LengthDimension::ComputeValue() const
{
  if (!IsValid())
    return 0.0;

  if (!myHasCustomDirection)
    return myFirstPoint.Distance(mySecondPoint);

  return fabs(gp_Vec(myFirstPoint, mySecondPoint).Dot(myDirection));
}

//=================================================================================================

void PrsDim_LengthDimension::Compute(const occ::handle<PrsMgr_PresentationManager>&,
                                     const occ::handle<Prs3d_Presentation>& thePresentation,
                                     const int                              theMode)
{
  mySelectionGeom.Clear(theMode);

  if (!IsValid())
  {
    return;
  }

  DrawLinearDimension(thePresentation, theMode, myFirstPoint, mySecondPoint);
}

//=================================================================================================

void PrsDim_LengthDimension::ComputeFlyoutLinePoints(const gp_Pnt& theFirstPoint,
                                                     const gp_Pnt& theSecondPoint,
                                                     gp_Pnt&       theLineBegPoint,
                                                     gp_Pnt&       theLineEndPoint)
{
  if (!myHasCustomDirection)
  {
    PrsDim_Dimension::ComputeFlyoutLinePoints(theFirstPoint,
                                              theSecondPoint,
                                              theLineBegPoint,
                                              theLineEndPoint);
    return;
  }

  // find scalar of projection target vector (from start to second point) to flyout vector
  gp_Ax1 aPlaneNormal = GetPlane().Axis();
  gp_Vec aFlyoutNormalizedDir(aPlaneNormal.Direction() ^ myDirection);
  aFlyoutNormalizedDir.Normalize();
  double aTargetProjectedToFlyout = gp_Vec(theFirstPoint, theSecondPoint).Dot(aFlyoutNormalizedDir);

  gp_Dir aFlyoutVector = aFlyoutNormalizedDir;
  // create lines for layouts
  gp_Lin aLine1(theFirstPoint, aFlyoutVector);
  gp_Lin aLine2(theSecondPoint, aFlyoutVector);

  // Get flyout end points
  theLineBegPoint =
    ElCLib::Value(ElCLib::Parameter(aLine1, theFirstPoint) + GetFlyout() + aTargetProjectedToFlyout,
                  aLine1);
  theLineEndPoint = ElCLib::Value(ElCLib::Parameter(aLine2, theSecondPoint) + GetFlyout(), aLine2);
}

//=================================================================================================

void PrsDim_LengthDimension::ComputeFlyoutSelection(
  const occ::handle<SelectMgr_Selection>&   theSelection,
  const occ::handle<SelectMgr_EntityOwner>& theEntityOwner)
{
  if (!IsValid())
  {
    return;
  }

  ComputeLinearFlyouts(theSelection, theEntityOwner, myFirstPoint, mySecondPoint);
}

//=================================================================================================

bool PrsDim_LengthDimension::IsValidPoints(const gp_Pnt& theFirstPoint,
                                           const gp_Pnt& theSecondPoint) const
{
  return theFirstPoint.Distance(theSecondPoint) > Precision::Confusion();
}

//=======================================================================
// function : InitTwoEdgesLength
// purpose  : Initialization of dimension between two linear edges
//=======================================================================
bool PrsDim_LengthDimension::InitTwoEdgesLength(const TopoDS_Edge& theFirstEdge,
                                                const TopoDS_Edge& theSecondEdge,
                                                gp_Dir&            theDirAttach)
{
  occ::handle<Geom_Curve> aFirstCurve, aSecondCurve;
  gp_Pnt                  aPoint11, aPoint12, aPoint21, aPoint22;
  bool                    isFirstInfinite  = false;
  bool                    isSecondInfinite = false;
  if (!PrsDim::ComputeGeometry(theFirstEdge,
                               theSecondEdge,
                               aFirstCurve,
                               aSecondCurve,
                               aPoint11,
                               aPoint12,
                               aPoint21,
                               aPoint22,
                               isFirstInfinite,
                               isSecondInfinite))
  {
    return false;
  }

  const occ::handle<Geom_Line> aFirstLine  = occ::down_cast<Geom_Line>(aFirstCurve);
  const occ::handle<Geom_Line> aSecondLine = occ::down_cast<Geom_Line>(aSecondCurve);
  if (!aFirstLine.IsNull() && !aSecondLine.IsNull())
  {
    if (!aFirstLine->Lin().Direction().IsParallel(aSecondLine->Lin().Direction(),
                                                  Precision::Angular()))
    {
      return false;
    }

    theDirAttach = aFirstLine->Lin().Direction();

    gp_Pnt aPoint;
    if (!isFirstInfinite)
    {
      if (PrsDim::Nearest(aSecondCurve, aPoint11, aPoint21, aPoint22, aPoint))
      {
        myFirstPoint  = aPoint11;
        mySecondPoint = aPoint;
        return IsValidPoints(myFirstPoint, mySecondPoint);
      }
      else if (PrsDim::Nearest(aSecondCurve, aPoint12, aPoint21, aPoint22, aPoint))
      {
        myFirstPoint  = aPoint12;
        mySecondPoint = aPoint;
        return IsValidPoints(myFirstPoint, mySecondPoint);
      }
    }

    if (!isSecondInfinite)
    {
      if (PrsDim::Nearest(aFirstCurve, aPoint21, aPoint11, aPoint12, aPoint))
      {
        myFirstPoint  = aPoint;
        mySecondPoint = aPoint21;
        return IsValidPoints(myFirstPoint, mySecondPoint);
      }
      if (PrsDim::Nearest(aFirstCurve, aPoint22, aPoint11, aPoint12, aPoint))
      {
        myFirstPoint  = aPoint;
        mySecondPoint = aPoint22;
        return IsValidPoints(myFirstPoint, mySecondPoint);
      }
    }
  }

  GeomAPI_ExtremaCurveCurve anExtrema(aFirstCurve, aSecondCurve);
  if (anExtrema.NbExtrema() == 0)
  {
    return false;
  }

  anExtrema.NearestPoints(myFirstPoint, mySecondPoint);
  if (!IsValidPoints(myFirstPoint, mySecondPoint))
  {
    return false;
  }

  if (aFirstLine.IsNull() || aSecondLine.IsNull())
  {
    double aParam1 = 0.0, aParam2 = 0.0;
    anExtrema.LowerDistanceParameters(aParam1, aParam2);
    BRepAdaptor_Curve aCurveAdaptor(theFirstEdge);
    gp_Pnt            aPoint;
    gp_Vec            aDir;
    aCurveAdaptor.D1(aParam1, aPoint, aDir);
    if (aDir.SquareMagnitude() <= gp::Resolution())
    {
      return false;
    }

    theDirAttach = aDir;
  }
  return true;
}

//=======================================================================
// function : InitEdgeVertexLength
// purpose  : for first edge and second vertex shapes
//=======================================================================
bool PrsDim_LengthDimension::InitEdgeVertexLength(const TopoDS_Edge&   theEdge,
                                                  const TopoDS_Vertex& theVertex,
                                                  gp_Dir&              theEdgeDir,
                                                  bool                 isInfinite)
{
  gp_Pnt                  anEdgePoint1, anEdgePoint2;
  occ::handle<Geom_Curve> aCurve;
  if (!PrsDim::ComputeGeometry(theEdge, aCurve, anEdgePoint1, anEdgePoint2, isInfinite))
  {
    return false;
  }

  myFirstPoint = BRep_Tool::Pnt(theVertex);

  if (occ::handle<Geom_Line> aGeomLine = occ::down_cast<Geom_Line>(aCurve))
  {
    const gp_Lin aLin = aGeomLine->Lin();

    // Get direction of edge to build plane automatically.
    theEdgeDir = aLin.Direction();

    mySecondPoint = PrsDim::Nearest(aLin, myFirstPoint);
    return IsValidPoints(myFirstPoint, mySecondPoint);
  }

  GeomAPI_ProjectPointOnCurve anExtrema(myFirstPoint, aCurve);
  if (anExtrema.NbPoints() == 0)
  {
    return false;
  }

  mySecondPoint = anExtrema.NearestPoint();
  if (!IsValidPoints(myFirstPoint, mySecondPoint))
  {
    return false;
  }

  BRepAdaptor_Curve aCurveAdaptor(theEdge);
  gp_Pnt            aPoint;
  gp_Vec            aDir;
  aCurveAdaptor.D1(anExtrema.LowerDistanceParameter(), aPoint, aDir);
  if (aDir.SquareMagnitude() <= gp::Resolution())
  {
    return false;
  }
  theEdgeDir = aDir;
  return true;
}

//=================================================================================================

bool PrsDim_LengthDimension::InitEdgeFaceLength(const TopoDS_Edge& theEdge,
                                                const TopoDS_Face& theFace,
                                                gp_Dir&            theEdgeDir)
{
  theEdgeDir = gp::DX();

  // Find attachment points (closest distance between the edge and the face)
  BRepExtrema_DistShapeShape aDistAdaptor(theEdge, theFace, Extrema_ExtFlag_MIN);
  if (!aDistAdaptor.IsDone() || aDistAdaptor.NbSolution() < 1)
  {
    return false;
  }
  myFirstPoint  = aDistAdaptor.PointOnShape1(1);
  mySecondPoint = aDistAdaptor.PointOnShape2(1);

  // Take direction for dimension line (will be orthogonalized later) parallel to edge
  BRepAdaptor_Curve aCurveAdaptor(theEdge);
  double            aParam;
  if (aDistAdaptor.SupportOnShape1(1).ShapeType() == TopAbs_EDGE)
  {
    aDistAdaptor.ParOnEdgeS1(1, aParam);
  }
  else
  {
    double aD1 = aCurveAdaptor.Value(aCurveAdaptor.FirstParameter()).SquareDistance(myFirstPoint);
    double aD2 = aCurveAdaptor.Value(aCurveAdaptor.LastParameter()).SquareDistance(myFirstPoint);
    aParam     = (aD1 < aD2 ? aCurveAdaptor.FirstParameter() : aCurveAdaptor.LastParameter());
  }
  gp_Pnt aP;
  gp_Vec aV;
  aCurveAdaptor.D1(aParam, aP, aV);
  if (aV.SquareMagnitude() > gp::Resolution())
  {
    theEdgeDir = aV;
  }

  // reverse direction if parameter is close to the end of the curve,
  // to reduce chances to have overlapping between dimension line and edge
  if (std::abs(aParam - aCurveAdaptor.FirstParameter())
      < std::abs(aParam - aCurveAdaptor.LastParameter()))
  {
    theEdgeDir.Reverse();
  }

  return IsValidPoints(myFirstPoint, mySecondPoint);
}

//=======================================================================
// function : InitTwoShapesPoints
// purpose  : Initialization of two points where dimension layouts
//           will be attached
//=======================================================================
bool PrsDim_LengthDimension::InitTwoShapesPoints(const TopoDS_Shape& theFirstShape,
                                                 const TopoDS_Shape& theSecondShape,
                                                 gp_Pln&             theComputedPlane,
                                                 bool&               theIsPlaneComputed)
{
  theIsPlaneComputed = false;
  gp_Dir aDirAttach;
  bool   isInfinite = false;
  bool   isSuccess  = false;
  switch (theFirstShape.ShapeType())
  {
    case TopAbs_FACE: {
      // Initialization for face
      gp_Pln                    aFirstPlane;
      occ::handle<Geom_Surface> aFirstSurface;
      PrsDim_KindOfSurface      aFirstSurfKind;
      double                    aFirstOffset;

      TopoDS_Face aFirstFace = TopoDS::Face(theFirstShape);

      PrsDim::InitFaceLength(TopoDS::Face(theFirstShape),
                             aFirstPlane,
                             aFirstSurface,
                             aFirstSurfKind,
                             aFirstOffset);

      if (theSecondShape.ShapeType() == TopAbs_FACE)
      {
        // Initialization for face
        myGeometryType = GeometryType_Faces;
        gp_Pln                    aSecondPlane;
        occ::handle<Geom_Surface> aSecondSurface;
        PrsDim_KindOfSurface      aSecondSurfKind;
        double                    aSecondOffset;

        TopoDS_Face aSecondFace = TopoDS::Face(theSecondShape);

        PrsDim::InitFaceLength(aSecondFace,
                               aSecondPlane,
                               aSecondSurface,
                               aSecondSurfKind,
                               aSecondOffset);

        if (aFirstSurfKind == PrsDim_KOS_Plane)
        {
          if (!aFirstPlane.Axis().Direction().IsParallel(aSecondPlane.Axis().Direction(),
                                                         Precision::Angular()))
          {
            return false;
          }

          TopExp_Explorer anExplorer(theFirstShape, TopAbs_VERTEX);

          // In case of infinite planes
          if (!anExplorer.More())
          {
            myFirstPoint = aFirstPlane.Location();
          }
          else
          {
            myFirstPoint = BRep_Tool::Pnt(TopoDS::Vertex(anExplorer.Current()));
          }

          mySecondPoint = PrsDim::ProjectPointOnPlane(myFirstPoint, aSecondPlane);

          double anU, aV;
          ElSLib::Parameters(aSecondPlane, mySecondPoint, anU, aV);

          BRepTopAdaptor_FClass2d aClassifier(aSecondFace, Precision::Confusion());
          TopAbs_State            aState = aClassifier.Perform(gp_Pnt2d(anU, aV), false);

          if (aState == TopAbs_OUT || aState == TopAbs_UNKNOWN)
          {
            mySecondPoint = PrsDim::Nearest(aSecondFace, myFirstPoint);
          }

          isSuccess = IsValidPoints(myFirstPoint, mySecondPoint);
          if (isSuccess)
          {
            theComputedPlane   = ComputePlane(aFirstPlane.Position().XDirection());
            theIsPlaneComputed = true;
          }
        }
        else // curvilinear faces
        {
          double aU1Min, aV1Min, aU1Max, aV1Max;
          double aU2Min, aV2Min, aU2Max, aV2Max;
          BRepTools::UVBounds(aFirstFace, aU1Min, aU1Max, aV1Min, aV1Max);
          BRepTools::UVBounds(aSecondFace, aU2Min, aU2Max, aV2Min, aV2Max);

          GeomAPI_ExtremaSurfaceSurface anExtrema(aFirstSurface,
                                                  aSecondSurface,
                                                  aU1Min,
                                                  aU1Max,
                                                  aV1Min,
                                                  aV1Max,
                                                  aU2Min,
                                                  aU2Max,
                                                  aV2Min,
                                                  aV2Max);

          double aU1, aV1, aU2, aV2;
          anExtrema.LowerDistanceParameters(aU1, aV1, aU2, aV2);
          myFirstPoint  = BRep_Tool::Surface(aFirstFace)->Value(aU1, aV1);
          mySecondPoint = BRep_Tool::Surface(aSecondFace)->Value(aU2, aV2);

          // Adjust automatic plane
          gp_Ax2 aLocalAxes(myFirstPoint, gce_MakeDir(myFirstPoint, mySecondPoint));
          aDirAttach = gce_MakeDir(aLocalAxes.XDirection());

          // Check points
          isSuccess = IsValidPoints(myFirstPoint, mySecondPoint);
          if (isSuccess)
          {
            theComputedPlane   = ComputePlane(aDirAttach);
            theIsPlaneComputed = true;
          }
        }

        return isSuccess && IsValidPoints(myFirstPoint, mySecondPoint);
      }
      else if (theSecondShape.ShapeType() == TopAbs_EDGE)
      {
        myGeometryType = GeometryType_EdgeFace;
        isSuccess =
          InitEdgeFaceLength(TopoDS::Edge(theSecondShape), TopoDS::Face(theFirstShape), aDirAttach);

        if (isSuccess)
        {
          theComputedPlane   = ComputePlane(aDirAttach);
          theIsPlaneComputed = true;
        }

        return isSuccess;
      }
    }
    break;

    case TopAbs_EDGE: {
      if (theSecondShape.ShapeType() == TopAbs_VERTEX)
      {
        myGeometryType = GeometryType_EdgeVertex;
        isSuccess      = InitEdgeVertexLength(TopoDS::Edge(theFirstShape),
                                         TopoDS::Vertex(theSecondShape),
                                         aDirAttach,
                                         isInfinite);

        if (isSuccess)
        {
          theComputedPlane   = ComputePlane(aDirAttach);
          theIsPlaneComputed = true;
        }

        return isSuccess;
      }
      else if (theSecondShape.ShapeType() == TopAbs_EDGE)
      {
        myGeometryType = GeometryType_Edges;
        isSuccess =
          InitTwoEdgesLength(TopoDS::Edge(theFirstShape), TopoDS::Edge(theSecondShape), aDirAttach);

        if (isSuccess)
        {
          theComputedPlane   = ComputePlane(aDirAttach);
          theIsPlaneComputed = true;
        }

        return isSuccess;
      }
      else if (theSecondShape.ShapeType() == TopAbs_FACE)
      {
        myGeometryType = GeometryType_EdgeFace;
        isSuccess =
          InitEdgeFaceLength(TopoDS::Edge(theFirstShape), TopoDS::Face(theSecondShape), aDirAttach);

        if (isSuccess)
        {
          theComputedPlane   = ComputePlane(aDirAttach);
          theIsPlaneComputed = true;
        }

        return isSuccess;
      }
    }
    break;

    case TopAbs_VERTEX: {
      if (theSecondShape.ShapeType() == TopAbs_VERTEX)
      {
        myGeometryType = GeometryType_Points;
        myFirstPoint   = BRep_Tool::Pnt(TopoDS::Vertex(theFirstShape));
        mySecondPoint  = BRep_Tool::Pnt(TopoDS::Vertex(theSecondShape));

        return IsValidPoints(myFirstPoint, mySecondPoint);
      }
      else if (theSecondShape.ShapeType() == TopAbs_EDGE)
      {
        myGeometryType = GeometryType_EdgeVertex;
        isSuccess      = InitEdgeVertexLength(TopoDS::Edge(theSecondShape),
                                         TopoDS::Vertex(theFirstShape),
                                         aDirAttach,
                                         isInfinite);
        if (isSuccess)
        {
          theComputedPlane   = ComputePlane(aDirAttach);
          theIsPlaneComputed = true;
        }

        return isSuccess;
      }
    }
    break;

    case TopAbs_COMPOUND:
    case TopAbs_COMPSOLID:
    case TopAbs_SOLID:
    case TopAbs_SHELL:
    case TopAbs_WIRE:
    case TopAbs_SHAPE:
      break;
  }

  return false;
}

//=======================================================================
// function : InitOneShapePoints
// purpose  : Initialization of two points where dimension layouts
//           will be attached
// Attention: 1) <theShape> can be only the edge in current implementation
//            2) No length for infinite edge
//=======================================================================
bool PrsDim_LengthDimension::InitOneShapePoints(const TopoDS_Shape& theShape)
{
  if (theShape.ShapeType() != TopAbs_EDGE)
  {
    return false;
  }

  TopoDS_Edge anEdge = TopoDS::Edge(theShape);

  BRepAdaptor_Curve aBrepCurve(anEdge);
  double            aFirst = aBrepCurve.FirstParameter();
  double            aLast  = aBrepCurve.LastParameter();

  if (aBrepCurve.GetType() != GeomAbs_Line)
  {
    return false;
  }

  bool isInfinite = (Precision::IsInfinite(aFirst) || Precision::IsInfinite(aLast));
  if (isInfinite)
  {
    return false;
  }

  myFirstPoint  = aBrepCurve.Value(aBrepCurve.FirstParameter());
  mySecondPoint = aBrepCurve.Value(aBrepCurve.LastParameter());

  return IsValidPoints(myFirstPoint, mySecondPoint);
}

//=================================================================================================

gp_Pnt PrsDim_LengthDimension::GetTextPosition() const
{
  if (IsTextPositionCustom())
  {
    return myFixedTextPosition;
  }

  // Counts text position according to the dimension parameters
  return GetTextPositionForLinear(myFirstPoint, mySecondPoint);
}

//=================================================================================================

void PrsDim_LengthDimension::SetTextPosition(const gp_Pnt& theTextPos)
{
  if (!IsValid())
  {
    return;
  }

  myIsTextPositionFixed = true;
  myFixedTextPosition   = theTextPos;

  SetToUpdate();
}

//=================================================================================================

void PrsDim_LengthDimension::SetDirection(const gp_Dir& theDirection, const bool theUseDirection)
{
  myHasCustomDirection = theUseDirection;
  if (myHasCustomDirection)
    myDirection = theDirection;
}
