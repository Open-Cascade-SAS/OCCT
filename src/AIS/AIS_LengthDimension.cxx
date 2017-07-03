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

#include <AIS_LengthDimension.hxx>

#include <AIS.hxx>
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
#include <GeomAPI_ExtremaSurfaceSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>


IMPLEMENT_STANDARD_RTTIEXT(AIS_LengthDimension,AIS_Dimension)

//=======================================================================
//function : Constructor
//purpose  : Dimension between two faces
//=======================================================================
AIS_LengthDimension::AIS_LengthDimension (const TopoDS_Face& theFirstFace,
                                          const TopoDS_Face& theSecondFace)
: AIS_Dimension (AIS_KOD_LENGTH),
  myHasCustomDirection (Standard_False)
{
  SetMeasuredGeometry (theFirstFace, theSecondFace);
  SetFlyout (15.0);
}

//=======================================================================
//function : Constructor
//purpose  : Dimension between two shape
//=======================================================================
AIS_LengthDimension::AIS_LengthDimension (const TopoDS_Face& theFace,
                                          const TopoDS_Edge& theEdge)
: AIS_Dimension (AIS_KOD_LENGTH),
  myHasCustomDirection (Standard_False)
{
  SetMeasuredGeometry (theFace, theEdge);
  SetFlyout (15.0);
}

//=======================================================================
//function : Constructor
//purpose  : Dimension between two points
//=======================================================================
AIS_LengthDimension::AIS_LengthDimension (const gp_Pnt& theFirstPoint,
                                          const gp_Pnt& theSecondPoint,
                                          const gp_Pln& thePlane)
: AIS_Dimension (AIS_KOD_LENGTH),
  myHasCustomDirection (Standard_False)
{
  SetMeasuredGeometry (theFirstPoint, theSecondPoint, thePlane);
  SetFlyout (15.0);
}

//=======================================================================
//function : Constructor
//purpose  : Dimension between two shape
//=======================================================================
AIS_LengthDimension::AIS_LengthDimension (const TopoDS_Shape& theFirstShape,
                                          const TopoDS_Shape& theSecondShape,
                                          const gp_Pln& thePlane)
: AIS_Dimension (AIS_KOD_LENGTH),
  myHasCustomDirection (Standard_False)
{
  SetCustomPlane (thePlane);
  SetMeasuredShapes (theFirstShape, theSecondShape);
  SetFlyout (15.0);
}

//=======================================================================
//function : Constructor
//purpose  : Dimension of one edge
//=======================================================================
AIS_LengthDimension::AIS_LengthDimension (const TopoDS_Edge& theEdge,
                                          const gp_Pln& thePlane)
: AIS_Dimension (AIS_KOD_LENGTH),
  myHasCustomDirection (Standard_False)
{
  SetMeasuredGeometry (theEdge, thePlane);
  SetFlyout (15.0);
}

//=======================================================================
//function : SetMeasuredGeometry
//purpose  : 
//=======================================================================
void AIS_LengthDimension::SetMeasuredGeometry (const gp_Pnt& theFirstPoint,
                                               const gp_Pnt& theSecondPoint,
                                               const gp_Pln& thePlane)
{
  myFirstPoint      = theFirstPoint;
  mySecondPoint     = theSecondPoint;
  myFirstShape      = BRepLib_MakeVertex (myFirstPoint);
  mySecondShape     = BRepLib_MakeVertex (mySecondPoint);
  myGeometryType    = GeometryType_Points;
  SetCustomPlane (thePlane);
  myIsGeometryValid = IsValidPoints (theFirstPoint, theSecondPoint);

  SetToUpdate();
}

//=======================================================================
//function : SetMeasuredGeometry
//purpose  : 
//=======================================================================
void AIS_LengthDimension::SetMeasuredGeometry (const TopoDS_Edge& theEdge,
                                               const gp_Pln& thePlane)
{
  myFirstShape      = theEdge;
  mySecondShape     = TopoDS_Shape();
  myGeometryType    = GeometryType_Edge;
  SetCustomPlane (thePlane);
  myIsGeometryValid = InitOneShapePoints (myFirstShape);

  SetToUpdate();
}

//=======================================================================
//function : SetMeasuredGeometry
//purpose  : 
//=======================================================================
void AIS_LengthDimension::SetMeasuredGeometry (const TopoDS_Face& theFirstFace,
                                               const TopoDS_Face& theSecondFace)
{
  SetMeasuredShapes (theFirstFace, theSecondFace);
}

//=======================================================================
//function : SetMeasuredGeometry
//purpose  : 
//=======================================================================
void AIS_LengthDimension::SetMeasuredGeometry (const TopoDS_Face& theFace,
                                               const TopoDS_Edge& theEdge)
{
  SetMeasuredShapes (theFace, theEdge);
}

//=======================================================================
//function : SetMeasuredShapes
//purpose  : 
//=======================================================================
void AIS_LengthDimension::SetMeasuredShapes (const TopoDS_Shape& theFirstShape,
                                             const TopoDS_Shape& theSecondShape)
{
  gp_Pln aComputedPlane;
  Standard_Boolean isPlaneReturned = Standard_False;

  myFirstShape      = theFirstShape;
  mySecondShape     = theSecondShape;
  myIsGeometryValid = InitTwoShapesPoints (myFirstShape, mySecondShape, aComputedPlane, isPlaneReturned);

  if (myIsGeometryValid && !myIsPlaneCustom)
  {
    if (isPlaneReturned)
    {
      myPlane = aComputedPlane;
    }
    else
    {
      myIsGeometryValid = Standard_False;
    }
  }

  SetToUpdate();
}

//=======================================================================
//function : CheckPlane
//purpose  : 
//=======================================================================
Standard_Boolean AIS_LengthDimension::CheckPlane (const gp_Pln& thePlane) const
{
  Standard_Boolean anIsFaultyNormal =
    thePlane.Axis().Direction().IsParallel(gce_MakeDir (myFirstPoint, mySecondPoint), Precision::Angular());

  if ((!thePlane.Contains (myFirstPoint, Precision::Confusion()) && !thePlane.Contains (mySecondPoint, Precision::Confusion()))
   || anIsFaultyNormal)
  {
    return Standard_False;
  }

  return Standard_True;
}

//=======================================================================
//function : ComputePlane
//purpose  : 
//=======================================================================
gp_Pln AIS_LengthDimension::ComputePlane (const gp_Dir& theAttachDir) const
{
  if (!IsValidPoints (myFirstPoint, mySecondPoint))
  {
    return gp_Pln();
  }

  gp_Pnt aThirdPoint (myFirstPoint.Translated (gp_Vec(theAttachDir)));
  gce_MakePln aPlaneConstrustor (myFirstPoint, mySecondPoint, aThirdPoint);
  return aPlaneConstrustor.Value();
}

//=======================================================================
//function : GetModelUnits
//purpose  :
//=======================================================================
const TCollection_AsciiString& AIS_LengthDimension::GetModelUnits() const
{
  return myDrawer->DimLengthModelUnits();
}

//=======================================================================
//function : GetDisplayUnits
//purpose  :
//=======================================================================
const TCollection_AsciiString& AIS_LengthDimension::GetDisplayUnits() const
{
  return myDrawer->DimLengthDisplayUnits();
}

//=======================================================================
//function : SetModelUnits
//purpose  :
//=======================================================================
void AIS_LengthDimension::SetModelUnits (const TCollection_AsciiString& theUnits)
{
  myDrawer->SetDimLengthModelUnits (theUnits);
}

//=======================================================================
//function : SetDisplayUnits
//purpose  :
//=======================================================================
void AIS_LengthDimension::SetDisplayUnits (const TCollection_AsciiString& theUnits)
{
  myDrawer->SetDimLengthDisplayUnits (theUnits);
}

//=======================================================================
//function : ComputeValue
//purpose  : 
//=======================================================================
Standard_Real AIS_LengthDimension::ComputeValue() const
{
  if (!IsValid())
    return 0.0;

  if (!myHasCustomDirection)
    return myFirstPoint.Distance (mySecondPoint);

  return fabs (gp_Vec(myFirstPoint, mySecondPoint).Dot (myDirection));
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void AIS_LengthDimension::Compute (const Handle(PrsMgr_PresentationManager3d)& /*thePM*/,
                                   const Handle(Prs3d_Presentation)& thePresentation,
                                   const Standard_Integer theMode)
{
  mySelectionGeom.Clear (theMode);

  if (!IsValid())
  {
    return;
  }

  DrawLinearDimension (thePresentation, theMode, myFirstPoint, mySecondPoint);
}

 //=======================================================================
//function : ComputeFlyoutLinePoints
//purpose  : 
//=======================================================================
void AIS_LengthDimension::ComputeFlyoutLinePoints (const gp_Pnt& theFirstPoint, const gp_Pnt& theSecondPoint,
                                                   gp_Pnt& theLineBegPoint, gp_Pnt& theLineEndPoint)
{
  if (!myHasCustomDirection)
  {
    AIS_Dimension::ComputeFlyoutLinePoints (theFirstPoint, theSecondPoint, theLineBegPoint, theLineEndPoint);
    return;
  }

  // find scalar of projection target vector (from start to second point) to flyout vector
  gp_Ax1 aPlaneNormal = GetPlane().Axis();
  gp_Vec aFlyoutNormalizedDir(aPlaneNormal.Direction() ^ myDirection);
  aFlyoutNormalizedDir.Normalize();
  Standard_Real aTargetProjectedToFlyout = gp_Vec(theFirstPoint, theSecondPoint).Dot (aFlyoutNormalizedDir);

  gp_Dir aFlyoutVector = aFlyoutNormalizedDir;
  // create lines for layouts
  gp_Lin aLine1 (theFirstPoint, aFlyoutVector);
  gp_Lin aLine2 (theSecondPoint, aFlyoutVector);

  // Get flyout end points
  theLineBegPoint = ElCLib::Value (ElCLib::Parameter (aLine1, theFirstPoint)  + GetFlyout() + aTargetProjectedToFlyout, aLine1);
  theLineEndPoint = ElCLib::Value (ElCLib::Parameter (aLine2, theSecondPoint) + GetFlyout(), aLine2);
}

//=======================================================================
//function : ComputeFlyoutSelection
//purpose  : 
//=======================================================================
void AIS_LengthDimension::ComputeFlyoutSelection (const Handle(SelectMgr_Selection)& theSelection,
                                                  const Handle(SelectMgr_EntityOwner)& theEntityOwner)
{
  if (!IsValid())
  {
    return;
  }

  ComputeLinearFlyouts (theSelection, theEntityOwner, myFirstPoint, mySecondPoint);
}

//=======================================================================
//function : IsValidPoints
//purpose  :
//=======================================================================
Standard_Boolean AIS_LengthDimension::IsValidPoints (const gp_Pnt& theFirstPoint,
                                                     const gp_Pnt& theSecondPoint) const
{
  return theFirstPoint.Distance (theSecondPoint) > Precision::Confusion();
}

//=======================================================================
//function : InitTwoEdgesLength
//purpose  : Initialization of dimension between two linear edges
//=======================================================================
Standard_Boolean AIS_LengthDimension::InitTwoEdgesLength (const TopoDS_Edge& theFirstEdge,
                                                          const TopoDS_Edge& theSecondEdge,
                                                          gp_Dir& theDirAttach)
{
  BRepAdaptor_Curve aFirstCurveAdapt (theFirstEdge);
  if (aFirstCurveAdapt.GetType() != GeomAbs_Line)
  {
    return Standard_False;
  }

  BRepAdaptor_Curve aSecondCurveAdapt (theSecondEdge);
  if (aSecondCurveAdapt.GetType() != GeomAbs_Line)
  {
    return Standard_False;
  }

  Handle(Geom_Curve) aFirstCurve;
  Handle(Geom_Curve) aSecondCurve;

  gp_Pnt aPoint11 (gp::Origin());
  gp_Pnt aPoint12 (gp::Origin());
  gp_Pnt aPoint21 (gp::Origin());
  gp_Pnt aPoint22 (gp::Origin());
  Standard_Boolean isFirstInfinite  = Standard_False;
  Standard_Boolean isSecondInfinite = Standard_False;

  if (!AIS::ComputeGeometry (theFirstEdge, theSecondEdge,
                             aFirstCurve, aSecondCurve,
                             aPoint11, aPoint12,
                             aPoint21, aPoint22,
                             isFirstInfinite,
                             isSecondInfinite))
  {
    return Standard_False;
  }

  const Handle(Geom_Line) aFirstLine = Handle(Geom_Line)::DownCast (aFirstCurve);
  const Handle(Geom_Line) aSecondLine = Handle(Geom_Line)::DownCast (aSecondCurve);

  if (!aFirstLine->Lin().Direction().IsParallel (aSecondLine->Lin().Direction(),Precision::Angular()))
  {
    return Standard_False;
  }

  theDirAttach = aFirstLine->Lin().Direction();

  gp_Pnt aPoint;

  if (!isFirstInfinite)
  {
    if (AIS::Nearest (aSecondCurve, aPoint11, aPoint21, aPoint22, aPoint))
    {
      myFirstPoint = aPoint11;
      mySecondPoint = aPoint;
      return IsValidPoints (myFirstPoint, mySecondPoint);
    }
    else if (AIS::Nearest (aSecondCurve, aPoint12, aPoint21, aPoint22, aPoint))
    {
      myFirstPoint = aPoint12;
      mySecondPoint = aPoint;
      return IsValidPoints (myFirstPoint, mySecondPoint);
    }
  }

  if (!isSecondInfinite)
  {
    if (AIS::Nearest (aFirstCurve, aPoint21, aPoint11, aPoint12, aPoint))
    {
      myFirstPoint = aPoint;
      mySecondPoint = aPoint21;
      return IsValidPoints (myFirstPoint, mySecondPoint);
    }
    if (AIS::Nearest (aFirstCurve, aPoint22, aPoint11, aPoint12, aPoint))
    {
      myFirstPoint = aPoint;
      mySecondPoint = aPoint22;
      return IsValidPoints (myFirstPoint, mySecondPoint);
    }
  }

  GeomAPI_ExtremaCurveCurve anExtrema (aFirstCurve, aSecondCurve);
  anExtrema.NearestPoints (myFirstPoint, mySecondPoint);
  return IsValidPoints (myFirstPoint, mySecondPoint);
}

//=======================================================================
//function : InitEdgeVertexLength
//purpose  : for first edge and second vertex shapes
//=======================================================================
Standard_Boolean AIS_LengthDimension::InitEdgeVertexLength (const TopoDS_Edge& theEdge,
                                                            const TopoDS_Vertex& theVertex,
                                                            gp_Dir& theEdgeDir,
                                                            Standard_Boolean isInfinite)
{
  gp_Pnt anEdgePoint1 (gp::Origin());
  gp_Pnt anEdgePoint2 (gp::Origin());
  Handle(Geom_Curve) aCurve;

  if (!AIS::ComputeGeometry (theEdge, aCurve, anEdgePoint1, anEdgePoint2, isInfinite))
  {
    return Standard_False;
  }

  myFirstPoint = BRep_Tool::Pnt (theVertex);

  Handle(Geom_Line) aGeomLine (Handle(Geom_Line)::DownCast (aCurve));
  const gp_Lin& aLin = aGeomLine->Lin();

  // Get direction of edge to build plane automatically.
  theEdgeDir = aLin.Direction();

  mySecondPoint = AIS::Nearest (aLin, myFirstPoint);

  return IsValidPoints (myFirstPoint, mySecondPoint);
}

//=======================================================================
//function : InitEdgeFaceLength
//purpose  : 
//=======================================================================
Standard_Boolean AIS_LengthDimension::InitEdgeFaceLength (const TopoDS_Edge& theEdge,
                                                          const TopoDS_Face& theFace,
                                                          gp_Dir& theEdgeDir)
{
  theEdgeDir = gp::DX();

  // Find attachment points (closest distance between the edge and the face)
  BRepExtrema_DistShapeShape aDistAdaptor (theEdge, theFace, Extrema_ExtFlag_MIN);
  if (!aDistAdaptor.IsDone() || aDistAdaptor.NbSolution() <1)
  {
    return Standard_False;
  }
  myFirstPoint = aDistAdaptor.PointOnShape1 (1);
  mySecondPoint = aDistAdaptor.PointOnShape2 (1);

  // Take direction for dimension line (will be orthogonalized later) parallel to edge
  BRepAdaptor_Curve aCurveAdaptor (theEdge);
  Standard_Real aParam;
  if (aDistAdaptor.SupportOnShape1 (1).ShapeType() == TopAbs_EDGE)
  {
    aDistAdaptor.ParOnEdgeS1 (1, aParam);
  }
  else
  {
    Standard_Real aD1 = aCurveAdaptor.Value(aCurveAdaptor.FirstParameter()).SquareDistance (myFirstPoint);
    Standard_Real aD2 = aCurveAdaptor.Value(aCurveAdaptor.LastParameter()).SquareDistance (myFirstPoint);
    aParam = (aD1 < aD2 ? aCurveAdaptor.FirstParameter() : aCurveAdaptor.LastParameter());
  }
  gp_Pnt aP;
  gp_Vec aV;
  aCurveAdaptor.D1 (aParam, aP, aV);
  if (aV.SquareMagnitude() > gp::Resolution())
  {
    theEdgeDir = aV;
  }

  // reverse direction if parameter is close to the end of the curve,
  // to reduce chances to have overlapping between dimension line and edge
  if (Abs (aParam - aCurveAdaptor.FirstParameter()) < Abs (aParam - aCurveAdaptor.LastParameter()))
  {
    theEdgeDir.Reverse();
  }

  return IsValidPoints (myFirstPoint, mySecondPoint);
}

//=======================================================================
//function : InitTwoShapesPoints
//purpose  : Initialization of two points where dimension layouts
//           will be attached
//=======================================================================
Standard_Boolean AIS_LengthDimension::InitTwoShapesPoints (const TopoDS_Shape& theFirstShape,
                                                           const TopoDS_Shape& theSecondShape,
                                                           gp_Pln& theComputedPlane,
                                                           Standard_Boolean& theIsPlaneComputed)
{
  theIsPlaneComputed = Standard_False;
  gp_Dir aDirAttach;
  Standard_Boolean isInfinite = Standard_False;
  Standard_Boolean isSuccess  = Standard_False;
  switch (theFirstShape.ShapeType())
  {
    case TopAbs_FACE:
    {
      // Initialization for face
      gp_Pln aFirstPlane;
      Handle(Geom_Surface) aFirstSurface;
      AIS_KindOfSurface aFirstSurfKind;
      Standard_Real aFirstOffset;

      TopoDS_Face aFirstFace = TopoDS::Face (theFirstShape);

      AIS::InitFaceLength (TopoDS::Face (theFirstShape),
                           aFirstPlane,
                           aFirstSurface,
                           aFirstSurfKind,
                           aFirstOffset);

      if (theSecondShape.ShapeType() == TopAbs_FACE)
      {
        // Initialization for face
        myGeometryType = GeometryType_Faces;
        gp_Pln aSecondPlane;
        Handle(Geom_Surface) aSecondSurface;
        AIS_KindOfSurface aSecondSurfKind;
        Standard_Real aSecondOffset;

        TopoDS_Face aSecondFace = TopoDS::Face (theSecondShape);

        AIS::InitFaceLength (aSecondFace,
                             aSecondPlane,
                             aSecondSurface,
                             aSecondSurfKind,
                             aSecondOffset);

        if (aFirstSurfKind == AIS_KOS_Plane)
        {
          if (!aFirstPlane.Axis().Direction().IsParallel (aSecondPlane.Axis().Direction(), Precision::Angular()))
          {
            return Standard_False;
          }

          TopExp_Explorer anExplorer (theFirstShape, TopAbs_VERTEX);

          // In case of infinite planes
          if (!anExplorer.More())
          {
            myFirstPoint = aFirstPlane.Location();
          }
          else
          {
            myFirstPoint = BRep_Tool::Pnt (TopoDS::Vertex (anExplorer.Current()));
          }

          mySecondPoint = AIS::ProjectPointOnPlane (myFirstPoint, aSecondPlane);

          Standard_Real anU, aV;
          ElSLib::Parameters (aSecondPlane, mySecondPoint, anU, aV);

          BRepTopAdaptor_FClass2d aClassifier (aSecondFace, Precision::Confusion());
          TopAbs_State aState = aClassifier.Perform (gp_Pnt2d (anU, aV), Standard_False);

          if (aState == TopAbs_OUT || aState == TopAbs_UNKNOWN)
          {
            mySecondPoint = AIS::Nearest (aSecondFace, myFirstPoint);
          }

          isSuccess = IsValidPoints (myFirstPoint, mySecondPoint);
          if (isSuccess)
          {
            theComputedPlane = ComputePlane (aFirstPlane.Position().XDirection());
            theIsPlaneComputed = Standard_True;
          }
        }
        else // curvilinear faces
        {
          Standard_Real aU1Min, aV1Min, aU1Max, aV1Max;
          Standard_Real aU2Min, aV2Min, aU2Max, aV2Max;
          BRepTools::UVBounds (aFirstFace, aU1Min, aU1Max, aV1Min,  aV1Max);
          BRepTools::UVBounds (aSecondFace, aU2Min, aU2Max, aV2Min, aV2Max);

          GeomAPI_ExtremaSurfaceSurface anExtrema (aFirstSurface, aSecondSurface,
                                                   aU1Min, aU1Max, aV1Min, aV1Max,
                                                   aU2Min, aU2Max, aV2Min, aV2Max);

          Standard_Real aU1, aV1, aU2, aV2;
          anExtrema.LowerDistanceParameters (aU1, aV1, aU2, aV2);
          myFirstPoint = BRep_Tool::Surface (aFirstFace)->Value (aU1, aV1);
          mySecondPoint = BRep_Tool::Surface (aSecondFace)->Value (aU2, aV2);

          // Adjust automatic plane
          gp_Ax2 aLocalAxes (myFirstPoint, gce_MakeDir (myFirstPoint, mySecondPoint));
          aDirAttach = gce_MakeDir (aLocalAxes.XDirection ());

          // Check points
          isSuccess = IsValidPoints (myFirstPoint, mySecondPoint);
          if (isSuccess)
          {
            theComputedPlane = ComputePlane (aDirAttach);
            theIsPlaneComputed = Standard_True;
          }
        }

        return isSuccess && IsValidPoints (myFirstPoint, mySecondPoint);
      }
      else if (theSecondShape.ShapeType() == TopAbs_EDGE)
      {
        myGeometryType = GeometryType_EdgeFace;
        isSuccess = InitEdgeFaceLength (TopoDS::Edge (theSecondShape),
                                        TopoDS::Face (theFirstShape),
                                        aDirAttach);

        if (isSuccess)
        {
          theComputedPlane = ComputePlane (aDirAttach);
          theIsPlaneComputed = Standard_True;
        }

        return isSuccess;
      }
    }
    break;

    case TopAbs_EDGE:
    {
      if (theSecondShape.ShapeType() == TopAbs_VERTEX)
      {
        myGeometryType = GeometryType_EdgeVertex;
        isSuccess = InitEdgeVertexLength (TopoDS::Edge (theFirstShape),
                                          TopoDS::Vertex (theSecondShape),
                                          aDirAttach,
                                          isInfinite);

        if (isSuccess)
        {
          theComputedPlane = ComputePlane (aDirAttach);
          theIsPlaneComputed = Standard_True;
        }

        return isSuccess;
      }
      else if (theSecondShape.ShapeType() == TopAbs_EDGE)
      {
        myGeometryType = GeometryType_Edges;
        isSuccess = InitTwoEdgesLength (TopoDS::Edge (theFirstShape),
                                        TopoDS::Edge (theSecondShape),
                                        aDirAttach);

        if (isSuccess)
        {
          theComputedPlane = ComputePlane (aDirAttach);
          theIsPlaneComputed = Standard_True;
        }

        return isSuccess;
      }
      else if (theSecondShape.ShapeType() == TopAbs_FACE)
      {
        myGeometryType = GeometryType_EdgeFace;
        isSuccess = InitEdgeFaceLength (TopoDS::Edge (theFirstShape),
                                        TopoDS::Face (theSecondShape),
                                        aDirAttach);

        if (isSuccess)
        {
          theComputedPlane = ComputePlane (aDirAttach);
          theIsPlaneComputed = Standard_True;
        }

        return isSuccess;
      }
    }
    break;

    case TopAbs_VERTEX:
    {
      if (theSecondShape.ShapeType() == TopAbs_VERTEX)
      {
        myGeometryType = GeometryType_Points;
        myFirstPoint  = BRep_Tool::Pnt (TopoDS::Vertex (theFirstShape));
        mySecondPoint = BRep_Tool::Pnt (TopoDS::Vertex (theSecondShape));

        return IsValidPoints (myFirstPoint, mySecondPoint);
      }
      else if (theSecondShape.ShapeType() == TopAbs_EDGE)
      {
        myGeometryType = GeometryType_EdgeVertex;
        isSuccess =  InitEdgeVertexLength (TopoDS::Edge(theSecondShape),
                                           TopoDS::Vertex(theFirstShape),
                                           aDirAttach,
                                           isInfinite);
        if (isSuccess)
        {
          theComputedPlane = ComputePlane (aDirAttach);
          theIsPlaneComputed = Standard_True;
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

  return Standard_False;
}

//=======================================================================
//function : InitOneShapePoints
//purpose  : Initialization of two points where dimension layouts
//           will be attached
// Attention: 1) <theShape> can be only the edge in currect implementation
//            2) No length for infinite edge
//=======================================================================
Standard_Boolean AIS_LengthDimension::InitOneShapePoints (const TopoDS_Shape& theShape)
{
  if (theShape.ShapeType() != TopAbs_EDGE)
  {
    return Standard_False;
  }

  TopoDS_Edge anEdge = TopoDS::Edge (theShape);

  BRepAdaptor_Curve aBrepCurve(anEdge);
  Standard_Real aFirst = aBrepCurve.FirstParameter();
  Standard_Real aLast  = aBrepCurve.LastParameter();

  if (aBrepCurve.GetType() != GeomAbs_Line)
  {
    return Standard_False;
  }

  Standard_Boolean isInfinite = (Precision::IsInfinite (aFirst) || Precision::IsInfinite (aLast));
  if (isInfinite)
  {
    return Standard_False;
  }

  myFirstPoint  = aBrepCurve.Value (aBrepCurve.FirstParameter());
  mySecondPoint = aBrepCurve.Value (aBrepCurve.LastParameter());

  return IsValidPoints (myFirstPoint, mySecondPoint);
}

//=======================================================================
//function : GetTextPosition
//purpose  : 
//=======================================================================
const gp_Pnt AIS_LengthDimension::GetTextPosition() const
{
  if (IsTextPositionCustom())
  {
    return myFixedTextPosition;
  }

  // Counts text position according to the dimension parameters
  return GetTextPositionForLinear (myFirstPoint, mySecondPoint);
}

//=======================================================================
//function : SetTextPosition
//purpose  : 
//=======================================================================
void AIS_LengthDimension::SetTextPosition (const gp_Pnt& theTextPos)
{
  if (!IsValid())
  {
    return;
  }

  myIsTextPositionFixed = Standard_True;
  myFixedTextPosition = theTextPos;

  SetToUpdate();
}

//=======================================================================
//function : SetDirection
//purpose  : 
//=======================================================================
void AIS_LengthDimension::SetDirection (const gp_Dir& theDirection, const Standard_Boolean theUseDirection)
{
  myHasCustomDirection = theUseDirection;
  if (myHasCustomDirection)
    myDirection = theDirection;
}
