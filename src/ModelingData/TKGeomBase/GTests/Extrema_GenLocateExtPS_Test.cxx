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

#include <gtest/gtest.h>

#include <Extrema_GenLocateExtPS.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <GeomConvert.hxx>
#include <Precision.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>

TEST(Extrema_GenLocateExtPSTest, ClosedSplineNormalizesStartingParameter)
{
  for (bool isExchanged : {false, true})
  {
    const occ::handle<Geom_BSplineSurface> aSurface = GeomConvert::SurfaceToBSplineSurface(
      new Geom_RectangularTrimmedSurface(new Geom_CylindricalSurface(gp_Ax3(), 2.0),
                                         0.0,
                                         2.0 * M_PI,
                                        -1.0,
                                        1.0));
    aSurface->SetUNotPeriodic();
    if (isExchanged)
    {
      aSurface->ExchangeUV();
    }
    const GeomAdaptor_Surface anAdaptor(aSurface);
    ASSERT_TRUE(isExchanged ? anAdaptor.IsVClosed() : anAdaptor.IsUClosed());
    ASSERT_FALSE(isExchanged ? anAdaptor.IsVPeriodic() : anAdaptor.IsUPeriodic());
    for (double aParameter : {0.2, 2.0 * M_PI - 0.2})
    {
      for (int aShift : {-2, -1, 0, 1, 2})
      {
        SCOPED_TRACE(::testing::Message() << isExchanged << ' ' << aParameter << ' ' << aShift);
        const double           aStart = aParameter + aShift * 2.0 * M_PI;
        const double           aU     = isExchanged ? 0.3 : aParameter;
        const double           aV     = isExchanged ? aParameter : 0.3;
        const gp_Pnt           aPoint = aSurface->EvalD0(aU, aV);
        Extrema_GenLocateExtPS aProjection(anAdaptor,
                                           Precision::PConfusion(),
                                           Precision::PConfusion());
        aProjection.Perform(aPoint, isExchanged ? aU : aStart, isExchanged ? aStart : aV);
        ASSERT_TRUE(aProjection.IsDone());
        EXPECT_LE(aProjection.SquareDistance(), Precision::SquareConfusion());
        double aResultU, aResultV;
        aProjection.Point().Parameter(aResultU, aResultV);
        EXPECT_GE(aResultU, anAdaptor.FirstUParameter());
        EXPECT_LE(aResultU, anAdaptor.LastUParameter());
        EXPECT_GE(aResultV, anAdaptor.FirstVParameter());
        EXPECT_LE(aResultV, anAdaptor.LastVParameter());
      }
    }
  }
}
