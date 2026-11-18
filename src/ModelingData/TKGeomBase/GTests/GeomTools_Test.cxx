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
