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

#include <Geom2dAPI_Interpolate.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <NCollection_HArray1.hxx>
#include <Precision.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>

#include <gtest/gtest.h>

// OCC28594: Geom2dAPI_Interpolate with and without tangent scale produces different curves.
// Tests that interpolation with tangent vectors (with scale and without scale) both produce
// valid B-Spline curves passing through the given points.

TEST(Geom2dAPI_InterpolateTest, OCC28594_InterpolateWithAndWithoutTangentScale)
{
  occ::handle<NCollection_HArray1<gp_Pnt2d>> aPoints      = new NCollection_HArray1<gp_Pnt2d>(1, 6);
  NCollection_Array1<gp_Pnt2d>&              aPointsArray = aPoints->ChangeArray1();
  aPointsArray(1)                                         = gp_Pnt2d(-30.4, 8);
  aPointsArray(2)                                         = gp_Pnt2d(-16.689912, 17.498217);
  aPointsArray(3)                                         = gp_Pnt2d(-23.803064, 24.748543);
  aPointsArray(4)                                         = gp_Pnt2d(-16.907466, 32.919615);
  aPointsArray(5)                                         = gp_Pnt2d(-8.543829, 26.549421);
  aPointsArray(6)                                         = gp_Pnt2d(0, 39.200000);

  NCollection_Array1<gp_Vec2d> aTangents(1, 6);
  aTangents(1) = gp_Vec2d(0.3, 0.4);
  aTangents(2) = gp_Vec2d(0, 0);
  aTangents(3) = gp_Vec2d(0, 0);
  aTangents(4) = gp_Vec2d(0, 0);
  aTangents(5) = gp_Vec2d(0, 0);
  aTangents(6) = gp_Vec2d(1, 0);

  occ::handle<NCollection_HArray1<bool>> aTangentFlags      = new NCollection_HArray1<bool>(1, 6);
  NCollection_Array1<bool>&              aTangentFlagsArray = aTangentFlags->ChangeArray1();
  aTangentFlagsArray(1)                                     = true;
  aTangentFlagsArray(2)                                     = false;
  aTangentFlagsArray(3)                                     = false;
  aTangentFlagsArray(4)                                     = false;
  aTangentFlagsArray(5)                                     = false;
  aTangentFlagsArray(6)                                     = true;

  // Interpolation with tangent scale
  Geom2dAPI_Interpolate anInterpWithScale(aPoints, false, Precision::Confusion());
  anInterpWithScale.Load(aTangents, aTangentFlags);
  anInterpWithScale.Perform();
  EXPECT_TRUE(anInterpWithScale.IsDone());
  const occ::handle<Geom2d_BSplineCurve> aCurveWithScale = anInterpWithScale.Curve();
  EXPECT_FALSE(aCurveWithScale.IsNull());

  // Interpolation without tangent scale
  Geom2dAPI_Interpolate anInterpWithoutScale(aPoints, false, Precision::Confusion());
  anInterpWithoutScale.Load(aTangents, aTangentFlags, false);
  anInterpWithoutScale.Perform();
  EXPECT_TRUE(anInterpWithoutScale.IsDone());
  const occ::handle<Geom2d_BSplineCurve> aCurveWithoutScale = anInterpWithoutScale.Curve();
  EXPECT_FALSE(aCurveWithoutScale.IsNull());

  // Both curves must pass through all given points
  const double aTol = Precision::Confusion() * 10;
  for (int anIndex = 1; anIndex <= aPoints->Length(); ++anIndex)
  {
    const gp_Pnt2d  aPtOnCurveWithScale = aCurveWithScale->EvalD0(aCurveWithScale->Knot(anIndex));
    const gp_Pnt2d& aPt                 = aPointsArray(anIndex);
    EXPECT_NEAR(aPt.X(), aPtOnCurveWithScale.X(), aTol) << " at point index " << anIndex;
    EXPECT_NEAR(aPt.Y(), aPtOnCurveWithScale.Y(), aTol) << " at point index " << anIndex;
  }
}
