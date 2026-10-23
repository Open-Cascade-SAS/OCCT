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

#include <AppDef_BSpParLeastSquareOfMyBSplGradientOfBSplineCompute.hxx>
#include <AppDef_ParLeastSquareOfMyGradientOfCompute.hxx>
#include <AppDef_ParLeastSquareOfMyGradientbisOfBSplineCompute.hxx>
#include <AppDef_ParLeastSquareOfTheGradient.hxx>
#include <AppDef_TheLeastSquares.hxx>
#include <AppDef_MultiLine.hxx>
#include <AppDef_MultiPointConstraint.hxx>
#include <AppParCurves_Constraint.hxx>
#include <AppParCurves_MultiCurve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <math_Vector.hxx>

#include <gtest/gtest.h>

#include <array>
#include <utility>

namespace
{
//==================================================================================================

template <typename Fit>
void checkEndpointConstraints()
{
  AppDef_MultiLine aLine(21);
  math_Vector      aParameters(1, 21);
  for (int anIndex = 1; anIndex <= 21; ++anIndex)
  {
    const double aU = (anIndex - 1) / 20.0;
    // SetValue shares the point arrays, so each row needs an independent object.
    AppDef_MultiPointConstraint aPoint(1, 1);
    aPoint.SetPoint(1, gp_Pnt(10 + aU, 20 + aU * aU, 30 + aU * aU * aU));
    aPoint.SetTang(1, gp_Vec(1, 2 * aU, 3 * aU * aU));
    aPoint.SetCurv(1, gp_Vec(0, 2, 6 * aU));
    aPoint.SetPoint2d(2, gp_Pnt2d(40 + aU, 50 + aU * aU));
    aPoint.SetTang2d(2, gp_Vec2d(1, 2 * aU));
    aPoint.SetCurv2d(2, gp_Vec2d(0, 2));
    aLine.SetValue(anIndex, aPoint);
    aParameters(anIndex) = aU;
  }

  const std::array<std::pair<AppParCurves_Constraint, AppParCurves_Constraint>, 5> aConstraints = {
    {{AppParCurves_PassPoint, AppParCurves_CurvaturePoint},
     {AppParCurves_CurvaturePoint, AppParCurves_PassPoint},
     {AppParCurves_NoConstraint, AppParCurves_CurvaturePoint},
     {AppParCurves_CurvaturePoint, AppParCurves_NoConstraint},
     {AppParCurves_CurvaturePoint, AppParCurves_CurvaturePoint}}};
  for (const auto& [aFirst, aLast] : aConstraints)
  {
    SCOPED_TRACE(testing::Message() << aFirst << "," << aLast);
    Fit aFit(aLine, 1, 21, aFirst, aLast, 8);
    // A degree-seven Bezier uses lambda = 1/7 for the supplied derivatives.
    aFit.Perform(aParameters, 1.0 / 7.0, 1.0 / 7.0);
    ASSERT_TRUE(aFit.IsDone());
    const AppParCurves_MultiCurve aCurve = aFit.BezierValue();
    for (const double aU : std::array<double, 2>{{0, 1}})
    {
      gp_Pnt aPoint;
      gp_Vec aD1, aD2;
      aCurve.D2(1, aU, aPoint, aD1, aD2);
      gp_Pnt2d aPoint2d;
      gp_Vec2d aD12d, aD22d;
      aCurve.D2(2, aU, aPoint2d, aD12d, aD22d);
      // The input is a cubic polynomial, so all endpoint values have an exact fit.
      EXPECT_NEAR(aPoint.Distance(gp_Pnt(10 + aU, 20 + aU * aU, 30 + aU * aU * aU)), 0, 1.e-9);
      EXPECT_NEAR(aPoint2d.Distance(gp_Pnt2d(40 + aU, 50 + aU * aU)), 0, 1.e-9);
      if ((aU == 0 ? aFirst : aLast) == AppParCurves_CurvaturePoint)
      {
        EXPECT_NEAR((aD1 - gp_Vec(1, 2 * aU, 3 * aU * aU)).Magnitude(), 0, 1.e-9);
        EXPECT_NEAR((aD2 - gp_Vec(0, 2, 6 * aU)).Magnitude(), 0, 1.e-8);
        EXPECT_NEAR((aD12d - gp_Vec2d(1, 2 * aU)).Magnitude(), 0, 1.e-9);
        EXPECT_NEAR((aD22d - gp_Vec2d(0, 2)).Magnitude(), 0, 1.e-8);
      }
    }
  }
}
} // namespace

//==================================================================================================

TEST(AppDef_LeastSquareTest, EndpointCurvature_TheLeastSquares)
{
  checkEndpointConstraints<AppDef_TheLeastSquares>();
}

//==================================================================================================

TEST(AppDef_LeastSquareTest, EndpointCurvature_BSpParLeastSquare)
{
  checkEndpointConstraints<AppDef_BSpParLeastSquareOfMyBSplGradientOfBSplineCompute>();
}

//==================================================================================================

TEST(AppDef_LeastSquareTest, EndpointCurvature_MyGradientOfCompute)
{
  checkEndpointConstraints<AppDef_ParLeastSquareOfMyGradientOfCompute>();
}

//==================================================================================================

TEST(AppDef_LeastSquareTest, EndpointCurvature_MyGradientbisOfBSplineCompute)
{
  checkEndpointConstraints<AppDef_ParLeastSquareOfMyGradientbisOfBSplineCompute>();
}

//==================================================================================================

TEST(AppDef_LeastSquareTest, EndpointCurvature_TheGradient)
{
  checkEndpointConstraints<AppDef_ParLeastSquareOfTheGradient>();
}
