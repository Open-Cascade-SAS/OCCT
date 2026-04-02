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

#include <Plate_Plate.hxx>
#include <Plate_PinpointConstraint.hxx>
#include <gp_XY.hxx>
#include <gp_XYZ.hxx>

#include <gtest/gtest.h>

// Regression test for bug #30: Init() must set IsDone() to false.
TEST(Plate_Plate, Init_SetsIsDoneFalse)
{
  Plate_Plate aPlate;

  // Freshly constructed plate is not done.
  EXPECT_FALSE(aPlate.IsDone());

  // Add a constraint and solve.
  aPlate.Load(Plate_PinpointConstraint(gp_XY(0., 0.), gp_XYZ(0., 0., 1.), 0, 0));
  aPlate.Load(Plate_PinpointConstraint(gp_XY(1., 0.), gp_XYZ(0., 0., 0.), 0, 0));
  aPlate.Load(Plate_PinpointConstraint(gp_XY(0., 1.), gp_XYZ(0., 0., 0.), 0, 0));
  aPlate.SolveTI(2);

  // After Init(), IsDone must be false regardless of prior success.
  aPlate.Init();
  EXPECT_FALSE(aPlate.IsDone());
}

// After Init(), Evaluate must return zero vector (no solution).
TEST(Plate_Plate, Init_EvaluateReturnsZero)
{
  Plate_Plate aPlate;
  aPlate.Init();

  gp_XYZ aResult = aPlate.Evaluate(gp_XY(0.5, 0.5));
  EXPECT_DOUBLE_EQ(aResult.X(), 0.0);
  EXPECT_DOUBLE_EQ(aResult.Y(), 0.0);
  EXPECT_DOUBLE_EQ(aResult.Z(), 0.0);
}
