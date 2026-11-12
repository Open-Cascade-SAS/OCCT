// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <GeomAdaptor_Curve.hxx>
#include <GeomFill_Boundary.hxx>
#include <GeomFill_CoonsAlgPatch.hxx>
#include <GeomFill_SimpleBound.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GC_MakeSegment.hxx>
#include <Precision.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>

#include <gtest/gtest.h>

#include <initializer_list>

namespace
{
//==================================================================================================

// Create a segment boundary with its native arc-length parameter range.
static occ::handle<GeomFill_SimpleBound> Bound(const gp_Pnt& theP1, const gp_Pnt& theP2)
{
  const occ::handle<Geom_TrimmedCurve> aSegment = GC_MakeSegment(theP1, theP2).Value();
  const occ::handle<GeomAdaptor_Curve> aCurve   = new GeomAdaptor_Curve(aSegment);
  return new GeomFill_SimpleBound(aCurve, Precision::Confusion(), Precision::Angular());
}
} // namespace

//==================================================================================================

// The four boundaries of a planar square, in the order the constructor expects: B1 and B3 run
// along U, B2 and B4 along V. Value(U, V) must sample B1 and B3 at U, as D1U and D1V do.
TEST(GeomFill_CoonsAlgPatchTest, ValueUsesUForTheUDirectionBoundaries)
{
  const gp_Pnt aP00(0., 0., 0.);
  const gp_Pnt aP10(10., 0., 0.);
  const gp_Pnt aP11(10., 10., 0.);
  const gp_Pnt aP01(0., 10., 0.);

  const occ::handle<GeomFill_Boundary> aB1 = Bound(aP00, aP10); // v = vMin, along U
  const occ::handle<GeomFill_Boundary> aB2 = Bound(aP10, aP11); // u = uMax, along V
  const occ::handle<GeomFill_Boundary> aB3 = Bound(aP01, aP11); // v = vMax, along U
  const occ::handle<GeomFill_Boundary> aB4 = Bound(aP00, aP01); // u = uMin, along V
  GeomFill_CoonsAlgPatch               aPatch(aB1, aB2, aB3, aB4);

  // The patch is parametrized on the boundaries' own ranges, U on B1's and V on B2's, which for
  // a segment is its arc length rather than [0, 1].
  double aUMin, aUMax, aVMin, aVMax;
  aB1->Bounds(aUMin, aUMax);
  aB2->Bounds(aVMin, aVMax);

  // On a planar square the patch is the plane itself, so the answer is known in closed form.
  const double aTol = Precision::Confusion();
  EXPECT_NEAR(aPatch.Value(aUMin, aVMin).Distance(aP00), 0., aTol);
  EXPECT_NEAR(aPatch.Value(aUMax, aVMin).Distance(aP10), 0., aTol);
  EXPECT_NEAR(aPatch.Value(aUMax, aVMax).Distance(aP11), 0., aTol);
  EXPECT_NEAR(aPatch.Value(aUMin, aVMax).Distance(aP01), 0., aTol);

  // Check distinct U and V values on the boundaries and inside the patch.
  const double aUMid = 0.5 * (aUMin + aUMax);
  const double aVMid = 0.5 * (aVMin + aVMax);
  EXPECT_NEAR(aPatch.Value(aUMax, aVMid).Distance(gp_Pnt(10., 5., 0.)), 0., aTol);
  EXPECT_NEAR(aPatch.Value(aUMin, aVMid).Distance(gp_Pnt(0., 5., 0.)), 0., aTol);
  EXPECT_NEAR(aPatch.Value(aUMid, aVMin).Distance(gp_Pnt(5., 0., 0.)), 0., aTol);
  EXPECT_NEAR(aPatch.Value(aUMid, aVMax).Distance(gp_Pnt(5., 10., 0.)), 0., aTol);
  EXPECT_NEAR(aPatch.Value(3., 7.).Distance(gp_Pnt(3., 7., 0.)), 0., aTol);
}

//==================================================================================================

TEST(GeomFill_CoonsAlgPatchTest, ValueUsesIndependentUAndVRanges)
{
  const occ::handle<GeomFill_Boundary> aB1 = Bound(gp_Pnt(-3., 2., 5.), gp_Pnt(7., 2., 5.));
  const occ::handle<GeomFill_Boundary> aB2 = Bound(gp_Pnt(7., 2., 5.), gp_Pnt(7., 6., 5.));
  const occ::handle<GeomFill_Boundary> aB3 = Bound(gp_Pnt(-3., 6., 5.), gp_Pnt(7., 6., 5.));
  const occ::handle<GeomFill_Boundary> aB4 = Bound(gp_Pnt(-3., 2., 5.), gp_Pnt(-3., 6., 5.));

  // Opposite boundaries share a range, while U and V have different, nonzero lower bounds.
  aB1->Reparametrize(2., 5., false, false, 1., 1., false);
  aB3->Reparametrize(2., 5., false, false, 1., 1., false);
  aB2->Reparametrize(-3., -1., false, false, 1., 1., false);
  aB4->Reparametrize(-3., -1., false, false, 1., 1., false);
  for (const occ::handle<GeomFill_Boundary>& aBound : {aB1, aB2, aB3, aB4})
  {
    double aFirst, aLast;
    aBound->Bounds(aFirst, aLast);
    const bool isU = aBound == aB1 || aBound == aB3;
    ASSERT_DOUBLE_EQ(aFirst, isU ? 2. : -3.);
    ASSERT_DOUBLE_EQ(aLast, isU ? 5. : -1.);
  }

  const GeomFill_CoonsAlgPatch aPatch(aB1, aB2, aB3, aB4);
  for (const double aU : {2., 2.9, 5.})
  {
    for (const double aV : {-3., -1.6, -1.})
    {
      const gp_Pnt anExpected(-3. + 10. * (aU - 2.) / 3., 2. + 4. * (aV + 3.) / 2., 5.);
      EXPECT_NEAR(aPatch.Value(aU, aV).Distance(anExpected), 0., Precision::Confusion());
    }
  }
}

//==================================================================================================

TEST(GeomFill_CoonsAlgPatchTest, ValueAgreesWithPartialDerivatives)
{
  const occ::handle<GeomFill_Boundary> aB1 = Bound(gp_Pnt(0., 0., 0.), gp_Pnt(10., 0., 2.));
  const occ::handle<GeomFill_Boundary> aB2 = Bound(gp_Pnt(10., 0., 2.), gp_Pnt(10., 10., 0.));
  const occ::handle<GeomFill_Boundary> aB3 = Bound(gp_Pnt(0., 10., 1.), gp_Pnt(10., 10., 0.));
  const occ::handle<GeomFill_Boundary> aB4 = Bound(gp_Pnt(0., 0., 0.), gp_Pnt(0., 10., 1.));
  for (const occ::handle<GeomFill_Boundary>& aBound : {aB1, aB2, aB3, aB4})
  {
    aBound->Reparametrize(0., 1., false, false, 1., 1., false);
    double aFirst, aLast;
    aBound->Bounds(aFirst, aLast);
    ASSERT_DOUBLE_EQ(aFirst, 0.);
    ASSERT_DOUBLE_EQ(aLast, 1.);
  }
  const GeomFill_CoonsAlgPatch aPatch(aB1, aB2, aB3, aB4);

  // For straight boundaries the patch is bilinear: (10 U, 10 V, 2 U + V - 3 U V).
  // The step and derivative tolerance allow subtraction roundoff for coordinates near 10.
  constexpr double aStep          = 1.e-5;
  constexpr double aDerivativeTol = 1.e-7;
  for (const double aU : {0.2, 0.3, 0.8})
  {
    for (const double aV : {0.1, 0.7, 0.9})
    {
      const gp_Pnt anExpected(10. * aU, 10. * aV, 2. * aU + aV - 3. * aU * aV);
      EXPECT_NEAR(aPatch.Value(aU, aV).Distance(anExpected), 0., Precision::Confusion());

      const gp_XYZ aNumU =
        (aPatch.Value(aU + aStep, aV).XYZ() - aPatch.Value(aU - aStep, aV).XYZ()) / (2. * aStep);
      const gp_XYZ aNumV =
        (aPatch.Value(aU, aV + aStep).XYZ() - aPatch.Value(aU, aV - aStep).XYZ()) / (2. * aStep);
      const gp_Vec aD1U = aPatch.D1U(aU, aV);
      const gp_Vec aD1V = aPatch.D1V(aU, aV);
      EXPECT_NEAR((aNumU - aD1U.XYZ()).Modulus(), 0., aDerivativeTol);
      EXPECT_NEAR((aNumV - aD1V.XYZ()).Modulus(), 0., aDerivativeTol);
      EXPECT_NEAR((aD1U - gp_Vec(10., 0., 2. - 3. * aV)).Magnitude(), 0., aDerivativeTol);
      EXPECT_NEAR((aD1V - gp_Vec(0., 10., 1. - 3. * aU)).Magnitude(), 0., aDerivativeTol);
    }
  }
}
