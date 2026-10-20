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

#include <Adaptor3d_TopolTool.hxx>
#include <Contap_Contour.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Plane.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <gp.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>

namespace
{
// A closed band through five circles of different radii: 49 points around
// (the last one repeats the first), 5 rows up. Interpolated, it has far more
// knots in U than in V, so the interior sample grid is strongly non-square.
occ::handle<Geom_BSplineSurface> makeVaseBand()
{
  const double aRadii[5]   = {30.0, 40.0, 47.0, 28.0, 25.0};
  const double aHeights[5] = {0.0, 45.0, 99.0, 144.0, 180.0};
  const int    aNbU        = 49;

  NCollection_Array2<gp_Pnt> aGrid(1, aNbU, 1, 5);
  for (int j = 1; j <= 5; ++j)
  {
    for (int i = 1; i <= aNbU; ++i)
    {
      const double aT = 2.0 * M_PI * (i - 1) / (aNbU - 1);
      aGrid.SetValue(
        i,
        j,
        gp_Pnt(aRadii[j - 1] * std::cos(aT), aRadii[j - 1] * std::sin(aT), aHeights[j - 1]));
    }
  }
  GeomAPI_PointsToBSplineSurface anInterp;
  anInterp.Interpolate(aGrid);
  return anInterp.Surface();
}
} // namespace

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

// The interior start points of Contap_Contour must cover the whole surface.
// SamplePoint used to swap its U and V grid indices, which is harmless only
// when both directions have the same number of samples: here the points fell
// into a narrow strip near U = UMin, most of them outside the V range. The
// V step was divided by nbIntV + 2 instead of nbIntV + 1, which left the rows
// one step in from VMin but two steps short of VMax.
TEST(Contap_HContToolTest, BSplineSamplePointsCoverTheSurface)
{
  const occ::handle<Geom_BSplineSurface> aBSpline = makeVaseBand();
  ASSERT_FALSE(aBSpline.IsNull());
  const occ::handle<Adaptor3d_Surface> aSurface = new GeomAdaptor_Surface(aBSpline);

  const double aUMin = aSurface->FirstUParameter();
  const double aUMax = aSurface->LastUParameter();
  const double aVMin = aSurface->FirstVParameter();
  const double aVMax = aSurface->LastVParameter();

  const int aNbPoints = Contap_HContTool::NbSamplePoints(aSurface);
  ASSERT_GT(aNbPoints, 5);

  double aUFirst = aUMax, aULast = aUMin;
  double aVFirst = aVMax, aVLast = aVMin;
  for (int anIndex = 1; anIndex <= aNbPoints; ++anIndex)
  {
    double aU = 0.0, aV = 0.0;
    Contap_HContTool::SamplePoint(aSurface, anIndex, aU, aV);
    EXPECT_GT(aU, aUMin) << "sample " << anIndex;
    EXPECT_LT(aU, aUMax) << "sample " << anIndex;
    EXPECT_GT(aV, aVMin) << "sample " << anIndex;
    EXPECT_LT(aV, aVMax) << "sample " << anIndex;
    aUFirst = std::min(aUFirst, aU);
    aULast  = std::max(aULast, aU);
    aVFirst = std::min(aVFirst, aV);
    aVLast  = std::max(aVLast, aV);
  }
  EXPECT_GT(aULast - aUFirst, 0.9 * (aUMax - aUMin));
  // The grid is symmetric: the first and last rows (columns) are at the same
  // distance from the two bounds.
  EXPECT_NEAR(aUFirst - aUMin, aUMax - aULast, 1.0e-9 * (aUMax - aUMin));
  EXPECT_NEAR(aVFirst - aVMin, aVMax - aVLast, 1.0e-9 * (aVMax - aVMin));
}

// With start points only near the seam, the contour of the band seen along Y
// (a silhouette running bottom to top on the side opposite the seam) was not
// found at all: Contap_Contour returned the boundary restrictions only, and
// HLR drew the band without its outline.
TEST(Contap_HContToolTest, ContourOfNonSquareBSplineIsFound)
{
  const occ::handle<Adaptor3d_Surface>   aSurface = new GeomAdaptor_Surface(makeVaseBand());
  const occ::handle<Adaptor3d_TopolTool> aDomain  = new Adaptor3d_TopolTool(aSurface);

  Contap_Contour aContour;
  aContour.Init(gp_Vec(0.0, 1.0, 0.0));
  aContour.Perform(aSurface, aDomain);
  ASSERT_TRUE(aContour.IsDone());

  int aNbWalking = 0;
  for (int i = 1; i <= aContour.NbLines(); ++i)
  {
    if (aContour.Line(i).TypeContour() == Contap_Walking)
    {
      ++aNbWalking;
    }
  }
  EXPECT_GE(aNbWalking, 1);
}
