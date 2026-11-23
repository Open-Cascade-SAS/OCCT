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

#include <Geom_Circle.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp_Ax2.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <gtest/gtest.h>

#include <array>
#include <utility>

TEST(Geom_TrimmedCurveTest, CopyPreservesAuthoredPeriodicRangesAndOwnership)
{
  const occ::handle<Geom_Circle> aCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(1, 2, 3), gp_Dir(0, .3, 1)), 5);
  const std::array<std::pair<double, double>, 4> aRanges{std::pair<double, double>{-1.2, -.2},
                                                         {7, 11},
                                                         {-8, 8},
                                                         {7, 7 + 2 * M_PI}};
  for (const auto& aRange : aRanges)
  {
    const occ::handle<Geom_TrimmedCurve> aCurve =
      new Geom_TrimmedCurve(aCircle, aRange.first, aRange.second, true, false);
    const occ::handle<Geom_TrimmedCurve> aCopy = occ::down_cast<Geom_TrimmedCurve>(aCurve->Copy());
    ASSERT_FALSE(aCopy.IsNull());
    EXPECT_DOUBLE_EQ(aCopy->FirstParameter(), aRange.first);
    EXPECT_DOUBLE_EQ(aCopy->LastParameter(), aRange.second);
    EXPECT_NE(aCopy->BasisCurve(), aCurve->BasisCurve());
    for (const double aParameter :
         {aRange.first, (aRange.first + aRange.second) / 2, aRange.second})
    {
      EXPECT_NEAR(aCopy->EvalD0(aParameter).Distance(aCurve->EvalD0(aParameter)), 0, 1e-12);
    }
    const gp_Pnt aStart = aCurve->StartPoint();
    aCopy->Translate(gp_Vec(3, 4, 5));
    EXPECT_NEAR(aCurve->StartPoint().Distance(aStart), 0, 1e-12);
  }
}

//==================================================================================================

TEST(Geom_TrimmedCurveTest, TransformedPreservesAuthoredPeriodicChart)
{
  const occ::handle<Geom_TrimmedCurve> aCurve =
    new Geom_TrimmedCurve(new Geom_Circle(gp_Ax2(), 5), 7, 11, true, false);
  gp_Trsf aScale;
  aScale.SetScale(gp_Pnt(1, 2, 3), 2.5);
  gp_Trsf aMirror;
  aMirror.SetMirror(gp_Ax2(gp_Pnt(), gp_Dir(1, 0, 0)));
  for (const gp_Trsf& aTransform : {gp_Trsf(), aScale, aMirror})
  {
    const occ::handle<Geom_Curve> aCopy =
      occ::down_cast<Geom_Curve>(aCurve->Transformed(aTransform));
    ASSERT_FALSE(aCopy.IsNull());
    EXPECT_DOUBLE_EQ(aCopy->FirstParameter(), aCurve->TransformedParameter(7, aTransform));
    EXPECT_DOUBLE_EQ(aCopy->LastParameter(), aCurve->TransformedParameter(11, aTransform));
    for (const double aParameter : {7.0, 9.0, 11.0})
    {
      const gp_Pnt aPoint = aCurve->EvalD0(aParameter).Transformed(aTransform);
      EXPECT_NEAR(
        aCopy->EvalD0(aCurve->TransformedParameter(aParameter, aTransform)).Distance(aPoint),
        0,
        1e-12);
    }
  }
  EXPECT_DOUBLE_EQ(aCurve->FirstParameter(), 7);
  EXPECT_DOUBLE_EQ(aCurve->LastParameter(), 11);
}

//==================================================================================================

TEST(Geom_TrimmedCurveTest, ReversedPreservesAuthoredPeriodicChart)
{
  const occ::handle<Geom_TrimmedCurve> aCurve =
    new Geom_TrimmedCurve(new Geom_Circle(gp_Ax2(), 5), 7, 11, true, false);
  const occ::handle<Geom_Curve> aReversed = aCurve->Reversed();
  ASSERT_FALSE(aReversed.IsNull());
  EXPECT_DOUBLE_EQ(aReversed->FirstParameter(), aCurve->ReversedParameter(11));
  EXPECT_DOUBLE_EQ(aReversed->LastParameter(), aCurve->ReversedParameter(7));
  EXPECT_NEAR(aReversed->EvalD0(aReversed->FirstParameter()).Distance(aCurve->EndPoint()),
              0,
              1e-12);
  EXPECT_NEAR(aReversed->EvalD0(aReversed->LastParameter()).Distance(aCurve->StartPoint()),
              0,
              1e-12);
}
