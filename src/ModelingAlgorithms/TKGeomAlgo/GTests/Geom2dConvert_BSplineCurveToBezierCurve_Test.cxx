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

#include <GCPnts_AbscissaPoint.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dAPI_Interpolate.hxx>
#include <Geom2dConvert_BSplineCurveToBezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <NCollection_HArray1.hxx>
#include <gp_Pnt2d.hxx>

#include <gtest/gtest.h>

// OCC7372: Invalid conversion of 2D periodic BSpline curve to Bezier segments after IncreaseDegree.
// Tests that a periodic BSpline curve (5 points, degree increased to 8) converts to exactly
// 5 Bezier arcs, and the 5th arc has the expected length ~73.3203.
TEST(Geom2dConvert_BSplineCurveToBezierCurveTest,
     OCC7372_PeriodicBSplineToBeziersAfterIncreaseDegree)
{
  occ::handle<NCollection_HArray1<gp_Pnt2d>> aPoints = new NCollection_HArray1<gp_Pnt2d>(1, 5);
  aPoints->SetValue(1, gp_Pnt2d(100.0, 0.0));
  aPoints->SetValue(2, gp_Pnt2d(100.0, 100.0));
  aPoints->SetValue(3, gp_Pnt2d(0.0, 100.0));
  aPoints->SetValue(4, gp_Pnt2d(0.0, 0.0));
  aPoints->SetValue(5, gp_Pnt2d(50.0, -50.0));

  Geom2dAPI_Interpolate anInterp(aPoints, true, 1e-6);
  anInterp.Perform();
  ASSERT_TRUE(anInterp.IsDone());

  occ::handle<Geom2d_BSplineCurve> aBSpline = anInterp.Curve();
  ASSERT_FALSE(aBSpline.IsNull());

  aBSpline->IncreaseDegree(8);

  Geom2dConvert_BSplineCurveToBezierCurve aConverter(aBSpline);
  const int                               aNbArcs = aConverter.NbArcs();
  EXPECT_EQ(aNbArcs, 5);

  occ::handle<Geom2d_BezierCurve> anArc5 = aConverter.Arc(5);
  ASSERT_FALSE(anArc5.IsNull());

  Geom2dAdaptor_Curve anAdaptor(anArc5);
  const double        aLen = GCPnts_AbscissaPoint::Length(anAdaptor);
  EXPECT_NEAR(aLen, 73.3203, 0.01);
}
