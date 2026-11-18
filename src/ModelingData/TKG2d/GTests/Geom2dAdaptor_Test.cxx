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

#include <Geom2dAdaptor.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>

#include <gtest/gtest.h>

#include <array>
#include <cmath>
#include <utility>

//==================================================================================================

TEST(Geom2dAdaptorTest, MakeCurve_PeriodicIntervals_PreserveParameterChart)
{
  const gp_Ax2d                                  anAxis(gp_Pnt2d(2, -3), gp_Dir2d(1, 2));
  const std::array<occ::handle<Geom2d_Curve>, 2> aCurves{new Geom2d_Circle(anAxis, 4, false),
                                                         new Geom2d_Ellipse(anAxis, 5, 2, false)};
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
      const occ::handle<Geom2d_Curve> aTrim =
        new Geom2d_TrimmedCurve(aCurve, aFirst, aLast, true, false);
      const Geom2dAdaptor_Curve       anAdaptor(aTrim);
      const occ::handle<Geom2d_Curve> aResult = Geom2dAdaptor::MakeCurve(anAdaptor);
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

TEST(Geom2dAdaptorTest, MakeCurve_BoundedLine_PreservesInterval)
{
  const occ::handle<Geom2d_Curve> aLine = new Geom2d_Line(gp_Pnt2d(2, -3), gp_Dir2d(1, 2));
  const Geom2dAdaptor_Curve       anAdaptor(aLine, -5, 7);
  const occ::handle<Geom2d_Curve> aResult = Geom2dAdaptor::MakeCurve(anAdaptor);
  ASSERT_FALSE(aResult.IsNull());
  EXPECT_DOUBLE_EQ(aResult->FirstParameter(), -5);
  EXPECT_DOUBLE_EQ(aResult->LastParameter(), 7);
  EXPECT_LT(aResult->EvalD0(-5).Distance(aLine->EvalD0(-5)), 1.e-12);
  EXPECT_LT(aResult->EvalD0(7).Distance(aLine->EvalD0(7)), 1.e-12);
}
