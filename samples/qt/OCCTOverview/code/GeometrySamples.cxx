// Copyright (c) 2020 OPEN CASCADE SAS
//
// This file is part of the examples of the Open CASCADE Technology software library.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

#include "GeometrySamples.h"

#include <limits>

#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Lin.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Parab.hxx>
#include <gp_Hypr.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Cone.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>

#include <gp_Pnt2d.hxx>
#include <gp_XY.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Hypr2d.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <ProjLib.hxx>
#include <ElSLib.hxx>
#include <Extrema_ExtElCS.hxx>
#include <Extrema_POnCurv.hxx>
#include <IntAna_Quadric.hxx>
#include <IntAna_IntConicQuad.hxx>
#include <GccAna_Lin2d2Tan.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <Geom2dAPI_ExtremaCurveCurve.hxx>
#include <Geom2dAPI_InterCurveCurve.hxx>
#include <Geom2dAPI_PointsToBSpline.hxx>

#include <Geom_CartesianPoint.hxx>
#include <Geom_VectorWithMagnitude.hxx>
#include <Geom_Axis1Placement.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>

#include <BndLib_Add3dCurve.hxx>
#include <BndLib_AddSurface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>
#include <GeomConvert.hxx>

#include "AdaptorPnt2d_AIS.h"
#include "AdaptorVec_AIS.h"
#include "AdaptorCurve_AIS.h"
#include "AdaptorCurve2d_AIS.h"

#include <Geom2d_CartesianPoint.hxx>
#include <Geom2d_VectorWithMagnitude.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <Adaptor2d_Curve2d.hxx>

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepPrimAPI_MakeBox.hxx>

#include <AIS_Point.hxx>
#include <AIS_TextLabel.hxx>
#include <AIS_Axis.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Circle.hxx>
#include <AIS_Plane.hxx>
#include <AIS_Shape.hxx>
#include <AIS_ColoredShape.hxx>

#include <GProp_PEquation.hxx>

void GeometrySamples::ExecuteSample (const TCollection_AsciiString& theSampleName)
{
  Standard_Boolean anIsSamplePresent = Standard_True;
  FindSourceCode(theSampleName);
  if (theSampleName == "ZeroDimensionObjects3dSample")
    ZeroDimensionObjects3dSample();
  else if (theSampleName == "Vectors3dSample")
    Vectors3dSample();
  else if (theSampleName == "InfinityLines3dSample")
    InfinityLines3dSample();
  else if (theSampleName == "SecondOrderCurves3dSample")
    SecondOrderCurves3dSample();
  else if (theSampleName == "PlaneSurfaces3dSample")
    PlaneSurfaces3dSample();
  else if (theSampleName == "SecondOrderSurfaces3dSample")
    SecondOrderSurfaces3dSample();
  else if (theSampleName == "ZeroDimensionObjects2dSample")
    ZeroDimensionObjects2dSample();
  else if (theSampleName == "Vectors2dSample")
    Vectors2dSample();
  else if (theSampleName == "InfinityLines2dSample")
    InfinityLines2dSample();
  else if (theSampleName == "SecondOrderCurves2dSample")
    SecondOrderCurves2dSample();
  else if (theSampleName == "BarycenterPoint3dSample")
    BarycenterPoint3dSample();
  else if (theSampleName == "RotatedVector3dSample")
    RotatedVector3dSample();
  else if (theSampleName == "MirroredLine3dSample")
    MirroredLine3dSample();
  else if (theSampleName == "ScaledEllipse3dSample")
    ScaledEllipse3dSample();
  else if (theSampleName == "TransformedCylinder3dSample")
    TransformedCylinder3dSample();
  else if (theSampleName == "TranslatedTorus3dSample")
    TranslatedTorus3dSample();
  else if (theSampleName == "ConjugateObjects3dSample")
    ConjugateObjects3dSample();
  else if (theSampleName == "ProjectionOfPoint3dSample")
    ProjectionOfPoint3dSample();
  else if (theSampleName == "MinimalDistance3dSample")
    MinimalDistance3dSample();
  else if (theSampleName == "MirroredAxis2dSample")
    MirroredAxis2dSample();
  else if (theSampleName == "TransformedEllipse2dSample")
    TransformedEllipse2dSample();
  else if (theSampleName == "ConjugateObjects2dSample")
    ConjugateObjects2dSample();
  else if (theSampleName == "Intersection3dSample")
    Intersection3dSample();
  else if (theSampleName == "TranslatedPoint2dSample")
    TranslatedPoint2dSample();
  else if (theSampleName == "RotatedDirection2dSample")
    RotatedDirection2dSample();
  else if (theSampleName == "Tangent2dSample")
    Tangent2dSample();
  else if (theSampleName == "ProjectionOfPoint2dSample")
    ProjectionOfPoint2dSample();
  else if (theSampleName == "MinimalDistance2dSample")
    MinimalDistance2dSample();
  else if (theSampleName == "Intersection2dSample")
    Intersection2dSample();
  else if (theSampleName == "PointInfo3dSample")
    PointInfo3dSample();
  else if (theSampleName == "EllipseInfo3dSample")
    EllipseInfo3dSample();
  else if (theSampleName == "PointInfo2dSample")
    PointInfo2dSample();
  else if (theSampleName == "CircleInfo2dSample")
    CircleInfo2dSample();
  else if (theSampleName == "SecondOrderCurves3dSample")
    SecondOrderCurves3dSample();
  else if (theSampleName == "FreeStyleCurves3dSample")
    FreeStyleCurves3dSample();
  else if (theSampleName == "AnalyticalSurfaces3dSample")
    AnalyticalSurfaces3dSample();
  else if (theSampleName == "FreeStyleSurfaces3dSample")
    FreeStyleSurfaces3dSample();
  else if (theSampleName == "SecondOrderCurves2dSample")
    SecondOrderCurves2dSample();
  else if (theSampleName == "FreeStyleCurves2dSample")
    FreeStyleCurves2dSample();
  else if (theSampleName == "TrimmedCurve3dSample")
    TrimmedCurve3dSample();
  else if (theSampleName == "OffsetCurve3dSample")
    OffsetCurve3dSample();
  else if (theSampleName == "BSplineFromCircle3dSample")
    BSplineFromCircle3dSample();
  else if (theSampleName == "TrimmedSurface3dSample")
    TrimmedSurface3dSample();
  else if (theSampleName == "OffsetSurface3dSample")
    OffsetSurface3dSample();
  else if (theSampleName == "ExtrusionSurface3dSample")
    ExtrusionSurface3dSample();
  else if (theSampleName == "RevolutionSurface3dSample")
    RevolutionSurface3dSample();
  else if (theSampleName == "TrimmedCurve2dSample")
    TrimmedCurve2dSample();
  else if (theSampleName == "OffsetCurve2dSample")
    OffsetCurve2dSample();
  else if (theSampleName == "BoundingBoxOfSurface3dSample")
    BoundingBoxOfSurface3dSample();
  else if (theSampleName == "BoundingBoxOfCurves3dSample")
    BoundingBoxOfCurves3dSample();
  else if (theSampleName == "BoundingBoxOfCurves2dSample")
    BoundingBoxOfCurves2dSample();
  else if (theSampleName == "DumpCircleInfoSample")
    DumpCircleInfoSample();
  else if (theSampleName == "DumpBSplineCurveInfoSample")
    DumpBSplineCurveInfoSample();
  else
  {
    myResult << "No function found: " << theSampleName;
    myCode += TCollection_AsciiString("No function found: ") + theSampleName;
    anIsSamplePresent = Standard_False;
  }
  myIsProcessed = anIsSamplePresent;
}

void GeometrySamples::DisplayPnt (const gp_Pnt2d& thePnt2d, const TCollection_AsciiString& theText,
                                  Aspect_TypeOfMarker theMarker, Standard_Real theDistance)
{
  gp_Pnt aPnt(thePnt2d.X(), thePnt2d.Y(), 0.0);
  Handle(Geom_CartesianPoint) aGeomPoint = new Geom_CartesianPoint(aPnt);
  Handle(AIS_Point) anAisPoint = new AIS_Point(aGeomPoint);
  anAisPoint->SetMarker(theMarker);
  myObject2d.Append(anAisPoint);
  Handle(AIS_TextLabel) aPntLabel = new AIS_TextLabel();
  aPntLabel->SetText(theText);
  aPntLabel->SetPosition(gp_Pnt(aPnt.X(), aPnt.Y() + theDistance, aPnt.Z()));
  myObject2d.Append(aPntLabel);
}
void GeometrySamples::DisplayPnt (const gp_Pnt& thePnt, const TCollection_AsciiString& theText,
                                  Aspect_TypeOfMarker theMarker, Standard_Real theDistance)
{
  Handle(Geom_CartesianPoint) aPoint = new Geom_CartesianPoint(thePnt);
  Handle(AIS_Point) anAisPoint = new AIS_Point(aPoint);
  anAisPoint->SetMarker(theMarker);
  myObject3d.Append(anAisPoint);
  Handle(AIS_TextLabel) aPntLabel = new AIS_TextLabel();
  aPntLabel->SetText(theText);
  aPntLabel->SetPosition(gp_Pnt(thePnt.X(), thePnt.Y(), thePnt.Z() + theDistance));
  myObject3d.Append(aPntLabel);
}

void GeometrySamples::ZeroDimensionObjects3dSample()
{
  // gp_Pnt describes a point in 3D space. A Geom_CartesianPoint is defined by
  // a gp_Pnt point, with its three Cartesian coordinates X, Y and Z.
  gp_Pnt aCoordPnt(10.0, 20.0, 30.0);
  Handle(Geom_CartesianPoint) aCoordGeomPoint = new Geom_CartesianPoint(aCoordPnt);
  Handle(AIS_Point) aCoordAisPoint = new AIS_Point(aCoordGeomPoint);
  myObject3d.Append(aCoordAisPoint);

  Handle(AIS_TextLabel) aPntLabel = new AIS_TextLabel();
  aPntLabel->SetText("  gp_Pnt");
  aPntLabel->SetPosition(gp_Pnt(aCoordPnt.X(), aCoordPnt.Y(), aCoordPnt.Z() + 5.0));
  myObject3d.Append(aPntLabel);
  myResult << "gp_Pnt was created" << std::endl;

  // gp_XYZ class describes a Cartesian coordinate entity in 3D space (X,Y,Z).
  // This entity is used for algebraic calculation.
  // This entity can be transformed with a "Trsf" or a "GTrsf" from package "gp".
  // It is used in vectorial computations or for holding this type of information
  // in data structures.
  gp_XYZ aXyz1(10.0, 20.0, 30.0);
  gp_XYZ aXyz2(20.0, 10.0, 30.0);
  gp_XYZ aXyzSum = aXyz1 + aXyz2;
  gp_Pnt aSumPnt(aXyzSum);
  Handle(Geom_CartesianPoint) aXyzGeomPoint = new Geom_CartesianPoint(aSumPnt);
  Handle(AIS_Point) aSumAisPoint = new AIS_Point(aXyzGeomPoint);
  myObject3d.Append(aSumAisPoint);

  Handle(AIS_TextLabel) aXyzLabel = new AIS_TextLabel();
  aXyzLabel->SetText("  gp_XYZ");
  aXyzLabel->SetPosition(gp_Pnt(aXyzSum.X(), aXyzSum.Y(), aXyzSum.Z() + 5.0));
  myObject3d.Append(aXyzLabel);
  myResult << "gp_XYZ was created" << std::endl;
}

void GeometrySamples::Vectors3dSample()
{
  gp_Pnt aPnt1(0.0, 0.0, 0.0);
  gp_Pnt aPnt2(5.0, 0.0, 0.0);

  // gp_Vec defines a non-persistent vector in 3D space.
  gp_Vec aVec(aPnt1, aPnt2);
  Handle(AdaptorVec_AIS) aVecAIS = new AdaptorVec_AIS(aPnt1, aVec, 0.5);
  aVecAIS->SetText("  gp_Vec");
  myObject3d.Append(aVecAIS);
  myResult << "gp_Vec magnitude: " << aVec.Magnitude() << std::endl;

  // Describes a unit vector in 3D space.
  // This unit vector is also called "Direction".
  // See Also gce_MakeDir which provides functions for more complex unit vector
  // constructions Geom_Direction which provides additional functions
  // for constructing unit vectors and works, in particular,
  // with the parametric equations of unit vectors.
  gp_Dir aDir(aVec);
  Handle(AdaptorVec_AIS) aDirAIS = new AdaptorVec_AIS(gp_Pnt(0.0, 0.0, 10.0), aDir, 1.0, 0.5);
  aDirAIS->SetText("  gp_Dir");
  myObject3d.Append(aDirAIS);
  myResult << "gp_Dir coordinates: X: " << aDir.X() << ", Y: " << aDir.Y() << ", Z: " << aDir.Z() << std::endl;
}

void GeometrySamples::InfinityLines3dSample()
{
  gp_Pnt aBasePoint(0.0, 0.0, 0.0);
  gp_Dir aX_Direction(1.0, 0.0, 0.0);
  gp_Dir anY_Direction(0.0, 1.0, 0.0);
  gp_Dir aZ_Direction(0.0, 0.0, 1.0);

  // Describes an axis in 3D space.
  gp_Ax1 anAxis1(aBasePoint, aZ_Direction);
  Handle(AdaptorVec_AIS) anAx1Ais = new AdaptorVec_AIS(anAxis1.Location(), anAxis1.Direction(), 1.0, 0.3);
  anAx1Ais->SetText("  gp_Ax1");
  myObject3d.Append(anAx1Ais);
  myResult << "gp_Ax1 was created" << std::endl << std::endl;

  //  Describes a right - handed coordinate system in 3D space.
  aBasePoint.SetCoord(0.0, 0.0, 3.0);
  gp_Ax2 anAxis2(aBasePoint, aZ_Direction);
  Handle(AdaptorVec_AIS) aAx2AisZ = new AdaptorVec_AIS(anAxis2.Location(), anAxis2.Direction(), 1.0, 0.3);
  aAx2AisZ->SetText("  gp_Ax2 Z");
  myObject3d.Append(aAx2AisZ);
  Handle(AdaptorVec_AIS) aAx2AisX = new AdaptorVec_AIS(anAxis2.Location(), anAxis2.XDirection(), 1.0, 0.3);
  aAx2AisX->SetText("  gp_Ax2 X");
  myObject3d.Append(aAx2AisX);
  Handle(AdaptorVec_AIS) aAx2AisY = new AdaptorVec_AIS(anAxis2.Location(), anAxis2.YDirection(), 1.0, 0.3);
  aAx2AisY->SetText("  gp_Ax2 Y");
  myObject3d.Append(aAx2AisY);
  myResult << "gp_Ax2 was created" << std::endl;

  // Describes a coordinate system in 3D space.Unlike a gp_Ax2 coordinate system,
  // a gp_Ax3 can be right - handed("direct sense") or left - handed("indirect sense").
  gp_Ax3 anAxis3(gp_XYZ(0.0, 0.0, 6.0), aZ_Direction, aX_Direction);
  anAxis3.YReverse();
  Handle(AdaptorVec_AIS) anAx3AisZ = new AdaptorVec_AIS(anAxis3.Location(), anAxis3.Direction(), 1.0, 0.3);
  anAx3AisZ->SetText("  gp_Ax3 Z");
  myObject3d.Append(anAx3AisZ);
  Handle(AdaptorVec_AIS) anAx3AisX = new AdaptorVec_AIS(anAxis3.Location(), anAxis3.XDirection(), 1.0, 0.3);
  anAx3AisX->SetText("  gp_Ax3 X");
  myObject3d.Append(anAx3AisX);
  Handle(AdaptorVec_AIS) anAx3AisY = new AdaptorVec_AIS(anAxis3.Location(), anAxis3.YDirection(), 1.0, 0.3);
  anAx3AisY->SetText("  gp_Ax3 Y");
  myObject3d.Append(anAx3AisY);

  myResult << "gp_Ax3 was created" << std::endl;
  const gp_Dir& anAxis3_xDir = anAxis3.XDirection();
  const gp_Dir& anAxis3_yDir = anAxis3.YDirection();
  myResult << "gp_Ax3 X direction: " << anAxis3_xDir.X() << " " << anAxis3_xDir.Y() << " " << anAxis3_xDir.Z() << std::endl;
  myResult << "gp_Ax3 Y direction: " << anAxis3_yDir.X() << " " << anAxis3_yDir.Y() << " " << anAxis3_yDir.Z() << std::endl;
  TCollection_AsciiString aDirectionDescription;
  if (anAxis3.Direct())
  {
    aDirectionDescription = "anAxis3 is a right-handed axis system";
  }
  else
  {
    aDirectionDescription = "anAxis3 is a left-handed axis system";
  }
  myResult << aDirectionDescription << std::endl << std::endl;

  // Describes a line in 3D space. A line is positioned in space with an axis
  // (a gp_Ax1 object) which gives it an origin and a unit vector.
  gp_Lin aLine(gp_Pnt(5.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0));
  Handle(AdaptorVec_AIS) anLineAis = new AdaptorVec_AIS(aLine.Location(), aLine.Direction(), 8.0);
  anLineAis->SetText("  gp_Lin");
  myObject3d.Append(anLineAis);
  myResult << "gp_Lin was created" << std::endl << std::endl;
}

void GeometrySamples::SecondOrderCurves3dSample()
{
  gp_Ax2 anAxis2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Circ aCirc(anAxis2, 10.0);
  Handle(Geom_Circle) aGeomCircle = new Geom_Circle(aCirc);
  Handle(AIS_Circle) anAisCircle = new AIS_Circle(aGeomCircle);
  myObject3d.Append(anAisCircle);

  // Describes an ellipse in 3D space. An ellipse is defined by its major and minor
  // radii and positioned in space with a coordinate system (a gp_Ax2 object)
  gp_Elips anElips(anAxis2.Translated(gp_Vec(0.0, 0.0, 10.0)), 20.0, 10.0);
  Handle(Geom_Ellipse) aGeomEllipse = new Geom_Ellipse(anElips);
  Handle(AdaptorCurve_AIS) anAisEllipce = new AdaptorCurve_AIS(aGeomEllipse);
  myObject3d.Append(anAisEllipce);

  // Describes a parabola in 3D space. A parabola is defined by its focal length
  // (that is, the distance between its focus and apex) and positioned in space with
  // a coordinate system (a gp_Ax2 object)
  gp_Parab aParab(anAxis2.Translated(gp_Vec(0.0, 0.0, 20.0)), 2.0);
  Handle(Geom_Parabola) aGeomParabola = new Geom_Parabola(aParab);
  Handle(Geom_TrimmedCurve) aTrimmedParabola = new Geom_TrimmedCurve(aGeomParabola, 20.0, -20.0);
  Handle(AdaptorCurve_AIS) anAisParabola = new AdaptorCurve_AIS(aTrimmedParabola);
  myObject3d.Append(anAisParabola);

  // Describes a branch of a hyperbola in 3D space. A hyperbola is defined by its major
  // and minor radii and positioned in space with a coordinate system (a gp_Ax2 object)
  gp_Hypr aHypr(anAxis2.Translated(gp_Vec(0.0, 0.0, 30.0)), 20.0, 10.0);
  Handle(Geom_Hyperbola) aGeomHyperbola = new Geom_Hyperbola(aHypr);
  Handle(Geom_TrimmedCurve) aTrimmedHyperbola = new Geom_TrimmedCurve(aGeomHyperbola, 2.0, -2.0);
  Handle(AdaptorCurve_AIS) anAisHyperbola = new AdaptorCurve_AIS(aTrimmedHyperbola);
  myObject3d.Append(anAisHyperbola);
}

void GeometrySamples::PlaneSurfaces3dSample()
{
  // Describes a plane.A plane is positioned in space with a coordinate system(a gp_Ax3 object),
  // such that the plane is defined by the origin, "X Direction" and "Y Direction" of this
  // coordinate system, which is the "local coordinate system" of the plane.The "main Direction"
  // of the coordinate system is a vector normal to the plane.
  gp_Pln aPln(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Plane) aPlane = new Geom_Plane(aPln);
  Handle(AIS_Plane) anAisPlane = new AIS_Plane(aPlane, aPln.Location(),
    gp_Pnt(10.0, 10.0, 0.0),
    gp_Pnt(-10.0, -10.0, 0.0),
    Standard_False);
  myObject3d.Append(anAisPlane);

  // Describes an infinite cylindrical surface.A cylinder is defined by its radius and positioned
  // in space with a coordinate system(a gp_Ax3 object), the "main Axis" of which is the axis of
  // the cylinder.This coordinate system is the "local coordinate system" of the cylinder.
  gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0.0, 0.0, 10.0), gp_Dir(0.0, 0.0, 1.0)), 10.0);
  Handle(Geom_CylindricalSurface) aCylindricalSurface = new Geom_CylindricalSurface(aCylinder);
  Handle(AIS_Shape) anAisCylinder = new AIS_Shape(BRepBuilderAPI_MakeFace(
    aCylindricalSurface, 0.0, 2.0*M_PI, 0.0, 10.0, Precision::Confusion()).Shape());
  myObject3d.Append(anAisCylinder);

  // Defines an infinite conical surface. A cone is defined by its half-angle (can be negative) at
  // the apex and positioned in space with a coordinate system (a gp_Ax3 object) and a "reference radius"
  gp_Cone aCone(gp_Ax3(gp_Pnt(0.0, 0.0, 30.0), gp_Dir(0.0, 0.0, 1.0)), 0.25*M_PI, 0.0);
  Handle(Geom_ConicalSurface) aConicalSurface = new Geom_ConicalSurface(aCone);
  Handle(AIS_Shape) anAisCone = new AIS_Shape(BRepBuilderAPI_MakeFace(
    aConicalSurface, 0.0, 2.0*M_PI, 0.0, 20.0, Precision::Confusion()).Shape());
  myObject3d.Append(anAisCone);
}

void GeometrySamples::SecondOrderSurfaces3dSample()
{
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 10.0);
  Handle(Geom_SphericalSurface) aSphericalSurface = new Geom_SphericalSurface(aSphere);
  Handle(AIS_Shape) anAisSphere = new AIS_Shape(BRepBuilderAPI_MakeFace(
    aSphericalSurface, 0.0, 2.0*M_PI, 0.0, 2.0*M_PI, Precision::Confusion()).Shape());
  myObject3d.Append(anAisSphere);

  gp_Torus aTorus(gp_Ax3(gp_Pnt(0.0, 0.0, 20.0), gp_Dir(0.0, 0.0, 1.0)), 40.0, 10.0);
  Handle(Geom_ToroidalSurface) aToroidalSurface = new Geom_ToroidalSurface(aTorus);
  Handle(AIS_Shape) anAisTorus = new AIS_Shape(BRepBuilderAPI_MakeFace(
    aToroidalSurface, 0.0, 2.0*M_PI, 0.0, 2.0*M_PI, Precision::Confusion()).Shape());
  myObject3d.Append(anAisTorus);
}

void GeometrySamples::ZeroDimensionObjects2dSample()
{
  // Defines a non-persistent 2D Cartesian point.
  gp_Pnt2d aCoordPnt(10.0, 20.0);
  Handle(Geom2d_CartesianPoint) aCoordGeomPoint = new Geom2d_CartesianPoint(aCoordPnt);
  Handle(AdaptorPnt2d_AIS) aCoordAisPoint = new AdaptorPnt2d_AIS(aCoordGeomPoint);
  myObject2d.Append(aCoordAisPoint);

  Handle(AIS_TextLabel) aPntLabel = new AIS_TextLabel();
  aPntLabel->SetText("gp_Pnt2d");
  aPntLabel->SetPosition(gp_Pnt(aCoordPnt.X(), aCoordPnt.Y() + 0.5, 0.0));
  myObject2d.Append(aPntLabel);
  myResult << "gp_Pnt was created" << std::endl;

  // This class describes a Cartesian coordinate entity in 2D space{ X,Y }.
  // This class is non persistent.This entity used for algebraic calculation.
  // An XY can be transformed with a Trsf2d or a GTrsf2d from package gp.
  // It is used in vectorial computations or for holding this type of information in data structures.
  gp_XY aXy(20.0, 10.0);
  Handle(Geom2d_CartesianPoint) aXyGeomPoint = new Geom2d_CartesianPoint(aXy);
  Handle(AdaptorPnt2d_AIS) aXyAisPoint = new AdaptorPnt2d_AIS(aXyGeomPoint);
  myObject2d.Append(aXyAisPoint);

  Handle(AIS_TextLabel) aXyLabel = new AIS_TextLabel();
  aXyLabel->SetText("  gp_XY");
  aXyLabel->SetPosition(gp_Pnt(aXy.X(), aXy.Y() + 0.5, 0.0));
  myObject2d.Append(aXyLabel);
  myResult << "gp_XY was created" << std::endl;
}

void GeometrySamples::Vectors2dSample()
{
  // Describes a unit vector in the plane (2D space).
  // This unit vector is also called "Direction".
  gp_Dir2d aDir(3.0, 4.0);
  Handle(AdaptorVec_AIS) anAisDir = new AdaptorVec_AIS(gp_Pnt2d(0.0, 0.0), aDir, 1.0, 0.3);
  anAisDir->SetText("  gp_Dir2d");
  myObject2d.Append(anAisDir);
  myResult << "gp_Dir2d coordinates: X: " << aDir.X() << ", Y: " << aDir.Y() << std::endl;

  // Defines a non-persistent vector in 2D space.
  gp_Vec2d aVec(aDir);
  aVec = aVec * 2;
  Handle(AdaptorVec_AIS) anAisVec = new AdaptorVec_AIS(gp_Pnt2d(0.0, 5.0), aVec, 0.3);
  anAisVec->SetText("  gp_Vec2d");
  myObject2d.Append(anAisVec);
  myResult << "gp_Vec2d magnitude: " << aVec.Magnitude() << std::endl;
}

void GeometrySamples::InfinityLines2dSample()
{
  // Describes an axis in the plane (2D space)
  gp_Ax2d anAx2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  Handle(AdaptorVec_AIS) anAisAx2d = new AdaptorVec_AIS(anAx2d.Location(), anAx2d.Direction(), 1.0, 0.3);
  anAisAx2d->SetText("  gp_Ax2d");
  myObject2d.Append(anAisAx2d);

  // Describes a coordinate system in a plane (2D space).
  gp_Ax22d anAx22d(gp_Pnt2d(0.0, 2.0), gp_Dir2d(1.0, 1.0), Standard_False);
  Handle(AdaptorVec_AIS) anAisAx2d_X = new AdaptorVec_AIS(anAx22d.Location(), anAx22d.XDirection(), 1.0, 0.3);
  anAisAx2d_X->SetText("  gp_Ax2d X");
  myObject2d.Append(anAisAx2d_X);
  Handle(AdaptorVec_AIS) anAisAx2d_Y = new AdaptorVec_AIS(anAx22d.Location(), anAx22d.YDirection(), 1.0, 0.3);
  anAisAx2d_Y->SetText("  gp_Ax2d Y");
  myObject2d.Append(anAisAx2d_Y);

  // Describes a line in 2D space. A line is positioned in the plane with an axis (a gp_Ax2d object) which gives
  // the line its origin and unit vector. A line and an axis are similar objects, thus,
  // we can convert one into the other. A line provides direct access to the majority of the edit and query
  // functions available on its positioning axis.
  gp_Lin2d aLin2d(gp_Pnt2d(2.0, 4.0), gp_Dir2d(0.0, -1.0));
  Handle(AdaptorVec_AIS) anAisLin = new AdaptorVec_AIS(aLin2d.Location(), aLin2d.Direction(), 1.0, 0.3);
  anAisLin->SetText("  gp_Lin2d");
  myObject2d.Append(anAisLin);
}

void GeometrySamples::SecondOrderCurves2dSample()
{
  // Describes a circle in the plane (2D space). A circle is defined by its radius
  // and positioned in the plane with a coordinate system (a gp_Ax22d object)
  gp_Circ2d aCirc2d;
  aCirc2d.SetLocation(gp_Pnt2d(0.0, 0.0));
  aCirc2d.SetRadius(10.0);
  Handle(Geom2d_Circle) aGeomCircle = new Geom2d_Circle(aCirc2d);
  Handle(AdaptorCurve2d_AIS) anAisCirc = new AdaptorCurve2d_AIS(aGeomCircle, Aspect_TOL_SOLID);
  myObject2d.Append(anAisCirc);

  // Describes an ellipse in the plane (2D space). An ellipse is defined by its major
  // and minor radii and positioned in the plane with a coordinate system (a gp_Ax22d object)
  gp_Elips2d anElips(gp_Ax2d(gp_Pnt2d(0.0, 30.0), gp_Dir2d(1.0, 0.0)), 20.0, 10.0);
  Handle(Geom2d_Ellipse) aGeomEllipse = new Geom2d_Ellipse(anElips);
  Handle(AdaptorCurve2d_AIS) anAisEllipse = new AdaptorCurve2d_AIS(aGeomEllipse, Aspect_TOL_DASH);
  myObject2d.Append(anAisEllipse);

  // Describes a parabola in the plane (2D space). A parabola is defined by its focal length
  // (that is, the distance between its focus and apex) and positioned in the plane with
  // a coordinate system (a gp_Ax22d object)
  gp_Parab2d aParab2d(gp_Ax2d(gp_Pnt2d(20.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0);
  Handle(Geom2d_Parabola) aGeomParabola = new Geom2d_Parabola(aParab2d);
  Handle(Geom2d_TrimmedCurve) aTrimmedParabola = new Geom2d_TrimmedCurve(aGeomParabola, 40.0, -40.0);
  Handle(AdaptorCurve2d_AIS) anAisParabola = new AdaptorCurve2d_AIS(aTrimmedParabola, Aspect_TOL_DOT);
  myObject2d.Append(anAisParabola);

  // Describes a branch of a hyperbola in the plane (2D space). A hyperbola is defined by its major and
  // minor radii, and positioned in the plane with a coordinate system (a gp_Ax22d object)
  gp_Hypr2d aHypr2d(gp_Ax2d(gp_Pnt2d(20.0, 0.0), gp_Dir2d(1.0, 0.0)), 20.0, 10.0);
  Handle(Geom2d_Hyperbola) aGeomHyperbola = new Geom2d_Hyperbola(aHypr2d);
  Handle(Geom2d_TrimmedCurve) aTrimmedHyperbola = new Geom2d_TrimmedCurve(aGeomHyperbola, 2.0, -2.0);
  Handle(AdaptorCurve2d_AIS) anAisHyperbola = new AdaptorCurve2d_AIS(aTrimmedHyperbola, Aspect_TOL_DOTDASH);
  myObject2d.Append(anAisHyperbola);
}

void GeometrySamples::BarycenterPoint3dSample()
{
  // Barycenter of 2 points
  gp_Pnt aPnt1(11, 2, 3);
  gp_Pnt aPnt2(13, 4, 5);
  gp_Pnt aBarycenterPnt2 = aPnt1;
  Standard_Real anAlpha = 3;
  Standard_Real anBeta = 7;
  // Assigns the result of the following expression to this point:
  // (Alpha*this + Beta*P) / (Alpha + Beta)
  aBarycenterPnt2.BaryCenter(anAlpha, aPnt2, anBeta);
  DisplayPnt(aPnt1, "Pnt1", Aspect_TOM_PLUS, 0.5);
  DisplayPnt(aPnt2, "Pnt2", Aspect_TOM_PLUS, 0.5);
  DisplayPnt(aBarycenterPnt2, "Barycenter Pnt", Aspect_TOM_O_PLUS, 0.5);

  //  BaryCenter of an array of point
  gp_Pnt aP1(0, 0, 5);
  gp_Pnt aP2(1, 2, 3);
  gp_Pnt aP3(2, 3, -2);
  gp_Pnt aP4(4, 3, 5);
  gp_Pnt aP5(5, 5, 4);
  TColgp_Array1OfPnt aPntArray(1, 5);
  aPntArray.SetValue(1, aP1);
  aPntArray.SetValue(2, aP2);
  aPntArray.SetValue(3, aP3);
  aPntArray.SetValue(4, aP4);
  aPntArray.SetValue(5, aP5);

  Standard_Real Tolerance = 8;
  GProp_PEquation aPEquation(aPntArray, Tolerance);

  gp_Pnt aBarycenterPnt5; // P declaration
  bool isPoint = false;
  if (aPEquation.IsPoint())
  {
    isPoint = true;
    aBarycenterPnt5 = aPEquation.Point();
    myResult << "GProp_PEquation is a point" << std::endl;
  }
  else
  {
    isPoint = false;
    myResult << "GProp_PEquation is not a point" << std::endl;
  }

  if (aPEquation.IsLinear())
  {
    /*... */
  }
  if (aPEquation.IsPlanar())
  {
    /*... */
  }
  if (aPEquation.IsSpace())
  {
    /*... */
  }

  const TCollection_AsciiString aPointName("P");
  for (Standard_Integer i = aPntArray.Lower(); i <= aPntArray.Upper(); i++)
  {
    TCollection_AsciiString aString(i);
    aString = aPointName + aString;
    DisplayPnt(aPntArray(i), aString, Aspect_TOM_STAR, 0.5);
  }

  DisplayPnt(aBarycenterPnt5, "Barycenter of 5 points", Aspect_TOM_O_STAR, 0.5);
  myResult << " IsPoint = ";
  if (isPoint)
  {
    myResult << "True   -->  " << " P ( " << aBarycenterPnt5.X() << aBarycenterPnt5.Y() << aBarycenterPnt5.Z() << " );" << std::endl;
  }
  else
  {
    myResult << "False";
  }
  myResult << std::endl << " IsLinear = " << (aPEquation.IsLinear() ? "True" : "False");
  myResult << std::endl << " IsPlanar = " << (aPEquation.IsPlanar() ? "True" : "False");
  myResult << std::endl << " IsSpace = "  << (aPEquation.IsSpace()  ? "True" : "False");
}

void GeometrySamples::RotatedVector3dSample()
{
  gp_Vec aBaseVec(0.0, 0.0, 10.0);
  gp_Pnt aZeroPnt(0.0, 0.0, 0.0);
  gp_Vec aRotatedVec = aBaseVec.Rotated(gp_Ax1(aZeroPnt, gp_Dir(1.0, 0.0, 0.0)), M_PI_4);

  Handle(AdaptorVec_AIS) aBaseVecAIS = new AdaptorVec_AIS(aZeroPnt, aBaseVec);
  aBaseVecAIS->SetText("  Base vector");
  myObject3d.Append(aBaseVecAIS);
  Handle(AdaptorVec_AIS) aRotatedVecAIS = new AdaptorVec_AIS(aZeroPnt, aRotatedVec);
  aRotatedVecAIS->SetText("  Rotated vector");
  myObject3d.Append(aRotatedVecAIS);
  Standard_Real anAdgle = aBaseVec.Angle(aRotatedVec)*180.0 / M_PI;
  myResult << "An angle between vectors = " << anAdgle << std::endl;
}

void GeometrySamples::MirroredLine3dSample()
{
  gp_Lin aBaseLin(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 1.0, 1.0));
  gp_Ax2 aXyzAxis;
  gp_Lin aMirroredLin = aBaseLin.Mirrored(aXyzAxis);

  Handle(AdaptorVec_AIS) aBaseLineAis = new AdaptorVec_AIS(aBaseLin.Location(), aBaseLin.Direction(), 8.0);
  aBaseLineAis->SetText("  Base Line");
  myObject3d.Append(aBaseLineAis);
  Handle(AdaptorVec_AIS) aMirroredLineAis = new AdaptorVec_AIS(aMirroredLin.Location(), aMirroredLin.Direction(), 8.0);
  aMirroredLineAis->SetText("  Mirrored Line");
  myObject3d.Append(aMirroredLineAis);
  Handle(AIS_Plane) anAisPlane = new AIS_Plane (new Geom_Plane(gp_Ax3(aXyzAxis)), aXyzAxis.Location(),
                                                gp_Pnt(10.0, 10.0, 0.0), gp_Pnt(-10.0, -10.0, 0.0), Standard_False);
  myObject3d.Append(anAisPlane);
  Standard_Real anAdgle = aBaseLin.Angle(aMirroredLin)*180.0 / M_PI;
  myResult << "An angle between lines = " << anAdgle << std::endl;
}

void GeometrySamples::ScaledEllipse3dSample()
{
  gp_Ax2 anAxis2(gp_Pnt(), gp_Dir(0.0, 0.0, 1.0));
  gp_Elips anBaseElips(anAxis2, 20.0, 10.0);
  gp_Elips anScaledElips = anBaseElips.Scaled(gp_Pnt(), 2.5);

  Handle(Geom_Ellipse) aBaseGeomEllipse = new Geom_Ellipse(anBaseElips);
  Handle(AdaptorCurve_AIS) anAisBaseEllipce = new AdaptorCurve_AIS(aBaseGeomEllipse);
  myObject3d.Append(anAisBaseEllipce);

  Handle(Geom_Ellipse) aScaledGeomEllipse = new Geom_Ellipse(anScaledElips);
  Handle(AdaptorCurve_AIS) anAisScaledEllipce = new AdaptorCurve_AIS(aScaledGeomEllipse);
  myObject3d.Append(anAisScaledEllipce);
}

void GeometrySamples::TransformedCylinder3dSample()
{
  gp_Cylinder aBaseCylinder(gp_Ax3(), 10.0);
  gp_Trsf aRotTrsf;
  aRotTrsf.SetRotation(gp_Ax1(gp_Pnt(), gp_Dir(1.0, 0.0, 0.0)), M_PI_2);
  gp_Trsf aScaleTrsf;
  aScaleTrsf.SetScale(gp_Pnt(), 1.5);
  gp_Trsf aTranslTrsf;
  aTranslTrsf.SetTranslation(gp_Vec(30.0, 0.0, 0.0));
  gp_Trsf aComplexTrsf = aRotTrsf * aScaleTrsf * aTranslTrsf;
  gp_Cylinder aTransfCylinder = aBaseCylinder.Transformed(aComplexTrsf);

  Handle(Geom_CylindricalSurface) aBaseCylinderSurface = new Geom_CylindricalSurface(aBaseCylinder);
  Handle(AIS_Shape) anAisBaseCylinder = new AIS_Shape(BRepBuilderAPI_MakeFace(
    aBaseCylinderSurface, 0.0, 2.0*M_PI, 0.0, 2.0*M_PI, Precision::Confusion()).Shape());
  myObject3d.Append(anAisBaseCylinder);
  Handle(Geom_CylindricalSurface) aTransfCylinderSurface = new Geom_CylindricalSurface(aTransfCylinder);
  Handle(AIS_Shape) anAisTransfCylinder = new AIS_Shape(BRepBuilderAPI_MakeFace(
    aTransfCylinderSurface, 0.0, 2.0*M_PI, 0.0, 2.0*M_PI, Precision::Confusion()).Shape());
  myObject3d.Append(anAisTransfCylinder);
}

void GeometrySamples::TranslatedTorus3dSample()
{
  gp_Torus aBaseTorus(gp_Ax3(gp_Pnt(), gp_Dir(0.0, 0.0, 1.0)), 40.0, 10.0);
  gp_Torus aTranslatedTorus = aBaseTorus.Translated(gp_Vec(70.0, 70.0, 70.0));

  Handle(Geom_ToroidalSurface) aBaseSurface = new Geom_ToroidalSurface(aBaseTorus);
  Handle(AIS_Shape) anAisBaseShape = new AIS_Shape(BRepBuilderAPI_MakeFace(
    aBaseSurface, 0.0, 2.0*M_PI, 0.0, 2.0*M_PI, Precision::Confusion()).Shape());
  myObject3d.Append(anAisBaseShape);
  Handle(Geom_ToroidalSurface) aTranslSurface = new Geom_ToroidalSurface(aTranslatedTorus);
  Handle(AIS_Shape) anAisTranslShape = new AIS_Shape(BRepBuilderAPI_MakeFace(
    aTranslSurface, 0.0, 2.0*M_PI, 0.0, 2.0*M_PI, Precision::Confusion()).Shape());
  myObject3d.Append(anAisTranslShape);
}

void GeometrySamples::ConjugateObjects3dSample()
{
  gp_Hypr aHypr(gp_Ax2(), 20.0, 10.0);
  gp_Ax1 anAsymptote1 = aHypr.Asymptote1();
  gp_Ax1 anAsymptote2 = aHypr.Asymptote2();
  gp_Ax1 aDirectrix1 = aHypr.Directrix1();
  gp_Ax1 aDirectrix2 = aHypr.Directrix2();
  gp_Pnt aFocus1 = aHypr.Focus1();
  gp_Pnt aFocus2 = aHypr.Focus2();
  gp_Pnt aLocation = aHypr.Location();

  Handle(AdaptorVec_AIS) anAsy1AIS = new AdaptorVec_AIS(anAsymptote1.Location(), gp_Vec(anAsymptote1.Direction())*10.0);
  anAsy1AIS->SetText("  Asymptote 1");
  myObject3d.Append(anAsy1AIS);
  Handle(AdaptorVec_AIS) anAsy2AIS = new AdaptorVec_AIS(anAsymptote2.Location(), gp_Vec(anAsymptote2.Direction())*10.0);
  anAsy2AIS->SetText("  Asymptote 2");
  myObject3d.Append(anAsy2AIS);
  Handle(AdaptorVec_AIS) anDir1AIS = new AdaptorVec_AIS(aDirectrix1.Location(), gp_Vec(aDirectrix1.Direction())*10.0);
  anDir1AIS->SetText("  Directrix 1");
  myObject3d.Append(anDir1AIS);
  Handle(AdaptorVec_AIS) anDir2AIS = new AdaptorVec_AIS(aDirectrix2.Location(), gp_Vec(aDirectrix2.Direction())*10.0);
  anDir2AIS->SetText("  Directrix 2");
  myObject3d.Append(anDir2AIS);

  DisplayPnt(aFocus1, "Focus 1", Aspect_TOM_PLUS, 2.0);
  DisplayPnt(aFocus2, "Focus 2", Aspect_TOM_PLUS, 2.0);
  DisplayPnt(aLocation, "Location", Aspect_TOM_O_STAR, 2.0);

  Handle(Geom_Hyperbola) aGeomHyperbola = new Geom_Hyperbola(aHypr);
  Handle(Geom_TrimmedCurve) aTrimmedHyperbola = new Geom_TrimmedCurve(aGeomHyperbola, 2.0, -2.0);
  Handle(AdaptorCurve_AIS) anAisHyperbola = new AdaptorCurve_AIS(aTrimmedHyperbola);
  myObject3d.Append(anAisHyperbola);
}

void GeometrySamples::ProjectionOfPoint3dSample()
{
  gp_Sphere aSphere(gp_Ax3(), 10.0);
  gp_Pnt aBasePnt(20.0, 20.0, 20.0);
  // A projection point in surface coordinate
  gp_Pnt2d aPrjPnt2d = ProjLib::Project(aSphere, aBasePnt);
  gp_Pnt aPrjPnt = ElSLib::Value(aPrjPnt2d.X(), aPrjPnt2d.Y(), aSphere);

  DisplayPnt(aBasePnt, "Base point", Aspect_TOM_PLUS, 2.0);
  DisplayPnt(aPrjPnt, "Projection point", Aspect_TOM_O_STAR, 2.0);
  Handle(Geom_SphericalSurface) aSphericalSurface = new Geom_SphericalSurface(aSphere);
  Handle(AIS_Shape) anAisSphere = new AIS_Shape(BRepBuilderAPI_MakeFace(
    aSphericalSurface, 0.0, 2.0*M_PI, 0.0, 2.0*M_PI, Precision::Confusion()).Shape());
  myObject3d.Append(anAisSphere);
}

void GeometrySamples::MinimalDistance3dSample()
{
  gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(), gp_Dir(0.0, 0.0, 1.0)), 10.0);
  gp_Lin aLine(gp_Pnt(20.0, 0.0, 5.0), gp_Dir(0.0, 1.0, 0.0));
  Extrema_ExtElCS anExtrema_ExtElCS(aLine, aCylinder);

  if (anExtrema_ExtElCS.IsDone())
  {
    NCollection_Array1<gp_Vec> aVecArray(1, anExtrema_ExtElCS.NbExt());
    NCollection_Array1<gp_Pnt> aPntArray(1, anExtrema_ExtElCS.NbExt());
    for (Standard_Integer i = 1; i <= anExtrema_ExtElCS.NbExt(); i++)
    {
      Extrema_POnCurv aCurvPoint;
      Extrema_POnSurf aSurfPoint;
      anExtrema_ExtElCS.Points(i, aCurvPoint, aSurfPoint);
      gp_Pnt aCurvPnt = aCurvPoint.Value();
      gp_Pnt aSurfPnt = aSurfPoint.Value();

      DisplayPnt(aCurvPnt, TCollection_AsciiString(i), Aspect_TOM_O_PLUS, 2.0);
      DisplayPnt(aSurfPnt, TCollection_AsciiString(i), Aspect_TOM_O_STAR, 2.0);
      gp_Vec aVec(aCurvPnt, aSurfPnt);
      aVecArray.SetValue(i, aVec);
      aPntArray.SetValue(i, aCurvPnt);
    }
    Standard_Integer aMinDistIndex(0);
    Standard_Real aMinDistance = std::numeric_limits<Standard_Real>::max();
    for (Standard_Integer i = 1; i <= anExtrema_ExtElCS.NbExt(); i++)
    {
      if (aMinDistance > aVecArray(i).Magnitude())
      {
        aMinDistIndex = i;
        aMinDistance = aVecArray(i).Magnitude();
      }
    }
    Handle(AdaptorVec_AIS) anMinDistanceAis =
      new AdaptorVec_AIS(aPntArray(aMinDistIndex), aVecArray(aMinDistIndex));
    anMinDistanceAis->SetText("  Min distance");
    myObject3d.Append(anMinDistanceAis);
  }
  Handle(Geom_CylindricalSurface) aCylindricalSurface = new Geom_CylindricalSurface(aCylinder);
  Handle(AIS_Shape) anAisCylinder = new AIS_Shape(BRepBuilderAPI_MakeFace(
    aCylindricalSurface, 0.0, 2.0*M_PI, 0.0, 10.0, Precision::Confusion()).Shape());
  myObject3d.Append(anAisCylinder);
  Handle(AdaptorVec_AIS) anLineAis = new AdaptorVec_AIS(aLine.Location(), aLine.Direction(), 8.0);
  anLineAis->SetText("  gp_Lin");
  myObject3d.Append(anLineAis);
}

void GeometrySamples::Intersection3dSample()
{
  gp_Lin aLine(gp_Pnt(0.0, 0.0, 10.0), gp_Dir(0.0, 1.0, 0.0));
  gp_Cone aCone(gp_Ax3(gp_Pnt(), gp_Dir(0.0, 0.0, 1.0)), 0.25*M_PI, 0.0);
  IntAna_Quadric anIntAna_Quadric(aCone);
  IntAna_IntConicQuad anIntAna_IntConicQuad(aLine, anIntAna_Quadric);
  if (anIntAna_IntConicQuad.IsDone())
  {
    for (int i = 1; i <= anIntAna_IntConicQuad.NbPoints(); i++)
    {
      const gp_Pnt& aIntersectionPnt = anIntAna_IntConicQuad.Point(i);
      DisplayPnt(aIntersectionPnt, TCollection_AsciiString(i));
    }
  }
  Handle(AdaptorVec_AIS) aLineVecAIS = new AdaptorVec_AIS(aLine.Location(), gp_Vec(aLine.Direction())*5.0);
  aLineVecAIS->SetText("  Base vector");
  myObject3d.Append(aLineVecAIS);
  Handle(Geom_ConicalSurface) aConicalSurface = new Geom_ConicalSurface(aCone);
  Handle(AIS_Shape) anAisCone = new AIS_Shape(BRepBuilderAPI_MakeFace(
    aConicalSurface, 0.0, 2.0*M_PI, 0.0, 20.0, Precision::Confusion()).Shape());
  myObject3d.Append(anAisCone);
}

void GeometrySamples::TranslatedPoint2dSample()
{
  gp_Pnt2d aPnt1;
  gp_Pnt2d aPnt2 = aPnt1.Translated(gp_Vec2d(10.0, 10.0));
  DisplayPnt(aPnt1, "1", Aspect_TOM_PLUS, 1.0);
  DisplayPnt(aPnt2, "2", Aspect_TOM_PLUS, 1.0);
  gp_Vec2d aTranslationVec(aPnt1, aPnt2);
  Handle(AdaptorVec_AIS) aVecAIS = new AdaptorVec_AIS(aPnt1, aTranslationVec);
  aVecAIS->SetText("   Translation");
  myObject2d.Append(aVecAIS);
}

void GeometrySamples::RotatedDirection2dSample()
{
  gp_Dir2d aBaseDir(1.0, 1.0);
  gp_Dir2d aRotatedDir = aBaseDir.Rotated(M_PI_4);

  myResult << "An angle between directions: " << aBaseDir.Angle(aRotatedDir)*180.0 / M_PI << " grad";
  Handle(AdaptorVec_AIS) aBaseAIS = new AdaptorVec_AIS(gp_Pnt2d(), aBaseDir, 5.0);
  aBaseAIS->SetText("  Base");
  myObject2d.Append(aBaseAIS);
  Handle(AdaptorVec_AIS) aRotatedAIS = new AdaptorVec_AIS(gp_Pnt2d(), aRotatedDir, 5.0);
  aRotatedAIS->SetText("  Rotated");
  myObject2d.Append(aRotatedAIS);
}

void GeometrySamples::MirroredAxis2dSample()
{
  gp_Ax22d aBaseAx(gp_Pnt2d(10.0, 0.0), gp_Dir2d(1.0, 0.0), Standard_True);
  gp_Ax22d aMirrorAx = aBaseAx.Mirrored(gp_Pnt2d());

  DisplayPnt(gp_Pnt2d(), "Mirror point", Aspect_TOM_PLUS, 1.0);
  Handle(AdaptorVec_AIS) aBaseX_AIS = new AdaptorVec_AIS(aBaseAx.Location(), aBaseAx.XDirection(), 5.0);
  aBaseX_AIS->SetText("  X (Base)");
  myObject2d.Append(aBaseX_AIS);
  Handle(AdaptorVec_AIS) aBaseY_AIS = new AdaptorVec_AIS(aBaseAx.Location(), aBaseAx.YDirection(), 5.0);
  aBaseY_AIS->SetText("Y (Base)");
  myObject2d.Append(aBaseY_AIS);
  Handle(AdaptorVec_AIS) aMirrorX_AIS = new AdaptorVec_AIS(aMirrorAx.Location(), aMirrorAx.XDirection(), 5.0);
  aMirrorX_AIS->SetText("X (Mirror)");
  myObject2d.Append(aMirrorX_AIS);
  Handle(AdaptorVec_AIS) aMirrorY_AIS = new AdaptorVec_AIS(aMirrorAx.Location(), aMirrorAx.YDirection(), 5.0);
  aMirrorY_AIS->SetText("  Y (Mirror)");
  myObject2d.Append(aMirrorY_AIS);
}

void GeometrySamples::TransformedEllipse2dSample()
{
  // Creates an ellipse with the major axis, the major and the minor radius.
  // The location of the MajorAxis is the center of the ellipse.The sense of
  // parametrization is given by Sense.Warnings : It is possible to create
  // an ellipse with MajorRadius = MinorRadius.Raises
  // ConstructionError if MajorRadius < MinorRadius or MinorRadius < 0.0.
  gp_Elips2d aBaseEllips(gp_Ax2d(gp_Pnt2d(), gp_Dir2d(1.0, 0.0)), 20.0, 10.0);
  gp_Trsf2d aRotTrsf;
  aRotTrsf.SetRotation(gp_Pnt2d(), M_PI_4);
  gp_Trsf2d aScaleTrsf;
  aScaleTrsf.SetScale(gp_Pnt2d(), 1.5);
  gp_Trsf2d aTranslTrsf;
  aTranslTrsf.SetTranslation(gp_Vec2d(30.0, 0.0));
  gp_Trsf2d aComplexTrsf = aRotTrsf * aScaleTrsf * aTranslTrsf;
  gp_Elips2d aTransfEllips = aBaseEllips.Transformed(aComplexTrsf);

  Handle(Geom2d_Ellipse) aBaseEllipse = new Geom2d_Ellipse(aBaseEllips);
  Handle(AdaptorCurve2d_AIS) anAisBaseEllipse = new AdaptorCurve2d_AIS(aBaseEllipse, Aspect_TOL_DASH);
  myObject2d.Append(anAisBaseEllipse);
  Handle(Geom2d_Ellipse) aTransfEllipse = new Geom2d_Ellipse(aTransfEllips);
  Handle(AdaptorCurve2d_AIS) anAisTransfEllipse = new AdaptorCurve2d_AIS(aTransfEllipse, Aspect_TOL_DASH);
  myObject2d.Append(anAisTransfEllipse);
}

void GeometrySamples::ConjugateObjects2dSample()
{
  gp_Parab2d aParab(gp_Ax2d(), 20.0);
  gp_Ax2d aDirectrix = aParab.Directrix();
  gp_Pnt2d aFocus = aParab.Focus();
  gp_Pnt2d aLocation = aParab.Location();
  gp_Ax2d aMirror = aParab.MirrorAxis();

  Handle(AdaptorVec_AIS) aDirectAIS = new AdaptorVec_AIS(aDirectrix.Location(), gp_Vec2d(aDirectrix.Direction())*10.0);
  aDirectAIS->SetText("  Directrix");
  myObject2d.Append(aDirectAIS);
  Handle(AdaptorVec_AIS) aMirrorAIS = new AdaptorVec_AIS(aMirror.Location(), gp_Vec2d(aMirror.Direction())*10.0);
  aMirrorAIS->SetText("  Mirror Axis");
  myObject2d.Append(aMirrorAIS);

  DisplayPnt(aFocus, "Focus", Aspect_TOM_PLUS, -3.0);
  DisplayPnt(aLocation, "  Location", Aspect_TOM_O_STAR, 3.0);
  Handle(Geom2d_Parabola) aGeomParabola = new Geom2d_Parabola(aParab);
  Handle(Geom2d_TrimmedCurve) aTrimmedParabola = new Geom2d_TrimmedCurve(aGeomParabola, 40.0, -40.0);
  Handle(AdaptorCurve2d_AIS) anAisParabola = new AdaptorCurve2d_AIS(aTrimmedParabola, Aspect_TOL_DOT);
  myObject2d.Append(anAisParabola);
}

void GeometrySamples::Tangent2dSample()
{
  gp_Circ2d aCirc1(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Vec2d(1.0, 0.0)), 10.0);
  gp_Circ2d aCirc2 = aCirc1.Translated(gp_Vec2d(50.0, 0.0));
  aCirc2.SetRadius(20.0);

  GccEnt_QualifiedCirc aQaCirc1(aCirc1, GccEnt_outside);
  GccEnt_QualifiedCirc aQaCirc2(aCirc2, GccEnt_outside);

  GccAna_Lin2d2Tan aLin2d2Tan(aQaCirc1, aQaCirc2, 1E-6);
  if (aLin2d2Tan.IsDone())
  {
    for (int i = 1; i <= aLin2d2Tan.NbSolutions(); i++)
    {
      const gp_Lin2d& aTangentLin = aLin2d2Tan.ThisSolution(i);
      Handle(AdaptorVec_AIS) anAisLin = new AdaptorVec_AIS(aTangentLin.Location(), aTangentLin.Direction(), 20.0);
      myObject2d.Append(anAisLin);
    }
  }

  Handle(Geom2d_Circle) aCircle1 = new Geom2d_Circle(aCirc1);
  Handle(AdaptorCurve2d_AIS) anAisCirc1 = new AdaptorCurve2d_AIS(aCircle1, Aspect_TOL_SOLID);
  myObject2d.Append(anAisCirc1);
  Handle(Geom2d_Circle) aCircle2 = new Geom2d_Circle(aCirc2);
  Handle(AdaptorCurve2d_AIS) anAisCirc2 = new AdaptorCurve2d_AIS(aCircle2, Aspect_TOL_SOLID);
  myObject2d.Append(anAisCirc2);
}

void GeometrySamples::ProjectionOfPoint2dSample()
{
  gp_Pnt2d aPntToProject(40.0, 40.0);
  gp_Circ2d aCirc(gp_Ax2d(), 20.0);
  Handle(Geom2d_Circle) aGeom_Circle = new Geom2d_Circle(aCirc);
  Geom2dAPI_ProjectPointOnCurve aProjector(aPntToProject, aGeom_Circle);
  gp_Pnt2d aProjectionPnt = aProjector.NearestPoint();

  Handle(AdaptorCurve2d_AIS) anAisCirc = new AdaptorCurve2d_AIS(aGeom_Circle, Aspect_TOL_SOLID);
  myObject2d.Append(anAisCirc);
  DisplayPnt(aPntToProject, "Pnt to project");
  DisplayPnt(aProjectionPnt, "Projection Pnt", Aspect_TOM_O_STAR);
}

void GeometrySamples::MinimalDistance2dSample()
{
  gp_Lin2d aLin(gp_Pnt2d(-40.0, 0.0), gp_Dir2d(1.0, 1.0));
  Handle(Geom2d_Line) aGeom_Line = new Geom2d_Line(aLin);
  gp_Circ2d aCirc(gp_Ax2d(), 20.0);
  Handle(Geom2d_Circle) aGeom_Circle = new Geom2d_Circle(aCirc);

  Geom2dAPI_ExtremaCurveCurve anExtremaFinder(aGeom_Line, aGeom_Circle,
    std::numeric_limits<Standard_Real>::min(),
    std::numeric_limits<Standard_Real>::max(), 0.0, M_PI*2.0);
  if (anExtremaFinder.NbExtrema())
  {
    gp_Pnt2d aPnt1, aPnt2;
    anExtremaFinder.NearestPoints(aPnt1, aPnt2);
    myResult << "Extrema found: " << anExtremaFinder.NbExtrema() << std::endl;
    myResult << "Minimal distance: " << anExtremaFinder.LowerDistance() << std::endl;
    DisplayPnt(aPnt1, "1");
    DisplayPnt(aPnt2, "2");
  }
  else
  {
    myResult << "No Extrema found" << std::endl;
  }

  Handle(AdaptorCurve2d_AIS) anAisCirc = new AdaptorCurve2d_AIS(aGeom_Circle, Aspect_TOL_SOLID);
  myObject2d.Append(anAisCirc);
  Handle(AdaptorVec_AIS) anAisLin = new AdaptorVec_AIS(aLin.Location(), aLin.Direction(), 60.0);
  anAisLin->SetText("  gp_Lin2d");
  myObject2d.Append(anAisLin);
}

void GeometrySamples::Intersection2dSample()
{
  gp_Lin2d aLin(gp_Pnt2d(-20.0, 20.0), gp_Dir2d(1.0, -1.5));
  Handle(Geom2d_Line) aGeom_Line = new Geom2d_Line(aLin);
  gp_Parab2d aParab(gp_Ax2d(), 20.0);
  Handle(Geom2d_Parabola) aGeom_Parabola = new Geom2d_Parabola(aParab);

  Geom2dAPI_InterCurveCurve anIntersectFinder(aGeom_Line, aGeom_Parabola);
  for (Standard_Integer i = 1; i <= anIntersectFinder.NbPoints(); i++)
  {
    gp_Pnt2d aPnt = anIntersectFinder.Point(i);
    DisplayPnt(aPnt, i);
  }

  myResult << "Number of intersections : " << anIntersectFinder.NbPoints() << std::endl;

  Handle(Geom2d_Parabola) aGeomParabola = new Geom2d_Parabola(aParab);
  Handle(Geom2d_TrimmedCurve) aTrimmedParabola = new Geom2d_TrimmedCurve(aGeomParabola, 60.0, -60.0);
  Handle(AdaptorCurve2d_AIS) anAisParabola = new AdaptorCurve2d_AIS(aTrimmedParabola, Aspect_TOL_DOT);
  myObject2d.Append(anAisParabola);
  Handle(AdaptorVec_AIS) anAisLin = new AdaptorVec_AIS(aLin.Location(), aLin.Direction(), 90.0);
  anAisLin->SetText("  gp_Lin2d");
  myObject2d.Append(anAisLin);
}

void GeometrySamples::PointInfo3dSample()
{
  gp_Pnt aPnt1;
  gp_Pnt aPnt2(10.0, 10.0, 10.0);
  gp_Pnt aPnt3(10.0, -10.0, 0.0);
  gp_Pnt aPnt4(10.0, 10.0, 10.0);
  Standard_Boolean anIsEqual2_3 = aPnt2.IsEqual(aPnt3, 1E-6);
  Standard_Boolean anIsEqual2_4 = aPnt2.IsEqual(aPnt4, 1E-6);
  Standard_Real aDistance1_2 = aPnt1.Distance(aPnt2);
  Standard_Real aDistance2_4 = aPnt2.Distance(aPnt4);
  Standard_Real aSquareDistance1_2 = aPnt1.SquareDistance(aPnt2);
  Standard_Real aSquareDistance2_4 = aPnt2.SquareDistance(aPnt4);

  myResult << "A coordinate of a point 1: X: " << aPnt1.X() << " Y: " << aPnt1.Y() << " Z: " << aPnt1.Z() << std::endl;
  myResult << "A coordinate of a point 2: X: " << aPnt2.X() << " Y: " << aPnt2.Y() << " Z: " << aPnt2.Z() << std::endl;
  myResult << "A coordinate of a point 3: X: " << aPnt3.X() << " Y: " << aPnt3.Y() << " Z: " << aPnt3.Z() << std::endl;
  myResult << "A coordinate of a point 4: X: " << aPnt4.X() << " Y: " << aPnt4.Y() << " Z: " << aPnt4.Z() << std::endl;

  if (anIsEqual2_3)
  {
    myResult << "A point 2 is equal to a point 3" << std::endl;
  }
  else
  {
    myResult << "A point 2 is different from a point 3" << std::endl;
  }
  if (anIsEqual2_4)
  {
    myResult << "A point 2 is equal to a point 4" << std::endl;
  }
  else
  {
    myResult << "A point 2 is different from a point 4" << std::endl;
  }
  myResult << "A distance from a point 1 to a point 2 is: " << aDistance1_2 << std::endl;
  myResult << "A distance from a point 2 to a point 4 is: " << aDistance2_4 << std::endl;

  myResult << "A square distance from a point 1 to a point 2 is: " << aSquareDistance1_2 << std::endl;
  myResult << "A square distance from a point 2 to a point 4 is: " << aSquareDistance2_4 << std::endl;

  DisplayPnt(aPnt1, "1", Aspect_TOM_PLUS, 0.5);
  DisplayPnt(aPnt2, "2 & 4", Aspect_TOM_PLUS, 0.5);
  DisplayPnt(aPnt3, "3", Aspect_TOM_PLUS, 0.5);
  DisplayPnt(aPnt4, "", Aspect_TOM_PLUS, 0.5);
}

void GeometrySamples::EllipseInfo3dSample()
{
  gp_Elips anElips(gp_Ax2(gp_Pnt(), gp_Dir(1.0, 0.0, 0.0)), 20.0, 10.0);
  Standard_Real anArea = anElips.Area();
  // Returns the eccentricity of the ellipse between 0.0 and 1.0
  // If f is the distance between the center of the ellipse and the Focus1 then
  // the eccentricity e = f / MajorRadius. Returns 0 if MajorRadius = 0.
  Standard_Real anEccentricity = anElips.Eccentricity();
  // Returns the distance between the center of the ellipse and focus1 or focus2.
  Standard_Real aFocal = anElips.Focal();
  // Returns p = (1 - e * e) * MajorRadius where e is the eccentricity
  // of the ellipse. Returns 0 if MajorRadius = 0.
  Standard_Real aParameter = anElips.Parameter();

  myResult << "Ellipse area = " << anArea << " square units" << std::endl;
  myResult << "Eccentricity = " << anEccentricity;
  myResult << "Focal distance = " << aFocal;
  myResult << "Ellipse parameter = " << aParameter;

  gp_Pnt aCenter = anElips.Location();
  gp_Pnt aFocus1 = anElips.Focus1();
  gp_Pnt aFocus2 = anElips.Focus2();
  DisplayPnt(aCenter, "Center", Aspect_TOM_PLUS, 2.0);
  DisplayPnt(aFocus1, "focus 1", Aspect_TOM_PLUS, 2.0);
  DisplayPnt(aFocus2, "focus 2", Aspect_TOM_PLUS, 2.0);

  Handle(Geom_Ellipse) aGeomEllipse = new Geom_Ellipse(anElips);
  Handle(AdaptorCurve_AIS) anAisEllipce = new AdaptorCurve_AIS(aGeomEllipse);
  myObject3d.Append(anAisEllipce);
}

void GeometrySamples::PointInfo2dSample()
{
  gp_Pnt2d aPnt1;
  gp_Pnt2d aPnt2(10.0, 10.0);
  gp_Pnt2d aPnt3(10.0, -10.0);
  gp_Pnt2d aPnt4(10.0, 10.0);
  Standard_Boolean anIsEqual2_3 = aPnt2.IsEqual(aPnt3, 1E-6);
  Standard_Boolean anIsEqual2_4 = aPnt2.IsEqual(aPnt4, 1E-6);
  Standard_Real aDistance1_2 = aPnt1.Distance(aPnt2);
  Standard_Real aDistance2_4 = aPnt2.Distance(aPnt4);
  Standard_Real aSquareDistance1_2 = aPnt1.SquareDistance(aPnt2);
  Standard_Real aSquareDistance2_4 = aPnt2.SquareDistance(aPnt4);

  myResult << "A coordinate of a point 1: X: " << aPnt1.X() << " Y: " << aPnt1.Y() << std::endl;
  myResult << "A coordinate of a point 2: X: " << aPnt2.X() << " Y: " << aPnt2.Y() << std::endl;
  myResult << "A coordinate of a point 3: X: " << aPnt3.X() << " Y: " << aPnt3.Y() << std::endl;
  myResult << "A coordinate of a point 4: X: " << aPnt4.X() << " Y: " << aPnt4.Y() << std::endl;
  if (anIsEqual2_3)
  {
    myResult << "A point 2 is equal to a point 3" << std::endl;
  }
  else
  {
    myResult << "A point 2 is different from a point 3" << std::endl;
  }
  if (anIsEqual2_4)
  {
    myResult << "A point 2 is equal to a point 4" << std::endl;
  }
  else
  {
    myResult << "A point 2 is different from a point 4" << std::endl;
  }

  myResult << "A distance from a point 1  to a point 2 is: " << aDistance1_2 << std::endl;
  myResult << "A distance from a point 2  to a point 4 is: " << aDistance2_4 << std::endl;

  myResult << "A square distance from a point 1  to a point 2 is: " << aSquareDistance1_2 << std::endl;
  myResult << "A square distance from a point 2  to a point 4 is: " << aSquareDistance2_4 << std::endl;

  DisplayPnt(aPnt1, "1", Aspect_TOM_PLUS, 0.5);
  DisplayPnt(aPnt2, "2 & 4", Aspect_TOM_PLUS, 0.5);
  DisplayPnt(aPnt3, "3", Aspect_TOM_PLUS, 0.5);
  DisplayPnt(aPnt4, "");
}

void GeometrySamples::CircleInfo2dSample()
{
  gp_Circ2d aCirc(gp_Ax22d(gp_Pnt2d(10.0, 10.0), gp_Vec2d(1.0, 0.0)), 10.0);
  gp_Pnt2d aPnt1(0.0, 10.0);
  gp_Pnt2d aPnt2(10.0, 0.0);
  gp_Pnt2d aPnt3(20.0, 20.0);

  if (aCirc.Contains(aPnt1, 1E-6))
  {
    DisplayPnt(aPnt1, "1", Aspect_TOM_STAR, 3.0);
    myResult << "A circle contains a point 1" << std::endl;
  }
  else
  {
    DisplayPnt(aPnt1, "1", Aspect_TOM_PLUS, 1.0);
    myResult << "A circle does contain a point 1" << std::endl;
  }
  if (aCirc.Contains(aPnt2, 1E-6))
  {
    DisplayPnt(aPnt2, "2", Aspect_TOM_STAR, 1.0);
    myResult << "A circle contains a point 2" << std::endl;
  }
  else
  {
    DisplayPnt(aPnt2, "2", Aspect_TOM_PLUS, 1.0);
    myResult << "A circle does contain a point 2" << std::endl;
  }
  if (aCirc.Contains(aPnt3, 1E-6))
  {
    DisplayPnt(aPnt3, "3", Aspect_TOM_STAR, 1.0);
    myResult << "A circle contains a point 3" << std::endl;
  }
  else
  {
    DisplayPnt(aPnt3, "3", Aspect_TOM_PLUS, 1.0);
    myResult << "A circle does contain a point 3" << std::endl;
  }
  myResult << "Circle area = " << aCirc.Area() << "square units" << std::endl;
  Handle(Geom2d_Circle) aGeomCircle = new Geom2d_Circle(aCirc);
  Handle(AdaptorCurve2d_AIS) anAisCirc = new AdaptorCurve2d_AIS(aGeomCircle);
  myObject2d.Append(anAisCirc);
}

void GeometrySamples::FreeStyleCurves3dSample()
{
  // Define points.
  gp_Pnt aPnt1(0.0, 0.0, 0.0);
  gp_Pnt aPnt2(5.0, 5.0, 0.0);
  gp_Pnt aPnt3(10.0, 5.0, 0.0);
  gp_Pnt aPnt4(15.0, 0.0, 0.0);

  // Add points to the curve poles array.
  TColgp_Array1OfPnt aPoles(1, 4);
  aPoles.SetValue(1, aPnt1);
  aPoles.SetValue(2, aPnt2);
  aPoles.SetValue(3, aPnt3);
  aPoles.SetValue(4, aPnt4);

  // Define BSpline weights.
  TColStd_Array1OfReal aBSplineWeights(1, 4);
  aBSplineWeights.SetValue(1, 1.0);
  aBSplineWeights.SetValue(2, 0.5);
  aBSplineWeights.SetValue(3, 0.5);
  aBSplineWeights.SetValue(4, 1.0);

  // Define knots.
  TColStd_Array1OfReal aKnots(1, 2);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 1.0);

  // Define multiplicities.
  TColStd_Array1OfInteger aMults(1, 2);
  aMults.SetValue(1, 4);
  aMults.SetValue(2, 4);

  // Define BSpline degree and periodicity.
  Standard_Integer aDegree = 3;
  Standard_Boolean aPeriodic = Standard_False;

  // Create a BSpline curve.
  Handle(Geom_BSplineCurve) aBSplineCurve = new Geom_BSplineCurve(
    aPoles, aBSplineWeights, aKnots, aMults, aDegree, aPeriodic);
  myResult << "Geom_BSplineCurve was created in red" << std::endl;

  // Define Bezier weights.
  TColStd_Array1OfReal aBezierWeights(1, 4);
  aBezierWeights.SetValue(1, 0.5);
  aBezierWeights.SetValue(2, 1.5);
  aBezierWeights.SetValue(3, 1.5);
  aBezierWeights.SetValue(4, 0.5);

  // Create Bezier curve.
  Handle(Geom_BezierCurve) aBezierCurve = new Geom_BezierCurve(aPoles, aBezierWeights);
  myResult << "Geom_BezierCurve was created in green" << std::endl;

  Handle(AIS_ColoredShape) anAisBSplineCurve = new AIS_ColoredShape(
    BRepBuilderAPI_MakeEdge(aBSplineCurve).Shape());
  Handle(AIS_ColoredShape) anAisBezierCurve = new AIS_ColoredShape(
    BRepBuilderAPI_MakeEdge(aBezierCurve).Shape());
  anAisBSplineCurve->SetColor(Quantity_Color(Quantity_NOC_RED));
  anAisBezierCurve->SetColor(Quantity_Color(Quantity_NOC_GREEN));
  myObject3d.Append(anAisBSplineCurve);
  myObject3d.Append(anAisBezierCurve);
  myObject3d.Append(new AIS_Point(new Geom_CartesianPoint(aPnt1)));
  myObject3d.Append(new AIS_Point(new Geom_CartesianPoint(aPnt2)));
  myObject3d.Append(new AIS_Point(new Geom_CartesianPoint(aPnt3)));
  myObject3d.Append(new AIS_Point(new Geom_CartesianPoint(aPnt4)));
}

void GeometrySamples::AnalyticalSurfaces3dSample()
{
  // Define a XY plane.
  gp_Pln aPln(gp::Origin(), gp::DZ());
  // Create plane geometry.
  Handle(Geom_Plane) aPlaneSurf = new Geom_Plane(aPln);
  myResult << "Geom_Plane was created in red" << std::endl;

  // Define a cylinder.
  gp_Cylinder aCyl(gp::XOY(), 2.5);
  // Create cylindrical surface.
  Handle(Geom_CylindricalSurface) aCylSurf = new Geom_CylindricalSurface(aCyl);
  myResult << "Geom_CylindricalSurface was created in green" << std::endl;

  // Define a cone.
  gp_Cone aCone(gp::XOY(), M_PI_4, 2.5);
  // Create conical surface.
  Handle(Geom_ConicalSurface) aConeSurf = new Geom_ConicalSurface(aCone);
  myResult << "Geom_ConicalSurface was created in blue" << std::endl;

  // Define a sphere.
  gp_Pnt aSphereCenter(15.0, 15.0, 15.0);
  gp_Sphere aSphere(gp_Ax3(aSphereCenter, gp::DZ()), 8.0);
  // Create conical surface.
  Handle(Geom_SphericalSurface) aSphereSurf = new Geom_SphericalSurface(aSphere);
  myResult << "Geom_SphericalSurface was created in cyan" << std::endl;

  // Define a sphere.
  gp_Pnt aTorusCenter(-15.0, -15.0, 25.0);
  gp_Torus aTorus(gp_Ax3(aTorusCenter, gp::DZ()), 15.0, 5.0);
  // Create toroidal surface.
  Handle(Geom_ToroidalSurface) aTorusSurf = new Geom_ToroidalSurface(aTorus);
  myResult << "Geom_ToroidalSurface was created in yellow" << std::endl;

  Handle(AIS_ColoredShape) anAisPlane = new AIS_ColoredShape(BRepBuilderAPI_MakeFace(
    aPlaneSurf, 0.0, 20.0, 0.0, 20.0, Precision::Confusion()).Shape());
  Handle(AIS_ColoredShape) anAisCylinder = new AIS_ColoredShape(BRepBuilderAPI_MakeFace(
    aCylSurf, 0.0, 2.0 * M_PI, 5.0, 15.0, Precision::Confusion()).Shape());
  Handle(AIS_ColoredShape) anAisCone = new AIS_ColoredShape(BRepBuilderAPI_MakeFace(
    aConeSurf, 0.0, 2.0 * M_PI, 0.0, 15.0, Precision::Confusion()).Shape());
  Handle(AIS_ColoredShape) anAisSphere = new AIS_ColoredShape(BRepBuilderAPI_MakeFace(
    aSphereSurf, Precision::Confusion()).Shape());
  Handle(AIS_ColoredShape) anAisTorus = new AIS_ColoredShape(BRepBuilderAPI_MakeFace(
    aTorusSurf, Precision::Confusion()).Shape());
  anAisPlane->SetColor(Quantity_Color(Quantity_NOC_RED));
  anAisCylinder->SetColor(Quantity_Color(Quantity_NOC_GREEN));
  anAisCone->SetColor(Quantity_Color(Quantity_NOC_BLUE1));
  anAisSphere->SetColor(Quantity_Color(Quantity_NOC_CYAN1));
  anAisTorus->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
  myObject3d.Append(anAisPlane);
  myObject3d.Append(anAisCylinder);
  myObject3d.Append(anAisCone);
  myObject3d.Append(anAisSphere);
  myObject3d.Append(anAisTorus);
}

void GeometrySamples::FreeStyleSurfaces3dSample()
{
  // Define a 4x4 grid of points for BSpline surface.
  TColgp_Array2OfPnt aBSplinePnts(1, 4, 1, 4);
  for (Standard_Integer i = 1; i <= 4; ++i)
  {
    gp_Pnt aPnt;
    aPnt.SetX(5.0 * i);
    for (Standard_Integer j = 1; j <= 4; ++j)
    {
      aPnt.SetY(5.0 * j);
      if (1 < i && i < 4 && 1 < j && j < 4)
      {
        aPnt.SetZ(5.0);
      }
      else
      {
        aPnt.SetZ(0.0);
      }
      aBSplinePnts.SetValue(i, j, aPnt);
    }
  }

  // Define a 4x4 grid of points for Bezier surface.
  TColgp_Array2OfPnt aBezierPnts(1, 4, 1, 4);
  for (Standard_Integer i = 1; i <= 4; ++i)
  {
    gp_Pnt aPnt;
    aPnt.SetX(20.0 + 5.0 * i);
    for (Standard_Integer j = 1; j <= 4; ++j)
    {
      aPnt.SetY(20.0 + 5.0 * j);
      if (1 < i && i < 4 && 1 < j && j < 4)
      {
        aPnt.SetZ(5.0);
      }
      else
      {
        aPnt.SetZ(0.0);
      }
      aBezierPnts.SetValue(i, j, aPnt);
    }
  }

  // Define BSpline weights.
  TColStd_Array2OfReal aBSplineWeights(1, 4, 1, 4);
  for (Standard_Integer i = 1; i <= 4; ++i)
  {
    for (Standard_Integer j = 1; j <= 4; ++j)
    {
      if (1 < i && i < 4 && 1 < j && j < 4)
      {
        aBSplineWeights.SetValue(i, j, 0.5);
      }
      else
      {
        aBSplineWeights.SetValue(i, j, 1.0);
      }
    }
  }

  // Define knots.
  TColStd_Array1OfReal aUKnots(1, 2), aVKnots(1, 2);
  aUKnots.SetValue(1, 0.0);
  aUKnots.SetValue(2, 1.0);
  aVKnots.SetValue(1, 0.0);
  aVKnots.SetValue(2, 1.0);

  // Define multiplicities.
  TColStd_Array1OfInteger aUMults(1, 2), aVMults(1, 2);
  aUMults.SetValue(1, 4);
  aUMults.SetValue(2, 4);
  aVMults.SetValue(1, 4);
  aVMults.SetValue(2, 4);

  // Define BSpline degree and periodicity.
  Standard_Integer aUDegree = 3;
  Standard_Integer aVDegree = 3;
  Standard_Boolean aUPeriodic = Standard_False;
  Standard_Boolean aVPeriodic = Standard_False;

  // Create a BSpline surface.
  Handle(Geom_BSplineSurface) aBSplineSurf = new Geom_BSplineSurface(
    aBSplinePnts, aBSplineWeights, aUKnots, aVKnots,
    aUMults, aVMults, aUDegree, aVDegree, aUPeriodic, aVPeriodic);
  myResult << "Geom_BSplineSurface was created in red" << std::endl;

  // Define BSpline weights.
  TColStd_Array2OfReal aBezierWeights(1, 4, 1, 4);
  for (Standard_Integer i = 1; i <= 4; ++i)
  {
    for (Standard_Integer j = 1; j <= 4; ++j)
    {
      if (1 < i && i < 4 && 1 < j && j < 4)
      {
        aBezierWeights.SetValue(i, j, 1.5);
      }
      else
      {
        aBezierWeights.SetValue(i, j, 0.5);
      }
    }
  }

  // Create a Bezier surface.
  Handle(Geom_BezierSurface) aBezierSurf = new Geom_BezierSurface(aBezierPnts, aBezierWeights);
  myResult << "Geom_BezierSurface was created in green" << std::endl;

  Handle(AIS_ColoredShape) anAisBSplineSurf = new AIS_ColoredShape(
    BRepBuilderAPI_MakeFace(aBSplineSurf, Precision::Confusion()).Shape());
  Handle(AIS_ColoredShape) anAisBezierSurf = new AIS_ColoredShape(
    BRepBuilderAPI_MakeFace(aBezierSurf, Precision::Confusion()).Shape());
  anAisBSplineSurf->SetColor(Quantity_Color(Quantity_NOC_RED));
  anAisBezierSurf->SetColor(Quantity_Color(Quantity_NOC_GREEN));
  myObject3d.Append(anAisBSplineSurf);
  myObject3d.Append(anAisBezierSurf);
  for (TColgp_Array2OfPnt::Iterator anIt(aBSplinePnts); anIt.More(); anIt.Next())
  {
    myObject3d.Append(new AIS_Point(new Geom_CartesianPoint(anIt.Value())));
  }
  for (TColgp_Array2OfPnt::Iterator anIt(aBezierPnts); anIt.More(); anIt.Next())
  {
    myObject3d.Append(new AIS_Point(new Geom_CartesianPoint(anIt.Value())));
  }
}

void GeometrySamples::FreeStyleCurves2dSample()
{

  // Define points.
  gp_Pnt2d aPnt1(0.0, 0.0);
  gp_Pnt2d aPnt2(5.0, 5.0);
  gp_Pnt2d aPnt3(10.0, 5.0);
  gp_Pnt2d aPnt4(15.0, 0.0);

  // Add points to the curve poles array.
  TColgp_Array1OfPnt2d aBSplinePoles(1, 4);
  aBSplinePoles.SetValue(1, aPnt1);
  aBSplinePoles.SetValue(2, aPnt2);
  aBSplinePoles.SetValue(3, aPnt3);
  aBSplinePoles.SetValue(4, aPnt4);

  // Define BSpline weights.
  TColStd_Array1OfReal aBSplineWeights(1, 4);
  aBSplineWeights.SetValue(1, 1.0);
  aBSplineWeights.SetValue(2, 0.5);
  aBSplineWeights.SetValue(3, 0.5);
  aBSplineWeights.SetValue(4, 1.0);

  // Define knots.
  TColStd_Array1OfReal aKnots(1, 2);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 1.0);

  // Define multiplicities.
  TColStd_Array1OfInteger aMults(1, 2);
  aMults.SetValue(1, 4);
  aMults.SetValue(2, 4);

  // Define BSpline degree and periodicity.
  Standard_Integer aDegree = 3;
  Standard_Boolean aPeriodic = Standard_False;

  // Create a BSpline curve.
  Handle(Geom2d_BSplineCurve) aBSplineCurve =
    new Geom2d_BSplineCurve(aBSplinePoles, aBSplineWeights, aKnots, aMults, aDegree, aPeriodic);


  TColgp_Array1OfPnt2d aBezierPoles(1, 4);
  gp_Vec2d anUp10Vec(0.0, 10.0);
  aBezierPoles.SetValue(1, aPnt1.Translated(anUp10Vec));
  aBezierPoles.SetValue(2, aPnt2.Translated(anUp10Vec));
  aBezierPoles.SetValue(3, aPnt3.Translated(anUp10Vec));
  aBezierPoles.SetValue(4, aPnt4.Translated(anUp10Vec));

  // Define Bezier weights.
  TColStd_Array1OfReal aBezierWeights(1, 4);
  aBezierWeights.SetValue(1, 0.5);
  aBezierWeights.SetValue(2, 1.5);
  aBezierWeights.SetValue(3, 1.5);
  aBezierWeights.SetValue(4, 0.5);

  // Create Bezier curve.
  Handle(Geom2d_BezierCurve) aBezierCurve = new Geom2d_BezierCurve(aBezierPoles, aBezierWeights);

  Handle(AdaptorCurve2d_AIS) anAisBSpline = new AdaptorCurve2d_AIS(aBSplineCurve);
  myObject2d.Append(anAisBSpline);
  Handle(AdaptorCurve2d_AIS) anAisBezier = new AdaptorCurve2d_AIS(aBezierCurve);
  myObject2d.Append(anAisBezier);

  DisplayPnt(aPnt1, "1", Aspect_TOM_PLUS, 0.5);
  DisplayPnt(aPnt2, "2", Aspect_TOM_PLUS, 0.5);
  DisplayPnt(aPnt3, "3", Aspect_TOM_PLUS, 0.5);
  DisplayPnt(aPnt4, "4", Aspect_TOM_PLUS, 0.5);
}

void GeometrySamples::TrimmedCurve3dSample()
{
  // Define a circle placed in the origin of XY coordinate
  // plane and with the radius equal to 5.
  gp_Circ aCirc(gp::XOY(), 5.0);
  // Create a closed circular curve.
  Handle(Geom_Circle) aCircCurve = new Geom_Circle(aCirc);
  myResult << "Geom_Circle was created in yellow" << std::endl;

  // Cut off a quarter of the circle.
  Handle(Geom_TrimmedCurve) aCircQuater = new Geom_TrimmedCurve(aCircCurve, 0.0, M_PI_2);
  myResult << "Geom_TrimmedCurve was created in red" << std::endl;

  Handle(AIS_ColoredShape) anAisCirc = new AIS_ColoredShape (BRepBuilderAPI_MakeEdge(aCircCurve).Shape());
  Handle(AIS_ColoredShape) anAisCircQuater = new AIS_ColoredShape (BRepBuilderAPI_MakeEdge(aCircQuater).Shape());
  anAisCirc->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
  anAisCircQuater->SetColor(Quantity_Color(Quantity_NOC_RED));
  anAisCircQuater->SetWidth(2.5);
  myObject3d.Append(anAisCirc);
  myObject3d.Append(anAisCircQuater);
}

void GeometrySamples::OffsetCurve3dSample()
{
  // Define a circle placed in the origin of XY coordinate
  // plane and with the radius equal to 5.
  gp_Circ aCirc(gp::XOY(), 5.0);
  // Create a closed circular curve.
  Handle(Geom_Circle) aCircCurve = new Geom_Circle(aCirc);
  myResult << "Geom_Circle was created in yellow" << std::endl;

  // An offset curve is a curve at constant distance (Offset) from
  // a basis curve in a reference direction V.
  // The offset curve takes its parametrization from the basis curve.
  // The Offset curve is in the direction of the normal N
  // defined with the cross product T^V, where the vector T
  // is given by the first derivative on the basis curve with non zero length.
  // The distance offset may be positive or negative to indicate the
  // preferred side of the curve:
  // . distance offset >0 => the curve is in the direction of N
  // . distance offset <0 => the curve is in the direction of - N
  // On the Offset curve:
  // Value (U) = BasisCurve.Value(U) + (Offset * (T ^ V)) / ||T ^ V||
  // At any point the Offset direction V must not be parallel to the
  // vector T and the vector T must not have null length else the
  // offset curve is not defined.

  // Expand the circle by Offset equal to a quarter of the radius
  // with direction V equal to Z.
  Standard_Real anExpandOffset = +aCirc.Radius() / 4.0;
  gp_Dir anExpandDir = gp::DZ();
  Handle(Geom_OffsetCurve) anExpandCircCurve = new Geom_OffsetCurve(
    aCircCurve, anExpandOffset, anExpandDir);
  myResult << "Geom_OffsetCurve (expanded circle) was created in red" << std::endl;

  // Collapse the circle by Offset equal to a half of the radius with direction V equal to Z.
  Standard_Real anCollapseOffset = -aCirc.Radius() / 2.0;
  gp_Dir anCollapseDir = gp::DZ();
  Handle(Geom_OffsetCurve) anCollapseCircCurve = new Geom_OffsetCurve (aCircCurve, anCollapseOffset, anCollapseDir);
  myResult << "Geom_OffsetCurve (collapsed circle) was created in green" << std::endl;

  Handle(AIS_ColoredShape) anAisCirc = new AIS_ColoredShape (BRepBuilderAPI_MakeEdge(aCircCurve).Shape());
  Handle(AIS_ColoredShape) anAisExpandCirc = new AIS_ColoredShape (BRepBuilderAPI_MakeEdge(anExpandCircCurve).Shape());
  Handle(AIS_ColoredShape) anAisCpllapsedCirc = new AIS_ColoredShape (BRepBuilderAPI_MakeEdge(anCollapseCircCurve).Shape());
  anAisCirc->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
  anAisExpandCirc->SetColor(Quantity_Color(Quantity_NOC_RED));
  anAisCpllapsedCirc->SetColor(Quantity_Color(Quantity_NOC_GREEN));
  myObject3d.Append(anAisCirc);
  myObject3d.Append(anAisExpandCirc);
  myObject3d.Append(anAisCpllapsedCirc);
}

void GeometrySamples::BSplineFromCircle3dSample()
{
  // Define a circle placed in the origin of XY coordinate
  // plane and with the radius equal to 5.
  gp_Circ aCirc(gp::XOY(), 5.0);
  // Create a closed circular curve.
  Handle(Geom_Circle) aCircCurve = new Geom_Circle(aCirc);
  myResult << "Geom_Circle was created in yellow" << std::endl;

  // Convert the circle curve to a BSpline one.
  Handle(Geom_BSplineCurve) aBSplineFromCirc = GeomConvert::CurveToBSplineCurve(aCircCurve);
  myResult << "Geom_BSplineCurve was created in red:" << std::endl;
  myResult << "Degree:   " << aBSplineFromCirc->Degree() << std::endl;
  myResult << "Periodic: " << (aBSplineFromCirc->IsPeriodic() ? "Yes" : "No") << std::endl;
  myResult << "Poles: [" << aBSplineFromCirc->Poles().Size() << "]" << std::endl;
  for (TColgp_Array1OfPnt::Iterator anIt(aBSplineFromCirc->Poles()); anIt.More(); anIt.Next())
  {
    myResult << "  (" << anIt.Value().X() << ", " << anIt.Value().Y() << ", " << anIt.Value().Z() << ")" << std::endl;
  }

  Handle(AIS_ColoredShape) anAisCirc = new AIS_ColoredShape (BRepBuilderAPI_MakeEdge(aCircCurve).Shape());
  Handle(AIS_ColoredShape) anAisBSpline = new AIS_ColoredShape (BRepBuilderAPI_MakeEdge(aBSplineFromCirc).Shape());
  anAisCirc->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
  anAisBSpline->SetColor(Quantity_Color(Quantity_NOC_RED));
  myObject3d.Append(anAisCirc);
  myObject3d.Append(anAisBSpline);
}

void GeometrySamples::TrimmedSurface3dSample()
{
  // Define a XY plane.
  gp_Pln aPln(gp::XOY());
  // Create a plane surface.
  Handle(Geom_Plane) aPlaneSurf = new Geom_Plane(aPln);
  myResult << "Geom_Plane was created" << std::endl;

  // Trim [0 ... 30 X 0 ... 50] rectangular range.
  Standard_Real aUMin = 0.0;
  Standard_Real aUMax = 30.0;
  Standard_Real aVMin = 0.0;
  Standard_Real aVMax = 50.0;
  Handle(Geom_RectangularTrimmedSurface) aTrimmedPlaneSurf
    = new Geom_RectangularTrimmedSurface(aPlaneSurf, aUMin, aUMax, aVMin, aVMax);
  myResult << "Geom_RectangularTrimmedSurface was created in red" << std::endl;

  Handle(AIS_Plane) anAisPlane = new AIS_Plane(aPlaneSurf);
  Handle(AIS_ColoredShape) anAisTimmedPlane = new AIS_ColoredShape (BRepBuilderAPI_MakeFace (aTrimmedPlaneSurf, 0.001).Shape());
  anAisTimmedPlane->SetColor(Quantity_Color(Quantity_NOC_RED));
  myObject3d.Append(anAisPlane);
  myObject3d.Append(anAisTimmedPlane);
}

void GeometrySamples::OffsetSurface3dSample()
{
  // Define a XY plane.
  gp_Pln aPln(gp::XOY());
  // Create a plane surface.
  Handle(Geom_Plane) aPlaneSurf = new Geom_Plane(aPln);
  myResult << "Geom_Plane was created" << std::endl;

  // An offset surface is defined by:
  // - the basis surface to which it is parallel, and
  // - the distance between the offset surface and its basis surface.
  // A point on the offset surface is built by measuring the
  // offset value along the normal vector at a point on the
  // basis surface. This normal vector is given by the cross
  // product D1u^D1v, where D1u and D1v are the
  // vectors tangential to the basis surface in the u and v
  // parametric directions at this point. The side of the
  // basis surface on which the offset is measured
  // depends on the sign of the offset value.

  // Offset the plane in the normal direction.
  Standard_Real aPosOffset = 10.0;
  Handle(Geom_OffsetSurface) aPosOffsetSurf = new Geom_OffsetSurface(aPlaneSurf, aPosOffset);
  myResult << "Geom_OffsetSurface with " << aPosOffset << " was created in red" << std::endl;

  // Offset the plane in direction opposite to the normal one.
  Standard_Real aNegOffset = -15.0;
  Handle(Geom_OffsetSurface) aNegOffsetSurf = new Geom_OffsetSurface(aPlaneSurf, aNegOffset);
  myResult << "Geom_OffsetSurface with " << aNegOffset << " was created in green" << std::endl;

  Handle(AIS_ColoredShape) anAisPlane = new AIS_ColoredShape(
    BRepBuilderAPI_MakeFace(aPlaneSurf, 0.0, 10.0, 0.0, 10.0, Precision::Confusion()).Shape());
  Handle(AIS_ColoredShape) anAisPosOffsetPlane = new AIS_ColoredShape(
    BRepBuilderAPI_MakeFace(aPosOffsetSurf, 0.0, 10.0, 0.0, 10.0, Precision::Confusion()).Shape());
  Handle(AIS_ColoredShape) anAisNegOffsetPlane = new AIS_ColoredShape(
    BRepBuilderAPI_MakeFace(aNegOffsetSurf, 0.0, 10.0, 0.0, 10.0, Precision::Confusion()).Shape());
  anAisPosOffsetPlane->SetColor(Quantity_Color(Quantity_NOC_RED));
  anAisNegOffsetPlane->SetColor(Quantity_Color(Quantity_NOC_GREEN));
  myObject3d.Append(anAisPlane);
  myObject3d.Append(anAisPosOffsetPlane);
  myObject3d.Append(anAisNegOffsetPlane);
}

void GeometrySamples::ExtrusionSurface3dSample()
{
  // Create an ellipse curve in XY plane.
  Standard_Real aMinorRadius = 10.0;
  Standard_Real aMajorRadius = 20.0;
  Handle(Geom_Ellipse) anEllipseCurve = new Geom_Ellipse(gp::XOY(), aMajorRadius, aMinorRadius);
  myResult << "Geom_Ellipse was created in yellow" << std::endl;

  // Make a linear extrusion of the ellipse at 45 degrees to Z axis
  gp_Dir aDirOfExtr = gp::DZ();
  Handle(Geom_SurfaceOfLinearExtrusion) aLinExtrSurf
    = new Geom_SurfaceOfLinearExtrusion(anEllipseCurve, aDirOfExtr);
  myResult << "Geom_SurfaceOfLinearExtrusion was created in red" << std::endl;

  Handle(AIS_ColoredShape) anAisEllipse = new AIS_ColoredShape(
    BRepBuilderAPI_MakeEdge(anEllipseCurve).Shape());
  Handle(AIS_ColoredShape) anAisExtrSurf = new AIS_ColoredShape(
    BRepBuilderAPI_MakeFace(aLinExtrSurf, 0.0, 2.0 * M_PI, 0.0, 30.0,
      Precision::Confusion()).Shape());
  anAisEllipse->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
  anAisEllipse->SetWidth(2.5);
  anAisExtrSurf->SetColor(Quantity_Color(Quantity_NOC_RED));
  myObject3d.Append(anAisEllipse);
  myObject3d.Append(anAisExtrSurf);
}

void GeometrySamples::RevolutionSurface3dSample()
{
  // Create an ellipse curve in XY plane with
  // the center at (-10, 0, 0).
  Standard_Real aMinorRadius = 5.0;
  Standard_Real aMajorRadius = 10.0;
  gp_Pnt aCenter(-30.0, 0.0, 0.0);
  Handle(Geom_Ellipse) anEllipseCurve = new Geom_Ellipse(gp_Ax2(aCenter, gp::DZ()),
    aMajorRadius, aMinorRadius);
  myResult << "Geom_Ellipse was created in yellow" << std::endl;

  // Make a revolution of the ellipse around Y axis
  Handle(Geom_SurfaceOfRevolution) aRevolSurf = new Geom_SurfaceOfRevolution(anEllipseCurve, gp::OY());
  myResult << "Geom_SurfaceOfRevolution was created in red" << std::endl;

  Handle(AIS_ColoredShape) anAisEllipse = new AIS_ColoredShape(
    BRepBuilderAPI_MakeEdge(anEllipseCurve).Shape());
  Handle(AIS_ColoredShape) anAisRevolSurf = new AIS_ColoredShape(
    BRepBuilderAPI_MakeFace(aRevolSurf, Precision::Confusion()).Shape());
  anAisEllipse->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
  anAisEllipse->SetWidth(2.5);
  anAisRevolSurf->SetColor(Quantity_Color(Quantity_NOC_RED));
  myObject3d.Append(anAisEllipse);
  myObject3d.Append(anAisRevolSurf);
}

void GeometrySamples::TrimmedCurve2dSample()
{
  // Create a closed circular curve.
  Handle(Geom2d_Circle) aGeomCircle = new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(), gp_Vec2d(1.0, 0.0)), 5.0);
  Handle(AdaptorCurve2d_AIS) anAisCirc = new AdaptorCurve2d_AIS(aGeomCircle);
  myObject2d.Append(anAisCirc);

  // Cut off a quarter of the circle.
  Handle(Geom2d_TrimmedCurve) aCircQuater = new Geom2d_TrimmedCurve(aGeomCircle, 0.0, M_PI_2);
  aCircQuater->Translate(gp_Vec2d(15.0, 0.0));
  Handle(AdaptorCurve2d_AIS) anAisCircQuater = new AdaptorCurve2d_AIS(aCircQuater);
  myObject2d.Append(anAisCircQuater);
}

void GeometrySamples::OffsetCurve2dSample()
{
  Handle(Geom2d_Circle) aGeomCircle = new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(), gp_Vec2d(1.0, 0.0)), 5.0);

  Standard_Real anExpandOffset = aGeomCircle->Radius() / 4.0;
  Handle(Geom2d_OffsetCurve) anExpandCircCurve = new Geom2d_OffsetCurve(aGeomCircle, anExpandOffset);

  Standard_Real anCollapseOffset = -aGeomCircle->Radius() / 2.0;
  Handle(Geom2d_OffsetCurve) anCollapseCircCurve = new Geom2d_OffsetCurve(aGeomCircle, anCollapseOffset);

  Handle(AdaptorCurve2d_AIS) anAisCirc = new AdaptorCurve2d_AIS(aGeomCircle);
  myObject2d.Append(anAisCirc);
  Handle(AdaptorCurve2d_AIS) anAisExpand = new AdaptorCurve2d_AIS(anExpandCircCurve);
  myObject2d.Append(anAisExpand);
  Handle(AdaptorCurve2d_AIS) anAisCollapse = new AdaptorCurve2d_AIS(anCollapseCircCurve);
  myObject2d.Append(anAisCollapse);
}

void GeometrySamples::BoundingBoxOfSurface3dSample()
{
  // Define a 4x4 grid of points for BSpline surface.
  TColgp_Array2OfPnt aPoints(1, 4, 1, 4);
  for (Standard_Integer i = 1; i <= 4; ++i)
  {
    gp_Pnt aPnt;
    aPnt.SetX(5.0 * i);
    for (Standard_Integer j = 1; j <= 4; ++j)
    {
      aPnt.SetY(5.0 * j);
      if (1 < i && i < 4 && 1 < j && j < 4)
      {
        aPnt.SetZ(5.0);
      }
      else
      {
        aPnt.SetZ(0.0);
      }
      aPoints.SetValue(i, j, aPnt);
    }
  }

  // Make a BSpline surface from the points array.
  Handle(Geom_BSplineSurface) aBSplineSurf = GeomAPI_PointsToBSplineSurface(aPoints).Surface();
  myResult << "Geom_BSplineSurface was created" << std::endl;

  // Compute BSpline surface bounding box.
  Bnd_Box aBndBox;
  BndLib_AddSurface::AddOptimal(GeomAdaptor_Surface(aBSplineSurf), Precision::Confusion(), aBndBox);
  myResult << "Bounding box:" << std::endl;
  myResult << "  Min corner = [ "
           << aBndBox.CornerMin().X() << ", "
           << aBndBox.CornerMin().Y() << ", "
           << aBndBox.CornerMin().Z() << " ]" << std::endl;
  myResult << "  Max corner = [ "
           << aBndBox.CornerMax().X() << ", "
           << aBndBox.CornerMax().Y() << ", "
           << aBndBox.CornerMax().Z() << " ]" << std::endl;

  Handle(AIS_ColoredShape) anAisBSplineSurf = new AIS_ColoredShape(
    BRepBuilderAPI_MakeFace(aBSplineSurf, Precision::Confusion()).Shape());
  Handle(AIS_ColoredShape) anAisBndBox = new AIS_ColoredShape(
    BRepPrimAPI_MakeBox(aBndBox.CornerMin(), aBndBox.CornerMax()).Shell());
  myObject3d.Append(anAisBSplineSurf);
  myObject3d.Append(anAisBndBox);
  myContext->SetDisplayMode(anAisBndBox, 0, Standard_True);
}

void GeometrySamples::BoundingBoxOfCurves3dSample()
{
  // Define points.
  gp_Pnt aPnt1(0.0, 0.0, 10.0);
  gp_Pnt aPnt2(5.0, 5.0, 5.0);
  gp_Pnt aPnt3(10.0, 10.0, 15.0);
  gp_Pnt aPnt4(15.0, 5.0, 20.0);

  // Add points to the curve poles array.
  TColgp_Array1OfPnt aPoles(1, 4);
  aPoles.SetValue(1, aPnt1);
  aPoles.SetValue(2, aPnt2);
  aPoles.SetValue(3, aPnt3);
  aPoles.SetValue(4, aPnt4);

  // Make a BSpline curve from the points array.
  Handle(Geom_BSplineCurve) aBSplineCurve = GeomAPI_PointsToBSpline(aPoles).Curve();
  myResult << "aBSplineCurve was created" << std::endl;

  // Compute BSpline curve bounding box.
  Bnd_Box aBndBox;
  BndLib_Add3dCurve::AddOptimal(GeomAdaptor_Curve(aBSplineCurve), Precision::Confusion(), aBndBox);
  myResult << "Bounding box:" << std::endl;
  myResult << "  Min corner = [ "
           << aBndBox.CornerMin().X() << ", "
           << aBndBox.CornerMin().Y() << ", "
           << aBndBox.CornerMin().Z() << " ]" << std::endl;
  myResult << "  Max corner = [ "
           << aBndBox.CornerMax().X() << ", "
           << aBndBox.CornerMax().Y() << ", "
           << aBndBox.CornerMax().Z() << " ]" << std::endl;

  Handle(AIS_ColoredShape) anAisBSplineCurve = new AIS_ColoredShape (BRepBuilderAPI_MakeEdge(aBSplineCurve).Shape());
  Handle(AIS_ColoredShape) anAisBndBox = new AIS_ColoredShape (BRepPrimAPI_MakeBox(aBndBox.CornerMin(), aBndBox.CornerMax()).Shell());
  myObject3d.Append(anAisBSplineCurve);
  myObject3d.Append(anAisBndBox);
  myContext->SetDisplayMode(anAisBndBox, 0, Standard_True);
}

void GeometrySamples::BoundingBoxOfCurves2dSample()
{
  // Define points.
  gp_Pnt2d aPnt1(0.0, 0.0);
  gp_Pnt2d aPnt2(5.0, 5.0);
  gp_Pnt2d aPnt3(10.0, 10.0);
  gp_Pnt2d aPnt4(15.0, 5.0);

  // Add points to the curve poles array.
  TColgp_Array1OfPnt2d aPoles(1, 4);
  aPoles.SetValue(1, aPnt1);
  aPoles.SetValue(2, aPnt2);
  aPoles.SetValue(3, aPnt3);
  aPoles.SetValue(4, aPnt4);

  // Make a BSpline curve from the points array.
  Handle(Geom2d_BSplineCurve) aBSplineCurve = Geom2dAPI_PointsToBSpline(aPoles).Curve();

  // Compute BSpline curve bounding box.
  Bnd_Box2d aBndBox;
  BndLib_Add2dCurve::AddOptimal(aBSplineCurve, 0.0, 1.0, Precision::PConfusion(), aBndBox);
  Standard_Real aXmin, aYmin, aXmax, aYmax;
  aBndBox.Get(aXmin, aYmin, aXmax, aYmax);

  myResult << "Bounding box:" << std::endl;
  myResult << "  Min corner = [ " << aXmin << ", " << aYmin << " ]" << std::endl;
  myResult << "  Max corner = [ " << aXmax << ", " << aYmax << " ]" << std::endl;

  Handle(AdaptorCurve2d_AIS) anAisBSpline = new AdaptorCurve2d_AIS(aBSplineCurve);

  Handle(AdaptorVec_AIS) anAisVec1 = new AdaptorVec_AIS(gp_Pnt2d(aXmin, aYmin), gp_Pnt2d(aXmin, aYmax));
  Handle(AdaptorVec_AIS) anAisVec2 = new AdaptorVec_AIS(gp_Pnt2d(aXmin, aYmax), gp_Pnt2d(aXmax, aYmax));
  Handle(AdaptorVec_AIS) anAisVec3 = new AdaptorVec_AIS(gp_Pnt2d(aXmax, aYmax), gp_Pnt2d(aXmax, aYmin));
  Handle(AdaptorVec_AIS) anAisVec4 = new AdaptorVec_AIS(gp_Pnt2d(aXmax, aYmin), gp_Pnt2d(aXmin, aYmin));

  myObject2d.Append(anAisBSpline);
  myObject2d.Append(anAisVec1);
  myObject2d.Append(anAisVec2);
  myObject2d.Append(anAisVec3);
  myObject2d.Append(anAisVec4);
}

void GeometrySamples::DumpCircleInfoSample()
{
  // Define a circle placed in the origin of XY coordinate
  // plane and with the radius equal to 0.5.
  gp_Circ aCirc(gp::XOY(), 0.5);
  // Create a closed circular curve.
  Handle(Geom_Circle) aCircCurve = new Geom_Circle(aCirc);
  myResult << "Geom_Circle was created:" << std::endl;
  myResult << " Center = [ "
           << aCircCurve->Position().Location().X() << ", "
           << aCircCurve->Position().Location().Y() << ", "
           << aCircCurve->Position().Location().Z() << " ]"
           << std::endl;
  myResult << " Radius = " << aCircCurve->Radius() << std::endl;
  myResult << " Plane normal = [ "
           << aCircCurve->Position().Direction().X() << ", "
           << aCircCurve->Position().Direction().Y() << ", "
           << aCircCurve->Position().Direction().Z() << " ]"
           << std::endl;

  Handle(AIS_Circle) anAisCircle = new AIS_Circle(aCircCurve);
  Handle(AIS_TextLabel) anAisCenterLabel = new AIS_TextLabel();
  anAisCenterLabel->SetText("  Center");
  anAisCenterLabel->SetPosition(aCircCurve->Position().Location());
  Handle(AIS_Point) anAisCenter = new AIS_Point(new Geom_CartesianPoint(aCirc.Location()));
  Handle(AIS_Axis) anAisAxis = new AIS_Axis(new Geom_Axis2Placement(aCircCurve->Position()), AIS_TOAX_ZAxis);
  myObject3d.Append(anAisCircle);
  myObject3d.Append(anAisCenterLabel);
  myObject3d.Append(anAisAxis);
}

void GeometrySamples::DumpBSplineCurveInfoSample()
{
  // Define points.
  gp_Pnt aPnt1(0.0, 0.0, 10.0);
  gp_Pnt aPnt2(5.0, 5.0, 5.0);
  gp_Pnt aPnt3(10.0, 10.0, 15.0);
  gp_Pnt aPnt4(15.0, 5.0, 20.0);

  // Add points to the curve poles array.
  TColgp_Array1OfPnt aPoles(1, 4);
  aPoles.SetValue(1, aPnt1);
  aPoles.SetValue(2, aPnt2);
  aPoles.SetValue(3, aPnt3);
  aPoles.SetValue(4, aPnt4);

  // Make a BSpline curve from the points array
  Handle(Geom_BSplineCurve) aBSplineCurve = GeomAPI_PointsToBSpline(aPoles).Curve();
  myResult << "aBSplineCurve was created:" << std::endl;
  myResult << "  Degree = " << aBSplineCurve->Degree() << std::endl;
  myResult << "  Parameter range = [ "
    << aBSplineCurve->FirstParameter() << ", "
    << aBSplineCurve->LastParameter() << " ]"
    << std::endl;
  NCollection_List<Standard_Real> aParams;
  aParams.Append(0.75 * aBSplineCurve->FirstParameter() + 0.25 * aBSplineCurve->LastParameter());
  aParams.Append(0.50 * aBSplineCurve->FirstParameter() + 0.50 * aBSplineCurve->LastParameter());
  aParams.Append(0.25 * aBSplineCurve->FirstParameter() + 0.75 * aBSplineCurve->LastParameter());
  myResult << "  Curve info:" << std::endl;
  for (NCollection_List<Standard_Real>::Iterator anIt(aParams); anIt.More(); anIt.Next())
  {
    Standard_Real aParam = anIt.Value();
    gp_Pnt aPnt;
    gp_Vec aVec;
    aBSplineCurve->D1(aParam, aPnt, aVec);
    myResult << "    Param = " << aParam << std::endl;
    myResult << "        P = [ " << aPnt.X() << ", " << aPnt.Y() << ", " << aPnt.Z() << " ]" << std::endl;
    myResult << "        D = [ " << aVec.X() << ", " << aVec.Y() << ", " << aVec.Z() << " ]" << std::endl;
    myObject3d.Append(new AIS_Point(new Geom_CartesianPoint(aPnt)));
    Handle(AIS_TextLabel) anAisCenterLabel = new AIS_TextLabel();
    Standard_SStream aSS;
    aSS << "P [" << aPnt.X() << ", " << aPnt.Y() << ", " << aPnt.Z() << "]" << std::endl;
    aSS << "D [" << aVec.X() << ", " << aVec.Y() << ", " << aVec.Z() << "]" << std::endl;
    anAisCenterLabel->SetText(aSS.str().c_str());
    anAisCenterLabel->SetPosition(aPnt);
    myObject3d.Append(anAisCenterLabel);
    Handle(AIS_Axis) anAisD = new AIS_Axis(new Geom_Axis1Placement(gp_Ax1(aPnt, aVec)));
    myObject3d.Append(anAisD);
  }

  Handle(AIS_ColoredShape) anAisBSplineCurve = new AIS_ColoredShape (BRepBuilderAPI_MakeEdge(aBSplineCurve).Shape());
  anAisBSplineCurve->SetColor(Quantity_Color(Quantity_NOC_RED));
  myObject3d.Append(anAisBSplineCurve);
}
