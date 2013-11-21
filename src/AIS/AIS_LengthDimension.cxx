// Created on: 1996-12-05
// Created by: Arnaud BOUZY/Odile Olivier
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <AIS_LengthDimension.hxx>

#include <AIS.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepGProp_Face.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Geom_Line.hxx>
#include <gce_MakeDir.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <PrsMgr_PresentationManager.hxx>
#include <Prs3d_Root.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

IMPLEMENT_STANDARD_HANDLE(AIS_LengthDimension, AIS_Dimension)
IMPLEMENT_STANDARD_RTTIEXT(AIS_LengthDimension, AIS_Dimension)

//=======================================================================
//function : Constructor
//purpose  : Dimension between two points
//=======================================================================

AIS_LengthDimension::AIS_LengthDimension (const gp_Pnt& theFirstPoint,
                                          const gp_Pnt& theSecondPoint,
                                          const gp_Pln& theDimensionPlane)
: AIS_Dimension()
{
  myIsInitialized = Standard_True;
  myFirstPoint = theFirstPoint;
  mySecondPoint = theSecondPoint;
  myFirstShape = BRepLib_MakeVertex (myFirstPoint);
  mySecondShape = BRepLib_MakeVertex (mySecondPoint);
  myShapesNumber = 2;
  SetKindOfDimension (AIS_KOD_LENGTH);
  SetWorkingPlane (theDimensionPlane);
  SetFlyout (15.0);
}

//=======================================================================
//function : Constructor
//purpose  : Dimension between two shape
//=======================================================================

AIS_LengthDimension::AIS_LengthDimension (const TopoDS_Shape& theFirstShape,
                                          const TopoDS_Shape& theSecondShape,
                                          const gp_Pln& theWorkingPlane)
: AIS_Dimension()
{
  myIsInitialized = Standard_False;
  myFirstShape = theFirstShape;
  mySecondShape = theSecondShape;
  myShapesNumber = 2;
  SetKindOfDimension (AIS_KOD_LENGTH);
  SetWorkingPlane (theWorkingPlane);
  SetFlyout (15.0);
}

//=======================================================================
//function : Constructor
//purpose  : Dimension of one edge
//=======================================================================

AIS_LengthDimension::AIS_LengthDimension (const TopoDS_Edge& theEdge,
                                          const gp_Pln& theWorkingPlane)
: AIS_Dimension()
{
  myIsInitialized = Standard_False;
  myFirstShape = theEdge;
  myShapesNumber = 1;
  SetKindOfDimension (AIS_KOD_LENGTH);
  SetWorkingPlane (theWorkingPlane);
  SetFlyout (15.0);
}

//=======================================================================
//function : Constructor
//purpose  : Dimension between two faces
//=======================================================================

AIS_LengthDimension::AIS_LengthDimension (const TopoDS_Face& theFirstFace,
                                          const TopoDS_Face& theSecondFace)
: AIS_Dimension()
{
  myIsInitialized = Standard_False;
  myFirstShape = theFirstFace;
  mySecondShape = theSecondFace;
  myShapesNumber = 2;
  SetKindOfDimension (AIS_KOD_LENGTH);
  SetFlyout (15.0);
}

//=======================================================================
//function : Constructor
//purpose  : Dimension between two shape
//=======================================================================

AIS_LengthDimension::AIS_LengthDimension (const TopoDS_Face& theFace,
                                          const TopoDS_Edge& theEdge)
: AIS_Dimension()
{
  myIsInitialized = Standard_False;
  myFirstShape = theFace;
  mySecondShape = theEdge;
  myShapesNumber = 2;
  SetKindOfDimension (AIS_KOD_LENGTH);
  SetFlyout (15.0);
}

//=======================================================================
//function : initTwoEdgesLength
//purpose  : Initialization of dimanesion between two linear edges
//=======================================================================

Standard_Boolean AIS_LengthDimension::initTwoEdgesLength (const TopoDS_Edge & theFirstEdge,
                                                          const TopoDS_Edge& theSecondEdge,
                                                          gp_Dir& theDirAttach)
{
  Standard_Integer anExtShapeIndex = 0;
  BRepAdaptor_Curve aFirstCurveAdapt (theFirstEdge);
  if (aFirstCurveAdapt.GetType() != GeomAbs_Line)
    return Standard_False;
  BRepAdaptor_Curve aSecondCurveAdapt (theSecondEdge);
  if (aSecondCurveAdapt.GetType() != GeomAbs_Line)
    return Standard_False;
  Handle(Geom_Curve) aFirstCurve, aSecondCurve;
  gp_Pnt aPoint11,aPoint12,aPoint21,aPoint22;
  Standard_Boolean isFirstInfinite (Standard_False),
                   isSecondInfinite (Standard_False);
  Handle(Geom_Curve) anExtCurve;

  if (!AIS::ComputeGeometry (theFirstEdge, theSecondEdge,anExtShapeIndex,
                            aFirstCurve, aSecondCurve, aPoint11, aPoint12,
                            aPoint21, aPoint22, anExtCurve, isFirstInfinite,
                            isSecondInfinite, new Geom_Plane(GetWorkingPlane())))
    return Standard_False;

  const Handle(Geom_Line)& aGeomLine1 = (Handle(Geom_Line)&) aFirstCurve;
  const Handle(Geom_Line)& aGeomLine2 = (Handle(Geom_Line)&) aSecondCurve;
  const gp_Lin& aLin1 = aGeomLine1->Lin();
  const gp_Lin& aLin2 = aGeomLine2->Lin();

  myValue = aLin1.Distance (aLin2);
  theDirAttach = aLin1.Direction();

  gp_Pnt aCurPos;
  if (!isFirstInfinite)
  {
    gp_Pnt aPoint2 = ElCLib::Value(ElCLib::Parameter (aLin2, aPoint11), aLin2);
    aCurPos.SetXYZ((aPoint11.XYZ() + aPoint2.XYZ()) / 2.);
  }
  else if (!isSecondInfinite)
  {
    gp_Pnt aPoint2 = ElCLib::Value (ElCLib::Parameter (aLin1, aPoint21), aLin1);
    aCurPos.SetXYZ ((aPoint21.XYZ() + aPoint2.XYZ()) / 2.);
  }
  else
    aCurPos.SetXYZ((aLin1.Location().XYZ() + aLin2.Location().XYZ()) / 2.);
  
  // Offset to avoid confusion Edge and Dimension
  gp_Vec anOffset(theDirAttach);
  anOffset = anOffset*myDrawer->DimensionAspect()->ArrowAspect()->Length()*(-10.);
  aCurPos.Translate(anOffset);
  myGeom.myTextPosition = aCurPos;
  // Find attachment points
  if (!isFirstInfinite)
  {
    if (myGeom.myTextPosition.Distance(aPoint11) > myGeom.myTextPosition.Distance(aPoint12))
      myFirstPoint = aPoint12;
    else
      myFirstPoint = aPoint11;
  }
  else
    myFirstPoint = ElCLib::Value(ElCLib::Parameter(aLin1,myGeom.myTextPosition), aLin1);

  if (!isSecondInfinite)
  {
    if (myGeom.myTextPosition.Distance(aPoint21) > myGeom.myTextPosition.Distance(aPoint22))
      mySecondPoint = aPoint22;
    else
      mySecondPoint = aPoint21;
  }
  else
    mySecondPoint = ElCLib::Value(ElCLib::Parameter(aLin2, myGeom.myTextPosition), aLin2);

  return Standard_True;
}

//=======================================================================
//function : initEdgeVertexLength
//purpose  : for first edge and second vertex shapes
//=======================================================================

Standard_Boolean AIS_LengthDimension::initEdgeVertexLength (const TopoDS_Edge & theEdge,
                                                            const TopoDS_Vertex & theVertex,
                                                            gp_Dir & theDirAttach,
                                                            Standard_Boolean isInfinite)
{
  gp_Pnt anEdgePoint1,anEdgePoint2;
  Handle(Geom_Curve) aCurve;
  Handle(Geom_Curve) anExtCurve;
  Standard_Boolean isEdgeOnPlane, isVertexOnPlane;
  if (!AIS::ComputeGeometry(theEdge,aCurve,anEdgePoint1,anEdgePoint2,
                            anExtCurve,isInfinite,isEdgeOnPlane, new Geom_Plane (GetWorkingPlane())))
    return Standard_False;
  AIS::ComputeGeometry (theVertex, myFirstPoint, new Geom_Plane(GetWorkingPlane()), isVertexOnPlane);

  const Handle(Geom_Line)& aGeomLine = (Handle(Geom_Line)&) aCurve;
  const gp_Lin& aLin = aGeomLine->Lin();

  myValue = aLin.Distance( myFirstPoint);
  theDirAttach = aLin.Direction();

  gp_Pnt aPoint = ElCLib::Value(ElCLib::Parameter(aLin,myFirstPoint),aLin);
  gp_Pnt aCurPos((myFirstPoint.XYZ() + aPoint.XYZ())/2.);

  if (!isInfinite)
  {
    if (myGeom.myTextPosition.Distance(anEdgePoint1) > myGeom.myTextPosition.Distance(anEdgePoint2))
      mySecondPoint = anEdgePoint2;
    else
      mySecondPoint = anEdgePoint1;
  }
  else
    mySecondPoint = ElCLib::Value(ElCLib::Parameter(aLin,myGeom.myTextPosition),aLin);
  return Standard_True;
}

//=======================================================================
//function : initEdgeVertexLength
//purpose  : 
//=======================================================================

Standard_Boolean AIS_LengthDimension::initEdgeFaceLength (const TopoDS_Edge& theEdge,
                                                          const TopoDS_Face& theFace,
                                                          gp_Dir& theDirAttach)
{
  // The first attachment point is <aPoint1> from the reference <anEdge>.
  // Find the second attachment point which belongs to the reference face
  // Iterate over the edges of the face and find the point <aFacePoint1>.
  // It is the closest point according to <aPoint1>.
  TopoDS_Vertex aVertex1, aVertex2;
  TopExp::Vertices (theEdge, aVertex1, aVertex2);
  myFirstPoint = BRep_Tool::Pnt (aVertex1);
  gp_Pnt aPoint  = BRep_Tool::Pnt (aVertex2);
  gp_Pnt2d aFacePoint1uv, aFacePoint2uv;
  Standard_Real aDist1 = RealLast ();
  Standard_Real aDist2 = RealLast ();

  TopExp_Explorer anIt (theFace, TopAbs_EDGE);
  for (; anIt.More (); anIt.Next ())
  {
    const TopoDS_Edge aFaceEdge = TopoDS::Edge(anIt.Current ());
    if (aFaceEdge == theEdge)
      return Standard_False;
    TopExp::Vertices (aFaceEdge, aVertex1, aVertex2);
    gp_Pnt aFacePoint1c = BRep_Tool::Pnt (aVertex1);
    gp_Pnt aFacePoint2c = BRep_Tool::Pnt (aVertex2);
    Standard_Real aDistc1 = myFirstPoint.SquareDistance (aFacePoint1c);
    Standard_Real aDistc2 = myFirstPoint.SquareDistance (aFacePoint2c);
    if (aDistc1 <= aDistc2)
    {
      if (aDistc1 <= aDist1)
      {
        aDistc2 = aPoint.SquareDistance (aFacePoint2c);
        if (aDistc2 <= aDist2)
        {
          mySecondPoint  = aFacePoint1c;
          aDist1 = aDistc1;
          aDist2 = aDistc2;
          BRep_Tool::UVPoints (aFaceEdge, theFace, aFacePoint1uv, aFacePoint2uv); 
        }
      }
    }
    else
    {
      if (aDistc2 <= aDist1)
      {
        aDistc1 = aPoint.SquareDistance (aFacePoint1c);
        if (aDistc1 <= aDist2)
        {
          mySecondPoint = aFacePoint2c;
          aDist1 = aDistc2;
          aDist2 = aDistc1;
          BRep_Tool::UVPoints (aFaceEdge, theFace, aFacePoint2uv, aFacePoint1uv); 
        }
      }
    }
  }

  gp_Vec anOffsetDirection (0.0, 0.0, 0.0);

  //The offset direction is the normal to the face at the point FP1 
  BRepGProp_Face aGFace;
  aGFace.Load (theFace);
  aGFace.Normal (aFacePoint1uv.X(), aFacePoint1uv.Y(), aPoint, anOffsetDirection);

  if (anOffsetDirection.Magnitude () > Precision::Confusion ())
  {
    theDirAttach = gp_Dir (anOffsetDirection);
  }
  else theDirAttach = gp::DZ ();

  gp_Vec aVector (theDirAttach);
  aVector.Multiply (1.5 * myValue); 
  myGeom.myTextPosition = mySecondPoint.Translated (aVector);
  return Standard_True;
}

//=======================================================================
//function : initTwoShapesPoints
//purpose  : Initialization of two points where dimension layouts
//           will be attached
//=======================================================================

Standard_Boolean AIS_LengthDimension::initTwoShapesPoints (const TopoDS_Shape& theFirstShape,
                                                           const TopoDS_Shape& theSecondShape)
{
  gp_Dir aDirAttach;
  Standard_Boolean isInfinite = Standard_False;
  Standard_Boolean isSuccess = Standard_False;
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
      AIS::InitFaceLength (TopoDS::Face (theFirstShape), aFirstPlane,
        aFirstSurface,aFirstSurfKind, aFirstOffset);

      if (theSecondShape.ShapeType () == TopAbs_FACE)
      {
        // Initialization for face
        gp_Pln aSecondPlane;
        Handle(Geom_Surface) aSecondSurface;
        AIS_KindOfSurface aSecondSurfKind;
        Standard_Real aSecondOffset;
        TopoDS_Face aSecondFace = TopoDS::Face (theSecondShape);
        AIS::InitFaceLength (aSecondFace, aSecondPlane,
          aSecondSurface, aSecondSurfKind, aSecondOffset);
        if (aFirstSurfKind == AIS_KOS_Plane)
        {
          TopExp_Explorer anExplorer (theFirstShape, TopAbs_VERTEX);
          // In case of infinite planes
          if (!anExplorer.More())
            myFirstPoint = aFirstPlane.Location();
          else myFirstPoint = BRep_Tool::Pnt (TopoDS::Vertex (anExplorer.Current()));
          mySecondPoint = AIS::ProjectPointOnPlane (myFirstPoint, aSecondPlane);

          gp_Dir aLengthDir = aFirstPlane.Axis().Direction();
          gp_Dir aDirAttach = aFirstPlane.Position().XDirection();
          Quantity_Parameter anU, aV;
          ElSLib::Parameters (aSecondPlane, mySecondPoint, anU, aV);
          BRepTopAdaptor_FClass2d aClassifier (aSecondFace, Precision::Confusion());
          TopAbs_State aState = aClassifier.Perform (gp_Pnt2d (anU, aV), Standard_False);
          if (aState == TopAbs_OUT || aState == TopAbs_UNKNOWN)
          {
            mySecondPoint = AIS::Nearest(aSecondFace, myFirstPoint);
            if (myFirstPoint.Distance(mySecondPoint) > Precision::Confusion())
            {
              gp_Vec aVec = gp_Vec(myFirstPoint, mySecondPoint) ^ aLengthDir;
              if (aVec.SquareMagnitude() > Precision::SquareConfusion())
                aDirAttach = aVec ^ aLengthDir;
            }
          }
          isSuccess = Standard_True;
        }
        else // curvilinear faces
        {
          AIS::ComputeLengthBetweenCurvilinearFaces (aFirstFace, aSecondFace, aFirstSurface, 
                                                     aSecondSurface,Standard_True, myValue,
                                                     myGeom.myTextPosition,myFirstPoint,mySecondPoint,aDirAttach);
          isSuccess = Standard_True;
        }
      }
      else if (theFirstShape.ShapeType() == TopAbs_EDGE)
      {
        isSuccess = initEdgeFaceLength (TopoDS::Edge (theFirstShape),
                                        TopoDS::Face (theSecondShape),
                                        aDirAttach);
      }
      if (!myIsWorkingPlaneCustom)
        resetWorkingPlane(gp_Pln(myFirstPoint, aDirAttach));
    }
    break;
  case TopAbs_EDGE:
    {
      if (theSecondShape.ShapeType() == TopAbs_VERTEX)
      {
        return initEdgeVertexLength (TopoDS::Edge(theFirstShape),
                                     TopoDS::Vertex(theSecondShape),
                                     aDirAttach, isInfinite);
      }
      else if (theSecondShape.ShapeType() == TopAbs_EDGE)
      {
        return initTwoEdgesLength (TopoDS::Edge(theFirstShape),
                                   TopoDS::Edge(theSecondShape),
                                   aDirAttach);
      }
    }
    break;
  case TopAbs_VERTEX:
    {
      if (theSecondShape.ShapeType() == TopAbs_VERTEX)
      {
        myFirstPoint = BRep_Tool::Pnt (TopoDS::Vertex (theFirstShape));
        mySecondPoint = BRep_Tool::Pnt (TopoDS::Vertex (theSecondShape));
        isSuccess = Standard_True;
      }
      else if (theSecondShape.ShapeType() == TopAbs_EDGE)
      {
        return initEdgeVertexLength (TopoDS::Edge(theSecondShape),
                                     TopoDS::Vertex(theFirstShape),
                                     aDirAttach, isInfinite);
      }
    }
    break;
  case TopAbs_COMPOUND:
  case TopAbs_COMPSOLID:
  case TopAbs_SOLID:
  case TopAbs_SHELL:
  case TopAbs_WIRE:
  case TopAbs_SHAPE:
    // nothing to do for these kinds
    break;
  }
  return isSuccess;
}

//=======================================================================
//function : initOneShapePoints
//purpose  : Initialization of two points where dimension layouts
//           will be attached
// Attention: 1) <theShape> can be only the edge in currect implementation
//            2) No length for infinite edge
//=======================================================================

Standard_Boolean AIS_LengthDimension::initOneShapePoints (const TopoDS_Shape& theShape)
{
  if (theShape.ShapeType() == TopAbs_EDGE)
  {
    TopoDS_Edge anEdge = TopoDS::Edge (theShape);
    BRepAdaptor_Curve aBrepCurve(anEdge);
    Standard_Real aFirst = aBrepCurve.FirstParameter(),
                  aLast  = aBrepCurve.LastParameter();
    Standard_Boolean isInfinite = (Precision::IsInfinite (aFirst)
                                   || Precision::IsInfinite (aLast));
    if (isInfinite)
      return Standard_False;
    
    myFirstPoint = aBrepCurve.Value (aBrepCurve.FirstParameter());
    mySecondPoint = aBrepCurve.Value (aBrepCurve.LastParameter());
  }
  else // Some other kinds of shapes
    return Standard_False;
  return Standard_True;
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void AIS_LengthDimension::Compute (const Handle(PrsMgr_PresentationManager3d)& /*thePM*/,
                                   const Handle(Prs3d_Presentation)& thePresentation,
                                   const Standard_Integer theMode)
{
  // Initialization of points, if they are not set
  if (!myIsInitialized)
  {
    if (myShapesNumber == 1)
    {
      myIsInitialized = initOneShapePoints (myFirstShape);
    }
    else if (myShapesNumber == 2)
    {
      myIsInitialized = initTwoShapesPoints (myFirstShape, mySecondShape);
    }
    else
    {
      return;
    }
  }

  // If initialization failed
  if (!myIsInitialized)
  {
    return;
  }

  thePresentation->Clear();

  drawLinearDimension (thePresentation, (AIS_DimensionDisplayMode)theMode);
}

//=======================================================================
//function : ComputeValue
//purpose  : 
//=======================================================================

void AIS_LengthDimension::computeValue()
{
  myValue = myFirstPoint.Distance (mySecondPoint);
  AIS_Dimension::computeValue ();
}
