// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.

#include <AppCont_Function.hxx>
#include <AppParCurves_MultiCurve.hxx>
#include <Approx_FitAndDivide.hxx>
#include <Geom_BezierCurve.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <gtest/gtest.h>
#include <cmath>

namespace
{
class ApproxTest_EndLayer : public AppCont_Function
{
public:
  ApproxTest_EndLayer();
  double FirstParameter() const override;
  double LastParameter() const override;
  bool   Value(double                        theU,
               NCollection_Array1<gp_Pnt2d>& thePoints2d,
               NCollection_Array1<gp_Pnt>&   thePoints) const override;
  bool   D1(double                        theU,
            NCollection_Array1<gp_Vec2d>& theVectors2d,
            NCollection_Array1<gp_Vec>&   theVectors) const override;
};

//===================================================================================================

ApproxTest_EndLayer::ApproxTest_EndLayer()
{
  myNbPnt   = 1;
  myNbPnt2d = 0;
}

//===================================================================================================

double ApproxTest_EndLayer::FirstParameter() const
{
  return 0.0;
}

//===================================================================================================

double ApproxTest_EndLayer::LastParameter() const
{
  return 1.0;
}

//===================================================================================================

bool ApproxTest_EndLayer::Value(double theU,
                                NCollection_Array1<gp_Pnt2d>&,
                                NCollection_Array1<gp_Pnt>& thePoints) const
{
  thePoints.ChangeAt(0) = gp_Pnt(theU, std::exp((theU - 1.0) / 1.0e-7), 0.0);
  return true;
}

//===================================================================================================

bool ApproxTest_EndLayer::D1(double theU,
                             NCollection_Array1<gp_Vec2d>&,
                             NCollection_Array1<gp_Vec>& theVectors) const
{
  theVectors.ChangeAt(0) = gp_Vec(1.0, std::exp((theU - 1.0) / 1.0e-7) / 1.0e-7, 0.0);
  return true;
}
} // namespace

TEST(ApproxFitAndDivideTest, DisabledHangCheckingResolvesNarrowEndLayer)
{
  const ApproxTest_EndLayer aFunction;
  constexpr double          aTolerance = 1.0e-5;
  Approx_FitAndDivide       aFit(8, 8, aTolerance, aTolerance, true);
  aFit.SetMaxSegments(100);
  aFit.SetHangChecking(false);
  aFit.Perform(aFunction);
  ASSERT_TRUE(aFit.IsAllApproximated());
  ASSERT_GT(aFit.NbMultiCurves(), 0);
  for (int i = 1; i <= aFit.NbMultiCurves(); ++i)
  {
    SCOPED_TRACE(i);
    double aFirst, aLast, anError3d, anError2d;
    aFit.Parameters(i, aFirst, aLast);
    aFit.Error(i, anError3d, anError2d);
    EXPECT_LE(anError3d, aTolerance);
    const AppParCurves_MultiCurve aSegment = aFit.Value(i);
    NCollection_Array1<gp_Pnt>    aPoles(1, aSegment.Degree() + 1);
    aSegment.Curve(1, aPoles);
    const Geom_BezierCurve aCurve(aPoles);
    for (int j = 0; j <= 100; ++j)
    {
      const double aFraction  = j / 100.0;
      const double aParameter = aFirst + (aLast - aFirst) * aFraction;
      const gp_Pnt anExpected(aParameter, std::exp((aParameter - 1.0) / 1.0e-7), 0.0);
      EXPECT_LE(aCurve.EvalD0(aFraction).Distance(anExpected), aTolerance);
    }
  }
}
