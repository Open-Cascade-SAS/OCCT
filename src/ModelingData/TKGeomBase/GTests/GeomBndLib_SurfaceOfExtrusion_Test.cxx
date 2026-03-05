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

#include <GeomBndLib_Surface.hxx>

#include <BndLib_AddSurface.hxx>
#include <Bnd_Box.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Line.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <gp_Pnt.hxx>

#include <cmath>

#include <gtest/gtest.h>

namespace
{

//! Helper: verify new box is no larger than old box (tighter or equal).
void ExpectNoLarger(const Bnd_Box& theNew, const Bnd_Box& theOld, const double theTol)
{
  const auto [aXminN, aXmaxN, aYminN, aYmaxN, aZminN, aZmaxN] = theNew.Get();
  const auto [aXminO, aXmaxO, aYminO, aYmaxO, aZminO, aZmaxO] = theOld.Get();
  EXPECT_GE(aXminN, aXminO - theTol) << "New Xmin is smaller than old";
  EXPECT_LE(aXmaxN, aXmaxO + theTol) << "New Xmax is larger than old";
  EXPECT_GE(aYminN, aYminO - theTol) << "New Ymin is smaller than old";
  EXPECT_LE(aYmaxN, aYmaxO + theTol) << "New Ymax is larger than old";
  EXPECT_GE(aZminN, aZminO - theTol) << "New Zmin is smaller than old";
  EXPECT_LE(aZmaxN, aZmaxO + theTol) << "New Zmax is larger than old";
}

//! Helper: verify box contains all sampled points of the surface.
void ExpectContainsSurface(const Bnd_Box&                                    theBox,
                           const occ::handle<Geom_SurfaceOfLinearExtrusion>& theSurf,
                           const double                                      theUMin,
                           const double                                      theUMax,
                           const double                                      theVMin,
                           const double                                      theVMax,
                           const int                                         theNbU,
                           const int                                         theNbV)
{
  double aXmin = 0., aYmin = 0., aZmin = 0., aXmax = 0., aYmax = 0., aZmax = 0.;
  theBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
  const double aUStep = (theUMax - theUMin) / theNbU;
  const double aVStep = (theVMax - theVMin) / theNbV;
  for (int i = 0; i <= theNbU; ++i)
  {
    const double aU = theUMin + i * aUStep;
    for (int j = 0; j <= theNbV; ++j)
    {
      const double aV   = theVMin + j * aVStep;
      const gp_Pnt aPnt = theSurf->Value(aU, aV);
      EXPECT_GE(aPnt.X(), aXmin - Precision::Confusion())
        << "Point outside box at U=" << aU << " V=" << aV;
      EXPECT_LE(aPnt.X(), aXmax + Precision::Confusion())
        << "Point outside box at U=" << aU << " V=" << aV;
      EXPECT_GE(aPnt.Y(), aYmin - Precision::Confusion())
        << "Point outside box at U=" << aU << " V=" << aV;
      EXPECT_LE(aPnt.Y(), aYmax + Precision::Confusion())
        << "Point outside box at U=" << aU << " V=" << aV;
      EXPECT_GE(aPnt.Z(), aZmin - Precision::Confusion())
        << "Point outside box at U=" << aU << " V=" << aV;
      EXPECT_LE(aPnt.Z(), aZmax + Precision::Confusion())
        << "Point outside box at U=" << aU << " V=" << aV;
    }
  }
}

} // namespace

// =========================================================================
// Circle along Z
// =========================================================================

TEST(GeomBndLib_ExtrusionTest, CircleAlongZ_Full_CompareWithBndLib)
{
  occ::handle<Geom_Circle>                   aCircle = new Geom_Circle(gp::XOY(), 5.0);
  occ::handle<Geom_SurfaceOfLinearExtrusion> anExtr =
    new Geom_SurfaceOfLinearExtrusion(aCircle, gp::DZ());
  GeomAdaptor_Surface anAdaptor(anExtr);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(anExtr).Add(0.0, 2.0 * M_PI, 0.0, 10.0, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, 0.0, 2.0 * M_PI, 0.0, 10.0, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsSurface(aNewBox, anExtr, 0.0, 2.0 * M_PI, 0.0, 10.0, 36, 10);
}

TEST(GeomBndLib_ExtrusionTest, CircleAlongZ_Arc_CompareWithBndLib)
{
  occ::handle<Geom_Circle>                   aCircle = new Geom_Circle(gp::XOY(), 5.0);
  occ::handle<Geom_SurfaceOfLinearExtrusion> anExtr =
    new Geom_SurfaceOfLinearExtrusion(aCircle, gp::DZ());
  GeomAdaptor_Surface anAdaptor(anExtr);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(anExtr).Add(0.0, M_PI, 0.0, 5.0, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, 0.0, M_PI, 0.0, 5.0, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsSurface(aNewBox, anExtr, 0.0, M_PI, 0.0, 5.0, 18, 5);
}

TEST(GeomBndLib_ExtrusionTest, InfiniteU_OpenInExtrusionBasisDirection)
{
  // Line along Z through (5, 0, 0) extruded along X.
  // P(U, V) = (5 + V, 0, U); U in (-inf, +inf), V in [0, 10].
  // Expected: X in [5, 15], Y finite (= 0), Z open both ways.
  occ::handle<Geom_Line>                     aLine = new Geom_Line(gp_Pnt(5.0, 0.0, 0.0), gp::DZ());
  occ::handle<Geom_SurfaceOfLinearExtrusion> anExtr =
    new Geom_SurfaceOfLinearExtrusion(aLine, gp::DX());

  Bnd_Box aNewBox;
  GeomBndLib_Surface(anExtr)
    .Add(-Precision::Infinite(), Precision::Infinite(), 0.0, 10.0, Precision::Confusion(), aNewBox);

  EXPECT_FALSE(aNewBox.IsVoid());
  EXPECT_FALSE(aNewBox.IsWhole());
  EXPECT_FALSE(aNewBox.IsOpenXmin());
  EXPECT_FALSE(aNewBox.IsOpenXmax());
  EXPECT_FALSE(aNewBox.IsOpenYmin());
  EXPECT_FALSE(aNewBox.IsOpenYmax());
  EXPECT_TRUE(aNewBox.IsOpenZmin());
  EXPECT_TRUE(aNewBox.IsOpenZmax());
  const auto [aXmin, aXmax, aYmin, aYmax, aZmin, aZmax] = aNewBox.Get();
  const double aTol = 2.0 * Precision::Confusion(); // box is enlarged by theTol on construction
  EXPECT_NEAR(aXmin, 5.0, aTol) << "Xmin";
  EXPECT_NEAR(aXmax, 15.0, aTol) << "Xmax";
  EXPECT_NEAR(aYmin, 0.0, aTol) << "Ymin";
  EXPECT_NEAR(aYmax, 0.0, aTol) << "Ymax";
}

// =========================================================================
// Circle along diagonal
// =========================================================================

TEST(GeomBndLib_ExtrusionTest, CircleAlongDiagonal_CompareWithBndLib)
{
  occ::handle<Geom_Circle>                   aCircle = new Geom_Circle(gp::XOY(), 5.0);
  occ::handle<Geom_SurfaceOfLinearExtrusion> anExtr =
    new Geom_SurfaceOfLinearExtrusion(aCircle, gp_Dir(1.0, 0.0, 1.0));
  GeomAdaptor_Surface anAdaptor(anExtr);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(anExtr).Add(0.0, 2.0 * M_PI, 0.0, 10.0, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, 0.0, 2.0 * M_PI, 0.0, 10.0, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsSurface(aNewBox, anExtr, 0.0, 2.0 * M_PI, 0.0, 10.0, 36, 10);
}

// =========================================================================
// Ellipse along Z
// =========================================================================

TEST(GeomBndLib_ExtrusionTest, EllipseAlongZ_CompareWithBndLib)
{
  occ::handle<Geom_Ellipse>                  anEllipse = new Geom_Ellipse(gp::XOY(), 8.0, 3.0);
  occ::handle<Geom_SurfaceOfLinearExtrusion> anExtr =
    new Geom_SurfaceOfLinearExtrusion(anEllipse, gp::DZ());
  GeomAdaptor_Surface anAdaptor(anExtr);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(anExtr).Add(0.0, 2.0 * M_PI, 0.0, 5.0, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, 0.0, 2.0 * M_PI, 0.0, 5.0, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsSurface(aNewBox, anExtr, 0.0, 2.0 * M_PI, 0.0, 5.0, 36, 5);
}

TEST(GeomBndLib_ExtrusionTest, EllipseAlongZ_Arc_CompareWithBndLib)
{
  occ::handle<Geom_Ellipse>                  anEllipse = new Geom_Ellipse(gp::XOY(), 8.0, 3.0);
  occ::handle<Geom_SurfaceOfLinearExtrusion> anExtr =
    new Geom_SurfaceOfLinearExtrusion(anEllipse, gp::DZ());
  GeomAdaptor_Surface anAdaptor(anExtr);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(anExtr).Add(0.0, M_PI, -2.0, 5.0, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, 0.0, M_PI, -2.0, 5.0, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsSurface(aNewBox, anExtr, 0.0, M_PI, -2.0, 5.0, 18, 7);
}

// =========================================================================
// Line along Z (planar surface)
// =========================================================================

TEST(GeomBndLib_ExtrusionTest, LineAlongZ_CompareWithBndLib)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  occ::handle<Geom_SurfaceOfLinearExtrusion> anExtr =
    new Geom_SurfaceOfLinearExtrusion(aLine, gp::DZ());
  GeomAdaptor_Surface anAdaptor(anExtr);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(anExtr).Add(0.0, 10.0, 0.0, 5.0, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, 0.0, 10.0, 0.0, 5.0, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsSurface(aNewBox, anExtr, 0.0, 10.0, 0.0, 5.0, 10, 5);
}

// =========================================================================
// Line along diagonal
// =========================================================================

TEST(GeomBndLib_ExtrusionTest, LineAlongDiagonal_CompareWithBndLib)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0));
  occ::handle<Geom_SurfaceOfLinearExtrusion> anExtr =
    new Geom_SurfaceOfLinearExtrusion(aLine, gp_Dir(1.0, 0.0, 1.0));
  GeomAdaptor_Surface anAdaptor(anExtr);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(anExtr).Add(0.0, 5.0, 0.0, 10.0, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, 0.0, 5.0, 0.0, 10.0, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsSurface(aNewBox, anExtr, 0.0, 5.0, 0.0, 10.0, 5, 10);
}

// =========================================================================
// BSpline along Z
// =========================================================================

TEST(GeomBndLib_ExtrusionTest, BSplineAlongZ_CompareWithBndLib)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt(3.0, 5.0, 0.0));
  aPoles.SetValue(3, gp_Pnt(7.0, -2.0, 0.0));
  aPoles.SetValue(4, gp_Pnt(10.0, 3.0, 0.0));

  NCollection_Array1<double> aKnots(1, 3);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 0.5);
  aKnots.SetValue(3, 1.0);

  NCollection_Array1<int> aMults(1, 3);
  aMults.SetValue(1, 3);
  aMults.SetValue(2, 1);
  aMults.SetValue(3, 3);

  occ::handle<Geom_BSplineCurve> aBSpl = new Geom_BSplineCurve(aPoles, aKnots, aMults, 2);
  occ::handle<Geom_SurfaceOfLinearExtrusion> anExtr =
    new Geom_SurfaceOfLinearExtrusion(aBSpl, gp::DZ());
  GeomAdaptor_Surface anAdaptor(anExtr);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(anExtr).AddOptimal(0.0, 1.0, 0.0, 5.0, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::AddOptimal(anAdaptor, 0.0, 1.0, 0.0, 5.0, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsSurface(aNewBox, anExtr, 0.0, 1.0, 0.0, 5.0, 10, 5);
}

TEST(GeomBndLib_ExtrusionTest, BSplineAlongDiagonal_CompareWithBndLib)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt(3.0, 5.0, 0.0));
  aPoles.SetValue(3, gp_Pnt(7.0, -2.0, 0.0));
  aPoles.SetValue(4, gp_Pnt(10.0, 3.0, 0.0));

  NCollection_Array1<double> aKnots(1, 3);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 0.5);
  aKnots.SetValue(3, 1.0);

  NCollection_Array1<int> aMults(1, 3);
  aMults.SetValue(1, 3);
  aMults.SetValue(2, 1);
  aMults.SetValue(3, 3);

  occ::handle<Geom_BSplineCurve> aBSpl = new Geom_BSplineCurve(aPoles, aKnots, aMults, 2);
  occ::handle<Geom_SurfaceOfLinearExtrusion> anExtr =
    new Geom_SurfaceOfLinearExtrusion(aBSpl, gp_Dir(1.0, 1.0, 1.0));
  Bnd_Box aNewBox;
  GeomBndLib_Surface(anExtr).AddOptimal(0.0, 1.0, -3.0, 7.0, Precision::Confusion(), aNewBox);

  // P(u,v) = C(u) + v*(1,1,1)/sqrt(3), u in [0,1], v in [-3, 7].
  // BSpline C(0)=(0,0,0), C(1)=(10,3,0). Extrusion shifts: v_min/sqrt(3) = -sqrt(3),
  // v_max/sqrt(3) = 7/sqrt(3). Reference bounds derive from endpoint extremes + extrusion range.
  const double aSqrt3                                   = std::sqrt(3.0);
  const double aVminShift                               = -3.0 / aSqrt3; // ~= -1.7321
  const double aVmaxShift                               = 7.0 / aSqrt3;  // ~=  4.0415
  const double aTolRef                                  = 1e-3;
  const auto [aXmin, aXmax, aYmin, aYmax, aZmin, aZmax] = aNewBox.Get();
  EXPECT_NEAR(aXmin, 0.0 + aVminShift, aTolRef) << "Xmin";
  EXPECT_NEAR(aXmax, 10.0 + aVmaxShift, aTolRef) << "Xmax";
  EXPECT_NEAR(aYmin, 0.0 + aVminShift, aTolRef) << "Ymin";
  EXPECT_NEAR(aYmax, 3.0 + aVmaxShift, aTolRef) << "Ymax";
  EXPECT_NEAR(aZmin, aVminShift, aTolRef) << "Zmin";
  EXPECT_NEAR(aZmax, aVmaxShift, aTolRef) << "Zmax";
  ExpectContainsSurface(aNewBox, anExtr, 0.0, 1.0, -3.0, 7.0, 10, 10);
}

// =========================================================================
// Negative V range
// =========================================================================

TEST(GeomBndLib_ExtrusionTest, CircleNegativeV_CompareWithBndLib)
{
  occ::handle<Geom_Circle>                   aCircle = new Geom_Circle(gp::XOY(), 5.0);
  occ::handle<Geom_SurfaceOfLinearExtrusion> anExtr =
    new Geom_SurfaceOfLinearExtrusion(aCircle, gp::DZ());
  GeomAdaptor_Surface anAdaptor(anExtr);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(anExtr).Add(0.0, 2.0 * M_PI, -5.0, 5.0, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, 0.0, 2.0 * M_PI, -5.0, 5.0, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsSurface(aNewBox, anExtr, 0.0, 2.0 * M_PI, -5.0, 5.0, 36, 10);
}
