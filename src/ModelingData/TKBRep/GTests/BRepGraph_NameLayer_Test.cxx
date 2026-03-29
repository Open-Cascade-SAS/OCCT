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
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Layer.hxx>
#include <BRepGraph_NameLayer.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>

#include <gtest/gtest.h>
#include <Standard_GUID.hxx>

// ============================================================
// Registration & Lookup
// ============================================================

TEST(BRepGraph_NameLayerTest, RegisterAndFind)
{
  BRepGraph                        aGraph;
  occ::handle<BRepGraph_NameLayer> aLayer = new BRepGraph_NameLayer();
  aGraph.RegisterLayer(aLayer);

  occ::handle<BRepGraph_Layer> aFound = aGraph.FindLayer(BRepGraph_NameLayer::GetID());
  ASSERT_FALSE(aFound.IsNull());
  EXPECT_EQ(aFound->Name(), "Name");
}

TEST(BRepGraph_NameLayerTest, UnregisterLayer)
{
  BRepGraph                        aGraph;
  occ::handle<BRepGraph_NameLayer> aLayer = new BRepGraph_NameLayer();
  aGraph.RegisterLayer(aLayer);

  aGraph.UnregisterLayer(BRepGraph_NameLayer::GetID());
  occ::handle<BRepGraph_Layer> aFound = aGraph.FindLayer(BRepGraph_NameLayer::GetID());
  EXPECT_TRUE(aFound.IsNull());
}

TEST(BRepGraph_NameLayerTest, FindNonExistent_ReturnsNull)
{
  BRepGraph                    aGraph;
  const Standard_GUID          aMissing("2f9b6a5c-1f2d-4a88-9c1c-7a0c16a1ff01");
  occ::handle<BRepGraph_Layer> aFound = aGraph.FindLayer(aMissing);
  EXPECT_TRUE(aFound.IsNull());
}

TEST(BRepGraph_NameLayerTest, RegisterReplacesExisting)
{
  BRepGraph                        aGraph;
  occ::handle<BRepGraph_NameLayer> aLayer1 = new BRepGraph_NameLayer();
  occ::handle<BRepGraph_NameLayer> aLayer2 = new BRepGraph_NameLayer();
  aLayer1->SetNodeName(BRepGraph_NodeId::Face(0), "FromLayer1");

  aGraph.RegisterLayer(aLayer1);
  aGraph.RegisterLayer(aLayer2);

  occ::handle<BRepGraph_Layer> aFound = aGraph.FindLayer(BRepGraph_NameLayer::GetID());
  ASSERT_FALSE(aFound.IsNull());
  // Layer2 replaced Layer1 - Layer2 has no names.
  occ::handle<BRepGraph_NameLayer> aCast = occ::down_cast<BRepGraph_NameLayer>(aFound);
  ASSERT_FALSE(aCast.IsNull());
  EXPECT_EQ(aCast->NbNames(), 0);
}

TEST(BRepGraph_NameLayerTest, RegisterNullLayer_NoOp)
{
  BRepGraph                    aGraph;
  occ::handle<BRepGraph_Layer> aNullLayer;
  aGraph.RegisterLayer(aNullLayer);
  // Should not crash, no layer registered.
  EXPECT_TRUE(aGraph.FindLayer(BRepGraph_NameLayer::GetID()).IsNull());
}

TEST(BRepGraph_NameLayerTest, UnregisterNonExistent_NoOp)
{
  BRepGraph           aGraph;
  const Standard_GUID aMissing("2f9b6a5c-1f2d-4a88-9c1c-7a0c16a1ff02");
  aGraph.UnregisterLayer(aMissing);
  // Should not crash.
}

// ============================================================
// Basic Set / Get / Remove
// ============================================================

TEST(BRepGraph_NameLayerTest, SetAndGetName)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  aGraph.Build(aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  occ::handle<BRepGraph_NameLayer> aLayer = new BRepGraph_NameLayer();
  aGraph.RegisterLayer(aLayer);

  const BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  aLayer->SetNodeName(aFaceId, "TopFace");

  const TCollection_ExtendedString* aName = aLayer->FindNodeName(aFaceId);
  ASSERT_NE(aName, nullptr);
  EXPECT_TRUE(aName->IsEqual("TopFace"));
  EXPECT_EQ(aLayer->NbNames(), 1);
}

TEST(BRepGraph_NameLayerTest, SetOverwritesExisting)
{
  occ::handle<BRepGraph_NameLayer> aLayer = new BRepGraph_NameLayer();
  const BRepGraph_NodeId           aId    = BRepGraph_NodeId::Face(0);

  aLayer->SetNodeName(aId, "First");
  aLayer->SetNodeName(aId, "Second");

  const TCollection_ExtendedString* aName = aLayer->FindNodeName(aId);
  ASSERT_NE(aName, nullptr);
  EXPECT_TRUE(aName->IsEqual("Second"));
  EXPECT_EQ(aLayer->NbNames(), 1);
}

TEST(BRepGraph_NameLayerTest, RemoveName)
{
  occ::handle<BRepGraph_NameLayer> aLayer = new BRepGraph_NameLayer();

  const BRepGraph_NodeId aNodeId(BRepGraph_NodeId::Kind::Edge, 0);
  aLayer->SetNodeName(aNodeId, "TestEdge");
  EXPECT_EQ(aLayer->NbNames(), 1);

  aLayer->RemoveNodeName(aNodeId);
  EXPECT_EQ(aLayer->NbNames(), 0);
  EXPECT_EQ(aLayer->FindNodeName(aNodeId), nullptr);
}

TEST(BRepGraph_NameLayerTest, RemoveNonExistent_NoOp)
{
  occ::handle<BRepGraph_NameLayer> aLayer = new BRepGraph_NameLayer();
  aLayer->RemoveNodeName(BRepGraph_NodeId::Edge(99));
  EXPECT_EQ(aLayer->NbNames(), 0);
}

TEST(BRepGraph_NameLayerTest, NamesOnAllEntityKinds)
{
  occ::handle<BRepGraph_NameLayer> aLayer = new BRepGraph_NameLayer();
  aLayer->SetNodeName(BRepGraph_NodeId::Vertex(0), "V0");
  aLayer->SetNodeName(BRepGraph_NodeId::Edge(0), "E0");
  aLayer->SetNodeName(BRepGraph_NodeId::Wire(0), "W0");
  aLayer->SetNodeName(BRepGraph_NodeId::Face(0), "F0");
  aLayer->SetNodeName(BRepGraph_NodeId::Shell(0), "Sh0");
  aLayer->SetNodeName(BRepGraph_NodeId::Solid(0), "So0");
  EXPECT_EQ(aLayer->NbNames(), 6);

  EXPECT_TRUE(aLayer->FindNodeName(BRepGraph_NodeId::Vertex(0))->IsEqual("V0"));
  EXPECT_TRUE(aLayer->FindNodeName(BRepGraph_NodeId::Face(0))->IsEqual("F0"));
  EXPECT_TRUE(aLayer->FindNodeName(BRepGraph_NodeId::Solid(0))->IsEqual("So0"));
}

TEST(BRepGraph_NameLayerTest, Clear)
{
  occ::handle<BRepGraph_NameLayer> aLayer = new BRepGraph_NameLayer();
  aLayer->SetNodeName(BRepGraph_NodeId::Face(0), "Face0");
  aLayer->SetNodeName(BRepGraph_NodeId::Face(1), "Face1");
  EXPECT_EQ(aLayer->NbNames(), 2);

  aLayer->Clear();
  EXPECT_EQ(aLayer->NbNames(), 0);
}

// ============================================================
// OnNodeRemoved - Dispatch via Builder
// ============================================================

TEST(BRepGraph_NameLayerTest, OnNodeRemoved_PureDeletion)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  aGraph.Build(aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  occ::handle<BRepGraph_NameLayer> aLayer = new BRepGraph_NameLayer();
  aGraph.RegisterLayer(aLayer);

  const BRepGraph_NodeId anEdgeId(BRepGraph_NodeId::Kind::Edge, 0);
  aLayer->SetNodeName(anEdgeId, "Edge0");

  aGraph.Builder().RemoveNode(anEdgeId);

  EXPECT_EQ(aLayer->FindNodeName(anEdgeId), nullptr);
  EXPECT_EQ(aLayer->NbNames(), 0);
}

TEST(BRepGraph_NameLayerTest, OnNodeRemoved_WithReplacement)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  aGraph.Build(aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  occ::handle<BRepGraph_NameLayer> aLayer = new BRepGraph_NameLayer();
  aGraph.RegisterLayer(aLayer);

  const BRepGraph_NodeId anOldId(BRepGraph_NodeId::Kind::Edge, 0);
  const BRepGraph_NodeId aNewId(BRepGraph_NodeId::Kind::Edge, 1);
  aLayer->SetNodeName(anOldId, "OriginalEdge");

  aGraph.Builder().RemoveNode(anOldId, aNewId);

  EXPECT_EQ(aLayer->FindNodeName(anOldId), nullptr);
  const TCollection_ExtendedString* aName = aLayer->FindNodeName(aNewId);
  ASSERT_NE(aName, nullptr);
  EXPECT_TRUE(aName->IsEqual("OriginalEdge"));
}

TEST(BRepGraph_NameLayerTest, OnNodeRemoved_ReplacementAlreadyHasName)
{
  occ::handle<BRepGraph_NameLayer> aLayer = new BRepGraph_NameLayer();

  const BRepGraph_NodeId anOldId(BRepGraph_NodeId::Kind::Edge, 0);
  const BRepGraph_NodeId aNewId(BRepGraph_NodeId::Kind::Edge, 1);
  aLayer->SetNodeName(anOldId, "OldName");
  aLayer->SetNodeName(aNewId, "ExistingName");

  aLayer->OnNodeRemoved(anOldId, aNewId);

  EXPECT_EQ(aLayer->FindNodeName(anOldId), nullptr);
  const TCollection_ExtendedString* aName = aLayer->FindNodeName(aNewId);
  ASSERT_NE(aName, nullptr);
  EXPECT_TRUE(aName->IsEqual("ExistingName"));
}

TEST(BRepGraph_NameLayerTest, OnNodeRemoved_NeitherHasName)
{
  occ::handle<BRepGraph_NameLayer> aLayer = new BRepGraph_NameLayer();
  aLayer->SetNodeName(BRepGraph_NodeId::Face(5), "Unrelated");

  // Removing a node that has no name - should not crash or affect other names.
  aLayer->OnNodeRemoved(BRepGraph_NodeId::Edge(0), BRepGraph_NodeId::Edge(1));

  EXPECT_EQ(aLayer->NbNames(), 1);
  EXPECT_NE(aLayer->FindNodeName(BRepGraph_NodeId::Face(5)), nullptr);
}

TEST(BRepGraph_NameLayerTest, OnNodeRemoved_MultipleSequential)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  aGraph.Build(aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  occ::handle<BRepGraph_NameLayer> aLayer = new BRepGraph_NameLayer();
  aGraph.RegisterLayer(aLayer);

  // Name edges 0, 1, 2.
  aLayer->SetNodeName(BRepGraph_NodeId::Edge(0), "E0");
  aLayer->SetNodeName(BRepGraph_NodeId::Edge(1), "E1");
  aLayer->SetNodeName(BRepGraph_NodeId::Edge(2), "E2");
  EXPECT_EQ(aLayer->NbNames(), 3);

  // Remove edge 0 with replacement edge 1 (E0 migrates to E1, but E1 already has name).
  aGraph.Builder().RemoveNode(BRepGraph_NodeId::Edge(0), BRepGraph_NodeId::Edge(1));
  EXPECT_EQ(aLayer->NbNames(), 2);
  EXPECT_TRUE(aLayer->FindNodeName(BRepGraph_NodeId::Edge(1))->IsEqual("E1"));

  // Remove edge 2 without replacement.
  aGraph.Builder().RemoveNode(BRepGraph_NodeId::Edge(2));
  EXPECT_EQ(aLayer->NbNames(), 1);
  EXPECT_NE(aLayer->FindNodeName(BRepGraph_NodeId::Edge(1)), nullptr);
}

// ============================================================
// Multiple layers on same graph
// ============================================================

//! Second test layer for multi-layer scenarios.
class BRepGraph_TestCounterLayer : public BRepGraph_Layer
{
public:
  const Standard_GUID& ID() const override
  {
    static const Standard_GUID THE_ID("2f9b6a5c-1f2d-4a88-9c1c-7a0c16a10005");
    return THE_ID;
  }

  const TCollection_AsciiString& Name() const override { return myName; }

  void OnNodeRemoved(const BRepGraph_NodeId, const BRepGraph_NodeId) noexcept override { ++myRemoveCount; }

  void OnCompact(const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>&) noexcept override
  {
    ++myCompactCount;
  }

  void InvalidateAll() noexcept override {}

  void Clear() noexcept override {}

  int myRemoveCount  = 0;
  int myCompactCount = 0;

  DEFINE_STANDARD_RTTIEXT(BRepGraph_TestCounterLayer, BRepGraph_Layer)
private:
  TCollection_AsciiString myName = "Counter";
};

IMPLEMENT_STANDARD_RTTIEXT(BRepGraph_TestCounterLayer, BRepGraph_Layer)

TEST(BRepGraph_NameLayerTest, MultipleLayers_BothNotified)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  aGraph.Build(aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  occ::handle<BRepGraph_NameLayer>        aNameLayer    = new BRepGraph_NameLayer();
  occ::handle<BRepGraph_TestCounterLayer> aCounterLayer = new BRepGraph_TestCounterLayer();
  aGraph.RegisterLayer(aNameLayer);
  aGraph.RegisterLayer(aCounterLayer);

  aNameLayer->SetNodeName(BRepGraph_NodeId::Edge(0), "E0");

  aGraph.Builder().RemoveNode(BRepGraph_NodeId::Edge(0));

  EXPECT_EQ(aNameLayer->NbNames(), 0);
  EXPECT_EQ(aCounterLayer->myRemoveCount, 1);
}

TEST(BRepGraph_NameLayerTest, MultipleLayers_RemoveWithReplacement)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  aGraph.Build(aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  occ::handle<BRepGraph_NameLayer>        aNameLayer    = new BRepGraph_NameLayer();
  occ::handle<BRepGraph_TestCounterLayer> aCounterLayer = new BRepGraph_TestCounterLayer();
  aGraph.RegisterLayer(aNameLayer);
  aGraph.RegisterLayer(aCounterLayer);

  aNameLayer->SetNodeName(BRepGraph_NodeId::Edge(0), "MigratingName");

  aGraph.Builder().RemoveNode(BRepGraph_NodeId::Edge(0), BRepGraph_NodeId::Edge(1));

  EXPECT_EQ(aCounterLayer->myRemoveCount, 1);
  const TCollection_ExtendedString* aName = aNameLayer->FindNodeName(BRepGraph_NodeId::Edge(1));
  ASSERT_NE(aName, nullptr);
  EXPECT_TRUE(aName->IsEqual("MigratingName"));
}

// ============================================================
// OnCompact - direct callback
// ============================================================

TEST(BRepGraph_NameLayerTest, OnCompact_RemapsNodeIds)
{
  occ::handle<BRepGraph_NameLayer> aLayer = new BRepGraph_NameLayer();

  // Simulate: face 0 removed, face 1 remapped to 0, face 2 remapped to 1.
  aLayer->SetNodeName(BRepGraph_NodeId::Face(1), "FaceA");
  aLayer->SetNodeName(BRepGraph_NodeId::Face(2), "FaceB");
  aLayer->SetNodeName(BRepGraph_NodeId::Edge(3), "EdgeC");
  EXPECT_EQ(aLayer->NbNames(), 3);

  NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId> aRemapMap;
  aRemapMap.Bind(BRepGraph_NodeId::Face(1), BRepGraph_NodeId::Face(0));
  aRemapMap.Bind(BRepGraph_NodeId::Face(2), BRepGraph_NodeId::Face(1));
  aRemapMap.Bind(BRepGraph_NodeId::Edge(3), BRepGraph_NodeId::Edge(0));

  aLayer->OnCompact(aRemapMap);

  EXPECT_EQ(aLayer->NbNames(), 3);
  EXPECT_TRUE(aLayer->FindNodeName(BRepGraph_NodeId::Face(0))->IsEqual("FaceA"));
  EXPECT_TRUE(aLayer->FindNodeName(BRepGraph_NodeId::Face(1))->IsEqual("FaceB"));
  EXPECT_TRUE(aLayer->FindNodeName(BRepGraph_NodeId::Edge(0))->IsEqual("EdgeC"));

  // Old IDs should not exist.
  EXPECT_EQ(aLayer->FindNodeName(BRepGraph_NodeId::Face(2)), nullptr);
  EXPECT_EQ(aLayer->FindNodeName(BRepGraph_NodeId::Edge(3)), nullptr);
}

TEST(BRepGraph_NameLayerTest, OnCompact_RemovedNodesDropped)
{
  occ::handle<BRepGraph_NameLayer> aLayer = new BRepGraph_NameLayer();

  aLayer->SetNodeName(BRepGraph_NodeId::Face(0), "RemovedFace");
  aLayer->SetNodeName(BRepGraph_NodeId::Face(1), "SurvivingFace");
  EXPECT_EQ(aLayer->NbNames(), 2);

  NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId> aRemapMap;
  // Face 0 not in map = removed during compact. Face 1 remapped to 0.
  aRemapMap.Bind(BRepGraph_NodeId::Face(1), BRepGraph_NodeId::Face(0));

  aLayer->OnCompact(aRemapMap);

  EXPECT_EQ(aLayer->NbNames(), 1);
  EXPECT_TRUE(aLayer->FindNodeName(BRepGraph_NodeId::Face(0))->IsEqual("SurvivingFace"));
}

TEST(BRepGraph_NameLayerTest, OnCompact_EmptyLayer)
{
  occ::handle<BRepGraph_NameLayer> aLayer = new BRepGraph_NameLayer();

  NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId> aRemapMap;
  aRemapMap.Bind(BRepGraph_NodeId::Face(0), BRepGraph_NodeId::Face(0));

  aLayer->OnCompact(aRemapMap);
  EXPECT_EQ(aLayer->NbNames(), 0);
}

TEST(BRepGraph_NameLayerTest, OnCompact_MixedEntityKinds)
{
  occ::handle<BRepGraph_NameLayer> aLayer = new BRepGraph_NameLayer();

  aLayer->SetNodeName(BRepGraph_NodeId::Vertex(5), "V5");
  aLayer->SetNodeName(BRepGraph_NodeId::Edge(3), "E3");
  aLayer->SetNodeName(BRepGraph_NodeId::Wire(2), "W2");
  aLayer->SetNodeName(BRepGraph_NodeId::Face(1), "F1");
  aLayer->SetNodeName(BRepGraph_NodeId::Shell(0), "Sh0");
  aLayer->SetNodeName(BRepGraph_NodeId::Solid(0), "So0");

  NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId> aRemapMap;
  aRemapMap.Bind(BRepGraph_NodeId::Vertex(5), BRepGraph_NodeId::Vertex(2));
  aRemapMap.Bind(BRepGraph_NodeId::Edge(3), BRepGraph_NodeId::Edge(1));
  aRemapMap.Bind(BRepGraph_NodeId::Wire(2), BRepGraph_NodeId::Wire(0));
  aRemapMap.Bind(BRepGraph_NodeId::Face(1), BRepGraph_NodeId::Face(0));
  aRemapMap.Bind(BRepGraph_NodeId::Shell(0), BRepGraph_NodeId::Shell(0));
  aRemapMap.Bind(BRepGraph_NodeId::Solid(0), BRepGraph_NodeId::Solid(0));

  aLayer->OnCompact(aRemapMap);

  EXPECT_EQ(aLayer->NbNames(), 6);
  EXPECT_TRUE(aLayer->FindNodeName(BRepGraph_NodeId::Vertex(2))->IsEqual("V5"));
  EXPECT_TRUE(aLayer->FindNodeName(BRepGraph_NodeId::Edge(1))->IsEqual("E3"));
  EXPECT_TRUE(aLayer->FindNodeName(BRepGraph_NodeId::Wire(0))->IsEqual("W2"));
  EXPECT_TRUE(aLayer->FindNodeName(BRepGraph_NodeId::Face(0))->IsEqual("F1"));
  EXPECT_TRUE(aLayer->FindNodeName(BRepGraph_NodeId::Shell(0))->IsEqual("Sh0"));
  EXPECT_TRUE(aLayer->FindNodeName(BRepGraph_NodeId::Solid(0))->IsEqual("So0"));
}

TEST(BRepGraph_NameLayerTest, OnCompact_CompoundNodesRemapped)
{
  occ::handle<BRepGraph_NameLayer> aLayer = new BRepGraph_NameLayer();

  aLayer->SetNodeName(BRepGraph_NodeId::Compound(2), "MyCompound");
  aLayer->SetNodeName(BRepGraph_NodeId::CompSolid(1), "MyCompSolid");
  EXPECT_EQ(aLayer->NbNames(), 2);

  NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId> aRemapMap;
  aRemapMap.Bind(BRepGraph_NodeId::Compound(2), BRepGraph_NodeId::Compound(0));
  aRemapMap.Bind(BRepGraph_NodeId::CompSolid(1), BRepGraph_NodeId::CompSolid(0));

  aLayer->OnCompact(aRemapMap);

  EXPECT_EQ(aLayer->NbNames(), 2);
  EXPECT_TRUE(aLayer->FindNodeName(BRepGraph_NodeId::Compound(0))->IsEqual("MyCompound"));
  EXPECT_TRUE(aLayer->FindNodeName(BRepGraph_NodeId::CompSolid(0))->IsEqual("MyCompSolid"));
  // Old IDs dropped.
  EXPECT_EQ(aLayer->FindNodeName(BRepGraph_NodeId::Compound(2)), nullptr);
  EXPECT_EQ(aLayer->FindNodeName(BRepGraph_NodeId::CompSolid(1)), nullptr);
}

// ============================================================
// InvalidateAll - no-op for value-based layer
// ============================================================

TEST(BRepGraph_NameLayerTest, InvalidateAll_PreservesData)
{
  occ::handle<BRepGraph_NameLayer> aLayer = new BRepGraph_NameLayer();
  aLayer->SetNodeName(BRepGraph_NodeId::Face(0), "Persistent");

  aLayer->InvalidateAll();

  EXPECT_EQ(aLayer->NbNames(), 1);
  EXPECT_TRUE(aLayer->FindNodeName(BRepGraph_NodeId::Face(0))->IsEqual("Persistent"));
}

// ============================================================
// Layer survives graph move (simulating Compact swap)
// ============================================================

TEST(BRepGraph_NameLayerTest, LayerSurvivesGraphMove)
{
  BRepGraph                        aGraph;
  occ::handle<BRepGraph_NameLayer> aLayer = new BRepGraph_NameLayer();
  aGraph.RegisterLayer(aLayer);
  aLayer->SetNodeName(BRepGraph_NodeId::Face(0), "BeforeMove");

  // Move graph into a new variable.
  BRepGraph aGraph2 = std::move(aGraph);

  // Layer should be on the new graph.
  occ::handle<BRepGraph_Layer> aFound = aGraph2.FindLayer(BRepGraph_NameLayer::GetID());
  ASSERT_FALSE(aFound.IsNull());
  occ::handle<BRepGraph_NameLayer> aCast = occ::down_cast<BRepGraph_NameLayer>(aFound);
  ASSERT_FALSE(aCast.IsNull());
  EXPECT_TRUE(aCast->FindNodeName(BRepGraph_NodeId::Face(0))->IsEqual("BeforeMove"));
}

// ============================================================
// Edge cases
// ============================================================

TEST(BRepGraph_NameLayerTest, RemoveSubgraph_DispatchesForEachNode)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  BRepGraph           aGraph;
  aGraph.Build(aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  occ::handle<BRepGraph_TestCounterLayer> aCounterLayer = new BRepGraph_TestCounterLayer();
  aGraph.RegisterLayer(aCounterLayer);

  // RemoveSubgraph removes a node and all descendants.
  // Each removed node should trigger a layer callback.
  const BRepGraph_NodeId aFaceId = BRepGraph_NodeId::Face(0);
  aGraph.Builder().RemoveSubgraph(aFaceId);

  // Face + its wire(s) + edges + vertices - at least 4 removals.
  EXPECT_GE(aCounterLayer->myRemoveCount, 4);
}

TEST(BRepGraph_NameLayerTest, NameUnicodeString)
{
  occ::handle<BRepGraph_NameLayer> aLayer = new BRepGraph_NameLayer();
  TCollection_ExtendedString       aUnicode;
  aUnicode += (char16_t)0x00C9; // E-acute
  aUnicode += (char16_t)0x00E7; // c-cedilla

  aLayer->SetNodeName(BRepGraph_NodeId::Face(0), aUnicode);

  const TCollection_ExtendedString* aName = aLayer->FindNodeName(BRepGraph_NodeId::Face(0));
  ASSERT_NE(aName, nullptr);
  EXPECT_TRUE(aName->IsEqual(aUnicode));
}

TEST(BRepGraph_NameLayerTest, ManyNodesStress)
{
  occ::handle<BRepGraph_NameLayer> aLayer = new BRepGraph_NameLayer();

  // Name 1000 nodes.
  for (int i = 0; i < 1000; ++i)
  {
    TCollection_ExtendedString aStr("Node_");
    aStr += i;
    aLayer->SetNodeName(BRepGraph_NodeId::Edge(i), aStr);
  }
  EXPECT_EQ(aLayer->NbNames(), 1000);

  // Remove all odd indices.
  for (int i = 1; i < 1000; i += 2)
  {
    aLayer->OnNodeRemoved(BRepGraph_NodeId::Edge(i), BRepGraph_NodeId());
  }
  EXPECT_EQ(aLayer->NbNames(), 500);

  // Verify even indices remain.
  for (int i = 0; i < 1000; i += 2)
  {
    ASSERT_NE(aLayer->FindNodeName(BRepGraph_NodeId::Edge(i)), nullptr);
  }
}
