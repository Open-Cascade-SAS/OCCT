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

#include <BinTools_Curve2dSet.hxx>
#include <BinTools_CurveSet.hxx>
#include <BinTools_OStream.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

#include <gtest/gtest.h>

#include <array>
#include <cmath>
#include <sstream>
#include <type_traits>
#include <utility>

namespace
{
//==================================================================================================

template <class CurveType>
void checkCurveRoundTrip(const occ::handle<CurveType>& theCurve)
{
  std::stringstream aStream(std::ios::in | std::ios::out | std::ios::binary);
  BinTools_OStream  aWriter(aStream);
  if constexpr (std::is_same_v<CurveType, Geom2d_Curve>)
  {
    BinTools_Curve2dSet::WriteCurve2d(theCurve, aWriter);
  }
  else
  {
    BinTools_CurveSet::WriteCurve(theCurve, aWriter);
  }
  ASSERT_TRUE(aStream.good());
  occ::handle<CurveType> aRestored;
  if constexpr (std::is_same_v<CurveType, Geom2d_Curve>)
  {
    BinTools_Curve2dSet::ReadCurve2d(aStream, aRestored);
  }
  else
  {
    BinTools_CurveSet::ReadCurve(aStream, aRestored);
  }
  ASSERT_FALSE(aRestored.IsNull());
  ASSERT_FALSE(aStream.fail());
  EXPECT_EQ(aStream.peek(), std::char_traits<char>::eof());
  EXPECT_EQ(aRestored->DynamicType(), theCurve->DynamicType());
  EXPECT_DOUBLE_EQ(aRestored->FirstParameter(), theCurve->FirstParameter());
  EXPECT_DOUBLE_EQ(aRestored->LastParameter(), theCurve->LastParameter());
  for (const double aParameter : {theCurve->FirstParameter(),
                                  (theCurve->FirstParameter() + theCurve->LastParameter()) / 2,
                                  theCurve->LastParameter()})
  {
    EXPECT_LT(aRestored->EvalD0(aParameter).Distance(theCurve->EvalD0(aParameter)), 1.e-12);
    EXPECT_LT((aRestored->EvalD1(aParameter).D1 - theCurve->EvalD1(aParameter).D1).Magnitude(),
              1.e-12);
  }
}
} // namespace

//==================================================================================================

TEST(BinToolsCurve2dSetTest, PeriodicTrims_PreserveParametersAndGeometry)
{
  const gp_Ax2d                                  anAxis(gp_Pnt2d(2, -3), gp_Dir2d(1, 2));
  const std::array<occ::handle<Geom2d_Curve>, 2> aCurves{new Geom2d_Circle(anAxis, 4, false),
                                                         new Geom2d_Ellipse(anAxis, 5, 2, false)};
  const double                                   aPeriod = 2 * std::acos(-1.0);
  for (const auto& aCurve : aCurves)
  {
    for (const auto& [aFirst, aLast] :
         std::array<std::pair<double, double>, 6>{std::pair{-8.0, -3.0},
                                                  {5.0, 8.0},
                                                  {7.0, 11.0},
                                                  {0.0, aPeriod},
                                                  {7.0, 7.0 + aPeriod},
                                                  {7.0, 21.0}})
    {
      const occ::handle<Geom2d_Curve> aTrim =
        new Geom2d_TrimmedCurve(aCurve, aFirst, aLast, true, false);
      checkCurveRoundTrip(aTrim);
      checkCurveRoundTrip(occ::handle<Geom2d_Curve>(new Geom2d_OffsetCurve(aTrim, 0.5)));
    }
  }
}

//==================================================================================================

TEST(BinToolsCurveSetTest, PeriodicTrims_PreserveParametersAndGeometry)
{
  const gp_Ax2                                 anAxis(gp_Pnt(2, -3, 1), gp_Dir(1, 1, 1));
  const std::array<occ::handle<Geom_Curve>, 2> aCurves{new Geom_Circle(anAxis, 4),
                                                       new Geom_Ellipse(anAxis, 5, 2)};
  const double                                 aPeriod = 2 * std::acos(-1.0);
  for (const auto& aCurve : aCurves)
  {
    for (const auto& [aFirst, aLast] :
         std::array<std::pair<double, double>, 6>{std::pair{-8.0, -3.0},
                                                  {5.0, 8.0},
                                                  {7.0, 11.0},
                                                  {0.0, aPeriod},
                                                  {7.0, 7.0 + aPeriod},
                                                  {7.0, 21.0}})
    {
      checkCurveRoundTrip(
        occ::handle<Geom_Curve>(new Geom_TrimmedCurve(aCurve, aFirst, aLast, true, false)));
    }
  }
}

//==================================================================================================

TEST(BinToolsCurve2dSetTest, BoundedLine_PreservesInterval)
{
  const occ::handle<Geom2d_Curve> aLine = new Geom2d_Line(gp_Pnt2d(2, -3), gp_Dir2d(1, 2));
  checkCurveRoundTrip(occ::handle<Geom2d_Curve>(new Geom2d_TrimmedCurve(aLine, -5, 7)));
}

//==================================================================================================

TEST(BinToolsCurveSetTest, BoundedLine_PreservesInterval)
{
  const occ::handle<Geom_Curve> aLine = new Geom_Line(gp_Pnt(2, -3, 1), gp_Dir(1, 2, 3));
  checkCurveRoundTrip(occ::handle<Geom_Curve>(new Geom_TrimmedCurve(aLine, -5, 7)));
}
