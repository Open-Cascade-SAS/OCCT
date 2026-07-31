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
#include <BRepGraph_Cache.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_CacheRegistry.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraph_RelatedIterator.hxx>
#include <BRepGraph_ReverseIterator.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_LayerHistory.hxx>
#include <BRepGraph_LayerTopoSupplement.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_UIDsView.hxx>

#include <BRepGraph_Tool.hxx>
#include <BRepGraph_Compact.hxx>
#include <BRepGraph_Deduplicate.hxx>
#include <BRepGraph_Validate.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <Geom2d_Line.hxx>
#include <Precision.hxx>
#include <Standard_ProgramError.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Wire.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_LinearVector.hxx>

#include <tuple>

#include <gtest/gtest.h>

namespace
{
class TestCacheService : public BRepGraph_Cache
{
public:
  DEFINE_STANDARD_RTTI_INLINE(TestCacheService, BRepGraph_Cache)

  static const Standard_GUID& GetID()
  {
    static const Standard_GUID THE_ID("2f9b6a5c-1f2d-4a88-9c1c-7a0c16a10020");
    return THE_ID;
  }

  const Standard_GUID& ID() const override { return GetID(); }

  const TCollection_AsciiString& Name() const override
  {
    static const TCollection_AsciiString THE_NAME("ViewsTestCacheService");
    return THE_NAME;
  }

  bool Attached() const { return IsAttached(); }

  void Set(const BRepGraph_NodeId theNode, const int theValue)
  {
    if (myValues.IsBound(theNode))
    {
      myValues.ChangeFind(theNode) = theValue;
    }
    else
    {
      myValues.Bind(theNode, theValue);
    }
  }

  bool Get(const BRepGraph_NodeId theNode, int& theValue) const
  {
    const int* aValue = myValues.Seek(theNode);
    if (aValue == nullptr)
    {
      return false;
    }
    theValue = *aValue;
    return true;
  }

  void Clear() noexcept override
  {
    ++myClearCount;
    myValues.Clear();
  }

  int ClearCount() const { return myClearCount; }

private:
  NCollection_DataMap<BRepGraph_NodeId, int> myValues;
  int                                        myClearCount = 0;
};

template <class theRefContainerType>
static uint32_t countActiveRefs(const BRepGraph& theGraph, const theRefContainerType& theRefIds)
{
  uint32_t aCount = 0;
  for (const auto& aRefId : theRefIds)
  {
    if (!aRefId.IsRemoved(theGraph))
    {
      ++aCount;
    }
  }
  return aCount;
}

template <class theNodeIdContainerType>
static uint32_t countActiveNodeIds(const BRepGraph&              theGraph,
                                   const theNodeIdContainerType& theNodeIds)
{
  uint32_t aCount = 0;
  for (const auto& aNodeId : theNodeIds)
  {
    if (!theGraph.Topo().Gen().IsRemoved(BRepGraph_NodeId(aNodeId)))
    {
      ++aCount;
    }
  }
  return aCount;
}

static uint32_t countActiveNodes(const BRepGraph&             theGraph,
                                 const BRepGraph_NodeId::Kind theKind,
                                 const uint32_t               theUpperBound)
{
  uint32_t aCount = 0;
  for (uint32_t anIdx = 0; anIdx < theUpperBound; ++anIdx)
  {
    if (!theGraph.Topo().Gen().IsRemoved(BRepGraph_NodeId(theKind, anIdx)))
    {
      ++aCount;
    }
  }
  return aCount;
}

template <class theIteratorType>
static uint32_t countIterator(theIteratorType theIterator)
{
  uint32_t aCount = 0;
  for (; theIterator.More(); theIterator.Next())
  {
    ++aCount;
  }
  return aCount;
}

static uint32_t countRelated(const BRepGraph&                              theGraph,
                             const BRepGraph_NodeId                        theNode,
                             const BRepGraph_RelatedIterator::RelationKind theKind)
{
  uint32_t aCount = 0;
  for (BRepGraph_RelatedIterator anIt(theGraph, theNode); anIt.More(); anIt.Next())
  {
    if (anIt.CurrentRelation() == theKind)
    {
      ++aCount;
    }
  }
  return aCount;
}

static BRepGraph_FaceId firstFaceOfEdge(const BRepGraph& theGraph, const BRepGraph_EdgeId theEdge)
{
  BRepGraph_FacesOfEdge aFaceIt = theGraph.Topo().Edges().FacesOf(theEdge);
  return aFaceIt.More() ? aFaceIt.CurrentId() : BRepGraph_FaceId();
}

static bool edgeHasFace(const BRepGraph&       theGraph,
                        const BRepGraph_EdgeId theEdge,
                        const BRepGraph_FaceId theFace)
{
  for (BRepGraph_FacesOfEdge aFaceIt = theGraph.Topo().Edges().FacesOf(theEdge); aFaceIt.More();
       aFaceIt.Next())
  {
    if (aFaceIt.CurrentId() == theFace)
    {
      return true;
    }
  }
  return false;
}

static bool containsEdge(const NCollection_LinearVector<BRepGraph_EdgeId>& theEdges,
                         const BRepGraph_EdgeId                            theEdge)
{
  for (const BRepGraph_EdgeId& anEdge : theEdges)
  {
    if (anEdge == theEdge)
    {
      return true;
    }
  }
  return false;
}

static uint32_t countAdjacentEdgesOfEdge(const BRepGraph& theGraph, const BRepGraph_EdgeId theEdge)
{
  if (!theEdge.IsValid(theGraph.Topo().Edges().Nb()) || theEdge.IsRemoved(theGraph))
  {
    return 0;
  }

  NCollection_LinearVector<BRepGraph_EdgeId> anAdjacentEdges;
  for (BRepGraph_DefsVertexOfEdge aVertexIt(theGraph, theEdge); aVertexIt.More(); aVertexIt.Next())
  {
    const BRepGraph_VertexId aVertexId = aVertexIt.CurrentId();
    for (const BRepGraph_EdgeId& anAdjacentEdgeId : theGraph.Topo().Vertices().Edges(aVertexId))
    {
      if (anAdjacentEdgeId == theEdge || anAdjacentEdgeId.IsRemoved(theGraph)
          || containsEdge(anAdjacentEdges, anAdjacentEdgeId))
      {
        continue;
      }
      anAdjacentEdges.Append(anAdjacentEdgeId);
    }
  }
  return static_cast<uint32_t>(anAdjacentEdges.Size());
}
} // namespace

class BRepGraph_ViewsTest : public testing::Test
{
protected:
  void SetUp() override
  {
    BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
    const TopoDS_Shape& aBox = aBoxMaker.Shape();
    myGraph.Clear();
    [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes1 = myGraph.Shapes().Add(aBox);
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
  const uint32_t aFacesBefore = myGraph.Topo().Faces().NbActive();
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

TEST_F(BRepGraph_ViewsTest, TopoView_GenValidity)
{
  const BRepGraph_NodeId aFaceId(BRepGraph_FaceId::Start());
  EXPECT_EQ(myGraph.Topo().Gen().Nb(BRepGraph_NodeId::Kind::Face), myGraph.Topo().Faces().Nb());
  EXPECT_TRUE(myGraph.Topo().Gen().IsValid(aFaceId));
  EXPECT_TRUE(myGraph.Topo().Gen().IsActive(aFaceId));
  EXPECT_FALSE(myGraph.Topo().Gen().IsRemoved(aFaceId));

  myGraph.Editor().Gen().RemoveNode(aFaceId);
  EXPECT_TRUE(myGraph.Topo().Gen().IsValid(aFaceId));
  EXPECT_FALSE(myGraph.Topo().Gen().IsActive(aFaceId));
  EXPECT_TRUE(myGraph.Topo().Gen().IsRemoved(aFaceId));

  const BRepGraph_NodeId anOutOfRangeFace(BRepGraph_NodeId::Kind::Face,
                                          myGraph.Topo().Faces().Nb());
  EXPECT_FALSE(myGraph.Topo().Gen().IsValid(anOutOfRangeFace));
  EXPECT_FALSE(myGraph.Topo().Gen().IsActive(anOutOfRangeFace));
  EXPECT_TRUE(myGraph.Topo().Gen().IsRemoved(anOutOfRangeFace));
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

TEST_F(BRepGraph_ViewsTest, DefsView_FindPCurveCoEdgeId_NoCrash)
{
  // FindPCurveCoEdgeId may or may not return a valid id for an arbitrary edge/face pair.
  // Just verify it does not crash.
  std::ignore = BRepGraph_Tool::Edge::FindPCurveCoEdgeId(myGraph,
                                                         BRepGraph_EdgeId::Start(),
                                                         BRepGraph_FaceId::Start());
}

TEST_F(BRepGraph_ViewsTest, DefsView_RepIdConvenienceAccessors_RoundTrip)
{
  const BRepGraph_FaceId aFaceId(0);
  const BRepGraph_EdgeId anEdgeId(0);

  EXPECT_FALSE(myGraph.Topo().Faces().Surface(aFaceId).IsNull());
  EXPECT_FALSE(myGraph.Topo().Edges().Curve3D(anEdgeId).IsNull());

  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdges =
    myGraph.Topo().Edges().CoEdges(anEdgeId);
  ASSERT_GT(aCoEdges.Size(), 0);
  const BRepGraph_CoEdgeId aCoEdgeId = aCoEdges.Value(0);
  EXPECT_FALSE(myGraph.Topo().CoEdges().Curve2D(aCoEdgeId).IsNull());
}

TEST_F(BRepGraph_ViewsTest, DefsView_RepIdConvenienceAccessors_InvalidInput)
{
  const BRepGraph_FaceId   aFaceOut(myGraph.Topo().Faces().Nb());
  const BRepGraph_EdgeId   anEdgeOut(myGraph.Topo().Edges().Nb());
  const BRepGraph_CoEdgeId aCoEdgeOut(myGraph.Topo().CoEdges().Nb());

  EXPECT_TRUE(myGraph.Topo().Faces().Surface(aFaceOut).IsNull());
  EXPECT_TRUE(myGraph.Topo().Faces().ActiveTriangulation(aFaceOut).IsNull());
  EXPECT_TRUE(myGraph.Topo().Edges().Curve3D(anEdgeOut).IsNull());
  EXPECT_TRUE(myGraph.Topo().CoEdges().Curve2D(aCoEdgeOut).IsNull());
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
  const BRepGraph_UID                     aFaceUID  = myGraph.UIDs().Of(BRepGraph_FaceId::Start());
  const BRepGraph_UID                     anEdgeUID = myGraph.UIDs().Of(BRepGraph_EdgeId::Start());
  ASSERT_TRUE(aFaceUID.IsValid());
  ASSERT_TRUE(anEdgeUID.IsValid());

  aUIDs.Append(aFaceUID);
  aUIDs.Append(anEdgeUID);

  ASSERT_EQ(aUIDs.Size(), 2);
  EXPECT_EQ(myGraph.UIDs().NodeIdFrom(aUIDs.Value(0)), BRepGraph_NodeId(BRepGraph_FaceId::Start()));
  EXPECT_EQ(myGraph.UIDs().NodeIdFrom(aUIDs.Value(1)), BRepGraph_NodeId(BRepGraph_EdgeId::Start()));
}

TEST_F(BRepGraph_ViewsTest, UIDsView_NodeIdFrom_InvalidAndUnknownUID)
{
  NCollection_DynamicArray<BRepGraph_UID> aUIDs;
  aUIDs.Append(BRepGraph_UID());
  aUIDs.Append(BRepGraph_UID(BRepGraph_NodeId::Kind::Face, 9999));

  ASSERT_EQ(aUIDs.Size(), 2);
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
  const NCollection_LinearVector<BRepGraph_FaceRefId>& aFaceRefs =
    myGraph.Refs().Faces().IdsOf(BRepGraph_ShellId::Start());
  ASSERT_GT(aFaceRefs.Size(), 0);
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
  const NCollection_LinearVector<BRepGraph_FaceRefId>& aFaceRefs =
    myGraph.Refs().Faces().IdsOf(BRepGraph_ShellId::Start());
  ASSERT_GT(aFaceRefs.Size(), 0);
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
  BRepGraph_FaceId aFaceId(0);
  EXPECT_EQ(countRelated(myGraph,
                         BRepGraph_NodeId(aFaceId),
                         BRepGraph_RelatedIterator::RelationKind::AdjacentFace),
            4);
}

TEST_F(BRepGraph_ViewsTest, SpatialView_FacesOfEdge_TwoPerBoxEdge)
{
  BRepGraph_EdgeId anEdgeId(0);
  EXPECT_EQ(countIterator(myGraph.Topo().Edges().FacesOf(anEdgeId)), 2);
}

TEST_F(BRepGraph_ViewsTest, SpatialView_OutParam_Parity)
{
  const BRepGraph_FaceId   aFaceId(0);
  const BRepGraph_EdgeId   anEdgeId(0);
  const BRepGraph_VertexId aVertexId(0);

  EXPECT_EQ(countRelated(myGraph,
                         BRepGraph_NodeId(aFaceId),
                         BRepGraph_RelatedIterator::RelationKind::AdjacentFace),
            4);
  EXPECT_GE(countAdjacentEdgesOfEdge(myGraph, anEdgeId), 4);
}

TEST_F(BRepGraph_ViewsTest, TopoView_GroupedFaceOps_Parity)
{
  const BRepGraph_FaceId aFaceId(0);

  EXPECT_EQ(BRepGraph_NodeId(aFaceId), BRepGraph_NodeId(aFaceId));
  EXPECT_FALSE(myGraph.Topo().Faces().Surface(aFaceId).IsNull());

  EXPECT_EQ(countRelated(myGraph,
                         BRepGraph_NodeId(aFaceId),
                         BRepGraph_RelatedIterator::RelationKind::AdjacentFace),
            4);
}

TEST_F(BRepGraph_ViewsTest, TopoView_GroupedEdgeAndVertexOps_Parity)
{
  const BRepGraph_EdgeId   anEdgeId(0);
  const BRepGraph_VertexId aVertexId(0);

  EXPECT_EQ(BRepGraph_NodeId(anEdgeId), BRepGraph_NodeId(anEdgeId));
  EXPECT_EQ(myGraph.Topo().Edges().NbFaces(anEdgeId), 2);
  EXPECT_FALSE(myGraph.Topo().Edges().Curve3D(anEdgeId).IsNull());
  EXPECT_FALSE(BRepGraph_Tool::Edge::IsBoundary(myGraph, anEdgeId));
  EXPECT_TRUE(BRepGraph_Tool::Edge::IsManifold(myGraph, anEdgeId));
  EXPECT_GE(countIterator(myGraph.Topo().Edges().WiresOf(anEdgeId)), 1);
  EXPECT_GE(myGraph.Topo().Edges().CoEdges(anEdgeId).Size(), 1);
  EXPECT_EQ(countIterator(myGraph.Topo().Edges().FacesOf(anEdgeId)), 2);
  EXPECT_GE(countAdjacentEdgesOfEdge(myGraph, anEdgeId), 4);

  EXPECT_EQ(BRepGraph_NodeId(aVertexId), BRepGraph_NodeId(aVertexId));
  EXPECT_GE(myGraph.Topo().Vertices().Edges(aVertexId).Size(), 1);
}

TEST_F(BRepGraph_ViewsTest, TopoView_GroupedCoEdgeOps_Parity)
{
  const BRepGraph_CoEdgeId       aCoEdgeId(0);
  const BRepGraphInc::CoEdgeDef& aCoEdge = myGraph.Topo().CoEdges().Definition(aCoEdgeId);

  EXPECT_EQ(BRepGraph_NodeId(aCoEdgeId), BRepGraph_NodeId(aCoEdgeId));
  EXPECT_EQ(myGraph.Topo().CoEdges().Definition(aCoEdgeId).ChildEdgeId, aCoEdge.ChildEdgeId);
  EXPECT_EQ(myGraph.Topo().CoEdges().Definition(aCoEdgeId).FaceId, aCoEdge.FaceId);
  EXPECT_FALSE(myGraph.Topo().CoEdges().Curve2D(aCoEdgeId).IsNull());
  // Verify TopoView::CoEdgeOps::SeamPair and Tool::CoEdge::SeamPair agree.
  EXPECT_EQ(BRepGraph_Tool::CoEdge::SeamPair(myGraph, aCoEdgeId),
            BRepGraph_Tool::CoEdge::SeamPair(myGraph, aCoEdgeId));
}

TEST_F(BRepGraph_ViewsTest, TopoView_GroupedProductAndOccurrenceOps_Parity)
{
  const BRepGraph_ProductId aPartProduct =
    myGraph.Editor().Products().Add(BRepGraph_NodeId(BRepGraph_SolidId::Start()));
  myGraph.Editor().Products().AppendDocumentRoot(aPartProduct);
  const BRepGraph_ProductId aSubAssembly = myGraph.Editor().Products().Add();
  myGraph.Editor().Products().AppendDocumentRoot(aSubAssembly);
  const BRepGraph_ProductId aRootAssembly = myGraph.Editor().Products().Add();
  myGraph.Editor().Products().AppendDocumentRoot(aRootAssembly);
  ASSERT_TRUE(aPartProduct.IsValid());
  ASSERT_TRUE(aSubAssembly.IsValid());
  ASSERT_TRUE(aRootAssembly.IsValid());

  const BRepGraph_OccurrenceId aSubOccurrence =
    myGraph.Editor().Products().Append(aRootAssembly, aSubAssembly, TopLoc_Location());
  const BRepGraph_OccurrenceId aPartOccurrence =
    myGraph.Editor().Products().Append(aSubAssembly,
                                       aPartProduct,
                                       TopLoc_Location(),
                                       aSubOccurrence);
  ASSERT_TRUE(aSubOccurrence.IsValid());
  ASSERT_TRUE(aPartOccurrence.IsValid());

  EXPECT_EQ(BRepGraph_NodeId(aPartProduct), BRepGraph_NodeId(aPartProduct));
  EXPECT_EQ(myGraph.Topo().Products().ShapeRoot(aPartProduct),
            BRepGraph_NodeId(BRepGraph_SolidId::Start()));
  EXPECT_FALSE(myGraph.Topo().Products().ShapeRoot(aRootAssembly).IsValid());
  EXPECT_EQ(myGraph.Refs().Occurrences().IdsOf(aRootAssembly).Size(), 1);
  EXPECT_EQ(myGraph.Refs().Occurrences().IdsOf(aSubAssembly).Size(), 1);

  EXPECT_EQ(BRepGraph_NodeId(aPartOccurrence), BRepGraph_NodeId(aPartOccurrence));
  EXPECT_EQ(myGraph.Topo().Occurrences().Product(aPartOccurrence), aPartProduct);
  EXPECT_EQ(myGraph.Topo().Occurrences().ParentProduct(aPartOccurrence), aSubAssembly);

  const NCollection_LinearVector<BRepGraph_OccurrenceRefId>& aOccurrenceRefs =
    myGraph.Refs().Occurrences().IdsOf(aSubAssembly);
  ASSERT_EQ(aOccurrenceRefs.Size(), 1);
  myGraph.Editor().Gen().RemoveRef(aOccurrenceRefs.Value(0));

  EXPECT_EQ(myGraph.Topo().Products().NbComponents(aSubAssembly), 0);
}

TEST_F(BRepGraph_ViewsTest, SpatialView_OutParam_ClearAndInvalid)
{
  EXPECT_EQ(countRelated(myGraph,
                         BRepGraph_NodeId(BRepGraph_FaceId::Start()),
                         BRepGraph_RelatedIterator::RelationKind::AdjacentFace),
            4);
  EXPECT_EQ(countRelated(myGraph,
                         BRepGraph_NodeId(BRepGraph_FaceId(999)),
                         BRepGraph_RelatedIterator::RelationKind::AdjacentFace),
            0);

  EXPECT_GE(countAdjacentEdgesOfEdge(myGraph, BRepGraph_EdgeId::Start()), 4);
  EXPECT_EQ(countAdjacentEdgesOfEdge(myGraph, BRepGraph_EdgeId(999)), 0);
}

// ---------- CacheRegistry ----------

TEST_F(BRepGraph_ViewsTest, CacheRegistry_EnsureRegistersAttachedService)
{
  occ::handle<TestCacheService> aCache = myGraph.CacheRegistry().Ensure<TestCacheService>();
  ASSERT_FALSE(aCache.IsNull());
  EXPECT_TRUE(aCache->Attached());
  EXPECT_EQ(myGraph.CacheRegistry().Find<TestCacheService>(), aCache);
}

TEST_F(BRepGraph_ViewsTest, CacheRegistry_CacheIter_RangeFor)
{
  [[maybe_unused]] occ::handle<TestCacheService> aRegistered =
    myGraph.CacheRegistry().Ensure<TestCacheService>();

  uint32_t aCount  = 0;
  bool     hasTest = false;
  for (const occ::handle<BRepGraph_Cache> aCache : myGraph.CacheRegistry().CacheIter())
  {
    if (!aCache.IsNull() && aCache->ID() == TestCacheService::GetID())
    {
      hasTest = true;
    }
    ++aCount;
  }
  EXPECT_EQ(aCount, 1u);
  EXPECT_TRUE(hasTest);
}

TEST_F(BRepGraph_ViewsTest, CacheRegistry_ClearAllClearsRepresentationButKeepsService)
{
  occ::handle<TestCacheService> aCache = myGraph.CacheRegistry().Ensure<TestCacheService>();
  aCache->Set(BRepGraph_FaceId(0), 42);
  int aValue = 0;
  ASSERT_TRUE(aCache->Get(BRepGraph_FaceId(0), aValue));

  myGraph.CacheRegistry().ClearAll();

  EXPECT_FALSE(aCache->Get(BRepGraph_FaceId(0), aValue));
  EXPECT_EQ(aCache->ClearCount(), 1);
  EXPECT_EQ(myGraph.CacheRegistry().Find<TestCacheService>(), aCache);
}

TEST_F(BRepGraph_ViewsTest, CacheRegistry_GraphClearClearsRepresentationButKeepsService)
{
  occ::handle<TestCacheService> aCache = myGraph.CacheRegistry().Ensure<TestCacheService>();
  aCache->Set(BRepGraph_FaceId(0), 42);

  myGraph.Clear();

  int aValue = 0;
  EXPECT_FALSE(aCache->Get(BRepGraph_FaceId(0), aValue));
  EXPECT_EQ(myGraph.CacheRegistry().Find<TestCacheService>(), aCache);
}

TEST_F(BRepGraph_ViewsTest, CacheRegistry_UnregisterDetachesService)
{
  occ::handle<TestCacheService> aCache = myGraph.CacheRegistry().Ensure<TestCacheService>();
  ASSERT_TRUE(aCache->Attached());

  myGraph.CacheRegistry().UnregisterCache(TestCacheService::GetID());

  EXPECT_FALSE(aCache->Attached());
  EXPECT_TRUE(myGraph.CacheRegistry().Find<TestCacheService>().IsNull());
}

TEST_F(BRepGraph_ViewsTest, BRepGraphTool_FindCoEdgeId_ValidPair)
{
  // A box has edges shared by faces. Pick an edge and one of its faces.
  const BRepGraph_EdgeId       anEdge(0);
  const BRepGraphInc::EdgeDef& anEdgeDef = myGraph.Topo().Edges().Definition(anEdge);
  std::ignore                            = anEdgeDef;

  // Use relation tables to get a face for this edge.
  const BRepGraph_FaceId aFace = firstFaceOfEdge(myGraph, anEdge);
  ASSERT_TRUE(aFace.IsValid());

  const BRepGraph_CoEdgeId aCoEdgeId = BRepGraph_Tool::Edge::FindCoEdgeId(myGraph, anEdge, aFace);
  ASSERT_TRUE(aCoEdgeId.IsValid());

  // The returned CoEdge must reference the same edge and face.
  const BRepGraphInc::CoEdgeDef& aCoEdge = myGraph.Topo().CoEdges().Definition(aCoEdgeId);
  EXPECT_EQ(aCoEdge.ChildEdgeId, anEdge);
  EXPECT_EQ(aCoEdge.FaceId, aFace);
}

TEST_F(BRepGraph_ViewsTest, BRepGraphTool_FindCoEdgeId_InvalidPair_ReturnsInvalid)
{
  // Use a valid edge but a face that doesn't share it.
  // Edge 0 and the last face are very unlikely to share a coedge in a box.
  const BRepGraph_EdgeId anEdge(0);

  // Find a face NOT adjacent to edge 0.
  BRepGraph_FaceId aNonAdjacentFace;
  for (BRepGraph_FaceId aFaceId = BRepGraph_FaceId::Start();
       aFaceId.IsValid(myGraph.Topo().Faces().Nb());
       ++aFaceId)
  {
    if (!edgeHasFace(myGraph, anEdge, aFaceId))
    {
      aNonAdjacentFace = aFaceId;
      break;
    }
  }
  if (aNonAdjacentFace.IsValid())
  {
    const BRepGraph_CoEdgeId aCoEdgeId =
      BRepGraph_Tool::Edge::FindCoEdgeId(myGraph, anEdge, aNonAdjacentFace);
    EXPECT_FALSE(aCoEdgeId.IsValid());
  }
}

TEST_F(BRepGraph_ViewsTest, RefsView_ActiveCounts_MatchFreshBuild)
{
  EXPECT_EQ(myGraph.Refs().Shells().NbActive(), myGraph.Refs().Shells().Nb());
  EXPECT_EQ(myGraph.Refs().Faces().NbActive(), myGraph.Refs().Faces().Nb());
  EXPECT_EQ(myGraph.Refs().Wires().NbActive(), myGraph.Refs().Wires().Nb());
  EXPECT_EQ(myGraph.Topo().CoEdges().NbActive(), myGraph.Topo().CoEdges().Nb());
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

  EXPECT_EQ(countActiveRefs(myGraph, myGraph.Refs().Faces().IdsOf(aShellId)),
            myGraph.Refs().Faces().IdsOf(aShellId).Size());
  EXPECT_EQ(countActiveRefs(myGraph, myGraph.Refs().Wires().IdsOf(aFaceId)),
            myGraph.Refs().Wires().IdsOf(aFaceId).Size());
  EXPECT_EQ(countActiveNodeIds(myGraph, myGraph.Topo().Wires().Relations(aWireId).CoEdgeIds),
            myGraph.Topo().Wires().Relations(aWireId).CoEdgeIds.Size());
  EXPECT_EQ(countActiveRefs(myGraph, myGraph.Refs().Shells().IdsOf(aSolidId)),
            myGraph.Refs().Shells().IdsOf(aSolidId).Size());
}

TEST_F(BRepGraph_ViewsTest, RefsView_FaceRefIdsOf_LocalFilteringHandlesRemoved)
{
  const BRepGraph_ShellId                              aShellId(0);
  const NCollection_LinearVector<BRepGraph_FaceRefId>& aFaceRefs =
    myGraph.Refs().Faces().IdsOf(aShellId);
  ASSERT_GT(aFaceRefs.Size(), 0);
  const size_t aNbFaceRefsBefore = aFaceRefs.Size();

  myGraph.Editor().Gen().RemoveRef(aFaceRefs.Value(0));

  EXPECT_EQ(countActiveRefs(myGraph, myGraph.Refs().Faces().IdsOf(aShellId)),
            aNbFaceRefsBefore - 1);
}

TEST_F(BRepGraph_ViewsTest, RefsView_VertexRefIdsOfEdge_ContainsBoundaryVertices)
{
  uint32_t aNbVertexRefs = 0;
  for (BRepGraph_RefsVertexOfEdge aRefIt(myGraph, BRepGraph_EdgeId::Start()); aRefIt.More();
       aRefIt.Next())
  {
    const BRepGraph_VertexRefId    aVertexRefId = aRefIt.CurrentId();
    const BRepGraphInc::VertexRef& aRef         = myGraph.Refs().Vertices().Entry(aVertexRefId);
    EXPECT_FALSE(aVertexRefId.IsRemoved(myGraph));
    EXPECT_TRUE(aRef.ChildVertexId.IsValid(myGraph.Topo().Vertices().Nb()));
    ++aNbVertexRefs;
  }
  EXPECT_GE(aNbVertexRefs, 2);
}

TEST_F(BRepGraph_ViewsTest, RefsView_GenericRefHelpers_RoundTripForTypedRef)
{
  const NCollection_LinearVector<BRepGraph_FaceRefId>& aFaceRefs =
    myGraph.Refs().Faces().IdsOf(BRepGraph_ShellId::Start());
  ASSERT_GT(aFaceRefs.Size(), 0);

  const BRepGraph_FaceRefId aFaceRefId = aFaceRefs.Value(0);
  {
    BRepGraph_MutGuard<BRepGraphInc::FaceRef> aFaceRef =
      myGraph.Editor().Faces().MutRef(aFaceRefId);
    myGraph.Editor().Faces().SetRefOrientation(aFaceRef, TopAbs_REVERSED);
  }

  const BRepGraphInc::FaceRef& aFaceRefEntry = myGraph.Refs().Faces().Entry(aFaceRefId);
  EXPECT_EQ(myGraph.Refs().Gen().ChildNode(aFaceRefId),
            BRepGraph_NodeId(aFaceRefEntry.ChildFaceId));
  EXPECT_TRUE(myGraph.Refs().Gen().LocalLocation(aFaceRefId).IsIdentity());
  EXPECT_EQ(myGraph.Refs().Gen().Orientation(aFaceRefId), aFaceRefEntry.Orientation);
  EXPECT_EQ(myGraph.Refs().Gen().Nb(BRepGraph_RefId::Kind::Face), myGraph.Refs().Faces().Nb());
  EXPECT_TRUE(myGraph.Refs().Gen().IsValid(aFaceRefId));
  EXPECT_TRUE(myGraph.Refs().Gen().IsActive(aFaceRefId));
  EXPECT_FALSE(myGraph.Refs().Gen().IsRemoved(aFaceRefId));

  const BRepGraph_CoEdgeId aCoEdgeId =
    myGraph.Topo().Wires().Relations(BRepGraph_WireId::Start()).CoEdgeIds.Value(0);
  const BRepGraphInc::CoEdgeDef& aCoEdgeEntry = myGraph.Topo().CoEdges().Definition(aCoEdgeId);
  EXPECT_TRUE(aCoEdgeEntry.ChildEdgeId.IsValid());
  EXPECT_TRUE(aCoEdgeEntry.FaceId.IsValid());
  EXPECT_NE(aCoEdgeEntry.Orientation, TopAbs_INTERNAL);
  EXPECT_NE(aCoEdgeEntry.Orientation, TopAbs_EXTERNAL);
}

TEST_F(BRepGraph_ViewsTest, RefsView_RefAtStep_RoundTrip)
{
  const BRepGraph_SolidId    aSolidId(0);
  const BRepGraph_ShellRefId aShellRefId =
    myGraph.Topo().Solids().Relations(aSolidId).ShellRefIds.Value(0);
  EXPECT_EQ(myGraph.Refs().Gen().RefAtStep(BRepGraph_NodeId(aSolidId), 0),
            BRepGraph_RefId(aShellRefId));

  const BRepGraph_WireId aWireId(0);
  EXPECT_FALSE(myGraph.Refs().Gen().RefAtStep(BRepGraph_NodeId(aWireId), 0).IsValid());

  EXPECT_FALSE(myGraph.Refs().Gen().RefAtStep(BRepGraph_NodeId(aWireId), 100).IsValid());
}

TEST_F(BRepGraph_ViewsTest, RefsView_GenericRefHelpers_OccurrenceLocalLocation)
{
  const BRepGraph_ProductId aPartProduct =
    myGraph.Editor().Products().Add(BRepGraph_NodeId(BRepGraph_SolidId::Start()));
  myGraph.Editor().Products().AppendDocumentRoot(aPartProduct);
  const BRepGraph_ProductId anAssembly = myGraph.Editor().Products().Add();
  myGraph.Editor().Products().AppendDocumentRoot(anAssembly);
  ASSERT_TRUE(aPartProduct.IsValid());
  ASSERT_TRUE(anAssembly.IsValid());

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(4.0, 5.0, 6.0));
  ASSERT_TRUE(
    myGraph.Editor().Products().Append(anAssembly, aPartProduct, TopLoc_Location(aTrsf)).IsValid());

  const NCollection_LinearVector<BRepGraph_OccurrenceRefId>& anOccurrenceRefs =
    myGraph.Refs().Occurrences().IdsOf(anAssembly);
  ASSERT_EQ(anOccurrenceRefs.Size(), 1);

  const BRepGraph_OccurrenceRefId    aRefId    = anOccurrenceRefs.Value(0);
  const BRepGraphInc::OccurrenceRef& aRefEntry = myGraph.Refs().Occurrences().Entry(aRefId);
  EXPECT_EQ(myGraph.Refs().Gen().RefAtStep(BRepGraph_NodeId(anAssembly), 0),
            BRepGraph_RefId(aRefId));
  EXPECT_EQ(myGraph.Refs().Gen().ChildNode(aRefId), BRepGraph_NodeId(aRefEntry.ChildOccurrenceId));
  EXPECT_TRUE(myGraph.Refs().Gen().LocalLocation(aRefId).IsEqual(aRefEntry.LocalLocation));
  EXPECT_EQ(myGraph.Refs().Gen().Orientation(aRefId), TopAbs_FORWARD);
  EXPECT_FALSE(myGraph.Refs().Gen().IsRemoved(aRefId));
}

TEST_F(BRepGraph_ViewsTest, RefsView_GenericRefHelpers_InvalidAndRemoved)
{
  EXPECT_FALSE(myGraph.Refs().Gen().ChildNode(BRepGraph_RefId()).IsValid());
  EXPECT_TRUE(myGraph.Refs().Gen().LocalLocation(BRepGraph_RefId()).IsEqual(TopLoc_Location()));
  EXPECT_EQ(myGraph.Refs().Gen().Orientation(BRepGraph_RefId()), TopAbs_FORWARD);
  EXPECT_FALSE(myGraph.Refs().Gen().IsValid(BRepGraph_RefId()));
  EXPECT_FALSE(myGraph.Refs().Gen().IsActive(BRepGraph_RefId()));
  EXPECT_TRUE(myGraph.Refs().Gen().IsRemoved(BRepGraph_RefId()));

  const NCollection_LinearVector<BRepGraph_FaceRefId>& aFaceRefs =
    myGraph.Refs().Faces().IdsOf(BRepGraph_ShellId::Start());
  ASSERT_GT(aFaceRefs.Size(), 0);
  const BRepGraph_FaceRefId aFaceRefId = aFaceRefs.Value(0);

  myGraph.Editor().Gen().RemoveRef(aFaceRefId);

  EXPECT_TRUE(myGraph.Refs().Gen().IsValid(aFaceRefId));
  EXPECT_FALSE(myGraph.Refs().Gen().IsActive(aFaceRefId));
  EXPECT_TRUE(myGraph.Refs().Gen().IsRemoved(aFaceRefId));

  const BRepGraph_FaceRefId anOutOfRangeFaceRef(myGraph.Refs().Faces().Nb());
  EXPECT_FALSE(myGraph.Refs().Gen().IsValid(anOutOfRangeFaceRef));
  EXPECT_FALSE(myGraph.Refs().Gen().IsActive(anOutOfRangeFaceRef));
  EXPECT_TRUE(myGraph.Refs().Gen().IsRemoved(anOutOfRangeFaceRef));
  EXPECT_FALSE(myGraph.Refs().Gen().ChildNode(anOutOfRangeFaceRef).IsValid());
  EXPECT_TRUE(myGraph.Refs().Gen().LocalLocation(anOutOfRangeFaceRef).IsEqual(TopLoc_Location()));
  EXPECT_EQ(myGraph.Refs().Gen().Orientation(anOutOfRangeFaceRef), TopAbs_FORWARD);
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

TEST_F(BRepGraph_ViewsTest, ShapesView_Original_ValidNode_ReturnsShape)
{
  const BRepGraph_FaceId aFaceId(0);
  const TopoDS_Shape     aFound = myGraph.Shapes().Original(aFaceId);
  EXPECT_FALSE(aFound.IsNull());
}

TEST_F(BRepGraph_ViewsTest, ShapesView_Original_InvalidNode_ReturnsNull)
{
  EXPECT_TRUE(myGraph.Shapes().Original(BRepGraph_NodeId()).IsNull());
}

TEST_F(BRepGraph_ViewsTest, ShapesView_RemovedNode_OriginalQueries_AreUnavailable)
{
  const BRepGraph_FaceId aFaceId(0);
  ASSERT_TRUE(myGraph.Shapes().HasOriginal(aFaceId));

  myGraph.Editor().Gen().RemoveNode(aFaceId);

  EXPECT_FALSE(myGraph.Shapes().HasOriginal(aFaceId));
  EXPECT_TRUE(myGraph.Shapes().Original(aFaceId).IsNull());
  EXPECT_TRUE(myGraph.Shapes().Shape(aFaceId).IsNull());
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
  const TopoDS_Shape     aFaceShape = myGraph.Shapes().Original(aFaceId);
  ASSERT_FALSE(aFaceShape.IsNull());

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
    anEdge.MarkDirty();
  }
  EXPECT_GT(myGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).OwnGen, 0u);
}

TEST_F(BRepGraph_ViewsTest, MutView_InvalidNode_ThrowsProgramError)
{
#ifndef No_Exception
  EXPECT_THROW(std::ignore = myGraph.Editor().Faces().Mut(BRepGraph_FaceId(777777)),
               Standard_ProgramError);
#endif
}

TEST_F(BRepGraph_ViewsTest, MutView_RemovedNode_ThrowsProgramError)
{
#ifndef No_Exception
  const BRepGraph_FaceId aFaceId(0);
  myGraph.Editor().Gen().RemoveNode(aFaceId);
  EXPECT_THROW(std::ignore = myGraph.Editor().Faces().Mut(aFaceId), Standard_ProgramError);
#endif
}

TEST_F(BRepGraph_ViewsTest, MutView_RemovedRef_ThrowsProgramError)
{
  const NCollection_LinearVector<BRepGraph_FaceRefId>& aFaceRefs =
    myGraph.Refs().Faces().IdsOf(BRepGraph_ShellId::Start());
  ASSERT_GT(aFaceRefs.Size(), 0);
  const BRepGraph_FaceRefId aFaceRefId = aFaceRefs.Value(0);

  ASSERT_TRUE(myGraph.Editor().Gen().RemoveRef(aFaceRefId));
#ifndef No_Exception
  EXPECT_THROW(std::ignore = myGraph.Editor().Faces().MutRef(aFaceRefId), Standard_ProgramError);
#endif
}

// ---------- EditorView ----------

TEST_F(BRepGraph_ViewsTest, EditorView_AddVertex_Works)
{
  const uint32_t     aNbBefore = myGraph.Topo().Vertices().Nb();
  BRepGraph_VertexId aVtx      = myGraph.Editor().Vertices().Add(gp_Pnt(1, 2, 3), 0.001);
  EXPECT_TRUE(aVtx.IsValid());
  EXPECT_EQ(myGraph.Topo().Vertices().Nb(), aNbBefore + 1);
}

TEST_F(BRepGraph_ViewsTest, EditorView_IsRemoved_False)
{
  BRepGraph_FaceId aFaceId(0);
  EXPECT_FALSE(myGraph.Topo().Gen().IsRemoved(aFaceId));
}

// ---------- History layer lookup ----------

TEST_F(BRepGraph_ViewsTest, History_ConstLookup)
{
  const BRepGraph&              aConstGraph = myGraph;
  const BRepGraph_LayerHistory* aHistory =
    aConstGraph.LayerRegistry().Find<BRepGraph_LayerHistory>().get();
  ASSERT_NE(aHistory, nullptr);
  EXPECT_TRUE(aHistory->IsEnabled());
}

TEST_F(BRepGraph_ViewsTest, History_MutableLookup)
{
  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->SetEnabled(false);
  EXPECT_FALSE(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->IsEnabled());
  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->SetEnabled(true);
  EXPECT_TRUE(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->IsEnabled());
}

// ---------- BRepGraph_Tool bug-fix regression tests ----------

TEST_F(BRepGraph_ViewsTest, BRepGraphTool_FindPCurveCoEdgeId_SkipsRemovedCoEdges)
{
  const BRepGraph_FaceId aFaceId = BRepGraph_FaceId::Start();
  const BRepGraph_WireId aWireId = BRepGraph_Tool::Face::OuterWire(myGraph, aFaceId);
  ASSERT_TRUE(aWireId.IsValid());

  const BRepGraphInc::WireRelations& aWireRelations = myGraph.Topo().Wires().Relations(aWireId);
  ASSERT_GT(aWireRelations.CoEdgeIds.Size(), 0);
  const BRepGraph_CoEdgeId aCoEdgeId = aWireRelations.CoEdgeIds.Value(0);
  ASSERT_TRUE(aCoEdgeId.IsValid());

  const BRepGraphInc::CoEdgeDef& aCoEdgeDef = myGraph.Topo().CoEdges().Definition(aCoEdgeId);
  const BRepGraph_EdgeId         anEdgeId   = aCoEdgeDef.ChildEdgeId;
  const BRepGraph_FaceId         aFaceId2   = aCoEdgeDef.FaceId;

  const BRepGraph_CoEdgeId aFoundBefore =
    BRepGraph_Tool::Edge::FindCoEdgeId(myGraph, anEdgeId, aFaceId2);
  ASSERT_EQ(aFoundBefore, aCoEdgeId);

  myGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(aCoEdgeId));
  ASSERT_TRUE(myGraph.Topo().Gen().IsRemoved(aCoEdgeId));

  const BRepGraph_CoEdgeId aPCurveId =
    BRepGraph_Tool::Edge::FindPCurveCoEdgeId(myGraph, anEdgeId, aFaceId2);
  EXPECT_FALSE(aPCurveId.IsValid());

  const BRepGraph_CoEdgeId aFoundAfter =
    BRepGraph_Tool::Edge::FindCoEdgeId(myGraph, anEdgeId, aFaceId2);
  EXPECT_NE(aFoundAfter, aCoEdgeId);
}

TEST_F(BRepGraph_ViewsTest, BRepGraphTool_OuterWire_SkipsRemovedWireDef)
{
  const BRepGraph_FaceId aFaceId = BRepGraph_FaceId::Start();
  const BRepGraph_WireId aWireId = BRepGraph_Tool::Face::OuterWire(myGraph, aFaceId);
  ASSERT_TRUE(aWireId.IsValid());

  myGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(aWireId));
  ASSERT_TRUE(myGraph.Topo().Gen().IsRemoved(aWireId));

  const BRepGraph_WireId anOuterAfter = BRepGraph_Tool::Face::OuterWire(myGraph, aFaceId);
  EXPECT_NE(anOuterAfter, aWireId);
}

TEST_F(BRepGraph_ViewsTest, BRepGraphTool_OuterWire_SkipsWireWithoutUVBounds)
{
  const BRepGraph_FaceId aFaceId    = BRepGraph_FaceId::Start();
  const BRepGraph_WireId aValidWire = BRepGraph_Tool::Face::OuterWire(myGraph, aFaceId);
  ASSERT_TRUE(aValidWire.IsValid());

  NCollection_LinearVector<BRepGraph_CoEdgeId> anEmptyCoEdges;
  const BRepGraph_WireId anEmptyWire = myGraph.Editor().Wires().Add(anEmptyCoEdges.ToArray1());
  ASSERT_TRUE(anEmptyWire.IsValid());

  const BRepGraph_WireRefId anEmptyWireRef =
    myGraph.Editor().Faces().Append(aFaceId, anEmptyWire, TopAbs_FORWARD);
  ASSERT_TRUE(anEmptyWireRef.IsValid());

  EXPECT_EQ(BRepGraph_Tool::Face::OuterWire(myGraph, aFaceId), aValidWire);
}

TEST_F(BRepGraph_ViewsTest, BRepGraphTool_IsBoundary_SkipsRemovedFaces)
{
  BRepGraph_EdgeId anEdgeWithTwoFaces;
  for (BRepGraph_EdgeId anEdgeId = BRepGraph_EdgeId::Start();
       anEdgeId.IsValid(myGraph.Topo().Edges().Nb());
       ++anEdgeId)
  {
    if (countIterator(myGraph.Topo().Edges().FacesOf(anEdgeId)) == 2)
    {
      anEdgeWithTwoFaces = anEdgeId;
      break;
    }
  }
  ASSERT_TRUE(anEdgeWithTwoFaces.IsValid());
  ASSERT_EQ(countIterator(myGraph.Topo().Edges().FacesOf(anEdgeWithTwoFaces)), 2);
  ASSERT_FALSE(BRepGraph_Tool::Edge::IsBoundary(myGraph, anEdgeWithTwoFaces));
  ASSERT_TRUE(BRepGraph_Tool::Edge::IsManifold(myGraph, anEdgeWithTwoFaces));

  const BRepGraph_FaceId aFaceToRemove = firstFaceOfEdge(myGraph, anEdgeWithTwoFaces);
  ASSERT_TRUE(aFaceToRemove.IsValid());

  myGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(aFaceToRemove));
  ASSERT_TRUE(myGraph.Topo().Gen().IsRemoved(aFaceToRemove));

  EXPECT_TRUE(BRepGraph_Tool::Edge::IsBoundary(myGraph, anEdgeWithTwoFaces));
  EXPECT_FALSE(BRepGraph_Tool::Edge::IsManifold(myGraph, anEdgeWithTwoFaces));
}

TEST_F(BRepGraph_ViewsTest, BRepGraphTool_IsBoundaryAndIsManifold_RemovedEdge_ReturnsFalse)
{
  BRepGraph_EdgeId anEdgeId = BRepGraph_EdgeId::Start();
  ASSERT_TRUE(anEdgeId.IsValid(myGraph.Topo().Edges().Nb()));
  ASSERT_FALSE(myGraph.Topo().Gen().IsRemoved(anEdgeId));

  myGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(anEdgeId));
  ASSERT_TRUE(myGraph.Topo().Gen().IsRemoved(anEdgeId));

  EXPECT_FALSE(BRepGraph_Tool::Edge::IsBoundary(myGraph, anEdgeId));
  EXPECT_FALSE(BRepGraph_Tool::Edge::IsManifold(myGraph, anEdgeId));
}

TEST_F(BRepGraph_ViewsTest, BRepGraphTool_SeamPair_BoundsCheck)
{
  myGraph.Clear();
  BRepPrimAPI_MakeSphere      aSphereMaker(gp_Pnt(0, 0, 0), 10.0);
  [[maybe_unused]] const auto aRes = myGraph.Shapes().Add(aSphereMaker.Shape());
  ASSERT_FALSE(myGraph.IsEmpty());

  bool aFoundSeamPair = false;
  for (BRepGraph_CoEdgeId aCoEdgeId = BRepGraph_CoEdgeId::Start();
       aCoEdgeId.IsValid(myGraph.Topo().CoEdges().Nb());
       ++aCoEdgeId)
  {
    const BRepGraph_CoEdgeId aSeamPair = BRepGraph_Tool::CoEdge::SeamPair(myGraph, aCoEdgeId);
    if (aSeamPair.IsValid())
    {
      aFoundSeamPair                 = true;
      const BRepGraph_CoEdgeId aBack = BRepGraph_Tool::CoEdge::SeamPair(myGraph, aSeamPair);
      EXPECT_EQ(aBack, aCoEdgeId);
      break;
    }
  }
  EXPECT_TRUE(aFoundSeamPair);
}

// ---------- EditorView bug-fix regression tests ----------

TEST_F(BRepGraph_ViewsTest, EditorView_ReplaceEdge_SkipsRemovedCoEdgeDef)
{
  BRepPrimAPI_MakeBox aBoxMaker1(10.0, 20.0, 30.0);
  BRepPrimAPI_MakeBox aBoxMaker2(10.0, 20.0, 30.0);

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBoxMaker1.Shape());
  aBuilder.Add(aCompound, aBoxMaker2.Shape());

  myGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes = myGraph.Shapes().Add(aCompound);
  ASSERT_FALSE(myGraph.IsEmpty());

  BRepGraph_Deduplicate::Options anOpts;
  anOpts.MergeEntitiesWhenSafe = true;
  [[maybe_unused]] const BRepGraph_Deduplicate::Result aDedupRes =
    BRepGraph_Deduplicate::Perform(myGraph, anOpts);

  std::ignore = BRepGraph_Compact::Perform(myGraph);

  const BRepGraph_Validate::Result aValResult = BRepGraph_Validate::Perform(myGraph);
  EXPECT_TRUE(aValResult.IsValid());
}

TEST_F(BRepGraph_ViewsTest, EditorView_RemoveSolid_PropagatesSubtreeGenToParent)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  aBB.Add(aCompound, aBox);

  myGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes = myGraph.Shapes().Add(aCompound);
  ASSERT_FALSE(myGraph.IsEmpty());
  ASSERT_EQ(myGraph.Topo().Compounds().Nb(), 1);
  ASSERT_EQ(myGraph.Topo().Solids().Nb(), 1);

  const BRepGraph_SolidId aSolidId = BRepGraph_SolidId::Start();
  myGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(aSolidId));
  ASSERT_TRUE(myGraph.Topo().Gen().IsRemoved(aSolidId));

  std::ignore = BRepGraph_Compact::Perform(myGraph);

  const BRepGraph_Validate::Result aValResult = BRepGraph_Validate::Perform(myGraph);
  EXPECT_TRUE(aValResult.IsValid());
}

TEST_F(BRepGraph_ViewsTest, EditorView_RemoveOccurrence_UnbindsProductOccurrence)
{
  BRepGraph aGraph;

  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  BRepBuilderAPI_Copy aCopy1(aBox, true);
  BRepBuilderAPI_Copy aCopy2(aBox, true);

  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  aBB.Add(aCompound, aCopy1.Shape());
  aBB.Add(aCompound, aCopy2.Shape());

  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes = aGraph.Shapes().Add(aCompound);
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph_Deduplicate::Options anOpts;
  anOpts.MergeEntitiesWhenSafe = true;
  [[maybe_unused]] const BRepGraph_Deduplicate::Result aDedupRes =
    BRepGraph_Deduplicate::Perform(aGraph, anOpts);

  BRepGraph_Compact::Options                       aCompactOpts;
  [[maybe_unused]] const BRepGraph_Compact::Result aCompactRes =
    BRepGraph_Compact::Perform(aGraph, aCompactOpts);

  EXPECT_TRUE(aGraph.ValidateRelations());
}

TEST_F(BRepGraph_ViewsTest, EditorView_ReplaceEdge_RejectsRemovedWire)
{
  BRepPrimAPI_MakeBox aBoxMaker1(10.0, 20.0, 30.0);
  BRepPrimAPI_MakeBox aBoxMaker2(10.0, 20.0, 30.0);

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBoxMaker1.Shape());
  aBuilder.Add(aCompound, aBoxMaker2.Shape());

  myGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes = myGraph.Shapes().Add(aCompound);
  ASSERT_FALSE(myGraph.IsEmpty());

  BRepGraph_Deduplicate::Options anOpts;
  anOpts.MergeEntitiesWhenSafe = true;
  std::ignore                  = BRepGraph_Deduplicate::Perform(myGraph, anOpts);

  std::ignore = BRepGraph_Compact::Perform(myGraph);

  const BRepGraph_Validate::Result aValResult = BRepGraph_Validate::Perform(myGraph);
  EXPECT_TRUE(aValResult.IsValid());
}

// ---------- Bug-fix regression: Split with removed vertex ----------

TEST_F(BRepGraph_ViewsTest, EditorView_Split_RejectsRemovedVertex)
{
  BRepGraph_EdgeId            anEdgeId   = BRepGraph_EdgeId::Start();
  const BRepGraph_VertexRefId aStartVRef = BRepGraph_Tool::Edge::StartVertexId(myGraph, anEdgeId);
  const BRepGraph_Tool::VertexUsage aStartVUsage =
    BRepGraph_Tool::Vertex::Usage(myGraph, aStartVRef);
  ASSERT_TRUE(aStartVUsage.IsValid());
  const BRepGraph_VertexId aStartV = aStartVUsage.DefId;
  ASSERT_FALSE(myGraph.Topo().Gen().IsRemoved(aStartV));

  myGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(aStartV));
  ASSERT_TRUE(myGraph.Topo().Gen().IsRemoved(aStartV));

  BRepGraph_EdgeId aSubA, aSubB;
  myGraph.Editor().Edges().Split(anEdgeId, aStartV, 0.5, aSubA, aSubB);

  EXPECT_FALSE(aSubA.IsValid());
  EXPECT_FALSE(aSubB.IsValid());
}

// ---------- Bug-fix regression: CleanupRemovedReferences retires orphaned CoEdges ----------

TEST_F(BRepGraph_ViewsTest, CleanupRemovedRefs_OrphanedCoEdgesMarkedRemoved)
{
  const BRepGraph_EdgeId anEdgeId = BRepGraph_EdgeId::Start();
  ASSERT_TRUE(anEdgeId.IsValid(myGraph.Topo().Edges().Nb()));
  const uint32_t aNbCoEdgesBefore = myGraph.Topo().CoEdges().NbActive();

  myGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(anEdgeId));
  ASSERT_TRUE(myGraph.Topo().Gen().IsRemoved(anEdgeId));

  myGraph.Editor().Gen().CleanupRemovedReferences();

  const uint32_t aNbCoEdgesAfter = myGraph.Topo().CoEdges().NbActive();
  EXPECT_LT(aNbCoEdgesAfter, aNbCoEdgesBefore);
  EXPECT_TRUE(myGraph.ValidateRelations());
}

// ---------- Bug-fix regression: TopoView accessors return invalid for removed entities ----------

TEST_F(BRepGraph_ViewsTest, TopoView_RepAccessors_ReturnNullForRemovedEntity)
{
  // Edge::Curve3D on removed edge
  BRepGraph_EdgeId anEdgeId = BRepGraph_EdgeId::Start();
  ASSERT_FALSE(myGraph.Topo().Edges().Curve3D(anEdgeId).IsNull());

  myGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(anEdgeId));
  ASSERT_TRUE(myGraph.Topo().Gen().IsRemoved(anEdgeId));

  EXPECT_TRUE(myGraph.Topo().Edges().Curve3D(anEdgeId).IsNull());

  // Face::Surface on removed face
  const BRepGraph_FaceId aFaceId = BRepGraph_FaceId::Start();
  ASSERT_FALSE(myGraph.Topo().Faces().Surface(aFaceId).IsNull());

  myGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(aFaceId));
  ASSERT_TRUE(myGraph.Topo().Gen().IsRemoved(aFaceId));

  EXPECT_TRUE(myGraph.Topo().Faces().Surface(aFaceId).IsNull());
  EXPECT_TRUE(myGraph.Topo().Faces().ActiveTriangulation(aFaceId).IsNull());
}

// ---------- ShapesView supplement routing for INTERNAL/EXTERNAL ----------

TEST_F(BRepGraph_ViewsTest, ShapesView_ShellAddFace_Internal_RoutedToSupplement)
{
  BRepGraph                      aGraph;
  BRepGraph::ShapesView::Options anOptions;
  anOptions.CreateAutoProduct = false;

  BRep_Builder aBB;
  TopoDS_Shell aShell;
  aBB.MakeShell(aShell);

  TopoDS_Vertex aV0, aV1, aV2, aV3;
  aBB.MakeVertex(aV0, gp_Pnt(0.0, 0.0, 0.0), 1.0e-7);
  aBB.MakeVertex(aV1, gp_Pnt(10.0, 0.0, 0.0), 1.0e-7);
  aBB.MakeVertex(aV2, gp_Pnt(10.0, 10.0, 0.0), 1.0e-7);
  aBB.MakeVertex(aV3, gp_Pnt(0.0, 10.0, 0.0), 1.0e-7);

  auto makeEdge = [&](const TopoDS_Vertex& theV0, const TopoDS_Vertex& theV1) {
    TopoDS_Edge anEdge;
    aBB.MakeEdge(anEdge);
    aBB.Add(anEdge, theV0.Oriented(TopAbs_FORWARD));
    aBB.Add(anEdge, theV1.Oriented(TopAbs_REVERSED));
    return anEdge;
  };

  TopoDS_Wire aWire;
  aBB.MakeWire(aWire);
  aBB.Add(aWire, makeEdge(aV0, aV1));
  aBB.Add(aWire, makeEdge(aV1, aV2));
  aBB.Add(aWire, makeEdge(aV2, aV3));
  aBB.Add(aWire, makeEdge(aV3, aV0));
  aWire.Closed(true);

  TopoDS_Face aFace;
  aBB.MakeFace(aFace);
  aBB.Add(aFace, aWire);
  aBB.Add(aShell, aFace);

  const BRepGraph::ShapesView::Result aShellResult = aGraph.Shapes().Add(aShell, anOptions);
  ASSERT_TRUE(aShellResult.IsOk());
  const BRepGraph_ShellId aShellId(aShellResult.TopologyRoot);

  TopoDS_Face aInternalFace;
  aBB.MakeFace(aInternalFace);
  aBB.Add(aInternalFace, aWire);
  aInternalFace.Orientation(TopAbs_INTERNAL);

  const BRepGraph::ShapesView::Result aAddResult =
    aGraph.Shapes().Add(aInternalFace, BRepGraph_NodeId(aShellId));
  ASSERT_TRUE(aAddResult.IsOk());

  const occ::handle<BRepGraph_LayerTopoSupplement> aLayer =
    aGraph.LayerRegistry().FindLayer<BRepGraph_LayerTopoSupplement>();
  ASSERT_FALSE(aLayer.IsNull());
  EXPECT_GT(aLayer->AttachedTo(BRepGraph_NodeId(aShellId)).Size(), 0);
}

TEST_F(BRepGraph_ViewsTest, ShapesView_CompoundAddChild_MixedOrientations_CoreRefAndSupplement)
{
  BRepGraph                      aGraph;
  BRepGraph::ShapesView::Options anOptions;
  anOptions.CreateAutoProduct = false;

  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  const BRepGraph::ShapesView::Result aCompResult = aGraph.Shapes().Add(aCompound, anOptions);
  ASSERT_TRUE(aCompResult.IsOk());
  const BRepGraph_CompoundId aCompoundId(aCompResult.TopologyRoot);

  TopoDS_Vertex aV0, aV1, aV2, aV3;
  aBB.MakeVertex(aV0, gp_Pnt(0.0, 0.0, 0.0), 1.0e-7);
  aBB.MakeVertex(aV1, gp_Pnt(10.0, 0.0, 0.0), 1.0e-7);
  aBB.MakeVertex(aV2, gp_Pnt(10.0, 10.0, 0.0), 1.0e-7);
  aBB.MakeVertex(aV3, gp_Pnt(0.0, 10.0, 0.0), 1.0e-7);

  auto makeEdge = [&](const TopoDS_Vertex& theV0, const TopoDS_Vertex& theV1) {
    TopoDS_Edge anEdge;
    aBB.MakeEdge(anEdge);
    aBB.Add(anEdge, theV0.Oriented(TopAbs_FORWARD));
    aBB.Add(anEdge, theV1.Oriented(TopAbs_REVERSED));
    return anEdge;
  };

  TopoDS_Wire aWire;
  aBB.MakeWire(aWire);
  aBB.Add(aWire, makeEdge(aV0, aV1));
  aBB.Add(aWire, makeEdge(aV1, aV2));
  aBB.Add(aWire, makeEdge(aV2, aV3));
  aBB.Add(aWire, makeEdge(aV3, aV0));
  aWire.Closed(true);

  TopoDS_Face aForwardFace;
  aBB.MakeFace(aForwardFace);
  aBB.Add(aForwardFace, aWire);
  aForwardFace.Orientation(TopAbs_FORWARD);
  const BRepGraph::ShapesView::Result aFwdResult =
    aGraph.Shapes().Add(aForwardFace, BRepGraph_NodeId(aCompoundId));
  ASSERT_TRUE(aFwdResult.IsOk());
  EXPECT_TRUE(aFwdResult.InsertedRef.IsValid());

  TopoDS_Face aInternalFace;
  aBB.MakeFace(aInternalFace);
  aBB.Add(aInternalFace, aWire);
  aInternalFace.Orientation(TopAbs_INTERNAL);
  const BRepGraph::ShapesView::Result aIntResult =
    aGraph.Shapes().Add(aInternalFace, BRepGraph_NodeId(aCompoundId));
  ASSERT_TRUE(aIntResult.IsOk());
  EXPECT_FALSE(aIntResult.InsertedRef.IsValid());

  const occ::handle<BRepGraph_LayerTopoSupplement> aLayer =
    aGraph.LayerRegistry().FindLayer<BRepGraph_LayerTopoSupplement>();
  ASSERT_FALSE(aLayer.IsNull());
  EXPECT_EQ(aLayer->AttachedTo(BRepGraph_NodeId(aCompoundId)).Size(), 1);
}
