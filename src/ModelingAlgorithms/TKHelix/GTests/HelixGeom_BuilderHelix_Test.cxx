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

#include <HelixGeom_BuilderHelix.hxx>
#include <TColGeom_SequenceOfCurve.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>

class HelixGeom_BuilderHelix_Test : public ::testing::Test
{
protected:
  void SetUp() override { myTolerance = 1.e-4; }

  Standard_Real myTolerance;
};

TEST_F(HelixGeom_BuilderHelix_Test, SingleCoil)
{
  HelixGeom_BuilderHelix aBuilder;

  gp_Ax2 aPosition(gp_Pnt(0., 0., 0.), gp_Dir(gp_Dir::D::Z), gp_Dir(gp_Dir::D::X));
  aBuilder.SetPosition(aPosition);
  aBuilder.SetTolerance(myTolerance);
  aBuilder.SetCurveParameters(0.0, 2.0 * M_PI, 10.0, 5.0, 0.0, Standard_True);

  aBuilder.Perform();

  EXPECT_EQ(aBuilder.ErrorStatus(), 0);

  const TColGeom_SequenceOfCurve& aCurves = aBuilder.Curves();
  EXPECT_EQ(aCurves.Length(), 1);
}

TEST_F(HelixGeom_BuilderHelix_Test, MultipleCoils)
{
  HelixGeom_BuilderHelix aBuilder;

  gp_Ax2 aPosition(gp_Pnt(0., 0., 0.), gp_Dir(gp_Dir::D::Z), gp_Dir(gp_Dir::D::X));
  aBuilder.SetPosition(aPosition);
  aBuilder.SetTolerance(myTolerance);

  // 3 full turns
  aBuilder.SetCurveParameters(0.0, 6.0 * M_PI, 10.0, 5.0, 0.0, Standard_True);

  aBuilder.Perform();

  EXPECT_EQ(aBuilder.ErrorStatus(), 0);

  const TColGeom_SequenceOfCurve& aCurves = aBuilder.Curves();
  EXPECT_EQ(aCurves.Length(), 3); // Should split into 3 coils
}

TEST_F(HelixGeom_BuilderHelix_Test, PositionGetterSetter)
{
  HelixGeom_BuilderHelix aBuilder;

  gp_Ax2 aTestPosition(gp_Pnt(10., 20., 30.), gp_Dir(gp_Dir::D::X), gp_Dir(gp_Dir::D::Y));
  aBuilder.SetPosition(aTestPosition);

  const gp_Ax2& aRetrievedPosition = aBuilder.Position();

  EXPECT_TRUE(aRetrievedPosition.Location().IsEqual(aTestPosition.Location(), 1e-15));
  EXPECT_TRUE(aRetrievedPosition.Direction().IsEqual(aTestPosition.Direction(), 1e-15));
  EXPECT_TRUE(aRetrievedPosition.XDirection().IsEqual(aTestPosition.XDirection(), 1e-15));
}

TEST_F(HelixGeom_BuilderHelix_Test, ParameterManagement)
{
  HelixGeom_BuilderHelix aBuilder;

  // Set curve parameters
  Standard_Real    aT1 = 1.0, aT2 = 7.0, aPitch = 15.0, aRStart = 4.0, aTaperAngle = 0.2;
  Standard_Boolean aIsClockwise = Standard_False;

  aBuilder.SetCurveParameters(aT1, aT2, aPitch, aRStart, aTaperAngle, aIsClockwise);

  // Get parameters back
  Standard_Real    aT1_out, aT2_out, aPitch_out, aRStart_out, aTaperAngle_out;
  Standard_Boolean aIsClockwise_out;

  aBuilder
    .CurveParameters(aT1_out, aT2_out, aPitch_out, aRStart_out, aTaperAngle_out, aIsClockwise_out);

  EXPECT_DOUBLE_EQ(aT1, aT1_out);
  EXPECT_DOUBLE_EQ(aT2, aT2_out);
  EXPECT_DOUBLE_EQ(aPitch, aPitch_out);
  EXPECT_DOUBLE_EQ(aRStart, aRStart_out);
  EXPECT_DOUBLE_EQ(aTaperAngle, aTaperAngle_out);
  EXPECT_EQ(aIsClockwise, aIsClockwise_out);
}