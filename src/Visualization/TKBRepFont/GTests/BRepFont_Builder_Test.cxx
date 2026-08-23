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
#include <BRepFont_Builder.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepGProp.hxx>
#include <BRep_Tool.hxx>
#include <GProp_GProps.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Font_FTFont.hxx>
#include <Font_FontMgr.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>

#include <limits>
#include <initializer_list>
#include <iterator>
#include <optional>
#include <utility>

#include <gtest/gtest.h>

namespace BRepFont_Builder_Test
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

std::optional<BRepFont_PlanarRegion> createTriangle()
{
  NCollection_LinearVector<Font_GlyphOutline::Segment> aSegments;
  aSegments.Append(Font_GlyphOutline::Segment::Line(gp_XY(0.0, 0.0), gp_XY(10.0, 0.0)));
  aSegments.Append(Font_GlyphOutline::Segment::Line(gp_XY(10.0, 0.0), gp_XY(5.0, 10.0)));
  aSegments.Append(Font_GlyphOutline::Segment::Line(gp_XY(5.0, 10.0), gp_XY(0.0, 0.0)));
  NCollection_LinearVector<Font_GlyphOutline::Contour> aContours;
  aContours.EmplaceAppend(0, 3);

  const Font_GlyphOutline anOutline =
    Font_GlyphOutline::Create(1000.0,
                              Font_GlyphOutline::FillRule::NonZero,
                              false,
                              false,
                              std::move(aSegments),
                              std::move(aContours));
  BRepFont_Regularizer::Options anOptions;
  anOptions.Tolerance                  = 1.0e-7;
  BRepFont_Regularizer::Result aResult = BRepFont_Regularizer().Build(anOutline, anOptions);
  return aResult.PlanarRegion().has_value()
           ? std::optional<BRepFont_PlanarRegion>(std::move(*aResult.ChangePlanarRegion()))
           : std::nullopt;
}

std::optional<BRepFont_PlanarRegion> createRegionWithHole()
{
  NCollection_LinearVector<Font_GlyphOutline::Segment> aSegments;
  NCollection_LinearVector<Font_GlyphOutline::Contour> aContours;
  appendClosedContour({gp_XY(0.0, 0.0), gp_XY(20.0, 0.0), gp_XY(20.0, 20.0), gp_XY(0.0, 20.0)},
                      aSegments,
                      aContours);
  appendClosedContour({gp_XY(5.0, 5.0), gp_XY(5.0, 15.0), gp_XY(15.0, 15.0), gp_XY(15.0, 5.0)},
                      aSegments,
                      aContours);

  const Font_GlyphOutline anOutline =
    Font_GlyphOutline::Create(1000.0,
                              Font_GlyphOutline::FillRule::NonZero,
                              false,
                              false,
                              std::move(aSegments),
                              std::move(aContours));
  BRepFont_Regularizer::Result aResult = BRepFont_Regularizer().Build(anOutline);
  return aResult.PlanarRegion().has_value()
           ? std::optional<BRepFont_PlanarRegion>(std::move(*aResult.ChangePlanarRegion()))
           : std::nullopt;
}

std::optional<BRepFont_PlanarRegion> createSameWindingNestedRegion()
{
  NCollection_LinearVector<Font_GlyphOutline::Segment> aSegments;
  NCollection_LinearVector<Font_GlyphOutline::Contour> aContours;
  appendClosedContour({gp_XY(0.0, 0.0), gp_XY(20.0, 0.0), gp_XY(20.0, 20.0), gp_XY(0.0, 20.0)},
                      aSegments,
                      aContours);
  appendClosedContour({gp_XY(5.0, 5.0), gp_XY(15.0, 5.0), gp_XY(15.0, 15.0), gp_XY(5.0, 15.0)},
                      aSegments,
                      aContours);
  const Font_GlyphOutline anOutline =
    Font_GlyphOutline::Create(1000.0,
                              Font_GlyphOutline::FillRule::NonZero,
                              false,
                              false,
                              std::move(aSegments),
                              std::move(aContours));
  BRepFont_Regularizer::Result aResult = BRepFont_Regularizer().Build(anOutline);
  return aResult.PlanarRegion().has_value()
           ? std::optional<BRepFont_PlanarRegion>(std::move(*aResult.ChangePlanarRegion()))
           : std::nullopt;
}

std::optional<BRepFont_PlanarRegion> createSingleStrokeRegion()
{
  NCollection_LinearVector<Font_GlyphOutline::Segment> aSegments;
  aSegments.Append(Font_GlyphOutline::Segment::Line(gp_XY(0.0, 0.0), gp_XY(10.0, 0.0)));
  aSegments.Append(
    Font_GlyphOutline::Segment::Quadratic(gp_XY(10.0, 0.0), gp_XY(15.0, 5.0), gp_XY(20.0, 0.0)));
  NCollection_LinearVector<Font_GlyphOutline::Contour> aContours;
  aContours.EmplaceAppend(0, 2);
  const Font_GlyphOutline anOutline =
    Font_GlyphOutline::Create(1000.0,
                              Font_GlyphOutline::FillRule::NonZero,
                              false,
                              true,
                              std::move(aSegments),
                              std::move(aContours));
  BRepFont_Regularizer::Result aResult = BRepFont_Regularizer().Build(anOutline);
  return aResult.PlanarRegion().has_value()
           ? std::optional<BRepFont_PlanarRegion>(std::move(*aResult.ChangePlanarRegion()))
           : std::nullopt;
}

std::optional<BRepFont_PlanarRegion> createOverlappingRegion()
{
  NCollection_LinearVector<Font_GlyphOutline::Segment> aSegments;
  NCollection_LinearVector<Font_GlyphOutline::Contour> aContours;
  appendClosedContour({gp_XY(0.0, 0.0), gp_XY(10.0, 0.0), gp_XY(10.0, 10.0), gp_XY(0.0, 10.0)},
                      aSegments,
                      aContours);
  appendClosedContour({gp_XY(5.0, 0.0), gp_XY(15.0, 0.0), gp_XY(15.0, 10.0), gp_XY(5.0, 10.0)},
                      aSegments,
                      aContours);
  const Font_GlyphOutline anOutline =
    Font_GlyphOutline::Create(1000.0,
                              Font_GlyphOutline::FillRule::NonZero,
                              true,
                              false,
                              std::move(aSegments),
                              std::move(aContours));
  BRepFont_Regularizer::Result aResult = BRepFont_Regularizer().Build(anOutline);
  return aResult.PlanarRegion().has_value()
           ? std::optional<BRepFont_PlanarRegion>(std::move(*aResult.ChangePlanarRegion()))
           : std::nullopt;
}
} // namespace BRepFont_Builder_Test

TEST(BRepFont_BuilderTest, BuildsFace)
{
  const std::optional<BRepFont_PlanarRegion> aRegion = BRepFont_Builder_Test::createTriangle();
  ASSERT_TRUE(aRegion.has_value());

  BRepFont_Builder::GlyphOptions anOptions;
  anOptions.Size            = 10.0;
  anOptions.Tolerance       = 1.0e-7;
  const TopoDS_Shape aShape = BRepFont_Builder::BuildGlyph(*aRegion, anOptions);
  ASSERT_FALSE(aShape.IsNull());
  EXPECT_TRUE(BRepCheck_Analyzer(aShape).IsValid());
}

TEST(BRepFont_BuilderTest, ConcatenatesContourIntoOneBSplineEdge)
{
  const std::optional<BRepFont_PlanarRegion> aRegion = BRepFont_Builder_Test::createTriangle();
  ASSERT_TRUE(aRegion.has_value());

  const TopoDS_Shape aDefaultShape =
    BRepFont_Builder::BuildGlyph(*aRegion, BRepFont_Builder::GlyphOptions{});
  ASSERT_FALSE(aDefaultShape.IsNull());
  int aNbDefaultEdges = 0;
  for (TopExp_Explorer anExplorer(aDefaultShape, TopAbs_EDGE); anExplorer.More(); anExplorer.Next())
  {
    ++aNbDefaultEdges;
  }
  EXPECT_EQ(3, aNbDefaultEdges);

  BRepFont_Builder::GlyphOptions anOptions;
  anOptions.Size                  = 10.0;
  anOptions.Tolerance             = 1.0e-7;
  anOptions.ToConcatenateContours = true;
  const TopoDS_Shape aShape       = BRepFont_Builder::BuildGlyph(*aRegion, anOptions);
  ASSERT_FALSE(aShape.IsNull());
  EXPECT_TRUE(BRepCheck_Analyzer(aShape).IsValid());

  TopExp_Explorer anEdgeExplorer(aShape, TopAbs_EDGE);
  ASSERT_TRUE(anEdgeExplorer.More());
  const TopoDS_Edge anEdge = TopoDS::Edge(anEdgeExplorer.Current());
  double            aFirst = 0.0;
  double            aLast  = 0.0;
  EXPECT_TRUE(BRep_Tool::Curve(anEdge, aFirst, aLast)->IsKind(STANDARD_TYPE(Geom_BSplineCurve)));
  anEdgeExplorer.Next();
  EXPECT_FALSE(anEdgeExplorer.More());

  EXPECT_LT(aFirst, aLast);
}

TEST(BRepFont_BuilderTest, CompositeContoursPreserveFaceWithHole)
{
  const std::optional<BRepFont_PlanarRegion> aRegion =
    BRepFont_Builder_Test::createRegionWithHole();
  ASSERT_TRUE(aRegion.has_value());

  BRepFont_Builder::GlyphOptions anOptions;
  anOptions.Size                  = 10.0;
  anOptions.Tolerance             = 1.0e-7;
  anOptions.ToConcatenateContours = true;
  const TopoDS_Shape aShape       = BRepFont_Builder::BuildGlyph(*aRegion, anOptions);
  ASSERT_FALSE(aShape.IsNull());
  EXPECT_TRUE(BRepCheck_Analyzer(aShape).IsValid());

  int aNbFaces = 0;
  int aNbWires = 0;
  int aNbEdges = 0;
  for (TopExp_Explorer anExplorer(aShape, TopAbs_FACE); anExplorer.More(); anExplorer.Next())
  {
    ++aNbFaces;
  }
  for (TopExp_Explorer anExplorer(aShape, TopAbs_WIRE); anExplorer.More(); anExplorer.Next())
  {
    ++aNbWires;
  }
  for (TopExp_Explorer anExplorer(aShape, TopAbs_EDGE); anExplorer.More(); anExplorer.Next())
  {
    ++aNbEdges;
  }
  EXPECT_EQ(1, aNbFaces);
  EXPECT_EQ(2, aNbWires);
  EXPECT_EQ(2, aNbEdges);
}

TEST(BRepFont_BuilderTest, NonBoundaryContoursAreExcluded)
{
  const std::optional<BRepFont_PlanarRegion> aRegion =
    BRepFont_Builder_Test::createSameWindingNestedRegion();
  ASSERT_TRUE(aRegion.has_value());
  ASSERT_EQ(4u, aRegion->Uses().Size());

  const TopoDS_Shape aShape =
    BRepFont_Builder::BuildGlyph(*aRegion, BRepFont_Builder::GlyphOptions{});
  ASSERT_FALSE(aShape.IsNull());
  int aNbEdges = 0;
  for (TopExp_Explorer anExplorer(aShape, TopAbs_EDGE); anExplorer.More(); anExplorer.Next())
  {
    ++aNbEdges;
  }
  EXPECT_EQ(4, aNbEdges);
}

TEST(BRepFont_BuilderTest, CompositeSingleStrokeBuildsOneOpenEdge)
{
  const std::optional<BRepFont_PlanarRegion> aRegion =
    BRepFont_Builder_Test::createSingleStrokeRegion();
  ASSERT_TRUE(aRegion.has_value());

  BRepFont_Builder::GlyphOptions anOptions;
  anOptions.ToConcatenateContours = true;
  const TopoDS_Shape aShape       = BRepFont_Builder::BuildGlyph(*aRegion, anOptions);
  ASSERT_FALSE(aShape.IsNull());
  EXPECT_TRUE(BRepCheck_Analyzer(aShape).IsValid());

  int aNbFaces = 0;
  int aNbWires = 0;
  int aNbEdges = 0;
  for (TopExp_Explorer anExplorer(aShape, TopAbs_FACE); anExplorer.More(); anExplorer.Next())
  {
    ++aNbFaces;
  }
  for (TopExp_Explorer anExplorer(aShape, TopAbs_WIRE); anExplorer.More(); anExplorer.Next())
  {
    ++aNbWires;
  }
  for (TopExp_Explorer anExplorer(aShape, TopAbs_EDGE); anExplorer.More(); anExplorer.Next())
  {
    ++aNbEdges;
  }
  EXPECT_EQ(0, aNbFaces);
  EXPECT_EQ(1, aNbWires);
  EXPECT_EQ(1, aNbEdges);
}

TEST(BRepFont_BuilderTest, BuildsMarkedOverlapAsSeparateFaces)
{
  const std::optional<BRepFont_PlanarRegion> aRegion =
    BRepFont_Builder_Test::createOverlappingRegion();
  ASSERT_TRUE(aRegion.has_value());
  BRepFont_Builder::GlyphOptions anOptions;
  anOptions.Size            = 1000.0;
  const TopoDS_Shape aShape = BRepFont_Builder::BuildGlyph(*aRegion, anOptions);
  ASSERT_FALSE(aShape.IsNull());
  EXPECT_TRUE(BRepCheck_Analyzer(aShape).IsValid());

  GProp_GProps aProperties;
  BRepGProp::SurfaceProperties(aShape, aProperties);
  EXPECT_NEAR(200.0, aProperties.Mass(), 1.0e-7);

  int aNbFaces = 0;
  for (TopExp_Explorer anExplorer(aShape, TopAbs_FACE); anExplorer.More(); anExplorer.Next())
  {
    ++aNbFaces;
  }
  EXPECT_EQ(2, aNbFaces);
}

TEST(BRepFont_BuilderTest, RejectsNonFiniteOptions)
{
  const std::optional<BRepFont_PlanarRegion> aRegion = BRepFont_Builder_Test::createTriangle();
  ASSERT_TRUE(aRegion.has_value());

  BRepFont_Builder::GlyphOptions anOptions;
  anOptions.Size = std::numeric_limits<double>::infinity();
  EXPECT_TRUE(BRepFont_Builder::BuildGlyph(*aRegion, anOptions).IsNull());
}

TEST(BRepFont_BuilderTest, RejectsFiniteCoordinateOverflow)
{
  const std::optional<BRepFont_PlanarRegion> aRegion = BRepFont_Builder_Test::createTriangle();
  ASSERT_TRUE(aRegion.has_value());

  BRepFont_Builder::GlyphOptions anOptions;
  anOptions.Size = std::numeric_limits<double>::max();
  EXPECT_TRUE(BRepFont_Builder::BuildGlyph(*aRegion, anOptions).IsNull());
  anOptions.ToConcatenateContours = true;
  EXPECT_TRUE(BRepFont_Builder::BuildGlyph(*aRegion, anOptions).IsNull());
}

TEST(BRepFont_BuilderGraphTest, AddsFaceToExistingGraph)
{
  const std::optional<BRepFont_PlanarRegion> aRegion = BRepFont_Builder_Test::createTriangle();
  ASSERT_TRUE(aRegion.has_value());

  BRepGraph                            aGraph;
  NCollection_Array1<BRepGraph_NodeId> anEmptyChildren;
  const BRepGraph_CompoundId anExisting = aGraph.Editor().Compounds().Add(anEmptyChildren);
  ASSERT_TRUE(anExisting.IsValid());

  BRepFont_Builder::GlyphOptions anOptions;
  anOptions.Size               = 10.0;
  anOptions.Tolerance          = 1.0e-7;
  const BRepGraph_NodeId aRoot = BRepFont_Builder::AddGlyph(aGraph, *aRegion, anOptions);
  EXPECT_TRUE(aRoot.IsValid());
  EXPECT_EQ(1u, aGraph.Topo().Compounds().NbActive());
  EXPECT_EQ(1u, aGraph.Topo().Faces().NbActive());
}

TEST(BRepFont_BuilderGraphTest, RejectsNonFiniteOptionsWithoutChangingGraph)
{
  const std::optional<BRepFont_PlanarRegion> aRegion = BRepFont_Builder_Test::createTriangle();
  ASSERT_TRUE(aRegion.has_value());

  BRepGraph                      aGraph;
  BRepFont_Builder::GlyphOptions anOptions;
  anOptions.Tolerance = std::numeric_limits<double>::infinity();
  EXPECT_FALSE(BRepFont_Builder::AddGlyph(aGraph, *aRegion, anOptions).IsValid());
  EXPECT_EQ(0u, aGraph.Topo().Vertices().NbActive());
}

TEST(BRepFont_BuilderGraphTest, SingleStrokeAddsWireWithoutFace)
{
  const std::optional<BRepFont_PlanarRegion> aRegion =
    BRepFont_Builder_Test::createSingleStrokeRegion();
  ASSERT_TRUE(aRegion.has_value());

  BRepGraph              aGraph;
  const BRepGraph_NodeId aRoot =
    BRepFont_Builder::AddGlyph(aGraph, *aRegion, BRepFont_Builder::GlyphOptions{});
  ASSERT_TRUE(aRoot.IsValid());
  EXPECT_EQ(2u, aGraph.Topo().Edges().NbActive());
  EXPECT_EQ(1u, aGraph.Topo().Wires().NbActive());
  EXPECT_EQ(0u, aGraph.Topo().Faces().NbActive());
}

TEST(BRepFont_BuilderGraphTest, AddsMarkedOverlapAsSeparateFaces)
{
  const std::optional<BRepFont_PlanarRegion> aRegion =
    BRepFont_Builder_Test::createOverlappingRegion();
  ASSERT_TRUE(aRegion.has_value());

  BRepGraph              aGraph;
  const BRepGraph_NodeId aRoot =
    BRepFont_Builder::AddGlyph(aGraph, *aRegion, BRepFont_Builder::GlyphOptions{});
  EXPECT_TRUE(aRoot.IsValid());
  EXPECT_EQ(2u, aGraph.Topo().Faces().NbActive());
}

TEST(BRepFont_BuilderGraphTest, RejectsFiniteCoordinateOverflow)
{
  const std::optional<BRepFont_PlanarRegion> aRegion = BRepFont_Builder_Test::createTriangle();
  ASSERT_TRUE(aRegion.has_value());

  BRepGraph                      aGraph;
  BRepFont_Builder::GlyphOptions anOptions;
  anOptions.Size = std::numeric_limits<double>::max();
  EXPECT_FALSE(BRepFont_Builder::AddGlyph(aGraph, *aRegion, anOptions).IsValid());
  EXPECT_EQ(0u, aGraph.Topo().Vertices().NbActive());
}

namespace BRepFont_BuilderText_Test
{
std::optional<BRepFont_Builder::TextPlan> createRepeatedGlyphPlan()
{
  occ::handle<Font_FTFont>  aFont = new Font_FTFont();
  const Font_FTFont::Params aParams{72, 4800};
  if (!aFont->Init(Font_FontMgr::EmbedFallbackFont(), "Embedded fallback", aParams, 0))
  {
    return std::nullopt;
  }

  const std::optional<Font_GlyphOutline::Glyph> aGlyph = aFont->ResolveGlyph(U'I');
  if (!aGlyph.has_value())
  {
    return std::nullopt;
  }
  const std::optional<Font_GlyphOutline> anOutline = aFont->LoadGlyphOutline(*aGlyph);
  if (!anOutline.has_value())
  {
    return std::nullopt;
  }

  BRepFont_Regularizer::Options aRegularizerOptions;
  aRegularizerOptions.Tolerance = 0.01;
  BRepFont_Regularizer::Result aRegionResult =
    BRepFont_Regularizer().Build(*anOutline, aRegularizerOptions);
  if (!aRegionResult.IsDone() || !aRegionResult.PlanarRegion().has_value())
  {
    return std::nullopt;
  }

  std::shared_ptr<const BRepFont_PlanarRegion> aRegion =
    std::make_shared<BRepFont_PlanarRegion>(std::move(*aRegionResult.ChangePlanarRegion()));
  NCollection_LinearVector<std::shared_ptr<const BRepFont_PlanarRegion>> aRegions;
  aRegions.Append(aRegion);
  NCollection_LinearVector<BRepFont_Builder::TextPlan::Item> anItems;
  anItems.EmplaceAppend(0, gp_XY(0.0, 0.0));
  anItems.EmplaceAppend(0, gp_XY(20.0, 0.0));
  return BRepFont_Builder::CreateTextPlan(12.0, 1.0e-7, std::move(aRegions), std::move(anItems));
}

std::optional<BRepFont_Builder::TextPlan> createEmptyPlan()
{
  NCollection_LinearVector<std::shared_ptr<const BRepFont_PlanarRegion>> aRegions;
  NCollection_LinearVector<BRepFont_Builder::TextPlan::Item>             anItems;
  return BRepFont_Builder::CreateTextPlan(12.0, 1.0e-7, std::move(aRegions), std::move(anItems));
}
} // namespace BRepFont_BuilderText_Test

TEST(BRepFont_BuilderTextTest, EmptyTextDoesNotCreateTopology)
{
  const std::optional<BRepFont_Builder::TextPlan> aPlan =
    BRepFont_BuilderText_Test::createEmptyPlan();
  ASSERT_TRUE(aPlan.has_value());
  EXPECT_TRUE(BRepFont_Builder::BuildText(*aPlan).IsNull());

  BRepGraph aGraph;
  EXPECT_FALSE(BRepFont_Builder::AddText(aGraph, *aPlan).IsValid());
  EXPECT_EQ(0u, aGraph.Topo().Compounds().NbActive());
}

TEST(BRepFont_BuilderTextTest, InvalidTextPlanOptionsAreRejected)
{
  NCollection_LinearVector<std::shared_ptr<const BRepFont_PlanarRegion>> aRegions;
  NCollection_LinearVector<BRepFont_Builder::TextPlan::Item>             anItems;
  EXPECT_FALSE(
    BRepFont_Builder::CreateTextPlan(0.0, 1.0e-7, std::move(aRegions), std::move(anItems))
      .has_value());
}

TEST(BRepFont_BuilderTextTest, NonFinitePenIsRejectedWithoutChangingGraph)
{
  const std::optional<BRepFont_Builder::TextPlan> aPlan =
    BRepFont_BuilderText_Test::createRepeatedGlyphPlan();
  if (!aPlan.has_value())
  {
    GTEST_SKIP() << "FreeType or the embedded fallback glyph is unavailable";
  }
  BRepFont_Builder::TextOptions anOptions;
  anOptions.Pen.SetLocation(gp_Pnt(std::numeric_limits<double>::infinity(), 0.0, 0.0));
  EXPECT_TRUE(BRepFont_Builder::BuildText(*aPlan, anOptions).IsNull());
  BRepGraph aGraph;
  EXPECT_FALSE(BRepFont_Builder::AddText(aGraph, *aPlan, anOptions).IsValid());
  EXPECT_EQ(0u, aGraph.Topo().Vertices().NbActive());
}

TEST(BRepFont_BuilderTextTest, RepeatedGlyphUsesLocatedTopoDSOccurrences)
{
  const std::optional<BRepFont_Builder::TextPlan> aPlan =
    BRepFont_BuilderText_Test::createRepeatedGlyphPlan();
  if (!aPlan.has_value())
  {
    GTEST_SKIP() << "FreeType or the embedded fallback glyph is unavailable";
  }

  BRepFont_Builder::TextOptions anOptions;
  anOptions.Pen             = gp_Ax3(gp_Pnt(3.0, 4.0, 5.0), gp::DZ(), gp::DX());
  const TopoDS_Shape aShape = BRepFont_Builder::BuildText(*aPlan, anOptions);
  ASSERT_FALSE(aShape.IsNull());
  ASSERT_EQ(2, aShape.NbChildren());
  TopoDS_Iterator    anIterator(aShape);
  const TopoDS_Shape aFirst = anIterator.Value();
  anIterator.Next();
  const TopoDS_Shape aSecond = anIterator.Value();
  EXPECT_TRUE(aFirst.IsPartner(aSecond));
  EXPECT_FALSE(aFirst.Location().IsEqual(aSecond.Location()));
  const gp_Pnt aFirstOrigin = gp_Pnt(0.0, 0.0, 0.0).Transformed(aFirst.Location().Transformation());
  EXPECT_DOUBLE_EQ(3.0, aFirstOrigin.X());
  EXPECT_DOUBLE_EQ(4.0, aFirstOrigin.Y());
  EXPECT_DOUBLE_EQ(5.0, aFirstOrigin.Z());
}

TEST(BRepFont_BuilderTextTest, RepeatedGlyphSharesOneGraphDefinition)
{
  const std::optional<BRepFont_Builder::TextPlan> aPlan =
    BRepFont_BuilderText_Test::createRepeatedGlyphPlan();
  if (!aPlan.has_value())
  {
    GTEST_SKIP() << "FreeType or the embedded fallback glyph is unavailable";
  }

  BRepGraph                            aGraph;
  NCollection_Array1<BRepGraph_NodeId> anEmptyChildren;
  const BRepGraph_CompoundId anExisting = aGraph.Editor().Compounds().Add(anEmptyChildren);
  ASSERT_TRUE(anExisting.IsValid());
  const BRepGraph_NodeId aRoot = BRepFont_Builder::AddText(aGraph, *aPlan);
  ASSERT_TRUE(aRoot.IsValid());
  EXPECT_EQ(2u, aGraph.Topo().Compounds().NbActive());
  const NCollection_LinearVector<BRepGraph_ChildRefId>& aReferences =
    aGraph.Refs().Children().IdsOf(BRepGraph_CompoundId(aRoot));
  ASSERT_EQ(2u, aReferences.Size());
  const BRepGraph_NodeId aFirstDef = aGraph.Refs().Gen().ChildNode(aReferences[0]);
  const BRepGraph_NodeId aLastDef  = aGraph.Refs().Gen().ChildNode(aReferences[1]);
  EXPECT_EQ(aFirstDef, aLastDef);
  EXPECT_TRUE(aGraph.Refs().Gen().LocalLocation(aReferences[0]).IsIdentity());
  EXPECT_FALSE(aGraph.Refs().Gen().LocalLocation(aReferences[1]).IsIdentity());
}
