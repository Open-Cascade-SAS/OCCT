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

#include <Precision.hxx>
#include <gce_MakeCirc2d.hxx>
#include <gp.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Pnt2d.hxx>

#include <gtest/gtest.h>

TEST(gce_MakeCirc2dTest, ThreePoints_AllCoincidentWithinResolution_DoneZeroRadius)
{
  // Intentional: points inside gp::Resolution() are treated as coincident
  // and should produce a valid zero-radius circle.
  const double aDelta = 0.25 * gp::Resolution();

  const gp_Pnt2d aP1(0.0, 0.0);
  const gp_Pnt2d aP2(aDelta, 0.0);
  const gp_Pnt2d aP3(0.0, aDelta);

  gce_MakeCirc2d aMaker(aP1, aP2, aP3);
  ASSERT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);
  EXPECT_NEAR(aMaker.Value().Radius(), 0.0, Precision::Confusion());
}

TEST(gce_MakeCirc2dTest, ThreePoints_OneNearCoincidentPair_Done)
{
  const gp_Pnt2d aP1(0.0, 0.0);
  const gp_Pnt2d aP2(0.5 * gp::Resolution(), 0.0);
  const gp_Pnt2d aP3(0.0, 1.0);

  gce_MakeCirc2d aMaker(aP1, aP2, aP3);
  ASSERT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);
  EXPECT_GT(aMaker.Value().Radius(), 0.0);
}

TEST(gce_MakeCirc2dTest, ThreePoints_AtResolutionBoundary_Done)
{
  const gp_Pnt2d aP1(0.0, 0.0);
  const gp_Pnt2d aP2(gp::Resolution(), 0.0);
  const gp_Pnt2d aP3(0.0, 1.0);

  gce_MakeCirc2d aMaker(aP1, aP2, aP3);
  ASSERT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);
  EXPECT_GT(aMaker.Value().Radius(), 0.0);
}
