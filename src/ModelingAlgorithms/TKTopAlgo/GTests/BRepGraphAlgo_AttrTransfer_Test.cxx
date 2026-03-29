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

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_CacheView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_TransientCache.hxx>
#include <BRepGraphAlgo_AttrTransfer.hxx>
#include <BRepGraphAlgo_Deduplicate.hxx>
#include <BRepGraphAlgo_Sewing.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_List.hxx>
#include <Precision.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Face.hxx>

#include <gtest/gtest.h>

namespace
{

const occ::handle<BRepGraph_CacheKind>& colorCacheKind()
{
  static const occ::handle<BRepGraph_CacheKind> THE_KIND =
    new BRepGraph_CacheKind(Standard_GUID("2f9b6a5c-1f2d-4a88-9c1c-7a0c16a10024"), "Color");
  return THE_KIND;
}

const occ::handle<BRepGraph_CacheKind>& labelCacheKind()
{
  static const occ::handle<BRepGraph_CacheKind> THE_KIND =
    new BRepGraph_CacheKind(Standard_GUID("2f9b6a5c-1f2d-4a88-9c1c-7a0c16a10025"), "Label");
  return THE_KIND;
}

occ::handle<BRepGraph_CacheValue> makeIntValue(int theValue)
{
  return new BRepGraph_TypedCacheValue<int>(theValue);
}

//! Find two adjacent faces from a shape (sharing a common edge).
bool findAdjacentFaces(const TopoDS_Shape& theShape, TopoDS_Face& theFace1, TopoDS_Face& theFace2)
{
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    anEdgeFaceMap;
  TopExp::MapShapesAndAncestors(theShape, TopAbs_EDGE, TopAbs_FACE, anEdgeFaceMap);
  for (int anEntryIdx = 1; anEntryIdx <= anEdgeFaceMap.Extent(); ++anEntryIdx)
  {
    const NCollection_List<TopoDS_Shape>& aFaces = anEdgeFaceMap(anEntryIdx);
    if (aFaces.Extent() == 2)
    {
      theFace1 = TopoDS::Face(aFaces.First());
      theFace2 = TopoDS::Face(aFaces.Last());
      return true;
    }
  }
  return false;
}

} // namespace

// --- FindNode / HasNode tests ---

TEST(BRepGraphAlgo_AttrTransferTest, FindNode_FaceFromBox)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  const TopoDS_Face& aFace = TopoDS::Face(anExp.Current());

  const BRepGraph_NodeId aNodeId = aGraph.Shapes().FindNode(aFace);
  EXPECT_TRUE(aNodeId.IsValid());
  EXPECT_EQ(aNodeId.NodeKind, BRepGraph_NodeId::Kind::Face);
}

TEST(BRepGraphAlgo_AttrTransferTest, FindNode_UnknownShape)
{
  BRepPrimAPI_MakeBox aBoxMaker1(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox1 = aBoxMaker1.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox1);
  ASSERT_TRUE(aGraph.IsDone());

  // A completely different shape not in the graph.
  BRepPrimAPI_MakeBox aBoxMaker2(5.0, 5.0, 5.0);
  const TopoDS_Shape& aBox2 = aBoxMaker2.Shape();

  TopExp_Explorer anExp(aBox2, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());

  const BRepGraph_NodeId aNodeId = aGraph.Shapes().FindNode(anExp.Current());
  EXPECT_FALSE(aNodeId.IsValid());
}

TEST(BRepGraphAlgo_AttrTransferTest, HasNode_BasicCheck)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  EXPECT_TRUE(aGraph.Shapes().HasNode(anExp.Current()));

  // Unknown shape.
  BRepPrimAPI_MakeBox aBoxMaker2(1.0, 1.0, 1.0);
  TopExp_Explorer     anExp2(aBoxMaker2.Shape(), TopAbs_FACE);
  ASSERT_TRUE(anExp2.More());
  EXPECT_FALSE(aGraph.Shapes().HasNode(anExp2.Current()));

  // Null shape.
  EXPECT_FALSE(aGraph.Shapes().HasNode(TopoDS_Shape()));
}

// --- CacheKinds tests ---

TEST(BRepGraphAlgo_AttrTransferTest, CacheKinds_Empty)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId                                       aFaceNode(BRepGraph_NodeId::Kind::Face, 0);
  const NCollection_Vector<occ::handle<BRepGraph_CacheKind>> aKinds =
    aGraph.Cache().CacheKinds(aFaceNode);
  EXPECT_TRUE(aKinds.IsEmpty());
}

TEST(BRepGraphAlgo_AttrTransferTest, CacheKinds_AfterSet)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId aFaceNode(BRepGraph_NodeId::Kind::Face, 0);

  aGraph.Cache().Set(aFaceNode, colorCacheKind(), makeIntValue(42));
  aGraph.Cache().Set(aFaceNode, labelCacheKind(), makeIntValue(99));

  const NCollection_Vector<occ::handle<BRepGraph_CacheKind>> aKinds =
    aGraph.Cache().CacheKinds(aFaceNode);
  EXPECT_EQ(aKinds.Length(), 2);

  // Verify both keys are present.
  bool hasColor = false;
  bool hasLabel = false;
  for (int anIdx = 0; anIdx < aKinds.Length(); ++anIdx)
  {
    if (!aKinds.Value(anIdx).IsNull() && aKinds.Value(anIdx)->ID() == colorCacheKind()->ID())
      hasColor = true;
    if (!aKinds.Value(anIdx).IsNull() && aKinds.Value(anIdx)->ID() == labelCacheKind()->ID())
      hasLabel = true;
  }
  EXPECT_TRUE(hasColor);
  EXPECT_TRUE(hasLabel);
}

TEST(BRepGraphAlgo_AttrTransferTest, CacheKinds_FreshNode_Empty)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // A node with no attributes set should return empty keys.
  const BRepGraph_NodeId                                       aVertexNode(BRepGraph_NodeId::Kind::Vertex, 0);
  const NCollection_Vector<occ::handle<BRepGraph_CacheKind>> aKinds =
    aGraph.Cache().CacheKinds(aVertexNode);
  EXPECT_TRUE(aKinds.IsEmpty());
}

// --- AttrTransfer tests ---

TEST(BRepGraphAlgo_AttrTransferTest, AttrTransfer_NoHistory)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // No operations performed, so history is empty.
  const BRepGraphAlgo_AttrTransfer::Result aResult = BRepGraphAlgo_AttrTransfer::Perform(aGraph);
  EXPECT_EQ(aResult.NbTransferred, 0);
  EXPECT_EQ(aResult.NbSkipped, 0);
}

TEST(BRepGraphAlgo_AttrTransferTest, AttrTransfer_NoOverwrite)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  // Find two adjacent faces.
  TopoDS_Face aOrigF1, aOrigF2;
  ASSERT_TRUE(findAdjacentFaces(aBox, aOrigF1, aOrigF2));

  // Copy each face independently to get disconnected edges.
  BRepBuilderAPI_Copy aCopy1(aOrigF1, true);
  BRepBuilderAPI_Copy aCopy2(aOrigF2, true);

  BRep_Builder    aBB;
  TopoDS_Compound aSewCompound;
  aBB.MakeCompound(aSewCompound);
  aBB.Add(aSewCompound, aCopy1.Shape());
  aBB.Add(aSewCompound, aCopy2.Shape());

  BRepGraphAlgo_Sewing::Options aSewOpts;
  aSewOpts.Tolerance = 1.0e-04;

  BRepGraph aSewGraph;
  aSewGraph.Build(aSewCompound);
  ASSERT_TRUE(aSewGraph.IsDone());

  BRepGraphAlgo_Sewing::Result aSewResult = BRepGraphAlgo_Sewing::Perform(aSewGraph, aSewOpts);
  ASSERT_TRUE(aSewResult.IsDone);

  // Get graph from sewing result and check history is not empty.
  // The sewing graph merges edges, producing history records.
  const int aNbRecords = aSewGraph.History().NbRecords();
  if (aNbRecords == 0)
  {
    // If no history records, transfer is trivially empty.
    return;
  }

  // Create a fresh graph from the sewing result to test attribute transfer
  // on a graph with manually-added history.
  // For this test, we verify the no-overwrite logic directly.
  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // Simulate a history record: face 0 -> face 1.
  const BRepGraph_NodeId aFace0(BRepGraph_NodeId::Kind::Face, 0);
  const BRepGraph_NodeId aFace1(BRepGraph_NodeId::Kind::Face, 1);

  aGraph.Cache().Set(aFace0, colorCacheKind(), makeIntValue(100));
  aGraph.Cache().Set(aFace1, colorCacheKind(), makeIntValue(200));

  NCollection_Vector<BRepGraph_NodeId> aRepl;
  aRepl.Append(aFace1);
  aGraph.History().Record(TCollection_AsciiString("Test:Replace"), aFace0, aRepl);

  // Transfer with OverwriteExisting=false.
  BRepGraphAlgo_AttrTransfer::Options anOpts;
  anOpts.OverwriteExisting = false;
  const BRepGraphAlgo_AttrTransfer::Result aResult =
    BRepGraphAlgo_AttrTransfer::Perform(aGraph, anOpts);

  EXPECT_EQ(aResult.NbSkipped, 1);
  EXPECT_EQ(aResult.NbTransferred, 0);

  // Target still has its original value.
  Handle(BRepGraph_TypedCacheValue<int>) aTargetAttr =
    Handle(BRepGraph_TypedCacheValue<int>)::DownCast(aGraph.Cache().Get(aFace1, colorCacheKind()));
  ASSERT_FALSE(aTargetAttr.IsNull());
  EXPECT_EQ(aTargetAttr->UncheckedValue(), 200);
}

TEST(BRepGraphAlgo_AttrTransferTest, AttrTransfer_Overwrite)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId aFace0(BRepGraph_NodeId::Kind::Face, 0);
  const BRepGraph_NodeId aFace1(BRepGraph_NodeId::Kind::Face, 1);

  aGraph.Cache().Set(aFace0, colorCacheKind(), makeIntValue(100));
  aGraph.Cache().Set(aFace1, colorCacheKind(), makeIntValue(200));

  NCollection_Vector<BRepGraph_NodeId> aRepl;
  aRepl.Append(aFace1);
  aGraph.History().Record(TCollection_AsciiString("Test:Replace"), aFace0, aRepl);

  // Transfer with OverwriteExisting=true.
  BRepGraphAlgo_AttrTransfer::Options anOpts;
  anOpts.OverwriteExisting = true;
  const BRepGraphAlgo_AttrTransfer::Result aResult =
    BRepGraphAlgo_AttrTransfer::Perform(aGraph, anOpts);

  EXPECT_EQ(aResult.NbTransferred, 1);
  EXPECT_EQ(aResult.NbSkipped, 0);

  // Target now has the source value.
  Handle(BRepGraph_TypedCacheValue<int>) aTargetAttr =
    Handle(BRepGraph_TypedCacheValue<int>)::DownCast(aGraph.Cache().Get(aFace1, colorCacheKind()));
  ASSERT_FALSE(aTargetAttr.IsNull());
  EXPECT_EQ(aTargetAttr->UncheckedValue(), 100);
}

TEST(BRepGraphAlgo_AttrTransferTest, AttrTransfer_MultipleKeys)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId aFace0(BRepGraph_NodeId::Kind::Face, 0);
  const BRepGraph_NodeId aFace1(BRepGraph_NodeId::Kind::Face, 1);

  aGraph.Cache().Set(aFace0, colorCacheKind(), makeIntValue(10));
  aGraph.Cache().Set(aFace0, labelCacheKind(), makeIntValue(20));

  NCollection_Vector<BRepGraph_NodeId> aRepl;
  aRepl.Append(aFace1);
  aGraph.History().Record(TCollection_AsciiString("Test:Replace"), aFace0, aRepl);

  const BRepGraphAlgo_AttrTransfer::Result aResult = BRepGraphAlgo_AttrTransfer::Perform(aGraph);
  EXPECT_EQ(aResult.NbTransferred, 2);

  Handle(BRepGraph_TypedCacheValue<int>) aColorAttr =
    Handle(BRepGraph_TypedCacheValue<int>)::DownCast(aGraph.Cache().Get(aFace1, colorCacheKind()));
  Handle(BRepGraph_TypedCacheValue<int>) aLabelAttr =
    Handle(BRepGraph_TypedCacheValue<int>)::DownCast(aGraph.Cache().Get(aFace1, labelCacheKind()));
  ASSERT_FALSE(aColorAttr.IsNull());
  ASSERT_FALSE(aLabelAttr.IsNull());
  EXPECT_EQ(aColorAttr->UncheckedValue(), 10);
  EXPECT_EQ(aLabelAttr->UncheckedValue(), 20);
}

TEST(BRepGraphAlgo_AttrTransferTest, AttrTransfer_GeometryHistorySkipped)
{
  // Create a compound with two identical faces to trigger surface dedup.
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  TopExp_Explorer    anExp(aBox, TopAbs_FACE);
  const TopoDS_Shape aFace = anExp.Current();

  BRepBuilderAPI_Copy aCopy1(aFace, true);
  BRepBuilderAPI_Copy aCopy2(aFace, true);

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aCopy1.Shape());
  aBuilder.Add(aCompound, aCopy2.Shape());

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  // Attach an attribute to face 0.
  const BRepGraph_NodeId aFaceNode(BRepGraph_NodeId::Kind::Face, 0);
  aGraph.Cache().Set(aFaceNode, colorCacheKind(), makeIntValue(42));

  // Dedup creates geometry-level history (Surface -> Surface).
  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);

  // AttrTransfer should skip the geometry-level history records.
  const BRepGraphAlgo_AttrTransfer::Result aResult = BRepGraphAlgo_AttrTransfer::Perform(aGraph);
  EXPECT_EQ(aResult.NbTransferred, 0);

  // The face still has its attribute.
  Handle(BRepGraph_TypedCacheValue<int>) aRetrieved =
    Handle(BRepGraph_TypedCacheValue<int>)::DownCast(aGraph.Cache().Get(aFaceNode, colorCacheKind()));
  ASSERT_FALSE(aRetrieved.IsNull());
  EXPECT_EQ(aRetrieved->UncheckedValue(), 42);
}

TEST(BRepGraphAlgo_AttrTransferTest, AttrTransfer_DeletionRecordSkipped)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId aFace0(BRepGraph_NodeId::Kind::Face, 0);
  aGraph.Cache().Set(aFace0, colorCacheKind(), makeIntValue(100));

  // Record a deletion (empty replacements).
  NCollection_Vector<BRepGraph_NodeId> aEmptyRepl;
  aGraph.History().Record(TCollection_AsciiString("Test:Delete"), aFace0, aEmptyRepl);

  const BRepGraphAlgo_AttrTransfer::Result aResult = BRepGraphAlgo_AttrTransfer::Perform(aGraph);
  EXPECT_EQ(aResult.NbTransferred, 0);
  EXPECT_EQ(aResult.NbSkipped, 0);
}

// --- FindNode for post-operation (reconstructed) shapes ---

TEST(BRepGraphAlgo_AttrTransferTest, FindNode_ReconstructedShape)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  TopExp_Explorer    anExp(aBox, TopAbs_FACE);
  const TopoDS_Shape aFace = anExp.Current();

  BRepBuilderAPI_Copy aCopy1(aFace, true);
  BRepBuilderAPI_Copy aCopy2(aFace, true);

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aCopy1.Shape());
  aBuilder.Add(aCompound, aCopy2.Shape());

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  // Run dedup to mark faces as modified.
  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);

  // Reconstruct face 0 - this creates a new TShape.
  const BRepGraph_NodeId aFaceNode(BRepGraph_NodeId::Kind::Face, 0);
  const TopoDS_Shape     aReconstructed = aGraph.Shapes().Shape(aFaceNode);
  ASSERT_FALSE(aReconstructed.IsNull());

  // FindNode should resolve the reconstructed shape back to its NodeId.
  const BRepGraph_NodeId aFound = aGraph.Shapes().FindNode(aReconstructed);
  EXPECT_TRUE(aFound.IsValid());
  EXPECT_EQ(aFound, aFaceNode);
}

// --- Dedup AffectedDefs tests ---

TEST(BRepGraphAlgo_AttrTransferTest, Deduplicate_AffectedFaceDefs)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  TopExp_Explorer    anExp(aBox, TopAbs_FACE);
  const TopoDS_Shape aFace = anExp.Current();

  BRepBuilderAPI_Copy aCopy1(aFace, true);
  BRepBuilderAPI_Copy aCopy2(aFace, true);

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aCopy1.Shape());
  aBuilder.Add(aCompound, aCopy2.Shape());

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_EQ(aGraph.Topo().NbFaces(), 2);

  const BRepGraphAlgo_Deduplicate::Result aResult = BRepGraphAlgo_Deduplicate::Perform(aGraph);

  // Both faces shared an identical surface, but only the non-canonical one
  // (face whose surface was rewritten) appears in AffectedFaces.
  EXPECT_EQ(aResult.NbSurfaceRewrites, aResult.AffectedFaces.Length());
  EXPECT_GE(aResult.AffectedFaces.Length(), 1);
}

TEST(BRepGraphAlgo_AttrTransferTest, Deduplicate_AffectedEdgeDefs)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  TopExp_Explorer    anExp(aBox, TopAbs_FACE);
  const TopoDS_Shape aFace = anExp.Current();

  BRepBuilderAPI_Copy aCopy1(aFace, true);
  BRepBuilderAPI_Copy aCopy2(aFace, true);

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aCopy1.Shape());
  aBuilder.Add(aCompound, aCopy2.Shape());

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraphAlgo_Deduplicate::Result aResult = BRepGraphAlgo_Deduplicate::Perform(aGraph);

  EXPECT_EQ(aResult.NbCurveRewrites, aResult.AffectedEdges.Length());
}
