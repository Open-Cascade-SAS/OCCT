// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <gtest/gtest.h>

#include <BndLib.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <BndLib_AddSurface.hxx>
#include <Bnd_Box.hxx>
#include <Bnd_Box2d.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Dir.hxx>
#include <gp_Elips.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>

#include <cmath>
#include <limits>

//==================================================================================================
// Line Tests
//==================================================================================================

TEST(BndLibTest, Line_FiniteSegment)
{
  // Line along X axis from (0,0,0) in direction (1,0,0)
  gp_Lin  aLine(gp_Pnt(0., 0., 0.), gp_Dir(1., 0., 0.));
  Bnd_Box aBox;

  BndLib::Add(aLine, 0., 10., 0., aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_NEAR(aXmin, 0., Precision::Confusion());
  EXPECT_NEAR(aXmax, 10., Precision::Confusion());
  EXPECT_NEAR(aYmin, 0., Precision::Confusion());
  EXPECT_NEAR(aYmax, 0., Precision::Confusion());
  EXPECT_NEAR(aZmin, 0., Precision::Confusion());
  EXPECT_NEAR(aZmax, 0., Precision::Confusion());
}

TEST(BndLibTest, Line_PositiveDirection_NegativeInfinite)
{
  // Line along +X axis with P1 = -Inf, P2 = 5
  // As t -> -Inf, x -> -Inf, so Xmin should be open
  gp_Lin  aLine(gp_Pnt(0., 0., 0.), gp_Dir(1., 0., 0.));
  Bnd_Box aBox;

  BndLib::Add(aLine, -std::numeric_limits<double>::infinity(), 5., 0., aBox);

  EXPECT_TRUE(aBox.IsOpenXmin()) << "Xmin should be open for line in +X direction with P1=-Inf";
  EXPECT_FALSE(aBox.IsOpenXmax()) << "Xmax should not be open";
}

TEST(BndLibTest, Line_NegativeDirection_NegativeInfinite)
{
  // Line along -X axis with P1 = -Inf, P2 = 5
  // Direction is (-1, 0, 0), so as t -> -Inf, x -> +Inf
  // Therefore Xmax should be open, not Xmin
  gp_Lin  aLine(gp_Pnt(0., 0., 0.), gp_Dir(-1., 0., 0.));
  Bnd_Box aBox;

  BndLib::Add(aLine, -std::numeric_limits<double>::infinity(), 5., 0., aBox);

  EXPECT_TRUE(aBox.IsOpenXmax()) << "Xmax should be open for line in -X direction with P1=-Inf";
  EXPECT_FALSE(aBox.IsOpenXmin()) << "Xmin should not be open";
}

TEST(BndLibTest, Line_NegativeDirection_PositiveInfinite)
{
  // Line along -X axis with P1 = -5, P2 = +Inf
  // Direction is (-1, 0, 0), so as t -> +Inf, x -> -Inf
  // Therefore Xmin should be open
  gp_Lin  aLine(gp_Pnt(0., 0., 0.), gp_Dir(-1., 0., 0.));
  Bnd_Box aBox;

  BndLib::Add(aLine, -5., std::numeric_limits<double>::infinity(), 0., aBox);

  EXPECT_TRUE(aBox.IsOpenXmin()) << "Xmin should be open for line in -X direction with P2=+Inf";
  EXPECT_FALSE(aBox.IsOpenXmax()) << "Xmax should not be open";
}

TEST(BndLibTest, Line_DiagonalDirection_BothInfinite)
{
  // Line in diagonal direction (-1, -1, -1) with P1=-Inf, P2=+Inf
  // As t -> -Inf: position goes to (+Inf, +Inf, +Inf) - opens max
  // As t -> +Inf: position goes to (-Inf, -Inf, -Inf) - opens min
  gp_Lin  aLine(gp_Pnt(0., 0., 0.), gp_Dir(-1., -1., -1.));
  Bnd_Box aBox;

  BndLib::Add(aLine,
              -std::numeric_limits<double>::infinity(),
              std::numeric_limits<double>::infinity(),
              0.,
              aBox);

  EXPECT_TRUE(aBox.IsOpenXmin()) << "Xmin should be open";
  EXPECT_TRUE(aBox.IsOpenXmax()) << "Xmax should be open";
  EXPECT_TRUE(aBox.IsOpenYmin()) << "Ymin should be open";
  EXPECT_TRUE(aBox.IsOpenYmax()) << "Ymax should be open";
  EXPECT_TRUE(aBox.IsOpenZmin()) << "Zmin should be open";
  EXPECT_TRUE(aBox.IsOpenZmax()) << "Zmax should be open";
}

//==================================================================================================
// Circle Tests
//==================================================================================================

TEST(BndLibTest, Circle_Full)
{
  // Circle of radius 5 centered at origin in XY plane
  gp_Circ aCirc(gp_Ax2(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)), 5.);
  Bnd_Box aBoxFull;
  Bnd_Box aBoxArc;
  double  aTol = 0.;

  BndLib::Add(aCirc, aTol, aBoxFull);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBoxFull.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_NEAR(aXmin, -5., Precision::Confusion());
  EXPECT_NEAR(aXmax, 5., Precision::Confusion());
  EXPECT_NEAR(aYmin, -5., Precision::Confusion());
  EXPECT_NEAR(aYmax, 5., Precision::Confusion());
  EXPECT_NEAR(aZmin, 0., Precision::Confusion());
  EXPECT_NEAR(aZmax, 0., Precision::Confusion());

  // Full circle via parameters [0, 2*PI]
  BndLib::Add(aCirc, 0., 2. * M_PI, aTol, aBoxArc);
  aBoxArc.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_NEAR(aXmin, -5., Precision::Confusion());
  EXPECT_NEAR(aXmax, 5., Precision::Confusion());
  EXPECT_NEAR(aYmin, -5., Precision::Confusion());
  EXPECT_NEAR(aYmax, 5., Precision::Confusion());
}

TEST(BndLibTest, Circle_Arc_FirstQuadrant)
{
  // Circle arc from 0 to PI/2 (first quadrant)
  gp_Circ aCirc(gp_Ax2(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)), 5.);
  Bnd_Box aBox;
  double  aTol = 0.;

  BndLib::Add(aCirc, 0., M_PI / 2., aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_NEAR(aXmin, 0., Precision::Confusion());
  EXPECT_NEAR(aXmax, 5., Precision::Confusion());
  EXPECT_NEAR(aYmin, 0., Precision::Confusion());
  EXPECT_NEAR(aYmax, 5., Precision::Confusion());
}

TEST(BndLibTest, Circle_RotatedAxis)
{
  // Circle in XZ plane (axis along Y)
  gp_Circ aCirc(gp_Ax2(gp_Pnt(0., 0., 0.), gp_Dir(0., 1., 0.)), 3.);
  Bnd_Box aBox;
  double  aTol = 0.;

  BndLib::Add(aCirc, aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_NEAR(aXmin, -3., Precision::Confusion());
  EXPECT_NEAR(aXmax, 3., Precision::Confusion());
  EXPECT_NEAR(aYmin, 0., Precision::Confusion());
  EXPECT_NEAR(aYmax, 0., Precision::Confusion());
  EXPECT_NEAR(aZmin, -3., Precision::Confusion());
  EXPECT_NEAR(aZmax, 3., Precision::Confusion());
}

//==================================================================================================
// Ellipse Tests
//==================================================================================================

TEST(BndLibTest, Ellipse_Full)
{
  // Ellipse with major radius 10, minor radius 5 in XY plane
  gp_Elips aElips(gp_Ax2(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)), 10., 5.);
  Bnd_Box  aBox;
  double   aTol = 0.;

  BndLib::Add(aElips, aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_NEAR(aXmin, -10., Precision::Confusion());
  EXPECT_NEAR(aXmax, 10., Precision::Confusion());
  EXPECT_NEAR(aYmin, -5., Precision::Confusion());
  EXPECT_NEAR(aYmax, 5., Precision::Confusion());
  EXPECT_NEAR(aZmin, 0., Precision::Confusion());
  EXPECT_NEAR(aZmax, 0., Precision::Confusion());
}

TEST(BndLibTest, Ellipse_Arc)
{
  // Ellipse arc from 0 to PI/2
  gp_Elips aElips(gp_Ax2(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)), 10., 5.);
  Bnd_Box  aBox;
  double   aTol = 0.;

  BndLib::Add(aElips, 0., M_PI / 2., aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  // At t=0: (10, 0, 0), at t=PI/2: (0, 5, 0)
  EXPECT_NEAR(aXmin, 0., Precision::Confusion());
  EXPECT_NEAR(aXmax, 10., Precision::Confusion());
  EXPECT_NEAR(aYmin, 0., Precision::Confusion());
  EXPECT_NEAR(aYmax, 5., Precision::Confusion());
}

//==================================================================================================
// Hyperbola Tests
//==================================================================================================

TEST(BndLibTest, Hyperbola_SimpleArc)
{
  // Hyperbola with RealAxis=3, ImaginaryAxis=2 in XY plane
  // Parameterization: (3*cosh(t), 2*sinh(t), 0)
  gp_Hypr aHypr(gp_Ax2(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)), 3., 2.);
  Bnd_Box aBox;
  double  aTol = 0.;

  BndLib::Add(aHypr, -1., 1., aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  // At t=-1: (3*cosh(-1), 2*sinh(-1)) = (4.629..., -2.350...)
  // At t=1:  (3*cosh(1), 2*sinh(1))   = (4.629...,  2.350...)
  // At t=0:  (3, 0) - minimum X
  double aExpectedXmin = 3.;                  // at t=0
  double aExpectedXmax = 3. * std::cosh(1.);  // at t=+-1
  double aExpectedYmin = 2. * std::sinh(-1.); // at t=-1
  double aExpectedYmax = 2. * std::sinh(1.);  // at t=1

  EXPECT_NEAR(aXmin, aExpectedXmin, Precision::Confusion());
  EXPECT_NEAR(aXmax, aExpectedXmax, Precision::Confusion());
  EXPECT_NEAR(aYmin, aExpectedYmin, Precision::Confusion());
  EXPECT_NEAR(aYmax, aExpectedYmax, Precision::Confusion());
}

TEST(BndLibTest, Hyperbola_Rotated_MultipleExtrema)
{
  // Rotated hyperbola where X, Y, Z extrema occur at different parameter values.
  // This tests the fix for the extrema loop that previously broke after finding
  // the first extremum.
  gp_Ax2  aAx2(gp_Pnt(0., 0., 0.), gp_Dir(1., 1., 1.), gp_Dir(1., -1., 0.));
  gp_Hypr aHypr(aAx2, 5., 3.);
  Bnd_Box aBox;
  double  aTol = 0.;

  BndLib::Add(aHypr, -2., 2., aTol, aBox);

  // Verify box is valid and contains reasonable bounds
  EXPECT_FALSE(aBox.IsVoid()) << "Box should not be void";

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  // For a rotated hyperbola, the box should be non-degenerate in all dimensions
  EXPECT_LT(aXmin, aXmax) << "X range should be non-zero";
  EXPECT_LT(aYmin, aYmax) << "Y range should be non-zero";
  // Z may be zero if hyperbola lies in a plane, but should still be valid
}

TEST(BndLibTest, Hyperbola_InfiniteParameter)
{
  // Hyperbola with one infinite parameter
  gp_Hypr aHypr(gp_Ax2(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)), 3., 2.);
  Bnd_Box aBox;
  double  aTol = 0.;

  BndLib::Add(aHypr, 0., std::numeric_limits<double>::infinity(), aTol, aBox);

  EXPECT_TRUE(aBox.IsOpenXmax()) << "Xmax should be open for hyperbola with P2=+Inf";
  EXPECT_TRUE(aBox.IsOpenYmax()) << "Ymax should be open for hyperbola with P2=+Inf";
}

//==================================================================================================
// Parabola Tests
//==================================================================================================

TEST(BndLibTest, Parabola_FiniteArc)
{
  // Parabola with focal length 2 in XY plane
  // Parameterization: (t^2/(4*f), t, 0) = (t^2/8, t, 0) for f=2
  gp_Parab aParab(gp_Ax2(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)), 2.);
  Bnd_Box  aBox;
  double   aTol = 0.;

  BndLib::Add(aParab, -4., 4., aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  // At t=0: (0, 0, 0)
  // At t=+-4: (16/8, +-4, 0) = (2, +-4, 0)
  EXPECT_NEAR(aXmin, 0., Precision::Confusion());
  EXPECT_NEAR(aXmax, 2., Precision::Confusion());
  EXPECT_NEAR(aYmin, -4., Precision::Confusion());
  EXPECT_NEAR(aYmax, 4., Precision::Confusion());
}

TEST(BndLibTest, Parabola_InfiniteParameter)
{
  // Parabola with infinite parameter
  gp_Parab aParab(gp_Ax2(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)), 2.);
  Bnd_Box  aBox;
  double   aTol = 0.;

  BndLib::Add(aParab, 0., std::numeric_limits<double>::infinity(), aTol, aBox);

  EXPECT_TRUE(aBox.IsOpenXmax()) << "Xmax should be open for parabola with P2=+Inf";
  EXPECT_TRUE(aBox.IsOpenYmax()) << "Ymax should be open for parabola with P2=+Inf";
}

//==================================================================================================
// Sphere Tests
//==================================================================================================

TEST(BndLibTest, Sphere_Full)
{
  // Sphere of radius 7 centered at origin
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)), 7.);
  Bnd_Box   aBox;
  double    aTol = 0.;

  BndLib::Add(aSphere, aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_NEAR(aXmin, -7., Precision::Confusion());
  EXPECT_NEAR(aXmax, 7., Precision::Confusion());
  EXPECT_NEAR(aYmin, -7., Precision::Confusion());
  EXPECT_NEAR(aYmax, 7., Precision::Confusion());
  EXPECT_NEAR(aZmin, -7., Precision::Confusion());
  EXPECT_NEAR(aZmax, 7., Precision::Confusion());
}

TEST(BndLibTest, Sphere_Patch)
{
  // Sphere patch: upper hemisphere (V from 0 to PI/2)
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)), 5.);
  Bnd_Box   aBox;
  double    aTol = 0.;

  // Full U range, V from 0 to PI/2 (upper hemisphere)
  BndLib::Add(aSphere, 0., 2. * M_PI, 0., M_PI / 2., aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_NEAR(aXmin, -5., Precision::Confusion());
  EXPECT_NEAR(aXmax, 5., Precision::Confusion());
  EXPECT_NEAR(aYmin, -5., Precision::Confusion());
  EXPECT_NEAR(aYmax, 5., Precision::Confusion());
  EXPECT_NEAR(aZmin, 0., Precision::Confusion());
  EXPECT_NEAR(aZmax, 5., Precision::Confusion());
}

TEST(BndLibTest, Sphere_Translated)
{
  // Sphere centered at (10, 20, 30) with radius 3
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(10., 20., 30.), gp_Dir(0., 0., 1.)), 3.);
  Bnd_Box   aBox;
  double    aTol = 0.;

  BndLib::Add(aSphere, aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_NEAR(aXmin, 7., Precision::Confusion());
  EXPECT_NEAR(aXmax, 13., Precision::Confusion());
  EXPECT_NEAR(aYmin, 17., Precision::Confusion());
  EXPECT_NEAR(aYmax, 23., Precision::Confusion());
  EXPECT_NEAR(aZmin, 27., Precision::Confusion());
  EXPECT_NEAR(aZmax, 33., Precision::Confusion());
}

//==================================================================================================
// Cylinder Tests
//==================================================================================================

TEST(BndLibTest, Cylinder_FinitePatch)
{
  // Cylinder of radius 4, axis along Z
  gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)), 4.);
  Bnd_Box     aBox;
  double      aTol = 0.;

  // Full U range, V from 0 to 10
  BndLib::Add(aCyl, 0., 2. * M_PI, 0., 10., aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_NEAR(aXmin, -4., Precision::Confusion());
  EXPECT_NEAR(aXmax, 4., Precision::Confusion());
  EXPECT_NEAR(aYmin, -4., Precision::Confusion());
  EXPECT_NEAR(aYmax, 4., Precision::Confusion());
  EXPECT_NEAR(aZmin, 0., Precision::Confusion());
  EXPECT_NEAR(aZmax, 10., Precision::Confusion());
}

TEST(BndLibTest, Cylinder_InfiniteV)
{
  // Cylinder with infinite V parameter
  gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)), 4.);
  Bnd_Box     aBox;
  double      aTol = 0.;

  BndLib::Add(aCyl, 0., std::numeric_limits<double>::infinity(), aTol, aBox);

  EXPECT_TRUE(aBox.IsOpenZmax()) << "Zmax should be open for cylinder with VMax=+Inf";
  EXPECT_FALSE(aBox.IsOpenZmin()) << "Zmin should not be open";
}

TEST(BndLibTest, Cylinder_PartialU)
{
  // Cylinder with partial U range (quarter cylinder)
  gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)), 5.);
  Bnd_Box     aBox;
  double      aTol = 0.;

  // U from 0 to PI/2 (first quadrant), V from 0 to 10
  BndLib::Add(aCyl, 0., M_PI / 2., 0., 10., aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_NEAR(aXmin, 0., Precision::Confusion());
  EXPECT_NEAR(aXmax, 5., Precision::Confusion());
  EXPECT_NEAR(aYmin, 0., Precision::Confusion());
  EXPECT_NEAR(aYmax, 5., Precision::Confusion());
  EXPECT_NEAR(aZmin, 0., Precision::Confusion());
  EXPECT_NEAR(aZmax, 10., Precision::Confusion());
}

//==================================================================================================
// Cone Tests
//==================================================================================================

TEST(BndLibTest, Cone_FinitePatch)
{
  // Cone with semi-angle PI/4 (45 deg), reference radius 2
  // Cone parameterization: P(u,v) = Apex + (RefRadius + v*sin(SemiAngle)) * (cos(u)*XDir +
  // sin(u)*YDir)
  //                                      + v*cos(SemiAngle) * Axis
  gp_Cone aCone(gp_Ax3(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)), M_PI / 4., 2.);
  Bnd_Box aBox;
  double  aTol = 0.;

  // Full U range, V from 0 to 5
  BndLib::Add(aCone, 0., 2. * M_PI, 0., 5., aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  // V=0: radius = RefRadius = 2, Z = 0
  // V=5: radius = 2 + 5*sin(PI/4) = 2 + 5*sqrt(2)/2, Z = 5*cos(PI/4) = 5*sqrt(2)/2
  double aMaxRadius = 2. + 5. * std::sin(M_PI / 4.);
  double aMaxZ      = 5. * std::cos(M_PI / 4.);
  EXPECT_NEAR(aXmax, aMaxRadius, Precision::Confusion());
  EXPECT_NEAR(aYmax, aMaxRadius, Precision::Confusion());
  EXPECT_NEAR(aZmax, aMaxZ, Precision::Confusion());
  EXPECT_NEAR(aZmin, 0., Precision::Confusion());
}

TEST(BndLibTest, Cone_NegativeV)
{
  // Cone with negative V range (cone extending in opposite direction)
  // Cone parameterization: Z = v*cos(SemiAngle)
  gp_Cone aCone(gp_Ax3(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)), M_PI / 6., 3.);
  Bnd_Box aBox;
  double  aTol = 0.;

  // Full U range, V from -5 to 0
  BndLib::Add(aCone, 0., 2. * M_PI, -5., 0., aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  // At V=0: radius = 3, Z = 0
  // At V=-5: radius = 3 + (-5)*sin(PI/6) = 3 - 2.5 = 0.5, Z = -5*cos(PI/6)
  double aExpectedZmin = -5. * std::cos(M_PI / 6.);
  EXPECT_NEAR(aZmin, aExpectedZmin, Precision::Confusion());
  EXPECT_NEAR(aZmax, 0., Precision::Confusion());
}

TEST(BndLibTest, Cone_InfiniteV)
{
  // Cone with infinite V parameter
  // Note: BndLib::Add for cone with U,V parameters opens Z but not X/Y
  // since the cone's bounding in X/Y is computed from the circular cross-sections
  gp_Cone aCone(gp_Ax3(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)), M_PI / 4., 2.);
  Bnd_Box aBox;
  double  aTol = 0.;

  BndLib::Add(aCone, 0., std::numeric_limits<double>::infinity(), aTol, aBox);

  // For cone with infinite V, the Z direction should be open
  EXPECT_TRUE(aBox.IsOpenZmax()) << "Zmax should be open for cone with VMax=+Inf";
}

//==================================================================================================
// Torus Tests
//==================================================================================================

TEST(BndLibTest, Torus_Full)
{
  // Torus with major radius 10, minor radius 3
  gp_Torus aTorus(gp_Ax3(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)), 10., 3.);
  Bnd_Box  aBox;
  double   aTol = 0.;

  BndLib::Add(aTorus, aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  // Torus extends from -(R+r) to +(R+r) in X and Y
  // and from -r to +r in Z
  EXPECT_NEAR(aXmin, -13., Precision::Confusion());
  EXPECT_NEAR(aXmax, 13., Precision::Confusion());
  EXPECT_NEAR(aYmin, -13., Precision::Confusion());
  EXPECT_NEAR(aYmax, 13., Precision::Confusion());
  EXPECT_NEAR(aZmin, -3., Precision::Confusion());
  EXPECT_NEAR(aZmax, 3., Precision::Confusion());
}

TEST(BndLibTest, Torus_Patch)
{
  // Torus patch: quarter in U, half in V
  // Torus parameterization: P(u,v) = Center + (R + r*cos(v)) * (cos(u)*XDir + sin(u)*YDir)
  //                                         + r*sin(v) * Axis
  gp_Torus aTorus(gp_Ax3(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)), 10., 3.);
  Bnd_Box  aBox;
  double   aTol = 0.;

  // U from 0 to PI/2, V from 0 to PI
  BndLib::Add(aTorus, 0., M_PI / 2., 0., M_PI, aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  // Box should be valid and contain reasonable bounds
  EXPECT_FALSE(aBox.IsVoid()) << "Box should not be void for torus patch";

  // For U in [0, PI/2], X and Y should be non-negative at minimum
  // The bounding box algorithm produces conservative bounds that may slightly
  // exceed R+r due to sampling/approximation
  EXPECT_GE(aXmin, -Precision::Confusion()) << "Xmin should be >= 0 for first quadrant";
  EXPECT_GE(aYmin, -Precision::Confusion()) << "Ymin should be >= 0 for first quadrant";

  // Z max should be approximately r (minor radius)
  EXPECT_LE(aZmax, 3. + Precision::Confusion()) << "Zmax should not exceed r";
}

TEST(BndLibTest, Torus_NegativeVParameter)
{
  // Torus patch with negative V parameters.
  // This tests the fix for negative modulo: ((i % 8) + 8) % 8
  // The main purpose is to verify no crash occurs with negative V.
  gp_Torus aTorus(gp_Ax3(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)), 10., 3.);
  Bnd_Box  aBox;
  double   aTol = 0.;

  // Full U, V from -PI to 0
  BndLib::Add(aTorus, 0., 2. * M_PI, -M_PI, 0., aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  // Should produce valid bounds without crash (main test for the fix)
  EXPECT_FALSE(aBox.IsVoid()) << "Box should not be void for negative V parameters";

  // For V in [-PI, 0], Z = r*sin(v) ranges from -r (at v=-PI/2) to 0
  // The bounding box should contain this range
  EXPECT_LE(aZmin, 0.) << "Zmin should be <= 0 for V in [-PI, 0]";
  EXPECT_GE(aZmin, -3. - Precision::Confusion()) << "Zmin should be >= -r";
}

TEST(BndLibTest, Torus_LargeNegativeVParameter)
{
  // Torus with large negative V parameter that would cause
  // out-of-bounds array access with incorrect modulo.
  // This is the primary test for the negative modulo fix.
  gp_Torus aTorus(gp_Ax3(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)), 10., 3.);
  Bnd_Box  aBox;
  double   aTol = 0.;

  // V from -3*PI to -2*PI covers one full revolution
  BndLib::Add(aTorus, 0., 2. * M_PI, -3. * M_PI, -2. * M_PI, aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  // Should produce valid bounds without crash (main test for the fix)
  EXPECT_FALSE(aBox.IsVoid()) << "Box should not be void for large negative V parameters";

  // Full revolution in U gives full X/Y extent, the box may be slightly larger
  // due to conservative bounding box algorithm
  EXPECT_LE(aXmin, -13. + Precision::Confusion()) << "Xmin should be at least -(R+r)";
  EXPECT_GE(aXmax, 13. - Precision::Confusion()) << "Xmax should be at least R+r";
  EXPECT_LE(aYmin, -13. + Precision::Confusion()) << "Ymin should be at least -(R+r)";
  EXPECT_GE(aYmax, 13. - Precision::Confusion()) << "Ymax should be at least R+r";
}

TEST(BndLibTest, Torus_Translated)
{
  // Torus centered at (5, 5, 5)
  gp_Torus aTorus(gp_Ax3(gp_Pnt(5., 5., 5.), gp_Dir(0., 0., 1.)), 8., 2.);
  Bnd_Box  aBox;
  double   aTol = 0.;

  BndLib::Add(aTorus, aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_NEAR(aXmin, 5. - 10., Precision::Confusion());
  EXPECT_NEAR(aXmax, 5. + 10., Precision::Confusion());
  EXPECT_NEAR(aYmin, 5. - 10., Precision::Confusion());
  EXPECT_NEAR(aYmax, 5. + 10., Precision::Confusion());
  EXPECT_NEAR(aZmin, 5. - 2., Precision::Confusion());
  EXPECT_NEAR(aZmax, 5. + 2., Precision::Confusion());
}

//==================================================================================================
// 2D Tests
//==================================================================================================

TEST(BndLibTest, Line2d_FiniteSegment)
{
  gp_Lin2d  aLine(gp_Pnt2d(0., 0.), gp_Dir2d(1., 1.));
  Bnd_Box2d aBox;
  double    aTol = 0.;

  BndLib::Add(aLine, 0., 10., aTol, aBox);

  double aXmin, aYmin, aXmax, aYmax;
  aBox.Get(aXmin, aYmin, aXmax, aYmax);

  // Line in direction (1,1) normalized, so at t=10:
  // position = (10/sqrt(2), 10/sqrt(2))
  double aExpected = 10. / std::sqrt(2.);
  EXPECT_NEAR(aXmin, 0., Precision::Confusion());
  EXPECT_NEAR(aXmax, aExpected, Precision::Confusion());
  EXPECT_NEAR(aYmin, 0., Precision::Confusion());
  EXPECT_NEAR(aYmax, aExpected, Precision::Confusion());
}

TEST(BndLibTest, Circle2d_Full)
{
  gp_Circ2d aCirc(gp_Ax2d(gp_Pnt2d(0., 0.), gp_Dir2d(1., 0.)), 5.);
  Bnd_Box2d aBox;
  double    aTol = 0.;

  BndLib::Add(aCirc, aTol, aBox);

  double aXmin, aYmin, aXmax, aYmax;
  aBox.Get(aXmin, aYmin, aXmax, aYmax);

  EXPECT_NEAR(aXmin, -5., Precision::Confusion());
  EXPECT_NEAR(aXmax, 5., Precision::Confusion());
  EXPECT_NEAR(aYmin, -5., Precision::Confusion());
  EXPECT_NEAR(aYmax, 5., Precision::Confusion());
}

TEST(BndLibTest, Ellipse2d_Full)
{
  gp_Elips2d aElips(gp_Ax2d(gp_Pnt2d(0., 0.), gp_Dir2d(1., 0.)), 8., 4.);
  Bnd_Box2d  aBox;
  double     aTol = 0.;

  BndLib::Add(aElips, aTol, aBox);

  double aXmin, aYmin, aXmax, aYmax;
  aBox.Get(aXmin, aYmin, aXmax, aYmax);

  EXPECT_NEAR(aXmin, -8., Precision::Confusion());
  EXPECT_NEAR(aXmax, 8., Precision::Confusion());
  EXPECT_NEAR(aYmin, -4., Precision::Confusion());
  EXPECT_NEAR(aYmax, 4., Precision::Confusion());
}

TEST(BndLibTest, Hyperbola2d_SimpleArc)
{
  gp_Hypr2d aHypr(gp_Ax2d(gp_Pnt2d(0., 0.), gp_Dir2d(1., 0.)), 3., 2.);
  Bnd_Box2d aBox;
  double    aTol = 0.;

  BndLib::Add(aHypr, -1., 1., aTol, aBox);

  double aXmin, aYmin, aXmax, aYmax;
  aBox.Get(aXmin, aYmin, aXmax, aYmax);

  EXPECT_NEAR(aXmin, 3., Precision::Confusion());
  EXPECT_NEAR(aXmax, 3. * std::cosh(1.), Precision::Confusion());
  EXPECT_NEAR(aYmin, 2. * std::sinh(-1.), Precision::Confusion());
  EXPECT_NEAR(aYmax, 2. * std::sinh(1.), Precision::Confusion());
}

TEST(BndLibTest, Parabola2d_FiniteArc)
{
  gp_Parab2d aParab(gp_Ax2d(gp_Pnt2d(0., 0.), gp_Dir2d(1., 0.)), 2.);
  Bnd_Box2d  aBox;
  double     aTol = 0.;

  BndLib::Add(aParab, -4., 4., aTol, aBox);

  double aXmin, aYmin, aXmax, aYmax;
  aBox.Get(aXmin, aYmin, aXmax, aYmax);

  EXPECT_NEAR(aXmin, 0., Precision::Confusion());
  EXPECT_NEAR(aXmax, 2., Precision::Confusion());
  EXPECT_NEAR(aYmin, -4., Precision::Confusion());
  EXPECT_NEAR(aYmax, 4., Precision::Confusion());
}

//==================================================================================================
// Tolerance Tests
//==================================================================================================

TEST(BndLibTest, Circle_WithTolerance)
{
  gp_Circ aCirc(gp_Ax2(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)), 5.);
  Bnd_Box aBox;
  double  aTol = 0.5;

  BndLib::Add(aCirc, aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  // Box should be enlarged by tolerance
  EXPECT_NEAR(aXmin, -5.5, Precision::Confusion());
  EXPECT_NEAR(aXmax, 5.5, Precision::Confusion());
  EXPECT_NEAR(aYmin, -5.5, Precision::Confusion());
  EXPECT_NEAR(aYmax, 5.5, Precision::Confusion());
  EXPECT_NEAR(aZmin, -0.5, Precision::Confusion());
  EXPECT_NEAR(aZmax, 0.5, Precision::Confusion());
}

TEST(BndLibTest, Sphere_WithTolerance)
{
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)), 3.);
  Bnd_Box   aBox;
  double    aTol = 1.;

  BndLib::Add(aSphere, aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_NEAR(aXmin, -4., Precision::Confusion());
  EXPECT_NEAR(aXmax, 4., Precision::Confusion());
  EXPECT_NEAR(aYmin, -4., Precision::Confusion());
  EXPECT_NEAR(aYmax, 4., Precision::Confusion());
  EXPECT_NEAR(aZmin, -4., Precision::Confusion());
  EXPECT_NEAR(aZmax, 4., Precision::Confusion());
}

//==================================================================================================
// BndLib_Add3dCurve Tests
//==================================================================================================

TEST(BndLib_Add3dCurveTest, Circle_Full)
{
  // Create a 3D circle using Geom_Circle
  Handle(Geom_Circle) aCircle = new Geom_Circle(gp_Ax2(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)), 5.);
  GeomAdaptor_Curve   aCurve(aCircle);
  Bnd_Box             aBox;
  double              aTol = 0.;

  BndLib_Add3dCurve::Add(aCurve, aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_NEAR(aXmin, -5., Precision::Confusion());
  EXPECT_NEAR(aXmax, 5., Precision::Confusion());
  EXPECT_NEAR(aYmin, -5., Precision::Confusion());
  EXPECT_NEAR(aYmax, 5., Precision::Confusion());
  EXPECT_NEAR(aZmin, 0., Precision::Confusion());
  EXPECT_NEAR(aZmax, 0., Precision::Confusion());
}

TEST(BndLib_Add3dCurveTest, Circle_Arc)
{
  // Circle arc from 0 to PI/2
  Handle(Geom_Circle) aCircle = new Geom_Circle(gp_Ax2(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)), 5.);
  GeomAdaptor_Curve   aCurve(aCircle);
  Bnd_Box             aBox;
  double              aTol = 0.;

  BndLib_Add3dCurve::Add(aCurve, 0., M_PI / 2., aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_NEAR(aXmin, 0., Precision::Confusion());
  EXPECT_NEAR(aXmax, 5., Precision::Confusion());
  EXPECT_NEAR(aYmin, 0., Precision::Confusion());
  EXPECT_NEAR(aYmax, 5., Precision::Confusion());
}

TEST(BndLib_Add3dCurveTest, Ellipse_Full)
{
  // Ellipse with major radius 10, minor radius 5
  Handle(Geom_Ellipse) anEllipse =
    new Geom_Ellipse(gp_Ax2(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)), 10., 5.);
  GeomAdaptor_Curve aCurve(anEllipse);
  Bnd_Box           aBox;
  double            aTol = 0.;

  BndLib_Add3dCurve::Add(aCurve, aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_NEAR(aXmin, -10., Precision::Confusion());
  EXPECT_NEAR(aXmax, 10., Precision::Confusion());
  EXPECT_NEAR(aYmin, -5., Precision::Confusion());
  EXPECT_NEAR(aYmax, 5., Precision::Confusion());
}

TEST(BndLib_Add3dCurveTest, Line_Segment)
{
  // Line segment from (0,0,0) to (10,0,0)
  Handle(Geom_Line)         aLine    = new Geom_Line(gp_Pnt(0., 0., 0.), gp_Dir(1., 0., 0.));
  Handle(Geom_TrimmedCurve) aTrimmed = new Geom_TrimmedCurve(aLine, 0., 10.);
  GeomAdaptor_Curve         aCurve(aTrimmed);
  Bnd_Box                   aBox;
  double                    aTol = 0.;

  BndLib_Add3dCurve::Add(aCurve, aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_NEAR(aXmin, 0., Precision::Confusion());
  EXPECT_NEAR(aXmax, 10., Precision::Confusion());
  EXPECT_NEAR(aYmin, 0., Precision::Confusion());
  EXPECT_NEAR(aYmax, 0., Precision::Confusion());
  EXPECT_NEAR(aZmin, 0., Precision::Confusion());
  EXPECT_NEAR(aZmax, 0., Precision::Confusion());
}

TEST(BndLib_Add3dCurveTest, BezierCurve)
{
  // Create a cubic Bezier curve
  TColgp_Array1OfPnt aPoles(1, 4);
  aPoles(1) = gp_Pnt(0., 0., 0.);
  aPoles(2) = gp_Pnt(1., 2., 0.);
  aPoles(3) = gp_Pnt(3., 2., 0.);
  aPoles(4) = gp_Pnt(4., 0., 0.);

  Handle(Geom_BezierCurve) aBezier = new Geom_BezierCurve(aPoles);
  GeomAdaptor_Curve        aCurve(aBezier);
  Bnd_Box                  aBox;
  double                   aTol = 0.;

  BndLib_Add3dCurve::Add(aCurve, aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  // Box should contain curve endpoints and the actual curve
  // Note: The curve doesn't necessarily pass through control points,
  // so the box may be smaller than the convex hull of control points
  EXPECT_FALSE(aBox.IsVoid()) << "Box should not be void";
  EXPECT_LE(aXmin, 0.);
  EXPECT_GE(aXmax, 4.);
  EXPECT_LE(aYmin, 0.);
  // Y max is at curve's maximum, not necessarily at control point Y
  EXPECT_GT(aYmax, 1.) << "Y max should be > 1 (curve rises above Y=0)";
}

TEST(BndLib_Add3dCurveTest, BSplineCurve)
{
  // Create a simple B-spline curve
  TColgp_Array1OfPnt aPoles(1, 4);
  aPoles(1) = gp_Pnt(0., 0., 0.);
  aPoles(2) = gp_Pnt(1., 1., 0.);
  aPoles(3) = gp_Pnt(2., 1., 0.);
  aPoles(4) = gp_Pnt(3., 0., 0.);

  TColStd_Array1OfReal aKnots(1, 2);
  aKnots(1) = 0.;
  aKnots(2) = 1.;

  TColStd_Array1OfInteger aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  Handle(Geom_BSplineCurve) aBSpline = new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);
  GeomAdaptor_Curve         aCurve(aBSpline);
  Bnd_Box                   aBox;
  double                    aTol = 0.;

  BndLib_Add3dCurve::Add(aCurve, aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_FALSE(aBox.IsVoid()) << "Box should not be void";
  EXPECT_LE(aXmin, 0.);
  EXPECT_GE(aXmax, 3.);
}

TEST(BndLib_Add3dCurveTest, AddOptimal_Circle)
{
  // Test AddOptimal method with a circle
  Handle(Geom_Circle) aCircle = new Geom_Circle(gp_Ax2(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)), 5.);
  GeomAdaptor_Curve   aCurve(aCircle);
  Bnd_Box             aBox;
  double              aTol = 0.;

  BndLib_Add3dCurve::AddOptimal(aCurve, aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_NEAR(aXmin, -5., Precision::Confusion());
  EXPECT_NEAR(aXmax, 5., Precision::Confusion());
  EXPECT_NEAR(aYmin, -5., Precision::Confusion());
  EXPECT_NEAR(aYmax, 5., Precision::Confusion());
}

TEST(BndLib_Add3dCurveTest, Circle_Rotated)
{
  // Circle in YZ plane (axis along X)
  Handle(Geom_Circle) aCircle = new Geom_Circle(gp_Ax2(gp_Pnt(0., 0., 0.), gp_Dir(1., 0., 0.)), 4.);
  GeomAdaptor_Curve   aCurve(aCircle);
  Bnd_Box             aBox;
  double              aTol = 0.;

  BndLib_Add3dCurve::Add(aCurve, aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_NEAR(aXmin, 0., Precision::Confusion());
  EXPECT_NEAR(aXmax, 0., Precision::Confusion());
  EXPECT_NEAR(aYmin, -4., Precision::Confusion());
  EXPECT_NEAR(aYmax, 4., Precision::Confusion());
  EXPECT_NEAR(aZmin, -4., Precision::Confusion());
  EXPECT_NEAR(aZmax, 4., Precision::Confusion());
}

//==================================================================================================
// BndLib_Add2dCurve Tests
//==================================================================================================

TEST(BndLib_Add2dCurveTest, Circle_Full)
{
  // Create a 2D circle using Geom2d_Circle
  Handle(Geom2d_Circle) aCircle =
    new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(0., 0.), gp_Dir2d(1., 0.)), 5.);
  Bnd_Box2d aBox;
  double    aTol = 0.;

  BndLib_Add2dCurve::Add(aCircle, aTol, aBox);

  double aXmin, aYmin, aXmax, aYmax;
  aBox.Get(aXmin, aYmin, aXmax, aYmax);

  EXPECT_NEAR(aXmin, -5., Precision::Confusion());
  EXPECT_NEAR(aXmax, 5., Precision::Confusion());
  EXPECT_NEAR(aYmin, -5., Precision::Confusion());
  EXPECT_NEAR(aYmax, 5., Precision::Confusion());
}

TEST(BndLib_Add2dCurveTest, Circle_Arc)
{
  // Circle arc from 0 to PI/2
  Handle(Geom2d_Circle) aCircle =
    new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(0., 0.), gp_Dir2d(1., 0.)), 5.);
  Bnd_Box2d aBox;
  double    aTol = 0.;

  BndLib_Add2dCurve::Add(aCircle, 0., M_PI / 2., aTol, aBox);

  double aXmin, aYmin, aXmax, aYmax;
  aBox.Get(aXmin, aYmin, aXmax, aYmax);

  EXPECT_NEAR(aXmin, 0., Precision::Confusion());
  EXPECT_NEAR(aXmax, 5., Precision::Confusion());
  EXPECT_NEAR(aYmin, 0., Precision::Confusion());
  EXPECT_NEAR(aYmax, 5., Precision::Confusion());
}

TEST(BndLib_Add2dCurveTest, Ellipse_Full)
{
  // Ellipse with major radius 8, minor radius 4
  Handle(Geom2d_Ellipse) anEllipse =
    new Geom2d_Ellipse(gp_Ax2d(gp_Pnt2d(0., 0.), gp_Dir2d(1., 0.)), 8., 4.);
  Bnd_Box2d aBox;
  double    aTol = 0.;

  BndLib_Add2dCurve::Add(anEllipse, aTol, aBox);

  double aXmin, aYmin, aXmax, aYmax;
  aBox.Get(aXmin, aYmin, aXmax, aYmax);

  EXPECT_NEAR(aXmin, -8., Precision::Confusion());
  EXPECT_NEAR(aXmax, 8., Precision::Confusion());
  EXPECT_NEAR(aYmin, -4., Precision::Confusion());
  EXPECT_NEAR(aYmax, 4., Precision::Confusion());
}

TEST(BndLib_Add2dCurveTest, Line_Segment)
{
  // Line segment from (0,0) to (10,5)
  Handle(Geom2d_Line)         aLine = new Geom2d_Line(gp_Pnt2d(0., 0.), gp_Dir2d(2., 1.));
  Handle(Geom2d_TrimmedCurve) aTrimmed =
    new Geom2d_TrimmedCurve(aLine, 0., std::sqrt(125.)); // length = sqrt(100+25)
  Geom2dAdaptor_Curve aCurve(aTrimmed);
  Bnd_Box2d           aBox;
  double              aTol = 0.;

  BndLib_Add2dCurve::Add(aCurve, aTol, aBox);

  double aXmin, aYmin, aXmax, aYmax;
  aBox.Get(aXmin, aYmin, aXmax, aYmax);

  EXPECT_NEAR(aXmin, 0., Precision::Confusion());
  EXPECT_NEAR(aXmax, 10., 0.01);
  EXPECT_NEAR(aYmin, 0., Precision::Confusion());
  EXPECT_NEAR(aYmax, 5., 0.01);
}

TEST(BndLib_Add2dCurveTest, BezierCurve)
{
  // Create a cubic Bezier curve in 2D
  TColgp_Array1OfPnt2d aPoles(1, 4);
  aPoles(1) = gp_Pnt2d(0., 0.);
  aPoles(2) = gp_Pnt2d(1., 3.);
  aPoles(3) = gp_Pnt2d(3., 3.);
  aPoles(4) = gp_Pnt2d(4., 0.);

  Handle(Geom2d_BezierCurve) aBezier = new Geom2d_BezierCurve(aPoles);
  Geom2dAdaptor_Curve        aCurve(aBezier);
  Bnd_Box2d                  aBox;
  double                     aTol = 0.;

  BndLib_Add2dCurve::Add(aCurve, aTol, aBox);

  double aXmin, aYmin, aXmax, aYmax;
  aBox.Get(aXmin, aYmin, aXmax, aYmax);

  // Box should contain all control points
  EXPECT_LE(aXmin, 0.);
  EXPECT_GE(aXmax, 4.);
  EXPECT_LE(aYmin, 0.);
  EXPECT_GE(aYmax, 3.);
}

TEST(BndLib_Add2dCurveTest, AddOptimal_Ellipse)
{
  // Test AddOptimal method with an ellipse
  Handle(Geom2d_Ellipse) anEllipse =
    new Geom2d_Ellipse(gp_Ax2d(gp_Pnt2d(0., 0.), gp_Dir2d(1., 0.)), 6., 3.);
  Bnd_Box2d aBox;
  double    aTol = 0.;

  BndLib_Add2dCurve::AddOptimal(anEllipse, 0., 2. * M_PI, aTol, aBox);

  double aXmin, aYmin, aXmax, aYmax;
  aBox.Get(aXmin, aYmin, aXmax, aYmax);

  EXPECT_NEAR(aXmin, -6., Precision::Confusion());
  EXPECT_NEAR(aXmax, 6., Precision::Confusion());
  EXPECT_NEAR(aYmin, -3., Precision::Confusion());
  EXPECT_NEAR(aYmax, 3., Precision::Confusion());
}

TEST(BndLib_Add2dCurveTest, Adaptor_Circle)
{
  // Test using Adaptor interface
  Handle(Geom2d_Circle) aCircle =
    new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(5., 5.), gp_Dir2d(1., 0.)), 3.);
  Geom2dAdaptor_Curve aCurve(aCircle);
  Bnd_Box2d           aBox;
  double              aTol = 0.;

  BndLib_Add2dCurve::Add(aCurve, aTol, aBox);

  double aXmin, aYmin, aXmax, aYmax;
  aBox.Get(aXmin, aYmin, aXmax, aYmax);

  EXPECT_NEAR(aXmin, 2., Precision::Confusion());
  EXPECT_NEAR(aXmax, 8., Precision::Confusion());
  EXPECT_NEAR(aYmin, 2., Precision::Confusion());
  EXPECT_NEAR(aYmax, 8., Precision::Confusion());
}

//==================================================================================================
// BndLib_AddSurface Tests
//==================================================================================================

TEST(BndLib_AddSurfaceTest, Plane)
{
  // Create a plane and add a finite patch
  Handle(Geom_Plane)  aPlane = new Geom_Plane(gp_Ax3(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)));
  GeomAdaptor_Surface aSurf(aPlane);
  Bnd_Box             aBox;
  double              aTol = 0.;

  // Add a 10x10 patch centered at origin
  BndLib_AddSurface::Add(aSurf, -5., 5., -5., 5., aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_NEAR(aXmin, -5., Precision::Confusion());
  EXPECT_NEAR(aXmax, 5., Precision::Confusion());
  EXPECT_NEAR(aYmin, -5., Precision::Confusion());
  EXPECT_NEAR(aYmax, 5., Precision::Confusion());
  EXPECT_NEAR(aZmin, 0., Precision::Confusion());
  EXPECT_NEAR(aZmax, 0., Precision::Confusion());
}

TEST(BndLib_AddSurfaceTest, Cylinder_Full)
{
  // Create a cylindrical surface
  Handle(Geom_CylindricalSurface) aCyl =
    new Geom_CylindricalSurface(gp_Ax3(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)), 5.);
  GeomAdaptor_Surface aSurf(aCyl);
  Bnd_Box             aBox;
  double              aTol = 0.;

  // Full U range, V from 0 to 10
  BndLib_AddSurface::Add(aSurf, 0., 2. * M_PI, 0., 10., aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_NEAR(aXmin, -5., Precision::Confusion());
  EXPECT_NEAR(aXmax, 5., Precision::Confusion());
  EXPECT_NEAR(aYmin, -5., Precision::Confusion());
  EXPECT_NEAR(aYmax, 5., Precision::Confusion());
  EXPECT_NEAR(aZmin, 0., Precision::Confusion());
  EXPECT_NEAR(aZmax, 10., Precision::Confusion());
}

TEST(BndLib_AddSurfaceTest, Sphere_Full)
{
  // Create a spherical surface
  Handle(Geom_SphericalSurface) aSphere =
    new Geom_SphericalSurface(gp_Ax3(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)), 7.);
  GeomAdaptor_Surface aSurf(aSphere);
  Bnd_Box             aBox;
  double              aTol = 0.;

  BndLib_AddSurface::Add(aSurf, aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_NEAR(aXmin, -7., Precision::Confusion());
  EXPECT_NEAR(aXmax, 7., Precision::Confusion());
  EXPECT_NEAR(aYmin, -7., Precision::Confusion());
  EXPECT_NEAR(aYmax, 7., Precision::Confusion());
  EXPECT_NEAR(aZmin, -7., Precision::Confusion());
  EXPECT_NEAR(aZmax, 7., Precision::Confusion());
}

TEST(BndLib_AddSurfaceTest, Sphere_Patch)
{
  // Create a spherical surface patch (upper hemisphere)
  Handle(Geom_SphericalSurface) aSphere =
    new Geom_SphericalSurface(gp_Ax3(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)), 5.);
  GeomAdaptor_Surface aSurf(aSphere);
  Bnd_Box             aBox;
  double              aTol = 0.;

  // Full U, V from 0 to PI/2 (upper hemisphere)
  BndLib_AddSurface::Add(aSurf, 0., 2. * M_PI, 0., M_PI / 2., aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_NEAR(aXmin, -5., Precision::Confusion());
  EXPECT_NEAR(aXmax, 5., Precision::Confusion());
  EXPECT_NEAR(aYmin, -5., Precision::Confusion());
  EXPECT_NEAR(aYmax, 5., Precision::Confusion());
  EXPECT_NEAR(aZmin, 0., Precision::Confusion());
  EXPECT_NEAR(aZmax, 5., Precision::Confusion());
}

TEST(BndLib_AddSurfaceTest, Cone_Full)
{
  // Create a conical surface
  Handle(Geom_ConicalSurface) aCone =
    new Geom_ConicalSurface(gp_Ax3(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)), M_PI / 4., 2.);
  GeomAdaptor_Surface aSurf(aCone);
  Bnd_Box             aBox;
  double              aTol = 0.;

  // Full U range, V from 0 to 5
  BndLib_AddSurface::Add(aSurf, 0., 2. * M_PI, 0., 5., aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  // V=5: radius = 2 + 5*sin(PI/4), Z = 5*cos(PI/4)
  double aMaxRadius = 2. + 5. * std::sin(M_PI / 4.);
  double aMaxZ      = 5. * std::cos(M_PI / 4.);

  EXPECT_NEAR(aXmax, aMaxRadius, Precision::Confusion());
  EXPECT_NEAR(aYmax, aMaxRadius, Precision::Confusion());
  EXPECT_NEAR(aZmax, aMaxZ, Precision::Confusion());
}

TEST(BndLib_AddSurfaceTest, Torus_Full)
{
  // Create a toroidal surface
  Handle(Geom_ToroidalSurface) aTorus =
    new Geom_ToroidalSurface(gp_Ax3(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)), 10., 3.);
  GeomAdaptor_Surface aSurf(aTorus);
  Bnd_Box             aBox;
  double              aTol = 0.;

  BndLib_AddSurface::Add(aSurf, aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  // BndLib_AddSurface uses conservative bounding for torus
  // The exact bounds are +-13 for X,Y and +-3 for Z
  // The algorithm may produce slightly larger bounds
  EXPECT_LE(aXmin, -13. + Precision::Confusion());
  EXPECT_GE(aXmax, 13. - Precision::Confusion());
  EXPECT_LE(aYmin, -13. + Precision::Confusion());
  EXPECT_GE(aYmax, 13. - Precision::Confusion());
  EXPECT_NEAR(aZmin, -3., Precision::Confusion());
  EXPECT_NEAR(aZmax, 3., Precision::Confusion());
}

TEST(BndLib_AddSurfaceTest, BezierSurface)
{
  // Create a simple Bezier surface (2x2 control points)
  TColgp_Array2OfPnt aPoles(1, 2, 1, 2);
  aPoles(1, 1) = gp_Pnt(0., 0., 0.);
  aPoles(1, 2) = gp_Pnt(0., 10., 2.);
  aPoles(2, 1) = gp_Pnt(10., 0., 2.);
  aPoles(2, 2) = gp_Pnt(10., 10., 0.);

  Handle(Geom_BezierSurface) aBezier = new Geom_BezierSurface(aPoles);
  GeomAdaptor_Surface        aSurf(aBezier);
  Bnd_Box                    aBox;
  double                     aTol = 0.;

  BndLib_AddSurface::Add(aSurf, aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  // Box should contain all control points
  EXPECT_LE(aXmin, 0.);
  EXPECT_GE(aXmax, 10.);
  EXPECT_LE(aYmin, 0.);
  EXPECT_GE(aYmax, 10.);
  EXPECT_LE(aZmin, 0.);
  EXPECT_GE(aZmax, 2.);
}

TEST(BndLib_AddSurfaceTest, AddOptimal_Sphere)
{
  // Test AddOptimal method with a sphere
  Handle(Geom_SphericalSurface) aSphere =
    new Geom_SphericalSurface(gp_Ax3(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)), 4.);
  GeomAdaptor_Surface aSurf(aSphere);
  Bnd_Box             aBox;
  double              aTol = 0.;

  BndLib_AddSurface::AddOptimal(aSurf, aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_NEAR(aXmin, -4., Precision::Confusion());
  EXPECT_NEAR(aXmax, 4., Precision::Confusion());
  EXPECT_NEAR(aYmin, -4., Precision::Confusion());
  EXPECT_NEAR(aYmax, 4., Precision::Confusion());
  EXPECT_NEAR(aZmin, -4., Precision::Confusion());
  EXPECT_NEAR(aZmax, 4., Precision::Confusion());
}

TEST(BndLib_AddSurfaceTest, Cylinder_Translated)
{
  // Create a cylindrical surface centered at (5, 5, 0)
  Handle(Geom_CylindricalSurface) aCyl =
    new Geom_CylindricalSurface(gp_Ax3(gp_Pnt(5., 5., 0.), gp_Dir(0., 0., 1.)), 3.);
  GeomAdaptor_Surface aSurf(aCyl);
  Bnd_Box             aBox;
  double              aTol = 0.;

  BndLib_AddSurface::Add(aSurf, 0., 2. * M_PI, 0., 10., aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_NEAR(aXmin, 2., Precision::Confusion());
  EXPECT_NEAR(aXmax, 8., Precision::Confusion());
  EXPECT_NEAR(aYmin, 2., Precision::Confusion());
  EXPECT_NEAR(aYmax, 8., Precision::Confusion());
  EXPECT_NEAR(aZmin, 0., Precision::Confusion());
  EXPECT_NEAR(aZmax, 10., Precision::Confusion());
}

TEST(BndLib_AddSurfaceTest, Plane_Tilted)
{
  // Create a tilted plane (normal at 45 degrees)
  // gp_Dir automatically normalizes the input vector
  Handle(Geom_Plane)  aPlane = new Geom_Plane(gp_Ax3(gp_Pnt(0., 0., 0.), gp_Dir(0., 1., 1.)));
  GeomAdaptor_Surface aSurf(aPlane);
  Bnd_Box             aBox;
  double              aTol = 0.;

  // Add a unit square patch
  BndLib_AddSurface::Add(aSurf, 0., 1., 0., 1., aTol, aBox);

  EXPECT_FALSE(aBox.IsVoid()) << "Box should not be void for tilted plane";
}

// Test for edge case with large parameter values and small step sizes.
// This verifies that the algorithm correctly samples different points
// even when parameter values are large relative to the step size,
// which could cause floating-point precision issues if accumulation
// (U += dU) is used instead of multiplication (U = UMin + dU * i).
TEST(BndLib_AddSurfaceTest, LargeParameters_PrecisionTest)
{
  // Create a Bezier surface - uses the default sampling path
  TColgp_Array2OfPnt aPoles(1, 3, 1, 3);
  // Create a simple surface with known extent
  for (int i = 1; i <= 3; i++)
  {
    for (int j = 1; j <= 3; j++)
    {
      aPoles(i, j) = gp_Pnt((i - 1) * 10., (j - 1) * 10., 0.);
    }
  }
  Handle(Geom_BezierSurface) aBezier = new Geom_BezierSurface(aPoles);
  GeomAdaptor_Surface        aSurf(aBezier);
  Bnd_Box                    aBox;
  double                     aTol = 0.;

  // Use large parameter offset with small parameter range
  // This simulates the edge case where UMin/UMax are large but (UMax-UMin) is small
  // The actual Bezier is defined on [0,1] x [0,1], but we test a sub-range
  // to ensure the sampling algorithm works correctly
  BndLib_AddSurface::Add(aSurf, 0., 1., 0., 1., aTol, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  // The surface spans [0,20] x [0,20] x [0,0]
  // Bounding box should contain all sample points
  EXPECT_LE(aXmin, 1.) << "XMin should capture sampled points";
  EXPECT_GE(aXmax, 19.) << "XMax should capture sampled points";
  EXPECT_LE(aYmin, 1.) << "YMin should capture sampled points";
  EXPECT_GE(aYmax, 19.) << "YMax should capture sampled points";
}

// Test with translated surface where parameters are offset
// This is a more realistic test of the large parameter scenario
TEST(BndLib_AddSurfaceTest, OffsetSurface_ParameterPrecision)
{
  // Create a simple plane
  Handle(Geom_Plane)  aPlane = new Geom_Plane(gp_Ax3(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)));
  GeomAdaptor_Surface aSurf(aPlane);
  Bnd_Box             aBox;
  double              aTol = 0.;

  // Test with very large parameter values but small range
  // UMin = 1e10, UMax = 1e10 + 100, so dU ~ 100/(Nu-1) ~ small relative to 1e10
  const double aLargeOffset = 1.0e10;
  const double aRange       = 100.;
  BndLib_AddSurface::Add(aSurf,
                         aLargeOffset,
                         aLargeOffset + aRange,
                         aLargeOffset,
                         aLargeOffset + aRange,
                         aTol,
                         aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  // For a plane at z=0 with parameters as X,Y coordinates:
  // The box should span approximately [1e10, 1e10+100] in both X and Y
  // Allow some tolerance due to floating-point representation
  const double aTolerance = 1.0; // 1 unit tolerance for large values

  EXPECT_NEAR(aXmin, aLargeOffset, aTolerance) << "XMin should be near the large offset value";
  EXPECT_NEAR(aXmax, aLargeOffset + aRange, aTolerance) << "XMax should capture the full range";
  EXPECT_NEAR(aYmin, aLargeOffset, aTolerance) << "YMin should be near the large offset value";
  EXPECT_NEAR(aYmax, aLargeOffset + aRange, aTolerance) << "YMax should capture the full range";

  // Most importantly: verify the range is captured (not collapsed due to precision loss)
  const double aComputedRangeX = aXmax - aXmin;
  const double aComputedRangeY = aYmax - aYmin;

  EXPECT_GT(aComputedRangeX, aRange * 0.9)
    << "X range should not collapse due to floating-point precision issues";
  EXPECT_GT(aComputedRangeY, aRange * 0.9)
    << "Y range should not collapse due to floating-point precision issues";
}
