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

#include <Contap_HContTool.hxx>

#include <GeomAdaptor_Surface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_BSplineSurface.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <gp_Pnt.hxx>
#include <gp.hxx>
#include <gp_Pln.hxx>

#include <gtest/gtest.h>

TEST(Contap_HContToolTest, SurfaceSamplingHasNoCrossCallState)
{
  const occ::handle<Geom_Plane>        aPlane = new Geom_Plane(gp_Pln(gp::XOY()));
  const occ::handle<Adaptor3d_Surface> aFirstSurface =
    new GeomAdaptor_Surface(aPlane, 0.0, 4.0, 10.0, 14.0);
  const occ::handle<Adaptor3d_Surface> aSecondSurface =
    new GeomAdaptor_Surface(aPlane, -100.0, -80.0, -40.0, -20.0);

  EXPECT_EQ(Contap_HContTool::NbSamplePoints(aFirstSurface), 5);
  EXPECT_EQ(Contap_HContTool::NbSamplePoints(aSecondSurface), 5);

  double aU = 0.0;
  double aV = 0.0;
  Contap_HContTool::SamplePoint(aFirstSurface, 1, aU, aV);
  EXPECT_DOUBLE_EQ(aU, 1.0);
  EXPECT_DOUBLE_EQ(aV, 11.0);
}

TEST(Contap_HContToolTest, UnequalSplineDegreesKeepSamplesInsideDomain)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 4, 1, 10);
  for (int i = 1; i <= 4; ++i)
  {
    for (int j = 1; j <= 10; ++j)
    {
      aPoles(i, j) = gp_Pnt((i - 1) / 3.0, (j - 1) / 9.0, 0.0);
    }
  }
  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;
  NCollection_Array1<int> aUMultiplicities(1, 2), aVMultiplicities(1, 2);
  aUMultiplicities.Init(4);
  aVMultiplicities.Init(10);
  for (bool isExchanged : {false, true})
  {
    const occ::handle<Geom_BSplineSurface> aSurface =
      new Geom_BSplineSurface(aPoles, aKnots, aKnots, aUMultiplicities, aVMultiplicities, 3, 9);
    if (isExchanged)
    {
      aSurface->ExchangeUV();
    }
    const occ::handle<Adaptor3d_Surface> anAdaptor = new GeomAdaptor_Surface(aSurface);
    const int                            aCount    = Contap_HContTool::NbSamplePoints(anAdaptor);
    ASSERT_GT(aCount, 5);
    for (int i = 1; i <= aCount; ++i)
    {
      SCOPED_TRACE(::testing::Message() << isExchanged << ' ' << i);
      double aU, aV;
      Contap_HContTool::SamplePoint(anAdaptor, i, aU, aV);
      EXPECT_GT(aU, anAdaptor->FirstUParameter());
      EXPECT_LT(aU, anAdaptor->LastUParameter());
      EXPECT_GT(aV, anAdaptor->FirstVParameter());
      EXPECT_LT(aV, anAdaptor->LastVParameter());
    }
  }
}
