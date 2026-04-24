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

#include <Bnd_Box.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_CacheView.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_UIDsView.hxx>

#include <BRepGraph_Tool.hxx>
#include <BRepGraph_Builder.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Precision.hxx>
#include <Standard_ProgramError.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <gtest/gtest.h>

namespace
{
//! Concrete subclass for testing cache values.
class TestCacheValue : public BRepGraph_CacheValue
{
public:
  DEFINE_STANDARD_RTTI_INLINE(TestCacheValue, BRepGraph_CacheValue)
  TestCacheValue() = default;
};

const occ::handle<BRepGraph_CacheKind>& testUserAttrKind()
{
  static const occ::handle<BRepGraph_CacheKind> THE_KIND =
    new BRepGraph_CacheKind(Standard_GUID("2f9b6a5c-1f2d-4a88-9c1c-7a0c16a10020"), "ViewsTestAttr");
  return THE_KIND;
}

template <class theRefIdType, class theRefFn>
static int countActiveRefs(const NCollection_DynamicArray<theRefIdType>& theRefIds,
                           const theRefFn&                         theRefAccess)
{
  int aCount = 0;
  for (const theRefIdType& aRefId : theRefIds)
  {
    if (!theRefAccess(aRefId).IsRemoved)
    {
      ++aCount;
    }
  }
  return aCount;
}

static int countActiveNodes(const BRepGraph&             theGraph,
                            const BRepGraph_NodeId::Kind theKind,
                            const int                    theUpperBound)
{
  int aCount = 0;
  for (int anIdx = 0; anIdx < theUpperBound; ++anIdx)
  {
    if (!theGraph.Topo().Gen().IsRemoved(BRepGraph_NodeId(theKind, anIdx)))
    {
      ++aCount;
    }
  }
  return aCount;
}
} // namespace

class BRepGraph_ViewsTest : public testing::Test
{
protected:
  void SetUp() override
  {
    BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
    const TopoDS_Shape& aBox = aBoxMaker.Shape();
    BRepGraph_Builder::Perform(myGraph, aBox);
  }

  BRepGraph myGraph;
};

// ---------- DefsView ----------

TEST_F(BRepGraph_ViewsTest, DefsView_NbFaces)
{
  EXPECT_EQ(myGraph.Topo().Faces().Nb(), 6);
}

TEST_F(BRepGraph_ViewsTest, DefsView_NbSolids)
{
  EXPECT_EQ(myGraph.Topo().Solids().Nb(), 1);
}

TEST_F(BRepGraph_ViewsTest, DefsView_NbShells)
{
  EXPECT_EQ(myGraph.Topo().Shells().Nb(), 1);
}

TEST_F(BRepGraph_ViewsTest, DefsView_NbWires)
{
  EXPECT_EQ(myGraph.Topo().Wires().Nb(), 6);
}

TEST_F(BRepGraph_ViewsTest, DefsView_NbEdges)
{
  EXPECT_EQ(myGraph.Topo().Edges().Nb(), 12);
}

TEST_F(BRepGraph_ViewsTest, DefsView_NbVertices)
{
  EXPECT_EQ(myGraph.Topo().Vertices().Nb(), 8);
}

TEST_F(BRepGraph_ViewsTest, DefsView_ActiveCounts_MatchStorageState)
{
  EXPECT_EQ(
    countActiveNodes(myGraph, BRepGraph_NodeId::Kind::Vertex, myGraph.Topo().Vertices().Nb()),
    myGraph.Topo().Vertices().NbActive());
  EXPECT_EQ(countActiveNodes(myGraph, BRepGraph_NodeId::Kind::Edge, myGraph.Topo().Edges().Nb()),
            myGraph.Topo().Edges().NbActive());
  EXPECT_EQ(
    countActiveNodes(myGraph, BRepGraph_NodeId::Kind::CoEdge, myGraph.Topo().CoEdges().Nb()),
    myGraph.Topo().CoEdges().NbActive());
  EXPECT_EQ(countActiveNodes(myGraph, BRepGraph_NodeId::Kind::Wire, myGraph.Topo().Wires().Nb()),
            myGraph.Topo().Wires().NbActive());
  EXPECT_EQ(countActiveNodes(myGraph, BRepGraph_NodeId::Kind::Face, myGraph.Topo().Faces().Nb()),
            myGraph.Topo().Faces().NbActive());
  EXPECT_EQ(countActiveNodes(myGraph, BRepGraph_NodeId::Kind::Shell, myGraph.Topo().Shells().Nb()),
            myGraph.Topo().Shells().NbActive());
  EXPECT_EQ(countActiveNodes(myGraph, BRepGraph_NodeId::Kind::Solid, myGraph.Topo().Solids().Nb()),
            myGraph.Topo().Solids().NbActive());
  EXPECT_EQ(
    countActiveNodes(myGraph, BRepGraph_NodeId::Kind::Compound, myGraph.Topo().Compounds().Nb()),
    myGraph.Topo().Compounds().NbActive());
  EXPECT_EQ(
    countActiveNodes(myGraph, BRepGraph_NodeId::Kind::CompSolid, myGraph.Topo().CompSolids().Nb()),
    myGraph.Topo().CompSolids().NbActive());
  EXPECT_EQ(
    countActiveNodes(myGraph, BRepGraph_NodeId::Kind::Product, myGraph.Topo().Products().Nb()),
    myGraph.Topo().Products().NbActive());
  EXPECT_EQ(countActiveNodes(myGraph,
                             BRepGraph_NodeId::Kind::Occurrence,
                             myGraph.Topo().Occurrences().Nb()),
            myGraph.Topo().Occurrences().NbActive());
}

TEST_F(BRepGraph_ViewsTest, DefsView_NbActiveFaces_ExcludeRemoved)
{
  const int aFacesBefore = myGraph.Topo().Faces().NbActive();
  myGraph.Editor().Gen().RemoveNode(BRepGraph_FaceId::Start());

  EXPECT_EQ(myGraph.Topo().Faces().NbActive(), aFacesBefore - 1);
  EXPECT_TRUE(myGraph.Topo().Gen().IsRemoved(BRepGraph_FaceId::Start()));
}

TEST_F(BRepGraph_ViewsTest, DefsView_FaceAccessor_Valid)
{
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    EXPECT_TRUE(aFaceIt.CurrentId().IsValid())
      << "Face " << aFaceIt.CurrentId().Index << " has invalid Id";
  }
}

TEST_F(BRepGraph_ViewsTest, DefsView_TopoEntity_Valid)
{
  BRepGraph_FaceId             aFaceId(0);
  const BRepGraphInc::BaseDef* aBase = myGraph.Topo().Gen().TopoEntity(aFaceId);
  EXPECT_NE(aBase, nullptr);
}

TEST_F(BRepGraph_ViewsTest, DefsView_NbNodes_Positive)
{
  EXPECT_GT(myGraph.Topo().Gen().NbNodes(), 0u);
}

// ---------- DefsView Geometry ----------

TEST_F(BRepGraph_ViewsTest, DefsView_FaceSurface_NonNull)
{
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    EXPECT_TRUE(BRepGraph_Tool::Face::HasSurface(myGraph, aFaceIt.CurrentId()))
      << "Face " << aFaceIt.CurrentId().Index << " has no surface representation";
  }
}

TEST_F(BRepGraph_ViewsTest, DefsView_EdgeCurve3d_NonNull)
{
  for (BRepGraph_EdgeIterator anEdgeIt(myGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    EXPECT_TRUE(BRepGraph_Tool::Edge::HasCurve(myGraph, anEdgeIt.CurrentId()))
      << "Edge " << anEdgeIt.CurrentId().Index << " has no Curve3D representation";
  }
}

TEST_F(BRepGraph_ViewsTest, DefsView_FindPCurve_NoCrash)
{
  // FindPCurve may or may not return a non-null pointer for an arbitrary edge/face pair.
  // Just verify it does not crash.
  (void)BRepGraph_Tool::Edge::FindPCurve(myGraph,
                                         BRepGraph_EdgeId::Start(),
                                         BRepGraph_FaceId::Start());
}

TEST_F(BRepGraph_ViewsTest, DefsView_RepIdConvenienceAccessors_RoundTrip)
{
  const BRepGraph_FaceId aFaceId(0);
  const BRepGraph_EdgeId anEdgeId(0);

  EXPECT_EQ(myGraph.Topo().Faces().SurfaceRepId(aFaceId),
            myGraph.Topo().Faces().Definition(aFaceId).SurfaceRepId);
  EXPECT_EQ(myGraph.Topo().Faces().ActiveTriangulationRepId(aFaceId),
            myGraph.Topo().Faces().Definition(aFaceId).TriangulationRepId);
  EXPECT_EQ(myGraph.Topo().Edges().Curve3DRepId(anEdgeId),
            myGraph.Topo().Edges().Definition(anEdgeId).Curve3DRepId);

  const NCollection_DynamicArray<BRepGraph_CoEdgeId>& aCoEdges = myGraph.Topo().Edges().CoEdges(anEdgeId);
  ASSERT_GT(aCoEdges.Length(), 0);
  const BRepGraph_CoEdgeId aCoEdgeId = aCoEdges.Value(0);
  EXPECT_EQ(myGraph.Topo().CoEdges().Curve2DRepId(aCoEdgeId),
            myGraph.Topo().CoEdges().Definition(aCoEdgeId).Curve2DRepId);
}

TEST_F(BRepGraph_ViewsTest, DefsView_RepIdConvenienceAccessors_InvalidInput)
{
  const BRepGraph_FaceId   aFaceOut(myGraph.Topo().Faces().Nb());
  const BRepGraph_EdgeId   anEdgeOut(myGraph.Topo().Edges().Nb());
  const BRepGraph_CoEdgeId aCoEdgeOut(myGraph.Topo().CoEdges().Nb());

  EXPECT_FALSE(myGraph.Topo().Faces().SurfaceRepId(aFaceOut).IsValid());
  EXPECT_FALSE(myGraph.Topo().Faces().ActiveTriangulationRepId(aFaceOut).IsValid());
  EXPECT_FALSE(myGraph.Topo().Edges().Curve3DRepId(anEdgeOut).IsValid());
  EXPECT_FALSE(myGraph.Topo().CoEdges().Curve2DRepId(aCoEdgeOut).IsValid());
}

// ---------- UIDsView ----------

TEST_F(BRepGraph_ViewsTest, UIDsView_Of_Valid)
{
  BRepGraph_FaceId     aFaceId(0);
  const BRepGraph_UID& aUID = myGraph.UIDs().Of(aFaceId);
  EXPECT_TRUE(aUID.IsValid());
}

TEST_F(BRepGraph_ViewsTest, UIDsView_Generation_Positive)
{
  EXPECT_GT(myGraph.UIDs().Generation(), 0u);
}

TEST_F(BRepGraph_ViewsTest, UIDsView_NodeIdFrom_RoundTrip)
{
  const BRepGraph_FaceId aFaceId(0);
  const BRepGraph_UID    aUID = myGraph.UIDs().Of(aFaceId);
  ASSERT_TRUE(aUID.IsValid());
  EXPECT_EQ(myGraph.UIDs().NodeIdFrom(aUID), BRepGraph_NodeId(aFaceId));
}

TEST_F(BRepGraph_ViewsTest, UIDsView_NodeIdFrom_MultipleRoundTrip)
{
  NCollection_DynamicArray<BRepGraph_UID> aUIDs;
  const BRepGraph_UID               aFaceUID  = myGraph.UIDs().Of(BRepGraph_FaceId::Start());
  const BRepGraph_UID               anEdgeUID = myGraph.UIDs().Of(BRepGraph_EdgeId::Start());
  ASSERT_TRUE(aFaceUID.IsValid());
  ASSERT_TRUE(anEdgeUID.IsValid());

  aUIDs.Append(aFaceUID);
  aUIDs.Append(anEdgeUID);

  ASSERT_EQ(aUIDs.Length(), 2);
  EXPECT_EQ(myGraph.UIDs().NodeIdFrom(aUIDs.Value(0)), BRepGraph_NodeId(BRepGraph_FaceId::Start()));
  EXPECT_EQ(myGraph.UIDs().NodeIdFrom(aUIDs.Value(1)), BRepGraph_NodeId(BRepGraph_EdgeId::Start()));
}

TEST_F(BRepGraph_ViewsTest, UIDsView_NodeIdFrom_InvalidAndWrongGeneration)
{
  NCollection_DynamicArray<BRepGraph_UID> aUIDs;
  aUIDs.Append(BRepGraph_UID());
  aUIDs.Append(BRepGraph_UID(BRepGraph_NodeId::Kind::Face, 1, myGraph.UIDs().Generation() + 1));

  ASSERT_EQ(aUIDs.Length(), 2);
  EXPECT_FALSE(myGraph.UIDs().NodeIdFrom(aUIDs.Value(0)).IsValid());
  EXPECT_FALSE(myGraph.UIDs().NodeIdFrom(aUIDs.Value(1)).IsValid());
}

TEST_F(BRepGraph_ViewsTest, UIDsView_NodeLookup_RemovedNode_ReturnsInvalidAndHasFalse)
{
  const BRepGraph_FaceId aFaceId(0);
  const BRepGraph_UID    aUID = myGraph.UIDs().Of(aFaceId);
  ASSERT_TRUE(aUID.IsValid());
  ASSERT_TRUE(myGraph.UIDs().Has(aUID));
  ASSERT_EQ(myGraph.UIDs().NodeIdFrom(aUID), BRepGraph_NodeId(aFaceId));

  myGraph.Editor().Gen().RemoveNode(aFaceId);

  EXPECT_FALSE(myGraph.UIDs().Has(aUID));
  EXPECT_FALSE(myGraph.UIDs().NodeIdFrom(aUID).IsValid());
}

TEST_F(BRepGraph_ViewsTest, UIDsView_Of_RemovedNode_ReturnsInvalid)
{
  const BRepGraph_FaceId aFaceId(0);
  const BRepGraph_UID    aUID = myGraph.UIDs().Of(aFaceId);
  ASSERT_TRUE(aUID.IsValid());

  myGraph.Editor().Gen().RemoveNode(aFaceId);

  EXPECT_FALSE(myGraph.UIDs().Of(aFaceId).IsValid());
}

TEST_F(BRepGraph_ViewsTest, UIDsView_RefLookup_RemovedRef_ReturnsInvalidAndHasFalse)
{
  const NCollection_DynamicArray<BRepGraph_FaceRefId>& aFaceRefs =
    myGraph.Refs().Faces().IdsOf(BRepGraph_ShellId::Start());
  ASSERT_GT(aFaceRefs.Length(), 0);
  const BRepGraph_FaceRefId aFaceRefId = aFaceRefs.Value(0);

  const BRepGraph_RefUID aUID = myGraph.UIDs().Of(aFaceRefId);
  ASSERT_TRUE(aUID.IsValid());
  ASSERT_TRUE(myGraph.UIDs().Has(aUID));
  ASSERT_EQ(myGraph.UIDs().RefIdFrom(aUID), BRepGraph_RefId(aFaceRefId));

  ASSERT_TRUE(myGraph.Editor().Gen().RemoveRef(aFaceRefId));

  EXPECT_FALSE(myGraph.UIDs().Has(aUID));
  EXPECT_FALSE(myGraph.UIDs().RefIdFrom(aUID).IsValid());
}

TEST_F(BRepGraph_ViewsTest, UIDsView_Of_RemovedRef_ReturnsInvalid)
{
  const NCollection_DynamicArray<BRepGraph_FaceRefId>& aFaceRefs =
    myGraph.Refs().Faces().IdsOf(BRepGraph_ShellId::Start());
  ASSERT_GT(aFaceRefs.Length(), 0);
  const BRepGraph_FaceRefId aFaceRefId = aFaceRefs.Value(0);
  const BRepGraph_RefUID    aUID       = myGraph.UIDs().Of(aFaceRefId);
  ASSERT_TRUE(aUID.IsValid());

  ASSERT_TRUE(myGraph.Editor().Gen().RemoveRef(aFaceRefId));

  EXPECT_FALSE(myGraph.UIDs().Of(aFaceRefId).IsValid());
}

TEST_F(BRepGraph_ViewsTest, UIDsView_Of_OutOfRangeRef_ReturnsInvalid)
{
  const BRepGraph_FaceRefId aOutOfRangeRefId(myGraph.Refs().Faces().Nb());
  EXPECT_FALSE(myGraph.UIDs().Of(aOutOfRangeRefId).IsValid());
}

// ---------- Topology adjacency ----------

TEST_F(BRepGraph_ViewsTest, SpatialView_AdjacentFaces_FourPerBoxFace)
{
  BRepGraph_FaceId                     aFaceId(0);
  NCollection_DynamicArray<BRepGraph_FaceId> aResult =
    myGraph.Topo().Faces().Adjacent(aFaceId, myGraph.Allocator());
  EXPECT_EQ(aResult.Length(), 4);
}

TEST_F(BRepGraph_ViewsTest, SpatialView_FacesOfEdge_TwoPerBoxEdge)
{
  BRepGraph_EdgeId                            anEdgeId(0);
  const NCollection_DynamicArray<BRepGraph_FaceId>& aResult = myGraph.Topo().Edges().Faces(anEdgeId);
  EXPECT_EQ(aResult.Length(), 2);
}

TEST_F(BRepGraph_ViewsTest, SpatialView_OutParam_Parity)
{
  const BRepGraph_FaceId                        aFaceId(0);
  const BRepGraph_EdgeId                        anEdgeId(0);
  const BRepGraph_VertexId                      aVertexId(0);
  const occ::handle<NCollection_BaseAllocator>& anAllocator = myGraph.Allocator();

  const NCollection_DynamicArray<BRepGraph_FaceId> aAdjacentByValue =
    myGraph.Topo().Faces().Adjacent(aFaceId, anAllocator);
  EXPECT_EQ(aAdjacentByValue.Length(), 4);

  const NCollection_DynamicArray<BRepGraph_EdgeId> anAdjEdgesByValue =
    myGraph.Topo().Edges().Adjacent(anEdgeId, anAllocator);
  EXPECT_GE(anAdjEdgesByValue.Length(), 4);
}

TEST_F(BRepGraph_ViewsTest, TopoView_GroupedFaceOps_Parity)
{
  const BRepGraph_FaceId                        aFaceId(0);
  const occ::handle<NCollection_BaseAllocator>& anAllocator = myGraph.Allocator();

  EXPECT_EQ(BRepGraph_NodeId(aFaceId), BRepGraph_NodeId(aFaceId));
  EXPECT_EQ(myGraph.Topo().Faces().SurfaceRepId(aFaceId),
            myGraph.Topo().Faces().Definition(aFaceId).SurfaceRepId);
  EXPECT_EQ(myGraph.Topo().Faces().ActiveTriangulationRepId(aFaceId),
            myGraph.Topo().Faces().Definition(aFaceId).TriangulationRepId);
  EXPECT_EQ(myGraph.Topo().Faces().OuterWire(aFaceId), BRepGraph_WireId::Start());

  EXPECT_EQ(myGraph.Topo().Faces().Adjacent(aFaceId, anAllocator).Length(), 4);
}

TEST_F(BRepGraph_ViewsTest, TopoView_GroupedEdgeAndVertexOps_Parity)
{
  const BRepGraph_EdgeId                        anEdgeId(0);
  const BRepGraph_VertexId                      aVertexId(0);
  const occ::handle<NCollection_BaseAllocator>& anAllocator = myGraph.Allocator();

  EXPECT_EQ(BRepGraph_NodeId(anEdgeId), BRepGraph_NodeId(anEdgeId));
  EXPECT_EQ(myGraph.Topo().Edges().NbFaces(anEdgeId), 2);
  EXPECT_EQ(myGraph.Topo().Edges().Curve3DRepId(anEdgeId),
            myGraph.Topo().Edges().Definition(anEdgeId).Curve3DRepId);
  EXPECT_FALSE(myGraph.Topo().Edges().IsBoundary(anEdgeId));
  EXPECT_TRUE(myGraph.Topo().Edges().IsManifold(anEdgeId));
  EXPECT_GE(myGraph.Topo().Edges().Wires(anEdgeId).Length(), 1);
  EXPECT_GE(myGraph.Topo().Edges().CoEdges(anEdgeId).Length(), 1);
  EXPECT_EQ(myGraph.Topo().Edges().Faces(anEdgeId).Length(), 2);
  EXPECT_GE(myGraph.Topo().Edges().Adjacent(anEdgeId, anAllocator).Length(), 4);

  EXPECT_EQ(BRepGraph_NodeId(aVertexId), BRepGraph_NodeId(aVertexId));
  EXPECT_GE(myGraph.Topo().Vertices().Edges(aVertexId).Length(), 1);
}

TEST_F(BRepGraph_ViewsTest, TopoView_GroupedCoEdgeOps_Parity)
{
  const BRepGraph_CoEdgeId       aCoEdgeId(0);
  const BRepGraphInc::CoEdgeDef& aCoEdge = myGraph.Topo().CoEdges().Definition(aCoEdgeId);

  EXPECT_EQ(BRepGraph_NodeId(aCoEdgeId), BRepGraph_NodeId(aCoEdgeId));
  EXPECT_EQ(myGraph.Topo().CoEdges().Definition(aCoEdgeId).EdgeDefId, aCoEdge.EdgeDefId);
  EXPECT_EQ(myGraph.Topo().CoEdges().Definition(aCoEdgeId).FaceDefId, aCoEdge.FaceDefId);
  EXPECT_EQ(myGraph.Topo().CoEdges().Curve2DRepId(aCoEdgeId),
            myGraph.Topo().CoEdges().Definition(aCoEdgeId).Curve2DRepId);
  EXPECT_EQ(myGraph.Topo().CoEdges().Definition(aCoEdgeId).SeamPairId, aCoEdge.SeamPairId);
}

TEST_F(BRepGraph_ViewsTest, TopoView_GroupedProductAndOccurrenceOps_Parity)
{
  const BRepGraph_ProductId aPartProduct =
    myGraph.Editor().Products().Add(BRepGraph_NodeId(BRepGraph_SolidId::Start()));
  const BRepGraph_ProductId aSubAssembly  = myGraph.Editor().Products().AddAssembly();
  const BRepGraph_ProductId aRootAssembly = myGraph.Editor().Products().AddAssembly();
  ASSERT_TRUE(aPartProduct.IsValid());
  ASSERT_TRUE(aSubAssembly.IsValid());
  ASSERT_TRUE(aRootAssembly.IsValid());

  const BRepGraph_OccurrenceId aSubOccurrence =
    myGraph.Editor().Products().AddOccurrence(aRootAssembly, aSubAssembly, TopLoc_Location());
  const BRepGraph_OccurrenceId aPartOccurrence =
    myGraph.Editor().Products().AddOccurrence(aSubAssembly,
                                              aPartProduct,
                                              TopLoc_Location(),
                                              aSubOccurrence);
  ASSERT_TRUE(aSubOccurrence.IsValid());
  ASSERT_TRUE(aPartOccurrence.IsValid());

  EXPECT_EQ(BRepGraph_NodeId(aPartProduct), BRepGraph_NodeId(aPartProduct));
  EXPECT_EQ(myGraph.Topo().Products().ShapeRoot(aPartProduct),
            BRepGraph_NodeId(BRepGraph_SolidId::Start()));
  EXPECT_FALSE(myGraph.Topo().Products().ShapeRoot(aRootAssembly).IsValid());
  EXPECT_EQ(myGraph.Refs().Occurrences().IdsOf(aRootAssembly).Length(), 1);
  EXPECT_EQ(myGraph.Refs().Occurrences().IdsOf(aSubAssembly).Length(), 1);

  EXPECT_EQ(BRepGraph_NodeId(aPartOccurrence), BRepGraph_NodeId(aPartOccurrence));
  EXPECT_EQ(myGraph.Topo().Occurrences().Product(aPartOccurrence), aPartProduct);
  EXPECT_EQ(myGraph.Topo().Occurrences().ParentProduct(aPartOccurrence), aSubAssembly);

  const NCollection_DynamicArray<BRepGraph_OccurrenceRefId>& aOccurrenceRefs =
    myGraph.Refs().Occurrences().IdsOf(aSubAssembly);
  ASSERT_EQ(aOccurrenceRefs.Length(), 1);
  {
    BRepGraph_MutGuard<BRepGraphInc::OccurrenceRef> anOccurrenceRef =
      myGraph.Editor().Products().MutOccurrenceRef(aOccurrenceRefs.Value(0));
    anOccurrenceRef->IsRemoved = true;
  }

  EXPECT_EQ(myGraph.Topo().Products().NbComponents(aSubAssembly), 0);
}

TEST_F(BRepGraph_ViewsTest, SpatialView_OutParam_ClearAndInvalid)
{
  const occ::handle<NCollection_BaseAllocator>& anAllocator = myGraph.Allocator();

  const NCollection_DynamicArray<BRepGraph_FaceId> aFaceResult =
    myGraph.Topo().Faces().Adjacent(BRepGraph_FaceId::Start(), anAllocator);
  EXPECT_EQ(aFaceResult.Length(), 4);
  EXPECT_EQ(myGraph.Topo().Faces().Adjacent(BRepGraph_FaceId(999), anAllocator).Length(), 0);

  const NCollection_DynamicArray<BRepGraph_EdgeId> anAdjEdgeResult =
    myGraph.Topo().Edges().Adjacent(BRepGraph_EdgeId::Start(), anAllocator);
  EXPECT_GE(anAdjEdgeResult.Length(), 4);
  EXPECT_EQ(myGraph.Topo().Edges().Adjacent(BRepGraph_EdgeId(999), anAllocator).Length(), 0);
}

// ---------- CacheView ----------

TEST_F(BRepGraph_ViewsTest, AttrsView_SetGet_RoundTrip)
{
  BRepGraph_FaceId                  aFaceId(0);
  occ::handle<BRepGraph_CacheValue> anAttr = new TestCacheValue();
  myGraph.Cache().Set(aFaceId, testUserAttrKind(), anAttr);
  occ::handle<BRepGraph_CacheValue> aRetrieved = myGraph.Cache().Get(aFaceId, testUserAttrKind());
  EXPECT_EQ(aRetrieved, anAttr);
  EXPECT_TRUE(myGraph.Cache().Has(aFaceId, testUserAttrKind()));
}

TEST_F(BRepGraph_ViewsTest, AttrsView_Remove_Works)
{
  BRepGraph_FaceId                  aFaceId(0);
  occ::handle<BRepGraph_CacheValue> anAttr = new TestCacheValue();
  myGraph.Cache().Set(aFaceId, testUserAttrKind(), anAttr);
  EXPECT_TRUE(myGraph.Cache().Remove(aFaceId, testUserAttrKind()));
  EXPECT_FALSE(myGraph.Cache().Has(aFaceId, testUserAttrKind()));
  EXPECT_TRUE(myGraph.Cache().Get(aFaceId, testUserAttrKind()).IsNull());
}

TEST_F(BRepGraph_ViewsTest, AttrsView_CacheKinds_ReportsStoredKind)
{
  BRepGraph_FaceId                  aFaceId(0);
  occ::handle<BRepGraph_CacheValue> anAttr = new TestCacheValue();
  myGraph.Cache().Set(aFaceId, testUserAttrKind(), anAttr);

  BRepGraph_CacheKindIterator<BRepGraph_NodeId> anIt = myGraph.Cache().CacheKindIter(aFaceId);

  ASSERT_EQ(anIt.NbKinds(), 1);
  ASSERT_TRUE(anIt.More());
  ASSERT_FALSE(anIt.Value().IsNull());
  EXPECT_EQ(anIt.Value()->ID(), testUserAttrKind()->ID());
  EXPECT_EQ(myGraph.Cache().Get(aFaceId, testUserAttrKind()), anAttr);
}

TEST_F(BRepGraph_ViewsTest, AttrsView_CacheKindIter_RangeFor)
{
  BRepGraph_FaceId aFaceId(0);
  myGraph.Cache().Set(aFaceId, testUserAttrKind(), new TestCacheValue());

  int  aCount      = 0;
  bool hasUserKind = false;
  for (const occ::handle<BRepGraph_CacheKind>& aKind : myGraph.Cache().CacheKindIter(aFaceId))
  {
    if (!aKind.IsNull() && aKind->ID() == testUserAttrKind()->ID())
      hasUserKind = true;
    ++aCount;
  }
  EXPECT_EQ(aCount, 1);
  EXPECT_TRUE(hasUserKind);
}

TEST_F(BRepGraph_ViewsTest, AttrsView_CacheKindIter_RangeFor_Empty)
{
  // No cache entries set - range-for should produce zero iterations.
  int aCount = 0;
  for (const occ::handle<BRepGraph_CacheKind>& aKind :
       myGraph.Cache().CacheKindIter(BRepGraph_FaceId::Start()))
  {
    (void)aKind;
    ++aCount;
  }
  EXPECT_EQ(aCount, 0);
}

TEST_F(BRepGraph_ViewsTest, AttrsView_CacheKindIter_KindSlot)
{
  BRepGraph_FaceId aFaceId(0);
  myGraph.Cache().Set(aFaceId, testUserAttrKind(), new TestCacheValue());

  BRepGraph_CacheKindIterator<BRepGraph_NodeId> anIt = myGraph.Cache().CacheKindIter(aFaceId);
  ASSERT_TRUE(anIt.More());
  // KindSlot must be a valid non-negative index matching the registered slot.
  EXPECT_GE(anIt.KindSlot(), 0);
  // Retrieving via slot must return the same value as via kind handle.
  occ::handle<BRepGraph_CacheValue> aByKind = myGraph.Cache().Get(aFaceId, testUserAttrKind());
  occ::handle<BRepGraph_CacheValue> aBySlot = myGraph.Cache().Get(aFaceId, anIt.KindSlot());
  EXPECT_EQ(aByKind, aBySlot);
}

TEST_F(BRepGraph_ViewsTest, AttrsView_CacheKindIter_RefId_RangeFor)
{
  // Set a cache value on a FaceRef.
  const BRepGraph_FaceRefId aRef(0);
  myGraph.Cache().Set(aRef, testUserAttrKind(), new TestCacheValue());

  int  aCount      = 0;
  bool hasUserKind = false;
  for (const occ::handle<BRepGraph_CacheKind>& aKind : myGraph.Cache().CacheKindIter(aRef))
  {
    if (!aKind.IsNull() && aKind->ID() == testUserAttrKind()->ID())
      hasUserKind = true;
    ++aCount;
  }
  EXPECT_EQ(aCount, 1);
  EXPECT_TRUE(hasUserKind);
}

TEST_F(BRepGraph_ViewsTest, AttrsView_CacheKindIter_RefId_Empty)
{
  // No cache entries on this ref - iterator should be empty.
  BRepGraph_CacheKindIterator<BRepGraph_RefId> anIt =
    myGraph.Cache().CacheKindIter(BRepGraph_FaceRefId::Start());
  EXPECT_FALSE(anIt.More());
  EXPECT_EQ(anIt.NbKinds(), 0);
}

TEST_F(BRepGraph_ViewsTest, EdgeOps_FindCoEdgeId_ValidPair)
{
  // A box has edges shared by faces. Pick an edge and one of its faces.
  const BRepGraph_EdgeId       anEdge(0);
  const BRepGraphInc::EdgeDef& anEdgeDef = myGraph.Topo().Edges().Definition(anEdge);
  (void)anEdgeDef;

  // Use reverse index to get a face for this edge.
  const NCollection_DynamicArray<BRepGraph_FaceId>& aFaces = myGraph.Topo().Edges().Faces(anEdge);
  ASSERT_GT(aFaces.Length(), 0);
  const BRepGraph_FaceId aFace = aFaces.Value(0);

  const BRepGraph_CoEdgeId aCoEdgeId = myGraph.Topo().Edges().FindCoEdgeId(anEdge, aFace);
  ASSERT_TRUE(aCoEdgeId.IsValid());

  // The returned CoEdge must reference the same edge and face.
  const BRepGraphInc::CoEdgeDef& aCoEdge = myGraph.Topo().CoEdges().Definition(aCoEdgeId);
  EXPECT_EQ(aCoEdge.EdgeDefId, anEdge);
  EXPECT_EQ(aCoEdge.FaceDefId, aFace);
}

TEST_F(BRepGraph_ViewsTest, EdgeOps_FindCoEdgeId_InvalidPair_ReturnsInvalid)
{
  // Use a valid edge but a face that doesn't share it.
  // Edge 0 and the last face are very unlikely to share a coedge in a box.
  const BRepGraph_EdgeId                      anEdge(0);
  const NCollection_DynamicArray<BRepGraph_FaceId>& aEdgeFaces = myGraph.Topo().Edges().Faces(anEdge);

  // Find a face NOT adjacent to edge 0.
  BRepGraph_FaceId aNonAdjacentFace;
  for (BRepGraph_FaceId aFaceId = BRepGraph_FaceId::Start();
       aFaceId.IsValid(myGraph.Topo().Faces().Nb());
       ++aFaceId)
  {
    bool isAdjacent = false;
    for (const BRepGraph_FaceId& aFace : aEdgeFaces)
    {
      if (aFace.Index == aFaceId.Index)
      {
        isAdjacent = true;
        break;
      }
    }
    if (!isAdjacent)
    {
      aNonAdjacentFace = aFaceId;
      break;
    }
  }
  if (aNonAdjacentFace.IsValid())
  {
    const BRepGraph_CoEdgeId aCoEdgeId =
      myGraph.Topo().Edges().FindCoEdgeId(anEdge, aNonAdjacentFace);
    EXPECT_FALSE(aCoEdgeId.IsValid());
  }
}

TEST_F(BRepGraph_ViewsTest, AttrsView_MutFace_InvalidatesEntry)
{
  BRepGraph_FaceId                  aFaceId(0);
  occ::handle<BRepGraph_CacheValue> anAttr = new TestCacheValue();
  myGraph.Cache().Set(aFaceId, testUserAttrKind(), anAttr);
  ASSERT_TRUE(myGraph.Cache().Has(aFaceId, testUserAttrKind()));

  {
    BRepGraph_MutGuard<BRepGraphInc::FaceDef> aFace =
      myGraph.Editor().Faces().Mut(BRepGraph_FaceId::Start());
    aFace->Tolerance += 0.1;
  }

  EXPECT_FALSE(myGraph.Cache().Has(aFaceId, testUserAttrKind()));
  EXPECT_TRUE(myGraph.Cache().Get(aFaceId, testUserAttrKind()).IsNull());
  EXPECT_FALSE(myGraph.Cache().CacheKindIter(aFaceId).More());
}

TEST_F(BRepGraph_ViewsTest, AttrsView_RemoveNode_InvalidatesEntry)
{
  BRepGraph_FaceId                  aFaceId(0);
  occ::handle<BRepGraph_CacheValue> anAttr = new TestCacheValue();
  myGraph.Cache().Set(aFaceId, testUserAttrKind(), anAttr);
  ASSERT_TRUE(myGraph.Cache().Has(aFaceId, testUserAttrKind()));

  myGraph.Editor().Gen().RemoveNode(aFaceId);

  EXPECT_TRUE(myGraph.Topo().Gen().IsRemoved(aFaceId));
  EXPECT_FALSE(myGraph.Cache().Has(aFaceId, testUserAttrKind()));
  EXPECT_FALSE(myGraph.Cache().CacheKindIter(aFaceId).More());
}

TEST_F(BRepGraph_ViewsTest, AttrsView_MutFaceRef_InvalidatesEntry)
{
  const BRepGraph_FaceRefId aRef(0);
  myGraph.Cache().Set(aRef, testUserAttrKind(), new TestCacheValue());
  ASSERT_TRUE(myGraph.Cache().Has(aRef, testUserAttrKind()));
  ASSERT_TRUE(myGraph.Cache().CacheKindIter(aRef).More());

  {
    BRepGraph_MutGuard<BRepGraphInc::FaceRef> aFaceRef = myGraph.Editor().Faces().MutRef(aRef);
    aFaceRef->Orientation                              = TopAbs::Reverse(aFaceRef->Orientation);
  }

  EXPECT_FALSE(myGraph.Cache().Has(aRef, testUserAttrKind()));
  EXPECT_TRUE(myGraph.Cache().Get(aRef, testUserAttrKind()).IsNull());
  EXPECT_FALSE(myGraph.Cache().CacheKindIter(aRef).More());
}

TEST_F(BRepGraph_ViewsTest, AttrsView_RemoveFaceRef_HidesCacheKindIterator)
{
  const BRepGraph_FaceRefId aRef(0);
  myGraph.Cache().Set(aRef, testUserAttrKind(), new TestCacheValue());
  ASSERT_TRUE(myGraph.Cache().Has(aRef, testUserAttrKind()));

  {
    BRepGraph_MutGuard<BRepGraphInc::FaceRef> aFaceRef = myGraph.Editor().Faces().MutRef(aRef);
    aFaceRef->IsRemoved                                = true;
  }

  EXPECT_FALSE(myGraph.Cache().Has(aRef, testUserAttrKind()));
  EXPECT_FALSE(myGraph.Cache().CacheKindIter(aRef).More());
}

TEST_F(BRepGraph_ViewsTest, RefsView_ActiveCounts_MatchFreshBuild)
{
  EXPECT_EQ(myGraph.Refs().Shells().NbActive(), myGraph.Refs().Shells().Nb());
  EXPECT_EQ(myGraph.Refs().Faces().NbActive(), myGraph.Refs().Faces().Nb());
  EXPECT_EQ(myGraph.Refs().Wires().NbActive(), myGraph.Refs().Wires().Nb());
  EXPECT_EQ(myGraph.Refs().CoEdges().NbActive(), myGraph.Refs().CoEdges().Nb());
  EXPECT_EQ(myGraph.Refs().Vertices().NbActive(), myGraph.Refs().Vertices().Nb());
  EXPECT_EQ(myGraph.Refs().Solids().NbActive(), myGraph.Refs().Solids().Nb());
  EXPECT_EQ(myGraph.Refs().Children().NbActive(), myGraph.Refs().Children().Nb());
  EXPECT_EQ(myGraph.Refs().Occurrences().NbActive(), myGraph.Refs().Occurrences().Nb());
}

TEST_F(BRepGraph_ViewsTest, RefsView_RefIdsOf_MatchFreshBuild)
{
  const BRepGraph_ShellId aShellId(0);
  const BRepGraph_FaceId  aFaceId(0);
  const BRepGraph_WireId  aWireId(0);
  const BRepGraph_SolidId aSolidId(0);

  EXPECT_EQ(
    countActiveRefs(myGraph.Refs().Faces().IdsOf(aShellId),
                    [this](const BRepGraph_FaceRefId theRefId) -> const BRepGraphInc::FaceRef& {
                      return myGraph.Refs().Faces().Entry(theRefId);
                    }),
    myGraph.Refs().Faces().IdsOf(aShellId).Length());
  EXPECT_EQ(
    countActiveRefs(myGraph.Refs().Wires().IdsOf(aFaceId),
                    [this](const BRepGraph_WireRefId theRefId) -> const BRepGraphInc::WireRef& {
                      return myGraph.Refs().Wires().Entry(theRefId);
                    }),
    myGraph.Refs().Wires().IdsOf(aFaceId).Length());
  EXPECT_EQ(
    countActiveRefs(myGraph.Refs().CoEdges().IdsOf(aWireId),
                    [this](const BRepGraph_CoEdgeRefId theRefId) -> const BRepGraphInc::CoEdgeRef& {
                      return myGraph.Refs().CoEdges().Entry(theRefId);
                    }),
    myGraph.Refs().CoEdges().IdsOf(aWireId).Length());
  EXPECT_EQ(
    countActiveRefs(myGraph.Refs().Shells().IdsOf(aSolidId),
                    [this](const BRepGraph_ShellRefId theRefId) -> const BRepGraphInc::ShellRef& {
                      return myGraph.Refs().Shells().Entry(theRefId);
                    }),
    myGraph.Refs().Shells().IdsOf(aSolidId).Length());
}

TEST_F(BRepGraph_ViewsTest, RefsView_FaceRefIdsOf_LocalFilteringHandlesRemoved)
{
  const BRepGraph_ShellId                        aShellId(0);
  const NCollection_DynamicArray<BRepGraph_FaceRefId>& aFaceRefs = myGraph.Refs().Faces().IdsOf(aShellId);
  ASSERT_GT(aFaceRefs.Length(), 0);

  {
    BRepGraph_MutGuard<BRepGraphInc::FaceRef> aFaceRef =
      myGraph.Editor().Faces().MutRef(aFaceRefs.Value(0));
    aFaceRef->IsRemoved = true;
  }

  EXPECT_EQ(
    countActiveRefs(myGraph.Refs().Faces().IdsOf(aShellId),
                    [this](const BRepGraph_FaceRefId theRefId) -> const BRepGraphInc::FaceRef& {
                      return myGraph.Refs().Faces().Entry(theRefId);
                    }),
    aFaceRefs.Length() - 1);
}

TEST_F(BRepGraph_ViewsTest, RefsView_VertexRefIdsOfEdge_ContainsBoundaryVertices)
{
  int aNbVertexRefs = 0;
  for (BRepGraph_RefsVertexOfEdge aRefIt(myGraph, BRepGraph_EdgeId::Start()); aRefIt.More();
       aRefIt.Next())
  {
    const BRepGraph_VertexRefId    aVertexRefId = aRefIt.CurrentId();
    const BRepGraphInc::VertexRef& aRef         = myGraph.Refs().Vertices().Entry(aVertexRefId);
    EXPECT_FALSE(aRef.IsRemoved);
    EXPECT_TRUE(aRef.VertexDefId.IsValid(myGraph.Topo().Vertices().Nb()));
    ++aNbVertexRefs;
  }
  EXPECT_GE(aNbVertexRefs, 2);
}

TEST_F(BRepGraph_ViewsTest, RefsView_GenericRefHelpers_RoundTripForTypedRef)
{
  const NCollection_DynamicArray<BRepGraph_FaceRefId>& aFaceRefs =
    myGraph.Refs().Faces().IdsOf(BRepGraph_ShellId::Start());
  ASSERT_GT(aFaceRefs.Length(), 0);

  const BRepGraph_FaceRefId aFaceRefId = aFaceRefs.Value(0);
  gp_Trsf                   aTrsf;
  aTrsf.SetTranslation(gp_Vec(1.0, 2.0, 3.0));

  {
    BRepGraph_MutGuard<BRepGraphInc::FaceRef> aFaceRef =
      myGraph.Editor().Faces().MutRef(aFaceRefId);
    aFaceRef->LocalLocation = TopLoc_Location(aTrsf);
    aFaceRef->Orientation   = TopAbs_REVERSED;
  }

  const BRepGraphInc::FaceRef& aFaceRefEntry = myGraph.Refs().Faces().Entry(aFaceRefId);
  EXPECT_EQ(myGraph.Refs().ChildNode(aFaceRefId), BRepGraph_NodeId(aFaceRefEntry.FaceDefId));
  EXPECT_TRUE(myGraph.Refs().LocalLocation(aFaceRefId).IsEqual(aFaceRefEntry.LocalLocation));
  EXPECT_EQ(myGraph.Refs().Orientation(aFaceRefId), aFaceRefEntry.Orientation);
  EXPECT_FALSE(myGraph.Refs().IsRemoved(aFaceRefId));

  const BRepGraph_CoEdgeRefId aCoEdgeRefId =
    myGraph.Topo().Wires().Definition(BRepGraph_WireId::Start()).CoEdgeRefIds.Value(0);
  const BRepGraphInc::CoEdgeRef& aCoEdgeRefEntry = myGraph.Refs().CoEdges().Entry(aCoEdgeRefId);
  EXPECT_EQ(myGraph.Refs().ChildNode(aCoEdgeRefId), BRepGraph_NodeId(aCoEdgeRefEntry.CoEdgeDefId));
  EXPECT_TRUE(myGraph.Refs().LocalLocation(aCoEdgeRefId).IsEqual(aCoEdgeRefEntry.LocalLocation));
  EXPECT_EQ(myGraph.Refs().Orientation(aCoEdgeRefId), TopAbs_FORWARD);
}

TEST_F(BRepGraph_ViewsTest, RefsView_RefAtStep_RoundTrip)
{
  const BRepGraph_SolidId    aSolidId(0);
  const BRepGraph_ShellRefId aShellRefId =
    myGraph.Topo().Solids().Definition(aSolidId).ShellRefIds.Value(0);
  EXPECT_EQ(myGraph.Refs().RefAtStep(BRepGraph_NodeId(aSolidId), 0), BRepGraph_RefId(aShellRefId));

  const BRepGraph_WireId      aWireId(0);
  const BRepGraph_CoEdgeRefId aCoEdgeRefId =
    myGraph.Topo().Wires().Definition(aWireId).CoEdgeRefIds.Value(0);
  EXPECT_EQ(myGraph.Refs().RefAtStep(BRepGraph_NodeId(aWireId), 0), BRepGraph_RefId(aCoEdgeRefId));

  EXPECT_FALSE(myGraph.Refs().RefAtStep(BRepGraph_NodeId(aWireId), 100).IsValid());
}

TEST_F(BRepGraph_ViewsTest, RefsView_GenericRefHelpers_OccurrenceDefaults)
{
  const BRepGraph_ProductId aPartProduct =
    myGraph.Editor().Products().Add(BRepGraph_NodeId(BRepGraph_SolidId::Start()));
  const BRepGraph_ProductId anAssembly = myGraph.Editor().Products().AddAssembly();
  ASSERT_TRUE(aPartProduct.IsValid());
  ASSERT_TRUE(anAssembly.IsValid());

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(4.0, 5.0, 6.0));
  ASSERT_TRUE(myGraph.Editor()
                .Products()
                .AddOccurrence(anAssembly, aPartProduct, TopLoc_Location(aTrsf))
                .IsValid());

  const NCollection_DynamicArray<BRepGraph_OccurrenceRefId>& anOccurrenceRefs =
    myGraph.Refs().Occurrences().IdsOf(anAssembly);
  ASSERT_EQ(anOccurrenceRefs.Length(), 1);

  const BRepGraph_OccurrenceRefId    aRefId    = anOccurrenceRefs.Value(0);
  const BRepGraphInc::OccurrenceRef& aRefEntry = myGraph.Refs().Occurrences().Entry(aRefId);
  EXPECT_EQ(myGraph.Refs().RefAtStep(BRepGraph_NodeId(anAssembly), 0), BRepGraph_RefId(aRefId));
  EXPECT_EQ(myGraph.Refs().ChildNode(aRefId), BRepGraph_NodeId(aRefEntry.OccurrenceDefId));
  EXPECT_TRUE(myGraph.Refs().LocalLocation(aRefId).IsEqual(TopLoc_Location()));
  EXPECT_EQ(myGraph.Refs().Orientation(aRefId), TopAbs_FORWARD);
  EXPECT_FALSE(myGraph.Refs().IsRemoved(aRefId));
}

TEST_F(BRepGraph_ViewsTest, RefsView_GenericRefHelpers_InvalidAndRemoved)
{
  EXPECT_FALSE(myGraph.Refs().ChildNode(BRepGraph_RefId()).IsValid());
  EXPECT_TRUE(myGraph.Refs().LocalLocation(BRepGraph_RefId()).IsEqual(TopLoc_Location()));
  EXPECT_EQ(myGraph.Refs().Orientation(BRepGraph_RefId()), TopAbs_FORWARD);
  EXPECT_TRUE(myGraph.Refs().IsRemoved(BRepGraph_RefId()));

  const NCollection_DynamicArray<BRepGraph_FaceRefId>& aFaceRefs =
    myGraph.Refs().Faces().IdsOf(BRepGraph_ShellId::Start());
  ASSERT_GT(aFaceRefs.Length(), 0);
  const BRepGraph_FaceRefId aFaceRefId = aFaceRefs.Value(0);

  {
    BRepGraph_MutGuard<BRepGraphInc::FaceRef> aFaceRef =
      myGraph.Editor().Faces().MutRef(aFaceRefId);
    aFaceRef->IsRemoved = true;
  }

  EXPECT_TRUE(myGraph.Refs().IsRemoved(aFaceRefId));
}

// ---------- ShapesView ----------

TEST_F(BRepGraph_ViewsTest, ShapesView_Shape_NonNull)
{
  BRepGraph_FaceId aFaceId(0);
  EXPECT_FALSE(myGraph.Shapes().Shape(aFaceId).IsNull());
}

TEST_F(BRepGraph_ViewsTest, ShapesView_HasOriginal_True)
{
  BRepGraph_FaceId aFaceId(0);
  EXPECT_TRUE(myGraph.Shapes().HasOriginal(aFaceId));
}

TEST_F(BRepGraph_ViewsTest, ShapesView_FindOriginal_ValidNode_ReturnsPointer)
{
  const BRepGraph_FaceId aFaceId(0);
  const TopoDS_Shape*    aFound = myGraph.Shapes().FindOriginal(aFaceId);
  const TopoDS_Shape&    anOrig = myGraph.Shapes().OriginalOf(aFaceId);
  ASSERT_NE(aFound, nullptr);
  EXPECT_TRUE(aFound->IsSame(anOrig));
}

TEST_F(BRepGraph_ViewsTest, ShapesView_FindOriginal_InvalidNode_ReturnsNull)
{
  EXPECT_EQ(myGraph.Shapes().FindOriginal(BRepGraph_NodeId()), nullptr);
}

TEST_F(BRepGraph_ViewsTest, ShapesView_OriginalOf_InvalidNode_Throws)
{
#ifndef No_Exception
  EXPECT_THROW((void)myGraph.Shapes().OriginalOf(BRepGraph_NodeId()), Standard_ProgramError);
#endif
}

TEST_F(BRepGraph_ViewsTest, ShapesView_RemovedNode_OriginalQueries_AreUnavailable)
{
  const BRepGraph_FaceId aFaceId(0);
  ASSERT_TRUE(myGraph.Shapes().HasOriginal(aFaceId));

  myGraph.Editor().Gen().RemoveNode(aFaceId);

  EXPECT_FALSE(myGraph.Shapes().HasOriginal(aFaceId));
  EXPECT_EQ(myGraph.Shapes().FindOriginal(aFaceId), nullptr);
  EXPECT_TRUE(myGraph.Shapes().Shape(aFaceId).IsNull());
#ifndef No_Exception
  EXPECT_THROW((void)myGraph.Shapes().OriginalOf(aFaceId), Standard_ProgramError);
#endif
}

TEST_F(BRepGraph_ViewsTest, ShapesView_Reconstruct_InvalidNode_ReturnsNull)
{
  EXPECT_TRUE(myGraph.Shapes().Reconstruct(BRepGraph_NodeId()).IsNull());
}

TEST_F(BRepGraph_ViewsTest, ShapesView_Reconstruct_RemovedNode_ReturnsNull)
{
  const BRepGraph_FaceId aFaceId(0);
  myGraph.Editor().Gen().RemoveNode(aFaceId);
  EXPECT_TRUE(myGraph.Shapes().Reconstruct(aFaceId).IsNull());
}

TEST_F(BRepGraph_ViewsTest, ShapesView_FindNodeAndHasNode_RemovedNode_AreUnavailable)
{
  const BRepGraph_FaceId aFaceId(0);
  const TopoDS_Shape*    aOrig = myGraph.Shapes().FindOriginal(aFaceId);
  ASSERT_NE(aOrig, nullptr);
  const TopoDS_Shape aFaceShape = *aOrig;

  ASSERT_TRUE(myGraph.Shapes().HasNode(aFaceShape));
  ASSERT_EQ(myGraph.Shapes().FindNode(aFaceShape), BRepGraph_NodeId(aFaceId));

  myGraph.Editor().Gen().RemoveNode(aFaceId);

  EXPECT_FALSE(myGraph.Shapes().HasNode(aFaceShape));
  EXPECT_FALSE(myGraph.Shapes().FindNode(aFaceShape).IsValid());
}

// ---------- MutView ----------

TEST_F(BRepGraph_ViewsTest, MutView_EdgeDef_IncrementsOwnGen)
{
  {
    BRepGraph_MutGuard<BRepGraphInc::EdgeDef> anEdge =
      myGraph.Editor().Edges().Mut(BRepGraph_EdgeId::Start());
  }
  EXPECT_GT(myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).OwnGen, 0u);
}

TEST_F(BRepGraph_ViewsTest, MutView_InvalidNode_ThrowsProgramError)
{
#ifndef No_Exception
  EXPECT_THROW((void)myGraph.Editor().Faces().Mut(BRepGraph_FaceId(777777)), Standard_ProgramError);
#endif
}

TEST_F(BRepGraph_ViewsTest, MutView_RemovedNode_ThrowsProgramError)
{
#ifndef No_Exception
  const BRepGraph_FaceId aFaceId(0);
  myGraph.Editor().Gen().RemoveNode(aFaceId);
  EXPECT_THROW((void)myGraph.Editor().Faces().Mut(aFaceId), Standard_ProgramError);
#endif
}

TEST_F(BRepGraph_ViewsTest, MutView_RemovedRef_ThrowsProgramError)
{
  const NCollection_DynamicArray<BRepGraph_FaceRefId>& aFaceRefs =
    myGraph.Refs().Faces().IdsOf(BRepGraph_ShellId::Start());
  ASSERT_GT(aFaceRefs.Length(), 0);
  const BRepGraph_FaceRefId aFaceRefId = aFaceRefs.Value(0);

  ASSERT_TRUE(myGraph.Editor().Gen().RemoveRef(aFaceRefId));
#ifndef No_Exception
  EXPECT_THROW((void)myGraph.Editor().Faces().MutRef(aFaceRefId), Standard_ProgramError);
#endif
}

TEST_F(BRepGraph_ViewsTest, MutView_RemovedRep_ThrowsProgramError)
{
  const BRepGraph_SurfaceRepId aSurfaceRepId =
    myGraph.Topo().Faces().SurfaceRepId(BRepGraph_FaceId::Start());
  ASSERT_TRUE(aSurfaceRepId.IsValid());

  myGraph.Editor().Gen().RemoveRep(aSurfaceRepId);
#ifndef No_Exception
  EXPECT_THROW((void)myGraph.Editor().Reps().MutSurface(aSurfaceRepId), Standard_ProgramError);
#endif
}

// ---------- EditorView ----------

TEST_F(BRepGraph_ViewsTest, EditorView_AddVertex_Works)
{
  const int          aNbBefore = myGraph.Topo().Vertices().Nb();
  BRepGraph_VertexId aVtx      = myGraph.Editor().Vertices().Add(gp_Pnt(1, 2, 3), 0.001);
  EXPECT_TRUE(aVtx.IsValid());
  EXPECT_EQ(myGraph.Topo().Vertices().Nb(), aNbBefore + 1);
}

TEST_F(BRepGraph_ViewsTest, EditorView_IsRemoved_False)
{
  BRepGraph_FaceId aFaceId(0);
  EXPECT_FALSE(myGraph.Topo().Gen().IsRemoved(aFaceId));
}

TEST_F(BRepGraph_ViewsTest, EditorView_RemoveRep_Surface_HidesSurfaceQueries)
{
  const BRepGraph_FaceId       aFaceId(0);
  const BRepGraph_SurfaceRepId aSurfaceRepId = myGraph.Topo().Faces().SurfaceRepId(aFaceId);
  ASSERT_TRUE(aSurfaceRepId.IsValid());
  ASSERT_TRUE(BRepGraph_Tool::Face::HasSurface(myGraph, aFaceId));

  myGraph.Editor().Gen().RemoveRep(aSurfaceRepId);

  EXPECT_TRUE(myGraph.Topo().Geometry().SurfaceRep(aSurfaceRepId).IsRemoved);
  EXPECT_FALSE(myGraph.Topo().Faces().SurfaceRepId(aFaceId).IsValid());
  EXPECT_FALSE(BRepGraph_Tool::Face::HasSurface(myGraph, aFaceId));
  EXPECT_TRUE(BRepGraph_Tool::Face::Surface(myGraph, aFaceId).IsNull());
}

TEST_F(BRepGraph_ViewsTest, EditorView_RemoveRep_CurveAndPCurve_HideCurveQueries)
{
  const BRepGraph_EdgeId       anEdgeId(0);
  const BRepGraph_Curve3DRepId aCurve3DRepId = myGraph.Topo().Edges().Curve3DRepId(anEdgeId);
  ASSERT_TRUE(aCurve3DRepId.IsValid());
  ASSERT_TRUE(BRepGraph_Tool::Edge::HasCurve(myGraph, anEdgeId));

  myGraph.Editor().Gen().RemoveRep(aCurve3DRepId);

  EXPECT_TRUE(myGraph.Topo().Geometry().Curve3DRep(aCurve3DRepId).IsRemoved);
  EXPECT_FALSE(myGraph.Topo().Edges().Curve3DRepId(anEdgeId).IsValid());
  EXPECT_FALSE(BRepGraph_Tool::Edge::HasCurve(myGraph, anEdgeId));
  EXPECT_TRUE(BRepGraph_Tool::Edge::Curve(myGraph, anEdgeId).IsNull());

  const NCollection_DynamicArray<BRepGraph_CoEdgeId>& aCoEdges = myGraph.Topo().Edges().CoEdges(anEdgeId);
  ASSERT_GT(aCoEdges.Length(), 0);
  const BRepGraph_CoEdgeId     aCoEdgeId     = aCoEdges.Value(0);
  const BRepGraph_Curve2DRepId aCurve2DRepId = myGraph.Topo().CoEdges().Curve2DRepId(aCoEdgeId);
  ASSERT_TRUE(aCurve2DRepId.IsValid());
  ASSERT_TRUE(BRepGraph_Tool::CoEdge::HasPCurve(myGraph, aCoEdgeId));

  myGraph.Editor().Gen().RemoveRep(aCurve2DRepId);

  EXPECT_TRUE(myGraph.Topo().Geometry().Curve2DRep(aCurve2DRepId).IsRemoved);
  EXPECT_FALSE(myGraph.Topo().CoEdges().Curve2DRepId(aCoEdgeId).IsValid());
  EXPECT_FALSE(BRepGraph_Tool::CoEdge::HasPCurve(myGraph, aCoEdgeId));
  EXPECT_TRUE(BRepGraph_Tool::CoEdge::PCurve(myGraph, aCoEdgeId).IsNull());
}

// ---------- History() accessor ----------

TEST_F(BRepGraph_ViewsTest, History_ConstAccessor)
{
  const BRepGraph& aConstGraph = myGraph;
  EXPECT_TRUE(aConstGraph.History().IsEnabled());
}

TEST_F(BRepGraph_ViewsTest, History_MutableAccessor)
{
  myGraph.History().SetEnabled(false);
  EXPECT_FALSE(myGraph.History().IsEnabled());
  myGraph.History().SetEnabled(true);
  EXPECT_TRUE(myGraph.History().IsEnabled());
}
