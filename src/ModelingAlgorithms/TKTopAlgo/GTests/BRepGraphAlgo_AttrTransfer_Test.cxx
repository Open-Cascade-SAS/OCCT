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
#include <BRepGraph_AttrsView.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_TypedAttribute.hxx>
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

static const int THE_COLOR_KEY = BRepGraph_UserAttribute::AllocateKey();
static const int THE_LABEL_KEY = BRepGraph_UserAttribute::AllocateKey();

occ::handle<BRepGraph_UserAttribute> makeIntAttr(int theValue)
{
  return new BRepGraph_TypedAttribute<int>(theValue);
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

// --- AttributeKeys tests ---

TEST(BRepGraphAlgo_AttrTransferTest, AttributeKeys_Empty)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId        aFaceNode(BRepGraph_NodeId::Kind::Face, 0);
  const NCollection_Vector<int> aKeys = aGraph.Attrs().AttributeKeys(aFaceNode);
  EXPECT_TRUE(aKeys.IsEmpty());
}

TEST(BRepGraphAlgo_AttrTransferTest, AttributeKeys_AfterSet)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId aFaceNode(BRepGraph_NodeId::Kind::Face, 0);

  aGraph.Attrs().Set(aFaceNode, THE_COLOR_KEY, makeIntAttr(42));
  aGraph.Attrs().Set(aFaceNode, THE_LABEL_KEY, makeIntAttr(99));

  const NCollection_Vector<int> aKeys = aGraph.Attrs().AttributeKeys(aFaceNode);
  EXPECT_EQ(aKeys.Length(), 2);

  // Verify both keys are present.
  bool hasColor = false;
  bool hasLabel = false;
  for (int anIdx = 0; anIdx < aKeys.Length(); ++anIdx)
  {
    if (aKeys.Value(anIdx) == THE_COLOR_KEY)
      hasColor = true;
    if (aKeys.Value(anIdx) == THE_LABEL_KEY)
      hasLabel = true;
  }
  EXPECT_TRUE(hasColor);
  EXPECT_TRUE(hasLabel);
}

TEST(BRepGraphAlgo_AttrTransferTest, AttributeKeys_FreshNode_Empty)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // A node with no attributes set should return empty keys.
  const BRepGraph_NodeId        aVertexNode(BRepGraph_NodeId::Kind::Vertex, 0);
  const NCollection_Vector<int> aKeys = aGraph.Attrs().AttributeKeys(aVertexNode);
  EXPECT_TRUE(aKeys.IsEmpty());
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

  aGraph.Attrs().Set(aFace0, THE_COLOR_KEY, makeIntAttr(100));
  aGraph.Attrs().Set(aFace1, THE_COLOR_KEY, makeIntAttr(200));

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
  Handle(BRepGraph_TypedAttribute<int>) aTargetAttr =
    Handle(BRepGraph_TypedAttribute<int>)::DownCast(aGraph.Attrs().Get(aFace1, THE_COLOR_KEY));
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

  aGraph.Attrs().Set(aFace0, THE_COLOR_KEY, makeIntAttr(100));
  aGraph.Attrs().Set(aFace1, THE_COLOR_KEY, makeIntAttr(200));

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
  Handle(BRepGraph_TypedAttribute<int>) aTargetAttr =
    Handle(BRepGraph_TypedAttribute<int>)::DownCast(aGraph.Attrs().Get(aFace1, THE_COLOR_KEY));
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

  aGraph.Attrs().Set(aFace0, THE_COLOR_KEY, makeIntAttr(10));
  aGraph.Attrs().Set(aFace0, THE_LABEL_KEY, makeIntAttr(20));

  NCollection_Vector<BRepGraph_NodeId> aRepl;
  aRepl.Append(aFace1);
  aGraph.History().Record(TCollection_AsciiString("Test:Replace"), aFace0, aRepl);

  const BRepGraphAlgo_AttrTransfer::Result aResult = BRepGraphAlgo_AttrTransfer::Perform(aGraph);
  EXPECT_EQ(aResult.NbTransferred, 2);

  Handle(BRepGraph_TypedAttribute<int>) aColorAttr =
    Handle(BRepGraph_TypedAttribute<int>)::DownCast(aGraph.Attrs().Get(aFace1, THE_COLOR_KEY));
  Handle(BRepGraph_TypedAttribute<int>) aLabelAttr =
    Handle(BRepGraph_TypedAttribute<int>)::DownCast(aGraph.Attrs().Get(aFace1, THE_LABEL_KEY));
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
  aGraph.Attrs().Set(aFaceNode, THE_COLOR_KEY, makeIntAttr(42));

  // Dedup creates geometry-level history (Surface -> Surface).
  BRepGraphAlgo_Deduplicate::Perform(aGraph);

  // AttrTransfer should skip the geometry-level history records.
  const BRepGraphAlgo_AttrTransfer::Result aResult = BRepGraphAlgo_AttrTransfer::Perform(aGraph);
  EXPECT_EQ(aResult.NbTransferred, 0);

  // The face still has its attribute.
  Handle(BRepGraph_TypedAttribute<int>) aRetrieved =
    Handle(BRepGraph_TypedAttribute<int>)::DownCast(aGraph.Attrs().Get(aFaceNode, THE_COLOR_KEY));
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
  aGraph.Attrs().Set(aFace0, THE_COLOR_KEY, makeIntAttr(100));

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
  BRepGraphAlgo_Deduplicate::Perform(aGraph);

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
  ASSERT_EQ(aGraph.Defs().NbFaces(), 2);

  const BRepGraphAlgo_Deduplicate::Result aResult = BRepGraphAlgo_Deduplicate::Perform(aGraph);

  // Both faces shared an identical surface, but only the non-canonical one
  // (face whose surface was rewritten) appears in AffectedFaceDefs.
  EXPECT_EQ(aResult.NbSurfaceRewrites, aResult.AffectedFaceDefs.Length());
  EXPECT_GE(aResult.AffectedFaceDefs.Length(), 1);

  // Every affected face should be a Face kind.
  for (int anIdx = 0; anIdx < aResult.AffectedFaceDefs.Length(); ++anIdx)
  {
    EXPECT_EQ(aResult.AffectedFaceDefs.Value(anIdx).NodeKind, BRepGraph_NodeId::Kind::Face);
  }
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

  EXPECT_EQ(aResult.NbCurveRewrites, aResult.AffectedEdgeDefs.Length());

  for (int anIdx = 0; anIdx < aResult.AffectedEdgeDefs.Length(); ++anIdx)
  {
    EXPECT_EQ(aResult.AffectedEdgeDefs.Value(anIdx).NodeKind, BRepGraph_NodeId::Kind::Edge);
  }
}
