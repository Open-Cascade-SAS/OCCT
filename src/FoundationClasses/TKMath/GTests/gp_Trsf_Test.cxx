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

#include <gp_Trsf.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

TEST(gp_TrsfTest, OCC23361_TransformationComposition)
{
  gp_Pnt aP(0, 0, 2);

  gp_Trsf aT1, aT2;
  aT1.SetRotation(gp_Ax1(aP, gp_Dir(gp_Dir::D::Y)), -0.49328285294022267);
  aT2.SetRotation(gp_Ax1(aP, gp_Dir(gp_Dir::D::Z)), 0.87538474718473880);

  gp_Trsf aTComp = aT2 * aT1;

  gp_Pnt aP1(10, 3, 4);
  gp_Pnt aP2 = aP1.Transformed(aTComp);
  gp_Pnt aP3 = aP1.Transformed(aT1);
  aP3.Transform(aT2);

  // Points must be equal: equivalent transformations should produce equal points
  EXPECT_TRUE(aP2.IsEqual(aP3, Precision::Confusion()));
}

TEST(gp_TrsfTest, PointMirrorAtNonzeroCenter_IsItsOwnInverse)
{
  for (const gp_Pnt& aCenter : {gp_Pnt(), gp_Pnt(2, -1, 3), gp_Pnt(-4, 5, 0.5)})
  {
    gp_Trsf aMirror;
    aMirror.SetMirror(aCenter);
    const gp_Trsf anInverse        = aMirror.Inverted();
    gp_Trsf       anInPlaceInverse = aMirror;
    anInPlaceInverse.Invert();
    for (int aRow = 1; aRow <= 3; ++aRow)
    {
      for (int aColumn = 1; aColumn <= 4; ++aColumn)
      {
        EXPECT_DOUBLE_EQ(anInverse.Value(aRow, aColumn), aMirror.Value(aRow, aColumn));
        EXPECT_DOUBLE_EQ(anInPlaceInverse.Value(aRow, aColumn), aMirror.Value(aRow, aColumn));
      }
    }
    for (const gp_Pnt& aPoint : {gp_Pnt(3, 2, 1), gp_Pnt(-7, 4, -2), aCenter})
    {
      EXPECT_DOUBLE_EQ(aPoint.Transformed(aMirror).Transformed(anInverse).Distance(aPoint), 0.0);
      EXPECT_DOUBLE_EQ(aPoint.Transformed(aMirror.Multiplied(anInverse)).Distance(aPoint), 0.0);
    }
  }
}

TEST(gp_TrsfTest, PointMirror2dAtNonzeroCenter_IsItsOwnInverse)
{
  for (const gp_Pnt2d& aCenter : {gp_Pnt2d(), gp_Pnt2d(2, -1), gp_Pnt2d(-4, 0.5)})
  {
    gp_Trsf2d aMirror;
    aMirror.SetMirror(aCenter);
    const gp_Trsf2d anInverse        = aMirror.Inverted();
    gp_Trsf2d       anInPlaceInverse = aMirror;
    anInPlaceInverse.Invert();
    for (int aRow = 1; aRow <= 2; ++aRow)
    {
      for (int aColumn = 1; aColumn <= 3; ++aColumn)
      {
        EXPECT_DOUBLE_EQ(anInverse.Value(aRow, aColumn), aMirror.Value(aRow, aColumn));
        EXPECT_DOUBLE_EQ(anInPlaceInverse.Value(aRow, aColumn), aMirror.Value(aRow, aColumn));
      }
    }
    for (const gp_Pnt2d& aPoint : {gp_Pnt2d(3, 2), gp_Pnt2d(-7, 4), aCenter})
    {
      EXPECT_DOUBLE_EQ(aPoint.Transformed(aMirror).Transformed(anInverse).Distance(aPoint), 0.0);
      EXPECT_DOUBLE_EQ(aPoint.Transformed(aMirror.Multiplied(anInverse)).Distance(aPoint), 0.0);
    }
    const gp_Trsf aMirror3d(aMirror);
    const gp_Pnt  aPoint(3, 2, 0);
    EXPECT_DOUBLE_EQ(
      aPoint.Transformed(aMirror3d).Transformed(aMirror3d.Inverted()).Distance(aPoint),
      0.0);
  }
}
