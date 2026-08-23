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

#include <BRepFont_Regularizer.hxx>

#include <cmath>
#include <initializer_list>
#include <iterator>
#include <utility>

#include <gtest/gtest.h>

namespace BRepFont_Regularizer_Test
{
void appendClosedContour(const std::initializer_list<gp_XY>&                   thePoints,
                         NCollection_LinearVector<Font_GlyphOutline::Segment>& theSegments,
                         NCollection_LinearVector<Font_GlyphOutline::Contour>& theContours)
{
  const uint32_t aFirstSegment = static_cast<uint32_t>(theSegments.Size());
  for (auto aPoint = thePoints.begin(); aPoint != thePoints.end(); ++aPoint)
  {
    const auto aNext = std::next(aPoint) == thePoints.end() ? thePoints.begin() : std::next(aPoint);
    theSegments.Append(Font_GlyphOutline::Segment::Line(*aPoint, *aNext));
  }
  theContours.EmplaceAppend(aFirstSegment, static_cast<uint32_t>(thePoints.size()));
}

void appendStationaryCubicContour(const std::initializer_list<gp_XY>&                   thePoints,
                                  NCollection_LinearVector<Font_GlyphOutline::Segment>& theSegments,
                                  NCollection_LinearVector<Font_GlyphOutline::Contour>& theContours)
{
  const uint32_t aFirstSegment = static_cast<uint32_t>(theSegments.Size());
  for (auto aPoint = thePoints.begin(); aPoint != thePoints.end(); ++aPoint)
  {
    const auto aNext = std::next(aPoint) == thePoints.end() ? thePoints.begin() : std::next(aPoint);
    theSegments.Append(Font_GlyphOutline::Segment::Cubic(*aPoint, *aNext, *aPoint, *aNext));
  }
  theContours.EmplaceAppend(aFirstSegment, static_cast<uint32_t>(thePoints.size()));
}

Font_GlyphOutline createOutline(
  NCollection_LinearVector<Font_GlyphOutline::Segment>&& theSegments,
  NCollection_LinearVector<Font_GlyphOutline::Contour>&& theContours,
  const bool                                             theHasPossibleOverlaps = false,
  const Font_GlyphOutline::FillRule theFillRule       = Font_GlyphOutline::FillRule::NonZero,
  const bool                        theIsSingleStroke = false)
{
  return Font_GlyphOutline::Create(1000.0,
                                   theFillRule,
                                   theHasPossibleOverlaps,
                                   theIsSingleStroke,
                                   std::move(theSegments),
                                   std::move(theContours));
}
} // namespace BRepFont_Regularizer_Test

TEST(BRepFont_RegularizerTest, ClosedRectangleBuildsOneRegion)
{
  NCollection_LinearVector<Font_GlyphOutline::Segment> aSegments;
  NCollection_LinearVector<Font_GlyphOutline::Contour> aContours;
  BRepFont_Regularizer_Test::appendClosedContour(
    {gp_XY(0.0, 0.0), gp_XY(20.0, 0.0), gp_XY(20.0, 10.0), gp_XY(0.0, 10.0)},
    aSegments,
    aContours);

  const BRepFont_Regularizer::Result aResult = BRepFont_Regularizer().Build(
    BRepFont_Regularizer_Test::createOutline(std::move(aSegments), std::move(aContours)));
  ASSERT_EQ(BRepFont_Regularizer::Status::Success, aResult.StatusValue());
  ASSERT_TRUE(aResult.PlanarRegion().has_value());
  EXPECT_EQ(4u, aResult.PlanarRegion()->Vertices().Size());
  EXPECT_EQ(4u, aResult.PlanarRegion()->Curves().Size());
  EXPECT_EQ(4u, aResult.PlanarRegion()->Uses().Size());
  EXPECT_EQ(1u, aResult.PlanarRegion()->Loops().Size());
  ASSERT_EQ(1u, aResult.PlanarRegion()->Regions().Size());
  EXPECT_EQ(0u, aResult.PlanarRegion()->Regions()[0].NbHoles());
}

TEST(BRepFont_RegularizerTest, NestedContourBuildsHole)
{
  NCollection_LinearVector<Font_GlyphOutline::Segment> aSegments;
  NCollection_LinearVector<Font_GlyphOutline::Contour> aContours;
  BRepFont_Regularizer_Test::appendClosedContour(
    {gp_XY(0.0, 0.0), gp_XY(20.0, 0.0), gp_XY(20.0, 20.0), gp_XY(0.0, 20.0)},
    aSegments,
    aContours);
  BRepFont_Regularizer_Test::appendClosedContour(
    {gp_XY(5.0, 5.0), gp_XY(5.0, 15.0), gp_XY(15.0, 15.0), gp_XY(15.0, 5.0)},
    aSegments,
    aContours);

  const BRepFont_Regularizer::Result aResult = BRepFont_Regularizer().Build(
    BRepFont_Regularizer_Test::createOutline(std::move(aSegments), std::move(aContours)));
  ASSERT_EQ(BRepFont_Regularizer::Status::Success, aResult.StatusValue());
  ASSERT_TRUE(aResult.PlanarRegion().has_value());
  EXPECT_EQ(2u, aResult.PlanarRegion()->Loops().Size());
  ASSERT_EQ(1u, aResult.PlanarRegion()->Regions().Size());
  EXPECT_EQ(1u, aResult.PlanarRegion()->Regions()[0].NbHoles());
  EXPECT_EQ(1u, aResult.PlanarRegion()->HoleLoops().Size());
}

TEST(BRepFont_RegularizerTest, NonZeroFillDropsSameWindingNestedContour)
{
  NCollection_LinearVector<Font_GlyphOutline::Segment> aSegments;
  NCollection_LinearVector<Font_GlyphOutline::Contour> aContours;
  BRepFont_Regularizer_Test::appendClosedContour(
    {gp_XY(0.0, 0.0), gp_XY(20.0, 0.0), gp_XY(20.0, 20.0), gp_XY(0.0, 20.0)},
    aSegments,
    aContours);
  BRepFont_Regularizer_Test::appendClosedContour(
    {gp_XY(5.0, 5.0), gp_XY(15.0, 5.0), gp_XY(15.0, 15.0), gp_XY(5.0, 15.0)},
    aSegments,
    aContours);

  const BRepFont_Regularizer::Result aResult = BRepFont_Regularizer().Build(
    BRepFont_Regularizer_Test::createOutline(std::move(aSegments), std::move(aContours)));
  ASSERT_EQ(BRepFont_Regularizer::Status::Success, aResult.StatusValue());
  ASSERT_TRUE(aResult.PlanarRegion().has_value());
  EXPECT_EQ(1u, aResult.PlanarRegion()->Loops().Size());
  EXPECT_EQ(4u, aResult.PlanarRegion()->Uses().Size());
  EXPECT_EQ(1u, aResult.PlanarRegion()->Regions().Size());
  EXPECT_TRUE(aResult.PlanarRegion()->HoleLoops().IsEmpty());
}

TEST(BRepFont_RegularizerTest, EvenOddFillKeepsSameWindingNestedContourAsHole)
{
  NCollection_LinearVector<Font_GlyphOutline::Segment> aSegments;
  NCollection_LinearVector<Font_GlyphOutline::Contour> aContours;
  BRepFont_Regularizer_Test::appendClosedContour(
    {gp_XY(0.0, 0.0), gp_XY(20.0, 0.0), gp_XY(20.0, 20.0), gp_XY(0.0, 20.0)},
    aSegments,
    aContours);
  BRepFont_Regularizer_Test::appendClosedContour(
    {gp_XY(5.0, 5.0), gp_XY(15.0, 5.0), gp_XY(15.0, 15.0), gp_XY(5.0, 15.0)},
    aSegments,
    aContours);

  const BRepFont_Regularizer::Result aResult = BRepFont_Regularizer().Build(
    BRepFont_Regularizer_Test::createOutline(std::move(aSegments),
                                             std::move(aContours),
                                             false,
                                             Font_GlyphOutline::FillRule::EvenOdd));
  ASSERT_EQ(BRepFont_Regularizer::Status::Success, aResult.StatusValue());
  ASSERT_TRUE(aResult.PlanarRegion().has_value());
  EXPECT_EQ(2u, aResult.PlanarRegion()->Loops().Size());
  ASSERT_EQ(1u, aResult.PlanarRegion()->Regions().Size());
  EXPECT_EQ(1u, aResult.PlanarRegion()->Regions()[0].NbHoles());
}

TEST(BRepFont_RegularizerTest, ThreeNestedWindingTransitionsBuildIslandRegion)
{
  NCollection_LinearVector<Font_GlyphOutline::Segment> aSegments;
  NCollection_LinearVector<Font_GlyphOutline::Contour> aContours;
  BRepFont_Regularizer_Test::appendClosedContour(
    {gp_XY(0.0, 0.0), gp_XY(30.0, 0.0), gp_XY(30.0, 30.0), gp_XY(0.0, 30.0)},
    aSegments,
    aContours);
  BRepFont_Regularizer_Test::appendClosedContour(
    {gp_XY(5.0, 5.0), gp_XY(5.0, 25.0), gp_XY(25.0, 25.0), gp_XY(25.0, 5.0)},
    aSegments,
    aContours);
  BRepFont_Regularizer_Test::appendClosedContour(
    {gp_XY(10.0, 10.0), gp_XY(20.0, 10.0), gp_XY(20.0, 20.0), gp_XY(10.0, 20.0)},
    aSegments,
    aContours);

  const BRepFont_Regularizer::Result aResult = BRepFont_Regularizer().Build(
    BRepFont_Regularizer_Test::createOutline(std::move(aSegments), std::move(aContours)));
  ASSERT_EQ(BRepFont_Regularizer::Status::Success, aResult.StatusValue());
  ASSERT_TRUE(aResult.PlanarRegion().has_value());
  EXPECT_EQ(3u, aResult.PlanarRegion()->Loops().Size());
  EXPECT_EQ(2u, aResult.PlanarRegion()->Regions().Size());
  EXPECT_EQ(1u, aResult.PlanarRegion()->HoleLoops().Size());
  EXPECT_EQ(1u, aResult.PlanarRegion()->Regions()[0].NbHoles());
  EXPECT_EQ(0u, aResult.PlanarRegion()->Regions()[1].NbHoles());
}

TEST(BRepFont_RegularizerTest, DisjointContoursBuildSeparateRegions)
{
  NCollection_LinearVector<Font_GlyphOutline::Segment> aSegments;
  NCollection_LinearVector<Font_GlyphOutline::Contour> aContours;
  BRepFont_Regularizer_Test::appendClosedContour(
    {gp_XY(0.0, 0.0), gp_XY(5.0, 0.0), gp_XY(5.0, 5.0), gp_XY(0.0, 5.0)},
    aSegments,
    aContours);
  BRepFont_Regularizer_Test::appendClosedContour(
    {gp_XY(20.0, 0.0), gp_XY(25.0, 0.0), gp_XY(25.0, 5.0), gp_XY(20.0, 5.0)},
    aSegments,
    aContours);

  const BRepFont_Regularizer::Result aResult = BRepFont_Regularizer().Build(
    BRepFont_Regularizer_Test::createOutline(std::move(aSegments), std::move(aContours)));
  ASSERT_EQ(BRepFont_Regularizer::Status::Success, aResult.StatusValue());
  ASSERT_TRUE(aResult.PlanarRegion().has_value());
  EXPECT_EQ(2u, aResult.PlanarRegion()->Regions().Size());
}

TEST(BRepFont_RegularizerTest, ConcaveStationaryCubicsUseFlattenedInteriorPoint)
{
  NCollection_LinearVector<Font_GlyphOutline::Segment> aSegments;
  NCollection_LinearVector<Font_GlyphOutline::Contour> aContours;
  BRepFont_Regularizer_Test::appendStationaryCubicContour({gp_XY(0.0, 0.0),
                                                           gp_XY(10.0, 0.0),
                                                           gp_XY(10.0, 3.0),
                                                           gp_XY(3.0, 3.0),
                                                           gp_XY(3.0, 7.0),
                                                           gp_XY(10.0, 7.0),
                                                           gp_XY(10.0, 10.0),
                                                           gp_XY(0.0, 10.0)},
                                                          aSegments,
                                                          aContours);

  const BRepFont_Regularizer::Result aResult = BRepFont_Regularizer().Build(
    BRepFont_Regularizer_Test::createOutline(std::move(aSegments), std::move(aContours)));
  ASSERT_EQ(BRepFont_Regularizer::Status::Success, aResult.StatusValue());
  ASSERT_TRUE(aResult.PlanarRegion().has_value());
  ASSERT_EQ(1u, aResult.PlanarRegion()->Regions().Size());
  EXPECT_EQ(0u, aResult.PlanarRegion()->Regions()[0].NbHoles());
}

TEST(BRepFont_RegularizerTest, IntersectingContourReturnsNoRegion)
{
  NCollection_LinearVector<Font_GlyphOutline::Segment> aSegments;
  NCollection_LinearVector<Font_GlyphOutline::Contour> aContours;
  BRepFont_Regularizer_Test::appendClosedContour(
    {gp_XY(0.0, 0.0), gp_XY(10.0, 10.0), gp_XY(2.0, 8.0), gp_XY(10.0, 0.0)},
    aSegments,
    aContours);

  const BRepFont_Regularizer::Result aResult = BRepFont_Regularizer().Build(
    BRepFont_Regularizer_Test::createOutline(std::move(aSegments), std::move(aContours)));
  EXPECT_EQ(BRepFont_Regularizer::Status::IntersectingContours, aResult.StatusValue());
  EXPECT_FALSE(aResult.IsDone());
  EXPECT_FALSE(aResult.PlanarRegion().has_value());
}

TEST(BRepFont_RegularizerTest, RetracingQuadraticReturnsNoRegion)
{
  NCollection_LinearVector<Font_GlyphOutline::Segment> aSegments;
  aSegments.Append(
    Font_GlyphOutline::Segment::Quadratic(gp_XY(0.0, 0.0), gp_XY(10.0, 0.0), gp_XY(0.0, 0.0)));
  aSegments.Append(Font_GlyphOutline::Segment::Line(gp_XY(0.0, 0.0), gp_XY(0.0, 10.0)));
  aSegments.Append(Font_GlyphOutline::Segment::Line(gp_XY(0.0, 10.0), gp_XY(-10.0, 10.0)));
  aSegments.Append(Font_GlyphOutline::Segment::Line(gp_XY(-10.0, 10.0), gp_XY(-10.0, 0.0)));
  aSegments.Append(Font_GlyphOutline::Segment::Line(gp_XY(-10.0, 0.0), gp_XY(0.0, 0.0)));
  NCollection_LinearVector<Font_GlyphOutline::Contour> aContours;
  aContours.EmplaceAppend(0, 5);

  const BRepFont_Regularizer::Result aResult = BRepFont_Regularizer().Build(
    BRepFont_Regularizer_Test::createOutline(std::move(aSegments), std::move(aContours)));
  EXPECT_EQ(BRepFont_Regularizer::Status::IntersectingContours, aResult.StatusValue());
  EXPECT_FALSE(aResult.PlanarRegion().has_value());
}

TEST(BRepFont_RegularizerTest, MarkedSelfIntersectingContourIsReported)
{
  NCollection_LinearVector<Font_GlyphOutline::Segment> aSegments;
  NCollection_LinearVector<Font_GlyphOutline::Contour> aContours;
  BRepFont_Regularizer_Test::appendClosedContour(
    {gp_XY(0.0, 0.0), gp_XY(10.0, 10.0), gp_XY(2.0, 8.0), gp_XY(10.0, 0.0)},
    aSegments,
    aContours);

  const BRepFont_Regularizer::Result aResult = BRepFont_Regularizer().Build(
    BRepFont_Regularizer_Test::createOutline(std::move(aSegments), std::move(aContours), true));
  EXPECT_EQ(BRepFont_Regularizer::Status::SelfIntersectingContour, aResult.StatusValue());
  EXPECT_FALSE(aResult.IsDone());
  EXPECT_FALSE(aResult.PlanarRegion().has_value());
}

TEST(BRepFont_RegularizerTest, OverlapMetadataWithoutIntersectionBuildsNormally)
{
  NCollection_LinearVector<Font_GlyphOutline::Segment> aSegments;
  NCollection_LinearVector<Font_GlyphOutline::Contour> aContours;
  BRepFont_Regularizer_Test::appendClosedContour(
    {gp_XY(0.0, 0.0), gp_XY(10.0, 0.0), gp_XY(10.0, 10.0), gp_XY(0.0, 10.0)},
    aSegments,
    aContours);

  const BRepFont_Regularizer::Result aResult = BRepFont_Regularizer().Build(
    BRepFont_Regularizer_Test::createOutline(std::move(aSegments), std::move(aContours), true));
  EXPECT_EQ(BRepFont_Regularizer::Status::Success, aResult.StatusValue());
  EXPECT_TRUE(aResult.PlanarRegion().has_value());
}

TEST(BRepFont_RegularizerTest, MarkedInterLoopOverlapBuildsSeparateRegions)
{
  NCollection_LinearVector<Font_GlyphOutline::Segment> aSegments;
  NCollection_LinearVector<Font_GlyphOutline::Contour> aContours;
  BRepFont_Regularizer_Test::appendClosedContour(
    {gp_XY(0.0, 0.0), gp_XY(10.0, 0.0), gp_XY(10.0, 10.0), gp_XY(0.0, 10.0)},
    aSegments,
    aContours);
  BRepFont_Regularizer_Test::appendClosedContour(
    {gp_XY(8.0, 2.0), gp_XY(12.0, 2.0), gp_XY(12.0, 8.0), gp_XY(8.0, 8.0)},
    aSegments,
    aContours);

  const BRepFont_Regularizer::Result aResult = BRepFont_Regularizer().Build(
    BRepFont_Regularizer_Test::createOutline(std::move(aSegments), std::move(aContours), true));
  ASSERT_EQ(BRepFont_Regularizer::Status::Success, aResult.StatusValue());
  ASSERT_TRUE(aResult.PlanarRegion().has_value());
  EXPECT_EQ(2u, aResult.PlanarRegion()->Regions().Size());
}

TEST(BRepFont_RegularizerTest, TouchingContoursAreRejectedAsIntersections)
{
  NCollection_LinearVector<Font_GlyphOutline::Segment> aSegments;
  NCollection_LinearVector<Font_GlyphOutline::Contour> aContours;
  BRepFont_Regularizer_Test::appendClosedContour(
    {gp_XY(0.0, 0.0), gp_XY(10.0, 0.0), gp_XY(10.0, 10.0), gp_XY(0.0, 10.0)},
    aSegments,
    aContours);
  BRepFont_Regularizer_Test::appendClosedContour(
    {gp_XY(10.0, 10.0), gp_XY(20.0, 10.0), gp_XY(20.0, 20.0), gp_XY(10.0, 20.0)},
    aSegments,
    aContours);

  const BRepFont_Regularizer::Result aResult = BRepFont_Regularizer().Build(
    BRepFont_Regularizer_Test::createOutline(std::move(aSegments), std::move(aContours)));
  EXPECT_EQ(BRepFont_Regularizer::Status::IntersectingContours, aResult.StatusValue());
  EXPECT_FALSE(aResult.PlanarRegion().has_value());
}

TEST(BRepFont_RegularizerTest, SingleStrokeAcceptsOpenContour)
{
  NCollection_LinearVector<Font_GlyphOutline::Segment> aSegments;
  aSegments.Append(Font_GlyphOutline::Segment::Line(gp_XY(0.0, 0.0), gp_XY(10.0, 0.0)));
  aSegments.Append(
    Font_GlyphOutline::Segment::Quadratic(gp_XY(10.0, 0.0), gp_XY(15.0, 5.0), gp_XY(20.0, 0.0)));
  NCollection_LinearVector<Font_GlyphOutline::Contour> aContours;
  aContours.EmplaceAppend(0, 2);

  const BRepFont_Regularizer::Result aResult = BRepFont_Regularizer().Build(
    BRepFont_Regularizer_Test::createOutline(std::move(aSegments),
                                             std::move(aContours),
                                             false,
                                             Font_GlyphOutline::FillRule::NonZero,
                                             true));
  ASSERT_EQ(BRepFont_Regularizer::Status::Success, aResult.StatusValue());
  ASSERT_TRUE(aResult.PlanarRegion().has_value());
  EXPECT_TRUE(aResult.PlanarRegion()->IsSingleStroke());
  EXPECT_EQ(1u, aResult.PlanarRegion()->Loops().Size());
  EXPECT_TRUE(aResult.PlanarRegion()->Regions().IsEmpty());
}

TEST(BRepFont_RegularizerTest, OpenContourIsRejected)
{
  NCollection_LinearVector<Font_GlyphOutline::Segment> aSegments;
  aSegments.Append(Font_GlyphOutline::Segment::Line(gp_XY(0.0, 0.0), gp_XY(10.0, 0.0)));
  aSegments.Append(Font_GlyphOutline::Segment::Line(gp_XY(11.0, 0.0), gp_XY(0.0, 0.0)));
  NCollection_LinearVector<Font_GlyphOutline::Contour> aContours;
  aContours.EmplaceAppend(0, 2);

  const BRepFont_Regularizer::Result aResult = BRepFont_Regularizer().Build(
    BRepFont_Regularizer_Test::createOutline(std::move(aSegments), std::move(aContours)));
  EXPECT_EQ(BRepFont_Regularizer::Status::OpenContour, aResult.StatusValue());
  EXPECT_FALSE(aResult.PlanarRegion().has_value());
}

TEST(BRepFont_RegularizerTest, ToleranceHealsEndpointsAndRemovesPointLikeSegments)
{
  NCollection_LinearVector<Font_GlyphOutline::Segment> aSegments;
  aSegments.Append(Font_GlyphOutline::Segment::Line(gp_XY(0.0, 0.0), gp_XY(10.0, 0.0)));
  aSegments.Append(Font_GlyphOutline::Segment::Line(gp_XY(10.0, 0.0), gp_XY(10.0005, 0.0)));
  aSegments.Append(Font_GlyphOutline::Segment::Line(gp_XY(10.0005, 0.0), gp_XY(10.0, 10.0)));
  aSegments.Append(Font_GlyphOutline::Segment::Line(gp_XY(10.0, 10.0), gp_XY(0.0, 10.0)));
  aSegments.Append(Font_GlyphOutline::Segment::Line(gp_XY(0.0, 10.0), gp_XY(0.0, -0.0005)));
  aSegments.Append(Font_GlyphOutline::Segment::Line(gp_XY(0.0, -0.0005), gp_XY(0.0, 0.0)));
  NCollection_LinearVector<Font_GlyphOutline::Contour> aContours;
  aContours.EmplaceAppend(0, 6);

  BRepFont_Regularizer::Options anOptions;
  anOptions.Tolerance                        = 0.001;
  const BRepFont_Regularizer::Result aResult = BRepFont_Regularizer().Build(
    BRepFont_Regularizer_Test::createOutline(std::move(aSegments), std::move(aContours)),
    anOptions);
  ASSERT_EQ(BRepFont_Regularizer::Status::Success, aResult.StatusValue());
  ASSERT_TRUE(aResult.PlanarRegion().has_value());
  const BRepFont_PlanarRegion& aRegion = *aResult.PlanarRegion();
  EXPECT_EQ(4u, aRegion.Vertices().Size());
  EXPECT_EQ(4u, aRegion.Curves().Size());
  ASSERT_EQ(4u, aRegion.Uses().Size());
  ASSERT_EQ(1u, aRegion.Loops().Size());
  EXPECT_EQ(4u, aRegion.Loops()[0].NbUses());
  double aMinX = 0.0;
  double aMinY = 0.0;
  double aMaxX = 0.0;
  double aMaxY = 0.0;
  aRegion.Loops()[0].Bounds().Get(aMinX, aMinY, aMaxX, aMaxY);
  EXPECT_DOUBLE_EQ(0.0, aMinX);
  EXPECT_DOUBLE_EQ(0.0, aMinY);
  EXPECT_DOUBLE_EQ(10.0, aMaxX);
  EXPECT_DOUBLE_EQ(10.0, aMaxY);
  for (const BRepFont_PlanarRegion::Use& aUse : aRegion.Uses())
  {
    const BRepFont_PlanarRegion::Curve& aCurve = aRegion.Curves()[aUse.CurveIndex().Value()];
    EXPECT_DOUBLE_EQ(
      0.0,
      (aCurve.Start() - aRegion.Vertices()[aUse.FirstVertex().Value()]).SquareModulus());
    EXPECT_DOUBLE_EQ(
      0.0,
      (aCurve.End() - aRegion.Vertices()[aUse.LastVertex().Value()]).SquareModulus());
  }
}

TEST(BRepFont_RegularizerTest, PointLikeContourBecomesEmpty)
{
  NCollection_LinearVector<Font_GlyphOutline::Segment> aSegments;
  aSegments.Append(Font_GlyphOutline::Segment::Quadratic(gp_XY(0.0, 0.0),
                                                         gp_XY(0.0004, 0.0003),
                                                         gp_XY(0.0002, 0.0001)));
  NCollection_LinearVector<Font_GlyphOutline::Contour> aContours;
  aContours.EmplaceAppend(0, 1);

  BRepFont_Regularizer::Options anOptions;
  anOptions.Tolerance                        = 0.001;
  const BRepFont_Regularizer::Result aResult = BRepFont_Regularizer().Build(
    BRepFont_Regularizer_Test::createOutline(std::move(aSegments), std::move(aContours)),
    anOptions);
  EXPECT_EQ(BRepFont_Regularizer::Status::Empty, aResult.StatusValue());
  ASSERT_TRUE(aResult.PlanarRegion().has_value());
  EXPECT_TRUE(aResult.PlanarRegion()->IsEmpty());
  EXPECT_TRUE(aResult.PlanarRegion()->Vertices().IsEmpty());
  EXPECT_TRUE(aResult.PlanarRegion()->Curves().IsEmpty());
  EXPECT_TRUE(aResult.PlanarRegion()->Uses().IsEmpty());
}

TEST(BRepFont_RegularizerTest, NonFiniteGeometryIsRejected)
{
  NCollection_LinearVector<Font_GlyphOutline::Segment> aSegments;
  aSegments.Append(Font_GlyphOutline::Segment::Line(gp_XY(0.0, 0.0), gp_XY(std::nan(""), 1.0)));
  NCollection_LinearVector<Font_GlyphOutline::Contour> aContours;
  aContours.EmplaceAppend(0, 1);

  const BRepFont_Regularizer::Result aResult = BRepFont_Regularizer().Build(
    BRepFont_Regularizer_Test::createOutline(std::move(aSegments), std::move(aContours)));
  EXPECT_EQ(BRepFont_Regularizer::Status::InvalidOutline, aResult.StatusValue());
  EXPECT_FALSE(aResult.PlanarRegion().has_value());
}

TEST(BRepFont_RegularizerTest, FiniteAreaOverflowIsRejected)
{
  const double                                         aLimit = std::numeric_limits<double>::max();
  NCollection_LinearVector<Font_GlyphOutline::Segment> aSegments;
  NCollection_LinearVector<Font_GlyphOutline::Contour> aContours;
  BRepFont_Regularizer_Test::appendClosedContour({gp_XY(aLimit, aLimit),
                                                  gp_XY(-aLimit, aLimit),
                                                  gp_XY(-aLimit, -aLimit),
                                                  gp_XY(aLimit, -aLimit)},
                                                 aSegments,
                                                 aContours);

  const BRepFont_Regularizer::Result aResult = BRepFont_Regularizer().Build(
    BRepFont_Regularizer_Test::createOutline(std::move(aSegments), std::move(aContours)));
  EXPECT_EQ(BRepFont_Regularizer::Status::InvalidOutline, aResult.StatusValue());
  EXPECT_FALSE(aResult.PlanarRegion().has_value());
}

TEST(BRepFont_RegularizerTest, OrphanedSegmentIsRejected)
{
  NCollection_LinearVector<Font_GlyphOutline::Segment> aSegments;
  NCollection_LinearVector<Font_GlyphOutline::Contour> aContours;
  BRepFont_Regularizer_Test::appendClosedContour(
    {gp_XY(0.0, 0.0), gp_XY(10.0, 0.0), gp_XY(10.0, 10.0), gp_XY(0.0, 10.0)},
    aSegments,
    aContours);
  aSegments.Append(Font_GlyphOutline::Segment::Line(gp_XY(20.0, 0.0), gp_XY(21.0, 0.0)));

  const BRepFont_Regularizer::Result aResult = BRepFont_Regularizer().Build(
    BRepFont_Regularizer_Test::createOutline(std::move(aSegments), std::move(aContours)));
  EXPECT_EQ(BRepFont_Regularizer::Status::InvalidOutline, aResult.StatusValue());
  EXPECT_FALSE(aResult.PlanarRegion().has_value());
}

TEST(BRepFont_RegularizerTest, EmptySegmentsWithContourIsRejected)
{
  NCollection_LinearVector<Font_GlyphOutline::Segment> aSegments;
  NCollection_LinearVector<Font_GlyphOutline::Contour> aContours;
  aContours.EmplaceAppend(0, 1);

  const BRepFont_Regularizer::Result aResult = BRepFont_Regularizer().Build(
    BRepFont_Regularizer_Test::createOutline(std::move(aSegments), std::move(aContours)));
  EXPECT_EQ(BRepFont_Regularizer::Status::InvalidOutline, aResult.StatusValue());
  EXPECT_FALSE(aResult.PlanarRegion().has_value());
}

TEST(BRepFont_RegularizerTest, NonPackedContourRangesAreRejected)
{
  NCollection_LinearVector<Font_GlyphOutline::Segment> aSegments;
  NCollection_LinearVector<Font_GlyphOutline::Contour> aContours;
  BRepFont_Regularizer_Test::appendClosedContour(
    {gp_XY(0.0, 0.0), gp_XY(10.0, 0.0), gp_XY(10.0, 10.0), gp_XY(0.0, 10.0)},
    aSegments,
    aContours);
  aSegments.Append(Font_GlyphOutline::Segment::Line(gp_XY(20.0, 0.0), gp_XY(21.0, 0.0)));
  BRepFont_Regularizer_Test::appendClosedContour(
    {gp_XY(30.0, 0.0), gp_XY(40.0, 0.0), gp_XY(40.0, 10.0), gp_XY(30.0, 10.0)},
    aSegments,
    aContours);

  const BRepFont_Regularizer::Result aResult = BRepFont_Regularizer().Build(
    BRepFont_Regularizer_Test::createOutline(std::move(aSegments), std::move(aContours)));
  EXPECT_EQ(BRepFont_Regularizer::Status::InvalidOutline, aResult.StatusValue());
  EXPECT_FALSE(aResult.PlanarRegion().has_value());
}

TEST(BRepFont_RegularizerTest, InvalidToleranceAndUnitsAreRejected)
{
  NCollection_LinearVector<Font_GlyphOutline::Segment> aSegments;
  NCollection_LinearVector<Font_GlyphOutline::Contour> aContours;
  BRepFont_Regularizer_Test::appendClosedContour(
    {gp_XY(0.0, 0.0), gp_XY(10.0, 0.0), gp_XY(0.0, 10.0)},
    aSegments,
    aContours);
  const Font_GlyphOutline anOutline =
    BRepFont_Regularizer_Test::createOutline(std::move(aSegments), std::move(aContours));

  BRepFont_Regularizer::Options anOptions;
  anOptions.Tolerance = 0.0;
  EXPECT_EQ(BRepFont_Regularizer::Status::InvalidOutline,
            BRepFont_Regularizer().Build(anOutline, anOptions).StatusValue());

  NCollection_LinearVector<Font_GlyphOutline::Segment> zeroSegments;
  NCollection_LinearVector<Font_GlyphOutline::Contour> zeroContours;
  const Font_GlyphOutline                              aZeroUnitsOutline =
    Font_GlyphOutline::Create(0.0,
                              Font_GlyphOutline::FillRule::NonZero,
                              false,
                              false,
                              std::move(zeroSegments),
                              std::move(zeroContours));
  EXPECT_EQ(BRepFont_Regularizer::Status::InvalidOutline,
            BRepFont_Regularizer().Build(aZeroUnitsOutline).StatusValue());
}
