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

#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomConvert.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array1.hxx>

//==================================================================================================
// Basic concatenation tests
//==================================================================================================

TEST(GeomConvert_CompCurveToBSplineCurveTest, ConcatenateClampedBSplines)
{
  // Create two simple clamped B-spline curves that share an endpoint.
  // For clamped B-splines, the first/last poles coincide with endpoints.
  NCollection_Array1<gp_Pnt> aPoles1(1, 4);
  aPoles1(1) = gp_Pnt(0., 0., 0.);
  aPoles1(2) = gp_Pnt(1., 1., 0.);
  aPoles1(3) = gp_Pnt(2., 1., 0.);
  aPoles1(4) = gp_Pnt(3., 0., 0.);

  NCollection_Array1<double> aKnots1(1, 2);
  aKnots1(1) = 0.;
  aKnots1(2) = 1.;

  NCollection_Array1<int> aMults1(1, 2);
  aMults1(1) = 4;
  aMults1(2) = 4;

  occ::handle<Geom_BSplineCurve> aCurve1 = new Geom_BSplineCurve(aPoles1, aKnots1, aMults1, 3);

  NCollection_Array1<gp_Pnt> aPoles2(1, 4);
  aPoles2(1) = gp_Pnt(3., 0., 0.); // Starts at aCurve1 endpoint
  aPoles2(2) = gp_Pnt(4., -1., 0.);
  aPoles2(3) = gp_Pnt(5., -1., 0.);
  aPoles2(4) = gp_Pnt(6., 0., 0.);

  occ::handle<Geom_BSplineCurve> aCurve2 = new Geom_BSplineCurve(aPoles2, aKnots1, aMults1, 3);

  GeomConvert_CompCurveToBSplineCurve aConcat(aCurve1);
  const bool                          isAdded = aConcat.Add(aCurve2, Precision::Confusion());

  EXPECT_TRUE(isAdded) << "Should successfully concatenate clamped B-splines";

  occ::handle<Geom_BSplineCurve> aResult = aConcat.BSplineCurve();
  ASSERT_FALSE(aResult.IsNull()) << "Result curve should not be null";

  // Verify endpoints
  const gp_Pnt aStart = aResult->StartPoint();
  const gp_Pnt aEnd   = aResult->EndPoint();

  EXPECT_NEAR(aStart.Distance(gp_Pnt(0., 0., 0.)), 0., Precision::Confusion());
  EXPECT_NEAR(aEnd.Distance(gp_Pnt(6., 0., 0.)), 0., Precision::Confusion());
}

//==================================================================================================

TEST(GeomConvert_CompCurveToBSplineCurveTest, ConcatenateTrimmedCircleArcs)
{
  // Create two trimmed circle arcs. When converted to B-spline, these create
  // non-clamped B-splines where poles don't coincide with endpoints.
  // This tests the fix for bug 0030007.
  gp_Circ aCirc(gp_Ax2(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.)), 5.);

  occ::handle<Geom_Circle> aGeomCircle = new Geom_Circle(aCirc);

  // First arc: 0 to PI/2 (first quadrant)
  occ::handle<Geom_TrimmedCurve> aArc1 = new Geom_TrimmedCurve(aGeomCircle, 0., M_PI / 2.);
  // Second arc: PI/2 to PI (second quadrant)
  occ::handle<Geom_TrimmedCurve> aArc2 = new Geom_TrimmedCurve(aGeomCircle, M_PI / 2., M_PI);

  // Verify arcs are continuous
  const gp_Pnt aArc1End   = aArc1->EndPoint();
  const gp_Pnt aArc2Start = aArc2->StartPoint();
  EXPECT_NEAR(aArc1End.Distance(aArc2Start), 0., Precision::Confusion())
    << "Arcs should share an endpoint";

  GeomConvert_CompCurveToBSplineCurve aConcat(aArc1);
  const bool                          isAdded = aConcat.Add(aArc2, Precision::Confusion());

  EXPECT_TRUE(isAdded) << "Should successfully concatenate trimmed circle arcs";

  occ::handle<Geom_BSplineCurve> aResult = aConcat.BSplineCurve();
  ASSERT_FALSE(aResult.IsNull()) << "Result curve should not be null";

  // Verify endpoints match the original arc endpoints
  const gp_Pnt aStart = aResult->StartPoint();
  const gp_Pnt aEnd   = aResult->EndPoint();

  // Arc1 starts at (5, 0, 0), Arc2 ends at (-5, 0, 0)
  EXPECT_NEAR(aStart.Distance(gp_Pnt(5., 0., 0.)), 0., Precision::Confusion());
  EXPECT_NEAR(aEnd.Distance(gp_Pnt(-5., 0., 0.)), 0., Precision::Confusion());
}

//==================================================================================================

TEST(GeomConvert_CompCurveToBSplineCurveTest, ConcatenateWithReversal)
{
  // Test concatenation where the second curve needs to be reversed.
  // The algorithm should detect this and reverse automatically.
  NCollection_Array1<gp_Pnt> aPoles1(1, 4);
  aPoles1(1) = gp_Pnt(0., 0., 0.);
  aPoles1(2) = gp_Pnt(1., 1., 0.);
  aPoles1(3) = gp_Pnt(2., 1., 0.);
  aPoles1(4) = gp_Pnt(3., 0., 0.);

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.;
  aKnots(2) = 1.;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  occ::handle<Geom_BSplineCurve> aCurve1 = new Geom_BSplineCurve(aPoles1, aKnots, aMults, 3);

  // Second curve ends at aCurve1's endpoint (needs reversal)
  NCollection_Array1<gp_Pnt> aPoles2(1, 4);
  aPoles2(1) = gp_Pnt(6., 0., 0.);
  aPoles2(2) = gp_Pnt(5., -1., 0.);
  aPoles2(3) = gp_Pnt(4., -1., 0.);
  aPoles2(4) = gp_Pnt(3., 0., 0.); // End matches aCurve1 end

  occ::handle<Geom_BSplineCurve> aCurve2 = new Geom_BSplineCurve(aPoles2, aKnots, aMults, 3);

  GeomConvert_CompCurveToBSplineCurve aConcat(aCurve1);
  const bool                          isAdded = aConcat.Add(aCurve2, Precision::Confusion());

  EXPECT_TRUE(isAdded) << "Should successfully concatenate curves with reversal";

  occ::handle<Geom_BSplineCurve> aResult = aConcat.BSplineCurve();
  ASSERT_FALSE(aResult.IsNull()) << "Result curve should not be null";

  // Verify endpoints
  const gp_Pnt aStart = aResult->StartPoint();
  const gp_Pnt aEnd   = aResult->EndPoint();

  EXPECT_NEAR(aStart.Distance(gp_Pnt(0., 0., 0.)), 0., Precision::Confusion());
  EXPECT_NEAR(aEnd.Distance(gp_Pnt(6., 0., 0.)), 0., Precision::Confusion());
}

//==================================================================================================

TEST(GeomConvert_CompCurveToBSplineCurveTest, FailsForDisjointCurves)
{
  // Test that concatenation fails for curves that don't share an endpoint.
  NCollection_Array1<gp_Pnt> aPoles1(1, 4);
  aPoles1(1) = gp_Pnt(0., 0., 0.);
  aPoles1(2) = gp_Pnt(1., 1., 0.);
  aPoles1(3) = gp_Pnt(2., 1., 0.);
  aPoles1(4) = gp_Pnt(3., 0., 0.);

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.;
  aKnots(2) = 1.;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  occ::handle<Geom_BSplineCurve> aCurve1 = new Geom_BSplineCurve(aPoles1, aKnots, aMults, 3);

  // Second curve is far away
  NCollection_Array1<gp_Pnt> aPoles2(1, 4);
  aPoles2(1) = gp_Pnt(10., 0., 0.);
  aPoles2(2) = gp_Pnt(11., 1., 0.);
  aPoles2(3) = gp_Pnt(12., 1., 0.);
  aPoles2(4) = gp_Pnt(13., 0., 0.);

  occ::handle<Geom_BSplineCurve> aCurve2 = new Geom_BSplineCurve(aPoles2, aKnots, aMults, 3);

  GeomConvert_CompCurveToBSplineCurve aConcat(aCurve1);
  const bool                          isAdded = aConcat.Add(aCurve2, Precision::Confusion());

  EXPECT_FALSE(isAdded) << "Should fail to concatenate disjoint curves";
}

//==================================================================================================

TEST(GeomConvert_CompCurveToBSplineCurveTest, ConcatenateNonClampedBSpline_Bug30007)
{
  // This test specifically addresses bug 0030007.
  // Create a non-clamped B-spline where first pole != start point.
  // Prior to the fix, the algorithm would incorrectly use poles for distance checks.

  // Create a cubic B-spline with internal knots (non-clamped at ends)
  // such that the first pole does not coincide with the start point.
  NCollection_Array1<gp_Pnt> aPoles(1, 6);
  aPoles(1) = gp_Pnt(0., 0., 0.);
  aPoles(2) = gp_Pnt(1., 2., 0.);
  aPoles(3) = gp_Pnt(2., 2., 0.);
  aPoles(4) = gp_Pnt(3., 2., 0.);
  aPoles(5) = gp_Pnt(4., 2., 0.);
  aPoles(6) = gp_Pnt(5., 0., 0.);

  NCollection_Array1<double> aKnots(1, 4);
  aKnots(1) = 0.;
  aKnots(2) = 0.33;
  aKnots(3) = 0.67;
  aKnots(4) = 1.;

  NCollection_Array1<int> aMults(1, 4);
  aMults(1) = 4; // Clamped at start (multiplicity = degree + 1)
  aMults(2) = 1;
  aMults(3) = 1;
  aMults(4) = 4; // Clamped at end

  occ::handle<Geom_BSplineCurve> aCurve1 = new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);

  // Second curve starts at aCurve1's actual endpoint
  const gp_Pnt aCurve1End = aCurve1->EndPoint();

  NCollection_Array1<gp_Pnt> aPoles2(1, 4);
  aPoles2(1) = aCurve1End; // Start at actual endpoint
  aPoles2(2) = gp_Pnt(6., -1., 0.);
  aPoles2(3) = gp_Pnt(7., -1., 0.);
  aPoles2(4) = gp_Pnt(8., 0., 0.);

  NCollection_Array1<double> aKnots2(1, 2);
  aKnots2(1) = 0.;
  aKnots2(2) = 1.;

  NCollection_Array1<int> aMults2(1, 2);
  aMults2(1) = 4;
  aMults2(2) = 4;

  occ::handle<Geom_BSplineCurve> aCurve2 = new Geom_BSplineCurve(aPoles2, aKnots2, aMults2, 3);

  GeomConvert_CompCurveToBSplineCurve aConcat(aCurve1);
  const bool                          isAdded = aConcat.Add(aCurve2, Precision::Confusion());

  EXPECT_TRUE(isAdded) << "Should concatenate using actual endpoints, not poles";

  occ::handle<Geom_BSplineCurve> aResult = aConcat.BSplineCurve();
  ASSERT_FALSE(aResult.IsNull()) << "Result curve should not be null";

  // Verify the result curve endpoints
  const gp_Pnt aStart = aResult->StartPoint();
  const gp_Pnt aEnd   = aResult->EndPoint();

  EXPECT_NEAR(aStart.Distance(aCurve1->StartPoint()), 0., Precision::Confusion());
  EXPECT_NEAR(aEnd.Distance(gp_Pnt(8., 0., 0.)), 0., Precision::Confusion());
}

//==================================================================================================

TEST(GeomConvert_CompCurveToBSplineCurveTest, PrependCurve)
{
  // Test concatenation before the existing curve.
  NCollection_Array1<gp_Pnt> aPoles1(1, 4);
  aPoles1(1) = gp_Pnt(3., 0., 0.);
  aPoles1(2) = gp_Pnt(4., 1., 0.);
  aPoles1(3) = gp_Pnt(5., 1., 0.);
  aPoles1(4) = gp_Pnt(6., 0., 0.);

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.;
  aKnots(2) = 1.;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  occ::handle<Geom_BSplineCurve> aCurve1 = new Geom_BSplineCurve(aPoles1, aKnots, aMults, 3);

  // Curve that ends at aCurve1's start (should be prepended)
  NCollection_Array1<gp_Pnt> aPoles2(1, 4);
  aPoles2(1) = gp_Pnt(0., 0., 0.);
  aPoles2(2) = gp_Pnt(1., -1., 0.);
  aPoles2(3) = gp_Pnt(2., -1., 0.);
  aPoles2(4) = gp_Pnt(3., 0., 0.); // End matches aCurve1 start

  occ::handle<Geom_BSplineCurve> aCurve2 = new Geom_BSplineCurve(aPoles2, aKnots, aMults, 3);

  GeomConvert_CompCurveToBSplineCurve aConcat(aCurve1);
  const bool                          isAdded = aConcat.Add(aCurve2, Precision::Confusion());

  EXPECT_TRUE(isAdded) << "Should successfully prepend curve";

  occ::handle<Geom_BSplineCurve> aResult = aConcat.BSplineCurve();
  ASSERT_FALSE(aResult.IsNull()) << "Result curve should not be null";

  // Verify endpoints
  const gp_Pnt aStart = aResult->StartPoint();
  const gp_Pnt aEnd   = aResult->EndPoint();

  EXPECT_NEAR(aStart.Distance(gp_Pnt(0., 0., 0.)), 0., Precision::Confusion());
  EXPECT_NEAR(aEnd.Distance(gp_Pnt(6., 0., 0.)), 0., Precision::Confusion());
}

//==================================================================================================

TEST(GeomConvert_CompCurveToBSplineCurveTest, EmptyInitialCurve)
{
  // Test adding to an empty converter.
  GeomConvert_CompCurveToBSplineCurve aConcat;

  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles(1) = gp_Pnt(0., 0., 0.);
  aPoles(2) = gp_Pnt(1., 1., 0.);
  aPoles(3) = gp_Pnt(2., 1., 0.);
  aPoles(4) = gp_Pnt(3., 0., 0.);

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.;
  aKnots(2) = 1.;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  occ::handle<Geom_BSplineCurve> aCurve = new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);

  const bool isAdded = aConcat.Add(aCurve, Precision::Confusion());

  EXPECT_TRUE(isAdded) << "Should successfully add to empty converter";

  occ::handle<Geom_BSplineCurve> aResult = aConcat.BSplineCurve();
  ASSERT_FALSE(aResult.IsNull()) << "Result curve should not be null";
}

//==================================================================================================

TEST(GeomConvert_CompCurveToBSplineCurveTest, ClearAndReuse)
{
  // Test Clear() method.
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles(1) = gp_Pnt(0., 0., 0.);
  aPoles(2) = gp_Pnt(1., 1., 0.);
  aPoles(3) = gp_Pnt(2., 1., 0.);
  aPoles(4) = gp_Pnt(3., 0., 0.);

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.;
  aKnots(2) = 1.;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  occ::handle<Geom_BSplineCurve> aCurve = new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);

  GeomConvert_CompCurveToBSplineCurve aConcat(aCurve);

  // Verify curve exists
  ASSERT_FALSE(aConcat.BSplineCurve().IsNull());

  // Clear
  aConcat.Clear();
  EXPECT_TRUE(aConcat.BSplineCurve().IsNull()) << "Curve should be null after Clear()";

  // Re-add
  const bool isAdded = aConcat.Add(aCurve, Precision::Confusion());
  EXPECT_TRUE(isAdded) << "Should successfully add after Clear()";
  EXPECT_FALSE(aConcat.BSplineCurve().IsNull()) << "Curve should exist after re-adding";
}
