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

#include <Font_GlyphOutline.hxx>

#include <utility>

#include <gtest/gtest.h>

TEST(Font_GlyphOutlineTest, CreatesOutline)
{
  NCollection_LinearVector<Font_GlyphOutline::Segment> aSegments;
  aSegments.Append(Font_GlyphOutline::Segment::Line(gp_XY(0.0, 0.0), gp_XY(10.0, 0.0)));
  aSegments.Append(
    Font_GlyphOutline::Segment::Quadratic(gp_XY(10.0, 0.0), gp_XY(12.0, 5.0), gp_XY(0.0, 0.0)));
  NCollection_LinearVector<Font_GlyphOutline::Contour> aContours;
  aContours.EmplaceAppend(0, 2);

  const Font_GlyphOutline anOutline =
    Font_GlyphOutline::Create(2048.0,
                              Font_GlyphOutline::FillRule::EvenOdd,
                              true,
                              false,
                              std::move(aSegments),
                              std::move(aContours));

  EXPECT_DOUBLE_EQ(2048.0, anOutline.UnitsPerEm());
  EXPECT_EQ(Font_GlyphOutline::FillRule::EvenOdd, anOutline.Rule());
  EXPECT_TRUE(anOutline.HasPossibleOverlaps());
  EXPECT_FALSE(anOutline.IsSingleStroke());
  ASSERT_EQ(2u, anOutline.Segments().Size());
  ASSERT_EQ(1u, anOutline.Contours().Size());
  EXPECT_EQ(Font_GlyphOutline::Segment::Kind::QuadraticBezier, anOutline.Segments()[1].Type());
  EXPECT_EQ(0u, anOutline.Contours()[0].FirstSegment());
  EXPECT_EQ(2u, anOutline.Contours()[0].NbSegments());
}
