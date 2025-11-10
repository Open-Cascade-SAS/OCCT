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

#include <GeomAPI_Interpolate.hxx>
#include <Geom_BSplineCurve.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>

#include <cmath>

// Test BUC60902: GeomAPI_Interpolate tangent computation
TEST(GeomAPI_Interpolate_Test, BUC60902_TangentPreservation)
{
  // Create 5 points along a sine wave
  Handle(TColgp_HArray1OfPnt) aPnts = new TColgp_HArray1OfPnt(1, 5);
  gp_Pnt                      aP(0., 0., 0.);
  for (Standard_Integer i = 1; i <= 5; i++)
  {
    aP.SetX((i - 1) * 1.57);
    aP.SetY(sin((i - 1) * 1.57));
    aPnts->SetValue(i, aP);
  }

  // First interpolation without tangents
  GeomAPI_Interpolate anInterpolater(aPnts, Standard_False, Precision::Confusion());
  anInterpolater.Perform();
  ASSERT_TRUE(anInterpolater.IsDone()) << "First interpolation should succeed";

  // Get the generated tangents
  Handle(Geom_BSplineCurve) aCur = anInterpolater.Curve();
  ASSERT_FALSE(aCur.IsNull()) << "Interpolated curve should not be null";

  gp_Vec aFirstTang, aLastTang;
  aCur->D1(aCur->FirstParameter(), aP, aFirstTang);
  aCur->D1(aCur->LastParameter(), aP, aLastTang);

  // Second interpolation with the same tangents explicitly provided
  GeomAPI_Interpolate anInterpolater1(aPnts, Standard_False, Precision::Confusion());
  anInterpolater1.Load(aFirstTang, aLastTang, Standard_False);
  anInterpolater1.Perform();
  ASSERT_TRUE(anInterpolater1.IsDone()) << "Second interpolation should succeed";

  // Get the tangents after recomputation
  aCur = anInterpolater1.Curve();
  ASSERT_FALSE(aCur.IsNull()) << "Second interpolated curve should not be null";

  gp_Vec aFirstTang1, aLastTang1;
  aCur->D1(aCur->FirstParameter(), aP, aFirstTang1);
  aCur->D1(aCur->LastParameter(), aP, aLastTang1);

  // Verify that the tangents are preserved
  EXPECT_TRUE(aFirstTang.IsEqual(aFirstTang1, Precision::Confusion(), Precision::Angular()))
    << "First tangent should be preserved after recomputation";

  EXPECT_TRUE(aLastTang.IsEqual(aLastTang1, Precision::Confusion(), Precision::Angular()))
    << "Last tangent should be preserved after recomputation";
}
