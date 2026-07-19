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

#include <ProjLib_Cone.hxx>
#include <Precision.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Cone.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

#include <cmath>

TEST(ProjLib_ConeTest, ProjectCircle_ParallelAxes_DoneLine)
{
  const gp_Cone aCone(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), M_PI / 6.0, 5.0);
  const gp_Circ aCircle(
    gp_Ax2(gp_Pnt(0.0, 0.0, 10.0), gp_Dir(0.0, 0.0, 1.0), gp_Dir(1.0, 0.0, 0.0)),
    2.0);

  ProjLib_Cone aProjector(aCone, aCircle);
  ASSERT_TRUE(aProjector.IsDone());
  EXPECT_EQ(aProjector.GetType(), GeomAbs_Line);
  EXPECT_NEAR(aProjector.Line().Direction().Y(), 0.0, Precision::Angular());
}

TEST(ProjLib_ConeTest, ProjectCircle_NonParallelAxes_NotDone)
{
  const gp_Cone aCone(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), M_PI / 6.0, 5.0);
  const gp_Circ aCircle(
    gp_Ax2(gp_Pnt(0.0, 0.0, 10.0), gp_Dir(1.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0)),
    2.0);

  ProjLib_Cone aProjector(aCone, aCircle);
  EXPECT_FALSE(aProjector.IsDone());
}

TEST(ProjLib_ConeTest, ProjectCircle_OppositeNormal_DoneLineWithNegativeDirection)
{
  const gp_Cone aCone(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), M_PI / 6.0, 5.0);
  const gp_Circ aCircle(
    gp_Ax2(gp_Pnt(0.0, 0.0, 6.0), gp_Dir(0.0, 0.0, -1.0), gp_Dir(1.0, 0.0, 0.0)),
    2.0);

  ProjLib_Cone aProjector(aCone, aCircle);
  ASSERT_TRUE(aProjector.IsDone());
  ASSERT_EQ(aProjector.GetType(), GeomAbs_Line);
  EXPECT_LT(aProjector.Line().Direction().X(), 0.0);
}
