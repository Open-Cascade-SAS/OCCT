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
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_GTrsf.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>

#include <gtest/gtest.h>

// OCC13963: SetAffinity with ratio=1 is identity transform for any point.
// The plane is defined by gp_Ax2 at origin with main dir (1,-1,0)/sqrt(2).
TEST(gp_GTrsfTest, OCC13963_SetAffinity_Ratio1_IsIdentity)
{
  const gp_Ax2 aPln(gp_Pnt(0., 0., 0.), gp_Dir(1., -1., 0.));
  gp_GTrsf     aTrf;
  aTrf.SetAffinity(aPln, 1.0);

  gp_XYZ aOrigin(1.0, 0.0, 0.0);
  gp_XYZ aResult(aOrigin);
  aTrf.Transforms(aResult);

  EXPECT_NEAR(aResult.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aResult.Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aResult.Z(), 0.0, Precision::Confusion());
}

// OCC13963: SetAffinity with ratio=2 scales the point component along the plane normal.
// Point (1,0,0) through plane with normal (1,-1,0)/sqrt(2) with ratio 2 gives (1.5,-0.5,0).
TEST(gp_GTrsfTest, OCC13963_SetAffinity_Ratio2_ScalesAlongNormal)
{
  const gp_Ax2 aPln(gp_Pnt(0., 0., 0.), gp_Dir(1., -1., 0.));
  gp_GTrsf     aTrf;
  aTrf.SetAffinity(aPln, 2.0);

  gp_XYZ aOrigin(1.0, 0.0, 0.0);
  gp_XYZ aResult(aOrigin);
  aTrf.Transforms(aResult);

  EXPECT_NEAR(aResult.X(), 1.5, Precision::Confusion());
  EXPECT_NEAR(aResult.Y(), -0.5, Precision::Confusion());
  EXPECT_NEAR(aResult.Z(), 0.0, Precision::Confusion());
}
