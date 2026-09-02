// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.

#include <gtest/gtest.h>

#include <BRep_Builder.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_Compact.hxx>
#include <BRepGraph_Copy.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_LayerSupplement.hxx>
#include <BRepGraph_LayerSupplementRegistry.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_SupplementsView.hxx>
#include <BRepGraphSupInc_TopologyStore.hxx>
#include <NCollection_LinearVector.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Vertex.hxx>
#include <gp_Pnt.hxx>

namespace
{
TopoDS_Vertex makeVertex(const gp_Pnt& thePoint)
{
  BRep_Builder aBuilder;
  TopoDS_Vertex aVertex;
  aBuilder.MakeVertex(aVertex, thePoint, 1.0e-7);
  return aVertex;
}

const BRepGraphSupInc_TopologyStore* topologyStore(const BRepGraph& theGraph)
{
  const occ::handle<BRepGraphSupInc_TopologyStore> aStore =
    occ::down_cast<BRepGraphSupInc_TopologyStore>(
      theGraph.Supplements().FindStore(BRepGraphSupInc_TopologyStore::GetID()));
  return aStore.get();
}

int vertexCount(const TopoDS_Shape& theShape)
{
  int aCount = 0;
  for (TopExp_Explorer anExplorer(theShape, TopAbs_VERTEX); anExplorer.More(); anExplorer.Next())
  {
    ++aCount;
  }
  return aCount;
}
} // namespace

TEST(BRepGraphSupInc_TopologyStoreTest, AttachPreservesOwnerOrderAndShape)
{
  BRepGraph aGraph;
  const BRepGraph_VertexId anOwner = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.0e-7);
  const BRepGraphSupInc_TopologyId aFirst = aGraph.Supplements().Attach(
    anOwner, BRepGraphSupInc::TopologyAttachmentKind::VertexSupplementShape, makeVertex(gp_Pnt(1, 0, 0)));
  const BRepGraphSupInc_TopologyId aSecond = aGraph.Supplements().Attach(
    anOwner, BRepGraphSupInc::TopologyAttachmentKind::VertexSupplementShape, makeVertex(gp_Pnt(2, 0, 0)));
  ASSERT_TRUE(aFirst.IsValid());
  ASSERT_TRUE(aSecond.IsValid());
  const NCollection_LinearVector<BRepGraphSupInc_TopologyId>& anItems =
    aGraph.Supplements().Attachments(anOwner);
  ASSERT_EQ(anItems.Size(), 2);
  EXPECT_EQ(anItems.First(), aFirst);
  EXPECT_EQ(anItems.Last(), aSecond);
  const BRepGraphSupInc::TopologyDef* anItem = aGraph.Supplements().Attachment(aFirst);
  ASSERT_NE(anItem, nullptr);
  EXPECT_EQ(anItem->Owner, BRepGraph_NodeId(anOwner));
  EXPECT_EQ(anItem->Kind, BRepGraphSupInc::TopologyAttachmentKind::VertexSupplementShape);
  EXPECT_FALSE(anItem->Shape.IsNull());
  EXPECT_EQ(aGraph.Supplements().AttachmentShape(*anItem), &anItem->Shape);
  EXPECT_TRUE(aGraph.Supplements().RemoveAttachment(aFirst));
  EXPECT_EQ(aGraph.Supplements().Attachments(anOwner).Size(), 1);
}

TEST(BRepGraphSupInc_TopologyStoreTest, RejectsIncompatibleAttachmentKind)
{
  BRepGraph aGraph;
  const BRepGraph_VertexId aVertex = aGraph.Editor().Vertices().Add(gp_Pnt(), 1.0e-7);
  EXPECT_FALSE(aGraph.Supplements().Attach(
    aVertex, BRepGraphSupInc::TopologyAttachmentKind::SolidAuxShape, makeVertex(gp_Pnt())).IsValid());
}

TEST(BRepGraphSupInc_TopologyStoreTest, NodeRemovalAndReplacementMaintainAttachments)
{
  BRepGraph aGraph;
  const BRepGraph_VertexId aFirst = aGraph.Editor().Vertices().Add(gp_Pnt(), 1.0e-7);
  const BRepGraph_VertexId aSecond = aGraph.Editor().Vertices().Add(gp_Pnt(1, 0, 0), 1.0e-7);
  ASSERT_TRUE(aGraph.Supplements().Attach(
    aFirst, BRepGraphSupInc::TopologyAttachmentKind::VertexSupplementShape, makeVertex(gp_Pnt())).IsValid());
  aGraph.Editor().Gen().ReplaceNode(aFirst, aSecond);
  EXPECT_EQ(aGraph.Supplements().Attachments(aFirst).Size(), 0);
  EXPECT_EQ(aGraph.Supplements().Attachments(aSecond).Size(), 1);
  aGraph.Editor().Gen().RemoveNode(aSecond);
  EXPECT_EQ(aGraph.Supplements().Attachments(aSecond).Size(), 0);
}

TEST(BRepGraphSupInc_TopologyStoreTest, SameOwnerReplacementPreservesAttachments)
{
  BRepGraph aGraph;
  const BRepGraph_VertexId anOwner = aGraph.Editor().Vertices().Add(gp_Pnt(), 1.0e-7);
  const BRepGraphSupInc_TopologyId anAttachment = aGraph.Supplements().Attach(
    anOwner, BRepGraphSupInc::TopologyAttachmentKind::VertexSupplementShape, makeVertex(gp_Pnt()));
  ASSERT_TRUE(anAttachment.IsValid());

  aGraph.Supplements().ReplaceOwnerAttachments(anOwner, anOwner);

  const NCollection_LinearVector<BRepGraphSupInc_TopologyId>& anAttachments =
    aGraph.Supplements().Attachments(anOwner);
  ASSERT_EQ(anAttachments.Size(), 1u);
  EXPECT_EQ(anAttachments.First(), anAttachment);
  const BRepGraphSupInc::TopologyDef* aDefinition = aGraph.Supplements().Attachment(anAttachment);
  ASSERT_NE(aDefinition, nullptr);
  EXPECT_EQ(aDefinition->Owner, BRepGraph_NodeId(anOwner));
}

TEST(BRepGraphSupInc_TopologyStoreTest, DirectShapeFollowsAttachmentLifecycle)
{
  BRepGraph aGraph;
  const BRepGraph_VertexId anOwner = aGraph.Editor().Vertices().Add(gp_Pnt(), 1.0e-7);
  const BRepGraphSupInc_TopologyId anAttachment = aGraph.Supplements().Attach(
    anOwner, BRepGraphSupInc::TopologyAttachmentKind::VertexSupplementShape, makeVertex(gp_Pnt()));
  ASSERT_TRUE(anAttachment.IsValid());
  const BRepGraphSupInc::TopologyDef* aDefinition = aGraph.Supplements().Attachment(anAttachment);
  ASSERT_NE(aDefinition, nullptr);
  EXPECT_FALSE(aDefinition->Shape.IsNull());
  EXPECT_EQ(aDefinition->Shape.ShapeType(), TopAbs_VERTEX);
  EXPECT_TRUE(aGraph.Supplements().RemoveAttachment(anAttachment));
  EXPECT_EQ(aGraph.Supplements().Attachment(anAttachment), nullptr);
  EXPECT_TRUE(aGraph.Supplements().Attachments(anOwner).IsEmpty());
}

TEST(BRepGraphSupInc_TopologyStoreTest, RemovalDispatchesTopologyEndpointNotification)
{
  BRepGraph aGraph;
  const BRepGraph_VertexId anOwner = aGraph.Editor().Vertices().Add(gp_Pnt(), 1.0e-7);
  const BRepGraphSupInc_TopologyId anAttachment = aGraph.Supplements().Attach(
    anOwner, BRepGraphSupInc::TopologyAttachmentKind::VertexSupplementShape, makeVertex(gp_Pnt()));
  ASSERT_TRUE(anAttachment.IsValid());
  const BRepGraphSupInc_TopologyStore* aTopology = topologyStore(aGraph);
  ASSERT_NE(aTopology, nullptr);
  const BRepGraphSupInc_Endpoint anEndpoint = BRepGraphSupInc_Endpoint::Supplemental(
    aTopology->ItemUID(anAttachment));
  const occ::handle<BRepGraph_LayerSupplement> aLayer =
    aGraph.LayerSupplementRegistry().Ensure<BRepGraph_LayerSupplement>();
  ASSERT_NE(aLayer->Add(BRepGraphSupInc_Endpoint::Core(BRepGraph_ItemId(anOwner)),
                        anEndpoint,
                        BRepGraph_LayerSupplement::GetID()),
            0u);
  EXPECT_TRUE(aGraph.Supplements().RemoveAttachment(anAttachment));
  EXPECT_TRUE(aLayer->Related(anEndpoint).IsEmpty());
}

TEST(BRepGraphSupInc_TopologyStoreTest, IncompatibleReplacementDispatchesTopologyRemoval)
{
  BRepGraph aGraph;
  const BRepGraph_VertexId aVertex = aGraph.Editor().Vertices().Add(gp_Pnt(), 1.0e-7);
  const BRepGraph_VertexId anEndVertex =
    aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_EdgeId anEdge = aGraph.Editor().Edges().Add(
    aVertex, anEndVertex, occ::handle<Geom_Curve>(), 0.0, 1.0, 1.0e-7);
  const BRepGraphSupInc_TopologyId anAttachment = aGraph.Supplements().Attach(
    aVertex, BRepGraphSupInc::TopologyAttachmentKind::VertexSupplementShape, makeVertex(gp_Pnt()));
  ASSERT_TRUE(anAttachment.IsValid());
  const BRepGraphSupInc_TopologyStore* aTopology = topologyStore(aGraph);
  ASSERT_NE(aTopology, nullptr);
  const BRepGraphSupInc_Endpoint anEndpoint = BRepGraphSupInc_Endpoint::Supplemental(
    aTopology->ItemUID(anAttachment));
  const occ::handle<BRepGraph_LayerSupplement> aLayer =
    aGraph.LayerSupplementRegistry().Ensure<BRepGraph_LayerSupplement>();
  ASSERT_NE(aLayer->Add(BRepGraphSupInc_Endpoint::Core(BRepGraph_ItemId(aVertex)),
                        anEndpoint,
                        BRepGraph_LayerSupplement::GetID()),
            0u);

  aGraph.Supplements().ReplaceOwnerAttachments(aVertex, anEdge);

  EXPECT_TRUE(aGraph.Supplements().Attachments(aVertex).IsEmpty());
  EXPECT_TRUE(aGraph.Supplements().Attachments(anEdge).IsEmpty());
  EXPECT_TRUE(aLayer->Related(anEndpoint).IsEmpty());
}

TEST(BRepGraphSupInc_TopologyStoreTest, GraphClearDoesNotReuseAttachmentUIDCounter)
{
  BRepGraph aGraph;
  const BRepGraph_VertexId anOwner = aGraph.Editor().Vertices().Add(gp_Pnt(), 1.0e-7);
  const BRepGraphSupInc_TopologyId aFirstAttachment = aGraph.Supplements().Attach(
    anOwner, BRepGraphSupInc::TopologyAttachmentKind::VertexSupplementShape, makeVertex(gp_Pnt()));
  ASSERT_TRUE(aFirstAttachment.IsValid());
  const BRepGraphSupInc_TopologyStore* aTopology = topologyStore(aGraph);
  ASSERT_NE(aTopology, nullptr);
  const BRepGraphSupInc_ItemUID aFirstUID = aTopology->ItemUID(aFirstAttachment);

  aGraph.Clear();
  const BRepGraph_VertexId aNewOwner = aGraph.Editor().Vertices().Add(gp_Pnt(), 1.0e-7);
  const BRepGraphSupInc_TopologyId aSecondAttachment = aGraph.Supplements().Attach(
    aNewOwner, BRepGraphSupInc::TopologyAttachmentKind::VertexSupplementShape, makeVertex(gp_Pnt(1, 0, 0)));
  ASSERT_TRUE(aSecondAttachment.IsValid());
  const BRepGraphSupInc_ItemUID aSecondUID = aTopology->ItemUID(aSecondAttachment);
  EXPECT_NE(aSecondUID, aFirstUID);
  EXPECT_GT(aSecondUID.Counter, aFirstUID.Counter);
}

TEST(BRepGraphSupInc_TopologyStoreTest, CompactRetainsDirectShape)
{
  BRepGraph aGraph;
  const BRepGraph_VertexId anOwner = aGraph.Editor().Vertices().Add(gp_Pnt(), 1.0e-7);
  const BRepGraphSupInc_TopologyId aRemoved = aGraph.Supplements().Attach(
    anOwner, BRepGraphSupInc::TopologyAttachmentKind::VertexSupplementShape, makeVertex(gp_Pnt(1, 0, 0)));
  const BRepGraphSupInc_TopologyId aRetained = aGraph.Supplements().Attach(
    anOwner, BRepGraphSupInc::TopologyAttachmentKind::VertexSupplementShape, makeVertex(gp_Pnt(2, 0, 0)));
  ASSERT_TRUE(aRemoved.IsValid());
  ASSERT_TRUE(aRetained.IsValid());
  ASSERT_TRUE(aGraph.Supplements().RemoveAttachment(aRemoved));

  const occ::handle<BRepGraphSupInc_TopologyStore> aTopology =
    occ::down_cast<BRepGraphSupInc_TopologyStore>(
      aGraph.Supplements().FindStore(BRepGraphSupInc_TopologyStore::GetID()));
  ASSERT_FALSE(aTopology.IsNull());
  aGraph.Supplements().Compact();

  const NCollection_LinearVector<BRepGraphSupInc_TopologyId>& anAttachments =
    aGraph.Supplements().Attachments(anOwner);
  ASSERT_EQ(anAttachments.Size(), 1u);
  const BRepGraphSupInc::TopologyDef* anAttachment = aGraph.Supplements().Attachment(anAttachments.First());
  ASSERT_NE(anAttachment, nullptr);
  EXPECT_EQ(anAttachment->Shape.ShapeType(), TopAbs_VERTEX);
}

TEST(BRepGraphSupInc_TopologyStoreTest, GraphCompactCompactsSoftRemovedAttachments)
{
  BRepGraph aGraph;
  const BRepGraph_VertexId anOwner = aGraph.Editor().Vertices().Add(gp_Pnt(), 1.0e-7);
  const BRepGraphSupInc_TopologyId aRemoved = aGraph.Supplements().Attach(
    anOwner, BRepGraphSupInc::TopologyAttachmentKind::VertexSupplementShape, makeVertex(gp_Pnt(1, 0, 0)));
  const BRepGraphSupInc_TopologyId aRetained = aGraph.Supplements().Attach(
    anOwner, BRepGraphSupInc::TopologyAttachmentKind::VertexSupplementShape, makeVertex(gp_Pnt(2, 0, 0)));
  ASSERT_TRUE(aRemoved.IsValid());
  ASSERT_TRUE(aRetained.IsValid());
  const occ::handle<BRepGraphSupInc_TopologyStore> aTopology =
    occ::down_cast<BRepGraphSupInc_TopologyStore>(
      aGraph.Supplements().FindStore(BRepGraphSupInc_TopologyStore::GetID()));
  ASSERT_FALSE(aTopology.IsNull());
  const BRepGraphSupInc_ItemUID aRetainedUID = aTopology->ItemUID(aRetained);
  ASSERT_TRUE(aGraph.Supplements().RemoveAttachment(aRemoved));
  EXPECT_EQ(aTopology->Count(), 2u);

  const BRepGraph_Compact::Result aResult = BRepGraph_Compact::Perform(aGraph);

  EXPECT_EQ(aResult.NbNodesAfter, aResult.NbNodesBefore);
  const BRepGraphSupInc_TopologyStore* aCompactedTopology = topologyStore(aGraph);
  ASSERT_NE(aCompactedTopology, nullptr);
  EXPECT_NE(aCompactedTopology, aTopology.get());
  EXPECT_EQ(aCompactedTopology->Count(), 1u);
  EXPECT_TRUE(aGraph.Supplements().Has(aRetainedUID));
  EXPECT_TRUE(aCompactedTopology->FindByUID(aRetainedUID).IsValid());
  EXPECT_EQ(aTopology->Count(), 2u);
}

TEST(BRepGraphSupInc_TopologyStoreTest, RuntimeUIDLookupReturnsGenericAndTypedIDs)
{
  BRepGraph aGraph;
  const BRepGraph_VertexId anOwner = aGraph.Editor().Vertices().Add(gp_Pnt(), 1.0e-7);
  const BRepGraphSupInc_TopologyId anAttachment = aGraph.Supplements().Attach(
    anOwner, BRepGraphSupInc::TopologyAttachmentKind::VertexSupplementShape, makeVertex(gp_Pnt()));
  ASSERT_TRUE(anAttachment.IsValid());
  const occ::handle<BRepGraphSupInc_TopologyStore> aTopology =
    occ::down_cast<BRepGraphSupInc_TopologyStore>(
      aGraph.Supplements().FindStore(BRepGraphSupInc_TopologyStore::GetID()));
  ASSERT_FALSE(aTopology.IsNull());
  const BRepGraphSupInc_ItemUID anUID = aTopology->ItemUID(anAttachment);

  const occ::handle<BRepGraphSupInc_Store> aStore = aTopology;
  const BRepGraphSupInc_DefinitionId aDefinition = aStore->FindDefinitionByUID(anUID);
  ASSERT_TRUE(aDefinition.IsValid());
  EXPECT_EQ(aDefinition,
            (BRepGraphSupInc_DefinitionId{
              BRepGraphSupInc_TopologyStore::THE_TOPOLOGY_KIND, anAttachment.Index}));
  EXPECT_EQ(aTopology->FindByUID(anUID), anAttachment);

  const BRepGraphSupInc_ItemUID aWrongStoreUID{0, anUID.Kind, anUID.Counter};
  const BRepGraphSupInc_ItemUID aWrongKindUID{aTopology->StoreId(), 2, anUID.Counter};
  const BRepGraphSupInc_ItemUID aMissingUID{aTopology->StoreId(), anUID.Kind, anUID.Counter + 1};
  EXPECT_FALSE(aStore->FindDefinitionByUID(aWrongStoreUID).IsValid());
  EXPECT_FALSE(aStore->FindDefinitionByUID(aWrongKindUID).IsValid());
  EXPECT_FALSE(aGraph.Supplements().FindDefinitionByUID(aMissingUID).IsValid());
  EXPECT_FALSE(aTopology->FindByUID(aWrongStoreUID).IsValid());
  EXPECT_FALSE(aTopology->FindByUID(aWrongKindUID).IsValid());
  EXPECT_FALSE(aTopology->FindByUID(aMissingUID).IsValid());
  ASSERT_TRUE(aGraph.Supplements().Remove(anUID));
  EXPECT_FALSE(aStore->FindDefinitionByUID(anUID).IsValid());
  EXPECT_FALSE(aTopology->FindByUID(anUID).IsValid());
}

TEST(BRepGraphSupInc_TopologyStoreTest, AttachmentInvalidatesOwnerAndAncestorShapes)
{
  BRepGraph aGraph;
  const BRepGraph_VertexId aStart = aGraph.Editor().Vertices().Add(gp_Pnt(), 1.0e-7);
  const BRepGraph_VertexId anEnd = aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_EdgeId anEdge = aGraph.Editor().Edges().Add(
    aStart, anEnd, occ::handle<Geom_Curve>(), 0.0, 1.0, 1.0e-7);
  NCollection_LinearVector<BRepGraph_NodeId> aChildren;
  aChildren.Append(anEdge);
  const BRepGraph_CompoundId aCompound = aGraph.Editor().Compounds().Add(aChildren.ToArray1());

  EXPECT_EQ(vertexCount(aGraph.Shapes().Shape(anEdge)), 2);
  EXPECT_EQ(vertexCount(aGraph.Shapes().Shape(aCompound)), 2);
  const BRepGraphSupInc_TopologyId anAttachment = aGraph.Supplements().Attach(
    anEdge, BRepGraphSupInc::TopologyAttachmentKind::EdgeInternalVertex, makeVertex(gp_Pnt(0.5, 0.0, 0.0)));
  ASSERT_TRUE(anAttachment.IsValid());
  EXPECT_EQ(vertexCount(aGraph.Shapes().Shape(anEdge)), 3);
  EXPECT_EQ(vertexCount(aGraph.Shapes().Shape(aCompound)), 3);
  EXPECT_EQ(vertexCount(aGraph.Shapes().Reconstruct(aCompound)), 3);

  ASSERT_TRUE(aGraph.Supplements().RemoveAttachment(anAttachment));
  EXPECT_EQ(vertexCount(aGraph.Shapes().Shape(anEdge)), 2);
  EXPECT_EQ(vertexCount(aGraph.Shapes().Shape(aCompound)), 2);
  EXPECT_EQ(vertexCount(aGraph.Shapes().Reconstruct(aCompound)), 2);
}

TEST(BRepGraphSupInc_TopologyStoreTest, ShapesViewRoutesSupplementWithoutCoreAppend)
{
  BRepGraph aGraph;
  const BRepGraph_ShellId aShell = aGraph.Editor().Shells().Add();
  TopoDS_Edge anEdge;
  BRep_Builder aBuilder;
  aBuilder.MakeEdge(anEdge);
  const BRepGraph::ShapesView::Result aResult = aGraph.Shapes().Add(anEdge, aShell);
  ASSERT_TRUE(aResult.IsOk());
  EXPECT_FALSE(aResult.TopologyRoot.IsValid());
  EXPECT_EQ(aGraph.Supplements().Attachments(aShell).Size(), 1);
}

TEST(BRepGraphSupInc_TopologyStoreTest, CopyPreservesDirectShapeThroughStorage)
{
  BRepGraph aSource;
  const BRepGraph_VertexId aSourceOwner = aSource.Editor().Vertices().Add(gp_Pnt(), 1.0e-7);
  ASSERT_TRUE(aSource.Supplements().Attach(
    aSourceOwner, BRepGraphSupInc::TopologyAttachmentKind::VertexSupplementShape, makeVertex(gp_Pnt(1, 0, 0))).IsValid());

  BRepGraph anIdentityTarget;
  ASSERT_TRUE(BRepGraph_Copy::Perform(aSource,
                                       anIdentityTarget,
                                       BRepGraph_Copy::GeomPolicy::Drop,
                                       BRepGraph_Copy::MeshPolicy::Drop));
  const NCollection_LinearVector<BRepGraphSupInc_TopologyId>& anIdentityAttachments =
    anIdentityTarget.Supplements().Attachments(aSourceOwner);
  ASSERT_EQ(anIdentityAttachments.Size(), 1u);
  const BRepGraphSupInc::TopologyDef* anIdentityAttachment =
    anIdentityTarget.Supplements().Attachment(anIdentityAttachments.First());
  ASSERT_NE(anIdentityAttachment, nullptr);
  EXPECT_EQ(anIdentityAttachment->Shape.ShapeType(), TopAbs_VERTEX);

  BRepGraph anAppendTarget;
  (void)anAppendTarget.Editor().Vertices().Add(gp_Pnt(-1, 0, 0), 1.0e-7);
  ASSERT_TRUE(BRepGraph_Copy::Perform(aSource,
                                       anAppendTarget,
                                       BRepGraph_Copy::GeomPolicy::Drop,
                                       BRepGraph_Copy::MeshPolicy::Drop));
  const BRepGraph_VertexId anAppendedOwner(1);
  const NCollection_LinearVector<BRepGraphSupInc_TopologyId>& anAppendedAttachments =
    anAppendTarget.Supplements().Attachments(anAppendedOwner);
  ASSERT_EQ(anAppendedAttachments.Size(), 1u);
  const BRepGraphSupInc::TopologyDef* anAppendedAttachment =
    anAppendTarget.Supplements().Attachment(anAppendedAttachments.First());
  ASSERT_NE(anAppendedAttachment, nullptr);
  EXPECT_EQ(anAppendedAttachment->Shape.ShapeType(), TopAbs_VERTEX);
}
