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

// Cross-validation tests comparing Geom2dProp_Curve against Geom2dLProp_CurAndInf2d
// for global curve analysis (curvature extrema and inflection point finding).

#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
Standard_DISABLE_DEPRECATION_WARNINGS
#include <Geom2dLProp_CurAndInf2d.hxx>
  Standard_ENABLE_DEPRECATION_WARNINGS
#include <Geom2dProp.hxx>
#include <Geom2dProp_Curve.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt2d.hxx>
    Standard_DISABLE_DEPRECATION_WARNINGS
#include <LProp_CIType.hxx>
      Standard_ENABLE_DEPRECATION_WARNINGS
#include <NCollection_Array1.hxx>

#include <gtest/gtest.h>

  namespace
{
  constexpr double THE_PARAM_TOL = 1.0e-4;

  //! Map LProp_CIType to Geom2dProp::CIType for comparison.
  Geom2dProp::CIType mapLPropType(const LProp_CIType theType)
  {
    switch (theType)
    {
      case LProp_Inflection:
        return Geom2dProp::CIType::Inflection;
      case LProp_MinCur:
        return Geom2dProp::CIType::MinCurvature;
      case LProp_MaxCur:
        return Geom2dProp::CIType::MaxCurvature;
    }
    return Geom2dProp::CIType::Inflection;
  }

  //! Compare extrema results from old and new APIs.
  void compareExtrema(const Geom2dProp::CurveAnalysis& theNew,
                      const Geom2dLProp_CurAndInf2d&   theOld,
                      const double                     theTol = THE_PARAM_TOL)
  {
    EXPECT_EQ(theNew.Points.Length(), theOld.NbPoints());

    const int aNb = std::min(theNew.Points.Length(), theOld.NbPoints());
    for (int i = 0; i < aNb; ++i)
    {
      EXPECT_NEAR(theNew.Points.Value(i).Parameter, theOld.Parameter(i + 1), theTol)
        << "Parameter mismatch at index " << i;
      EXPECT_EQ(theNew.Points.Value(i).Type, mapLPropType(theOld.Type(i + 1)))
        << "Type mismatch at index " << i;
    }
  }

} // namespace

// ============================================================================
// Circle - no extrema, no inflections
// ============================================================================

TEST(Geom2dProp_VsCurAndInf2dTest, Circle_NoExtrema)
{
  gp_Circ2d                  aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0);
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(aCirc);

  Geom2dLProp_CurAndInf2d anOld;
  anOld.PerformCurExt(aCircle);
  ASSERT_TRUE(anOld.IsDone());
  EXPECT_EQ(anOld.NbPoints(), 0);

  Geom2dProp_Curve                aProp(aCircle);
  const Geom2dProp::CurveAnalysis aNew = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aNew.IsDone);
  EXPECT_EQ(aNew.Points.Length(), 0);
}

TEST(Geom2dProp_VsCurAndInf2dTest, Circle_NoInflections)
{
  gp_Circ2d                  aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0);
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(aCirc);

  Geom2dLProp_CurAndInf2d anOld;
  anOld.PerformInf(aCircle);
  ASSERT_TRUE(anOld.IsDone());
  EXPECT_EQ(anOld.NbPoints(), 0);

  Geom2dProp_Curve                aProp(aCircle);
  const Geom2dProp::CurveAnalysis aNew = aProp.FindInflections();
  ASSERT_TRUE(aNew.IsDone);
  EXPECT_EQ(aNew.Points.Length(), 0);
}

// ============================================================================
// Ellipse - 4 extrema, no inflections
// ============================================================================

TEST(Geom2dProp_VsCurAndInf2dTest, Ellipse_Extrema)
{
  gp_Elips2d                  anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0, 5.0);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);

  Geom2dLProp_CurAndInf2d anOld;
  anOld.PerformCurExt(anEllipse);
  ASSERT_TRUE(anOld.IsDone());

  Geom2dProp_Curve                aProp(anEllipse);
  const Geom2dProp::CurveAnalysis aNew = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aNew.IsDone);

  compareExtrema(aNew, anOld, 1.0e-6);
}

TEST(Geom2dProp_VsCurAndInf2dTest, Ellipse_NoInflections)
{
  gp_Elips2d                  anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0, 5.0);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);

  Geom2dLProp_CurAndInf2d anOld;
  anOld.PerformInf(anEllipse);
  ASSERT_TRUE(anOld.IsDone());
  EXPECT_EQ(anOld.NbPoints(), 0);

  Geom2dProp_Curve                aProp(anEllipse);
  const Geom2dProp::CurveAnalysis aNew = aProp.FindInflections();
  ASSERT_TRUE(aNew.IsDone);
  EXPECT_EQ(aNew.Points.Length(), 0);
}

TEST(Geom2dProp_VsCurAndInf2dTest, Ellipse_HighEccentricity_Extrema)
{
  gp_Elips2d                  anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 50.0, 2.0);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);

  Geom2dLProp_CurAndInf2d anOld;
  anOld.PerformCurExt(anEllipse);
  ASSERT_TRUE(anOld.IsDone());

  Geom2dProp_Curve                aProp(anEllipse);
  const Geom2dProp::CurveAnalysis aNew = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aNew.IsDone);

  compareExtrema(aNew, anOld, 1.0e-6);
}

TEST(Geom2dProp_VsCurAndInf2dTest, Ellipse_FullPerform)
{
  gp_Elips2d                  anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 8.0, 3.0);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);

  Geom2dLProp_CurAndInf2d anOld;
  anOld.Perform(anEllipse);
  ASSERT_TRUE(anOld.IsDone());

  Geom2dProp_Curve                aProp(anEllipse);
  const Geom2dProp::CurveAnalysis aNewExt  = aProp.FindCurvatureExtrema();
  const Geom2dProp::CurveAnalysis aNewInfl = aProp.FindInflections();
  ASSERT_TRUE(aNewExt.IsDone);
  ASSERT_TRUE(aNewInfl.IsDone);

  const int aNewTotal = aNewExt.Points.Length() + aNewInfl.Points.Length();
  EXPECT_EQ(aNewTotal, anOld.NbPoints());
}

// ============================================================================
// Hyperbola - 1 extremum at vertex, no inflections
// ============================================================================

TEST(Geom2dProp_VsCurAndInf2dTest, Hyperbola_Extrema)
{
  gp_Hypr2d                     anHypr(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 6.0, 3.0);
  occ::handle<Geom2d_Hyperbola> aHyperbola = new Geom2d_Hyperbola(anHypr);

  Geom2dLProp_CurAndInf2d anOld;
  anOld.PerformCurExt(aHyperbola);
  ASSERT_TRUE(anOld.IsDone());

  Geom2dProp_Curve                aProp(aHyperbola);
  const Geom2dProp::CurveAnalysis aNew = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aNew.IsDone);

  compareExtrema(aNew, anOld, 1.0e-6);
}

TEST(Geom2dProp_VsCurAndInf2dTest, Hyperbola_NoInflections)
{
  gp_Hypr2d                     anHypr(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 6.0, 3.0);
  occ::handle<Geom2d_Hyperbola> aHyperbola = new Geom2d_Hyperbola(anHypr);

  Geom2dLProp_CurAndInf2d anOld;
  anOld.PerformInf(aHyperbola);
  ASSERT_TRUE(anOld.IsDone());
  EXPECT_EQ(anOld.NbPoints(), 0);

  Geom2dProp_Curve                aProp(aHyperbola);
  const Geom2dProp::CurveAnalysis aNew = aProp.FindInflections();
  ASSERT_TRUE(aNew.IsDone);
  EXPECT_EQ(aNew.Points.Length(), 0);
}

// ============================================================================
// Parabola - 1 extremum at vertex, no inflections
// ============================================================================

TEST(Geom2dProp_VsCurAndInf2dTest, Parabola_Extrema)
{
  gp_Parab2d                   aParab(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 2.0);
  occ::handle<Geom2d_Parabola> aParabola = new Geom2d_Parabola(aParab);

  Geom2dLProp_CurAndInf2d anOld;
  anOld.PerformCurExt(aParabola);
  ASSERT_TRUE(anOld.IsDone());

  Geom2dProp_Curve                aProp(aParabola);
  const Geom2dProp::CurveAnalysis aNew = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aNew.IsDone);

  compareExtrema(aNew, anOld, 1.0e-6);
}

TEST(Geom2dProp_VsCurAndInf2dTest, Parabola_NoInflections)
{
  gp_Parab2d                   aParab(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 2.0);
  occ::handle<Geom2d_Parabola> aParabola = new Geom2d_Parabola(aParab);

  Geom2dLProp_CurAndInf2d anOld;
  anOld.PerformInf(aParabola);
  ASSERT_TRUE(anOld.IsDone());
  EXPECT_EQ(anOld.NbPoints(), 0);

  Geom2dProp_Curve                aProp(aParabola);
  const Geom2dProp::CurveAnalysis aNew = aProp.FindInflections();
  ASSERT_TRUE(aNew.IsDone);
  EXPECT_EQ(aNew.Points.Length(), 0);
}

TEST(Geom2dProp_VsCurAndInf2dTest, Parabola_FullPerform)
{
  gp_Parab2d                   aParab(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0);
  occ::handle<Geom2d_Parabola> aParabola = new Geom2d_Parabola(aParab);

  Geom2dLProp_CurAndInf2d anOld;
  anOld.Perform(aParabola);
  ASSERT_TRUE(anOld.IsDone());

  Geom2dProp_Curve                aProp(aParabola);
  const Geom2dProp::CurveAnalysis aNewExt  = aProp.FindCurvatureExtrema();
  const Geom2dProp::CurveAnalysis aNewInfl = aProp.FindInflections();
  ASSERT_TRUE(aNewExt.IsDone);
  ASSERT_TRUE(aNewInfl.IsDone);

  const int aNewTotal = aNewExt.Points.Length() + aNewInfl.Points.Length();
  EXPECT_EQ(aNewTotal, anOld.NbPoints());
}

// ============================================================================
// Bezier - numeric extrema and inflections
// ============================================================================

TEST(Geom2dProp_VsCurAndInf2dTest, Bezier_CubicS_Inflections)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles(1)                               = gp_Pnt2d(0.0, 0.0);
  aPoles(2)                               = gp_Pnt2d(1.0, 2.0);
  aPoles(3)                               = gp_Pnt2d(3.0, -1.0);
  aPoles(4)                               = gp_Pnt2d(4.0, 1.0);
  occ::handle<Geom2d_BezierCurve> aBezier = new Geom2d_BezierCurve(aPoles);

  Geom2dLProp_CurAndInf2d anOld;
  anOld.PerformInf(aBezier);
  ASSERT_TRUE(anOld.IsDone());

  Geom2dProp_Curve                aProp(aBezier);
  const Geom2dProp::CurveAnalysis aNew = aProp.FindInflections();
  ASSERT_TRUE(aNew.IsDone);

  compareExtrema(aNew, anOld);
}

TEST(Geom2dProp_VsCurAndInf2dTest, Bezier_CubicS_Extrema)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles(1)                               = gp_Pnt2d(0.0, 0.0);
  aPoles(2)                               = gp_Pnt2d(1.0, 2.0);
  aPoles(3)                               = gp_Pnt2d(3.0, -1.0);
  aPoles(4)                               = gp_Pnt2d(4.0, 1.0);
  occ::handle<Geom2d_BezierCurve> aBezier = new Geom2d_BezierCurve(aPoles);

  Geom2dLProp_CurAndInf2d anOld;
  anOld.PerformCurExt(aBezier);
  ASSERT_TRUE(anOld.IsDone());

  Geom2dProp_Curve                aProp(aBezier);
  const Geom2dProp::CurveAnalysis aNew = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aNew.IsDone);

  compareExtrema(aNew, anOld);
}

TEST(Geom2dProp_VsCurAndInf2dTest, Bezier_CubicS_FullPerform)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles(1)                               = gp_Pnt2d(0.0, 0.0);
  aPoles(2)                               = gp_Pnt2d(1.0, 2.0);
  aPoles(3)                               = gp_Pnt2d(3.0, -2.0);
  aPoles(4)                               = gp_Pnt2d(4.0, 0.0);
  occ::handle<Geom2d_BezierCurve> aBezier = new Geom2d_BezierCurve(aPoles);

  Geom2dLProp_CurAndInf2d anOld;
  anOld.Perform(aBezier);
  ASSERT_TRUE(anOld.IsDone());

  Geom2dProp_Curve                aProp(aBezier);
  const Geom2dProp::CurveAnalysis aNewExt  = aProp.FindCurvatureExtrema();
  const Geom2dProp::CurveAnalysis aNewInfl = aProp.FindInflections();
  ASSERT_TRUE(aNewExt.IsDone);
  ASSERT_TRUE(aNewInfl.IsDone);

  const int aNewTotal = aNewExt.Points.Length() + aNewInfl.Points.Length();
  EXPECT_EQ(aNewTotal, anOld.NbPoints());
}

TEST(Geom2dProp_VsCurAndInf2dTest, Bezier_Quadratic_NoInflections)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles(1)                               = gp_Pnt2d(0.0, 0.0);
  aPoles(2)                               = gp_Pnt2d(2.0, 4.0);
  aPoles(3)                               = gp_Pnt2d(4.0, 0.0);
  occ::handle<Geom2d_BezierCurve> aBezier = new Geom2d_BezierCurve(aPoles);

  Geom2dLProp_CurAndInf2d anOld;
  anOld.PerformInf(aBezier);
  ASSERT_TRUE(anOld.IsDone());

  Geom2dProp_Curve                aProp(aBezier);
  const Geom2dProp::CurveAnalysis aNew = aProp.FindInflections();
  ASSERT_TRUE(aNew.IsDone);

  EXPECT_EQ(aNew.Points.Length(), anOld.NbPoints());
}

TEST(Geom2dProp_VsCurAndInf2dTest, Bezier_HighDegree_FullPerform)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 6);
  aPoles(1)                               = gp_Pnt2d(0.0, 0.0);
  aPoles(2)                               = gp_Pnt2d(1.0, 3.0);
  aPoles(3)                               = gp_Pnt2d(2.0, -1.0);
  aPoles(4)                               = gp_Pnt2d(3.0, 2.0);
  aPoles(5)                               = gp_Pnt2d(4.0, -2.0);
  aPoles(6)                               = gp_Pnt2d(5.0, 1.0);
  occ::handle<Geom2d_BezierCurve> aBezier = new Geom2d_BezierCurve(aPoles);

  Geom2dLProp_CurAndInf2d anOld;
  anOld.Perform(aBezier);
  ASSERT_TRUE(anOld.IsDone());

  Geom2dProp_Curve                aProp(aBezier);
  const Geom2dProp::CurveAnalysis aNewExt  = aProp.FindCurvatureExtrema();
  const Geom2dProp::CurveAnalysis aNewInfl = aProp.FindInflections();
  ASSERT_TRUE(aNewExt.IsDone);
  ASSERT_TRUE(aNewInfl.IsDone);

  const int aNewTotal = aNewExt.Points.Length() + aNewInfl.Points.Length();
  EXPECT_EQ(aNewTotal, anOld.NbPoints());
}

// ============================================================================
// BSpline - numeric with C3 interval subdivision
// ============================================================================

TEST(Geom2dProp_VsCurAndInf2dTest, BSpline_Degree4_FullPerform)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 5);
  aPoles(1) = gp_Pnt2d(0.0, 0.0);
  aPoles(2) = gp_Pnt2d(1.0, 3.0);
  aPoles(3) = gp_Pnt2d(2.0, -1.0);
  aPoles(4) = gp_Pnt2d(3.0, 2.0);
  aPoles(5) = gp_Pnt2d(4.0, 0.0);

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 5;
  aMults(2) = 5;

  occ::handle<Geom2d_BSplineCurve> aBSpline = new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 4);

  Geom2dLProp_CurAndInf2d anOld;
  anOld.Perform(aBSpline);
  ASSERT_TRUE(anOld.IsDone());

  Geom2dProp_Curve                aProp(aBSpline);
  const Geom2dProp::CurveAnalysis aNewExt  = aProp.FindCurvatureExtrema();
  const Geom2dProp::CurveAnalysis aNewInfl = aProp.FindInflections();
  ASSERT_TRUE(aNewExt.IsDone);
  ASSERT_TRUE(aNewInfl.IsDone);

  const int aNewTotal = aNewExt.Points.Length() + aNewInfl.Points.Length();
  EXPECT_EQ(aNewTotal, anOld.NbPoints());
}

TEST(Geom2dProp_VsCurAndInf2dTest, BSpline_Cubic_Extrema)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 6);
  aPoles(1) = gp_Pnt2d(0.0, 0.0);
  aPoles(2) = gp_Pnt2d(1.0, 3.0);
  aPoles(3) = gp_Pnt2d(2.0, 1.0);
  aPoles(4) = gp_Pnt2d(3.0, 4.0);
  aPoles(5) = gp_Pnt2d(4.0, 2.0);
  aPoles(6) = gp_Pnt2d(5.0, 0.0);

  NCollection_Array1<double> aKnots(1, 4);
  aKnots(1) = 0.0;
  aKnots(2) = 0.33;
  aKnots(3) = 0.66;
  aKnots(4) = 1.0;

  NCollection_Array1<int> aMults(1, 4);
  aMults(1) = 4;
  aMults(2) = 1;
  aMults(3) = 1;
  aMults(4) = 4;

  occ::handle<Geom2d_BSplineCurve> aBSpline = new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 3);

  Geom2dLProp_CurAndInf2d anOld;
  anOld.PerformCurExt(aBSpline);
  ASSERT_TRUE(anOld.IsDone());

  Geom2dProp_Curve                aProp(aBSpline);
  const Geom2dProp::CurveAnalysis aNew = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aNew.IsDone);

  compareExtrema(aNew, anOld);
}

TEST(Geom2dProp_VsCurAndInf2dTest, BSpline_Cubic_Inflections)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 6);
  aPoles(1) = gp_Pnt2d(0.0, 0.0);
  aPoles(2) = gp_Pnt2d(1.0, 3.0);
  aPoles(3) = gp_Pnt2d(2.0, 1.0);
  aPoles(4) = gp_Pnt2d(3.0, 4.0);
  aPoles(5) = gp_Pnt2d(4.0, 2.0);
  aPoles(6) = gp_Pnt2d(5.0, 0.0);

  NCollection_Array1<double> aKnots(1, 4);
  aKnots(1) = 0.0;
  aKnots(2) = 0.33;
  aKnots(3) = 0.66;
  aKnots(4) = 1.0;

  NCollection_Array1<int> aMults(1, 4);
  aMults(1) = 4;
  aMults(2) = 1;
  aMults(3) = 1;
  aMults(4) = 4;

  occ::handle<Geom2d_BSplineCurve> aBSpline = new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 3);

  Geom2dLProp_CurAndInf2d anOld;
  anOld.PerformInf(aBSpline);
  ASSERT_TRUE(anOld.IsDone());

  Geom2dProp_Curve                aProp(aBSpline);
  const Geom2dProp::CurveAnalysis aNew = aProp.FindInflections();
  ASSERT_TRUE(aNew.IsDone);

  compareExtrema(aNew, anOld);
}

TEST(Geom2dProp_VsCurAndInf2dTest, BSpline_LowContinuity_FullPerform)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 5);
  aPoles(1) = gp_Pnt2d(0.0, 0.0);
  aPoles(2) = gp_Pnt2d(1.0, 3.0);
  aPoles(3) = gp_Pnt2d(2.0, 1.0);
  aPoles(4) = gp_Pnt2d(3.0, 3.0);
  aPoles(5) = gp_Pnt2d(4.0, 0.0);

  NCollection_Array1<double> aKnots(1, 4);
  aKnots(1) = 0.0;
  aKnots(2) = 0.33;
  aKnots(3) = 0.66;
  aKnots(4) = 1.0;

  NCollection_Array1<int> aMults(1, 4);
  aMults(1) = 3;
  aMults(2) = 1;
  aMults(3) = 1;
  aMults(4) = 3;

  occ::handle<Geom2d_BSplineCurve> aBSpline = new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 2);

  Geom2dLProp_CurAndInf2d anOld;
  anOld.Perform(aBSpline);
  ASSERT_TRUE(anOld.IsDone());

  Geom2dProp_Curve                aProp(aBSpline);
  const Geom2dProp::CurveAnalysis aNewExt  = aProp.FindCurvatureExtrema();
  const Geom2dProp::CurveAnalysis aNewInfl = aProp.FindInflections();
  ASSERT_TRUE(aNewExt.IsDone);
  ASSERT_TRUE(aNewInfl.IsDone);

  const int aNewTotal = aNewExt.Points.Length() + aNewInfl.Points.Length();

  // The new C3-subdivision solver analyzes each smooth interval independently,
  // finding inflection points near knots that the old global solver misses.
  // Verify the new API finds at least as many points as the old.
  EXPECT_GE(aNewTotal, anOld.NbPoints());

  // Verify all old points are found by the new API.
  for (int i = 1; i <= anOld.NbPoints(); ++i)
  {
    const double anOldParam = anOld.Parameter(i);
    bool         aFound     = false;
    for (int j = 0; j < aNewExt.Points.Length(); ++j)
    {
      if (std::abs(aNewExt.Points[j].Parameter - anOldParam) < 1.0e-3)
      {
        aFound = true;
        break;
      }
    }
    if (!aFound)
    {
      for (int j = 0; j < aNewInfl.Points.Length(); ++j)
      {
        if (std::abs(aNewInfl.Points[j].Parameter - anOldParam) < 1.0e-3)
        {
          aFound = true;
          break;
        }
      }
    }
    EXPECT_TRUE(aFound) << "Old point at param=" << anOldParam << " not found in new results";
  }
}

// ============================================================================
// Trimmed curve - should work through unwrapping
// ============================================================================

TEST(Geom2dProp_VsCurAndInf2dTest, TrimmedEllipse_Extrema)
{
  gp_Elips2d                  anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0, 5.0);
  occ::handle<Geom2d_Ellipse> anEllipse     = new Geom2d_Ellipse(anElips);
  occ::handle<Geom2d_TrimmedCurve> aTrimmed = new Geom2d_TrimmedCurve(anEllipse, 0.0, M_PI);

  Geom2dLProp_CurAndInf2d anOld;
  anOld.PerformCurExt(aTrimmed);
  ASSERT_TRUE(anOld.IsDone());

  Geom2dProp_Curve                aProp(aTrimmed);
  const Geom2dProp::CurveAnalysis aNew = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aNew.IsDone);

  compareExtrema(aNew, anOld, 1.0e-6);
}

TEST(Geom2dProp_VsCurAndInf2dTest, TrimmedBezier_FullPerform)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles(1)                                 = gp_Pnt2d(0.0, 0.0);
  aPoles(2)                                 = gp_Pnt2d(1.0, 3.0);
  aPoles(3)                                 = gp_Pnt2d(3.0, -1.0);
  aPoles(4)                                 = gp_Pnt2d(4.0, 1.0);
  occ::handle<Geom2d_BezierCurve>  aBezier  = new Geom2d_BezierCurve(aPoles);
  occ::handle<Geom2d_TrimmedCurve> aTrimmed = new Geom2d_TrimmedCurve(aBezier, 0.1, 0.9);

  Geom2dLProp_CurAndInf2d anOld;
  anOld.Perform(aTrimmed);
  ASSERT_TRUE(anOld.IsDone());

  Geom2dProp_Curve                aProp(aTrimmed);
  const Geom2dProp::CurveAnalysis aNewExt  = aProp.FindCurvatureExtrema();
  const Geom2dProp::CurveAnalysis aNewInfl = aProp.FindInflections();
  ASSERT_TRUE(aNewExt.IsDone);
  ASSERT_TRUE(aNewInfl.IsDone);

  const int aNewTotal = aNewExt.Points.Length() + aNewInfl.Points.Length();
  EXPECT_EQ(aNewTotal, anOld.NbPoints());
}

// ============================================================================
// Offset curve - numeric
// ============================================================================

TEST(Geom2dProp_VsCurAndInf2dTest, OffsetEllipse_Extrema)
{
  gp_Elips2d                  anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0, 5.0);
  occ::handle<Geom2d_Ellipse> anEllipse    = new Geom2d_Ellipse(anElips);
  occ::handle<Geom2d_OffsetCurve> anOffset = new Geom2d_OffsetCurve(anEllipse, 1.0);

  Geom2dLProp_CurAndInf2d anOld;
  anOld.PerformCurExt(anOffset);
  ASSERT_TRUE(anOld.IsDone());

  Geom2dProp_Curve                aProp(anOffset);
  const Geom2dProp::CurveAnalysis aNew = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aNew.IsDone);

  compareExtrema(aNew, anOld);
}

TEST(Geom2dProp_VsCurAndInf2dTest, OffsetEllipse_Inflections)
{
  gp_Elips2d                  anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0, 5.0);
  occ::handle<Geom2d_Ellipse> anEllipse    = new Geom2d_Ellipse(anElips);
  occ::handle<Geom2d_OffsetCurve> anOffset = new Geom2d_OffsetCurve(anEllipse, 1.0);

  Geom2dLProp_CurAndInf2d anOld;
  anOld.PerformInf(anOffset);
  ASSERT_TRUE(anOld.IsDone());

  Geom2dProp_Curve                aProp(anOffset);
  const Geom2dProp::CurveAnalysis aNew = aProp.FindInflections();
  ASSERT_TRUE(aNew.IsDone);

  EXPECT_EQ(aNew.Points.Length(), anOld.NbPoints());
}

TEST(Geom2dProp_VsCurAndInf2dTest, OffsetCircle_NoExtrema)
{
  gp_Circ2d                       aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0);
  occ::handle<Geom2d_Circle>      aCircle  = new Geom2d_Circle(aCirc);
  occ::handle<Geom2d_OffsetCurve> anOffset = new Geom2d_OffsetCurve(aCircle, 2.0);

  Geom2dLProp_CurAndInf2d anOld;
  anOld.PerformCurExt(anOffset);
  ASSERT_TRUE(anOld.IsDone());

  Geom2dProp_Curve                aProp(anOffset);
  const Geom2dProp::CurveAnalysis aNew = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aNew.IsDone);

  EXPECT_EQ(aNew.Points.Length(), anOld.NbPoints());
}
