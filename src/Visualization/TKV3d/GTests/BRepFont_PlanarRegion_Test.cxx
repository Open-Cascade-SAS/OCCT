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

#include <BRepFont_PlanarRegion.hxx>

#include <limits>

#include <gtest/gtest.h>

TEST(BRepFont_PlanarRegionTest, UseSeparatesRangeFromOrientation)
{
  const BRepFont_PlanarRegion::Use aUse(BRepFont_PlanarRegion::CurveId(7),
                                        BRepFont_PlanarRegion::VertexId(3),
                                        BRepFont_PlanarRegion::VertexId(2),
                                        0.25,
                                        0.75,
                                        true);
  EXPECT_EQ(7u, aUse.CurveIndex().Value());
  EXPECT_EQ(3u, aUse.FirstVertex().Value());
  EXPECT_EQ(2u, aUse.LastVertex().Value());
  EXPECT_DOUBLE_EQ(0.25, aUse.FirstParameter());
  EXPECT_DOUBLE_EQ(0.75, aUse.LastParameter());
  EXPECT_TRUE(aUse.IsReversed());
}

TEST(BRepFont_PlanarRegionTest, QuadraticBoundsUseInteriorExtremum)
{
  const BRepFont_PlanarRegion::Curve aCurve(
    Font_GlyphOutline::Segment::Quadratic(gp_XY(0.0, 0.0), gp_XY(1.0, 2.0), gp_XY(2.0, 0.0)));

  double aMinX = 0.0;
  double aMinY = 0.0;
  double aMaxX = 0.0;
  double aMaxY = 0.0;
  aCurve.Bounds().Get(aMinX, aMinY, aMaxX, aMaxY);
  EXPECT_NEAR(0.0, aMinX, 1.0e-12);
  EXPECT_NEAR(0.0, aMinY, 1.0e-12);
  EXPECT_NEAR(2.0, aMaxX, 1.0e-12);
  EXPECT_NEAR(1.0, aMaxY, 1.0e-12);
}

TEST(BRepFont_PlanarRegionTest, CubicBoundsHandleConstantCoordinate)
{
  const BRepFont_PlanarRegion::Curve aCurve(Font_GlyphOutline::Segment::Cubic(gp_XY(0.0, 2.0),
                                                                              gp_XY(4.0, 2.0),
                                                                              gp_XY(4.0, 2.0),
                                                                              gp_XY(0.0, 2.0)));

  double aMinX = 0.0;
  double aMinY = 0.0;
  double aMaxX = 0.0;
  double aMaxY = 0.0;
  aCurve.Bounds().Get(aMinX, aMinY, aMaxX, aMaxY);
  EXPECT_NEAR(0.0, aMinX, 1.0e-12);
  EXPECT_NEAR(2.0, aMinY, 1.0e-12);
  EXPECT_NEAR(3.0, aMaxX, 1.0e-12);
  EXPECT_NEAR(2.0, aMaxY, 1.0e-12);
}

TEST(BRepFont_PlanarRegionTest, BoundsRemainConservativeWhenCoefficientsOverflow)
{
  const double                       aLimit = std::numeric_limits<double>::max();
  const BRepFont_PlanarRegion::Curve aCurve(
    Font_GlyphOutline::Segment::Quadratic(gp_XY(aLimit, 0.0),
                                          gp_XY(-aLimit, 1.0),
                                          gp_XY(aLimit, 0.0)));

  double aMinX = 0.0;
  double aMinY = 0.0;
  double aMaxX = 0.0;
  double aMaxY = 0.0;
  aCurve.Bounds().Get(aMinX, aMinY, aMaxX, aMaxY);
  EXPECT_DOUBLE_EQ(-aLimit, aMinX);
  EXPECT_DOUBLE_EQ(0.0, aMinY);
  EXPECT_DOUBLE_EQ(aLimit, aMaxX);
  EXPECT_DOUBLE_EQ(1.0, aMaxY);
}
