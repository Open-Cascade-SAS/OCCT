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

#include <gtest/gtest.h>

namespace
{
//! Boundary from a straight segment, parametrized on [0, 1].
static occ::handle<GeomFill_SimpleBound> Bound(const gp_Pnt& theP1, const gp_Pnt& theP2)
{
  occ::handle<Geom_TrimmedCurve> aSeg = GC_MakeSegment(theP1, theP2).Value();
  occ::handle<Geom_Curve>        aCrv = aSeg;
  occ::handle<GeomAdaptor_Curve> anAd =
    new GeomAdaptor_Curve(aCrv, aCrv->FirstParameter(), aCrv->LastParameter());
  return new GeomFill_SimpleBound(anAd, Precision::Confusion(), Precision::Angular());
}
} // namespace

// The four boundaries of a planar square, in the order the constructor expects: B1 and B3 run
// along U, B2 and B4 along V. Value(U, V) must sample B1 and B3 at U, as D1U and D1V do.
TEST(GeomFill_CoonsAlgPatchTest, ValueUsesUForTheUDirectionBoundaries)
{
  const gp_Pnt p00(0., 0., 0.);
  const gp_Pnt p10(10., 0., 0.);
  const gp_Pnt p11(10., 10., 0.);
  const gp_Pnt p01(0., 10., 0.);

  occ::handle<GeomFill_Boundary> aB1 = Bound(p00, p10); // v = vMin, along U
  occ::handle<GeomFill_Boundary> aB2 = Bound(p10, p11); // u = uMax, along V
  occ::handle<GeomFill_Boundary> aB3 = Bound(p01, p11); // v = vMax, along U
  occ::handle<GeomFill_Boundary> aB4 = Bound(p00, p01); // u = uMin, along V
  GeomFill_CoonsAlgPatch         aPatch(aB1, aB2, aB3, aB4);

  // The patch is parametrized on the boundaries' own ranges, U on B1's and V on B2's, which for
  // a segment is its arc length rather than [0, 1].
  double aUMin, aUMax, aVMin, aVMax;
  aB1->Bounds(aUMin, aUMax);
  aB2->Bounds(aVMin, aVMax);

  // On a planar square the patch is the plane itself, so the answer is known in closed form.
  const double aTol = 1.e-7;
  EXPECT_NEAR(aPatch.Value(aUMin, aVMin).Distance(p00), 0., aTol);
  EXPECT_NEAR(aPatch.Value(aUMax, aVMin).Distance(p10), 0., aTol);
  EXPECT_NEAR(aPatch.Value(aUMax, aVMax).Distance(p11), 0., aTol);
  EXPECT_NEAR(aPatch.Value(aUMin, aVMax).Distance(p01), 0., aTol);

  // Off the diagonal is where sampling B1 and B3 at V instead of U shows: every such point
  // collapsed onto x == y, so the whole patch degenerated to a line.
  const double aUMid = 0.5 * (aUMin + aUMax);
  const double aVMid = 0.5 * (aVMin + aVMax);
  EXPECT_NEAR(aPatch.Value(aUMax, aVMid).Distance(gp_Pnt(10., 5., 0.)), 0., aTol);
  EXPECT_NEAR(aPatch.Value(aUMin, aVMid).Distance(gp_Pnt(0., 5., 0.)), 0., aTol);
  EXPECT_NEAR(aPatch.Value(aUMid, aVMin).Distance(gp_Pnt(5., 0., 0.)), 0., aTol);
  EXPECT_NEAR(aPatch.Value(aUMid, aVMax).Distance(gp_Pnt(5., 10., 0.)), 0., aTol);
}

// Value must agree with the derivatives it is differentiated from: a central difference of
// Value along U reproduces D1U. This fails for any parameter mix-up in Value alone.
TEST(GeomFill_CoonsAlgPatchTest, ValueAgreesWithD1U)
{
  GeomFill_CoonsAlgPatch aPatch(Bound(gp_Pnt(0., 0., 0.), gp_Pnt(10., 0., 2.)),
                                Bound(gp_Pnt(10., 0., 2.), gp_Pnt(10., 10., 0.)),
                                Bound(gp_Pnt(0., 10., 1.), gp_Pnt(10., 10., 0.)),
                                Bound(gp_Pnt(0., 0., 0.), gp_Pnt(0., 10., 1.)));

  const double aStep = 1.e-6;
  const double aU = 3.0, aV = 7.0;
  const gp_XYZ aFwd = aPatch.Value(aU + aStep, aV).XYZ();
  const gp_XYZ aBwd = aPatch.Value(aU - aStep, aV).XYZ();
  const gp_XYZ aNum = (aFwd - aBwd) / (2. * aStep);

  const gp_Vec anAna = aPatch.D1U(aU, aV);
  EXPECT_NEAR(aNum.X(), anAna.X(), 1.e-4);
  EXPECT_NEAR(aNum.Y(), anAna.Y(), 1.e-4);
  EXPECT_NEAR(aNum.Z(), anAna.Z(), 1.e-4);
}
