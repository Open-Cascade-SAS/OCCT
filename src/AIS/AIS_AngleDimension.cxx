// Created on: 1996-12-05
// Created by: Arnaud BOUZY/Odile Olivier
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

#include <AIS_AngleDimension.hxx>

#include <AIS.hxx>
#include <AIS_Drawer.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <ElCLib.hxx>
#include <gce_MakeLin2d.hxx>
#include <gce_MakeCone.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <IntAna_ResultType.hxx>
#include <ProjLib.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <Select3D_SensitiveCurve.hxx>
#include <Select3D_SensitiveGroup.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <SelectMgr_Selection.hxx>
#include <UnitsAPI.hxx>

IMPLEMENT_STANDARD_HANDLE (AIS_AngleDimension, AIS_Dimension)
IMPLEMENT_STANDARD_RTTIEXT (AIS_AngleDimension, AIS_Dimension)

namespace
{
  static const TCollection_ExtendedString THE_EMPTY_LABEL;
  static const Standard_ExtCharacter THE_DEGREE_SYMBOL (0x00B0);
};

//=======================================================================
//function : init
//purpose  : Private constructor for default initialization
//=======================================================================
void AIS_AngleDimension::init()
{
  // Default values of units
  UnitsAPI::SetLocalSystem (UnitsAPI_SI);
  SetUnitsQuantity ("PLANE ANGLE");
  SetModelUnits ("rad");
  SetDisplayUnits ("deg");
  SetSpecialSymbol (THE_DEGREE_SYMBOL);
  SetDisplaySpecialSymbol (AIS_DSS_After);
  SetFlyout (15.0);
  SetKindOfDimension (AIS_KOD_PLANEANGLE);
  MakeUnitsDisplayed (Standard_False);
}

//=======================================================================
//function : Constructor
//purpose  : Two edges dimension
//=======================================================================
AIS_AngleDimension::AIS_AngleDimension (const TopoDS_Edge& theFirstEdge,
                                        const TopoDS_Edge& theSecondEdge)
: AIS_Dimension(),
  myIsFlyoutLines (Standard_True)
{
  init();
  myShapesNumber = 2;
  myFirstShape   = theFirstEdge;
  mySecondShape  = theSecondEdge;
}

//=======================================================================
//function : Constructor
//purpose  : Two edges dimension
//           <thePlane> is used in case of Angle=PI
//=======================================================================
AIS_AngleDimension::AIS_AngleDimension (const TopoDS_Edge& theFirstEdge,
                                        const TopoDS_Edge& theSecondEdge,
                                        const gp_Pln& thePlane)
: AIS_Dimension(),
  myIsFlyoutLines (Standard_True)
{
  init();
  myShapesNumber = 2;
  myFirstShape   = theFirstEdge;
  mySecondShape  = theSecondEdge;
  SetWorkingPlane (thePlane);
}

//=======================================================================
//function : Constructor
//purpose  : Three points dimension
//=======================================================================
AIS_AngleDimension::AIS_AngleDimension (const gp_Pnt& theFirstPoint,
                                        const gp_Pnt& theSecondPoint,
                                        const gp_Pnt& theThirdPoint)
: AIS_Dimension(),
  myIsFlyoutLines (Standard_True)
{
  init();
  myIsInitialized = Standard_True;
  myFirstPoint    = theFirstPoint;
  myCenter        = theSecondPoint;
  mySecondPoint   = theThirdPoint;
  myShapesNumber  = 3;
}

//=======================================================================
//function : Constructor
//purpose  : Cone dimension
//=======================================================================
AIS_AngleDimension::AIS_AngleDimension (const TopoDS_Face& theCone)
: AIS_Dimension(),
  myIsFlyoutLines (Standard_True)
{
  init();
  myIsInitialized = Standard_False;
  myFirstShape    = theCone;
  myShapesNumber  = 1;
}

//=======================================================================
//function : Constructor
//purpose  : Two faces dimension
//=======================================================================
AIS_AngleDimension::AIS_AngleDimension (const TopoDS_Face& theFirstFace,
                                        const TopoDS_Face& theSecondFace,
                                        const gp_Ax1& theAxis)
: AIS_Dimension(),
  myIsFlyoutLines (Standard_True)
{
  init();
  myIsInitialized = Standard_False;
  myFirstShape    = theFirstFace;
  mySecondShape   = theSecondFace;
  myShapesNumber  = 2;
  gp_Pln aPlane;
  aPlane.SetAxis (theAxis);
  SetWorkingPlane (aPlane);
}

//=======================================================================
//function : SetFirstShape
//purpose  : 
//=======================================================================
void AIS_AngleDimension::SetFirstShape (const TopoDS_Shape& theShape,
                                        const Standard_Boolean isSingleShape /*= Standard_False*/)
{
  AIS_Dimension::SetFirstShape (theShape);
  if (isSingleShape)
    myShapesNumber = 1;
}

//=======================================================================
//function : aboveInBelowCone
//purpose  : Returns 1 if <theC> center is above of <theCMin> center;
//                   0 if <theC> center is between <theCMin> and
//                        <theCMax> centers;
//                  -1 if <theC> center is below <theCMax> center.
//=======================================================================
Standard_Integer AIS_AngleDimension::aboveInBelowCone (const gp_Circ &theCMax,
                                                       const gp_Circ &theCMin,
                                                       const gp_Circ &theC)
{
  const Standard_Real aD  = theCMax.Location().Distance (theCMin.Location());
  const Standard_Real aD1 = theCMax.Location().Distance (theC.Location());
  const Standard_Real aD2 = theCMin.Location().Distance (theC.Location());

  if (aD >= aD1 && aD >= aD2) return  0;
  if (aD  < aD2 && aD1 < aD2) return -1;
  if (aD  < aD1 && aD2 < aD1) return  1;
  return 0;
}

//=======================================================================
//function : initConeAngle
//purpose  : initialization of the cone angle
//=======================================================================
Standard_Boolean AIS_AngleDimension::initConeAngle (const TopoDS_Face& theCone)
{
  if (theCone.IsNull ())
    return Standard_False;

  gp_Pln aPln;
  gp_Cone aCone;
  gp_Circ aCircle;
  // A surface from the Face
  Handle(Geom_Surface) aSurf;
  Handle(Geom_OffsetSurface) aOffsetSurf; 
  Handle(Geom_ConicalSurface) aConicalSurf;
  Handle(Geom_SurfaceOfRevolution) aRevSurf;
  Handle(Geom_Line) aLine;
  BRepAdaptor_Surface aConeAdaptor (theCone);
  TopoDS_Face aFace;
  AIS_KindOfSurface aSurfType;
  Standard_Real anOffset = 0.;
  Handle(Standard_Type) aType;

  Standard_Real aMaxV = aConeAdaptor.FirstVParameter();
  Standard_Real aMinV = aConeAdaptor.LastVParameter();

  AIS::GetPlaneFromFace(theCone, aPln, aSurf, aSurfType, anOffset);

  if (aSurfType == AIS_KOS_Revolution)
  {
    // Surface of revolution
    aRevSurf = Handle(Geom_SurfaceOfRevolution)::DownCast(aSurf);
    gp_Lin aLin (aRevSurf->Axis());
    Handle(Geom_Curve) aBasisCurve = aRevSurf->BasisCurve();
    //Must be a part of line (basis curve should be linear)
    if (aBasisCurve ->DynamicType() != STANDARD_TYPE(Geom_Line))
      return Standard_False;

    gp_Pnt aFirst1 = aConeAdaptor.Value (0., aMinV);
    gp_Pnt aLast1 = aConeAdaptor.Value (0., aMaxV);
    gp_Vec aVec1 (aFirst1, aLast1);

    //Projection <aFirst> on <aLin>
    gp_Pnt aFirst2 = ElCLib::Value (ElCLib::Parameter (aLin, aFirst1), aLin);
    // Projection <aLast> on <aLin>
    gp_Pnt aLast2 = ElCLib::Value (ElCLib::Parameter (aLin, aLast1), aLin);

    gp_Vec aVec2 (aFirst2, aLast2);

    // Check if two parts of revolution are parallel (it's a cylinder) or normal (it's a circle).
    if (aVec1.IsParallel (aVec2, Precision::Angular())
        || aVec1.IsNormal (aVec2,Precision::Angular()))
      return Standard_False;

    gce_MakeCone aMkCone (aRevSurf->Axis(), aFirst1, aLast1);
    aCone =  aMkCone.Value();
    myCenter = aCone.Apex();
  }
  else
  {
    aType = aSurf->DynamicType();
    if (aType == STANDARD_TYPE(Geom_OffsetSurface) || anOffset > 0.01)
    {
      // Offset surface
      aOffsetSurf = new Geom_OffsetSurface (aSurf, anOffset);
      aSurf = aOffsetSurf->Surface();
      BRepBuilderAPI_MakeFace aMkFace(aSurf, Precision::Confusion());
      aMkFace.Build();
      if (!aMkFace.IsDone())
        return Standard_False;
      aConeAdaptor.Initialize (aMkFace.Face());
    }
    aCone = aConeAdaptor.Cone();
    aConicalSurf = Handle(Geom_ConicalSurface)::DownCast (aSurf);
    myCenter =  aConicalSurf->Apex();
  }

  // A circle where the angle is drawn
  Handle(Geom_Curve) aCurve;
  Standard_Real aMidV = ( aMinV + aMaxV ) / 2.5;
  aCurve = aSurf->VIso (aMidV);
  aCircle = Handle(Geom_Circle)::DownCast (aCurve)->Circ();

  aCurve = aSurf->VIso(aMaxV);
  gp_Circ aCircVmax = Handle(Geom_Circle)::DownCast(aCurve)->Circ();
  aCurve = aSurf->VIso(aMinV);
  gp_Circ aCircVmin = Handle(Geom_Circle)::DownCast(aCurve)->Circ();


  if (aCircVmax.Radius() < aCircVmin.Radius())
  {
   gp_Circ aTmpCirc = aCircVmax;
   aCircVmax = aCircVmin;
   aCircVmin = aTmpCirc;
  }

  myFirstPoint  = ElCLib::Value (0, aCircle);
  mySecondPoint = ElCLib::Value (M_PI, aCircle);
  return Standard_True;
}

//=======================================================================
//function : initTwoFacesAngle
//purpose  : initialization of angle dimension between two faces
//=======================================================================
Standard_Boolean AIS_AngleDimension::initTwoFacesAngle ()
{
  TopoDS_Face aFirstFace = TopoDS::Face (myFirstShape);
  TopoDS_Face aSecondFace = TopoDS::Face (mySecondShape);
  gp_Dir aFirstDir, aSecondDir;
  gp_Pln aFirstPlane, aSecondPlane;
  Handle(Geom_Surface) aFirstBasisSurf, aSecondBasisSurf;
  AIS_KindOfSurface aFirstSurfType, aSecondSurfType;
  Standard_Real aFirstOffset, aSecondOffset;
  
  AIS::GetPlaneFromFace (aFirstFace, aFirstPlane,
                         aFirstBasisSurf,aFirstSurfType,aFirstOffset);
  AIS::GetPlaneFromFace (aSecondFace, aSecondPlane,
                         aSecondBasisSurf, aSecondSurfType, aSecondOffset);

  if (aFirstSurfType == AIS_KOS_Plane)
  {
    //Planar faces angle
    AIS::ComputeAngleBetweenPlanarFaces (aFirstFace,
                                        aSecondFace,
                                        aSecondBasisSurf,
                                        GetWorkingPlane().Axis(),
                                        myValue,
                                        Standard_True,
                                        myGeom.myTextPosition,
                                        myCenter,
                                        myFirstPoint,
                                        mySecondPoint,
                                        aFirstDir,
                                        aSecondDir);
  }
  else
  {
        // Curvilinear faces angle
    Handle(Geom_Plane) aPlane = new Geom_Plane (GetWorkingPlane());
    AIS::ComputeAngleBetweenCurvilinearFaces (aFirstFace,
                                             aSecondFace,
                                             aFirstBasisSurf,
                                             aSecondBasisSurf,
                                             aFirstSurfType,
                                             aSecondSurfType,
                                             GetWorkingPlane().Axis(),
                                             myValue,
                                             Standard_True,
                                             myGeom.myTextPosition,
                                             myCenter,
                                             myFirstPoint,
                                             mySecondPoint,
                                             aFirstDir,
                                             aSecondDir,
                                             aPlane);
    SetWorkingPlane (aPlane->Pln());
  }
  return Standard_True;
}

//=======================================================================
//function : countDefaultPlane
//purpose  : 
//=======================================================================
void AIS_AngleDimension::countDefaultPlane ()
{
  if (!myIsInitialized)
    return;
  // Compute normal of the default plane.
  gp_Vec aVec1(myCenter, myFirstPoint),
         aVec2(myCenter, mySecondPoint);
  myDefaultPlane = gp_Pln(myCenter, aVec1^aVec2);
  // Set computed value to <myWorkingPlane>
  ResetWorkingPlane ();
}

//=======================================================================
//function : computeValue
//purpose  : 
//=======================================================================
void AIS_AngleDimension::computeValue ()
{
  gp_Vec aVec1 (myCenter, myFirstPoint),
         aVec2 (myCenter, mySecondPoint);
  myValue = aVec1.Angle (aVec2);
  // To model units
  AIS_Dimension::computeValue();
}

//=======================================================================
//function : initTwoEdgesAngle
//purpose  : Fill gp_Pnt fields for further presentation computation
//           If intersection between two edges doesn't exist
//           <myIsInitialized> is set to false
//=======================================================================
Standard_Boolean AIS_AngleDimension::initTwoEdgesAngle ()
{
  // Data initialization
  TopoDS_Edge aFirstEdge = TopoDS::Edge (myFirstShape);
  TopoDS_Edge aSecondEdge = TopoDS::Edge (mySecondShape);
  BRepAdaptor_Curve aMakeFirstLine (aFirstEdge);
  BRepAdaptor_Curve aMakeSecondLine (aSecondEdge);

  if (aMakeFirstLine.GetType() != GeomAbs_Line || aMakeSecondLine.GetType() != GeomAbs_Line)
  {
    return  Standard_False;
  }

  Handle(Geom_Line) aFirstLine  = new Geom_Line (aMakeFirstLine.Line());
  Handle(Geom_Line) aSecondLine = new Geom_Line (aMakeSecondLine.Line());

  gp_Lin aFirstLin  = aFirstLine->Lin ();
  gp_Lin aSecondLin = aSecondLine->Lin ();
  gp_Lin2d aFirstLin2d, aSecondLin2d;
  Standard_Boolean isParallelLines = aFirstLin.Direction().IsParallel (aSecondLin.Direction(), Precision::Angular());
  Standard_Boolean isSameLines = isParallelLines && aFirstLin.Distance (aSecondLin.Location()) <= Precision::Confusion();
  // In case where we can't compute plane automatically
  if ((isParallelLines || isSameLines) && !myIsWorkingPlaneCustom)
  {
    return Standard_False;
  }

  gp_Pln aPlane;

  /// PART 1 is for automatic plane computation from two edges if it is possible
  // Build plane
  if (!myIsWorkingPlaneCustom)
  {
    gp_Pnt aPoint = aFirstLine->Value (0.);
    gp_Dir aNormal = isParallelLines
                     ? gp_Vec(aSecondLin.Normal (aPoint).Direction()) ^ gp_Vec (aSecondLin.Direction())
                     : gp_Vec (aFirstLin.Direction()) ^ gp_Vec (aSecondLin.Direction());
    aPlane = gp_Pln (aPoint, aNormal);
    resetWorkingPlane (aPlane);
  }
  else
  {
    aPlane = GetWorkingPlane();
  }

  // Compute geometry for this plane and edges
  Standard_Boolean isInfinite1,isInfinite2;
  gp_Pnt aFirstPoint1, aLastPoint1, aFirstPoint2, aLastPoint2;
  Standard_Integer anExtIndex = -1;
  Handle(Geom_Curve) anExtCurve;
  Handle(Geom_Plane) aGeomPlane = new Geom_Plane (aPlane);
  if (!AIS::ComputeGeometry (aFirstEdge, aSecondEdge,
                             anExtIndex,
                             aFirstLine, aSecondLine,
                             aFirstPoint1, aLastPoint1,
                             aFirstPoint2, aLastPoint2,
                             anExtCurve,
                             isInfinite1, isInfinite2,
                             aGeomPlane))
  {
    return Standard_False;
  }

  // Check if both edges are on this plane
  if (!anExtCurve.IsNull())
  {
    if (anExtIndex == 1) // First curve is out of the plane
    {
      // Project curve on the plane
      if (myIsWorkingPlaneCustom)
      {
        aFirstLin2d = ProjLib::Project (aPlane, aFirstLin);
        aFirstLin = ElCLib::To3d (aPlane.Position().Ax2(), aFirstLin2d);
      }
      else
      {
        aFirstLin.Translate (gp_Vec (aFirstLin.Location(), aSecondLin.Location()));
      }

      aFirstLine = new Geom_Line (aFirstLin);
    }
    else if (anExtIndex == 2) // Second curve is out of the plane
    {
      if (myIsWorkingPlaneCustom)
      {
        aSecondLin2d = ProjLib::Project (aPlane, aSecondLin);
        aSecondLin = ElCLib::To3d (aPlane.Position().Ax2(), aSecondLin2d);
      }
      else
      {
        aSecondLin.Translate (gp_Vec (aSecondLin.Location(), aFirstLin.Location()));
      }

      aSecondLine = new Geom_Line (aSecondLin);
    }
  }

  /// PART 2 is for dimension computation using the working plane

  if (aFirstLin.Direction ().IsParallel (aSecondLin.Direction (), Precision::Angular ()))
  {
    // Parallel lines
    isSameLines = aFirstLin.Distance(aSecondLin.Location()) <= Precision::Confusion();
    if (!isSameLines)
      return Standard_False;

     myFirstPoint = aFirstLin.Location();
     mySecondPoint = ElCLib::Value (ElCLib::Parameter (aFirstLin, myFirstPoint), aSecondLin);
     if (mySecondPoint.Distance (mySecondPoint) <= Precision::Confusion ())
       mySecondPoint.Translate (gp_Vec (aSecondLin.Direction ())*Abs(GetFlyout()));
     myCenter.SetXYZ( (myFirstPoint.XYZ() + mySecondPoint.XYZ()) / 2. );
  }
  else
  {
    // Find intersection
    aFirstLin2d = ProjLib::Project (aPlane, aFirstLin);
    aSecondLin2d = ProjLib::Project (aPlane, aSecondLin);

    IntAna2d_AnaIntersection anInt2d (aFirstLin2d, aSecondLin2d);
    gp_Pnt2d anIntersectPoint;
    if (!anInt2d.IsDone() || anInt2d.IsEmpty())
    {
      return Standard_False;
    }

    anIntersectPoint = gp_Pnt2d (anInt2d.Point(1).Value());
    myCenter = ElCLib::To3d(aPlane.Position().Ax2(), anIntersectPoint);

    if (isInfinite1 || isInfinite2)
    {
      myFirstPoint  = myCenter.Translated (gp_Vec (aFirstLin.Direction())*Abs (GetFlyout()));
      mySecondPoint = myCenter.Translated (gp_Vec (aSecondLin.Direction())*Abs (GetFlyout()));
      return Standard_True;
    }

    // |
    // | <- dimension should be here
    // *----
    myFirstPoint  = myCenter.Distance (aFirstPoint1) > myCenter.Distance (aLastPoint1) ? aFirstPoint1 : aLastPoint1;
    mySecondPoint = myCenter.Distance (aFirstPoint2) > myCenter.Distance (aLastPoint2) ? aFirstPoint2 : aLastPoint2;
  }
  return Standard_True;
}

//=======================================================================
//function: getCenterOnArc
//purpose :
//=======================================================================
gp_Pnt AIS_AngleDimension::getCenterOnArc (const gp_Pnt& theFirstAttach,
                                           const gp_Pnt& theSecondAttach)
{
  gp_Pnt2d aCenter2d       = ProjLib::Project (GetWorkingPlane(), myCenter),
           aFirstAttach2d  = ProjLib::Project (GetWorkingPlane(), theFirstAttach),
           aSecondAttach2d = ProjLib::Project (GetWorkingPlane(), theSecondAttach);
  gp_Lin2d anAttachLine2d = gce_MakeLin2d (aFirstAttach2d, aSecondAttach2d);

  // Getting text center
  gp_Pnt2d aTextCenterPnt = ElCLib::Value ((ElCLib::Parameter (anAttachLine2d, aFirstAttach2d) + ElCLib::Parameter (anAttachLine2d, aSecondAttach2d)) / 2., anAttachLine2d);
  gp_Lin2d aCenterToTextCenterLin = gce_MakeLin2d (aCenter2d, aTextCenterPnt);

  // Drawing circle
  Standard_Real aRadius = theFirstAttach.Distance (myCenter);
  gp_Circ2d aCircle (gp_Ax22d (aCenter2d, gp_Dir2d (1, 0)), aRadius);

  // Getting text position in the center of arc
  IntAna2d_AnaIntersection anInt2d (aCenterToTextCenterLin, aCircle);
  gp_Pnt2d aTextCenterOnArc2d;
  if (anInt2d.IsDone())
    if (!anInt2d.IsEmpty())
      aTextCenterOnArc2d = gp_Pnt2d (anInt2d.Point (1).Value());
  gp_Pnt aCenterOnArc = ElCLib::To3d (GetWorkingPlane().Position().Ax2(), aTextCenterOnArc2d);
  return aCenterOnArc;
}

//=======================================================================
//function: drawArcWithText
//purpose :
//=======================================================================
void AIS_AngleDimension::drawArcWithText (const Handle(Prs3d_Presentation)& thePresentation,
                                          const gp_Pnt& theFirstAttach,
                                          const gp_Pnt& theSecondAttach,
                                          const TCollection_ExtendedString& theText,
                                          const AIS_DimensionDisplayMode theMode,
                                          const Standard_Integer theLabelPosition)
{
  gp_Pnt2d aCenter2d       = ProjLib::Project (GetWorkingPlane(), myCenter),
           aFirstAttach2d  = ProjLib::Project (GetWorkingPlane(), theFirstAttach),
           aSecondAttach2d = ProjLib::Project (GetWorkingPlane(), theSecondAttach);
  gp_Lin2d anAttachLine2d = gce_MakeLin2d (aFirstAttach2d, aSecondAttach2d);

  // Getting text center
  gp_Pnt2d aTextCenterPnt = ElCLib::Value ((ElCLib::Parameter (anAttachLine2d, aFirstAttach2d) + ElCLib::Parameter (anAttachLine2d, aSecondAttach2d)) / 2., anAttachLine2d);
  gp_Lin2d aCenterToTextCenterLin = gce_MakeLin2d (aCenter2d, aTextCenterPnt);

  // Drawing circle
  Standard_Real aRadius = theFirstAttach.Distance (myCenter);
  gp_Circ2d aCircle (gp_Ax22d (aCenter2d, gp_Dir2d (1, 0)), aRadius);

  // Getting text position in the center of arc
  IntAna2d_AnaIntersection anInt2d (aCenterToTextCenterLin, aCircle);
  gp_Pnt2d aTextCenterOnArc2d;
  if (anInt2d.IsDone())
    if (!anInt2d.IsEmpty())
      aTextCenterOnArc2d = gp_Pnt2d (anInt2d.Point (1).Value());
  myGeom.myTextPosition = ElCLib::To3d (GetWorkingPlane().Position().Ax2(), aTextCenterOnArc2d);

  // Drawing text
  gp_Vec aVec (theFirstAttach, theSecondAttach);
  Standard_Real aTextWidth = drawText (thePresentation,
                                       myIsTextReversed ? aVec.Reversed() : aVec,
                                       theText, theMode,
                                       theLabelPosition);

  // Getting text begin and end points
  gp_Pnt2d aTextBeginPnt = ElCLib::Value ((ElCLib::Parameter (anAttachLine2d, aFirstAttach2d) +
                                           ElCLib::Parameter (anAttachLine2d, aSecondAttach2d) -
                                           aTextWidth) / 2., anAttachLine2d),
           aTextEndPnt   = ElCLib::Value (ElCLib::Parameter (anAttachLine2d,aTextBeginPnt) + aTextWidth, anAttachLine2d);


  gp_Lin2d aCenterToTextBeginLin = gce_MakeLin2d (aCenter2d, aTextBeginPnt),
           aCenterToTextEndLin   = gce_MakeLin2d (aCenter2d, aTextEndPnt);

  // Text begin and end on the dimension arc
  gp_Pnt2d aTextBeginOnArc2d, aTextEndOnArc2d;
  anInt2d.Perform (aCenterToTextBeginLin, aCircle);
  if (anInt2d.IsDone())
    if (!anInt2d.IsEmpty())
      aTextBeginOnArc2d = gp_Pnt2d (anInt2d.Point (1).Value());

  anInt2d.Perform (aCenterToTextEndLin, aCircle);
  if (anInt2d.IsDone())
    if (!anInt2d.IsEmpty())
      aTextEndOnArc2d = gp_Pnt2d (anInt2d.Point (1).Value());

  gp_Pnt aTextBeginOnArc = ElCLib::To3d (GetWorkingPlane().Position().Ax2(), aTextBeginOnArc2d);
  gp_Pnt aTextEndOnArc   = ElCLib::To3d (GetWorkingPlane().Position().Ax2(), aTextEndOnArc2d);

  // Drawing arcs
  if (theMode != AIS_DDM_Text)
  {
    drawArc (thePresentation, theFirstAttach, aTextBeginOnArc, myCenter, aRadius, theMode);
    drawArc (thePresentation, aTextEndOnArc, theSecondAttach, myCenter, aRadius, theMode);
  }
}

//=======================================================================
//function : drawArc
//purpose  : draws the arc between two attach points
//=======================================================================
void AIS_AngleDimension::drawArc (const Handle(Prs3d_Presentation)& thePresentation,
                                  const gp_Pnt& theFirstAttach,
                                  const gp_Pnt& theSecondAttach,
                                  const gp_Pnt& theCenter,
                                  const Standard_Real theRadius,
                                  const AIS_DimensionDisplayMode theMode)
{
  Handle(SelectMgr_EntityOwner) anEmptyOwner;

  gp_Vec aCenterToFirstVec (theCenter,theFirstAttach);
  gp_Vec aCenterToSecondVec (theCenter,theSecondAttach);
  gp_Dir aCenterToFirstDir (aCenterToFirstVec);
  gp_Dir aPlaneNormal = GetWorkingPlane().Axis().Direction();
  gp_Dir aCenterToSecondDir = aPlaneNormal.Crossed (aCenterToFirstDir);

  const Standard_Real anAngle = aCenterToFirstVec.Angle(aCenterToSecondVec);
  const Standard_Integer aPointsOnArc = Max (4 , Standard_Integer (50. * anAngle / M_PI));
  const Standard_Real anAngleStep = anAngle / (aPointsOnArc - 1);
  TColgp_Array1OfPnt aPointArray (0,aPointsOnArc-1);
  Handle(Graphic3d_ArrayOfPolylines) aPrimSegments = new Graphic3d_ArrayOfPolylines (aPointsOnArc,2);
  aPrimSegments->AddVertex (theFirstAttach);
  aPointArray.SetValue(0, theFirstAttach);
  gp_Pnt aPoint = theFirstAttach;
  gp_Vec aVector;

  for (Standard_Integer anI = 1; anI < aPointsOnArc - 1; ++anI)
  {
    aVector = (gp_Vec(aCenterToFirstDir) * Cos ( (anI - 1) * anAngleStep) + gp_Vec(aCenterToSecondDir) * Sin ( (anI - 1) * anAngleStep)) * theRadius;
    aPoint = theCenter.Translated(aVector);
    aPrimSegments->AddVertex(aPoint);
    aPointArray.SetValue (anI,aPoint);
  }
  aPrimSegments->AddVertex (theSecondAttach);
  aPointArray.SetValue (aPointsOnArc - 1,theSecondAttach);

  // Fill sensitive list
  myGeom.mySensitiveSegments.Append(new Select3D_SensitiveCurve(anEmptyOwner,aPointArray));

  // Fill display presentation
  if (!myDrawer->DimensionAspect()->IsText3d() && theMode == AIS_DDM_All)
  {
    Prs3d_Root::CurrentGroup (thePresentation)->SetStencilTestOptions (Standard_True);
  }
  Handle(Graphic3d_AspectLine3d) aDimensionLineStyle = myDrawer->DimensionAspect()->LineAspect()->Aspect();
  Prs3d_Root::CurrentGroup (thePresentation)->SetPrimitivesAspect (aDimensionLineStyle);
  Prs3d_Root::CurrentGroup (thePresentation)->AddPrimitiveArray (aPrimSegments);
  if (!myDrawer->DimensionAspect()->IsText3d() && theMode == AIS_DDM_All)
  {
    Prs3d_Root::CurrentGroup (thePresentation)->SetStencilTestOptions (Standard_False);
  }
}

//=======================================================================
//function : Compute
//purpose  : Having three gp_Pnt points compute presentation
//=======================================================================
void AIS_AngleDimension::Compute (const Handle(PrsMgr_PresentationManager3d)& /*thePM*/,
                                  const Handle(Prs3d_Presentation)& thePresentation,
                                  const Standard_Integer theMode)
{
  thePresentation->Clear();
  myGeom.mySensitiveSegments.Clear();
  Handle(SelectMgr_EntityOwner) anEmptyOwner;

  if (!myIsInitialized)
  {
    if (myShapesNumber == 1)
    {
      myIsInitialized = initConeAngle (TopoDS::Face (myFirstShape));
    }
    else if (myShapesNumber == 2)
    {
      switch (myFirstShape.ShapeType())
      {
      case TopAbs_FACE:
        {
          myIsInitialized = initTwoFacesAngle ();
        }
        break;
      case TopAbs_EDGE:
        {
          myIsInitialized = initTwoEdgesAngle ();
        }
        break;
      default:
        return;
      }
    }
    else
      return;
  }

  // If initialization failed
  if (!myIsInitialized)
    return;

  // Parameters for presentation
  Handle(Prs3d_DimensionAspect) aDimensionAspect = myDrawer->DimensionAspect();

  Prs3d_Root::CurrentGroup(thePresentation)->SetPrimitivesAspect(aDimensionAspect->LineAspect()->Aspect());

  Quantity_Length anArrowLength = aDimensionAspect->ArrowAspect()->Length();

  if (!myIsValueCustom)
  {
    computeValue();
  }

  TCollection_ExtendedString aValueString;
  Standard_Real aTextLength;
  getTextWidthAndString (aTextLength, aValueString);

  if (!myIsWorkingPlaneCustom)
  {
    countDefaultPlane();
  }

  gp_Pnt aFirstAttach = myCenter.Translated (gp_Vec(myCenter, myFirstPoint).Normalized() * GetFlyout());
  gp_Pnt aSecondAttach = myCenter.Translated (gp_Vec(myCenter, mySecondPoint).Normalized() * GetFlyout());

  // Handle user-defined and automatic arrow placement
  bool isArrowsExternal = false;
  switch (aDimensionAspect->ArrowOrientation())
  {
    case Prs3d_DAO_External: isArrowsExternal = true; break;
    case Prs3d_DAO_Internal: isArrowsExternal = false; break;
    case Prs3d_DAO_Fit:
    {
      gp_Vec anAttachVector (aFirstAttach, aSecondAttach);
      Standard_Real aDimensionWidth = anAttachVector.Magnitude();
      Standard_Real anArrowsWidth   = anArrowLength * 2.0;

      isArrowsExternal = aDimensionWidth < aTextLength + anArrowsWidth;
      break;
    }
  }

  //Arrows positions and directions
  gp_Vec aWPDir = gp_Vec (GetWorkingPlane().Axis().Direction());

  gp_Dir aFirstExtensionDir  = gp_Vec (myCenter, aFirstAttach) ^ aWPDir;
  gp_Dir aSecondExtensionDir = gp_Vec (myCenter, aSecondAttach)^ aWPDir.Reversed();

  gp_Vec aFirstArrowVec  = gp_Vec (aFirstExtensionDir)  * anArrowLength;
  gp_Vec aSecondArrowVec = gp_Vec (aSecondExtensionDir) * anArrowLength;

  gp_Pnt aFirstArrowBegin  (0.0, 0.0, 0.0);
  gp_Pnt aFirstArrowEnd    (0.0, 0.0, 0.0);
  gp_Pnt aSecondArrowBegin (0.0, 0.0, 0.0);
  gp_Pnt aSecondArrowEnd   (0.0, 0.0, 0.0);

  if (isArrowsExternal)
  {
    aFirstArrowVec.Reverse();
    aSecondArrowVec.Reverse();
  }

  aFirstArrowBegin  = aFirstAttach;
  aSecondArrowBegin = aSecondAttach;
  aFirstArrowEnd    = aFirstAttach.Translated (-aFirstArrowVec);
  aSecondArrowEnd   = aSecondAttach.Translated (-aSecondArrowVec);

  Standard_Integer aLabelPosition = LabelPosition_None;

  // Handle user-defined and automatic text placement
  switch (aDimensionAspect->TextHorizontalPosition())
  {
    case Prs3d_DTHP_Left  : aLabelPosition |= LabelPosition_Left; break;
    case Prs3d_DTHP_Right : aLabelPosition |= LabelPosition_Right; break;
    case Prs3d_DTHP_Center: aLabelPosition |= LabelPosition_HCenter; break;
    case Prs3d_DTHP_Fit:
    {
      gp_Vec anAttachVector (aFirstAttach, aSecondAttach);
      Standard_Real aDimensionWidth = anAttachVector.Magnitude();
      Standard_Real anArrowsWidth   = anArrowLength * 2.0;
      Standard_Real aContentWidth   = isArrowsExternal ? aTextLength : aTextLength + anArrowsWidth;

      aLabelPosition |= aDimensionWidth < aContentWidth ? LabelPosition_Left : LabelPosition_HCenter;
      break;
    }
  }

  switch (aDimensionAspect->TextVerticalPosition())
  {
    case Prs3d_DTVP_Above  : aLabelPosition |= LabelPosition_Above; break;
    case Prs3d_DTVP_Below  : aLabelPosition |= LabelPosition_Below; break;
    case Prs3d_DTVP_Center : aLabelPosition |= LabelPosition_VCenter; break;
  }

  // Group1: stenciling text and the angle dimension arc
  Prs3d_Root::NewGroup (thePresentation);

  Standard_Real anExtensionSize = aDimensionAspect->ExtensionSize();

  Standard_Integer aHPosition = aLabelPosition & LabelPosition_HMask;

  // draw text label
  switch (aHPosition)
  {
    case LabelPosition_HCenter :
    {
      Standard_Boolean isLineBreak = aDimensionAspect->TextVerticalPosition() == Prs3d_DTVP_Center
                                  && aDimensionAspect->IsText3d();

      if (isLineBreak)
      {
        drawArcWithText (thePresentation,
                         aFirstAttach,
                         aSecondAttach,
                         aValueString,
                         (AIS_DimensionDisplayMode)theMode,
                         aLabelPosition);
        break;
      }

      gp_Vec aTextDir (aFirstArrowEnd, aSecondArrowBegin);
      myGeom.myTextPosition = getCenterOnArc (aFirstArrowEnd, aSecondArrowBegin);

      drawText (thePresentation,
                myIsTextReversed ? aTextDir.Reversed() : aTextDir,
                aValueString,
                (AIS_DimensionDisplayMode)theMode,
                aLabelPosition);

      if (theMode == AIS_DDM_Text)
      {
        break;
      }

      drawArc (thePresentation,
               isArrowsExternal ? aFirstAttach : aFirstArrowEnd,
               isArrowsExternal ? aSecondAttach : aSecondArrowEnd,
               myCenter,
               Abs (GetFlyout()),
               (AIS_DimensionDisplayMode)theMode);
    }
    break;

    case LabelPosition_Left :
    {
      drawExtension (thePresentation,
                     anExtensionSize,
                     isArrowsExternal ? aFirstArrowEnd : aFirstAttach,
                     aFirstExtensionDir,
                     aValueString,
                     (AIS_DimensionDisplayMode)theMode,
                     aLabelPosition);
    }
    break;

    case LabelPosition_Right :
    {
      drawExtension (thePresentation,
                     anExtensionSize,
                     isArrowsExternal ? aSecondArrowEnd : aSecondAttach,
                     aSecondExtensionDir,
                     aValueString,
                     (AIS_DimensionDisplayMode)theMode,
                     aLabelPosition);
    }
    break;
  }

  // dimension arc without text
  if (theMode != AIS_DDM_Text && aHPosition != LabelPosition_HCenter)
  {
    Prs3d_Root::NewGroup (thePresentation);

    drawArc (thePresentation,
             isArrowsExternal ? aFirstAttach  : aFirstArrowEnd,
             isArrowsExternal ? aSecondAttach : aSecondArrowEnd,
             myCenter,
             Abs(GetFlyout ()),
             (AIS_DimensionDisplayMode)theMode);
  }

  // arrows and arrow extensions
  if (theMode != AIS_DDM_Text)
  {
    Prs3d_Root::NewGroup (thePresentation);

    drawArrow (thePresentation, aFirstArrowBegin,  gp_Dir (aFirstArrowVec));
    drawArrow (thePresentation, aSecondArrowBegin, gp_Dir (aSecondArrowVec));
  }

  if (theMode != AIS_DDM_Text && isArrowsExternal)
  {
    Prs3d_Root::NewGroup (thePresentation);

    if (aHPosition != LabelPosition_Left)
    {
      drawExtension (thePresentation,
                     anExtensionSize,
                     aFirstArrowEnd,
                     aFirstExtensionDir,
                     THE_EMPTY_LABEL,
                     (AIS_DimensionDisplayMode)theMode,
                     LabelPosition_None);
    }

    if (aHPosition != LabelPosition_Right)
    {
      drawExtension (thePresentation,
                     anExtensionSize,
                     aSecondArrowEnd,
                     aSecondExtensionDir,
                     THE_EMPTY_LABEL,
                     (AIS_DimensionDisplayMode)theMode,
                     LabelPosition_None);
    }
  }

  // flyouts
  if (theMode == AIS_DDM_All && myIsFlyoutLines)
  {
    Prs3d_Root::NewGroup (thePresentation);

    Handle(Graphic3d_ArrayOfSegments) aPrimSegments = new Graphic3d_ArrayOfSegments (4);
    aPrimSegments->AddVertex (myCenter);
    aPrimSegments->AddVertex (aFirstAttach);
    aPrimSegments->AddVertex (myCenter);
    aPrimSegments->AddVertex (aSecondAttach);

    Handle(Graphic3d_AspectLine3d) aFlyoutStyle = myDrawer->DimensionAspect()->LineAspect()->Aspect();
    Prs3d_Root::CurrentGroup (thePresentation)->SetPrimitivesAspect (aFlyoutStyle);
    Prs3d_Root::CurrentGroup (thePresentation)->AddPrimitiveArray (aPrimSegments);
  }

  setComputed (Standard_True);
}

//=======================================================================
//function : computeFlyoutSelection
//purpose  : computes selection for flyouts
//=======================================================================
void AIS_AngleDimension::computeFlyoutSelection (const Handle(SelectMgr_Selection)& theSelection,
                                                const Handle(AIS_DimensionOwner)& theOwner)
{
 if (!myIsFlyoutLines)
 {
   return;
 }

 gp_Pnt aFirstAttach = myCenter.Translated (gp_Vec (myCenter, myFirstPoint).Normalized() * GetFlyout());
 gp_Pnt aSecondAttach = myCenter.Translated (gp_Vec (myCenter, mySecondPoint).Normalized() * GetFlyout());
 Handle(Select3D_SensitiveGroup) aSensitiveEntity = new Select3D_SensitiveGroup (theOwner);
 aSensitiveEntity->Add (new Select3D_SensitiveSegment (theOwner, myCenter, aFirstAttach));
 aSensitiveEntity->Add (new Select3D_SensitiveSegment (theOwner, myCenter, aSecondAttach));
 theSelection->Add (aSensitiveEntity);
}
