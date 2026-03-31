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
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_ParentExplorer.hxx>
#include <BRepGraph_RefsView.hxx>

#include <BRepPrimAPI_MakeBox.hxx>

#include <NCollection_IncAllocator.hxx>

#include <Precision.hxx>

#include <gtest/gtest.h>

namespace
{
static occ::handle<NCollection_BaseAllocator> pathAllocator()
{
  return new NCollection_IncAllocator();
}
}

TEST(BRepGraph_ParentExplorerTest, FaceParents_All_CountAndOrder)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_ParentExplorer anExp(aGraph, BRepGraph_FaceId(0));
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current(), BRepGraph_NodeId(BRepGraph_ShellId(0)));

  anExp.Next();
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current(), BRepGraph_NodeId(BRepGraph_SolidId(0)));

  anExp.Next();
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().NodeKind, BRepGraph_NodeId::Kind::Product);

  anExp.Next();
  EXPECT_FALSE(anExp.More());
}

TEST(BRepGraph_ParentExplorerTest, FaceParents_TypedSolid_OneResult)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_ParentExplorer anExp(aGraph, BRepGraph_FaceId(0), BRepGraph_NodeId::Kind::Solid);
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current(), BRepGraph_NodeId(BRepGraph_SolidId(0)));

  anExp.Next();
  EXPECT_FALSE(anExp.More());
}

TEST(BRepGraph_ParentExplorerTest, SharedProduct_ProductParentsKeepDistinctContexts)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_ProductId aPart = aGraph.Builder().AddProduct(BRepGraph_SolidId(0));
  const BRepGraph_ProductId anAssembly = aGraph.Builder().AddAssemblyProduct();
  ASSERT_TRUE(aPart.IsValid());
  ASSERT_TRUE(anAssembly.IsValid());

  gp_Trsf aT1;
  aT1.SetTranslation(gp_Vec(10.0, 0.0, 0.0));
  gp_Trsf aT2;
  aT2.SetTranslation(gp_Vec(25.0, 0.0, 0.0));
  ASSERT_TRUE(aGraph.Builder().AddOccurrence(anAssembly, aPart, TopLoc_Location(aT1)).IsValid());
  ASSERT_TRUE(aGraph.Builder().AddOccurrence(anAssembly, aPart, TopLoc_Location(aT2)).IsValid());

  int             aPartCount = 0;
  TopLoc_Location aLoc1;
  TopLoc_Location aLoc2;
  for (BRepGraph_ParentExplorer anExp(aGraph,
                                      BRepGraph_SolidId(0),
                                      BRepGraph_NodeId::Kind::Product);
       anExp.More(); anExp.Next())
  {
    if (anExp.Current() != BRepGraph_NodeId(aPart))
    {
      continue;
    }
    if (aPartCount == 0)
    {
      aLoc1 = anExp.Location();
    }
    else if (aPartCount == 1)
    {
      aLoc2 = anExp.Location();
    }
    ++aPartCount;
  }

  ASSERT_EQ(aPartCount, 2);
  EXPECT_FALSE(aLoc1.IsEqual(aLoc2));
}

TEST(BRepGraph_ParentExplorerTest, SharedProduct_ProductParentCurrentPath_OwnsOccurrenceBranch)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_ProductId aPart = aGraph.Builder().AddProduct(BRepGraph_SolidId(0));
  const BRepGraph_ProductId anAssembly = aGraph.Builder().AddAssemblyProduct();
  ASSERT_TRUE(aPart.IsValid());
  ASSERT_TRUE(anAssembly.IsValid());

  gp_Trsf aT1;
  aT1.SetTranslation(gp_Vec(10.0, 0.0, 0.0));
  const BRepGraph_OccurrenceId anOcc =
    aGraph.Builder().AddOccurrence(anAssembly, aPart, TopLoc_Location(aT1));
  ASSERT_TRUE(anOcc.IsValid());

  BRepGraph_ParentExplorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Product);
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current(), BRepGraph_NodeId(aPart));

  const BRepGraph_TopologyPath aPath = anExp.CurrentPath(pathAllocator());
  EXPECT_EQ(aPath.Root(), BRepGraph_NodeId(anAssembly));
  EXPECT_EQ(aPath.Depth(), 1);
}

TEST(BRepGraph_ParentExplorerTest, CoEdgeParents_ImmediateWireIsVisible)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const NCollection_Vector<BRepGraph_WireRefId>& aWireRefIds = aGraph.Refs().WireRefIdsOf(BRepGraph_FaceId(0));
  ASSERT_GT(aWireRefIds.Length(), 0);
  const BRepGraph_WireId aWireId = aGraph.Refs().Wire(aWireRefIds.Value(0)).WireDefId;

  const NCollection_Vector<BRepGraph_CoEdgeRefId>& aCoEdgeRefIds = aGraph.Refs().CoEdgeRefIdsOf(aWireId);
  ASSERT_GT(aCoEdgeRefIds.Length(), 0);
  const BRepGraph_CoEdgeId aCoEdgeId = aGraph.Refs().CoEdge(aCoEdgeRefIds.Value(0)).CoEdgeDefId;

  BRepGraph_ParentExplorer anExp(aGraph, aCoEdgeId);
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current(), BRepGraph_NodeId(aWireId));

  anExp.Next();
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current(), BRepGraph_NodeId(BRepGraph_FaceId(0)));
}

TEST(BRepGraph_ParentExplorerTest, ProductRoot_HasNoParents)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_ProductId aRootProduct = aGraph.Builder().AddAssemblyProduct();
  ASSERT_TRUE(aRootProduct.IsValid());

  BRepGraph_ParentExplorer anExp(aGraph, aRootProduct);
  EXPECT_FALSE(anExp.More());
}