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
  explicit ApproxTest_EndLayer(double theWidth);
  double FirstParameter() const override;
  double LastParameter() const override;
  bool   Value(double                        theU,
               NCollection_Array1<gp_Pnt2d>& thePoints2d,
               NCollection_Array1<gp_Pnt>&   thePoints) const override;
  bool   D1(double                        theU,
            NCollection_Array1<gp_Vec2d>& theVectors2d,
            NCollection_Array1<gp_Vec>&   theVectors) const override;

private:
  double myWidth; //!< Width of the exponential end layer in parameter units.
};

//===================================================================================================

ApproxTest_EndLayer::ApproxTest_EndLayer(const double theWidth)
    : myWidth(theWidth)
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
  thePoints.ChangeAt(0) = gp_Pnt(theU, std::exp((theU - 1.0) / myWidth), 0.0);
  return true;
}

//===================================================================================================

bool ApproxTest_EndLayer::D1(double theU,
                             NCollection_Array1<gp_Vec2d>&,
                             NCollection_Array1<gp_Vec>& theVectors) const
{
  theVectors.ChangeAt(0) = gp_Vec(1.0, std::exp((theU - 1.0) / myWidth) / myWidth, 0.0);
  return true;
}

//====================================================================================================

constexpr double THE_TOLERANCE = 1.0e-5;

//====================================================================================================

void checkEndLayer(Approx_FitAndDivide& theFit, const double theWidth)
{
  const ApproxTest_EndLayer aFunction(theWidth);
  theFit.SetMaxSegments(100);
  theFit.Perform(aFunction);
  ASSERT_TRUE(theFit.IsAllApproximated());
  ASSERT_GT(theFit.NbMultiCurves(), 0);
  for (int aSegmentIndex = 1; aSegmentIndex <= theFit.NbMultiCurves(); ++aSegmentIndex)
  {
    SCOPED_TRACE(aSegmentIndex);
    double aFirst, aLast, anError3d, anError2d;
    theFit.Parameters(aSegmentIndex, aFirst, aLast);
    theFit.Error(aSegmentIndex, anError3d, anError2d);
    EXPECT_LE(anError3d, THE_TOLERANCE);
    const AppParCurves_MultiCurve aSegment = theFit.Value(aSegmentIndex);
    NCollection_Array1<gp_Pnt>    aPoles(1, aSegment.Degree() + 1);
    aSegment.Curve(1, aPoles);
    const Geom_BezierCurve aCurve(aPoles);
    for (int aSampleIndex = 0; aSampleIndex <= 100; ++aSampleIndex)
    {
      const double aFraction  = aSampleIndex / 100.0;
      const double aParameter = aFirst + (aLast - aFirst) * aFraction;
      const gp_Pnt anExpected(aParameter, std::exp((aParameter - 1.0) / theWidth), 0.0);
      EXPECT_LE(aCurve.EvalD0(aFraction).Distance(anExpected), THE_TOLERANCE);
    }
  }
}

} // namespace

TEST(ApproxFitAndDivideTest, DisabledHangCheckingResolvesNarrowEndLayer)
{
  Approx_FitAndDivide aFit(8, 8, THE_TOLERANCE, THE_TOLERANCE, true);
  aFit.SetHangChecking(false);
  checkEndLayer(aFit, 1.0e-7);
}

TEST(ApproxFitAndDivideTest, DefaultHangCheckingApproximatesSmoothEndLayer)
{
  Approx_FitAndDivide aFit(8, 8, THE_TOLERANCE, THE_TOLERANCE, true);
  checkEndLayer(aFit, 0.02);
}
