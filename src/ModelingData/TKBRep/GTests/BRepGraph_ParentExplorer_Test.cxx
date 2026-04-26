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
#include <BRepGraph_ParentExplorer.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Builder.hxx>

#include <BRepPrimAPI_MakeBox.hxx>

#include <Precision.hxx>

#include <gtest/gtest.h>

TEST(BRepGraph_ParentExplorerTest, FaceParents_All_CountAndOrder)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_ParentExplorer anExp(aGraph, BRepGraph_FaceId::Start());
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId, BRepGraph_NodeId(BRepGraph_ShellId::Start()));

  anExp.Next();
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId, BRepGraph_NodeId(BRepGraph_SolidId::Start()));

  anExp.Next();
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId.NodeKind, BRepGraph_NodeId::Kind::Product);

  anExp.Next();
  EXPECT_FALSE(anExp.More());
}

TEST(BRepGraph_ParentExplorerTest, FaceParents_TypedSolid_OneResult)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_ParentExplorer anExp(aGraph, BRepGraph_FaceId::Start(), BRepGraph_NodeId::Kind::Solid);
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId, BRepGraph_NodeId(BRepGraph_SolidId::Start()));

  anExp.Next();
  EXPECT_FALSE(anExp.More());
}

TEST(BRepGraph_ParentExplorerTest, FaceParents_DirectParents_StopsAtImmediateShell)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_ParentExplorer anExp(aGraph,
                                 BRepGraph_FaceId::Start(),
                                 BRepGraph_ParentExplorer::TraversalMode::DirectParents);
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId, BRepGraph_NodeId(BRepGraph_ShellId::Start()));

  anExp.Next();
  EXPECT_FALSE(anExp.More());
}

TEST(BRepGraph_ParentExplorerTest, FaceParents_DirectParents_ExposeChildAndRef)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_ParentExplorer anExp(aGraph,
                                 BRepGraph_FaceId::Start(),
                                 BRepGraph_ParentExplorer::TraversalMode::DirectParents);
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId, BRepGraph_NodeId(BRepGraph_ShellId::Start()));
  EXPECT_EQ(anExp.CurrentChild(), BRepGraph_NodeId(BRepGraph_FaceId::Start()));
  EXPECT_EQ(anExp.CurrentLinkKind(), BRepGraph_ParentExplorer::LinkKind::Reference);

  const BRepGraph_FaceRefId aFaceRefId =
    aGraph.Refs().Faces().IdsOf(BRepGraph_ShellId::Start()).Value(0);
  EXPECT_EQ(anExp.CurrentRef(), BRepGraph_RefId(aFaceRefId));
}

TEST(BRepGraph_ParentExplorerTest, AvoidKind_Solid_PrunesProducts)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_ParentExplorer anExp(aGraph,
                                 BRepGraph_FaceId::Start(),
                                 BRepGraph_NodeId::Kind::Product,
                                 BRepGraph_NodeId::Kind::Solid,
                                 false);
  EXPECT_FALSE(anExp.More());
}

TEST(BRepGraph_ParentExplorerTest, AvoidKind_EmitBoundary_ReturnsSolidInsteadOfProducts)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_ParentExplorer anExp(aGraph,
                                 BRepGraph_FaceId::Start(),
                                 BRepGraph_NodeId::Kind::Product,
                                 BRepGraph_NodeId::Kind::Solid,
                                 true);
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId, BRepGraph_NodeId(BRepGraph_SolidId::Start()));

  anExp.Next();
  EXPECT_FALSE(anExp.More());
}

TEST(BRepGraph_ParentExplorerTest, AvoidKind_SameAsTarget_IsIgnored)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_ParentExplorer anExp(aGraph,
                                 BRepGraph_FaceId::Start(),
                                 BRepGraph_NodeId::Kind::Solid,
                                 BRepGraph_NodeId::Kind::Solid,
                                 false);
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId, BRepGraph_NodeId(BRepGraph_SolidId::Start()));

  anExp.Next();
  EXPECT_FALSE(anExp.More());
}

TEST(BRepGraph_ParentExplorerTest, AllParents_AvoidSolid_PrunesProducts)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_ParentExplorer anExp(aGraph,
                                 BRepGraph_FaceId::Start(),
                                 BRepGraph_NodeId::Kind::Solid,
                                 false);
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId, BRepGraph_NodeId(BRepGraph_ShellId::Start()));

  anExp.Next();
  EXPECT_FALSE(anExp.More());
}

TEST(BRepGraph_ParentExplorerTest, AllParents_AvoidSolidEmitBoundary_ReturnsShellAndSolid)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_ParentExplorer anExp(aGraph,
                                 BRepGraph_FaceId::Start(),
                                 BRepGraph_NodeId::Kind::Solid,
                                 true);
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId, BRepGraph_NodeId(BRepGraph_ShellId::Start()));

  anExp.Next();
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId, BRepGraph_NodeId(BRepGraph_SolidId::Start()));

  anExp.Next();
  EXPECT_FALSE(anExp.More());
}

TEST(BRepGraph_ParentExplorerTest, SharedProduct_ProductParentsKeepDistinctContexts)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_ProductId aPart      = aGraph.Editor().Products().LinkProductToTopology(BRepGraph_SolidId::Start());
  const BRepGraph_ProductId anAssembly = aGraph.Editor().Products().CreateEmptyProduct();
  ASSERT_TRUE(aPart.IsValid());
  ASSERT_TRUE(anAssembly.IsValid());

  gp_Trsf aT1;
  aT1.SetTranslation(gp_Vec(10.0, 0.0, 0.0));
  gp_Trsf aT2;
  aT2.SetTranslation(gp_Vec(25.0, 0.0, 0.0));
  ASSERT_TRUE(
    aGraph.Editor().Products().LinkProducts(anAssembly, aPart, TopLoc_Location(aT1)).IsValid());
  ASSERT_TRUE(
    aGraph.Editor().Products().LinkProducts(anAssembly, aPart, TopLoc_Location(aT2)).IsValid());

  int             aPartCount = 0;
  TopLoc_Location aLoc1;
  TopLoc_Location aLoc2;
  for (BRepGraph_ParentExplorer anExp(aGraph,
                                      BRepGraph_SolidId::Start(),
                                      BRepGraph_NodeId::Kind::Product);
       anExp.More();
       anExp.Next())
  {
    if (anExp.Current().DefId != BRepGraph_NodeId(aPart))
    {
      continue;
    }
    if (aPartCount == 0)
    {
      aLoc1 = anExp.Current().Location;
    }
    else if (aPartCount == 1)
    {
      aLoc2 = anExp.Current().Location;
    }
    ++aPartCount;
  }

  ASSERT_EQ(aPartCount, 2);
  EXPECT_FALSE(aLoc1.IsEqual(aLoc2));
}

TEST(BRepGraph_ParentExplorerTest, ShapeRootProductParent_HasChildButNoRef)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // BRepGraph_Builder::Add() auto-creates a root Product for the shape root node.
  // Use that product instead of creating a duplicate.
  ASSERT_GT(aGraph.Topo().Products().Nb(), 0);
  const BRepGraph_ProductId aPart(0);

  BRepGraph_ParentExplorer anExp(aGraph,
                                 BRepGraph_SolidId::Start(),
                                 BRepGraph_NodeId::Kind::Product,
                                 BRepGraph_ParentExplorer::TraversalMode::DirectParents);
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId, BRepGraph_NodeId(aPart));
  EXPECT_EQ(anExp.CurrentChild(), BRepGraph_NodeId(BRepGraph_SolidId::Start()));
  EXPECT_EQ(anExp.CurrentLinkKind(), BRepGraph_ParentExplorer::LinkKind::Structural);
  EXPECT_FALSE(anExp.CurrentRef().IsValid());

  anExp.Next();
  EXPECT_FALSE(anExp.More());
}

TEST(BRepGraph_ParentExplorerTest, OccurrenceParent_ExposeOccurrenceRef)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_ProductId aPart      = aGraph.Editor().Products().LinkProductToTopology(BRepGraph_SolidId::Start());
  const BRepGraph_ProductId anAssembly = aGraph.Editor().Products().CreateEmptyProduct();
  ASSERT_TRUE(aPart.IsValid());
  ASSERT_TRUE(anAssembly.IsValid());

  const BRepGraph_OccurrenceId anOccurrence =
    aGraph.Editor().Products().LinkProducts(anAssembly, aPart, TopLoc_Location());
  ASSERT_TRUE(anOccurrence.IsValid());

  BRepGraph_ParentExplorer anExp(aGraph,
                                 anOccurrence,
                                 BRepGraph_NodeId::Kind::Product,
                                 BRepGraph_ParentExplorer::TraversalMode::DirectParents);
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId, BRepGraph_NodeId(anAssembly));
  EXPECT_EQ(anExp.CurrentChild(), BRepGraph_NodeId(anOccurrence));
  EXPECT_EQ(anExp.CurrentLinkKind(), BRepGraph_ParentExplorer::LinkKind::Reference);

  const BRepGraph_OccurrenceRefId anOccurrenceRefId =
    aGraph.Refs().Occurrences().IdsOf(anAssembly).Value(0);
  EXPECT_EQ(anExp.CurrentRef(), BRepGraph_RefId(anOccurrenceRefId));
}

TEST(BRepGraph_ParentExplorerTest, ProductParents_ImmediateOccurrence_IsStructural)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_ProductId aPart      = aGraph.Editor().Products().LinkProductToTopology(BRepGraph_SolidId::Start());
  const BRepGraph_ProductId anAssembly = aGraph.Editor().Products().CreateEmptyProduct();
  ASSERT_TRUE(aPart.IsValid());
  ASSERT_TRUE(anAssembly.IsValid());

  const BRepGraph_OccurrenceId anOccurrence =
    aGraph.Editor().Products().LinkProducts(anAssembly, aPart, TopLoc_Location());
  ASSERT_TRUE(anOccurrence.IsValid());

  BRepGraph_ParentExplorer anExp(aGraph,
                                 aPart,
                                 BRepGraph_ParentExplorer::TraversalMode::DirectParents);
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId, BRepGraph_NodeId(anOccurrence));
  EXPECT_EQ(anExp.CurrentChild(), BRepGraph_NodeId(aPart));
  EXPECT_EQ(anExp.CurrentLinkKind(), BRepGraph_ParentExplorer::LinkKind::Structural);
  EXPECT_FALSE(anExp.CurrentRef().IsValid());
}

TEST(BRepGraph_ParentExplorerTest, CoEdgeParents_ImmediateWireIsVisible)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const NCollection_DynamicArray<BRepGraph_WireRefId>& aWireRefIds =
    aGraph.Refs().Wires().IdsOf(BRepGraph_FaceId::Start());
  ASSERT_GT(aWireRefIds.Length(), 0);
  const BRepGraph_WireId aWireId = aGraph.Refs().Wires().Entry(aWireRefIds.Value(0)).WireDefId;

  const NCollection_DynamicArray<BRepGraph_CoEdgeRefId>& aCoEdgeRefIds =
    aGraph.Refs().CoEdges().IdsOf(aWireId);
  ASSERT_GT(aCoEdgeRefIds.Length(), 0);
  const BRepGraph_CoEdgeId aCoEdgeId =
    aGraph.Refs().CoEdges().Entry(aCoEdgeRefIds.Value(0)).CoEdgeDefId;

  BRepGraph_ParentExplorer anExp(aGraph, aCoEdgeId);
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId, BRepGraph_NodeId(aWireId));

  anExp.Next();
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId, BRepGraph_NodeId(BRepGraph_FaceId::Start()));
}

TEST(BRepGraph_ParentExplorerTest, ProductRoot_HasNoParents)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_ProductId aRootProduct = aGraph.Editor().Products().CreateEmptyProduct();
  ASSERT_TRUE(aRootProduct.IsValid());

  BRepGraph_ParentExplorer anExp(aGraph, aRootProduct);
  EXPECT_FALSE(anExp.More());
}