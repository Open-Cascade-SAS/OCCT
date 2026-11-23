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

#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_Curve.hxx>
#include <GeomTools.hxx>
#include <Precision.hxx>
#include <TCollection_AsciiString.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>

#include <gtest/gtest.h>

#include <iomanip>
#include <limits>
#include <sstream>

//==================================================================================================

TEST(GeomToolsTest, Curve2dRoundTripPreservesTrimmedPeriodicChart)
{
  const occ::handle<Geom2d_Curve> aCurve =
    new Geom2d_TrimmedCurve(new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(0, 0), gp_Dir2d(1, 0)), 2),
                            7,
                            11,
                            true,
                            false);
  std::stringstream aStream;
  GeomTools::Write(aCurve, aStream);
  occ::handle<Geom2d_Curve> aResult;
  GeomTools::Read(aResult, aStream);
  ASSERT_FALSE(aResult.IsNull());
  EXPECT_DOUBLE_EQ(aResult->FirstParameter(), 7);
  EXPECT_DOUBLE_EQ(aResult->LastParameter(), 11);
  for (const double aParameter : {7.0, 9.0, 11.0})
  {
    EXPECT_LE(aResult->EvalD0(aParameter).Distance(aCurve->EvalD0(aParameter)), 1.e-12);
  }
}

TEST(GeomToolsTest, LongRealTokenPreservesValueAndFollowingField)
{
  const double aValues[] = {Precision::Infinite(),
                            -Precision::Infinite(),
                            std::numeric_limits<double>::max(),
                            -std::numeric_limits<double>::max(),
                            1.25,
                            -1.25,
                            0.0};
  for (const double aValue : aValues)
  {
    std::stringstream aStream;
    aStream << std::fixed << std::setprecision(40) << aValue << " 7";
    double aResult = 0.0;
    GeomTools::GetReal(aStream, aResult);
    EXPECT_DOUBLE_EQ(aResult, aValue);
    GeomTools::GetReal(aStream, aResult);
    EXPECT_DOUBLE_EQ(aResult, 7.0);
  }
}

TEST(GeomToolsTest, RealTokenBufferBoundaryAndEndOfStream)
{
  for (int aLength = 28; aLength <= 34; ++aLength)
  {
    const TCollection_AsciiString aToken = TCollection_AsciiString(aLength - 1, '0') + "1";
    std::istringstream            aStream(aToken.ToCString());
    double                        aResult = 0.0;
    GeomTools::GetReal(aStream, aResult);
    EXPECT_DOUBLE_EQ(aResult, 1.0);
    EXPECT_FALSE(aStream.fail());
    GeomTools::GetReal(aStream, aResult);
    EXPECT_DOUBLE_EQ(aResult, 0.0);
  }
  std::istringstream aStream(
    "1.00000000000000000000000000000000000000000000000000000000001e-100 9");
  double aResult = 0.0;
  GeomTools::GetReal(aStream, aResult);
  EXPECT_DOUBLE_EQ(aResult, 1.e-100);
  GeomTools::GetReal(aStream, aResult);
  EXPECT_DOUBLE_EQ(aResult, 9.0);
}

TEST(GeomToolsTest, LongLeadingZerosPreserveSignAndStreamWidth)
{
  const TCollection_AsciiString aZeros(480, '0');
  const TCollection_AsciiString aToken = TCollection_AsciiString("-") + aZeros + "1.25e-20 8";
  std::istringstream            aStream(aToken.ToCString());
  aStream.width(7);
  double aValue = 0.0;
  GeomTools::GetReal(aStream, aValue);
  EXPECT_DOUBLE_EQ(aValue, -1.25e-20);
  EXPECT_EQ(aStream.width(), 7);
  GeomTools::GetReal(aStream, aValue);
  EXPECT_DOUBLE_EQ(aValue, 8.0);
}

TEST(GeomToolsTest, RealTokenLimitRejectsOversizedInput)
{
  for (const int aLength : {511, 512, 2048})
  {
    const TCollection_AsciiString aToken = TCollection_AsciiString(aLength - 1, '0') + "1";
    std::istringstream            aStream(aToken.ToCString());
    double                        aValue = 0.0;
    if (aLength == 511)
    {
      GeomTools::GetReal(aStream, aValue);
      EXPECT_DOUBLE_EQ(aValue, 1.0);
    }
    else
    {
      EXPECT_NO_THROW(GeomTools::GetReal(aStream, aValue));
      EXPECT_TRUE(aStream.fail());
      EXPECT_DOUBLE_EQ(aValue, 0.0);
    }
  }
}

TEST(GeomToolsTest, OversizedCurveCoordinateStopsGeometryRead)
{
  const TCollection_AsciiString aToken  = TCollection_AsciiString(511, '0') + "1";
  const TCollection_AsciiString aRecord = TCollection_AsciiString("1 ") + aToken + " 0 0 1 0 0";
  std::istringstream            aStream(aRecord.ToCString());
  occ::handle<Geom_Curve>       aCurve;
  EXPECT_NO_THROW(GeomTools::Read(aCurve, aStream));
  EXPECT_TRUE(aStream.fail());
  EXPECT_TRUE(aCurve.IsNull());
}
