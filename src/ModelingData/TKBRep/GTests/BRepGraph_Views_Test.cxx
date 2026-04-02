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
#include <BRepGraph_CacheView.hxx>
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_UIDsView.hxx>

#include <BRepGraph_Tool.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Precision.hxx>
#include <gp_Pnt.hxx>

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
static int countActiveRefs(const NCollection_Vector<theRefIdType>& theRefIds,
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
    myGraph.Build(aBox);
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
  myGraph.Builder().RemoveNode(BRepGraph_FaceId(0));

  EXPECT_EQ(myGraph.Topo().Faces().NbActive(), aFacesBefore - 1);
  EXPECT_TRUE(myGraph.Topo().Gen().IsRemoved(BRepGraph_FaceId(0)));
}

TEST_F(BRepGraph_ViewsTest, DefsView_FaceAccessor_Valid)
{
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    EXPECT_TRUE(aFaceIt.Current().Id.IsValid())
      << "Face " << aFaceIt.CurrentId().Index << " has invalid Id";
  }
}

TEST_F(BRepGraph_ViewsTest, DefsView_TopoEntity_Valid)
{
  BRepGraph_FaceId             aFaceId(0);
  const BRepGraphInc::BaseDef* aBase = myGraph.Topo().Gen().TopoEntity(aFaceId);
  ASSERT_NE(aBase, nullptr);
  EXPECT_EQ(aBase->Id, myGraph.Topo().Faces().Definition(BRepGraph_FaceId(0)).Id);
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
  (void)BRepGraph_Tool::Edge::FindPCurve(myGraph, BRepGraph_EdgeId(0), BRepGraph_FaceId(0));
}

TEST_F(BRepGraph_ViewsTest, DefsView_RepIdConvenienceAccessors_RoundTrip)
{
  const BRepGraph_FaceId aFaceId(0);
  const BRepGraph_EdgeId anEdgeId(0);

  EXPECT_EQ(myGraph.Topo().Faces().SurfaceRepId(aFaceId),
            myGraph.Topo().Faces().Definition(aFaceId).SurfaceRepId);
  EXPECT_EQ(myGraph.Topo().Faces().ActiveTriangulationRepId(aFaceId),
            myGraph.Topo().Faces().Definition(aFaceId).ActiveTriangulationRepId());
  EXPECT_EQ(myGraph.Topo().Edges().Curve3DRepId(anEdgeId),
            myGraph.Topo().Edges().Definition(anEdgeId).Curve3DRepId);

  const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdges = myGraph.Topo().Edges().CoEdges(anEdgeId);
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
  NCollection_Vector<BRepGraph_UID> aUIDs;
  const BRepGraph_UID               aFaceUID  = myGraph.UIDs().Of(BRepGraph_FaceId(0));
  const BRepGraph_UID               anEdgeUID = myGraph.UIDs().Of(BRepGraph_EdgeId(0));
  ASSERT_TRUE(aFaceUID.IsValid());
  ASSERT_TRUE(anEdgeUID.IsValid());

  aUIDs.Append(aFaceUID);
  aUIDs.Append(anEdgeUID);

  ASSERT_EQ(aUIDs.Length(), 2);
  EXPECT_EQ(myGraph.UIDs().NodeIdFrom(aUIDs.Value(0)), BRepGraph_NodeId(BRepGraph_FaceId(0)));
  EXPECT_EQ(myGraph.UIDs().NodeIdFrom(aUIDs.Value(1)), BRepGraph_NodeId(BRepGraph_EdgeId(0)));
}

TEST_F(BRepGraph_ViewsTest, UIDsView_NodeIdFrom_InvalidAndWrongGeneration)
{
  NCollection_Vector<BRepGraph_UID> aUIDs;
  aUIDs.Append(BRepGraph_UID());
  aUIDs.Append(BRepGraph_UID(BRepGraph_NodeId::Kind::Face, 1, myGraph.UIDs().Generation() + 1));

  ASSERT_EQ(aUIDs.Length(), 2);
  EXPECT_FALSE(myGraph.UIDs().NodeIdFrom(aUIDs.Value(0)).IsValid());
  EXPECT_FALSE(myGraph.UIDs().NodeIdFrom(aUIDs.Value(1)).IsValid());
}

// ---------- Topology adjacency ----------

TEST_F(BRepGraph_ViewsTest, SpatialView_AdjacentFaces_FourPerBoxFace)
{
  BRepGraph_FaceId                     aFaceId(0);
  NCollection_Vector<BRepGraph_FaceId> aResult =
    myGraph.Topo().Faces().Adjacent(aFaceId, myGraph.Allocator());
  EXPECT_EQ(aResult.Length(), 4);
}

TEST_F(BRepGraph_ViewsTest, SpatialView_FacesOfEdge_TwoPerBoxEdge)
{
  BRepGraph_EdgeId                            anEdgeId(0);
  const NCollection_Vector<BRepGraph_FaceId>& aResult = myGraph.Topo().Edges().Faces(anEdgeId);
  EXPECT_EQ(aResult.Length(), 2);
}

TEST_F(BRepGraph_ViewsTest, SpatialView_OutParam_Parity)
{
  const BRepGraph_FaceId                        aFaceId(0);
  const BRepGraph_EdgeId                        anEdgeId(0);
  const BRepGraph_VertexId                      aVertexId(0);
  const occ::handle<NCollection_BaseAllocator>& anAllocator = myGraph.Allocator();

  const NCollection_Vector<BRepGraph_FaceId> aAdjacentByValue =
    myGraph.Topo().Faces().Adjacent(aFaceId, anAllocator);
  EXPECT_EQ(aAdjacentByValue.Length(), 4);

  const NCollection_Vector<BRepGraph_EdgeId> anAdjEdgesByValue =
    myGraph.Topo().Edges().Adjacent(anEdgeId, anAllocator);
  EXPECT_GE(anAdjEdgesByValue.Length(), 4);
}

TEST_F(BRepGraph_ViewsTest, TopoView_GroupedFaceOps_Parity)
{
  const BRepGraph_FaceId                        aFaceId(0);
  const occ::handle<NCollection_BaseAllocator>& anAllocator = myGraph.Allocator();

  EXPECT_EQ(myGraph.Topo().Faces().Definition(aFaceId).Id, BRepGraph_NodeId(aFaceId));
  EXPECT_EQ(myGraph.Topo().Faces().SurfaceRepId(aFaceId),
            myGraph.Topo().Faces().Definition(aFaceId).SurfaceRepId);
  EXPECT_EQ(myGraph.Topo().Faces().ActiveTriangulationRepId(aFaceId),
            myGraph.Topo().Faces().Definition(aFaceId).ActiveTriangulationRepId());
  EXPECT_EQ(myGraph.Topo().Faces().OuterWire(aFaceId), BRepGraph_WireId(0));

  EXPECT_EQ(myGraph.Topo().Faces().Adjacent(aFaceId, anAllocator).Length(), 4);
}

TEST_F(BRepGraph_ViewsTest, TopoView_GroupedEdgeAndVertexOps_Parity)
{
  const BRepGraph_EdgeId                        anEdgeId(0);
  const BRepGraph_VertexId                      aVertexId(0);
  const occ::handle<NCollection_BaseAllocator>& anAllocator = myGraph.Allocator();

  EXPECT_EQ(myGraph.Topo().Edges().Definition(anEdgeId).Id, BRepGraph_NodeId(anEdgeId));
  EXPECT_EQ(myGraph.Topo().Edges().NbFaces(anEdgeId), 2);
  EXPECT_EQ(myGraph.Topo().Edges().Curve3DRepId(anEdgeId),
            myGraph.Topo().Edges().Definition(anEdgeId).Curve3DRepId);
  EXPECT_FALSE(myGraph.Topo().Edges().IsBoundary(anEdgeId));
  EXPECT_TRUE(myGraph.Topo().Edges().IsManifold(anEdgeId));
  EXPECT_GE(myGraph.Topo().Edges().Wires(anEdgeId).Length(), 1);
  EXPECT_GE(myGraph.Topo().Edges().CoEdges(anEdgeId).Length(), 1);
  EXPECT_EQ(myGraph.Topo().Edges().Faces(anEdgeId).Length(), 2);
  EXPECT_GE(myGraph.Topo().Edges().Adjacent(anEdgeId, anAllocator).Length(), 4);

  EXPECT_EQ(myGraph.Topo().Vertices().Definition(aVertexId).Id, BRepGraph_NodeId(aVertexId));
  EXPECT_GE(myGraph.Topo().Vertices().Edges(aVertexId).Length(), 1);
}

TEST_F(BRepGraph_ViewsTest, TopoView_GroupedCoEdgeOps_Parity)
{
  const BRepGraph_CoEdgeId       aCoEdgeId(0);
  const BRepGraphInc::CoEdgeDef& aCoEdge = myGraph.Topo().CoEdges().Definition(aCoEdgeId);

  EXPECT_EQ(myGraph.Topo().CoEdges().Definition(aCoEdgeId).Id, aCoEdge.Id);
  EXPECT_EQ(myGraph.Topo().CoEdges().Definition(aCoEdgeId).EdgeDefId, aCoEdge.EdgeDefId);
  EXPECT_EQ(myGraph.Topo().CoEdges().Definition(aCoEdgeId).FaceDefId, aCoEdge.FaceDefId);
  EXPECT_EQ(myGraph.Topo().CoEdges().Curve2DRepId(aCoEdgeId),
            myGraph.Topo().CoEdges().Definition(aCoEdgeId).Curve2DRepId);
  EXPECT_EQ(myGraph.Topo().CoEdges().Definition(aCoEdgeId).SeamPairId, aCoEdge.SeamPairId);
}

TEST_F(BRepGraph_ViewsTest, TopoView_GroupedProductAndOccurrenceOps_Parity)
{
  const BRepGraph_ProductId aPartProduct =
    myGraph.Builder().AddProduct(BRepGraph_NodeId(BRepGraph_SolidId(0)));
  const BRepGraph_ProductId aSubAssembly  = myGraph.Builder().AddAssemblyProduct();
  const BRepGraph_ProductId aRootAssembly = myGraph.Builder().AddAssemblyProduct();
  ASSERT_TRUE(aPartProduct.IsValid());
  ASSERT_TRUE(aSubAssembly.IsValid());
  ASSERT_TRUE(aRootAssembly.IsValid());

  const BRepGraph_OccurrenceId aSubOccurrence =
    myGraph.Builder().AddOccurrence(aRootAssembly, aSubAssembly, TopLoc_Location());
  const BRepGraph_OccurrenceId aPartOccurrence =
    myGraph.Builder().AddOccurrence(aSubAssembly, aPartProduct, TopLoc_Location(), aSubOccurrence);
  ASSERT_TRUE(aSubOccurrence.IsValid());
  ASSERT_TRUE(aPartOccurrence.IsValid());

  EXPECT_EQ(myGraph.Topo().Products().Definition(aPartProduct).Id, BRepGraph_NodeId(aPartProduct));
  EXPECT_EQ(myGraph.Topo().Products().Definition(aPartProduct).ShapeRootId,
            BRepGraph_NodeId(BRepGraph_SolidId(0)));
  EXPECT_FALSE(myGraph.Topo().Products().Definition(aRootAssembly).ShapeRootId.IsValid());
  EXPECT_EQ(myGraph.Refs().Occurrences().IdsOf(aRootAssembly).Length(), 1);
  EXPECT_EQ(myGraph.Refs().Occurrences().IdsOf(aSubAssembly).Length(), 1);

  EXPECT_EQ(myGraph.Topo().Occurrences().Definition(aPartOccurrence).Id,
            BRepGraph_NodeId(aPartOccurrence));
  EXPECT_EQ(myGraph.Topo().Occurrences().Definition(aPartOccurrence).ProductDefId, aPartProduct);
  EXPECT_EQ(myGraph.Topo().Occurrences().Definition(aPartOccurrence).ParentProductDefId,
            aSubAssembly);
  EXPECT_EQ(myGraph.Topo().Occurrences().Definition(aPartOccurrence).ParentOccurrenceDefId,
            aSubOccurrence);

  const NCollection_Vector<BRepGraph_OccurrenceRefId>& aOccurrenceRefs =
    myGraph.Refs().Occurrences().IdsOf(aSubAssembly);
  ASSERT_EQ(aOccurrenceRefs.Length(), 1);
  {
    BRepGraph_MutGuard<BRepGraphInc::OccurrenceRef> anOccurrenceRef =
      myGraph.Builder().MutOccurrenceRef(aOccurrenceRefs.Value(0));
    anOccurrenceRef->IsRemoved = true;
  }

  EXPECT_EQ(myGraph.Topo().Products().NbComponents(aSubAssembly), 0);
}

TEST_F(BRepGraph_ViewsTest, SpatialView_OutParam_ClearAndInvalid)
{
  const occ::handle<NCollection_BaseAllocator>& anAllocator = myGraph.Allocator();

  const NCollection_Vector<BRepGraph_FaceId> aFaceResult =
    myGraph.Topo().Faces().Adjacent(BRepGraph_FaceId(0), anAllocator);
  EXPECT_EQ(aFaceResult.Length(), 4);
  EXPECT_EQ(myGraph.Topo().Faces().Adjacent(BRepGraph_FaceId(999), anAllocator).Length(), 0);

  const NCollection_Vector<BRepGraph_EdgeId> anAdjEdgeResult =
    myGraph.Topo().Edges().Adjacent(BRepGraph_EdgeId(0), anAllocator);
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

  const NCollection_Vector<occ::handle<BRepGraph_CacheKind>> aKinds =
    myGraph.Cache().CacheKinds(aFaceId);

  ASSERT_EQ(aKinds.Length(), 1);
  ASSERT_FALSE(aKinds.Value(0).IsNull());
  EXPECT_EQ(aKinds.Value(0)->ID(), testUserAttrKind()->ID());
  EXPECT_EQ(myGraph.Cache().Get(aFaceId, testUserAttrKind()), anAttr);
}

TEST_F(BRepGraph_ViewsTest, AttrsView_MutFace_InvalidatesEntry)
{
  BRepGraph_FaceId                  aFaceId(0);
  occ::handle<BRepGraph_CacheValue> anAttr = new TestCacheValue();
  myGraph.Cache().Set(aFaceId, testUserAttrKind(), anAttr);
  ASSERT_TRUE(myGraph.Cache().Has(aFaceId, testUserAttrKind()));

  {
    BRepGraph_MutGuard<BRepGraphInc::FaceDef> aFace =
      myGraph.Builder().MutFace(BRepGraph_FaceId(0));
    aFace->Tolerance += 0.1;
  }

  EXPECT_FALSE(myGraph.Cache().Has(aFaceId, testUserAttrKind()));
  EXPECT_TRUE(myGraph.Cache().Get(aFaceId, testUserAttrKind()).IsNull());
}

TEST_F(BRepGraph_ViewsTest, AttrsView_RemoveNode_InvalidatesEntry)
{
  BRepGraph_FaceId                  aFaceId(0);
  occ::handle<BRepGraph_CacheValue> anAttr = new TestCacheValue();
  myGraph.Cache().Set(aFaceId, testUserAttrKind(), anAttr);
  ASSERT_TRUE(myGraph.Cache().Has(aFaceId, testUserAttrKind()));

  myGraph.Builder().RemoveNode(aFaceId);

  EXPECT_TRUE(myGraph.Topo().Gen().IsRemoved(aFaceId));
  EXPECT_FALSE(myGraph.Cache().Has(aFaceId, testUserAttrKind()));
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
  const NCollection_Vector<BRepGraph_FaceRefId>& aFaceRefs = myGraph.Refs().Faces().IdsOf(aShellId);
  ASSERT_GT(aFaceRefs.Length(), 0);

  {
    BRepGraph_MutGuard<BRepGraphInc::FaceRef> aFaceRef =
      myGraph.Builder().MutFaceRef(aFaceRefs.Value(0));
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
  const occ::handle<NCollection_BaseAllocator>&   anAllocator = myGraph.Allocator();
  const NCollection_Vector<BRepGraph_VertexRefId> aVertexRefs =
    myGraph.Refs().Vertices().IdsOf(BRepGraph_EdgeId(0), anAllocator);

  EXPECT_GE(aVertexRefs.Length(), 2);
  for (const BRepGraph_VertexRefId& aVertexRefId : aVertexRefs)
  {
    const BRepGraphInc::VertexRef& aRef = myGraph.Refs().Vertices().Entry(aVertexRefId);
    EXPECT_FALSE(aRef.IsRemoved);
    EXPECT_TRUE(aRef.VertexDefId.IsValid(myGraph.Topo().Vertices().Nb()));
  }
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

// ---------- MutView ----------

TEST_F(BRepGraph_ViewsTest, MutView_EdgeDef_IncrementsOwnGen)
{
  {
    BRepGraph_MutGuard<BRepGraphInc::EdgeDef> anEdge =
      myGraph.Builder().MutEdge(BRepGraph_EdgeId(0));
  }
  EXPECT_GT(myGraph.Topo().Edges().Definition(BRepGraph_EdgeId(0)).OwnGen, 0u);
}

// ---------- BuilderView ----------

TEST_F(BRepGraph_ViewsTest, BuilderView_AddVertex_Works)
{
  const int          aNbBefore = myGraph.Topo().Vertices().Nb();
  BRepGraph_VertexId aVtx      = myGraph.Builder().AddVertex(gp_Pnt(1, 2, 3), 0.001);
  EXPECT_TRUE(aVtx.IsValid());
  EXPECT_EQ(myGraph.Topo().Vertices().Nb(), aNbBefore + 1);
}

TEST_F(BRepGraph_ViewsTest, BuilderView_IsRemoved_False)
{
  BRepGraph_FaceId aFaceId(0);
  EXPECT_FALSE(myGraph.Topo().Gen().IsRemoved(aFaceId));
}

TEST_F(BRepGraph_ViewsTest, BuilderView_RemoveRep_Surface_HidesSurfaceQueries)
{
  const BRepGraph_FaceId       aFaceId(0);
  const BRepGraph_SurfaceRepId aSurfaceRepId = myGraph.Topo().Faces().SurfaceRepId(aFaceId);
  ASSERT_TRUE(aSurfaceRepId.IsValid());
  ASSERT_TRUE(BRepGraph_Tool::Face::HasSurface(myGraph, aFaceId));

  myGraph.Builder().RemoveRep(aSurfaceRepId);

  EXPECT_TRUE(myGraph.Topo().Geometry().SurfaceRep(aSurfaceRepId).IsRemoved);
  EXPECT_FALSE(myGraph.Topo().Faces().SurfaceRepId(aFaceId).IsValid());
  EXPECT_FALSE(BRepGraph_Tool::Face::HasSurface(myGraph, aFaceId));
  EXPECT_TRUE(BRepGraph_Tool::Face::Surface(myGraph, aFaceId).IsNull());
}

TEST_F(BRepGraph_ViewsTest, BuilderView_RemoveRep_CurveAndPCurve_HideCurveQueries)
{
  const BRepGraph_EdgeId       anEdgeId(0);
  const BRepGraph_Curve3DRepId aCurve3DRepId = myGraph.Topo().Edges().Curve3DRepId(anEdgeId);
  ASSERT_TRUE(aCurve3DRepId.IsValid());
  ASSERT_TRUE(BRepGraph_Tool::Edge::HasCurve(myGraph, anEdgeId));

  myGraph.Builder().RemoveRep(aCurve3DRepId);

  EXPECT_TRUE(myGraph.Topo().Geometry().Curve3DRep(aCurve3DRepId).IsRemoved);
  EXPECT_FALSE(myGraph.Topo().Edges().Curve3DRepId(anEdgeId).IsValid());
  EXPECT_FALSE(BRepGraph_Tool::Edge::HasCurve(myGraph, anEdgeId));
  EXPECT_TRUE(BRepGraph_Tool::Edge::Curve(myGraph, anEdgeId).IsNull());

  const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdges = myGraph.Topo().Edges().CoEdges(anEdgeId);
  ASSERT_GT(aCoEdges.Length(), 0);
  const BRepGraph_CoEdgeId     aCoEdgeId     = aCoEdges.Value(0);
  const BRepGraph_Curve2DRepId aCurve2DRepId = myGraph.Topo().CoEdges().Curve2DRepId(aCoEdgeId);
  ASSERT_TRUE(aCurve2DRepId.IsValid());
  ASSERT_TRUE(BRepGraph_Tool::CoEdge::HasPCurve(myGraph, aCoEdgeId));

  myGraph.Builder().RemoveRep(aCurve2DRepId);

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
