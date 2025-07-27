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

#include <HelixGeom_HelixCurve.hxx>
#include <HelixGeom_BuilderApproxCurve.hxx>
#include <HelixGeom_BuilderHelixCoil.hxx>
#include <HelixGeom_Tools.hxx>

#include <Geom_BSplineCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <TColGeom_SequenceOfCurve.hxx>
#include <Standard_ConstructionError.hxx>

class HelixGeomTest : public ::testing::Test
{
protected:
  void SetUp() override { myTolerance = 1.e-6; }

  Standard_Real myTolerance;
};

// Test HelixGeom_HelixCurve derivatives
TEST_F(HelixGeomTest, HelixCurve_Derivatives)
{
  HelixGeom_HelixCurve aHelix;
  aHelix.Load(0.0, 2.0 * M_PI, 5.0, 2.0, 0.0, Standard_True);

  Standard_Real aParam = M_PI / 2.0;
  gp_Pnt        aP;
  gp_Vec        aV1, aV2;

  // Test D1
  aHelix.D1(aParam, aP, aV1);
  EXPECT_NEAR(aP.X(), 0.0, 1e-15);
  EXPECT_NEAR(aP.Y(), 2.0, 1e-15);
  EXPECT_GT(aV1.Magnitude(), 0.0);

  // Test D2
  aHelix.D2(aParam, aP, aV1, aV2);
  EXPECT_NEAR(aP.X(), 0.0, 1e-15);
  EXPECT_NEAR(aP.Y(), 2.0, 1e-15);
  EXPECT_GT(aV2.Magnitude(), 0.0);

  // Test DN
  gp_Vec aVN1 = aHelix.DN(aParam, 1);
  gp_Vec aVN2 = aHelix.DN(aParam, 2);

  EXPECT_NEAR(aVN1.X(), aV1.X(), 1e-15);
  EXPECT_NEAR(aVN1.Y(), aV1.Y(), 1e-15);
  EXPECT_NEAR(aVN1.Z(), aV1.Z(), 1e-15);

  EXPECT_NEAR(aVN2.X(), aV2.X(), 1e-15);
  EXPECT_NEAR(aVN2.Y(), aV2.Y(), 1e-15);
  EXPECT_NEAR(aVN2.Z(), aV2.Z(), 1e-15);
}

// Test HelixGeom_HelixCurve error conditions
TEST_F(HelixGeomTest, HelixCurve_ErrorConditions)
{
  HelixGeom_HelixCurve aHelix;

  // Test invalid parameter range (T1 >= T2)
  EXPECT_THROW(aHelix.Load(2.0, 1.0, 5.0, 2.0, 0.0, Standard_True), Standard_ConstructionError);

  // Test negative pitch
  EXPECT_THROW(aHelix.Load(0.0, 2.0 * M_PI, -1.0, 2.0, 0.0, Standard_True),
               Standard_ConstructionError);

  // Test negative radius
  EXPECT_THROW(aHelix.Load(0.0, 2.0 * M_PI, 5.0, -1.0, 0.0, Standard_True),
               Standard_ConstructionError);

  // Test invalid taper angle
  EXPECT_THROW(aHelix.Load(0.0, 2.0 * M_PI, 5.0, 2.0, M_PI / 2.0, Standard_True),
               Standard_ConstructionError);
}

// Test HelixGeom_HelixCurve with counter-clockwise direction
TEST_F(HelixGeomTest, HelixCurve_CounterClockwise)
{
  HelixGeom_HelixCurve aHelix;
  aHelix.Load(0.0, 2.0 * M_PI, 5.0, 2.0, 0.0, Standard_False); // Counter-clockwise

  gp_Pnt aP0 = aHelix.Value(0.0);
  gp_Pnt aP1 = aHelix.Value(M_PI / 2.0);

  EXPECT_NEAR(aP0.X(), 2.0, 1e-15);
  EXPECT_NEAR(aP0.Y(), 0.0, 1e-15);

  EXPECT_NEAR(aP1.X(), 0.0, 1e-15);
  EXPECT_NEAR(aP1.Y(), -2.0, 1e-15); // Negative Y for counter-clockwise
}

// Test HelixGeom_HelixCurve adaptor interface
TEST_F(HelixGeomTest, HelixCurve_AdaptorInterface)
{
  HelixGeom_HelixCurve aHelix;
  aHelix.Load(0.0, 4.0 * M_PI, 10.0, 3.0, 0.0, Standard_True);

  // Test continuity
  EXPECT_EQ(aHelix.Continuity(), GeomAbs_CN);

  // Test intervals
  EXPECT_EQ(aHelix.NbIntervals(GeomAbs_C0), 1);
  EXPECT_EQ(aHelix.NbIntervals(GeomAbs_C1), 1);
  EXPECT_EQ(aHelix.NbIntervals(GeomAbs_C2), 1);

  // Test intervals array
  TColStd_Array1OfReal anIntervals(1, 2);
  aHelix.Intervals(anIntervals, GeomAbs_C0);
  EXPECT_DOUBLE_EQ(anIntervals(1), 0.0);
  EXPECT_DOUBLE_EQ(anIntervals(2), 4.0 * M_PI);

  // Test not implemented methods
  EXPECT_THROW(aHelix.Resolution(1.0), Standard_NotImplemented);
  EXPECT_THROW(aHelix.IsClosed(), Standard_NotImplemented);
  EXPECT_THROW(aHelix.IsPeriodic(), Standard_NotImplemented);
  EXPECT_THROW(aHelix.Period(), Standard_DomainError);
}

// Test HelixGeom_BuilderHelixCoil approximation
TEST_F(HelixGeomTest, BuilderHelixCoil_Approximation)
{
  HelixGeom_BuilderHelixCoil aBuilder;

  aBuilder.SetTolerance(myTolerance);
  aBuilder.SetApproxParameters(GeomAbs_C2, 8, 100);
  aBuilder.SetCurveParameters(0.0,
                              2.0 * M_PI,
                              20.0,
                              5.0,
                              0.05,
                              Standard_True); // Reduced parameter range and taper angle

  aBuilder.Perform();

  EXPECT_EQ(aBuilder.ErrorStatus(), 0);
  EXPECT_LE(aBuilder.ToleranceReached(),
            0.1); // Allow up to 0.1 tolerance for tapered helix approximation

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

// Test HelixGeom_Tools static methods
TEST_F(HelixGeomTest, Tools_ApprCurve3D)
{
  // Create a helix curve adaptor
  HelixGeom_HelixCurve aHelix;
  aHelix.Load(0.0, 2.0 * M_PI, 10.0, 3.0, 0.0, Standard_True);
  Handle(HelixGeom_HelixCurve) aHAdaptor = new HelixGeom_HelixCurve(aHelix);

  Handle(Geom_BSplineCurve) aBSpline;
  Standard_Real             aMaxError;

  Standard_Integer aResult = HelixGeom_Tools::ApprCurve3D(aHAdaptor,
                                                          myTolerance,
                                                          GeomAbs_C1,
                                                          50, // Max segments
                                                          6,  // Max degree
                                                          aBSpline,
                                                          aMaxError);

  EXPECT_EQ(aResult, 0);
  EXPECT_FALSE(aBSpline.IsNull());
  EXPECT_LE(aMaxError, myTolerance * 10);

  // Verify approximation quality by sampling points
  Standard_Integer aNbSamples = 10;
  for (Standard_Integer i = 0; i <= aNbSamples; i++)
  {
    Standard_Real aParam =
      aHelix.FirstParameter() + i * (aHelix.LastParameter() - aHelix.FirstParameter()) / aNbSamples;

    gp_Pnt aOrigPnt = aHelix.Value(aParam);

    // Map parameter to B-spline parameter space
    Standard_Real aBSplineParam =
      aBSpline->FirstParameter()
      + i * (aBSpline->LastParameter() - aBSpline->FirstParameter()) / aNbSamples;

    gp_Pnt aApproxPnt;
    aBSpline->D0(aBSplineParam, aApproxPnt);

    Standard_Real aDistance = aOrigPnt.Distance(aApproxPnt);
    EXPECT_LE(aDistance, aMaxError * 2); // Allow some margin
  }
}

// Test HelixGeom_Tools with different continuity requirements
TEST_F(HelixGeomTest, Tools_DifferentContinuity)
{
  HelixGeom_HelixCurve aHelix;
  aHelix.Load(0.0, 6.0 * M_PI, 15.0, 4.0, 0.05, Standard_True);
  Handle(HelixGeom_HelixCurve) aHAdaptor = new HelixGeom_HelixCurve(aHelix);

  // Test C0 continuity
  Handle(Geom_BSplineCurve) aBSplineC0;
  Standard_Real             aMaxErrorC0;
  Standard_Integer          aResultC0 = HelixGeom_Tools::ApprCurve3D(aHAdaptor,
                                                            myTolerance,
                                                            GeomAbs_C0,
                                                            30,
                                                            4,
                                                            aBSplineC0,
                                                            aMaxErrorC0);

  EXPECT_EQ(aResultC0, 0);
  EXPECT_FALSE(aBSplineC0.IsNull());

  // Test C2 continuity
  Handle(Geom_BSplineCurve) aBSplineC2;
  Standard_Real             aMaxErrorC2;
  Standard_Integer          aResultC2 = HelixGeom_Tools::ApprCurve3D(aHAdaptor,
                                                            myTolerance,
                                                            GeomAbs_C2,
                                                            30,
                                                            6,
                                                            aBSplineC2,
                                                            aMaxErrorC2);

  EXPECT_EQ(aResultC2, 0);
  EXPECT_FALSE(aBSplineC2.IsNull());

  // C2 curve should generally have higher degree
  EXPECT_GE(aBSplineC2->Degree(), aBSplineC0->Degree());
}

// Test Builder with default parameters
TEST_F(HelixGeomTest, BuilderHelixCoil_DefaultParameters)
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

// Test parameter getter/setter symmetry
TEST_F(HelixGeomTest, BuilderHelixCoil_ParameterSymmetry)
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