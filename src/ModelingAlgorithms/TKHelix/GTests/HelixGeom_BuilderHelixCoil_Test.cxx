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

#include <HelixGeom_BuilderHelixCoil.hxx>
#include <TColGeom_SequenceOfCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <gp_Pnt.hxx>

class HelixGeom_BuilderHelixCoil_Test : public ::testing::Test
{
protected:
  void SetUp() override { myTolerance = 1.e-4; }

  Standard_Real myTolerance;
};

TEST_F(HelixGeom_BuilderHelixCoil_Test, BasicConstruction)
{
  HelixGeom_BuilderHelixCoil aBuilder;

  aBuilder.SetTolerance(myTolerance);
  aBuilder.SetCurveParameters(0.0, 2.0 * M_PI, 5.0, 2.0, 0.0, Standard_True);

  aBuilder.Perform();

  EXPECT_EQ(aBuilder.ErrorStatus(), 0);

  const TColGeom_SequenceOfCurve& aCurves = aBuilder.Curves();
  EXPECT_EQ(aCurves.Length(), 1);

  Handle(Geom_Curve) aCurve = aCurves(1);
  EXPECT_FALSE(aCurve.IsNull());

  // Test curve endpoints
  gp_Pnt aP1, aP2;
  aCurve->D0(aCurve->FirstParameter(), aP1);
  aCurve->D0(aCurve->LastParameter(), aP2);

  EXPECT_NEAR(aP1.X(), 2.0, myTolerance);
  EXPECT_NEAR(aP1.Y(), 0.0, myTolerance);
  EXPECT_NEAR(aP1.Z(), 0.0, myTolerance);

  EXPECT_NEAR(aP2.X(), 2.0, myTolerance);
  EXPECT_NEAR(aP2.Y(), 0.0, myTolerance);
  EXPECT_NEAR(aP2.Z(), 5.0, myTolerance);
}

TEST_F(HelixGeom_BuilderHelixCoil_Test, DefaultParameters)
{
  HelixGeom_BuilderHelixCoil aBuilder;

  // Use default parameters
  aBuilder.Perform();

  EXPECT_EQ(aBuilder.ErrorStatus(), 0);

  // Check default approximation parameters
  GeomAbs_Shape    aCont;
  Standard_Integer aMaxDegree, aMaxSeg;
  aBuilder.ApproxParameters(aCont, aMaxDegree, aMaxSeg);

  EXPECT_EQ(aCont, GeomAbs_C2);
  EXPECT_EQ(aMaxDegree, 8);
  EXPECT_EQ(aMaxSeg, 150);

  EXPECT_EQ(aBuilder.Tolerance(), 0.0001);
}

TEST_F(HelixGeom_BuilderHelixCoil_Test, ParameterSymmetry)
{
  HelixGeom_BuilderHelixCoil aBuilder;

  // Set parameters
  Standard_Real    aT1 = 0.5, aT2 = 5.5, aPitch = 12.5, aRStart = 3.5, aTaperAngle = 0.15;
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

TEST_F(HelixGeom_BuilderHelixCoil_Test, TaperedHelix)
{
  HelixGeom_BuilderHelixCoil aBuilder;

  aBuilder.SetTolerance(myTolerance);
  aBuilder.SetApproxParameters(GeomAbs_C2, 8, 100);
  aBuilder.SetCurveParameters(0.0, 4.0 * M_PI, 20.0, 5.0, 0.1, Standard_True);

  aBuilder.Perform();

  EXPECT_EQ(aBuilder.ErrorStatus(), 0);
  EXPECT_LE(aBuilder.ToleranceReached(), myTolerance * 10); // Allow some tolerance margin

  const TColGeom_SequenceOfCurve& aCurves = aBuilder.Curves();
  EXPECT_EQ(aCurves.Length(), 1);

  Handle(Geom_Curve) aCurve = aCurves(1);
  EXPECT_FALSE(aCurve.IsNull());

  // Verify that the curve is a B-spline
  Handle(Geom_BSplineCurve) aBSpline = Handle(Geom_BSplineCurve)::DownCast(aCurve);
  EXPECT_FALSE(aBSpline.IsNull());

  // Check B-spline properties
  EXPECT_LE(aBSpline->Degree(), 8);
  EXPECT_GT(aBSpline->NbPoles(), 0);
}