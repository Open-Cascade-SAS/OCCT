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
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_ChildIterator.hxx>
#include <BRepGraph_Explorer.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>

#include <BRepPrimAPI_MakeBox.hxx>
#include <BRep_Builder.hxx>
#include <TopAbs.hxx>
#include <TopoDS_Compound.hxx>

#include <NCollection_DataMap.hxx>
#include <NCollection_Vector.hxx>

#include <gtest/gtest.h>

TEST(BRepGraph_ChildIteratorTest, ShellFaces_CountAndOrder)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_ShellId aShellId(0);
  const NCollection_Vector<BRepGraph_FaceRefId>& aFaceRefIds = aGraph.Refs().FaceRefIdsOf(aShellId);

  NCollection_Vector<int> anExpectedFaceIds;
  for (int i = 0; i < aFaceRefIds.Length(); ++i)
  {
    const BRepGraphInc::FaceRef& aRef = aGraph.Refs().Face(aFaceRefIds.Value(i));
    if (!aRef.IsRemoved)
      anExpectedFaceIds.Append(aRef.FaceDefId.Index);
  }

  NCollection_Vector<int> anActualFaceIds;
  for (BRepGraph_ChildIterator anIt(aGraph, aShellId); anIt.More(); anIt.Next())
  {
    ASSERT_EQ(anIt.Current().NodeKind, BRepGraph_NodeId::Kind::Face);
    anActualFaceIds.Append(anIt.Current().Index);
  }

  ASSERT_EQ(anActualFaceIds.Length(), anExpectedFaceIds.Length());
  for (int i = 0; i < anExpectedFaceIds.Length(); ++i)
    EXPECT_EQ(anActualFaceIds.Value(i), anExpectedFaceIds.Value(i));
}

TEST(BRepGraph_ChildIteratorTest, RemovedFaceRef_IsSkipped)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_ShellId aShellId(0);
  const NCollection_Vector<BRepGraph_FaceRefId>& aFaceRefIds = aGraph.Refs().FaceRefIdsOf(aShellId);
  ASSERT_GT(aFaceRefIds.Length(), 0);

  const BRepGraph_FaceRefId aRemovedRef = aFaceRefIds.Value(0);
  const BRepGraph_FaceId aRemovedFaceId = aGraph.Refs().Face(aRemovedRef).FaceDefId;

  {
    BRepGraph_MutGuard<BRepGraphInc::FaceRef> aFaceRef = aGraph.Builder().MutFaceRef(aRemovedRef);
    aFaceRef->IsRemoved = true;
  }

  int aCount = 0;
  for (BRepGraph_ChildIterator anIt(aGraph, aShellId); anIt.More(); anIt.Next())
  {
    EXPECT_NE(anIt.Current(), BRepGraph_NodeId(aRemovedFaceId));
    ++aCount;
  }

  EXPECT_EQ(aCount, aFaceRefIds.Length() - 1);
}

TEST(BRepGraph_ChildIteratorTest, WireChildren_AreResolvedEdges)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_FaceId aFaceId(0);
  const NCollection_Vector<BRepGraph_WireRefId>& aWireRefs = aGraph.Refs().WireRefIdsOf(aFaceId);
  ASSERT_GT(aWireRefs.Length(), 0);

  const BRepGraph_WireId aWireId = aGraph.Refs().Wire(aWireRefs.Value(0)).WireDefId;
  const NCollection_Vector<BRepGraph_CoEdgeRefId>& aCoEdgeRefs = aGraph.Refs().CoEdgeRefIdsOf(aWireId);

  int aCount = 0;
  for (BRepGraph_ChildIterator anIt(aGraph, aWireId); anIt.More(); anIt.Next())
  {
    EXPECT_EQ(anIt.Current().NodeKind, BRepGraph_NodeId::Kind::Edge);
    ++aCount;
  }

  EXPECT_EQ(aCount, aCoEdgeRefs.Length());
}

TEST(BRepGraph_ChildIteratorTest, CompoundChildren_Basic)
{
  TopoDS_Compound aComp;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(aComp);
  aBuilder.Add(aComp, BRepPrimAPI_MakeBox(10, 10, 10).Shape());
  aBuilder.Add(aComp, BRepPrimAPI_MakeBox(20, 20, 20).Shape());

  BRepGraph aGraph;
  aGraph.Build(aComp);
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (BRepGraph_ChildIterator anIt(aGraph, BRepGraph_CompoundId(0)); anIt.More(); anIt.Next())
  {
    EXPECT_TRUE(BRepGraph_NodeId::IsTopologyKind(anIt.Current().NodeKind));
    ++aCount;
  }

  EXPECT_EQ(aCount, 2);
}

TEST(BRepGraph_ChildIteratorTest, ChainedTraversal_ParityWithExplorer)
{
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(100.0, 0.0, 0.0));

  TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10, 20, 30).Shape();
  aBox.Move(TopLoc_Location(aTrsf));

  TopoDS_Compound aComp;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(aComp);
  aBuilder.Add(aComp, aBox);

  BRepGraph aGraph;
  aGraph.Build(aComp);
  ASSERT_TRUE(aGraph.IsDone());

  NCollection_DataMap<int, TopLoc_Location> aExpectedLoc;
  NCollection_DataMap<int, TopAbs_Orientation> aExpectedOri;
  for (BRepGraph_Explorer anExp(aGraph, BRepGraph_CompoundId(0), BRepGraph_NodeId::Kind::Face);
       anExp.More(); anExp.Next())
  {
    const int aFaceIdx = anExp.Current().Index;
    if (!aExpectedLoc.IsBound(aFaceIdx))
    {
      aExpectedLoc.Bind(aFaceIdx, anExp.Location());
      aExpectedOri.Bind(aFaceIdx, anExp.Orientation());
    }
  }

  int aVisited = 0;
  for (BRepGraph_ChildIterator aSolidIt(aGraph, BRepGraph_CompoundId(0)); aSolidIt.More(); aSolidIt.Next())
  {
    for (BRepGraph_ChildIterator aShellIt(aGraph,
                                          aSolidIt.Current(),
                                          aSolidIt.Location(),
                                          aSolidIt.Orientation());
         aShellIt.More(); aShellIt.Next())
    {
      for (BRepGraph_ChildIterator aFaceIt(aGraph,
                                           aShellIt.Current(),
                                           aShellIt.Location(),
                                           aShellIt.Orientation());
           aFaceIt.More(); aFaceIt.Next())
      {
        ASSERT_EQ(aFaceIt.Current().NodeKind, BRepGraph_NodeId::Kind::Face);
        const int aFaceIdx = aFaceIt.Current().Index;
        ASSERT_TRUE(aExpectedLoc.IsBound(aFaceIdx));
        EXPECT_TRUE(aFaceIt.Location().IsEqual(aExpectedLoc.Find(aFaceIdx)));
        EXPECT_EQ(aFaceIt.Orientation(), aExpectedOri.Find(aFaceIdx));
        ++aVisited;
      }
    }
  }

  EXPECT_EQ(aVisited, aExpectedLoc.Extent());
}

TEST(BRepGraph_ChildIteratorTest, SharedProduct_ChildrenHaveDistinctContexts)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_ProductId aPart = aGraph.Builder().AddProduct(BRepGraph_SolidId(0));
  ASSERT_TRUE(aPart.IsValid());
  const BRepGraph_ProductId anAssembly = aGraph.Builder().AddAssemblyProduct();
  ASSERT_TRUE(anAssembly.IsValid());

  gp_Trsf aT1;
  aT1.SetTranslation(gp_Vec(10.0, 0.0, 0.0));
  gp_Trsf aT2;
  aT2.SetTranslation(gp_Vec(20.0, 0.0, 0.0));

  const BRepGraph_OccurrenceId anOcc1 =
    aGraph.Builder().AddOccurrence(anAssembly, aPart, TopLoc_Location(aT1));
  const BRepGraph_OccurrenceId anOcc2 =
    aGraph.Builder().AddOccurrence(anAssembly, aPart, TopLoc_Location(aT2));
  ASSERT_TRUE(anOcc1.IsValid());
  ASSERT_TRUE(anOcc2.IsValid());

  int             aCount = 0;
  TopLoc_Location aLoc1;
  TopLoc_Location aLoc2;
  for (BRepGraph_ChildIterator anIt(aGraph, anAssembly); anIt.More(); anIt.Next())
  {
    ASSERT_EQ(anIt.Current(), BRepGraph_NodeId(BRepGraph_SolidId(0)));
    if (aCount == 0)
      aLoc1 = anIt.Location();
    else if (aCount == 1)
      aLoc2 = anIt.Location();
    ++aCount;
  }

  ASSERT_EQ(aCount, 2);
  EXPECT_FALSE(aLoc1.IsEqual(aLoc2));
}

TEST(BRepGraph_ChildIteratorTest, ProductPartRootContext_ComposedWithOccurrence)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_ProductId aPart = aGraph.Builder().AddProduct(BRepGraph_SolidId(0));
  ASSERT_TRUE(aPart.IsValid());
  const BRepGraph_ProductId anAssembly = aGraph.Builder().AddAssemblyProduct();
  ASSERT_TRUE(anAssembly.IsValid());

  gp_Trsf aOccTrsf;
  aOccTrsf.SetTranslation(gp_Vec(10.0, 0.0, 0.0));
  const BRepGraph_OccurrenceId anOcc =
    aGraph.Builder().AddOccurrence(anAssembly, aPart, TopLoc_Location(aOccTrsf));
  ASSERT_TRUE(anOcc.IsValid());

  gp_Trsf aRootTrsf;
  aRootTrsf.SetTranslation(gp_Vec(0.0, 20.0, 0.0));
  {
    BRepGraph_MutGuard<BRepGraphInc::ProductDef> aMutPart = aGraph.Builder().MutProduct(aPart);
    aMutPart->RootLocation    = TopLoc_Location(aRootTrsf);
    aMutPart->RootOrientation = TopAbs_REVERSED;
  }

  BRepGraph_ChildIterator anIt(aGraph, anAssembly);
  ASSERT_TRUE(anIt.More());
  EXPECT_EQ(anIt.Current(), BRepGraph_NodeId(BRepGraph_SolidId(0)));

  const TopLoc_Location anExpectedLoc = TopLoc_Location(aOccTrsf) * TopLoc_Location(aRootTrsf);
  EXPECT_TRUE(anIt.Location().IsEqual(anExpectedLoc));
  EXPECT_EQ(anIt.Orientation(), TopAbs_REVERSED);

  anIt.Next();
  EXPECT_FALSE(anIt.More());
}

TEST(BRepGraph_ChildIteratorTest, HighFanout_DirectChildrenComplete)
{
  TopoDS_Compound aComp;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(aComp);

  constexpr int THE_NB_CHILDREN = 80;
  for (int i = 0; i < THE_NB_CHILDREN; ++i)
    aBuilder.Add(aComp, BRepPrimAPI_MakeBox(1.0 + i, 2.0, 3.0).Shape());

  BRepGraph aGraph;
  aGraph.Build(aComp);
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (BRepGraph_ChildIterator anIt(aGraph, BRepGraph_CompoundId(0)); anIt.More(); anIt.Next())
    ++aCount;

  EXPECT_EQ(aCount, THE_NB_CHILDREN);
}
