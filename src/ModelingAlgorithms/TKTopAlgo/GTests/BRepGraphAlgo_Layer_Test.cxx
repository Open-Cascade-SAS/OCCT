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
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Layer.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraphAlgo_Compact.hxx>
#include <BRepGraphAlgo_Deduplicate.hxx>
#include <BRepGraphAlgo_Sewing.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <Geom_Line.hxx>
#include <NCollection_DataMap.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <gp_Trsf.hxx>

#include <utility>

#include <gtest/gtest.h>
#include <Standard_GUID.hxx>

namespace
{

//! Build a compound of two adjacent box faces (sharing an edge) for sewing tests.
TopoDS_Compound makeTwoAdjacentFaces()
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  TopExp_Explorer    anExp(aBox, TopAbs_FACE);
  const TopoDS_Shape aFace1 = anExp.Current();
  anExp.Next();
  const TopoDS_Shape aFace2 = anExp.Current();

  BRepBuilderAPI_Copy aCopy1(aFace1, true);
  BRepBuilderAPI_Copy aCopy2(aFace2, true);

  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  aBB.Add(aCompound, aCopy1.Shape());
  aBB.Add(aCompound, aCopy2.Shape());
  return aCompound;
}

//! Build a compound of two copies of the same face (for deduplicate tests).
TopoDS_Compound makeTwoCopiedFaces()
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  TopExp_Explorer    anExp(aBox, TopAbs_FACE);
  const TopoDS_Shape aFace = anExp.Current();

  BRepBuilderAPI_Copy aCopy1(aFace, true);
  BRepBuilderAPI_Copy aCopy2(aFace, true);

  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  aBB.Add(aCompound, aCopy1.Shape());
  aBB.Add(aCompound, aCopy2.Shape());
  return aCompound;
}

//! Test-only NameLayer replacement for BRepGraphAlgo layer integration tests.
class BRepGraphAlgo_NameLayer : public BRepGraph_Layer
{
public:
  [[nodiscard]] static const Standard_GUID& GetID()
  {
    static const Standard_GUID THE_LAYER_ID("2f9b6a5c-1f2d-4a88-9c1c-7a0c16a10003");
    return THE_LAYER_ID;
  }

  [[nodiscard]] const Standard_GUID& ID() const override
  {
    return GetID();
  }

  [[nodiscard]] const TCollection_AsciiString& Name() const override
  {
    static const TCollection_AsciiString THE_LAYER_NAME("Name");
    return THE_LAYER_NAME;
  }

  void SetNodeName(const BRepGraph_NodeId            theNode,
                   const TCollection_ExtendedString& theName)
  {
    myNames.Bind(theNode, theName);
  }

  const TCollection_ExtendedString* FindNodeName(const BRepGraph_NodeId theNode) const
  {
    return myNames.Seek(theNode);
  }

  void RemoveNodeName(const BRepGraph_NodeId theNode)
  {
    myNames.UnBind(theNode);
  }

  int NbNames() const
  {
    return myNames.Extent();
  }

  void OnNodeRemoved(const BRepGraph_NodeId theNode,
                     const BRepGraph_NodeId theReplacement) noexcept override
  {
    if (theReplacement.IsValid())
    {
      const TCollection_ExtendedString* aName = myNames.Seek(theNode);
      if (aName != nullptr && myNames.Seek(theReplacement) == nullptr)
      {
        myNames.Bind(theReplacement, *aName);
      }
    }
    myNames.UnBind(theNode);
  }

  void OnCompact(
    const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theRemapMap) noexcept override
  {
    NCollection_DataMap<BRepGraph_NodeId, TCollection_ExtendedString> aRemapped;
    for (NCollection_DataMap<BRepGraph_NodeId, TCollection_ExtendedString>::Iterator anIter(myNames);
         anIter.More();
         anIter.Next())
    {
      const BRepGraph_NodeId* aNewId = theRemapMap.Seek(anIter.Key());
      if (aNewId != nullptr)
      {
        aRemapped.Bind(*aNewId, anIter.Value());
      }
    }
    myNames = std::move(aRemapped);
  }

  void InvalidateAll() noexcept override
  {
  }

  void Clear() noexcept override
  {
    myNames.Clear();
  }

  DEFINE_STANDARD_RTTIEXT(BRepGraphAlgo_NameLayer, BRepGraph_Layer)

private:
  NCollection_DataMap<BRepGraph_NodeId, TCollection_ExtendedString> myNames;
};

IMPLEMENT_STANDARD_RTTIEXT(BRepGraphAlgo_NameLayer, BRepGraph_Layer)

} // namespace

// ============================================================
// Sewing integration: names survive edge merging
// ============================================================

TEST(BRepGraphAlgo_LayerTest, Sewing_NameMigratesToKeptEdge)
{
  const TopoDS_Compound aCompound = makeTwoAdjacentFaces();

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  occ::handle<BRepGraphAlgo_NameLayer> aLayer = new BRepGraphAlgo_NameLayer();
  aGraph.LayerRegistry().RegisterLayer(aLayer);

  // Name all edges before sewing.
  const int aNbEdges = aGraph.Topo().NbEdges();
  for (int i = 0; i < aNbEdges; ++i)
  {
    TCollection_ExtendedString aStr("Edge_");
    aStr += i;
    aLayer->SetNodeName(BRepGraph_EdgeId(i), aStr);
  }
  EXPECT_EQ(aLayer->NbNames(), aNbEdges);

  // Sew - edges at shared boundary will be merged (removed + replaced).
  BRepGraphAlgo_Sewing::Result aResult = BRepGraphAlgo_Sewing::Perform(aGraph);
  EXPECT_TRUE(aResult.IsDone);

  // After sewing: if edges were merged, removed edges should have
  // their names migrated to kept edges. Count may decrease or stay the same
  // depending on whether the copied faces had geometrically coincident edges.
  EXPECT_LE(aLayer->NbNames(), aNbEdges);

  // Core invariant: no name should reference a removed edge.
  for (int i = 0; i < aGraph.Topo().NbEdges(); ++i)
  {
    const BRepGraphInc::EdgeDef& anEdge =
      aGraph.Topo().Edges().Definition(BRepGraph_EdgeId(i));
    if (anEdge.IsRemoved)
    {
      EXPECT_EQ(aLayer->FindNodeName(BRepGraph_EdgeId(i)), nullptr)
        << "Removed edge " << i << " still has a name in the layer";
    }
  }

  // All surviving names should reference non-removed edges.
  int aNbNamedKept = 0;
  for (int i = 0; i < aGraph.Topo().NbEdges(); ++i)
  {
    if (!aGraph.Topo().Edges().Definition(BRepGraph_EdgeId(i)).IsRemoved
        && aLayer->FindNodeName(BRepGraph_EdgeId(i)) != nullptr)
      ++aNbNamedKept;
  }
  EXPECT_EQ(aNbNamedKept, aLayer->NbNames());
}

TEST(BRepGraphAlgo_LayerTest, Sewing_FaceNamesUntouched)
{
  const TopoDS_Compound aCompound = makeTwoAdjacentFaces();

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  occ::handle<BRepGraphAlgo_NameLayer> aLayer = new BRepGraphAlgo_NameLayer();
  aGraph.LayerRegistry().RegisterLayer(aLayer);

  // Name faces - they should not be affected by sewing (only edges merge).
  const int aNbFaces = aGraph.Topo().NbFaces();
  for (int i = 0; i < aNbFaces; ++i)
  {
    TCollection_ExtendedString aStr("Face_");
    aStr += i;
    aLayer->SetNodeName(BRepGraph_FaceId(i), aStr);
  }

  (void)BRepGraphAlgo_Sewing::Perform(aGraph);

  // All face names should remain unchanged.
  for (int i = 0; i < aNbFaces; ++i)
  {
    TCollection_ExtendedString aExpected("Face_");
    aExpected += i;
    const TCollection_ExtendedString* aName = aLayer->FindNodeName(BRepGraph_FaceId(i));
    ASSERT_NE(aName, nullptr) << "Face " << i << " lost its name after sewing";
    EXPECT_TRUE(aName->IsEqual(aExpected));
  }
}

TEST(BRepGraphAlgo_LayerTest, Sewing_LayerStillRegisteredAfter)
{
  const TopoDS_Compound aCompound = makeTwoAdjacentFaces();

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  occ::handle<BRepGraphAlgo_NameLayer> aLayer = new BRepGraphAlgo_NameLayer();
  aGraph.LayerRegistry().RegisterLayer(aLayer);

  (void)BRepGraphAlgo_Sewing::Perform(aGraph);

  // Layer should still be registered after sewing.
  occ::handle<BRepGraph_Layer> aFound = aGraph.LayerRegistry().FindLayer(BRepGraphAlgo_NameLayer::GetID());
  ASSERT_FALSE(aFound.IsNull());
  EXPECT_EQ(aFound.get(), aLayer.get());
}

// ============================================================
// Deduplicate integration: names survive vertex/edge merging
// ============================================================

TEST(BRepGraphAlgo_LayerTest, Deduplicate_NameMigratesToCanonical)
{
  const TopoDS_Compound aCompound = makeTwoCopiedFaces();

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  occ::handle<BRepGraphAlgo_NameLayer> aLayer = new BRepGraphAlgo_NameLayer();
  aGraph.LayerRegistry().RegisterLayer(aLayer);

  // Name all vertices.
  const int aNbVertices = aGraph.Topo().NbVertices();
  for (int i = 0; i < aNbVertices; ++i)
  {
    TCollection_ExtendedString aStr("Vertex_");
    aStr += i;
    aLayer->SetNodeName(BRepGraph_VertexId(i), aStr);
  }

  BRepGraphAlgo_Deduplicate::Result aResult = BRepGraphAlgo_Deduplicate::Perform(aGraph);

  // Some vertices were merged - names should have migrated.
  if (aResult.NbMergedVertices > 0)
  {
    // Total named vertices should be less (removed duplicates had names cleared).
    EXPECT_LT(aLayer->NbNames(), aNbVertices);

    // No name should reference a removed vertex.
    for (int i = 0; i < aGraph.Topo().NbVertices(); ++i)
    {
      if (aGraph.Topo().Vertices().Definition(BRepGraph_VertexId(i)).IsRemoved)
      {
        EXPECT_EQ(aLayer->FindNodeName(BRepGraph_VertexId(i)), nullptr)
          << "Removed vertex " << i << " still has a name";
      }
    }
  }
}

TEST(BRepGraphAlgo_LayerTest, Deduplicate_EdgeNames)
{
  const TopoDS_Compound aCompound = makeTwoCopiedFaces();

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  occ::handle<BRepGraphAlgo_NameLayer> aLayer = new BRepGraphAlgo_NameLayer();
  aGraph.LayerRegistry().RegisterLayer(aLayer);

  const int aNbEdges = aGraph.Topo().NbEdges();
  for (int i = 0; i < aNbEdges; ++i)
  {
    TCollection_ExtendedString aStr("Edge_");
    aStr += i;
    aLayer->SetNodeName(BRepGraph_EdgeId(i), aStr);
  }

  BRepGraphAlgo_Deduplicate::Result aResult = BRepGraphAlgo_Deduplicate::Perform(aGraph);

  if (aResult.NbMergedEdges > 0)
  {
    EXPECT_LT(aLayer->NbNames(), aNbEdges);
  }

  // Layer is still registered.
  ASSERT_FALSE(aGraph.LayerRegistry().FindLayer(BRepGraphAlgo_NameLayer::GetID()).IsNull());
}

// ============================================================
// Compact integration: layers survive graph rebuild
// ============================================================

TEST(BRepGraphAlgo_LayerTest, Compact_LayerSurvivesSwap)
{
  const TopoDS_Compound aCompound = makeTwoCopiedFaces();

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  occ::handle<BRepGraphAlgo_NameLayer> aLayer = new BRepGraphAlgo_NameLayer();
  aGraph.LayerRegistry().RegisterLayer(aLayer);

  // Name some faces.
  aLayer->SetNodeName(BRepGraph_FaceId(0), "FirstFace");
  aLayer->SetNodeName(BRepGraph_FaceId(1), "SecondFace");

  // Deduplicate to create some removed nodes, then compact to rebuild indices.
  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);
  (void)BRepGraphAlgo_Compact::Perform(aGraph);

  // Layer should still be registered after compact.
  occ::handle<BRepGraph_Layer> aFound = aGraph.LayerRegistry().FindLayer(BRepGraphAlgo_NameLayer::GetID());
  ASSERT_FALSE(aFound.IsNull());

  occ::handle<BRepGraphAlgo_NameLayer> aCast =
    occ::down_cast<BRepGraphAlgo_NameLayer>(aFound);
  ASSERT_FALSE(aCast.IsNull());

  // Names should still exist (possibly remapped to new indices).
  EXPECT_GT(aCast->NbNames(), 0);
}

TEST(BRepGraphAlgo_LayerTest, Compact_RemappedNamesAccessible)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  aGraph.Build(aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  occ::handle<BRepGraphAlgo_NameLayer> aLayer = new BRepGraphAlgo_NameLayer();
  aGraph.LayerRegistry().RegisterLayer(aLayer);

  // Name all faces.
  const int aNbFaces = aGraph.Topo().NbFaces();
  for (int i = 0; i < aNbFaces; ++i)
  {
    TCollection_ExtendedString aStr("F_");
    aStr += i;
    aLayer->SetNodeName(BRepGraph_FaceId(i), aStr);
  }
  EXPECT_EQ(aLayer->NbNames(), aNbFaces);

  // Remove one face, then compact.
  aGraph.Builder().RemoveNode(BRepGraph_FaceId(0));
  (void)BRepGraphAlgo_Compact::Perform(aGraph);

  // After compact, face count decreased by 1.
  const int aNbFacesAfter = aGraph.Topo().NbFaces();
  EXPECT_EQ(aNbFacesAfter, aNbFaces - 1);

  // Names should have been remapped: (aNbFaces - 1) names survive.
  // Face 0 was removed (name dropped), faces 1..N remapped to 0..N-1.
  EXPECT_EQ(aLayer->NbNames(), aNbFaces - 1);

  // Each surviving face should have a name.
  for (int i = 0; i < aNbFacesAfter; ++i)
  {
    EXPECT_NE(aLayer->FindNodeName(BRepGraph_FaceId(i)), nullptr)
      << "Remapped face " << i << " has no name after compact";
  }
}

TEST(BRepGraphAlgo_LayerTest, Compact_MultipleEntityKindsRemapped)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  aGraph.Build(aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  occ::handle<BRepGraphAlgo_NameLayer> aLayer = new BRepGraphAlgo_NameLayer();
  aGraph.LayerRegistry().RegisterLayer(aLayer);

  // Name vertices and edges.
  aLayer->SetNodeName(BRepGraph_VertexId(0), "V0");
  aLayer->SetNodeName(BRepGraph_EdgeId(0), "E0");
  aLayer->SetNodeName(BRepGraph_FaceId(0), "F0");

  // Remove vertex 0, edge 0, face 0.
  aGraph.Builder().RemoveNode(BRepGraph_VertexId(0));
  aGraph.Builder().RemoveNode(BRepGraph_EdgeId(0));
  aGraph.Builder().RemoveNode(BRepGraph_FaceId(0));

  (void)BRepGraphAlgo_Compact::Perform(aGraph);

  // All three names should have been removed (no remapping for removed nodes).
  EXPECT_EQ(aLayer->FindNodeName(BRepGraph_VertexId(0)), nullptr);
  EXPECT_EQ(aLayer->FindNodeName(BRepGraph_EdgeId(0)), nullptr);
  EXPECT_EQ(aLayer->FindNodeName(BRepGraph_FaceId(0)), nullptr);
}

// ============================================================
// SplitEdge: original edge marked removed, sub-edges created
// ============================================================

TEST(BRepGraphAlgo_LayerTest, SplitEdge_OriginalEdgeRemoved)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  aGraph.Build(aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  occ::handle<BRepGraphAlgo_NameLayer> aLayer = new BRepGraphAlgo_NameLayer();
  aGraph.LayerRegistry().RegisterLayer(aLayer);

  // Find an edge with a 3D curve for splitting.
  int aSplitEdgeIdx = -1;
  for (int i = 0; i < aGraph.Topo().NbEdges(); ++i)
  {
    const BRepGraphInc::EdgeDef& anEdge =
      aGraph.Topo().Edges().Definition(BRepGraph_EdgeId(i));
    const BRepGraph_EdgeId       anEdgeId(i);
    if (!anEdge.IsDegenerate && anEdge.Curve3DRepId.IsValid()
        && BRepGraph_Tool::Edge::StartVertex(aGraph, anEdgeId).VertexDefId.IsValid()
        && BRepGraph_Tool::Edge::EndVertex(aGraph, anEdgeId).VertexDefId.IsValid())
    {
      aSplitEdgeIdx = i;
      break;
    }
  }
  ASSERT_GE(aSplitEdgeIdx, 0) << "No suitable edge found for split test";

  const BRepGraph_EdgeId aSplitEdgeId(aSplitEdgeIdx);
  aLayer->SetNodeName(aSplitEdgeId, "OriginalEdge");

  // Create a split vertex.
  const auto [aEdgeFirst, aEdgeLast] = BRepGraph_Tool::Edge::Range(aGraph, aSplitEdgeId);
  const double aMidParam = 0.5 * (aEdgeFirst + aEdgeLast);
  const gp_Pnt aMidPnt = BRepGraph_Tool::Edge::Curve(aGraph, aSplitEdgeId)->EvalD0(aMidParam);
  const BRepGraph_VertexId aSplitVtx = aGraph.Builder().AddVertex(aMidPnt, Precision::Confusion());

  // Split the edge.
  BRepGraph_EdgeId aSubA, aSubB;
  aGraph.Builder().SplitEdge(aSplitEdgeId, aSplitVtx, aMidParam, aSubA, aSubB);

  // SplitEdge marks original as removed directly (not via RemoveNode),
  // so the layer callback is NOT triggered. Original name stays in the layer
  // but the edge is marked IsRemoved. This is a known limitation -
  // SplitEdge doesn't know the "replacement" (it creates 2 sub-edges).
  EXPECT_TRUE(aGraph.Topo().Edges().Definition(aSplitEdgeId).IsRemoved);

  // Sub-edges should be valid.
  EXPECT_TRUE(aSubA.IsValid());
  EXPECT_TRUE(aSubB.IsValid());
}

// ============================================================
// Full pipeline: Build -> Name -> Sew -> Deduplicate -> Compact
// ============================================================

TEST(BRepGraphAlgo_LayerTest, FullPipeline_NamesTrackThroughAllStages)
{
  const TopoDS_Compound aCompound = makeTwoCopiedFaces();

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  occ::handle<BRepGraphAlgo_NameLayer> aLayer = new BRepGraphAlgo_NameLayer();
  aGraph.LayerRegistry().RegisterLayer(aLayer);

  // Name all faces.
  const int aNbFaces = aGraph.Topo().NbFaces();
  for (int i = 0; i < aNbFaces; ++i)
  {
    TCollection_ExtendedString aStr("Face_");
    aStr += i;
    aLayer->SetNodeName(BRepGraph_FaceId(i), aStr);
  }

  // Stage 1: Sew.
  (void)BRepGraphAlgo_Sewing::Perform(aGraph);
  EXPECT_FALSE(aGraph.LayerRegistry().FindLayer(BRepGraphAlgo_NameLayer::GetID()).IsNull());
  const int aNamesAfterSew = aLayer->NbNames();
  EXPECT_GT(aNamesAfterSew, 0);

  // Stage 2: Deduplicate.
  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);
  EXPECT_FALSE(aGraph.LayerRegistry().FindLayer(BRepGraphAlgo_NameLayer::GetID()).IsNull());
  const int aNamesAfterDedup = aLayer->NbNames();
  EXPECT_GT(aNamesAfterDedup, 0);

  // Stage 3: Compact.
  (void)BRepGraphAlgo_Compact::Perform(aGraph);
  EXPECT_FALSE(aGraph.LayerRegistry().FindLayer(BRepGraphAlgo_NameLayer::GetID()).IsNull());
  const int aNamesAfterCompact = aLayer->NbNames();
  EXPECT_GT(aNamesAfterCompact, 0);

  // After compact, all remaining names should reference valid (non-removed) nodes.
  const int aNbFacesAfter = aGraph.Topo().NbFaces();
  for (int i = 0; i < aNbFacesAfter; ++i)
  {
    EXPECT_FALSE(aGraph.Topo().Faces().Definition(BRepGraph_FaceId(i)).IsRemoved)
      << "Face " << i << " is removed after compact - should have been eliminated";
  }
}

// ============================================================
// Sewing with cylinder (periodic surface / seam edges)
// ============================================================

TEST(BRepGraphAlgo_LayerTest, Sewing_Cylinder_NamesPreserved)
{
  // Build two half-cylinders and sew them.
  BRepPrimAPI_MakeCylinder aCylMaker(10.0, 30.0);
  const TopoDS_Shape&      aCyl = aCylMaker.Shape();

  // Extract lateral face and copy it twice (simulates separate shells).
  TopExp_Explorer    anExp(aCyl, TopAbs_FACE);
  const TopoDS_Shape aFace = anExp.Current();

  BRepBuilderAPI_Copy aCopy1(aFace, true);
  BRepBuilderAPI_Copy aCopy2(aFace, true);

  BRep_Builder    aBB;
  TopoDS_Compound aComp;
  aBB.MakeCompound(aComp);
  aBB.Add(aComp, aCopy1.Shape());
  aBB.Add(aComp, aCopy2.Shape());

  BRepGraph aGraph;
  aGraph.Build(aComp);
  ASSERT_TRUE(aGraph.IsDone());

  occ::handle<BRepGraphAlgo_NameLayer> aLayer = new BRepGraphAlgo_NameLayer();
  aGraph.LayerRegistry().RegisterLayer(aLayer);

  // Name all faces.
  for (int i = 0; i < aGraph.Topo().NbFaces(); ++i)
  {
    TCollection_ExtendedString aStr("CylFace_");
    aStr += i;
    aLayer->SetNodeName(BRepGraph_FaceId(i), aStr);
  }

  (void)BRepGraphAlgo_Sewing::Perform(aGraph);

  // Layer should survive sewing on periodic surfaces.
  EXPECT_FALSE(aGraph.LayerRegistry().FindLayer(BRepGraphAlgo_NameLayer::GetID()).IsNull());
  EXPECT_GT(aLayer->NbNames(), 0);
}

// ============================================================
// Multiple layers through full pipeline
// ============================================================

//! A simple integer counter layer for testing multi-layer dispatch.
class BRepGraphAlgo_IntCounterLayer : public BRepGraph_Layer
{
public:
  const Standard_GUID& ID() const override
  {
    static const Standard_GUID THE_ID("2f9b6a5c-1f2d-4a88-9c1c-7a0c16a10006");
    return THE_ID;
  }

  const TCollection_AsciiString& Name() const override
  {
    static const TCollection_AsciiString THE_NAME("IntCounter");
    return THE_NAME;
  }

  void OnNodeRemoved(const BRepGraph_NodeId theNode, const BRepGraph_NodeId) noexcept override
  {
    myData.UnBind(theNode);
  }

  void OnCompact(
    const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theRemapMap) noexcept override
  {
    NCollection_DataMap<BRepGraph_NodeId, int> aRemapped;
    for (NCollection_DataMap<BRepGraph_NodeId, int>::Iterator anIter(myData); anIter.More();
         anIter.Next())
    {
      const BRepGraph_NodeId* aNewId = theRemapMap.Seek(anIter.Key());
      if (aNewId != nullptr)
        aRemapped.Bind(*aNewId, anIter.Value());
    }
    myData = std::move(aRemapped);
  }

  void InvalidateAll() noexcept override {}

  void Clear() noexcept override { myData.Clear(); }

  void Set(BRepGraph_NodeId theId, int theVal) { myData.Bind(theId, theVal); }

  const int* Get(BRepGraph_NodeId theId) const { return myData.Seek(theId); }

  int NbEntries() const { return myData.Extent(); }

  DEFINE_STANDARD_RTTIEXT(BRepGraphAlgo_IntCounterLayer, BRepGraph_Layer)

private:
  NCollection_DataMap<BRepGraph_NodeId, int> myData;
};

IMPLEMENT_STANDARD_RTTIEXT(BRepGraphAlgo_IntCounterLayer, BRepGraph_Layer)

TEST(BRepGraphAlgo_LayerTest, TwoLayers_BothSurviveFullPipeline)
{
  const TopoDS_Compound aCompound = makeTwoCopiedFaces();

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  occ::handle<BRepGraphAlgo_NameLayer>      aNameLayer = new BRepGraphAlgo_NameLayer();
  occ::handle<BRepGraphAlgo_IntCounterLayer> aIntLayer = new BRepGraphAlgo_IntCounterLayer();
  aGraph.LayerRegistry().RegisterLayer(aNameLayer);
  aGraph.LayerRegistry().RegisterLayer(aIntLayer);

  // Populate both layers.
  for (int i = 0; i < aGraph.Topo().NbFaces(); ++i)
  {
    TCollection_ExtendedString aStr("F");
    aStr += i;
    aNameLayer->SetNodeName(BRepGraph_FaceId(i), aStr);
    aIntLayer->Set(BRepGraph_FaceId(i), i * 10);
  }

  // Full pipeline.
  (void)BRepGraphAlgo_Sewing::Perform(aGraph);
  (void)BRepGraphAlgo_Deduplicate::Perform(aGraph);
  (void)BRepGraphAlgo_Compact::Perform(aGraph);

  // Both layers should survive.
  EXPECT_FALSE(aGraph.LayerRegistry().FindLayer(BRepGraphAlgo_NameLayer::GetID()).IsNull());
  EXPECT_FALSE(aGraph.LayerRegistry().FindLayer(aIntLayer->ID()).IsNull());
  EXPECT_GT(aNameLayer->NbNames(), 0);
  EXPECT_GT(aIntLayer->NbEntries(), 0);

  // All entries should reference valid (non-removed) face indices.
  const int aNbFaces = aGraph.Topo().NbFaces();
  for (int i = 0; i < aNbFaces; ++i)
  {
    EXPECT_FALSE(aGraph.Topo().Faces().Definition(BRepGraph_FaceId(i)).IsRemoved);
  }
}
