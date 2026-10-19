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

#include <GeomTools.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>

#include <gtest/gtest.h>

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
  aStream.precision(17);
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

//==================================================================================================

TEST(GeomToolsTest, CurveRoundTripPreservesTrimmedPeriodicChart)
{
  const occ::handle<Geom_Curve> aBasis = new Geom_Circle(gp_Ax2(gp_Pnt(3, 0, 0), gp_Dir(0, 0, 1)), 2);
  const occ::handle<Geom_Curve> aCurve = new Geom_TrimmedCurve(aBasis, 18, 23, false, false);
  std::stringstream aStream;
  aStream.precision(17);
  GeomTools::Write(aCurve, aStream);
  occ::handle<Geom_Curve> aResult;
  GeomTools::Read(aResult, aStream);
  ASSERT_FALSE(aResult.IsNull());
  EXPECT_EQ(aResult->DynamicType(), aCurve->DynamicType());
  EXPECT_NEAR(aResult->FirstParameter(), aCurve->FirstParameter(), 1.e-14);
  EXPECT_NEAR(aResult->LastParameter(), aCurve->LastParameter(), 1.e-14);
  const double aParameter = (aCurve->FirstParameter() + aCurve->LastParameter()) / 2;
  EXPECT_LE(aResult->EvalD0(aParameter).Distance(aCurve->EvalD0(aParameter)), 1.e-12);
}
