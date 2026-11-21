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

#include <GeomAdaptor.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

#include <array>
#include <cmath>
#include <utility>

//==================================================================================================

TEST(GeomAdaptorTest, MakeCurve_PeriodicIntervals_PreserveParameterChart)
{
  const gp_Ax2                                   anAxis(gp_Pnt(2, -3, 1), gp_Dir(1, 1, 1));
  const std::array<occ::handle<Geom_Curve>, 2>   aCurves{new Geom_Circle(anAxis, 4),
                                                       new Geom_Ellipse(anAxis, 5, 2)};
  const double                                   aPeriod = 2 * std::acos(-1.0);
  const std::array<std::pair<double, double>, 6> aRanges{std::pair{-8.0, -3.0},
                                                         {5.0, 8.0},
                                                         {7.0, 11.0},
                                                         {0.0, aPeriod},
                                                         {7.0, 7.0 + aPeriod},
                                                         {7.0, 21.0}};
  for (const auto& aCurve : aCurves)
  {
    for (const auto& [aFirst, aLast] : aRanges)
    {
      const occ::handle<Geom_Curve> aTrim =
        new Geom_TrimmedCurve(aCurve, aFirst, aLast, true, false);
      const GeomAdaptor_Curve       anAdaptor(aTrim);
      const occ::handle<Geom_Curve> aResult = GeomAdaptor::MakeCurve(anAdaptor);
      ASSERT_FALSE(aResult.IsNull());
      EXPECT_DOUBLE_EQ(aResult->FirstParameter(), aFirst);
      EXPECT_DOUBLE_EQ(aResult->LastParameter(), aLast);
      for (const double aParameter : {aFirst, (aFirst + aLast) / 2, aLast})
      {
        EXPECT_LT(aResult->EvalD0(aParameter).Distance(aTrim->EvalD0(aParameter)), 1.e-12);
        EXPECT_LT((aResult->EvalD1(aParameter).D1 - aTrim->EvalD1(aParameter).D1).Magnitude(),
                  1.e-12);
      }
    }
  }
}

//==================================================================================================

TEST(GeomAdaptorTest, MakeCurve_BoundedLine_PreservesInterval)
{
  const occ::handle<Geom_Curve> aLine = new Geom_Line(gp_Pnt(2, -3, 1), gp_Dir(1, 2, 3));
  const GeomAdaptor_Curve       anAdaptor(aLine, -5, 7);
  const occ::handle<Geom_Curve> aResult = GeomAdaptor::MakeCurve(anAdaptor);
  ASSERT_FALSE(aResult.IsNull());
  EXPECT_DOUBLE_EQ(aResult->FirstParameter(), -5);
  EXPECT_DOUBLE_EQ(aResult->LastParameter(), 7);
  EXPECT_LT(aResult->EvalD0(-5).Distance(aLine->EvalD0(-5)), 1.e-12);
  EXPECT_LT(aResult->EvalD0(7).Distance(aLine->EvalD0(7)), 1.e-12);
}

//==================================================================================================

TEST(GeomAdaptorTest, CopyAndOffsetPreserveReversedMultiPeriodTrim)
{
  const occ::handle<Geom_Curve> aBasis =
    new Geom_Circle(gp_Ax2(gp_Pnt(2, -3, 1), gp_Dir(0, 0, 1)), 4);
  const occ::handle<Geom_TrimmedCurve> aTrim =
    new Geom_TrimmedCurve(aBasis, 18.0, 23.0, false, false);
  const occ::handle<Geom_Curve> aCopy = occ::down_cast<Geom_Curve>(aTrim->Copy());
  ASSERT_FALSE(aCopy.IsNull());
  const Geom_OffsetCurve anOffset(aTrim, 0.5, gp_Dir(0, 0, 1));
  EXPECT_DOUBLE_EQ(aCopy->FirstParameter(), aTrim->FirstParameter());
  EXPECT_DOUBLE_EQ(aCopy->LastParameter(), aTrim->LastParameter());
  EXPECT_DOUBLE_EQ(anOffset.FirstParameter(), aTrim->FirstParameter());
  EXPECT_DOUBLE_EQ(anOffset.LastParameter(), aTrim->LastParameter());
  const double aParameter = (aTrim->FirstParameter() + aTrim->LastParameter()) / 2;
  EXPECT_LE(aCopy->EvalD0(aParameter).Distance(aTrim->EvalD0(aParameter)), 1.e-12);
  EXPECT_NEAR(anOffset.EvalD0(aParameter).Distance(aTrim->EvalD0(aParameter)), 0.5, 1.e-12);
}
