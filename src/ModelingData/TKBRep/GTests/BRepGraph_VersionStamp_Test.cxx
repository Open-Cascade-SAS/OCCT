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
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_ItemId.hxx>
#include <BRepGraph_ItemUID.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_VersionStamp.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Geom_Surface.hxx>
#include <Standard_GUID.hxx>

#include <gtest/gtest.h>

class BRepGraph_VersionStampTest : public testing::Test
{
protected:
  void SetUp() override
  {
    BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
    const TopoDS_Shape& aBox = aBoxMaker.Shape();
    myGraph.Clear();
    [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes1 = myGraph.Shapes().Add(aBox);
    ASSERT_FALSE(myGraph.IsEmpty());
  }

  BRepGraph myGraph;
};

// --- VersionStamp struct tests ---

TEST(BRepGraph_VersionStampBasicTest, DefaultStamp_IsInvalid)
{
  const BRepGraph_VersionStamp aStamp;
  EXPECT_FALSE(aStamp.IsValid());
}

TEST_F(BRepGraph_VersionStampTest, StampOf_ValidNode_ReturnsValidStamp)
{
  const BRepGraph_FaceId aFaceId(0);
  ASSERT_TRUE(aFaceId.IsValid(myGraph.Topo().Faces().Nb()));

  const BRepGraph_VersionStamp aStamp = myGraph.UIDs().StampOf(aFaceId);
  EXPECT_TRUE(aStamp.IsValid());
  EXPECT_TRUE(aStamp.myNodeUID.IsValid());
  EXPECT_EQ(aStamp.myMutationGen, 0u);
  EXPECT_EQ(aStamp.myGeneration, myGraph.UIDs().Generation());
}

TEST_F(BRepGraph_VersionStampTest, StampOf_InvalidNode_ReturnsInvalid)
{
  const BRepGraph_VersionStamp aStamp = myGraph.UIDs().StampOf(BRepGraph_NodeId());
  EXPECT_FALSE(aStamp.IsValid());
}

// --- IsStale tests ---

TEST_F(BRepGraph_VersionStampTest, IsStale_UnmutatedNode_ReturnsFalse)
{
  const BRepGraph_VersionStamp aStamp = myGraph.UIDs().StampOf(BRepGraph_FaceId::Start());
  EXPECT_FALSE(myGraph.UIDs().IsStale(aStamp));
}

TEST_F(BRepGraph_VersionStampTest, IsStale_MutatedNode_ReturnsTrue)
{
  const BRepGraph_VersionStamp aStamp = myGraph.UIDs().StampOf(BRepGraph_FaceId::Start());

  // Mutate the face.
  myGraph.Editor().Faces().SetTolerance(
    BRepGraph_FaceId::Start(),
    BRepGraph_Tool::Face::Tolerance(myGraph, BRepGraph_FaceId::Start()) + 0.01);

  EXPECT_TRUE(myGraph.UIDs().IsStale(aStamp));
}

TEST_F(BRepGraph_VersionStampTest, IsStale_RemovedNode_ReturnsTrue)
{
  const BRepGraph_VersionStamp aStamp = myGraph.UIDs().StampOf(BRepGraph_FaceId::Start());

  myGraph.Editor().Gen().RemoveNode(BRepGraph_FaceId::Start());

  EXPECT_TRUE(myGraph.UIDs().IsStale(aStamp));
}

TEST_F(BRepGraph_VersionStampTest, IsStale_DifferentGeneration_ReturnsTrue)
{
  const BRepGraph_VersionStamp aStamp = myGraph.UIDs().StampOf(BRepGraph_FaceId::Start());

  // Rebuild the graph - generation changes.
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  myGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes2 =
    myGraph.Shapes().Add(aBoxMaker.Shape());
  ASSERT_FALSE(myGraph.IsEmpty());

  EXPECT_TRUE(myGraph.UIDs().IsStale(aStamp));
}

TEST_F(BRepGraph_VersionStampTest, IsStale_DeferredMode_TracksCorrectly)
{
  const BRepGraph_VersionStamp aStamp = myGraph.UIDs().StampOf(BRepGraph_EdgeId::Start());

  // Mutate in deferred mode.
  myGraph.Editor().BeginDeferredInvalidation();
  myGraph.Editor().Edges().SetTolerance(BRepGraph_EdgeId::Start(), 0.5);
  myGraph.Editor().EndDeferredInvalidation();

  EXPECT_TRUE(myGraph.UIDs().IsStale(aStamp));
}

TEST_F(BRepGraph_VersionStampTest, IsStale_InvalidStamp_ReturnsTrue)
{
  const BRepGraph_VersionStamp aStamp;
  EXPECT_TRUE(myGraph.UIDs().IsStale(aStamp));
}

// --- Identity and version comparison ---

TEST_F(BRepGraph_VersionStampTest, StampIdentity_SameNode_Equal)
{
  const BRepGraph_VersionStamp aStamp1 = myGraph.UIDs().StampOf(BRepGraph_FaceId::Start());
  const BRepGraph_VersionStamp aStamp2 = myGraph.UIDs().StampOf(BRepGraph_FaceId::Start());
  EXPECT_EQ(aStamp1, aStamp2);
}

TEST_F(BRepGraph_VersionStampTest, StampIdentity_DifferentNodes_NotEqual)
{
  const BRepGraph_VersionStamp aStamp1 = myGraph.UIDs().StampOf(BRepGraph_FaceId::Start());
  const BRepGraph_VersionStamp aStamp2 = myGraph.UIDs().StampOf(BRepGraph_FaceId(1));
  EXPECT_NE(aStamp1, aStamp2);
}

TEST_F(BRepGraph_VersionStampTest, IsSameItem_SameVersion_ReturnsTrue)
{
  const BRepGraph_VersionStamp aStamp1 = myGraph.UIDs().StampOf(BRepGraph_FaceId::Start());
  const BRepGraph_VersionStamp aStamp2 = myGraph.UIDs().StampOf(BRepGraph_FaceId::Start());
  EXPECT_TRUE(aStamp1.IsSameItem(aStamp2));
}

TEST_F(BRepGraph_VersionStampTest, IsSameItem_DifferentVersion_StillSameItem)
{
  const BRepGraph_VersionStamp aStampBefore = myGraph.UIDs().StampOf(BRepGraph_FaceId::Start());

  myGraph.Editor().Faces().SetTolerance(
    BRepGraph_FaceId::Start(),
    BRepGraph_Tool::Face::Tolerance(myGraph, BRepGraph_FaceId::Start()) + 0.01);

  const BRepGraph_VersionStamp aStampAfter = myGraph.UIDs().StampOf(BRepGraph_FaceId::Start());

  // Full equality fails (different MutationGen).
  EXPECT_NE(aStampBefore, aStampAfter);
  // But they refer to the same item identity.
  EXPECT_TRUE(aStampBefore.IsSameItem(aStampAfter));
}

TEST_F(BRepGraph_VersionStampTest, StampOf_AssemblyNodes_WorksForProductsAndOccurrences)
{
  // Box graph auto-creates a root Product.
  ASSERT_GT(myGraph.Topo().Products().Nb(), 0);

  const BRepGraph_VersionStamp aProdStamp = myGraph.UIDs().StampOf(BRepGraph_ProductId::Start());
  EXPECT_TRUE(aProdStamp.IsValid());
  EXPECT_FALSE(myGraph.UIDs().IsStale(aProdStamp));
}

TEST_F(BRepGraph_VersionStampTest, GenericItemUID_NodeAndReferenceItems_RoundTrip)
{
  const BRepGraph_FaceId  aFaceId = BRepGraph_FaceId::Start();
  const BRepGraph_ItemId  aFaceItem(aFaceId);
  const BRepGraph_ItemUID aFaceUID = myGraph.UIDs().Of(aFaceItem);
  ASSERT_TRUE(aFaceUID.IsValid());
  EXPECT_TRUE(aFaceUID.IsNode());
  EXPECT_EQ(aFaceUID.NodeKind(), BRepGraph_NodeId::Kind::Face);
  EXPECT_EQ(myGraph.UIDs().ItemIdFrom(aFaceUID), aFaceItem);
  EXPECT_TRUE(myGraph.UIDs().Has(aFaceUID));

  const BRepGraph_VersionStamp aFaceStamp = myGraph.UIDs().StampOf(aFaceItem);
  EXPECT_TRUE(aFaceStamp.IsValid());
  EXPECT_TRUE(aFaceStamp.IsNodeStamp());
  EXPECT_EQ(aFaceStamp.ItemUID(), aFaceUID);

  const BRepGraph_FaceRefId aFaceRefId = BRepGraph_FaceRefId::Start();
  const BRepGraph_ItemId    aFaceRefItem(aFaceRefId);
  const BRepGraph_ItemUID   aFaceRefUID = myGraph.UIDs().Of(aFaceRefItem);
  ASSERT_TRUE(aFaceRefUID.IsValid());
  EXPECT_TRUE(aFaceRefUID.IsReference());
  EXPECT_EQ(aFaceRefUID.RefKind(), BRepGraph_RefId::Kind::Face);
  EXPECT_EQ(myGraph.UIDs().ItemIdFrom(aFaceRefUID), aFaceRefItem);
  EXPECT_TRUE(myGraph.UIDs().Has(aFaceRefUID));

  const BRepGraph_VersionStamp aFaceRefStamp = myGraph.UIDs().StampOf(aFaceRefItem);
  EXPECT_TRUE(aFaceRefStamp.IsValid());
  EXPECT_TRUE(aFaceRefStamp.IsRefStamp());
  EXPECT_EQ(aFaceRefStamp.ItemUID(), aFaceRefUID);
}

TEST_F(BRepGraph_VersionStampTest, StampOf_RemovedUse_ReturnsInvalidUntilReused)
{
  const BRepGraph_FaceId           aFaceId = BRepGraph_FaceId::Start();
  const BRepGraph_FaceSurfaceRepId aRepId = myGraph.Topo().Faces().Definition(aFaceId).SurfaceRepId;
  ASSERT_TRUE(aRepId.IsValid());

  const occ::handle<Geom_Surface> aSurface = BRepGraph_Tool::Face::Surface(myGraph, aFaceId);
  ASSERT_FALSE(aSurface.IsNull());
  EXPECT_TRUE(myGraph.UIDs().StampOf(aRepId).IsValid());

  myGraph.Editor().Faces().ClearSurface(aFaceId);
  EXPECT_FALSE(myGraph.UIDs().StampOf(aRepId).IsValid());

  myGraph.Editor().Faces().SetSurface(aFaceId, aSurface);
  EXPECT_EQ(myGraph.Topo().Faces().Definition(aFaceId).SurfaceRepId, aRepId);
  EXPECT_TRUE(myGraph.UIDs().StampOf(aRepId).IsValid());
}

// --- Graph GUID tests ---

TEST_F(BRepGraph_VersionStampTest, GraphGUID_AfterBuild_IsValid)
{
  const Standard_GUID& aGUID = myGraph.UIDs().GraphGUID();
  // A random GUID should not be all zeros.
  const Standard_GUID aZero;
  EXPECT_NE(aGUID, aZero);
}

TEST_F(BRepGraph_VersionStampTest, GraphGUID_Rebuild_Changes)
{
  const Standard_GUID aGUID1 = myGraph.UIDs().GraphGUID();

  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  myGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes3 =
    myGraph.Shapes().Add(aBoxMaker.Shape());
  ASSERT_FALSE(myGraph.IsEmpty());

  const Standard_GUID aGUID2 = myGraph.UIDs().GraphGUID();
  // Two random GUIDs should differ (probability of collision is negligible).
  EXPECT_NE(aGUID1, aGUID2);
}

// --- ToGUID tests ---

TEST_F(BRepGraph_VersionStampTest, ToGUID_Deterministic_SameInputSameOutput)
{
  const BRepGraph_VersionStamp aStamp = myGraph.UIDs().StampOf(BRepGraph_FaceId::Start());
  const Standard_GUID&         aGraph = myGraph.UIDs().GraphGUID();
  const Standard_GUID          aGUID1 = aStamp.ToGUID(aGraph);
  const Standard_GUID          aGUID2 = aStamp.ToGUID(aGraph);
  EXPECT_EQ(aGUID1, aGUID2);
}

TEST_F(BRepGraph_VersionStampTest, ToGUID_DifferentMutationGen_DifferentGUID)
{
  const BRepGraph_VersionStamp aStampBefore = myGraph.UIDs().StampOf(BRepGraph_FaceId::Start());
  const Standard_GUID&         aGraph       = myGraph.UIDs().GraphGUID();
  const Standard_GUID          aGUIDBefore  = aStampBefore.ToGUID(aGraph);

  myGraph.Editor().Faces().SetTolerance(
    BRepGraph_FaceId::Start(),
    BRepGraph_Tool::Face::Tolerance(myGraph, BRepGraph_FaceId::Start()) + 0.01);

  const BRepGraph_VersionStamp aStampAfter = myGraph.UIDs().StampOf(BRepGraph_FaceId::Start());
  const Standard_GUID          aGUIDAfter  = aStampAfter.ToGUID(aGraph);

  EXPECT_NE(aGUIDBefore, aGUIDAfter);
}

TEST_F(BRepGraph_VersionStampTest, ToGUID_DifferentGraphGUID_DifferentGUID)
{
  const BRepGraph_VersionStamp aStamp = myGraph.UIDs().StampOf(BRepGraph_FaceId::Start());

  const Standard_GUID aGraphA("a1b2c3d4-e5f6-7890-abcd-ef0123456789");
  const Standard_GUID aGraphB("11223344-5566-7788-99aa-bbccddeeff00");

  const Standard_GUID aGUIDA = aStamp.ToGUID(aGraphA);
  const Standard_GUID aGUIDB = aStamp.ToGUID(aGraphB);

  EXPECT_NE(aGUIDA, aGUIDB);
}

TEST_F(BRepGraph_VersionStampTest, ToGUID_DifferentNodes_DifferentGUID)
{
  const Standard_GUID& aGraph = myGraph.UIDs().GraphGUID();

  const BRepGraph_VersionStamp aStamp0 = myGraph.UIDs().StampOf(BRepGraph_FaceId::Start());
  const BRepGraph_VersionStamp aStamp1 = myGraph.UIDs().StampOf(BRepGraph_FaceId(1));

  EXPECT_NE(aStamp0.ToGUID(aGraph), aStamp1.ToGUID(aGraph));
}
