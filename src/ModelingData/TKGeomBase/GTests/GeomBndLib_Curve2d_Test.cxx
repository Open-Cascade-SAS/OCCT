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

#include <GeomBndLib_Curve2d.hxx>

#include <BndLib_Add2dCurve.hxx>
#include <Bnd_Box2d.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomBndLib_SamplingHelpers.pxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Pnt2d.hxx>

#include <gtest/gtest.h>

namespace
{

//! Helper: compare two Bnd_Box2d results within tolerance.
void CompareBoxes2d(const Bnd_Box2d& theNew, const Bnd_Box2d& theOld, const double theTol)
{
  const auto [aXminN, aXmaxN, aYminN, aYmaxN] = theNew.Get();
  const auto [aXminO, aXmaxO, aYminO, aYmaxO] = theOld.Get();
  EXPECT_NEAR(aXminN, aXminO, theTol);
  EXPECT_NEAR(aXmaxN, aXmaxO, theTol);
  EXPECT_NEAR(aYminN, aYminO, theTol);
  EXPECT_NEAR(aYmaxN, aYmaxO, theTol);
}

//! Helper: verify that the new box conservatively encloses the old box.
void ExpectConservativeEnvelope2d(const Bnd_Box2d& theNew, const Bnd_Box2d& theOld, const double theTol)
{
  const auto [aXminN, aXmaxN, aYminN, aYmaxN] = theNew.Get();
  const auto [aXminO, aXmaxO, aYminO, aYmaxO] = theOld.Get();
  EXPECT_LE(aXminN, aXminO + theTol);
  EXPECT_GE(aXmaxN, aXmaxO - theTol);
  EXPECT_LE(aYminN, aYminO + theTol);
  EXPECT_GE(aYmaxN, aYmaxO - theTol);
}

} // namespace

// =========================================================================
// Line2d
// =========================================================================

TEST(GeomBndLib_Curve2dTest, Line_FiniteSegment)
{
  Handle(Geom2d_Line) aLine = new Geom2d_Line(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 1.0));
  GeomBndLib_Curve2d  aCurve(aLine);
  EXPECT_EQ(aCurve.GetType(), GeomAbs_Line);

  Bnd_Box2d aBox;
  aCurve.Add(0.0, 10.0, 0.0, aBox);

  const auto [aXmin, aXmax, aYmin, aYmax] = aBox.Get();

  // Line from (0,0) in direction (1,1) normalized, parameter 10
  const double aLen = 10.0 / std::sqrt(2.0);
  EXPECT_NEAR(aXmin, 0.0, Precision::Confusion());
  EXPECT_NEAR(aYmin, 0.0, Precision::Confusion());
  EXPECT_NEAR(aXmax, aLen, Precision::Confusion());
  EXPECT_NEAR(aYmax, aLen, Precision::Confusion());
}

TEST(GeomBndLib_Curve2dTest, Line_CompareWithBndLib)
{
  Handle(Geom2d_Line)  aLine = new Geom2d_Line(gp_Pnt2d(1.0, 2.0), gp_Dir2d(0.0, 1.0));
  Geom2dAdaptor_Curve  anAdaptor(aLine);

  Bnd_Box2d aNewBox;
  GeomBndLib_Curve2d(aLine).Add(0.0, 5.0, Precision::Confusion(), aNewBox);

  Bnd_Box2d anOldBox;
  BndLib_Add2dCurve::Add(anAdaptor, 0.0, 5.0, Precision::Confusion(), anOldBox);

  CompareBoxes2d(aNewBox, anOldBox, Precision::Confusion());
}

// =========================================================================
// Circle2d
// =========================================================================

TEST(GeomBndLib_Curve2dTest, Circle_Full)
{
  const double          aRadius = 5.0;
  Handle(Geom2d_Circle) aCircle =
    new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), aRadius);
  GeomBndLib_Curve2d aCurve(aCircle);
  EXPECT_EQ(aCurve.GetType(), GeomAbs_Circle);

  Bnd_Box2d aBox;
  aCurve.Add(0.0, aBox);

  const auto [aXmin, aXmax, aYmin, aYmax] = aBox.Get();

  EXPECT_NEAR(aXmin, -aRadius, Precision::Confusion());
  EXPECT_NEAR(aYmin, -aRadius, Precision::Confusion());
  EXPECT_NEAR(aXmax, aRadius, Precision::Confusion());
  EXPECT_NEAR(aYmax, aRadius, Precision::Confusion());
}

TEST(GeomBndLib_Curve2dTest, Circle_Arc)
{
  const double          aRadius = 10.0;
  Handle(Geom2d_Circle) aCircle =
    new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), aRadius);
  GeomBndLib_Curve2d aCurve(aCircle);

  Bnd_Box2d aBox;
  // Pass 0 tolerance for precise reference comparison
  aCurve.Add(0.0, M_PI / 2.0, 0.0, aBox);

  const auto [aXmin, aXmax, aYmin, aYmax] = aBox.Get();

  EXPECT_NEAR(aXmin, 0.0, Precision::Confusion());
  EXPECT_NEAR(aYmin, 0.0, Precision::Confusion());
  EXPECT_NEAR(aXmax, aRadius, Precision::Confusion());
  EXPECT_NEAR(aYmax, aRadius, Precision::Confusion());
}

TEST(GeomBndLib_Curve2dTest, Circle_CompareWithBndLib)
{
  Handle(Geom2d_Circle) aCircle =
    new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 7.5);
  Geom2dAdaptor_Curve anAdaptor(aCircle);

  Bnd_Box2d aNewBox;
  GeomBndLib_Curve2d(aCircle).Add(Precision::Confusion(), aNewBox);

  Bnd_Box2d anOldBox;
  BndLib_Add2dCurve::Add(anAdaptor, Precision::Confusion(), anOldBox);

  CompareBoxes2d(aNewBox, anOldBox, Precision::Confusion());
}

// =========================================================================
// Ellipse2d
// =========================================================================

TEST(GeomBndLib_Curve2dTest, Ellipse_Full)
{
  const double aMajR = 10.0;
  const double aMinR = 5.0;
  Handle(Geom2d_Ellipse) anEllipse =
    new Geom2d_Ellipse(gp_Ax22d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0), gp_Dir2d(0.0, 1.0)),
                       aMajR,
                       aMinR);
  GeomBndLib_Curve2d aCurve(anEllipse);
  EXPECT_EQ(aCurve.GetType(), GeomAbs_Ellipse);

  Bnd_Box2d aBox;
  aCurve.Add(0.0, aBox);

  const auto [aXmin, aXmax, aYmin, aYmax] = aBox.Get();

  EXPECT_NEAR(aXmin, -aMajR, Precision::Confusion());
  EXPECT_NEAR(aXmax, aMajR, Precision::Confusion());
  EXPECT_NEAR(aYmin, -aMinR, Precision::Confusion());
  EXPECT_NEAR(aYmax, aMinR, Precision::Confusion());
}

TEST(GeomBndLib_Curve2dTest, Ellipse_CompareWithBndLib)
{
  Handle(Geom2d_Ellipse) anEllipse =
    new Geom2d_Ellipse(gp_Ax22d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0), gp_Dir2d(0.0, 1.0)),
                       8.0,
                       3.0);
  Geom2dAdaptor_Curve anAdaptor(anEllipse);

  Bnd_Box2d aNewBox;
  GeomBndLib_Curve2d(anEllipse).Add(Precision::Confusion(), aNewBox);

  Bnd_Box2d anOldBox;
  BndLib_Add2dCurve::Add(anAdaptor, Precision::Confusion(), anOldBox);

  CompareBoxes2d(aNewBox, anOldBox, Precision::Confusion());
}

// =========================================================================
// Hyperbola2d
// =========================================================================

TEST(GeomBndLib_Curve2dTest, Hyperbola_FiniteArc)
{
  const double             aMajR = 5.0;
  const double             aMinR = 3.0;
  Handle(Geom2d_Hyperbola) aHyp =
    new Geom2d_Hyperbola(gp_Ax22d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0), gp_Dir2d(0.0, 1.0)),
                         aMajR,
                         aMinR);
  GeomBndLib_Curve2d aCurve(aHyp);
  EXPECT_EQ(aCurve.GetType(), GeomAbs_Hyperbola);

  Bnd_Box2d    aBox;
  const double aU = 1.0;
  aCurve.Add(-aU, aU, 0.0, aBox);

  const auto [aXmin, aXmax, aYmin, aYmax] = aBox.Get();

  const double aXEnd = aMajR * std::cosh(aU);
  const double aYEnd = aMinR * std::sinh(aU);

  EXPECT_LE(aXmin, aMajR + Precision::Confusion());
  EXPECT_GE(aXmax, aXEnd - Precision::Confusion());
  EXPECT_LE(aYmin, -aYEnd + Precision::Confusion());
  EXPECT_GE(aYmax, aYEnd - Precision::Confusion());
}

TEST(GeomBndLib_Curve2dTest, Hyperbola_CompareWithBndLib)
{
  Handle(Geom2d_Hyperbola) aHyp =
    new Geom2d_Hyperbola(gp_Ax22d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0), gp_Dir2d(0.0, 1.0)),
                         5.0,
                         3.0);
  Geom2dAdaptor_Curve anAdaptor(aHyp);

  Bnd_Box2d aNewBox;
  GeomBndLib_Curve2d(aHyp).Add(-1.0, 1.0, Precision::Confusion(), aNewBox);

  Bnd_Box2d anOldBox;
  BndLib_Add2dCurve::Add(anAdaptor, -1.0, 1.0, Precision::Confusion(), anOldBox);

  CompareBoxes2d(aNewBox, anOldBox, Precision::Confusion());
}

// =========================================================================
// Parabola2d
// =========================================================================

TEST(GeomBndLib_Curve2dTest, Parabola_FiniteArc)
{
  const double             aFocal = 2.0;
  Handle(Geom2d_Parabola)  aParab =
    new Geom2d_Parabola(gp_Ax22d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0), gp_Dir2d(0.0, 1.0)),
                        aFocal);
  GeomBndLib_Curve2d aCurve(aParab);
  EXPECT_EQ(aCurve.GetType(), GeomAbs_Parabola);

  Bnd_Box2d    aBox;
  const double aU = 5.0;
  aCurve.Add(-aU, aU, 0.0, aBox);

  const auto [aXmin, aXmax, aYmin, aYmax] = aBox.Get();

  const double aXEnd = aU * aU / (4.0 * aFocal);

  EXPECT_LE(aXmin, Precision::Confusion());
  EXPECT_GE(aXmax, aXEnd - Precision::Confusion());
  EXPECT_LE(aYmin, -aU + Precision::Confusion());
  EXPECT_GE(aYmax, aU - Precision::Confusion());
}

TEST(GeomBndLib_Curve2dTest, Parabola_CompareWithBndLib)
{
  Handle(Geom2d_Parabola) aParab =
    new Geom2d_Parabola(gp_Ax22d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0), gp_Dir2d(0.0, 1.0)),
                        2.0);
  Geom2dAdaptor_Curve anAdaptor(aParab);

  Bnd_Box2d aNewBox;
  GeomBndLib_Curve2d(aParab).Add(-5.0, 5.0, Precision::Confusion(), aNewBox);

  Bnd_Box2d anOldBox;
  BndLib_Add2dCurve::Add(anAdaptor, -5.0, 5.0, Precision::Confusion(), anOldBox);

  CompareBoxes2d(aNewBox, anOldBox, Precision::Confusion());
}

// =========================================================================
// BezierCurve2d
// =========================================================================

TEST(GeomBndLib_Curve2dTest, BezierCurve_Simple)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt2d(5.0, 10.0));
  aPoles.SetValue(3, gp_Pnt2d(10.0, 0.0));

  Handle(Geom2d_BezierCurve) aBez = new Geom2d_BezierCurve(aPoles);
  GeomBndLib_Curve2d          aCurve(aBez);
  EXPECT_EQ(aCurve.GetType(), GeomAbs_BezierCurve);

  Bnd_Box2d aBox;
  aCurve.Add(Precision::Confusion(), aBox);

  const auto [aXmin, aXmax, aYmin, aYmax] = aBox.Get();

  EXPECT_LE(aXmin, Precision::Confusion());
  EXPECT_GE(aXmax, 10.0 - Precision::Confusion());
  EXPECT_LE(aYmin, Precision::Confusion());
  EXPECT_GE(aYmax, 0.0);
}

TEST(GeomBndLib_Curve2dTest, BezierCurve_CompareWithBndLib)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt2d(3.0, 8.0));
  aPoles.SetValue(3, gp_Pnt2d(7.0, 2.0));
  aPoles.SetValue(4, gp_Pnt2d(10.0, 5.0));

  Handle(Geom2d_BezierCurve) aBez = new Geom2d_BezierCurve(aPoles);
  Geom2dAdaptor_Curve        anAdaptor(aBez);

  Bnd_Box2d aNewBox;
  GeomBndLib_Curve2d(aBez).Add(Precision::Confusion(), aNewBox);

  Bnd_Box2d anOldBox;
  BndLib_Add2dCurve::Add(anAdaptor, Precision::Confusion(), anOldBox);

  // New box should be no larger than old box (may be tighter due to convex hull approach)
  const auto [aXminN, aXmaxN, aYminN, aYmaxN] = aNewBox.Get();
  const auto [aXminO, aXmaxO, aYminO, aYmaxO] = anOldBox.Get();
  EXPECT_GE(aXminN, aXminO - Precision::Confusion());
  EXPECT_LE(aXmaxN, aXmaxO + Precision::Confusion());
  EXPECT_GE(aYminN, aYminO - Precision::Confusion());
  EXPECT_LE(aYmaxN, aYmaxO + Precision::Confusion());
}

// =========================================================================
// BSplineCurve2d
// =========================================================================

TEST(GeomBndLib_Curve2dTest, BSplineCurve_Simple)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt2d(2.0, 5.0));
  aPoles.SetValue(3, gp_Pnt2d(8.0, 5.0));
  aPoles.SetValue(4, gp_Pnt2d(10.0, 0.0));

  NCollection_Array1<double> aKnots(1, 3);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 0.5);
  aKnots.SetValue(3, 1.0);

  NCollection_Array1<int> aMults(1, 3);
  aMults.SetValue(1, 3);
  aMults.SetValue(2, 1);
  aMults.SetValue(3, 3);

  Handle(Geom2d_BSplineCurve) aBSpl = new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 2);
  GeomBndLib_Curve2d           aCurve(aBSpl);
  EXPECT_EQ(aCurve.GetType(), GeomAbs_BSplineCurve);

  Bnd_Box2d aBox;
  aCurve.Add(Precision::Confusion(), aBox);

  const auto [aXmin, aXmax, aYmin, aYmax] = aBox.Get();

  EXPECT_LE(aXmin, Precision::Confusion());
  EXPECT_GE(aXmax, 10.0 - Precision::Confusion());
  EXPECT_LE(aYmin, Precision::Confusion());
  EXPECT_GE(aYmax, 0.0);
}

TEST(GeomBndLib_Curve2dTest, BSplineCurve_CompareWithBndLib)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt2d(2.0, 5.0));
  aPoles.SetValue(3, gp_Pnt2d(8.0, 5.0));
  aPoles.SetValue(4, gp_Pnt2d(10.0, 0.0));

  NCollection_Array1<double> aKnots(1, 3);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 0.5);
  aKnots.SetValue(3, 1.0);

  NCollection_Array1<int> aMults(1, 3);
  aMults.SetValue(1, 3);
  aMults.SetValue(2, 1);
  aMults.SetValue(3, 3);

  Handle(Geom2d_BSplineCurve) aBSpl = new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 2);
  Geom2dAdaptor_Curve         anAdaptor(aBSpl);

  Bnd_Box2d aNewBox;
  GeomBndLib_Curve2d(aBSpl).Add(Precision::Confusion(), aNewBox);

  Bnd_Box2d anOldBox;
  BndLib_Add2dCurve::Add(anAdaptor, Precision::Confusion(), anOldBox);

  CompareBoxes2d(aNewBox, anOldBox, Precision::Confusion());
}

// =========================================================================
// Adaptor constructor
// =========================================================================

TEST(GeomBndLib_Curve2dTest, AdaptorConstructor_Circle)
{
  const double          aRadius = 5.0;
  Handle(Geom2d_Circle) aCircle =
    new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), aRadius);

  // Construct from handle
  GeomBndLib_Curve2d aCurveH(aCircle);
  Bnd_Box2d          aBoxH;
  aCurveH.Add(Precision::Confusion(), aBoxH);

  // Construct from adaptor
  Geom2dAdaptor_Curve anAdaptor(aCircle);
  GeomBndLib_Curve2d  aCurveA(anAdaptor);
  EXPECT_EQ(aCurveA.GetType(), GeomAbs_Circle);

  Bnd_Box2d aBoxA;
  aCurveA.Add(Precision::Confusion(), aBoxA);

  CompareBoxes2d(aBoxH, aBoxA, Precision::Confusion());
}

// =========================================================================
// AddOptimal
// =========================================================================

TEST(GeomBndLib_Curve2dTest, AddOptimal_Circle)
{
  const double          aRadius = 5.0;
  Handle(Geom2d_Circle) aCircle =
    new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), aRadius);
  GeomBndLib_Curve2d aCurve(aCircle);

  Bnd_Box2d aBoxOpt;
  aCurve.AddOptimal(0.0, aBoxOpt);

  const auto [aXminO, aXmaxO, aYminO, aYmaxO] = aBoxOpt.Get();

  // Analytical circle should give precise box
  EXPECT_NEAR(aXminO, -aRadius, Precision::Confusion());
  EXPECT_NEAR(aYminO, -aRadius, Precision::Confusion());
  EXPECT_NEAR(aXmaxO, aRadius, Precision::Confusion());
  EXPECT_NEAR(aYmaxO, aRadius, Precision::Confusion());
}

TEST(GeomBndLib_Curve2dTest, AdaptorConstructor_TrimmedRangeUsedForFullAdd)
{
  Handle(Geom2d_Circle) aCircle =
    new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0);
  Geom2dAdaptor_Curve anAdaptor(aCircle, 0.0, M_PI / 2.0);

  Bnd_Box2d aNewBox;
  GeomBndLib_Curve2d(anAdaptor).Add(Precision::Confusion(), aNewBox);

  Bnd_Box2d anOldBox;
  BndLib_Add2dCurve::Add(anAdaptor, Precision::Confusion(), anOldBox);

  CompareBoxes2d(aNewBox, anOldBox, Precision::Confusion());
}

TEST(GeomBndLib_Curve2dTest, TrimmedCurveHandle_Fallback_CompareWithBndLib)
{
  Handle(Geom2d_Circle) aCircle =
    new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0);
  Handle(Geom2d_TrimmedCurve) aTrim = new Geom2d_TrimmedCurve(aCircle, 0.0, M_PI / 2.0);
  Geom2dAdaptor_Curve         anAdaptor(aTrim);

  Bnd_Box2d aNewBox;
  GeomBndLib_Curve2d(aTrim).Add(Precision::Confusion(), aNewBox);

  Bnd_Box2d anOldBox;
  BndLib_Add2dCurve::Add(anAdaptor, Precision::Confusion(), anOldBox);

  EXPECT_FALSE(aNewBox.IsVoid());
  ExpectConservativeEnvelope2d(aNewBox, anOldBox, Precision::Confusion());
}

TEST(GeomBndLib_Curve2dTest, TrimmedCurveHandle_UsesBasisSpecialization)
{
  Handle(Geom2d_Circle) aCircle =
    new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0);
  Handle(Geom2d_TrimmedCurve) aTrim = new Geom2d_TrimmedCurve(aCircle, 0.0, M_PI / 2.0);
  GeomBndLib_Curve2d          aCurve(aTrim);
  EXPECT_EQ(aCurve.GetType(), GeomAbs_Circle);

  Geom2dAdaptor_Curve anAdaptor(aTrim);
  Bnd_Box2d           aNewBox;
  aCurve.Add(Precision::Confusion(), aNewBox);
  Bnd_Box2d anOldBox;
  BndLib_Add2dCurve::Add(anAdaptor, Precision::Confusion(), anOldBox);
  CompareBoxes2d(aNewBox, anOldBox, Precision::Confusion());
}

TEST(GeomBndLib_Curve2dTest, SamplingHelpers2d_MatchesLegacyPolicy)
{
  Handle(Geom2d_Circle) aCircle =
    new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0);
  Geom2dAdaptor_Curve aCircleGA(aCircle);
  EXPECT_EQ(GeomBndLib_SamplingHelpers::ComputeNbSamples2d(aCircleGA, 0.0, 1.0), 17);

  constexpr int aDegree  = 3;
  constexpr int aNbKnots = 40;
  constexpr int aNbPoles = 42;
  NCollection_Array1<gp_Pnt2d> aPoles(1, aNbPoles);
  for (int i = 1; i <= aNbPoles; ++i)
  {
    const double t = static_cast<double>(i - 1) / static_cast<double>(aNbPoles - 1);
    aPoles.SetValue(i, gp_Pnt2d(10.0 * t, t));
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

  Handle(Geom2d_BSplineCurve) aBSpl = new Geom2d_BSplineCurve(aPoles, aKnots, aMults, aDegree);
  Geom2dAdaptor_Curve         aBSplGA(aBSpl);
  EXPECT_EQ(GeomBndLib_SamplingHelpers::ComputeNbSamples2d(aBSplGA,
                                                            aBSpl->FirstParameter(),
                                                            aBSpl->LastParameter()),
            23);
}
