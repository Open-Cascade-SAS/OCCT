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

#include <GeomFill_NSections.hxx>
#include <Geom_BSplineCurve.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_Sequence.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

namespace
{

//! Builds a simple degree-1 BSpline line segment from (0,0,0) to (1,0,0).
static Handle(Geom_BSplineCurve) makeSimpleLinearCurve()
{
  NCollection_Array1<gp_Pnt> aPoles(1, 2);
  aPoles(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(2) = gp_Pnt(1.0, 0.0, 0.0);

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 2;
  aMults(2) = 2;

  return new Geom_BSplineCurve(aPoles, aKnots, aMults, 1);
}

} // namespace

// Tests that GeomFill_NSections with a single-curve sequence does not throw.
// The resulting BSplineSurface may be null for a degenerate single-section case,
// but no exception should be raised (OCC27875).
TEST(GeomFill_NSectionsTest, OCC27875_SingleCurveDoesNotThrow)
{
  Handle(Geom_BSplineCurve) aCurve = makeSimpleLinearCurve();
  ASSERT_FALSE(aCurve.IsNull());

  NCollection_Sequence<Handle(Geom_Curve)> aNC(new NCollection_IncAllocator());
  aNC.Append(Handle(Geom_Curve)(aCurve));

  // Must not throw even though result may be degenerate
  EXPECT_NO_THROW({ GeomFill_NSections aNS(aNC); });
}
