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

#include <IntCurveSurface_IntersectionPoint.hxx>
#include <IntCurveSurface_TransitionOnCurve.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

// Regression test for bug #63: default constructor must initialize transition enum.
TEST(IntCurveSurface_IntersectionPoint, DefaultConstructor_TransitionInitialized)
{
  IntCurveSurface_IntersectionPoint aPt;

  // Transition must be deterministic (initialized to IntCurveSurface_Tangent).
  EXPECT_EQ(aPt.Transition(), IntCurveSurface_Tangent);

  // Scalar fields must be zero-initialized.
  EXPECT_DOUBLE_EQ(aPt.U(), 0.0);
  EXPECT_DOUBLE_EQ(aPt.V(), 0.0);
  EXPECT_DOUBLE_EQ(aPt.W(), 0.0);
}

// Value constructor initializes all fields correctly.
TEST(IntCurveSurface_IntersectionPoint, ValueConstructor_AllFieldsSet)
{
  gp_Pnt                                  aP(1.0, 2.0, 3.0);
  IntCurveSurface_IntersectionPoint aPt(aP, 0.5, 0.6, 0.7, IntCurveSurface_In);

  EXPECT_NEAR(aPt.Pnt().X(), 1.0, 1e-15);
  EXPECT_NEAR(aPt.Pnt().Y(), 2.0, 1e-15);
  EXPECT_NEAR(aPt.Pnt().Z(), 3.0, 1e-15);
  EXPECT_DOUBLE_EQ(aPt.U(), 0.5);
  EXPECT_DOUBLE_EQ(aPt.V(), 0.6);
  EXPECT_DOUBLE_EQ(aPt.W(), 0.7);
  EXPECT_EQ(aPt.Transition(), IntCurveSurface_In);
}

// SetValues overwrites all fields including transition.
TEST(IntCurveSurface_IntersectionPoint, SetValues_OverwritesTransition)
{
  IntCurveSurface_IntersectionPoint aPt;
  EXPECT_EQ(aPt.Transition(), IntCurveSurface_Tangent);

  gp_Pnt aP(5.0, 6.0, 7.0);
  aPt.SetValues(aP, 1.0, 2.0, 3.0, IntCurveSurface_Out);
  EXPECT_EQ(aPt.Transition(), IntCurveSurface_Out);
}
