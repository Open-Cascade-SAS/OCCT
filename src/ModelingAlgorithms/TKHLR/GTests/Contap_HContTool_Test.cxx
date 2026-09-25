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

// The interior start points of Contap_Contour must cover the whole surface.
// SamplePoint used to swap its U and V grid indices, which is harmless only
// when both directions have the same number of samples: here the points fell
// into a narrow strip near U = UMin, most of them outside the V range.
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
  }
  EXPECT_GT(aULast - aUFirst, 0.9 * (aUMax - aUMin));
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
