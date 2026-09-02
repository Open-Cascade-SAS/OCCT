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

#include <BRepGraph.hxx>
#include <BRepGraph_CopyRemap.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <BRepGraphInc_Definition.hxx>

#include <BRepPrimAPI_MakeBox.hxx>
#include <gtest/gtest.h>

namespace
{
BRepGraph makeBoxGraph()
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  return aGraph;
}

BRepGraph_ItemId item(const BRepGraph_NodeId theNode)
{
  return BRepGraph_ItemId(theNode);
}
} // namespace

TEST(BRepGraph_CopyRemapTest, IdentityMappingReturnsSourceItemsAndKeepsItemsMapEmpty)
{
  BRepGraph aSource = makeBoxGraph();
  BRepGraph aTarget = makeBoxGraph();

  const BRepGraph_CopyRemap aRemap(aSource,
                                   aTarget,
                                   BRepGraph_CopyRemap::MappingKind::Identity,
                                   BRepGraph_CopyRemap::Mode::Copy);

  const BRepGraph_ItemId aSourceFace = item(BRepGraph_FaceId::Start());
  ASSERT_TRUE(aSourceFace.IsValid());

  EXPECT_EQ(aRemap.CopyMode(), BRepGraph_CopyRemap::Mode::Copy);
  EXPECT_FALSE(aRemap.IsCompact());
  EXPECT_EQ(&aRemap.SourceGraph(), &aSource);
  EXPECT_EQ(&aRemap.TargetGraph(), &aTarget);
  EXPECT_EQ(&aRemap.TargetGraphConst(), &aTarget);
  EXPECT_TRUE(aRemap.Items().IsEmpty());

  EXPECT_EQ(aRemap.TargetItem(aSourceFace), aSourceFace);
  EXPECT_EQ(aRemap.TargetItemOrInvalid(aSourceFace), aSourceFace);
  EXPECT_TRUE(aRemap.HasTargetItem(aSourceFace));
  EXPECT_FALSE(aRemap.HasTargetItem(BRepGraph_ItemId()));

  EXPECT_EQ(aRemap.SourceUID(aSourceFace), aSource.UIDs().Of(aSourceFace));
  EXPECT_EQ(aRemap.TargetUID(aSourceFace), aTarget.UIDs().Of(aSourceFace));
  EXPECT_EQ(aRemap.TargetUIDFromSource(aSourceFace), aTarget.UIDs().Of(aSourceFace));
}

TEST(BRepGraph_CopyRemapTest, ExplicitMappingResolvesMappedItemsAndUIDs)
{
  BRepGraph aSource = makeBoxGraph();
  BRepGraph aTarget = makeBoxGraph();

  const BRepGraph_ItemId aSourceFace = item(BRepGraph_FaceId::Start());
  const BRepGraph_ItemId aTargetFace = item(BRepGraph_FaceId(1));
  const BRepGraph_ItemId aSourceEdge = item(BRepGraph_EdgeId::Start());
  const BRepGraph_ItemId aTargetEdge = item(BRepGraph_EdgeId::Start());
  const BRepGraph_ItemId anUnmappedFace = item(BRepGraph_FaceId(2));
  ASSERT_TRUE(aSourceFace.IsValid());
  ASSERT_TRUE(aTargetFace.IsValid());
  ASSERT_TRUE(aSourceEdge.IsValid());
  ASSERT_TRUE(aTargetEdge.IsValid());
  ASSERT_TRUE(anUnmappedFace.IsValid());

  BRepGraph_CopyRemap::ItemMap anItemMap;
  anItemMap.Reserve(2);
  anItemMap.Bind(aSourceFace, aTargetFace);
  anItemMap.Bind(aSourceEdge, aTargetEdge);

  const BRepGraph_CopyRemap aRemap(aSource,
                                   aTarget,
                                   anItemMap,
                                   BRepGraph_CopyRemap::Mode::Compact);

  EXPECT_EQ(aRemap.CopyMode(), BRepGraph_CopyRemap::Mode::Compact);
  EXPECT_TRUE(aRemap.IsCompact());
  EXPECT_EQ(aRemap.Items().Extent(), 2);
  EXPECT_EQ(aRemap.TargetItem(aSourceFace), aTargetFace);
  EXPECT_EQ(aRemap.TargetItemOrInvalid(aSourceEdge), aTargetEdge);
  EXPECT_TRUE(aRemap.HasTargetItem(aSourceFace));
  EXPECT_FALSE(aRemap.HasTargetItem(anUnmappedFace));
  EXPECT_FALSE(aRemap.TargetItem(anUnmappedFace).IsValid());
  EXPECT_FALSE(aRemap.TargetItem(BRepGraph_ItemId()).IsValid());

  EXPECT_EQ(aRemap.SourceUID(aSourceFace), aSource.UIDs().Of(aSourceFace));
  EXPECT_EQ(aRemap.TargetUID(aTargetFace), aTarget.UIDs().Of(aTargetFace));
  EXPECT_EQ(aRemap.TargetUIDFromSource(aSourceFace), aTarget.UIDs().Of(aTargetFace));
  EXPECT_FALSE(aRemap.TargetUIDFromSource(anUnmappedFace).IsValid());
}

TEST(BRepGraph_CopyRemapTest, SubtreeMatchRejectsDivergentNodesWithEqualCounters)
{
  BRepGraph aSource = makeBoxGraph();
  BRepGraph aTarget = makeBoxGraph();

  const BRepGraph_FaceId aFace = BRepGraph_FaceId::Start();
  const BRepGraphInc::BaseDef* aSourceDef = aSource.Topo().Gen().TopoEntity(aFace);
  const BRepGraphInc::BaseDef* aTargetDef = aTarget.Topo().Gen().TopoEntity(aFace);
  ASSERT_NE(aSourceDef, nullptr);
  ASSERT_NE(aTargetDef, nullptr);
  ASSERT_EQ(aSourceDef->SubtreeGen, aTargetDef->SubtreeGen);
  ASSERT_NE(aSource.UIDs().GraphGUID(), aTarget.UIDs().GraphGUID());

  const BRepGraph_CopyRemap aRemap(aSource,
                                   aTarget,
                                   BRepGraph_CopyRemap::MappingKind::Identity,
                                   BRepGraph_CopyRemap::Mode::Copy,
                                   BRepGraph_CopyRemap::FreshnessPolicy::MatchTarget);

  EXPECT_FALSE(aRemap.HasSameSubtreeGeneration(aFace));
  EXPECT_FALSE(aRemap.IsSubtreeGenerationCompatible(aFace));
}
