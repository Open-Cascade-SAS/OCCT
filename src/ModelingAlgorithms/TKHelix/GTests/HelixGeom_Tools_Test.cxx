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

#include <HelixGeom_Tools.hxx>
#include <HelixGeom_HelixCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>

class HelixGeom_Tools_Test : public ::testing::Test
{
protected:
  void SetUp() override { myTolerance = 1.e-6; }

  double myTolerance;
};

TEST_F(HelixGeom_Tools_Test, ApprHelix)
{
  occ::handle<Geom_BSplineCurve> aBSpline;
  double             aMaxError;

  int aResult = HelixGeom_Tools::ApprHelix(0.0,           // T1
                                                        2.0 * M_PI,    // T2
                                                        10.0,          // Pitch
                                                        5.0,           // Start radius
                                                        0.0,           // Taper angle
                                                        true, // Clockwise
                                                        myTolerance,   // Tolerance
                                                        aBSpline,      // Result
                                                        aMaxError      // Max error
  );

  EXPECT_EQ(aResult, 0); // Success
  EXPECT_FALSE(aBSpline.IsNull());
  EXPECT_LE(aMaxError, myTolerance);

  // Test curve properties
  EXPECT_GT(aBSpline->Degree(), 0);
  EXPECT_GT(aBSpline->NbPoles(), 0);
}

TEST_F(HelixGeom_Tools_Test, ApprCurve3D)
{
  // Create a helix curve adaptor
  HelixGeom_HelixCurve aHelix;
  aHelix.Load(0.0, 2.0 * M_PI, 10.0, 3.0, 0.0, true);
  occ::handle<HelixGeom_HelixCurve> aHAdaptor = new HelixGeom_HelixCurve(aHelix);

  occ::handle<Geom_BSplineCurve> aBSpline;
  double             aMaxError;

  int aResult = HelixGeom_Tools::ApprCurve3D(aHAdaptor,
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
  int aNbSamples = 10;
  for (int i = 0; i <= aNbSamples; i++)
  {
    double aParam =
      aHelix.FirstParameter() + i * (aHelix.LastParameter() - aHelix.FirstParameter()) / aNbSamples;

    gp_Pnt aOrigPnt = aHelix.Value(aParam);

    // Map parameter to B-spline parameter space
    double aBSplineParam =
      aBSpline->FirstParameter()
      + i * (aBSpline->LastParameter() - aBSpline->FirstParameter()) / aNbSamples;

    gp_Pnt aApproxPnt;
    aBSpline->D0(aBSplineParam, aApproxPnt);

    double aDistance = aOrigPnt.Distance(aApproxPnt);
    EXPECT_LE(aDistance, aMaxError * 2); // Allow some margin
  }
}

TEST_F(HelixGeom_Tools_Test, DifferentContinuity)
{
  HelixGeom_HelixCurve aHelix;
  aHelix.Load(0.0, 6.0 * M_PI, 15.0, 4.0, 0.05, true);
  occ::handle<HelixGeom_HelixCurve> aHAdaptor = new HelixGeom_HelixCurve(aHelix);

  // Test C0 continuity
  occ::handle<Geom_BSplineCurve> aBSplineC0;
  double             aMaxErrorC0;
  int          aResultC0 = HelixGeom_Tools::ApprCurve3D(aHAdaptor,
                                                            myTolerance,
                                                            GeomAbs_C0,
                                                            30,
                                                            4,
                                                            aBSplineC0,
                                                            aMaxErrorC0);

  EXPECT_EQ(aResultC0, 0);
  EXPECT_FALSE(aBSplineC0.IsNull());

  // Test C2 continuity
  occ::handle<Geom_BSplineCurve> aBSplineC2;
  double             aMaxErrorC2;
  int          aResultC2 = HelixGeom_Tools::ApprCurve3D(aHAdaptor,
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