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

#include <GeomBndLib_Curve.hxx>

#include <BndLib_Add3dCurve.hxx>
#include <Bnd_Box.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomBndLib_SamplingHelpers.pxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Parab.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

namespace
{

//! Helper: compare two Bnd_Box results within tolerance.
void CompareBoxes(const Bnd_Box& theNew, const Bnd_Box& theOld, const double theTol)
{
  const auto [aXminN, aXmaxN, aYminN, aYmaxN, aZminN, aZmaxN] = theNew.Get();
  const auto [aXminO, aXmaxO, aYminO, aYmaxO, aZminO, aZmaxO] = theOld.Get();
  EXPECT_NEAR(aXminN, aXminO, theTol);
  EXPECT_NEAR(aXmaxN, aXmaxO, theTol);
  EXPECT_NEAR(aYminN, aYminO, theTol);
  EXPECT_NEAR(aYmaxN, aYmaxO, theTol);
  EXPECT_NEAR(aZminN, aZminO, theTol);
  EXPECT_NEAR(aZmaxN, aZmaxO, theTol);
}

//! Helper: verify that the new box conservatively encloses the old box.
void ExpectConservativeEnvelope(const Bnd_Box& theNew, const Bnd_Box& theOld, const double theTol)
{
  const auto [aXminN, aXmaxN, aYminN, aYmaxN, aZminN, aZmaxN] = theNew.Get();
  const auto [aXminO, aXmaxO, aYminO, aYmaxO, aZminO, aZmaxO] = theOld.Get();
  EXPECT_LE(aXminN, aXminO + theTol);
  EXPECT_GE(aXmaxN, aXmaxO - theTol);
  EXPECT_LE(aYminN, aYminO + theTol);
  EXPECT_GE(aYmaxN, aYmaxO - theTol);
  EXPECT_LE(aZminN, aZminO + theTol);
  EXPECT_GE(aZmaxN, aZmaxO - theTol);
}

} // namespace

// =========================================================================
// Line
// =========================================================================

TEST(GeomBndLib_CurveTest, Line_FiniteSegment)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 1.0, 0.0));
  GeomBndLib_Curve       aCurve(aLine);
  EXPECT_EQ(aCurve.GetType(), GeomAbs_Line);

  Bnd_Box aBox;
  aCurve.Add(0.0, 10.0, 0.0, aBox);

  const auto [aXmin, aXmax, aYmin, aYmax, aZmin, aZmax] = aBox.Get();

  // Line from (0,0,0) in direction (1,1,0) normalized, parameter 10
  // Endpoint: (10/sqrt(2), 10/sqrt(2), 0)
  const double aLen = 10.0 / std::sqrt(2.0);
  EXPECT_NEAR(aXmin, 0.0, Precision::Confusion());
  EXPECT_NEAR(aYmin, 0.0, Precision::Confusion());
  EXPECT_NEAR(aZmin, 0.0, Precision::Confusion());
  EXPECT_NEAR(aXmax, aLen, Precision::Confusion());
  EXPECT_NEAR(aYmax, aLen, Precision::Confusion());
  EXPECT_NEAR(aZmax, 0.0, Precision::Confusion());
}

TEST(GeomBndLib_CurveTest, Line_CompareWithBndLib)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(1.0, 2.0, 3.0), gp_Dir(0.0, 0.0, 1.0));
  GeomAdaptor_Curve      anAdaptor(aLine);

  Bnd_Box aNewBox;
  GeomBndLib_Curve(aLine).Add(0.0, 5.0, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_Add3dCurve::Add(anAdaptor, 0.0, 5.0, Precision::Confusion(), anOldBox);

  CompareBoxes(aNewBox, anOldBox, Precision::Confusion());
}

// =========================================================================
// Circle
// =========================================================================

TEST(GeomBndLib_CurveTest, Circle_Full)
{
  const double             aRadius = 5.0;
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(gp::XOY(), aRadius);
  GeomBndLib_Curve         aCurve(aCircle);
  EXPECT_EQ(aCurve.GetType(), GeomAbs_Circle);

  Bnd_Box aBox;
  aCurve.Add(0.0, aBox);

  const auto [aXmin, aXmax, aYmin, aYmax, aZmin, aZmax] = aBox.Get();

  EXPECT_NEAR(aXmin, -aRadius, Precision::Confusion());
  EXPECT_NEAR(aYmin, -aRadius, Precision::Confusion());
  EXPECT_NEAR(aXmax, aRadius, Precision::Confusion());
  EXPECT_NEAR(aYmax, aRadius, Precision::Confusion());
  EXPECT_NEAR(aZmin, 0.0, Precision::Confusion());
  EXPECT_NEAR(aZmax, 0.0, Precision::Confusion());
}

TEST(GeomBndLib_CurveTest, Circle_Arc)
{
  const double             aRadius = 10.0;
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(gp::XOY(), aRadius);
  GeomBndLib_Curve         aCurve(aCircle);

  Bnd_Box aBox;
  // Pass 0 tolerance for precise reference comparison
  aCurve.Add(0.0, M_PI / 2.0, 0.0, aBox);

  const auto [aXmin, aXmax, aYmin, aYmax, aZmin, aZmax] = aBox.Get();

  EXPECT_NEAR(aXmin, 0.0, Precision::Confusion());
  EXPECT_NEAR(aYmin, 0.0, Precision::Confusion());
  EXPECT_NEAR(aXmax, aRadius, Precision::Confusion());
  EXPECT_NEAR(aYmax, aRadius, Precision::Confusion());
  EXPECT_NEAR(aZmin, 0.0, Precision::Confusion());
  EXPECT_NEAR(aZmax, 0.0, Precision::Confusion());
}

TEST(GeomBndLib_CurveTest, Circle_CompareWithBndLib)
{
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(gp::XOY(), 7.5);
  GeomAdaptor_Curve        anAdaptor(aCircle);

  Bnd_Box aNewBox;
  GeomBndLib_Curve(aCircle).Add(Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_Add3dCurve::Add(anAdaptor, Precision::Confusion(), anOldBox);

  CompareBoxes(aNewBox, anOldBox, Precision::Confusion());
}

TEST(GeomBndLib_CurveTest, Circle_Arc_CompareWithBndLib)
{
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(gp::XOY(), 7.5);
  GeomAdaptor_Curve        anAdaptor(aCircle);

  Bnd_Box aNewBox;
  GeomBndLib_Curve(aCircle).Add(0.5, 2.5, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_Add3dCurve::Add(anAdaptor, 0.5, 2.5, Precision::Confusion(), anOldBox);

  CompareBoxes(aNewBox, anOldBox, Precision::Confusion());
}

// =========================================================================
// Ellipse
// =========================================================================

TEST(GeomBndLib_CurveTest, Ellipse_Full)
{
  const double              aMajR     = 10.0;
  const double              aMinR     = 5.0;
  occ::handle<Geom_Ellipse> anEllipse = new Geom_Ellipse(gp::XOY(), aMajR, aMinR);
  GeomBndLib_Curve          aCurve(anEllipse);
  EXPECT_EQ(aCurve.GetType(), GeomAbs_Ellipse);

  Bnd_Box aBox;
  aCurve.Add(0.0, aBox);

  const auto [aXmin, aXmax, aYmin, aYmax, aZmin, aZmax] = aBox.Get();

  EXPECT_NEAR(aXmin, -aMajR, Precision::Confusion());
  EXPECT_NEAR(aXmax, aMajR, Precision::Confusion());
  EXPECT_NEAR(aYmin, -aMinR, Precision::Confusion());
  EXPECT_NEAR(aYmax, aMinR, Precision::Confusion());
  EXPECT_NEAR(aZmin, 0.0, Precision::Confusion());
  EXPECT_NEAR(aZmax, 0.0, Precision::Confusion());
}

TEST(GeomBndLib_CurveTest, Ellipse_CompareWithBndLib)
{
  occ::handle<Geom_Ellipse> anEllipse = new Geom_Ellipse(gp::XOY(), 8.0, 3.0);
  GeomAdaptor_Curve         anAdaptor(anEllipse);

  Bnd_Box aNewBox;
  GeomBndLib_Curve(anEllipse).Add(Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_Add3dCurve::Add(anAdaptor, Precision::Confusion(), anOldBox);

  CompareBoxes(aNewBox, anOldBox, Precision::Confusion());
}

TEST(GeomBndLib_CurveTest, Ellipse_Arc_CompareWithBndLib)
{
  occ::handle<Geom_Ellipse> anEllipse = new Geom_Ellipse(gp::XOY(), 8.0, 3.0);
  GeomAdaptor_Curve         anAdaptor(anEllipse);

  Bnd_Box aNewBox;
  GeomBndLib_Curve(anEllipse).Add(0.3, 2.0, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_Add3dCurve::Add(anAdaptor, 0.3, 2.0, Precision::Confusion(), anOldBox);

  CompareBoxes(aNewBox, anOldBox, Precision::Confusion());
}

// =========================================================================
// Hyperbola
// =========================================================================

TEST(GeomBndLib_CurveTest, Hyperbola_FiniteArc)
{
  const double                aMajR = 5.0;
  const double                aMinR = 3.0;
  occ::handle<Geom_Hyperbola> aHyp  = new Geom_Hyperbola(gp::XOY(), aMajR, aMinR);
  GeomBndLib_Curve            aCurve(aHyp);
  EXPECT_EQ(aCurve.GetType(), GeomAbs_Hyperbola);

  Bnd_Box      aBox;
  const double aU = 1.0;
  aCurve.Add(-aU, aU, 0.0, aBox);

  const auto [aXmin, aXmax, aYmin, aYmax, aZmin, aZmax] = aBox.Get();

  // Hyperbola: x = aMajR * cosh(t), y = aMinR * sinh(t)
  const double aXEnd = aMajR * std::cosh(aU);
  const double aYEnd = aMinR * std::sinh(aU);

  // Box should contain the vertex (aMajR, 0) and endpoints
  EXPECT_LE(aXmin, aMajR + Precision::Confusion());
  EXPECT_GE(aXmax, aXEnd - Precision::Confusion());
  EXPECT_LE(aYmin, -aYEnd + Precision::Confusion());
  EXPECT_GE(aYmax, aYEnd - Precision::Confusion());
}

TEST(GeomBndLib_CurveTest, Hyperbola_CompareWithBndLib)
{
  occ::handle<Geom_Hyperbola> aHyp = new Geom_Hyperbola(gp::XOY(), 5.0, 3.0);
  GeomAdaptor_Curve           anAdaptor(aHyp);

  Bnd_Box aNewBox;
  GeomBndLib_Curve(aHyp).Add(-1.0, 1.0, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_Add3dCurve::Add(anAdaptor, -1.0, 1.0, Precision::Confusion(), anOldBox);

  CompareBoxes(aNewBox, anOldBox, Precision::Confusion());
}

// =========================================================================
// Parabola
// =========================================================================

TEST(GeomBndLib_CurveTest, Parabola_FiniteArc)
{
  const double               aFocal = 2.0;
  occ::handle<Geom_Parabola> aParab = new Geom_Parabola(gp::XOY(), aFocal);
  GeomBndLib_Curve           aCurve(aParab);
  EXPECT_EQ(aCurve.GetType(), GeomAbs_Parabola);

  Bnd_Box      aBox;
  const double aU = 5.0;
  aCurve.Add(-aU, aU, 0.0, aBox);

  const auto [aXmin, aXmax, aYmin, aYmax, aZmin, aZmax] = aBox.Get();

  // Parabola: x = t^2/(4*focal), y = t
  const double aXEnd = aU * aU / (4.0 * aFocal);

  EXPECT_LE(aXmin, Precision::Confusion());
  EXPECT_GE(aXmax, aXEnd - Precision::Confusion());
  EXPECT_LE(aYmin, -aU + Precision::Confusion());
  EXPECT_GE(aYmax, aU - Precision::Confusion());
}

TEST(GeomBndLib_CurveTest, Parabola_CompareWithBndLib)
{
  occ::handle<Geom_Parabola> aParab = new Geom_Parabola(gp::XOY(), 2.0);
  GeomAdaptor_Curve          anAdaptor(aParab);

  Bnd_Box aNewBox;
  GeomBndLib_Curve(aParab).Add(-5.0, 5.0, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_Add3dCurve::Add(anAdaptor, -5.0, 5.0, Precision::Confusion(), anOldBox);

  CompareBoxes(aNewBox, anOldBox, Precision::Confusion());
}

// =========================================================================
// BezierCurve
// =========================================================================

TEST(GeomBndLib_CurveTest, BezierCurve_Simple)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt(5.0, 10.0, 0.0));
  aPoles.SetValue(3, gp_Pnt(10.0, 0.0, 0.0));

  occ::handle<Geom_BezierCurve> aBez = new Geom_BezierCurve(aPoles);
  GeomBndLib_Curve              aCurve(aBez);
  EXPECT_EQ(aCurve.GetType(), GeomAbs_BezierCurve);

  Bnd_Box aBox;
  aCurve.Add(Precision::Confusion(), aBox);

  const auto [aXmin, aXmax, aYmin, aYmax, aZmin, aZmax] = aBox.Get();

  // Bezier convex hull: x in [0,10], y in [0, <=10], z = 0
  EXPECT_LE(aXmin, Precision::Confusion());
  EXPECT_GE(aXmax, 10.0 - Precision::Confusion());
  EXPECT_LE(aYmin, Precision::Confusion());
  EXPECT_GE(aYmax, 0.0);
  EXPECT_NEAR(aZmin, 0.0, Precision::Confusion());
  EXPECT_NEAR(aZmax, 0.0, Precision::Confusion());
}

TEST(GeomBndLib_CurveTest, BezierCurve_CompareWithBndLib)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt(3.0, 8.0, 1.0));
  aPoles.SetValue(3, gp_Pnt(7.0, 2.0, -1.0));
  aPoles.SetValue(4, gp_Pnt(10.0, 5.0, 0.0));

  occ::handle<Geom_BezierCurve> aBez = new Geom_BezierCurve(aPoles);
  GeomAdaptor_Curve             anAdaptor(aBez);

  Bnd_Box aNewBox;
  GeomBndLib_Curve(aBez).Add(Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_Add3dCurve::Add(anAdaptor, Precision::Confusion(), anOldBox);

  CompareBoxes(aNewBox, anOldBox, Precision::Confusion());
}

// =========================================================================
// BSplineCurve
// =========================================================================

TEST(GeomBndLib_CurveTest, BSplineCurve_Simple)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt(2.0, 5.0, 0.0));
  aPoles.SetValue(3, gp_Pnt(8.0, 5.0, 0.0));
  aPoles.SetValue(4, gp_Pnt(10.0, 0.0, 0.0));

  NCollection_Array1<double> aKnots(1, 3);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 0.5);
  aKnots.SetValue(3, 1.0);

  NCollection_Array1<int> aMults(1, 3);
  aMults.SetValue(1, 3);
  aMults.SetValue(2, 1);
  aMults.SetValue(3, 3);

  occ::handle<Geom_BSplineCurve> aBSpl = new Geom_BSplineCurve(aPoles, aKnots, aMults, 2);
  GeomBndLib_Curve               aCurve(aBSpl);
  EXPECT_EQ(aCurve.GetType(), GeomAbs_BSplineCurve);

  Bnd_Box aBox;
  aCurve.Add(Precision::Confusion(), aBox);

  const auto [aXmin, aXmax, aYmin, aYmax, aZmin, aZmax] = aBox.Get();

  EXPECT_LE(aXmin, Precision::Confusion());
  EXPECT_GE(aXmax, 10.0 - Precision::Confusion());
  EXPECT_LE(aYmin, Precision::Confusion());
  EXPECT_GE(aYmax, 0.0);
  EXPECT_NEAR(aZmin, 0.0, Precision::Confusion());
  EXPECT_NEAR(aZmax, 0.0, Precision::Confusion());
}

TEST(GeomBndLib_CurveTest, BSplineCurve_CompareWithBndLib)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt(2.0, 5.0, 1.0));
  aPoles.SetValue(3, gp_Pnt(8.0, 5.0, -1.0));
  aPoles.SetValue(4, gp_Pnt(10.0, 0.0, 0.0));

  NCollection_Array1<double> aKnots(1, 3);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 0.5);
  aKnots.SetValue(3, 1.0);

  NCollection_Array1<int> aMults(1, 3);
  aMults.SetValue(1, 3);
  aMults.SetValue(2, 1);
  aMults.SetValue(3, 3);

  occ::handle<Geom_BSplineCurve> aBSpl = new Geom_BSplineCurve(aPoles, aKnots, aMults, 2);
  GeomAdaptor_Curve              anAdaptor(aBSpl);

  Bnd_Box aNewBox;
  GeomBndLib_Curve(aBSpl).Add(Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_Add3dCurve::Add(anAdaptor, Precision::Confusion(), anOldBox);

  CompareBoxes(aNewBox, anOldBox, Precision::Confusion());
}

TEST(GeomBndLib_CurveTest, BSplineCurve_TrimmedRange_CompareWithBndLib)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 7);
  aPoles.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt(10.0, 20.0, 0.0));
  aPoles.SetValue(3, gp_Pnt(20.0, -10.0, 5.0));
  aPoles.SetValue(4, gp_Pnt(30.0, 40.0, 0.0));
  aPoles.SetValue(5, gp_Pnt(40.0, 5.0, -5.0));
  aPoles.SetValue(6, gp_Pnt(50.0, 15.0, 0.0));
  aPoles.SetValue(7, gp_Pnt(60.0, 0.0, 0.0));

  NCollection_Array1<double> aKnots(1, 5);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 1.0);
  aKnots.SetValue(3, 2.0);
  aKnots.SetValue(4, 3.0);
  aKnots.SetValue(5, 4.0);

  NCollection_Array1<int> aMults(1, 5);
  aMults.SetValue(1, 4);
  aMults.SetValue(2, 1);
  aMults.SetValue(3, 2);
  aMults.SetValue(4, 1);
  aMults.SetValue(5, 4);

  occ::handle<Geom_BSplineCurve> aBSpl = new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);
  GeomAdaptor_Curve              anAdaptor(aBSpl);

  constexpr double aU1 = 1.82;
  constexpr double aU2 = 2.04;

  Bnd_Box aNewBox;
  GeomBndLib_Curve(aBSpl).Add(aU1, aU2, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_Add3dCurve::Add(anAdaptor, aU1, aU2, Precision::Confusion(), anOldBox);

  CompareBoxes(aNewBox, anOldBox, Precision::Confusion());
}

// =========================================================================
// Adaptor constructor
// =========================================================================

TEST(GeomBndLib_CurveTest, AdaptorConstructor_Circle)
{
  const double             aRadius = 5.0;
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(gp::XOY(), aRadius);

  // Construct from handle
  GeomBndLib_Curve aCurveH(aCircle);
  Bnd_Box          aBoxH;
  aCurveH.Add(Precision::Confusion(), aBoxH);

  // Construct from adaptor
  GeomAdaptor_Curve anAdaptor(aCircle);
  GeomBndLib_Curve  aCurveA(anAdaptor);
  EXPECT_EQ(aCurveA.GetType(), GeomAbs_Circle);

  Bnd_Box aBoxA;
  aCurveA.Add(Precision::Confusion(), aBoxA);

  CompareBoxes(aBoxH, aBoxA, Precision::Confusion());
}

// =========================================================================
// AddOptimal
// =========================================================================

TEST(GeomBndLib_CurveTest, AddOptimal_Circle)
{
  const double             aRadius = 5.0;
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(gp::XOY(), aRadius);
  GeomBndLib_Curve         aCurve(aCircle);

  Bnd_Box aBox;
  aCurve.Add(Precision::Confusion(), aBox);

  Bnd_Box aBoxOpt;
  aCurve.AddOptimal(Precision::Confusion(), aBoxOpt);

  const auto [aXmin, aXmax, aYmin, aYmax, aZmin, aZmax]       = aBox.Get();
  const auto [aXminO, aXmaxO, aYminO, aYmaxO, aZminO, aZmaxO] = aBoxOpt.Get();

  // AddOptimal should produce a box no larger than Add
  EXPECT_GE(aXminO, aXmin - Precision::Confusion());
  EXPECT_GE(aYminO, aYmin - Precision::Confusion());
  EXPECT_GE(aZminO, aZmin - Precision::Confusion());
  EXPECT_LE(aXmaxO, aXmax + Precision::Confusion());
  EXPECT_LE(aYmaxO, aYmax + Precision::Confusion());
  EXPECT_LE(aZmaxO, aZmax + Precision::Confusion());
}

TEST(GeomBndLib_CurveTest, AddOptimal_CompareWithBndLib)
{
  occ::handle<Geom_Ellipse> anEllipse = new Geom_Ellipse(gp::XOY(), 8.0, 3.0);
  GeomAdaptor_Curve         anAdaptor(anEllipse);

  Bnd_Box aNewBox;
  GeomBndLib_Curve(anEllipse).AddOptimal(Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_Add3dCurve::AddOptimal(anAdaptor, Precision::Confusion(), anOldBox);

  CompareBoxes(aNewBox, anOldBox, Precision::Confusion());
}

TEST(GeomBndLib_CurveTest, AdaptorConstructor_TrimmedRangeUsedForFullAdd)
{
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(gp::XOY(), 10.0);
  GeomAdaptor_Curve        anAdaptor(aCircle, 0.0, M_PI / 2.0);

  Bnd_Box aNewBox;
  GeomBndLib_Curve(anAdaptor).Add(Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_Add3dCurve::Add(anAdaptor, Precision::Confusion(), anOldBox);

  CompareBoxes(aNewBox, anOldBox, Precision::Confusion());
}

TEST(GeomBndLib_CurveTest, TrimmedCurveHandle_Fallback_CompareWithBndLib)
{
  occ::handle<Geom_Circle>       aCircle = new Geom_Circle(gp::XOY(), 10.0);
  occ::handle<Geom_TrimmedCurve> aTrim   = new Geom_TrimmedCurve(aCircle, 0.0, M_PI / 2.0);
  GeomAdaptor_Curve              anAdaptor(aTrim);

  Bnd_Box aNewBox;
  GeomBndLib_Curve(aTrim).Add(Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_Add3dCurve::Add(anAdaptor, Precision::Confusion(), anOldBox);

  EXPECT_FALSE(aNewBox.IsVoid());
  ExpectConservativeEnvelope(aNewBox, anOldBox, Precision::Confusion());
}

TEST(GeomBndLib_CurveTest, TrimmedCurveHandle_UsesBasisSpecialization)
{
  occ::handle<Geom_Circle>       aCircle = new Geom_Circle(gp::XOY(), 10.0);
  occ::handle<Geom_TrimmedCurve> aTrim   = new Geom_TrimmedCurve(aCircle, 0.0, M_PI / 2.0);
  GeomBndLib_Curve               aCurve(aTrim);
  EXPECT_EQ(aCurve.GetType(), GeomAbs_Circle);

  GeomAdaptor_Curve anAdaptor(aTrim);
  Bnd_Box           aNewBox;
  aCurve.Add(Precision::Confusion(), aNewBox);
  Bnd_Box anOldBox;
  BndLib_Add3dCurve::Add(anAdaptor, Precision::Confusion(), anOldBox);
  CompareBoxes(aNewBox, anOldBox, Precision::Confusion());
}

TEST(GeomBndLib_CurveTest, SamplingHelpers3d_MatchesLegacyPolicy)
{
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(gp::XOY(), 10.0);
  GeomAdaptor_Curve        aCircleGA(aCircle);
  EXPECT_EQ(GeomBndLib_SamplingHelpers::ComputeNbSamples(aCircleGA, 0.0, 1.0), 33);

  constexpr int              aDegree  = 3;
  constexpr int              aNbKnots = 40;
  constexpr int              aNbPoles = 42;
  NCollection_Array1<gp_Pnt> aPoles(1, aNbPoles);
  for (int i = 1; i <= aNbPoles; ++i)
  {
    const double t = static_cast<double>(i - 1) / static_cast<double>(aNbPoles - 1);
    aPoles.SetValue(i, gp_Pnt(10.0 * t, t, 0.0));
  }
  NCollection_Array1<double> aKnots(1, aNbKnots);
  for (int i = 1; i <= aNbKnots; ++i)
  {
    aKnots.SetValue(i, static_cast<double>(i - 1));
  }
  NCollection_Array1<int> aMults(1, aNbKnots);
  aMults.SetValue(1, aDegree + 1);
  for (int i = 2; i < aNbKnots; ++i)
  {
    aMults.SetValue(i, 1);
  }
  aMults.SetValue(aNbKnots, aDegree + 1);

  occ::handle<Geom_BSplineCurve> aBSpl = new Geom_BSplineCurve(aPoles, aKnots, aMults, aDegree);
  GeomAdaptor_Curve              aBSplGA(aBSpl);
  EXPECT_EQ(GeomBndLib_SamplingHelpers::ComputeNbSamples(aBSplGA,
                                                         aBSpl->FirstParameter(),
                                                         aBSpl->LastParameter()),
            312);
}
