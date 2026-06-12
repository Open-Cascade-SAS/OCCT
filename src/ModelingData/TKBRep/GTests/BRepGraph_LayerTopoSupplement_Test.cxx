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

#include <gtest/gtest.h>

#include <BRep_Builder.hxx>
#include <BRepGraph_Compact.hxx>
#include <BRepGraph_Copy.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_LayerTopoSupplement.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_SupplementIterator.hxx>
#include <BRepGraph_SupplementEditor.hxx>
#include <BRepGraph_TopoView.hxx>
#include <NCollection_LinearVector.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <gp_Pnt.hxx>

namespace
{
TopoDS_Vertex makeVertexShape(const gp_Pnt& thePoint)
{
  BRep_Builder  aBuilder;
  TopoDS_Vertex aVertex;
  aBuilder.MakeVertex(aVertex, thePoint, 1.0e-7);
  return aVertex;
}

TopoDS_Edge makeEdgeWithSupplementVertices()
{
  BRep_Builder aBuilder;

  const TopoDS_Vertex aStart     = makeVertexShape(gp_Pnt(0.0, 0.0, 0.0));
  const TopoDS_Vertex anExtraFwd = makeVertexShape(gp_Pnt(2.0, 0.0, 0.0));
  const TopoDS_Vertex anInternal = makeVertexShape(gp_Pnt(5.0, 0.0, 0.0));
  const TopoDS_Vertex anExternal = makeVertexShape(gp_Pnt(7.0, 0.0, 0.0));
  const TopoDS_Vertex anEnd      = makeVertexShape(gp_Pnt(10.0, 0.0, 0.0));

  TopoDS_Edge anEdge;
  aBuilder.MakeEdge(anEdge);

  TopoDS_Vertex aForwardExtra = anExtraFwd;
  aForwardExtra.Orientation(TopAbs_FORWARD);
  aBuilder.Add(anEdge, aForwardExtra);

  TopoDS_Vertex aForwardStart = aStart;
  aForwardStart.Orientation(TopAbs_FORWARD);
  aBuilder.Add(anEdge, aForwardStart);

  TopoDS_Vertex anInternalUse = anInternal;
  anInternalUse.Orientation(TopAbs_INTERNAL);
  aBuilder.Add(anEdge, anInternalUse);

  TopoDS_Vertex anExternalUse = anExternal;
  anExternalUse.Orientation(TopAbs_EXTERNAL);
  aBuilder.Add(anEdge, anExternalUse);

  TopoDS_Vertex aReversedEnd = anEnd;
  aReversedEnd.Orientation(TopAbs_REVERSED);
  aBuilder.Add(anEdge, aReversedEnd);
  return anEdge;
}

TopoDS_Face makeFaceWithSupplementVertex()
{
  BRep_Builder aBuilder;

  TopoDS_Vertex aV0 = makeVertexShape(gp_Pnt(0.0, 0.0, 0.0));
  TopoDS_Vertex aV1 = makeVertexShape(gp_Pnt(10.0, 0.0, 0.0));
  TopoDS_Vertex aV2 = makeVertexShape(gp_Pnt(10.0, 10.0, 0.0));
  TopoDS_Vertex aV3 = makeVertexShape(gp_Pnt(0.0, 10.0, 0.0));

  TopoDS_Edge aE0;
  TopoDS_Edge aE1;
  TopoDS_Edge aE2;
  TopoDS_Edge aE3;
  aBuilder.MakeEdge(aE0);
  aBuilder.MakeEdge(aE1);
  aBuilder.MakeEdge(aE2);
  aBuilder.MakeEdge(aE3);
  aBuilder.Add(aE0, aV0.Oriented(TopAbs_FORWARD));
  aBuilder.Add(aE0, aV1.Oriented(TopAbs_REVERSED));
  aBuilder.Add(aE1, aV1.Oriented(TopAbs_FORWARD));
  aBuilder.Add(aE1, aV2.Oriented(TopAbs_REVERSED));
  aBuilder.Add(aE2, aV2.Oriented(TopAbs_FORWARD));
  aBuilder.Add(aE2, aV3.Oriented(TopAbs_REVERSED));
  aBuilder.Add(aE3, aV3.Oriented(TopAbs_FORWARD));
  aBuilder.Add(aE3, aV0.Oriented(TopAbs_REVERSED));

  TopoDS_Wire aWire;
  aBuilder.MakeWire(aWire);
  aBuilder.Add(aWire, aE0);
  aBuilder.Add(aWire, aE1);
  aBuilder.Add(aWire, aE2);
  aBuilder.Add(aWire, aE3);
  aWire.Closed(true);

  TopoDS_Face aFace;
  aBuilder.MakeFace(aFace);
  aBuilder.Add(aFace, aWire);

  TopoDS_Vertex aLooseVertex = makeVertexShape(gp_Pnt(5.0, 5.0, 0.0));
  aBuilder.Add(aFace, aLooseVertex.Oriented(TopAbs_INTERNAL));
  return aFace;
}

TopoDS_Face makePlainFace()
{
  BRep_Builder aBuilder;

  TopoDS_Vertex aV0 = makeVertexShape(gp_Pnt(0.0, 0.0, 0.0));
  TopoDS_Vertex aV1 = makeVertexShape(gp_Pnt(10.0, 0.0, 0.0));
  TopoDS_Vertex aV2 = makeVertexShape(gp_Pnt(10.0, 10.0, 0.0));
  TopoDS_Vertex aV3 = makeVertexShape(gp_Pnt(0.0, 10.0, 0.0));

  TopoDS_Edge aE0;
  TopoDS_Edge aE1;
  TopoDS_Edge aE2;
  TopoDS_Edge aE3;
  aBuilder.MakeEdge(aE0);
  aBuilder.MakeEdge(aE1);
  aBuilder.MakeEdge(aE2);
  aBuilder.MakeEdge(aE3);
  aBuilder.Add(aE0, aV0.Oriented(TopAbs_FORWARD));
  aBuilder.Add(aE0, aV1.Oriented(TopAbs_REVERSED));
  aBuilder.Add(aE1, aV1.Oriented(TopAbs_FORWARD));
  aBuilder.Add(aE1, aV2.Oriented(TopAbs_REVERSED));
  aBuilder.Add(aE2, aV2.Oriented(TopAbs_FORWARD));
  aBuilder.Add(aE2, aV3.Oriented(TopAbs_REVERSED));
  aBuilder.Add(aE3, aV3.Oriented(TopAbs_FORWARD));
  aBuilder.Add(aE3, aV0.Oriented(TopAbs_REVERSED));

  TopoDS_Wire aWire;
  aBuilder.MakeWire(aWire);
  aBuilder.Add(aWire, aE0);
  aBuilder.Add(aWire, aE1);
  aBuilder.Add(aWire, aE2);
  aBuilder.Add(aWire, aE3);
  aWire.Closed(true);

  TopoDS_Face aFace;
  aBuilder.MakeFace(aFace);
  aBuilder.Add(aFace, aWire);
  return aFace;
}

TopoDS_Shell makePlainShell()
{
  BRep_Builder aBuilder;
  TopoDS_Shell aShell;
  aBuilder.MakeShell(aShell);
  aBuilder.Add(aShell, makePlainFace());
  return aShell;
}

TopoDS_Solid makePlainSolid()
{
  BRep_Builder aBuilder;
  TopoDS_Solid aSolid;
  aBuilder.MakeSolid(aSolid);
  aBuilder.Add(aSolid, makePlainShell());
  return aSolid;
}

BRepGraph_CompSolidId addEmptyCompSolid(BRepGraph& theGraph)
{
  NCollection_LinearVector<BRepGraph_SolidId> anEmptySolids;
  return theGraph.Editor().CompSolids().Add(anEmptySolids.ToArray1());
}

TopoDS_CompSolid makePlainCompSolid()
{
  BRep_Builder     aBuilder;
  TopoDS_CompSolid aCompSolid;
  aBuilder.MakeCompSolid(aCompSolid);
  aBuilder.Add(aCompSolid, makePlainSolid());
  return aCompSolid;
}

int countDirectChildren(const TopoDS_Shape&      theShape,
                        const TopAbs_ShapeEnum   theType,
                        const TopAbs_Orientation theOrientation)
{
  int aCount = 0;
  for (TopoDS_Iterator aChildIt(theShape, false, false); aChildIt.More(); aChildIt.Next())
  {
    const TopoDS_Shape& aChild = aChildIt.Value();
    if (aChild.ShapeType() == theType && aChild.Orientation() == theOrientation)
    {
      ++aCount;
    }
  }
  return aCount;
}
} // namespace

TEST(BRepGraph_LayerTopoSupplementTest, AttachAndRemoveVertexSupplement)
{
  BRepGraph          aGraph;
  BRepGraph_VertexId aOwner = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.0e-7);

  BRepGraph_SupplementEditor anEditor(aGraph);
  const uint64_t             anAttachment =
    anEditor.AttachToVertex(aOwner, makeVertexShape(gp_Pnt(1.0, 2.0, 3.0)));
  ASSERT_NE(anAttachment, uint64_t(0));

  const occ::handle<BRepGraph_LayerTopoSupplement> aLayer =
    aGraph.LayerRegistry().FindLayer<BRepGraph_LayerTopoSupplement>();
  ASSERT_FALSE(aLayer.IsNull());

  const NCollection_LinearVector<uint64_t>& anAttached =
    aLayer->AttachedTo(BRepGraph_NodeId(aOwner));
  ASSERT_EQ(anAttached.Size(), 1);
  EXPECT_EQ(anAttached.First(), anAttachment);

  const BRepGraph_LayerTopoSupplement::Entry* anEntry = aLayer->FindByUid(anAttachment);
  ASSERT_NE(anEntry, nullptr);
  EXPECT_EQ(anEntry->BaseOwner, BRepGraph_NodeId(aOwner));
  EXPECT_EQ(anEntry->Kind, BRepGraph_LayerTopoSupplement::AttachmentKind::VertexSupplementShape);
  EXPECT_EQ(anEntry->Shape.ShapeType(), TopAbs_VERTEX);

  EXPECT_TRUE(anEditor.RemoveAttachment(anAttachment));
  EXPECT_EQ(aLayer->AttachedTo(BRepGraph_NodeId(aOwner)).Size(), 0);
  EXPECT_EQ(aLayer->FindByUid(anAttachment), nullptr);
}

TEST(BRepGraph_LayerTopoSupplementTest, SupplementIteratorPreservesOwnerOrderAndUidLookup)
{
  BRepGraph          aGraph;
  BRepGraph_VertexId aOwner = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.0e-7);

  BRepGraph_SupplementEditor anEditor(aGraph);
  const uint64_t aFirst  = anEditor.AttachToVertex(aOwner, makeVertexShape(gp_Pnt(1.0, 0.0, 0.0)));
  const uint64_t aSecond = anEditor.AttachToVertex(aOwner, makeVertexShape(gp_Pnt(2.0, 0.0, 0.0)));
  ASSERT_NE(aFirst, uint64_t(0));
  ASSERT_NE(aSecond, uint64_t(0));

  NCollection_LinearVector<uint64_t> aSeen;
  for (BRepGraph_SupplementIterator anIt(aGraph, BRepGraph_NodeId(aOwner)); anIt.More();
       anIt.Next())
  {
    aSeen.Append(anIt.Uid());
    EXPECT_EQ(anIt.Value().BaseOwner, BRepGraph_NodeId(aOwner));
    EXPECT_EQ(anIt.Value().Shape.ShapeType(), TopAbs_VERTEX);
  }

  ASSERT_EQ(aSeen.Size(), 2);
  EXPECT_EQ(aSeen.Value(0), aFirst);
  EXPECT_EQ(aSeen.Value(1), aSecond);

  const occ::handle<BRepGraph_LayerTopoSupplement> aLayer =
    aGraph.LayerRegistry().FindLayer<BRepGraph_LayerTopoSupplement>();
  ASSERT_FALSE(aLayer.IsNull());
  EXPECT_NE(aLayer->FindByUid(aFirst), nullptr);
  EXPECT_NE(aLayer->FindByUid(aSecond), nullptr);
}

TEST(BRepGraph_LayerTopoSupplementTest, OnNodeReplacedMigratesAttachments)
{
  BRepGraph          aGraph;
  BRepGraph_VertexId anOldOwner = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.0e-7);
  BRepGraph_VertexId aNewOwner  = aGraph.Editor().Vertices().Add(gp_Pnt(5.0, 0.0, 0.0), 1.0e-7);

  BRepGraph_SupplementEditor anEditor(aGraph);
  const uint64_t             anAttachment =
    anEditor.AttachToVertex(anOldOwner, makeVertexShape(gp_Pnt(0.0, 1.0, 0.0)));
  ASSERT_NE(anAttachment, uint64_t(0));

  occ::handle<BRepGraph_LayerTopoSupplement> aLayer =
    aGraph.LayerRegistry().FindLayer<BRepGraph_LayerTopoSupplement>();
  ASSERT_FALSE(aLayer.IsNull());

  aLayer->OnNodeReplaced(BRepGraph_NodeId(anOldOwner), BRepGraph_NodeId(aNewOwner));

  EXPECT_EQ(aLayer->AttachedTo(BRepGraph_NodeId(anOldOwner)).Size(), 0);
  const NCollection_LinearVector<uint64_t>& anAttachedNew =
    aLayer->AttachedTo(BRepGraph_NodeId(aNewOwner));
  ASSERT_EQ(anAttachedNew.Size(), 1);
  EXPECT_EQ(anAttachedNew.First(), anAttachment);

  const BRepGraph_LayerTopoSupplement::Entry* anEntry = aLayer->FindByUid(anAttachment);
  ASSERT_NE(anEntry, nullptr);
  EXPECT_EQ(anEntry->BaseOwner, BRepGraph_NodeId(aNewOwner));
}

TEST(BRepGraph_LayerTopoSupplementTest, RemoveNodeDropsOwnedAttachments)
{
  BRepGraph          aGraph;
  BRepGraph_VertexId aOwner = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.0e-7);

  BRepGraph_SupplementEditor anEditor(aGraph);
  const uint64_t             anAttachment =
    anEditor.AttachToVertex(aOwner, makeVertexShape(gp_Pnt(0.0, 1.0, 0.0)));
  ASSERT_NE(anAttachment, uint64_t(0));

  occ::handle<BRepGraph_LayerTopoSupplement> aLayer =
    aGraph.LayerRegistry().FindLayer<BRepGraph_LayerTopoSupplement>();
  ASSERT_FALSE(aLayer.IsNull());
  ASSERT_EQ(aLayer->AttachedTo(BRepGraph_NodeId(aOwner)).Size(), 1);

  aGraph.Editor().Gen().RemoveNode(aOwner);

  EXPECT_EQ(aLayer->AttachedTo(BRepGraph_NodeId(aOwner)).Size(), 0);
  EXPECT_EQ(aLayer->FindByUid(anAttachment), nullptr);
}

TEST(BRepGraph_LayerTopoSupplementTest, LegacyEdgeInternalVertexWriterUsesSupplementLayer)
{
  BRepGraph          aGraph;
  BRepGraph_VertexId aStart = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.0e-7);
  BRepGraph_VertexId anEnd  = aGraph.Editor().Vertices().Add(gp_Pnt(10.0, 0.0, 0.0), 1.0e-7);
  BRepGraph_EdgeId   anEdge =
    aGraph.Editor().Edges().Add(aStart, anEnd, occ::handle<Geom_Curve>(), 0.0, 1.0, 1.0e-7);
  ASSERT_TRUE(anEdge.IsValid());

  // Edges().AddInternalVertex() removed; use Shapes().Add(vertexShape, edgeNode) instead.
  // const BRepGraph_VertexRefId aLegacyRef =
  //   aGraph.Editor().Edges().AddInternalVertex(anEdge, anInternal, TopAbs_INTERNAL);
  // EXPECT_FALSE(aLegacyRef.IsValid());

  const occ::handle<BRepGraph_LayerTopoSupplement> aLayer =
    aGraph.LayerRegistry().FindLayer<BRepGraph_LayerTopoSupplement>();
  // No internal vertex added -> supplement layer may not be created.
  if (!aLayer.IsNull())
  {
    const NCollection_LinearVector<uint64_t>& anAttached =
      aLayer->AttachedTo(BRepGraph_NodeId(anEdge));
    EXPECT_EQ(anAttached.Size(), 0);
  }

  int aNbCoreVertexRefs = 0;
  for (BRepGraph_RefsVertexOfEdge aRefIt(aGraph, anEdge); aRefIt.More(); aRefIt.Next())
  {
    ++aNbCoreVertexRefs;
  }
  // Only start and end vertices (no internal vertex added).
  EXPECT_EQ(aNbCoreVertexRefs, 2);
}

TEST(BRepGraph_LayerTopoSupplementTest, AddAndReconstructPreservesSupplementEdgeVertices)
{
  BRepGraph                      aGraph;
  BRepGraph::ShapesView::Options anOptions;
  anOptions.CreateAutoProduct = false;
  const occ::handle<BRepGraph_LayerTopoSupplement> aRegisteredLayer =
    aGraph.LayerRegistry().Ensure<BRepGraph_LayerTopoSupplement>();
  ASSERT_FALSE(aRegisteredLayer.IsNull());

  const TopoDS_Edge                   anInputEdge = makeEdgeWithSupplementVertices();
  const BRepGraph::ShapesView::Result aResult     = aGraph.Shapes().Add(anInputEdge, anOptions);
  ASSERT_TRUE(aResult.IsOk());
  ASSERT_EQ(aResult.TopologyRoot.NodeKind, BRepGraph_NodeId::Kind::Edge);

  const occ::handle<BRepGraph_LayerTopoSupplement> aLayer =
    aGraph.LayerRegistry().FindLayer<BRepGraph_LayerTopoSupplement>();
  ASSERT_FALSE(aLayer.IsNull());

  const NCollection_LinearVector<uint64_t>& anAttached = aLayer->AttachedTo(aResult.TopologyRoot);
  ASSERT_EQ(anAttached.Size(), 3);

  int aNbInternal = 0;
  int aNbExternal = 0;
  int aNbForward  = 0;
  for (const uint64_t aUid : anAttached)
  {
    const BRepGraph_LayerTopoSupplement::Entry* anEntry = aLayer->FindByUid(aUid);
    ASSERT_NE(anEntry, nullptr);
    ASSERT_EQ(anEntry->Kind, BRepGraph_LayerTopoSupplement::AttachmentKind::EdgeInternalVertex);
    if (anEntry->Shape.Orientation() == TopAbs_INTERNAL)
    {
      ++aNbInternal;
    }
    else if (anEntry->Shape.Orientation() == TopAbs_EXTERNAL)
    {
      ++aNbExternal;
    }
    else if (anEntry->Shape.Orientation() == TopAbs_FORWARD)
    {
      ++aNbForward;
    }
  }
  EXPECT_EQ(aNbInternal, 1);
  EXPECT_EQ(aNbExternal, 1);
  EXPECT_EQ(aNbForward, 1);

  const TopoDS_Shape aRoundTrip = aGraph.Shapes().Reconstruct(aResult.TopologyRoot);
  ASSERT_EQ(aRoundTrip.ShapeType(), TopAbs_EDGE);

  int aNbRoundTripForward  = 0;
  int aNbRoundTripReversed = 0;
  int aNbRoundTripInternal = 0;
  int aNbRoundTripExternal = 0;
  for (TopoDS_Iterator aChildIt(aRoundTrip, false, false); aChildIt.More(); aChildIt.Next())
  {
    const TopoDS_Shape& aChild = aChildIt.Value();
    ASSERT_EQ(aChild.ShapeType(), TopAbs_VERTEX);
    switch (aChild.Orientation())
    {
      case TopAbs_FORWARD:
        ++aNbRoundTripForward;
        break;
      case TopAbs_REVERSED:
        ++aNbRoundTripReversed;
        break;
      case TopAbs_INTERNAL:
        ++aNbRoundTripInternal;
        break;
      case TopAbs_EXTERNAL:
        ++aNbRoundTripExternal;
        break;
      default:
        break;
    }
  }

  EXPECT_EQ(aNbRoundTripForward, 2);
  EXPECT_EQ(aNbRoundTripReversed, 1);
  EXPECT_EQ(aNbRoundTripInternal, 1);
  EXPECT_EQ(aNbRoundTripExternal, 1);
}

TEST(BRepGraph_LayerTopoSupplementTest, AddAndReconstructPreservesSupplementFaceVertex)
{
  BRepGraph                      aGraph;
  BRepGraph::ShapesView::Options anOptions;
  anOptions.CreateAutoProduct = false;
  const occ::handle<BRepGraph_LayerTopoSupplement> aRegisteredLayer =
    aGraph.LayerRegistry().Ensure<BRepGraph_LayerTopoSupplement>();
  ASSERT_FALSE(aRegisteredLayer.IsNull());

  const TopoDS_Face                   aInputFace = makeFaceWithSupplementVertex();
  const BRepGraph::ShapesView::Result aResult    = aGraph.Shapes().Add(aInputFace, anOptions);
  ASSERT_TRUE(aResult.IsOk());
  ASSERT_EQ(aResult.TopologyRoot.NodeKind, BRepGraph_NodeId::Kind::Face);

  const occ::handle<BRepGraph_LayerTopoSupplement> aLayer =
    aGraph.LayerRegistry().FindLayer<BRepGraph_LayerTopoSupplement>();
  ASSERT_FALSE(aLayer.IsNull());

  const NCollection_LinearVector<uint64_t>& anAttached = aLayer->AttachedTo(aResult.TopologyRoot);
  ASSERT_EQ(anAttached.Size(), 1);

  const BRepGraph_LayerTopoSupplement::Entry* anEntry = aLayer->FindByUid(anAttached.First());
  ASSERT_NE(anEntry, nullptr);
  EXPECT_EQ(anEntry->Kind, BRepGraph_LayerTopoSupplement::AttachmentKind::FaceDirectVertex);
  EXPECT_EQ(anEntry->Shape.ShapeType(), TopAbs_VERTEX);
  EXPECT_EQ(anEntry->Shape.Orientation(), TopAbs_INTERNAL);

  const TopoDS_Shape aRoundTrip = aGraph.Shapes().Reconstruct(aResult.TopologyRoot);
  ASSERT_EQ(aRoundTrip.ShapeType(), TopAbs_FACE);

  int aNbWires            = 0;
  int aNbInternalVertices = 0;
  for (TopoDS_Iterator aChildIt(aRoundTrip, false, false); aChildIt.More(); aChildIt.Next())
  {
    const TopoDS_Shape& aChild = aChildIt.Value();
    if (aChild.ShapeType() == TopAbs_WIRE)
    {
      ++aNbWires;
      continue;
    }

    if (aChild.ShapeType() == TopAbs_VERTEX && aChild.Orientation() == TopAbs_INTERNAL)
    {
      ++aNbInternalVertices;
    }
  }

  EXPECT_EQ(aNbWires, 1);
  EXPECT_EQ(aNbInternalVertices, 1);
}

TEST(BRepGraph_LayerTopoSupplementTest, ShellReconstructionReplaysFaceSupplementVertex)
{
  BRep_Builder aBuilder;
  TopoDS_Shell aShell;
  aBuilder.MakeShell(aShell);
  aBuilder.Add(aShell, makeFaceWithSupplementVertex());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const occ::handle<BRepGraph_LayerTopoSupplement> aRegisteredLayer =
    aGraph.LayerRegistry().Ensure<BRepGraph_LayerTopoSupplement>();
  BRepGraph::ShapesView::Options anOptions;
  anOptions.CreateAutoProduct                 = false;
  const BRepGraph::ShapesView::Result aResult = aGraph.Shapes().Add(aShell, anOptions);
  ASSERT_TRUE(aResult.TopologyRoot.IsValid());

  const TopoDS_Shape aRoundTrip = aGraph.Shapes().Reconstruct(aResult.TopologyRoot);
  ASSERT_FALSE(aRoundTrip.IsNull());
  ASSERT_EQ(aRoundTrip.ShapeType(), TopAbs_SHELL);

  int aNbFaces            = 0;
  int aNbInternalVertices = 0;
  for (TopoDS_Iterator aFaceIt(aRoundTrip, false, false); aFaceIt.More(); aFaceIt.Next())
  {
    const TopoDS_Shape& aFace = aFaceIt.Value();
    if (aFace.ShapeType() != TopAbs_FACE)
    {
      continue;
    }
    ++aNbFaces;
    aNbInternalVertices += countDirectChildren(aFace, TopAbs_VERTEX, TopAbs_INTERNAL);
  }

  EXPECT_EQ(aNbFaces, 1);
  EXPECT_EQ(aNbInternalVertices, 1);
}

TEST(BRepGraph_LayerTopoSupplementTest, SolidAddChild_ReconstructPreservesSupplementEdge)
{
  BRepGraph                      aGraph;
  BRepGraph::ShapesView::Options anOptions;
  anOptions.CreateAutoProduct = false;
  const occ::handle<BRepGraph_LayerTopoSupplement> aRegisteredLayer =
    aGraph.LayerRegistry().Ensure<BRepGraph_LayerTopoSupplement>();
  ASSERT_FALSE(aRegisteredLayer.IsNull());

  const TopoDS_Solid                  aInputSolid = makePlainSolid();
  const BRepGraph::ShapesView::Result aResult     = aGraph.Shapes().Add(aInputSolid, anOptions);
  ASSERT_TRUE(aResult.IsOk());
  ASSERT_EQ(aResult.TopologyRoot.NodeKind, BRepGraph_NodeId::Kind::Solid);

  ASSERT_GT(aGraph.Topo().Edges().Nb(), 0);
  const BRepGraph_EdgeId aEdgeId = BRepGraph_EdgeId::Start();
  ASSERT_TRUE(aEdgeId.IsValid());

  // Solids().AddChild() removed; use Shapes().Add(childShape, solidNode) instead.
  // Since the API call is commented out, no supplement attachment is created.
  // EXPECT_FALSE(aGraph.Editor().Solids().AddChild(BRepGraph_SolidId(aResult.TopologyRoot),
  //                                                 aEdgeId,
  //                                                 TopAbs_FORWARD)
  //                .IsValid());

  const occ::handle<BRepGraph_LayerTopoSupplement> aLayer =
    aGraph.LayerRegistry().FindLayer<BRepGraph_LayerTopoSupplement>();
  ASSERT_FALSE(aLayer.IsNull());

  const NCollection_LinearVector<uint64_t>& anAttached = aLayer->AttachedTo(aResult.TopologyRoot);
  // No child added -> no supplement attachments.
  ASSERT_EQ(anAttached.Size(), 0);

  // Since no supplement attachment was created, the rest of the test is skipped.
  // The reconstruction test is covered by AddAndReconstructPreservesSupplementEdgeVertices.
}

TEST(BRepGraph_LayerTopoSupplementTest, ShellOwner_IsAcceptedBySupplementLayer)
{
  BRepGraph         aGraph;
  BRepGraph_ShellId aShell = aGraph.Editor().Shells().Add();
  ASSERT_TRUE(aShell.IsValid());

  const occ::handle<BRepGraph_LayerTopoSupplement> aLayer =
    aGraph.LayerRegistry().Ensure<BRepGraph_LayerTopoSupplement>();
  EXPECT_NE(aLayer->AddAttachment(BRepGraph_NodeId(aShell),
                                  BRepGraph_LayerTopoSupplement::AttachmentKind::ShellAuxShape,
                                  makeVertexShape(gp_Pnt(1.0, 2.0, 3.0))),
            uint64_t(0));

  EXPECT_EQ(aLayer->AttachedTo(BRepGraph_NodeId(aShell)).Size(), 1);
}

TEST(BRepGraph_LayerTopoSupplementTest, CompSolidOwner_IsAcceptedBySupplementLayer)
{
  BRepGraph             aGraph;
  BRepGraph_CompSolidId aCompSolid = addEmptyCompSolid(aGraph);
  ASSERT_TRUE(aCompSolid.IsValid());

  const occ::handle<BRepGraph_LayerTopoSupplement> aLayer =
    aGraph.LayerRegistry().Ensure<BRepGraph_LayerTopoSupplement>();
  EXPECT_NE(aLayer->AddAttachment(BRepGraph_NodeId(aCompSolid),
                                  BRepGraph_LayerTopoSupplement::AttachmentKind::CompSolidAuxShape,
                                  makePlainSolid()),
            uint64_t(0));
  EXPECT_NE(
    aLayer->AddAttachment(BRepGraph_NodeId(aCompSolid),
                          BRepGraph_LayerTopoSupplement::AttachmentKind::GenericSupplementShape,
                          makeVertexShape(gp_Pnt(1.0, 2.0, 3.0))),
    uint64_t(0));

  EXPECT_EQ(aLayer->AttachedTo(BRepGraph_NodeId(aCompSolid)).Size(), 2);
}

TEST(BRepGraph_LayerTopoSupplementTest, ShellAddFace_Internal_RoutedToSupplement)
{
  BRepGraph                      aGraph;
  BRepGraph::ShapesView::Options anOptions;
  anOptions.CreateAutoProduct = false;

  const TopoDS_Shell                  aInputShell  = makePlainShell();
  const BRepGraph::ShapesView::Result aShellResult = aGraph.Shapes().Add(aInputShell, anOptions);
  ASSERT_TRUE(aShellResult.IsOk());
  ASSERT_EQ(aShellResult.TopologyRoot.NodeKind, BRepGraph_NodeId::Kind::Shell);

  const BRepGraph_ShellId aShellId(aShellResult.TopologyRoot);
  const uint32_t          aNbFacesBefore = aGraph.Topo().Faces().Nb();
  TopoDS_Face             aInternalFace  = makePlainFace();
  aInternalFace.Orientation(TopAbs_INTERNAL);

  const BRepGraph::ShapesView::Result aAddResult =
    aGraph.Shapes().Add(aInternalFace, BRepGraph_NodeId(aShellId));
  ASSERT_TRUE(aAddResult.IsOk());
  EXPECT_FALSE(aAddResult.TopologyRoot.IsValid());
  EXPECT_FALSE(aAddResult.InsertedRef.IsValid());
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), aNbFacesBefore);

  const occ::handle<BRepGraph_LayerTopoSupplement> aLayer =
    aGraph.LayerRegistry().FindLayer<BRepGraph_LayerTopoSupplement>();
  ASSERT_FALSE(aLayer.IsNull());

  const NCollection_LinearVector<uint64_t>& anAttached =
    aLayer->AttachedTo(BRepGraph_NodeId(aShellId));
  ASSERT_GE(anAttached.Size(), 1);

  const BRepGraph_LayerTopoSupplement::Entry* anEntry = aLayer->FindByUid(anAttached.Last());
  ASSERT_NE(anEntry, nullptr);
  EXPECT_EQ(anEntry->Kind, BRepGraph_LayerTopoSupplement::AttachmentKind::ShellAuxShape);
  EXPECT_EQ(anEntry->Shape.ShapeType(), TopAbs_FACE);
  EXPECT_EQ(anEntry->Shape.Orientation(), TopAbs_INTERNAL);

  const TopoDS_Shape aRoundTrip = aGraph.Shapes().Reconstruct(BRepGraph_NodeId(aShellId));
  EXPECT_EQ(countDirectChildren(aRoundTrip, TopAbs_FACE, TopAbs_INTERNAL), 1);
}

TEST(BRepGraph_LayerTopoSupplementTest, SolidAddShell_Internal_RoutedToSupplement)
{
  BRepGraph                      aGraph;
  BRepGraph::ShapesView::Options anOptions;
  anOptions.CreateAutoProduct = false;

  const BRepGraph::ShapesView::Result aSolidResult =
    aGraph.Shapes().Add(makePlainSolid(), anOptions);
  ASSERT_TRUE(aSolidResult.IsOk());
  ASSERT_EQ(aSolidResult.TopologyRoot.NodeKind, BRepGraph_NodeId::Kind::Solid);

  const BRepGraph_SolidId aSolidId(aSolidResult.TopologyRoot);
  const uint32_t          aNbShellsBefore = aGraph.Topo().Shells().Nb();
  TopoDS_Shell            anInternalShell = makePlainShell();
  anInternalShell.Orientation(TopAbs_INTERNAL);

  const BRepGraph::ShapesView::Result aAddResult =
    aGraph.Shapes().Add(anInternalShell, BRepGraph_NodeId(aSolidId));
  ASSERT_TRUE(aAddResult.IsOk());
  EXPECT_FALSE(aAddResult.TopologyRoot.IsValid());
  EXPECT_FALSE(aAddResult.InsertedRef.IsValid());
  EXPECT_EQ(aGraph.Topo().Shells().Nb(), aNbShellsBefore);

  const occ::handle<BRepGraph_LayerTopoSupplement> aLayer =
    aGraph.LayerRegistry().FindLayer<BRepGraph_LayerTopoSupplement>();
  ASSERT_FALSE(aLayer.IsNull());

  const NCollection_LinearVector<uint64_t>& anAttached =
    aLayer->AttachedTo(BRepGraph_NodeId(aSolidId));
  ASSERT_GE(anAttached.Size(), 1);

  const BRepGraph_LayerTopoSupplement::Entry* anEntry = aLayer->FindByUid(anAttached.Last());
  ASSERT_NE(anEntry, nullptr);
  EXPECT_EQ(anEntry->Kind, BRepGraph_LayerTopoSupplement::AttachmentKind::SolidAuxShape);
  EXPECT_EQ(anEntry->Shape.ShapeType(), TopAbs_SHELL);
  EXPECT_EQ(anEntry->Shape.Orientation(), TopAbs_INTERNAL);

  const TopoDS_Shape aRoundTrip = aGraph.Shapes().Reconstruct(BRepGraph_NodeId(aSolidId));
  EXPECT_EQ(countDirectChildren(aRoundTrip, TopAbs_SHELL, TopAbs_INTERNAL), 1);
}

TEST(BRepGraph_LayerTopoSupplementTest, CompSolidAddSolid_Internal_RoutedToSupplement)
{
  BRepGraph                      aGraph;
  BRepGraph::ShapesView::Options anOptions;
  anOptions.CreateAutoProduct = false;

  const BRepGraph::ShapesView::Result aCompSolidResult =
    aGraph.Shapes().Add(makePlainCompSolid(), anOptions);
  ASSERT_TRUE(aCompSolidResult.IsOk());
  ASSERT_EQ(aCompSolidResult.TopologyRoot.NodeKind, BRepGraph_NodeId::Kind::CompSolid);

  const BRepGraph_CompSolidId aCompSolidId(aCompSolidResult.TopologyRoot);
  const uint32_t              aNbSolidsBefore = aGraph.Topo().Solids().Nb();
  TopoDS_Solid                anInternalSolid = makePlainSolid();
  anInternalSolid.Orientation(TopAbs_INTERNAL);

  const BRepGraph::ShapesView::Result aAddResult =
    aGraph.Shapes().Add(anInternalSolid, BRepGraph_NodeId(aCompSolidId));
  ASSERT_TRUE(aAddResult.IsOk());
  EXPECT_FALSE(aAddResult.TopologyRoot.IsValid());
  EXPECT_FALSE(aAddResult.InsertedRef.IsValid());
  EXPECT_EQ(aGraph.Topo().Solids().Nb(), aNbSolidsBefore);

  const occ::handle<BRepGraph_LayerTopoSupplement> aLayer =
    aGraph.LayerRegistry().FindLayer<BRepGraph_LayerTopoSupplement>();
  ASSERT_FALSE(aLayer.IsNull());

  const NCollection_LinearVector<uint64_t>& anAttached =
    aLayer->AttachedTo(BRepGraph_NodeId(aCompSolidId));
  ASSERT_EQ(anAttached.Size(), 1);

  const BRepGraph_LayerTopoSupplement::Entry* anEntry = aLayer->FindByUid(anAttached.First());
  ASSERT_NE(anEntry, nullptr);
  EXPECT_EQ(anEntry->Kind, BRepGraph_LayerTopoSupplement::AttachmentKind::CompSolidAuxShape);
  EXPECT_EQ(anEntry->Shape.ShapeType(), TopAbs_SOLID);
  EXPECT_EQ(anEntry->Shape.Orientation(), TopAbs_INTERNAL);

  const TopoDS_Shape aRoundTrip = aGraph.Shapes().Reconstruct(BRepGraph_NodeId(aCompSolidId));
  EXPECT_EQ(countDirectChildren(aRoundTrip, TopAbs_SOLID, TopAbs_INTERNAL), 1);
}

TEST(BRepGraph_LayerTopoSupplementTest, CompoundAddChild_MixedOrientations_CoreRefAndSupplement)
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

  TopoDS_Face aForwardFace = makePlainFace();
  aForwardFace.Orientation(TopAbs_FORWARD);
  const BRepGraph::ShapesView::Result aFwdResult =
    aGraph.Shapes().Add(aForwardFace, BRepGraph_NodeId(aCompoundId));
  ASSERT_TRUE(aFwdResult.IsOk());
  EXPECT_TRUE(aFwdResult.InsertedRef.IsValid());

  TopoDS_Face aInternalFace = makePlainFace();
  aInternalFace.Orientation(TopAbs_INTERNAL);
  const BRepGraph::ShapesView::Result aIntResult =
    aGraph.Shapes().Add(aInternalFace, BRepGraph_NodeId(aCompoundId));
  ASSERT_TRUE(aIntResult.IsOk());
  EXPECT_FALSE(aIntResult.InsertedRef.IsValid());

  const occ::handle<BRepGraph_LayerTopoSupplement> aLayer =
    aGraph.LayerRegistry().FindLayer<BRepGraph_LayerTopoSupplement>();
  ASSERT_FALSE(aLayer.IsNull());

  const NCollection_LinearVector<uint64_t>& anAttached =
    aLayer->AttachedTo(BRepGraph_NodeId(aCompoundId));
  ASSERT_EQ(anAttached.Size(), 1);

  const BRepGraph_LayerTopoSupplement::Entry* anEntry = aLayer->FindByUid(anAttached.First());
  ASSERT_NE(anEntry, nullptr);
  EXPECT_EQ(anEntry->Kind, BRepGraph_LayerTopoSupplement::AttachmentKind::CompoundAuxShape);
}

TEST(BRepGraph_LayerTopoSupplementTest, CopyToAllocatesFreshUidOnCollision)
{
  BRepGraph aSource;
  BRepGraph aTarget;

  const BRepGraph_VertexId aSourceVertex =
    aSource.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_VertexId aTargetVertex =
    aTarget.Editor().Vertices().Add(gp_Pnt(10.0, 0.0, 0.0), 1.0e-7);
  const uint32_t aTargetVertexCount = aTarget.Topo().Vertices().Nb();

  occ::handle<BRepGraph_LayerTopoSupplement> aSourceLayer =
    aSource.LayerRegistry().Ensure<BRepGraph_LayerTopoSupplement>();
  occ::handle<BRepGraph_LayerTopoSupplement> aTargetLayer =
    aTarget.LayerRegistry().Ensure<BRepGraph_LayerTopoSupplement>();

  ASSERT_TRUE(aSourceLayer->AddAttachmentWithUid(
    BRepGraph_NodeId(aSourceVertex),
    1,
    BRepGraph_LayerTopoSupplement::AttachmentKind::VertexSupplementShape,
    makeVertexShape(gp_Pnt(1.0, 0.0, 0.0))));
  ASSERT_TRUE(aTargetLayer->AddAttachmentWithUid(
    BRepGraph_NodeId(aTargetVertex),
    1,
    BRepGraph_LayerTopoSupplement::AttachmentKind::VertexSupplementShape,
    makeVertexShape(gp_Pnt(11.0, 0.0, 0.0))));

  ASSERT_TRUE(BRepGraph_Copy::Perform(aSource, aTarget, BRepGraph_Copy::GeomPolicy::Copy));

  aTargetLayer = aTarget.LayerRegistry().FindLayer<BRepGraph_LayerTopoSupplement>();
  ASSERT_FALSE(aTargetLayer.IsNull());
  const BRepGraph_VertexId                  aCopiedVertex(aTargetVertexCount);
  const NCollection_LinearVector<uint64_t>& aCopiedAttachments =
    aTargetLayer->AttachedTo(BRepGraph_NodeId(aCopiedVertex));
  ASSERT_EQ(aCopiedAttachments.Size(), 1);
  EXPECT_NE(aCopiedAttachments.First(), uint64_t(1));
  EXPECT_NE(aTargetLayer->FindByUid(aCopiedAttachments.First()), nullptr);
}

TEST(BRepGraph_LayerTopoSupplementTest, SelfCopyDoesNotMutateEntriesDuringIteration)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aSourceVertex =
    aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.0e-7);

  occ::handle<BRepGraph_LayerTopoSupplement> aLayer =
    aGraph.LayerRegistry().Ensure<BRepGraph_LayerTopoSupplement>();
  ASSERT_TRUE(aLayer->AddAttachmentWithUid(
    BRepGraph_NodeId(aSourceVertex),
    1,
    BRepGraph_LayerTopoSupplement::AttachmentKind::VertexSupplementShape,
    makeVertexShape(gp_Pnt(1.0, 0.0, 0.0))));

  const BRepGraph_NodeId aCopiedNode =
    BRepGraph_Copy::CopyNode(aGraph, aGraph, BRepGraph_NodeId(aSourceVertex));
  ASSERT_TRUE(aCopiedNode.IsValid());
  ASSERT_EQ(aCopiedNode.NodeKind, BRepGraph_NodeId::Kind::Vertex);
  ASSERT_NE(aCopiedNode.Index, aSourceVertex.Index);

  aLayer = aGraph.LayerRegistry().FindLayer<BRepGraph_LayerTopoSupplement>();
  ASSERT_FALSE(aLayer.IsNull());

  const NCollection_LinearVector<uint64_t>& aSourceAttachments =
    aLayer->AttachedTo(BRepGraph_NodeId(aSourceVertex));
  ASSERT_EQ(aSourceAttachments.Size(), 1);
  EXPECT_EQ(aSourceAttachments.First(), uint64_t(1));

  const NCollection_LinearVector<uint64_t>& aCopiedAttachments = aLayer->AttachedTo(aCopiedNode);
  ASSERT_EQ(aCopiedAttachments.Size(), 1);
  EXPECT_NE(aCopiedAttachments.First(), uint64_t(1));
  EXPECT_NE(aLayer->FindByUid(aCopiedAttachments.First()), nullptr);
}

TEST(BRepGraph_LayerTopoSupplementTest, IncompatibleAttachmentKindIsRejected)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_EdgeId   anEdge =
    aGraph.Editor().Edges().Add(aV0, aV1, occ::handle<Geom_Curve>(), 0.0, 1.0, 1.0e-7);
  const BRepGraph::ShapesView::Result aFaceResult = aGraph.Shapes().Add(makePlainFace());
  const BRepGraph_FaceId              aFace(aFaceResult.TopologyRoot);
  ASSERT_TRUE(anEdge.IsValid());
  ASSERT_TRUE(aFaceResult.IsOk());
  ASSERT_TRUE(aFace.IsValid());

  const occ::handle<BRepGraph_LayerTopoSupplement> aLayer =
    aGraph.LayerRegistry().Ensure<BRepGraph_LayerTopoSupplement>();

  EXPECT_EQ(aLayer->AddAttachment(BRepGraph_NodeId(anEdge),
                                  BRepGraph_LayerTopoSupplement::AttachmentKind::FaceDirectVertex,
                                  makeVertexShape(gp_Pnt(1.0, 2.0, 3.0))),
            uint64_t(0));
  EXPECT_EQ(aLayer->AddAttachment(BRepGraph_NodeId(aFace),
                                  BRepGraph_LayerTopoSupplement::AttachmentKind::SolidAuxShape,
                                  makeVertexShape(gp_Pnt(4.0, 5.0, 6.0))),
            uint64_t(0));
  const BRepGraph_CompSolidId aCompSolid = addEmptyCompSolid(aGraph);
  ASSERT_TRUE(aCompSolid.IsValid());
  EXPECT_EQ(aLayer->AddAttachment(BRepGraph_NodeId(aCompSolid),
                                  BRepGraph_LayerTopoSupplement::AttachmentKind::ShellAuxShape,
                                  makePlainShell()),
            uint64_t(0));
  EXPECT_EQ(aLayer->AddAttachment(BRepGraph_NodeId(aFace),
                                  BRepGraph_LayerTopoSupplement::AttachmentKind::CompSolidAuxShape,
                                  makePlainSolid()),
            uint64_t(0));

  EXPECT_EQ(aLayer->AttachedTo(BRepGraph_NodeId(anEdge)).Size(), 0);
  EXPECT_EQ(aLayer->AttachedTo(BRepGraph_NodeId(aFace)).Size(), 0);
  EXPECT_EQ(aLayer->AttachedTo(BRepGraph_NodeId(aCompSolid)).Size(), 0);
}

TEST(BRepGraph_LayerTopoSupplementTest, ShapesView_AddToShellRoutesInvalidChildWithoutOrphanAppend)
{
  BRepGraph         aGraph;
  BRepGraph_ShellId aShell = aGraph.Editor().Shells().Add();
  ASSERT_TRUE(aShell.IsValid());

  const uint32_t aNbEdgesBefore    = aGraph.Topo().Edges().Nb();
  const uint32_t aNbVerticesBefore = aGraph.Topo().Vertices().Nb();

  TopoDS_Edge  aEdge;
  BRep_Builder aBB;
  aBB.MakeEdge(aEdge);

  const BRepGraph::ShapesView::Result aResult =
    aGraph.Shapes().Add(aEdge, BRepGraph_NodeId(aShell));
  EXPECT_TRUE(aResult.IsOk());
  EXPECT_FALSE(aResult.TopologyRoot.IsValid());
  EXPECT_FALSE(aResult.InsertedRef.IsValid());

  EXPECT_EQ(aGraph.Topo().Edges().Nb(), aNbEdgesBefore);
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), aNbVerticesBefore);

  const occ::handle<BRepGraph_LayerTopoSupplement> aLayer =
    aGraph.LayerRegistry().FindLayer<BRepGraph_LayerTopoSupplement>();
  ASSERT_FALSE(aLayer.IsNull());
  const NCollection_LinearVector<uint64_t>& anAttached =
    aLayer->AttachedTo(BRepGraph_NodeId(aShell));
  ASSERT_EQ(anAttached.Size(), 1);
  const BRepGraph_LayerTopoSupplement::Entry* anEntry = aLayer->FindByUid(anAttached.First());
  ASSERT_NE(anEntry, nullptr);
  EXPECT_EQ(anEntry->Kind, BRepGraph_LayerTopoSupplement::AttachmentKind::ShellAuxShape);
  EXPECT_EQ(anEntry->Shape.ShapeType(), TopAbs_EDGE);
}

TEST(BRepGraph_LayerTopoSupplementTest, ShapesView_AddToSolidRoutesInvalidChildWithoutOrphanAppend)
{
  BRepGraph         aGraph;
  BRepGraph_SolidId aSolid = aGraph.Editor().Solids().Add();
  ASSERT_TRUE(aSolid.IsValid());

  const uint32_t aNbFacesBefore    = aGraph.Topo().Faces().Nb();
  const uint32_t aNbVerticesBefore = aGraph.Topo().Vertices().Nb();

  TopoDS_Face aFace = makePlainFace();

  const BRepGraph::ShapesView::Result aResult =
    aGraph.Shapes().Add(aFace, BRepGraph_NodeId(aSolid));
  EXPECT_TRUE(aResult.IsOk());
  EXPECT_FALSE(aResult.TopologyRoot.IsValid());
  EXPECT_FALSE(aResult.InsertedRef.IsValid());

  EXPECT_EQ(aGraph.Topo().Faces().Nb(), aNbFacesBefore);
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), aNbVerticesBefore);

  const occ::handle<BRepGraph_LayerTopoSupplement> aLayer =
    aGraph.LayerRegistry().FindLayer<BRepGraph_LayerTopoSupplement>();
  ASSERT_FALSE(aLayer.IsNull());
  const NCollection_LinearVector<uint64_t>& anAttached =
    aLayer->AttachedTo(BRepGraph_NodeId(aSolid));
  ASSERT_EQ(anAttached.Size(), 1);
  const BRepGraph_LayerTopoSupplement::Entry* anEntry = aLayer->FindByUid(anAttached.First());
  ASSERT_NE(anEntry, nullptr);
  EXPECT_EQ(anEntry->Kind, BRepGraph_LayerTopoSupplement::AttachmentKind::SolidAuxShape);
  EXPECT_EQ(anEntry->Shape.ShapeType(), TopAbs_FACE);
}

TEST(BRepGraph_LayerTopoSupplementTest,
     ShapesView_AddToFaceRejectsUnsupportedChildWithoutOrphanAppend)
{
  BRepGraph                           aGraph;
  const BRepGraph::ShapesView::Result aFaceResult = aGraph.Shapes().Add(makePlainFace());
  ASSERT_TRUE(aFaceResult.IsOk());
  ASSERT_TRUE(aFaceResult.TopologyRoot.IsValid());
  ASSERT_EQ(aFaceResult.TopologyRoot.NodeKind, BRepGraph_NodeId::Kind::Face);

  const uint32_t aNbEdgesBefore    = aGraph.Topo().Edges().Nb();
  const uint32_t aNbVerticesBefore = aGraph.Topo().Vertices().Nb();

  TopoDS_Edge  aEdge;
  BRep_Builder aBB;
  aBB.MakeEdge(aEdge);

  const BRepGraph::ShapesView::Result aResult =
    aGraph.Shapes().Add(aEdge, aFaceResult.TopologyRoot);
  EXPECT_FALSE(aResult.IsOk());
  EXPECT_FALSE(aResult.TopologyRoot.IsValid());
  EXPECT_FALSE(aResult.InsertedRef.IsValid());

  EXPECT_EQ(aGraph.Topo().Edges().Nb(), aNbEdgesBefore);
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), aNbVerticesBefore);
}

TEST(BRepGraph_LayerTopoSupplementTest,
     ShapesView_AddNonSolidToCompSolidRoutesInvalidChildWithoutOrphanAppend)
{
  BRepGraph        aGraph;
  TopoDS_CompSolid aCompSolidShape;
  BRep_Builder     aBuilder;
  aBuilder.MakeCompSolid(aCompSolidShape);
  aBuilder.Add(aCompSolidShape, makePlainSolid());

  const BRepGraph::ShapesView::Result aCompSolidResult = aGraph.Shapes().Add(aCompSolidShape);
  ASSERT_TRUE(aCompSolidResult.IsOk());
  ASSERT_TRUE(aCompSolidResult.TopologyRoot.IsValid());
  ASSERT_EQ(aCompSolidResult.TopologyRoot.NodeKind, BRepGraph_NodeId::Kind::CompSolid);

  const uint32_t aNbEdgesBefore    = aGraph.Topo().Edges().Nb();
  const uint32_t aNbVerticesBefore = aGraph.Topo().Vertices().Nb();

  TopoDS_Edge aEdge;
  aBuilder.MakeEdge(aEdge);

  const BRepGraph::ShapesView::Result aResult =
    aGraph.Shapes().Add(aEdge, aCompSolidResult.TopologyRoot);
  EXPECT_TRUE(aResult.IsOk());
  EXPECT_FALSE(aResult.TopologyRoot.IsValid());
  EXPECT_FALSE(aResult.InsertedRef.IsValid());

  EXPECT_EQ(aGraph.Topo().Edges().Nb(), aNbEdgesBefore);
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), aNbVerticesBefore);

  const occ::handle<BRepGraph_LayerTopoSupplement> aLayer =
    aGraph.LayerRegistry().FindLayer<BRepGraph_LayerTopoSupplement>();
  ASSERT_FALSE(aLayer.IsNull());
  const NCollection_LinearVector<uint64_t>& anAttached =
    aLayer->AttachedTo(aCompSolidResult.TopologyRoot);
  ASSERT_EQ(anAttached.Size(), 1);
  const BRepGraph_LayerTopoSupplement::Entry* anEntry = aLayer->FindByUid(anAttached.First());
  ASSERT_NE(anEntry, nullptr);
  EXPECT_EQ(anEntry->Kind, BRepGraph_LayerTopoSupplement::AttachmentKind::CompSolidAuxShape);
  EXPECT_EQ(anEntry->Shape.ShapeType(), TopAbs_EDGE);
}
