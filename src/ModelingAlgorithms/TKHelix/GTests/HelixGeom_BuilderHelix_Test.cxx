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
#include <Geom_Curve.hxx>
#include <NCollection_Sequence.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>

#include <cmath>

class HelixGeom_BuilderHelix_Test : public ::testing::Test
{
protected:
  void SetUp() override { myTolerance = 1.e-4; }

  double myTolerance;
};

TEST_F(HelixGeom_BuilderHelix_Test, SingleCoil)
{
  HelixGeom_BuilderHelix aBuilder;

  gp_Ax2 aPosition(gp_Pnt(0., 0., 0.), gp_Dir(gp_Dir::D::Z), gp_Dir(gp_Dir::D::X));
  aBuilder.SetPosition(aPosition);
  aBuilder.SetTolerance(myTolerance);
  aBuilder.SetCurveParameters(0.0, 2.0 * M_PI, 10.0, 5.0, 0.0, true);

  aBuilder.Perform();

  EXPECT_EQ(aBuilder.ErrorStatus(), 0);

  const NCollection_Sequence<occ::handle<Geom_Curve>>& aCurves = aBuilder.Curves();
  EXPECT_EQ(aCurves.Length(), 1);
}

TEST_F(HelixGeom_BuilderHelix_Test, MultipleCoils)
{
  HelixGeom_BuilderHelix aBuilder;

  gp_Ax2 aPosition(gp_Pnt(0., 0., 0.), gp_Dir(gp_Dir::D::Z), gp_Dir(gp_Dir::D::X));
  aBuilder.SetPosition(aPosition);
  aBuilder.SetTolerance(myTolerance);

  // 3 full turns
  aBuilder.SetCurveParameters(0.0, 6.0 * M_PI, 10.0, 5.0, 0.0, true);

  aBuilder.Perform();

  EXPECT_EQ(aBuilder.ErrorStatus(), 0);

  const NCollection_Sequence<occ::handle<Geom_Curve>>& aCurves = aBuilder.Curves();
  EXPECT_EQ(aCurves.Length(), 3); // Should split into 3 coils
}

TEST_F(HelixGeom_BuilderHelix_Test, TaperedMultipleCoilsPreserveRadius)
{
  const double aT1    = 0.7;
  const double aT2    = aT1 + 2.5 * M_PI * 2.0;
  const double aPitch = 4.6;
  const double aR0    = 3.2;
  const double aTaper = 0.23;
  const double aDR    = aPitch / (2.0 * M_PI) * std::tan(aTaper);

  HelixGeom_BuilderHelix aBuilder;
  aBuilder.SetPosition(gp_Ax2(gp_Pnt(0., 0., 0.), gp_Dir(gp_Dir::D::Z), gp_Dir(gp_Dir::D::X)));
  aBuilder.SetTolerance(myTolerance);
  aBuilder.SetCurveParameters(aT1, aT2, aPitch, aR0, aTaper, true);
  aBuilder.Perform();

  ASSERT_EQ(aBuilder.ErrorStatus(), 0);
  const NCollection_Sequence<occ::handle<Geom_Curve>>& aCurves = aBuilder.Curves();
  ASSERT_EQ(aCurves.Length(), 3);

  gp_Pnt aStart, anEnd;
  aCurves.First()->D0(aCurves.First()->FirstParameter(), aStart);
  aCurves.Last()->D0(aCurves.Last()->LastParameter(), anEnd);
  EXPECT_NEAR(aStart.X(), aR0 * std::cos(aT1), myTolerance);
  EXPECT_NEAR(aStart.Y(), aR0 * std::sin(aT1), myTolerance);
  EXPECT_NEAR(anEnd.X(), (aR0 + aDR * (aT2 - aT1)) * std::cos(aT2), myTolerance);
  EXPECT_NEAR(anEnd.Y(), (aR0 + aDR * (aT2 - aT1)) * std::sin(aT2), myTolerance);
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
  double aT1 = 1.0, aT2 = 7.0, aPitch = 15.0, aRStart = 4.0, aTaperAngle = 0.2;
  bool   aIsClockwise = false;

  aBuilder.SetCurveParameters(aT1, aT2, aPitch, aRStart, aTaperAngle, aIsClockwise);

  // Get parameters back
  double aT1_out, aT2_out, aPitch_out, aRStart_out, aTaperAngle_out;
  bool   aIsClockwise_out;

  aBuilder
    .CurveParameters(aT1_out, aT2_out, aPitch_out, aRStart_out, aTaperAngle_out, aIsClockwise_out);

  EXPECT_DOUBLE_EQ(aT1, aT1_out);
  EXPECT_DOUBLE_EQ(aT2, aT2_out);
  EXPECT_DOUBLE_EQ(aPitch, aPitch_out);
  EXPECT_DOUBLE_EQ(aRStart, aRStart_out);
  EXPECT_DOUBLE_EQ(aTaperAngle, aTaperAngle_out);
  EXPECT_EQ(aIsClockwise, aIsClockwise_out);
}

//==================================================================================================

TEST(HelixGeom_BuilderHelixTest, SmallTaperIsKeptAcrossCoils)
{
  const double aFirst       = 0.7;
  const double aLast        = aFirst + 5 * M_PI;
  const double aPitch       = 4.6;
  const double aStartRadius = 3.2;
  const double aTolerance   = 1.e-7;
  for (const double aTaper : {-5.e-5, 0.0, 5.e-5})
  {
    SCOPED_TRACE(aTaper);
    HelixGeom_BuilderHelix aBuilder;
    aBuilder.SetTolerance(aTolerance);
    aBuilder.SetCurveParameters(aFirst, aLast, aPitch, aStartRadius, aTaper, true);
    aBuilder.Perform();
    ASSERT_EQ(aBuilder.ErrorStatus(), 0);
    const auto& aCurves = aBuilder.Curves();
    ASSERT_EQ(aCurves.Length(), 3);
    const double aRadius = aStartRadius + aPitch / (2 * M_PI) * std::tan(aTaper) * (aLast - aFirst);
    const gp_Pnt anEnd   = aCurves.Last()->EvalD0(aCurves.Last()->LastParameter());
    EXPECT_NEAR(anEnd.X(), aRadius * std::cos(aLast), aTolerance);
    EXPECT_NEAR(anEnd.Y(), aRadius * std::sin(aLast), aTolerance);
    for (int anIndex = 2; anIndex <= aCurves.Length(); ++anIndex)
    {
      EXPECT_LE(aCurves(anIndex - 1)
                  ->EvalD0(aCurves(anIndex - 1)->LastParameter())
                  .Distance(aCurves(anIndex)->EvalD0(aCurves(anIndex)->FirstParameter())),
                2 * aTolerance);
    }
  }
}

//==================================================================================================

TEST(HelixGeom_BuilderHelixTest, ShortFinalCoilKeepsRequestedEndpoint)
{
  const double           aLast      = 2 * M_PI + 1.e-7;
  const double           aPitch     = 1.e4;
  const double           aTolerance = 1.e-7;
  HelixGeom_BuilderHelix aBuilder;
  aBuilder.SetTolerance(aTolerance);
  aBuilder.SetCurveParameters(0, aLast, aPitch, 3.2, 0, true);
  aBuilder.Perform();
  ASSERT_EQ(aBuilder.ErrorStatus(), 0);
  const auto& aCurves = aBuilder.Curves();
  ASSERT_FALSE(aCurves.IsEmpty());
  const gp_Pnt anEnd = aCurves.Last()->EvalD0(aCurves.Last()->LastParameter());
  EXPECT_NEAR(anEnd.Z(), aPitch / (2 * M_PI) * aLast, aTolerance);
  EXPECT_NEAR(anEnd.Y(), 3.2 * std::sin(aLast), aTolerance);
}
