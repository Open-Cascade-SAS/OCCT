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

#include <GeomHash_CurveHasher.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_OffsetCurve.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>

class GeomHash_CurveHasherTest : public ::testing::Test
{
protected:
  GeomHash_CurveHasher myHasher;
};

// ============================================================================
// Line Tests
// ============================================================================

TEST_F(GeomHash_CurveHasherTest, Line_CopiedLines_SameHash)
{
  gp_Pnt            aLoc(1.0, 2.0, 3.0);
  gp_Dir            aDir(1.0, 0.0, 0.0);
  Handle(Geom_Line) aLine1 = new Geom_Line(aLoc, aDir);
  Handle(Geom_Line) aLine2 = Handle(Geom_Line)::DownCast(aLine1->Copy());

  EXPECT_EQ(myHasher(aLine1), myHasher(aLine2));
  EXPECT_TRUE(myHasher(aLine1, aLine2));
}

TEST_F(GeomHash_CurveHasherTest, Line_DifferentLines_DifferentHash)
{
  Handle(Geom_Line) aLine1 = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  Handle(Geom_Line) aLine2 = new Geom_Line(gp_Pnt(1.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));

  EXPECT_NE(myHasher(aLine1), myHasher(aLine2));
  EXPECT_FALSE(myHasher(aLine1, aLine2));
}

// ============================================================================
// Circle Tests
// ============================================================================

TEST_F(GeomHash_CurveHasherTest, Circle_CopiedCircles_SameHash)
{
  gp_Ax2              anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Circle) aCircle1 = new Geom_Circle(anAxis, 5.0);
  Handle(Geom_Circle) aCircle2 = Handle(Geom_Circle)::DownCast(aCircle1->Copy());

  EXPECT_EQ(myHasher(aCircle1), myHasher(aCircle2));
  EXPECT_TRUE(myHasher(aCircle1, aCircle2));
}

TEST_F(GeomHash_CurveHasherTest, Circle_DifferentRadius_DifferentHash)
{
  gp_Ax2              anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Circle) aCircle1 = new Geom_Circle(anAxis, 5.0);
  Handle(Geom_Circle) aCircle2 = new Geom_Circle(anAxis, 10.0);

  EXPECT_NE(myHasher(aCircle1), myHasher(aCircle2));
  EXPECT_FALSE(myHasher(aCircle1, aCircle2));
}

// ============================================================================
// Ellipse Tests
// ============================================================================

TEST_F(GeomHash_CurveHasherTest, Ellipse_CopiedEllipses_SameHash)
{
  gp_Ax2               anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Ellipse) anEllipse1 = new Geom_Ellipse(anAxis, 10.0, 5.0);
  Handle(Geom_Ellipse) anEllipse2 = Handle(Geom_Ellipse)::DownCast(anEllipse1->Copy());

  EXPECT_EQ(myHasher(anEllipse1), myHasher(anEllipse2));
  EXPECT_TRUE(myHasher(anEllipse1, anEllipse2));
}

TEST_F(GeomHash_CurveHasherTest, Ellipse_DifferentRadii_DifferentHash)
{
  gp_Ax2               anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Ellipse) anEllipse1 = new Geom_Ellipse(anAxis, 10.0, 5.0);
  Handle(Geom_Ellipse) anEllipse2 = new Geom_Ellipse(anAxis, 10.0, 7.0);

  EXPECT_NE(myHasher(anEllipse1), myHasher(anEllipse2));
  EXPECT_FALSE(myHasher(anEllipse1, anEllipse2));
}

// ============================================================================
// Hyperbola Tests
// ============================================================================

TEST_F(GeomHash_CurveHasherTest, Hyperbola_CopiedHyperbolas_SameHash)
{
  gp_Ax2                 anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Hyperbola) aHyp1 = new Geom_Hyperbola(anAxis, 5.0, 3.0);
  Handle(Geom_Hyperbola) aHyp2 = Handle(Geom_Hyperbola)::DownCast(aHyp1->Copy());

  EXPECT_EQ(myHasher(aHyp1), myHasher(aHyp2));
  EXPECT_TRUE(myHasher(aHyp1, aHyp2));
}

TEST_F(GeomHash_CurveHasherTest, Hyperbola_DifferentRadii_DifferentHash)
{
  gp_Ax2                 anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Hyperbola) aHyp1 = new Geom_Hyperbola(anAxis, 5.0, 3.0);
  Handle(Geom_Hyperbola) aHyp2 = new Geom_Hyperbola(anAxis, 5.0, 4.0);

  EXPECT_NE(myHasher(aHyp1), myHasher(aHyp2));
  EXPECT_FALSE(myHasher(aHyp1, aHyp2));
}

// ============================================================================
// Parabola Tests
// ============================================================================

TEST_F(GeomHash_CurveHasherTest, Parabola_CopiedParabolas_SameHash)
{
  gp_Ax2                anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Parabola) aPar1 = new Geom_Parabola(anAxis, 2.0);
  Handle(Geom_Parabola) aPar2 = Handle(Geom_Parabola)::DownCast(aPar1->Copy());

  EXPECT_EQ(myHasher(aPar1), myHasher(aPar2));
  EXPECT_TRUE(myHasher(aPar1, aPar2));
}

TEST_F(GeomHash_CurveHasherTest, Parabola_DifferentFocal_DifferentHash)
{
  gp_Ax2                anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Parabola) aPar1 = new Geom_Parabola(anAxis, 2.0);
  Handle(Geom_Parabola) aPar2 = new Geom_Parabola(anAxis, 3.0);

  EXPECT_NE(myHasher(aPar1), myHasher(aPar2));
  EXPECT_FALSE(myHasher(aPar1, aPar2));
}

// ============================================================================
// BezierCurve Tests
// ============================================================================

TEST_F(GeomHash_CurveHasherTest, BezierCurve_CopiedCurves_SameHash)
{
  TColgp_Array1OfPnt aPoles(1, 4);
  aPoles(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(2) = gp_Pnt(1.0, 2.0, 0.0);
  aPoles(3) = gp_Pnt(3.0, 2.0, 0.0);
  aPoles(4) = gp_Pnt(4.0, 0.0, 0.0);

  Handle(Geom_BezierCurve) aCurve1 = new Geom_BezierCurve(aPoles);
  Handle(Geom_BezierCurve) aCurve2 = Handle(Geom_BezierCurve)::DownCast(aCurve1->Copy());

  EXPECT_EQ(myHasher(aCurve1), myHasher(aCurve2));
  EXPECT_TRUE(myHasher(aCurve1, aCurve2));
}

TEST_F(GeomHash_CurveHasherTest, BezierCurve_DifferentPoles_DifferentComparison)
{
  TColgp_Array1OfPnt aPoles1(1, 3);
  aPoles1(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles1(2) = gp_Pnt(1.0, 2.0, 0.0);
  aPoles1(3) = gp_Pnt(2.0, 0.0, 0.0);

  TColgp_Array1OfPnt aPoles2(1, 3);
  aPoles2(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles2(2) = gp_Pnt(1.0, 3.0, 0.0); // Different
  aPoles2(3) = gp_Pnt(2.0, 0.0, 0.0);

  Handle(Geom_BezierCurve) aCurve1 = new Geom_BezierCurve(aPoles1);
  Handle(Geom_BezierCurve) aCurve2 = new Geom_BezierCurve(aPoles2);

  // Hash may be same (metadata only), but comparison should differ
  EXPECT_FALSE(myHasher(aCurve1, aCurve2));
}

// ============================================================================
// BSplineCurve Tests
// ============================================================================

TEST_F(GeomHash_CurveHasherTest, BSplineCurve_CopiedCurves_SameHash)
{
  TColgp_Array1OfPnt aPoles(1, 4);
  aPoles(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(2) = gp_Pnt(1.0, 1.0, 0.0);
  aPoles(3) = gp_Pnt(2.0, 1.0, 0.0);
  aPoles(4) = gp_Pnt(3.0, 0.0, 0.0);

  TColStd_Array1OfReal aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  TColStd_Array1OfInteger aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  Handle(Geom_BSplineCurve) aCurve1 = new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);
  Handle(Geom_BSplineCurve) aCurve2 = Handle(Geom_BSplineCurve)::DownCast(aCurve1->Copy());

  EXPECT_EQ(myHasher(aCurve1), myHasher(aCurve2));
  EXPECT_TRUE(myHasher(aCurve1, aCurve2));
}

// ============================================================================
// TrimmedCurve Tests
// ============================================================================

TEST_F(GeomHash_CurveHasherTest, TrimmedCurve_CopiedCurves_SameHash)
{
  Handle(Geom_Line)         aLine     = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  Handle(Geom_TrimmedCurve) aTrimmed1 = new Geom_TrimmedCurve(aLine, 0.0, 10.0);
  Handle(Geom_TrimmedCurve) aTrimmed2 = Handle(Geom_TrimmedCurve)::DownCast(aTrimmed1->Copy());

  EXPECT_EQ(myHasher(aTrimmed1), myHasher(aTrimmed2));
  EXPECT_TRUE(myHasher(aTrimmed1, aTrimmed2));
}

TEST_F(GeomHash_CurveHasherTest, TrimmedCurve_DifferentBounds_DifferentHash)
{
  Handle(Geom_Line)         aLine     = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  Handle(Geom_TrimmedCurve) aTrimmed1 = new Geom_TrimmedCurve(aLine, 0.0, 10.0);
  Handle(Geom_TrimmedCurve) aTrimmed2 = new Geom_TrimmedCurve(aLine, 0.0, 20.0);

  EXPECT_NE(myHasher(aTrimmed1), myHasher(aTrimmed2));
  EXPECT_FALSE(myHasher(aTrimmed1, aTrimmed2));
}

// ============================================================================
// OffsetCurve Tests
// ============================================================================

TEST_F(GeomHash_CurveHasherTest, OffsetCurve_CopiedCurves_SameHash)
{
  Handle(Geom_Line)        aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  gp_Dir                   aRefDir(0.0, 0.0, 1.0);
  Handle(Geom_OffsetCurve) anOffset1 = new Geom_OffsetCurve(aLine, 5.0, aRefDir);
  Handle(Geom_OffsetCurve) anOffset2 = Handle(Geom_OffsetCurve)::DownCast(anOffset1->Copy());

  EXPECT_EQ(myHasher(anOffset1), myHasher(anOffset2));
  EXPECT_TRUE(myHasher(anOffset1, anOffset2));
}

TEST_F(GeomHash_CurveHasherTest, OffsetCurve_DifferentOffset_DifferentHash)
{
  Handle(Geom_Line)        aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  gp_Dir                   aRefDir(0.0, 0.0, 1.0);
  Handle(Geom_OffsetCurve) anOffset1 = new Geom_OffsetCurve(aLine, 5.0, aRefDir);
  Handle(Geom_OffsetCurve) anOffset2 = new Geom_OffsetCurve(aLine, 10.0, aRefDir);

  EXPECT_NE(myHasher(anOffset1), myHasher(anOffset2));
  EXPECT_FALSE(myHasher(anOffset1, anOffset2));
}

// ============================================================================
// Cross-type Tests
// ============================================================================

TEST_F(GeomHash_CurveHasherTest, DifferentTypes_DifferentComparison)
{
  Handle(Geom_Line)   aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  gp_Ax2              anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Circle) aCircle = new Geom_Circle(anAxis, 5.0);

  EXPECT_FALSE(myHasher(aLine, aCircle));
}

TEST_F(GeomHash_CurveHasherTest, NullCurves_HandledCorrectly)
{
  Handle(Geom_Curve) aNullCurve;
  Handle(Geom_Line)  aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));

  EXPECT_EQ(myHasher(aNullCurve), 0u);
  EXPECT_TRUE(myHasher(aNullCurve, aNullCurve));
  EXPECT_FALSE(myHasher(aLine, aNullCurve));
}

TEST_F(GeomHash_CurveHasherTest, SameObject_Equal)
{
  Handle(Geom_Line) aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  EXPECT_TRUE(myHasher(aLine, aLine));
}

// ============================================================================
// Weighted BSpline Tests
// ============================================================================

TEST_F(GeomHash_CurveHasherTest, BSplineCurve_Weighted_CopiedCurves_SameHash)
{
  TColgp_Array1OfPnt aPoles(1, 4);
  aPoles(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(2) = gp_Pnt(1.0, 1.0, 0.0);
  aPoles(3) = gp_Pnt(2.0, 1.0, 0.0);
  aPoles(4) = gp_Pnt(3.0, 0.0, 0.0);

  TColStd_Array1OfReal aWeights(1, 4);
  aWeights(1) = 1.0;
  aWeights(2) = 2.0;
  aWeights(3) = 2.0;
  aWeights(4) = 1.0;

  TColStd_Array1OfReal aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  TColStd_Array1OfInteger aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  Handle(Geom_BSplineCurve) aCurve1 = new Geom_BSplineCurve(aPoles, aWeights, aKnots, aMults, 3);
  Handle(Geom_BSplineCurve) aCurve2 = Handle(Geom_BSplineCurve)::DownCast(aCurve1->Copy());

  EXPECT_EQ(myHasher(aCurve1), myHasher(aCurve2));
  EXPECT_TRUE(myHasher(aCurve1, aCurve2));
}

TEST_F(GeomHash_CurveHasherTest, BSplineCurve_DifferentWeights_DifferentComparison)
{
  TColgp_Array1OfPnt aPoles(1, 4);
  aPoles(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(2) = gp_Pnt(1.0, 1.0, 0.0);
  aPoles(3) = gp_Pnt(2.0, 1.0, 0.0);
  aPoles(4) = gp_Pnt(3.0, 0.0, 0.0);

  TColStd_Array1OfReal aWeights1(1, 4);
  aWeights1(1) = 1.0;
  aWeights1(2) = 2.0;
  aWeights1(3) = 2.0;
  aWeights1(4) = 1.0;

  TColStd_Array1OfReal aWeights2(1, 4);
  aWeights2(1) = 1.0;
  aWeights2(2) = 3.0; // Different
  aWeights2(3) = 2.0;
  aWeights2(4) = 1.0;

  TColStd_Array1OfReal aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  TColStd_Array1OfInteger aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  Handle(Geom_BSplineCurve) aCurve1 = new Geom_BSplineCurve(aPoles, aWeights1, aKnots, aMults, 3);
  Handle(Geom_BSplineCurve) aCurve2 = new Geom_BSplineCurve(aPoles, aWeights2, aKnots, aMults, 3);

  EXPECT_FALSE(myHasher(aCurve1, aCurve2));
}

// ============================================================================
// Weighted Bezier Tests
// ============================================================================

TEST_F(GeomHash_CurveHasherTest, BezierCurve_Weighted_CopiedCurves_SameHash)
{
  TColgp_Array1OfPnt aPoles(1, 3);
  aPoles(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(2) = gp_Pnt(1.0, 2.0, 0.0);
  aPoles(3) = gp_Pnt(2.0, 0.0, 0.0);

  TColStd_Array1OfReal aWeights(1, 3);
  aWeights(1) = 1.0;
  aWeights(2) = 2.0;
  aWeights(3) = 1.0;

  Handle(Geom_BezierCurve) aCurve1 = new Geom_BezierCurve(aPoles, aWeights);
  Handle(Geom_BezierCurve) aCurve2 = Handle(Geom_BezierCurve)::DownCast(aCurve1->Copy());

  EXPECT_EQ(myHasher(aCurve1), myHasher(aCurve2));
  EXPECT_TRUE(myHasher(aCurve1, aCurve2));
}

// ============================================================================
// Axis Orientation Tests
// ============================================================================

TEST_F(GeomHash_CurveHasherTest, Circle_DifferentAxisOrientation_DifferentHash)
{
  gp_Ax2              anAxis1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Ax2              anAxis2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0));
  Handle(Geom_Circle) aCircle1 = new Geom_Circle(anAxis1, 5.0);
  Handle(Geom_Circle) aCircle2 = new Geom_Circle(anAxis2, 5.0);

  EXPECT_NE(myHasher(aCircle1), myHasher(aCircle2));
  EXPECT_FALSE(myHasher(aCircle1, aCircle2));
}

TEST_F(GeomHash_CurveHasherTest, Line_DifferentDirection_DifferentHash)
{
  Handle(Geom_Line) aLine1 = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  Handle(Geom_Line) aLine2 = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0));

  EXPECT_NE(myHasher(aLine1), myHasher(aLine2));
  EXPECT_FALSE(myHasher(aLine1, aLine2));
}

// ============================================================================
// BSpline with Different Knots Tests
// ============================================================================

TEST_F(GeomHash_CurveHasherTest, BSplineCurve_DifferentKnots_DifferentComparison)
{
  TColgp_Array1OfPnt aPoles(1, 4);
  aPoles(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(2) = gp_Pnt(1.0, 1.0, 0.0);
  aPoles(3) = gp_Pnt(2.0, 1.0, 0.0);
  aPoles(4) = gp_Pnt(3.0, 0.0, 0.0);

  TColStd_Array1OfReal aKnots1(1, 2);
  aKnots1(1) = 0.0;
  aKnots1(2) = 1.0;

  TColStd_Array1OfReal aKnots2(1, 2);
  aKnots2(1) = 0.0;
  aKnots2(2) = 2.0; // Different

  TColStd_Array1OfInteger aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  Handle(Geom_BSplineCurve) aCurve1 = new Geom_BSplineCurve(aPoles, aKnots1, aMults, 3);
  Handle(Geom_BSplineCurve) aCurve2 = new Geom_BSplineCurve(aPoles, aKnots2, aMults, 3);

  EXPECT_FALSE(myHasher(aCurve1, aCurve2));
}

// ============================================================================
// Periodic BSpline Tests
// ============================================================================

TEST_F(GeomHash_CurveHasherTest, BSplineCurve_QuadraticBezierEquivalent_CopiedCurves_SameHash)
{
  // Simple quadratic B-spline (Bezier-like, single span)
  TColgp_Array1OfPnt aPoles(1, 3);
  aPoles(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(2) = gp_Pnt(1.0, 2.0, 0.0);
  aPoles(3) = gp_Pnt(2.0, 0.0, 0.0);

  TColStd_Array1OfReal aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  TColStd_Array1OfInteger aMults(1, 2);
  aMults(1) = 3;
  aMults(2) = 3;

  Handle(Geom_BSplineCurve) aCurve1 = new Geom_BSplineCurve(aPoles, aKnots, aMults, 2);
  Handle(Geom_BSplineCurve) aCurve2 = Handle(Geom_BSplineCurve)::DownCast(aCurve1->Copy());

  EXPECT_EQ(myHasher(aCurve1), myHasher(aCurve2));
  EXPECT_TRUE(myHasher(aCurve1, aCurve2));
}

// ============================================================================
// Reversed Curve Tests
// ============================================================================

TEST_F(GeomHash_CurveHasherTest, Line_Reversed_DifferentComparison)
{
  Handle(Geom_Line) aLine1 = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  Handle(Geom_Line) aLine2 = Handle(Geom_Line)::DownCast(aLine1->Reversed());

  // Reversed line has opposite direction
  EXPECT_FALSE(myHasher(aLine1, aLine2));
}

TEST_F(GeomHash_CurveHasherTest, BezierCurve_Reversed_DifferentComparison)
{
  TColgp_Array1OfPnt aPoles(1, 3);
  aPoles(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(2) = gp_Pnt(1.0, 2.0, 0.0);
  aPoles(3) = gp_Pnt(2.0, 0.0, 0.0);

  Handle(Geom_BezierCurve) aCurve1 = new Geom_BezierCurve(aPoles);
  Handle(Geom_BezierCurve) aCurve2 = Handle(Geom_BezierCurve)::DownCast(aCurve1->Reversed());

  // Reversed curve has poles in different order
  EXPECT_FALSE(myHasher(aCurve1, aCurve2));
}

// ============================================================================
// Transformed Curve Tests
// ============================================================================

TEST_F(GeomHash_CurveHasherTest, Circle_Translated_DifferentHash)
{
  gp_Ax2              anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Circle) aCircle1 = new Geom_Circle(anAxis, 5.0);
  Handle(Geom_Circle) aCircle2 = Handle(Geom_Circle)::DownCast(aCircle1->Copy());
  aCircle2->Translate(gp_Vec(1.0, 0.0, 0.0));

  EXPECT_NE(myHasher(aCircle1), myHasher(aCircle2));
  EXPECT_FALSE(myHasher(aCircle1, aCircle2));
}

TEST_F(GeomHash_CurveHasherTest, Circle_Scaled_DifferentHash)
{
  gp_Ax2              anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Circle) aCircle1 = new Geom_Circle(anAxis, 5.0);
  Handle(Geom_Circle) aCircle2 = Handle(Geom_Circle)::DownCast(aCircle1->Copy());
  aCircle2->Scale(gp_Pnt(0.0, 0.0, 0.0), 2.0);

  EXPECT_NE(myHasher(aCircle1), myHasher(aCircle2));
  EXPECT_FALSE(myHasher(aCircle1, aCircle2));
}

// ============================================================================
// Higher Degree BSpline Tests
// ============================================================================

TEST_F(GeomHash_CurveHasherTest, BSplineCurve_HigherDegree_CopiedCurves_SameHash)
{
  TColgp_Array1OfPnt aPoles(1, 6);
  aPoles(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(2) = gp_Pnt(1.0, 1.0, 0.0);
  aPoles(3) = gp_Pnt(2.0, 0.5, 0.0);
  aPoles(4) = gp_Pnt(3.0, 1.5, 0.0);
  aPoles(5) = gp_Pnt(4.0, 0.0, 0.0);
  aPoles(6) = gp_Pnt(5.0, 1.0, 0.0);

  TColStd_Array1OfReal aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  TColStd_Array1OfInteger aMults(1, 2);
  aMults(1) = 6;
  aMults(2) = 6;

  // Degree 5 curve
  Handle(Geom_BSplineCurve) aCurve1 = new Geom_BSplineCurve(aPoles, aKnots, aMults, 5);
  Handle(Geom_BSplineCurve) aCurve2 = Handle(Geom_BSplineCurve)::DownCast(aCurve1->Copy());

  EXPECT_EQ(myHasher(aCurve1), myHasher(aCurve2));
  EXPECT_TRUE(myHasher(aCurve1, aCurve2));
}

TEST_F(GeomHash_CurveHasherTest, BSplineCurve_DifferentDegree_DifferentComparison)
{
  TColgp_Array1OfPnt aPoles(1, 4);
  aPoles(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(2) = gp_Pnt(1.0, 1.0, 0.0);
  aPoles(3) = gp_Pnt(2.0, 1.0, 0.0);
  aPoles(4) = gp_Pnt(3.0, 0.0, 0.0);

  TColStd_Array1OfReal aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  TColStd_Array1OfInteger aMults1(1, 2);
  aMults1(1) = 4;
  aMults1(2) = 4;

  TColStd_Array1OfInteger aMults2(1, 2);
  aMults2(1) = 3;
  aMults2(2) = 3;

  // Different degrees: 3 vs 2
  Handle(Geom_BSplineCurve) aCurve1 = new Geom_BSplineCurve(aPoles, aKnots, aMults1, 3);

  TColgp_Array1OfPnt aPoles2(1, 3);
  aPoles2(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles2(2) = gp_Pnt(1.5, 1.0, 0.0);
  aPoles2(3) = gp_Pnt(3.0, 0.0, 0.0);

  Handle(Geom_BSplineCurve) aCurve2 = new Geom_BSplineCurve(aPoles2, aKnots, aMults2, 2);

  EXPECT_FALSE(myHasher(aCurve1, aCurve2));
}

// ============================================================================
// Multiple Knot Spans Tests
// ============================================================================

TEST_F(GeomHash_CurveHasherTest, BSplineCurve_LinearMultipleSpans_CopiedCurves_SameHash)
{
  // Linear B-spline with multiple knots (piecewise linear)
  TColgp_Array1OfPnt aPoles(1, 4);
  aPoles(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(2) = gp_Pnt(1.0, 1.0, 0.0);
  aPoles(3) = gp_Pnt(2.0, 0.0, 0.0);
  aPoles(4) = gp_Pnt(3.0, 1.0, 0.0);

  TColStd_Array1OfReal aKnots(1, 4);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;
  aKnots(3) = 2.0;
  aKnots(4) = 3.0;

  TColStd_Array1OfInteger aMults(1, 4);
  aMults(1) = 2;
  aMults(2) = 1;
  aMults(3) = 1;
  aMults(4) = 2;

  Handle(Geom_BSplineCurve) aCurve1 = new Geom_BSplineCurve(aPoles, aKnots, aMults, 1);
  Handle(Geom_BSplineCurve) aCurve2 = Handle(Geom_BSplineCurve)::DownCast(aCurve1->Copy());

  EXPECT_EQ(myHasher(aCurve1), myHasher(aCurve2));
  EXPECT_TRUE(myHasher(aCurve1, aCurve2));
}

// ============================================================================
// Cross-type Conic Tests
// ============================================================================

TEST_F(GeomHash_CurveHasherTest, Ellipse_vs_Hyperbola_DifferentComparison)
{
  gp_Ax2                 anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Ellipse)   anEllipse  = new Geom_Ellipse(anAxis, 5.0, 3.0);
  Handle(Geom_Hyperbola) aHyperbola = new Geom_Hyperbola(anAxis, 5.0, 3.0);

  EXPECT_FALSE(myHasher(anEllipse, aHyperbola));
}

TEST_F(GeomHash_CurveHasherTest, Circle_vs_Ellipse_DifferentComparison)
{
  gp_Ax2               anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Circle)  aCircle   = new Geom_Circle(anAxis, 5.0);
  Handle(Geom_Ellipse) anEllipse = new Geom_Ellipse(anAxis, 5.0, 5.0);

  EXPECT_FALSE(myHasher(aCircle, anEllipse));
}

// ============================================================================
// Trimmed vs Base Curve Tests
// ============================================================================

TEST_F(GeomHash_CurveHasherTest, TrimmedCurve_vs_BaseCurve_DifferentComparison)
{
  Handle(Geom_Line)         aLine    = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  Handle(Geom_TrimmedCurve) aTrimmed = new Geom_TrimmedCurve(aLine, 0.0, 10.0);

  // Trimmed curve is a different type from base curve
  EXPECT_FALSE(myHasher(aLine, aTrimmed));
}

// ============================================================================
// Edge Cases - Degenerate/Special Values
// ============================================================================

TEST_F(GeomHash_CurveHasherTest, Circle_VerySmallRadius_CopiedCircles_SameHash)
{
  gp_Ax2              anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Circle) aCircle1 = new Geom_Circle(anAxis, 1e-10);
  Handle(Geom_Circle) aCircle2 = Handle(Geom_Circle)::DownCast(aCircle1->Copy());

  EXPECT_EQ(myHasher(aCircle1), myHasher(aCircle2));
  EXPECT_TRUE(myHasher(aCircle1, aCircle2));
}

TEST_F(GeomHash_CurveHasherTest, Circle_VeryLargeRadius_CopiedCircles_SameHash)
{
  gp_Ax2              anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Circle) aCircle1 = new Geom_Circle(anAxis, 1e10);
  Handle(Geom_Circle) aCircle2 = Handle(Geom_Circle)::DownCast(aCircle1->Copy());

  EXPECT_EQ(myHasher(aCircle1), myHasher(aCircle2));
  EXPECT_TRUE(myHasher(aCircle1, aCircle2));
}

TEST_F(GeomHash_CurveHasherTest, Line_AtOrigin_vs_FarFromOrigin_DifferentHash)
{
  Handle(Geom_Line) aLine1 = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  Handle(Geom_Line) aLine2 = new Geom_Line(gp_Pnt(1e10, 1e10, 1e10), gp_Dir(1.0, 0.0, 0.0));

  EXPECT_NE(myHasher(aLine1), myHasher(aLine2));
  EXPECT_FALSE(myHasher(aLine1, aLine2));
}
