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

#include <GeomAPI.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_Curve.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Pln.hxx>

#include <gtest/gtest.h>

TEST(GeomAPITest, PlaneConversionPreservesPeriodicTrimChart)
{
  const occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(gp_Ax2d(), 1.0);
  for (const double aLast : {11.0, 21.0})
  {
    const occ::handle<Geom2d_TrimmedCurve> aTrim =
      new Geom2d_TrimmedCurve(aCircle, 7.0, aLast, true, false);
    const gp_Pln                  aPlane;
    const occ::handle<Geom_Curve> aCurve = GeomAPI::To3d(aTrim, aPlane);
    ASSERT_FALSE(aCurve.IsNull());
    EXPECT_EQ(aCurve->FirstParameter(), 7.0);
    EXPECT_EQ(aCurve->LastParameter(), aLast);
    const occ::handle<Geom2d_Curve> aRoundTrip = GeomAPI::To2d(aCurve, aPlane);
    ASSERT_FALSE(aRoundTrip.IsNull());
    EXPECT_EQ(aRoundTrip->FirstParameter(), 7.0);
    EXPECT_EQ(aRoundTrip->LastParameter(), aLast);
  }
}
