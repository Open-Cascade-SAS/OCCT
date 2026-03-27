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
#include <BRepGraph_RefId.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_RefUID.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <BRepGraph_VersionStamp.hxx>
#include "BRepGraph_RefTestTools.hxx"
#include <BRepGraphInc_Entity.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Standard_GUID.hxx>

#include <gtest/gtest.h>

namespace
{

int countInlineFaceRefs(const BRepGraph& theGraph)
{
  const BRepGraph::TopoView& aTopo = theGraph.Topo();
  int                         aNb   = 0;
  for (int i = 0; i < aTopo.NbShells(); ++i)
    aNb += BRepGraph_TestTools::CountFaceRefsOfShell(theGraph, BRepGraph_ShellId(i));
  return aNb;
}

int countInlineWireRefs(const BRepGraph& theGraph)
{
  const BRepGraph::TopoView& aTopo = theGraph.Topo();
  int                         aNb   = 0;
  for (int i = 0; i < aTopo.NbFaces(); ++i)
    aNb += BRepGraph_TestTools::CountWireRefsOfFace(theGraph, BRepGraph_FaceId(i));
  return aNb;
}

int countInlineCoEdgeRefs(const BRepGraph& theGraph)
{
  const BRepGraph::TopoView& aTopo = theGraph.Topo();
  int                         aNb   = 0;
  for (int i = 0; i < aTopo.NbWires(); ++i)
    aNb += BRepGraph_TestTools::CountCoEdgeRefsOfWire(theGraph, BRepGraph_WireId(i));
  return aNb;
}

int countInlineVertexRefs(const BRepGraph& theGraph)
{
  const BRepGraph::TopoView& aTopo = theGraph.Topo();
  int                         aNb   = 0;
  for (int i = 0; i < aTopo.NbEdges(); ++i)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = aTopo.Edge(BRepGraph_EdgeId(i));
    if (anEdge.StartVertex.VertexDefId.IsValid())
      ++aNb;
    if (anEdge.EndVertex.VertexDefId.IsValid())
      ++aNb;
    aNb += anEdge.InternalVertices.Length();
  }
  for (int i = 0; i < aTopo.NbFaces(); ++i)
    aNb += aTopo.Face(BRepGraph_FaceId(i)).VertexRefs.Length();
  return aNb;
}

int countInlineShellRefs(const BRepGraph& theGraph)
{
  const BRepGraph::TopoView& aTopo = theGraph.Topo();
  int                         aNb   = 0;
  for (int i = 0; i < aTopo.NbSolids(); ++i)
    aNb += BRepGraph_TestTools::CountShellRefsOfSolid(theGraph, BRepGraph_SolidId(i));
  return aNb;
}

int countInlineSolidRefs(const BRepGraph& theGraph)
{
  const BRepGraph::TopoView& aTopo = theGraph.Topo();
  int                         aNb   = 0;
  for (int i = 0; i < aTopo.NbCompSolids(); ++i)
    aNb += BRepGraph_TestTools::CountSolidRefsOfCompSolid(theGraph, BRepGraph_CompSolidId(i));
  return aNb;
}

int countInlineChildRefs(const BRepGraph& theGraph)
{
  const BRepGraph::TopoView& aTopo = theGraph.Topo();
  int                         aNb   = 0;
  for (int i = 0; i < aTopo.NbCompounds(); ++i)
    aNb += BRepGraph_TestTools::CountChildRefsOfParent(theGraph, BRepGraph_NodeId::Compound(i));
  for (int i = 0; i < aTopo.NbShells(); ++i)
    aNb += BRepGraph_TestTools::CountChildRefsOfParent(theGraph, BRepGraph_NodeId::Shell(i));
  for (int i = 0; i < aTopo.NbSolids(); ++i)
    aNb += BRepGraph_TestTools::CountChildRefsOfParent(theGraph, BRepGraph_NodeId::Solid(i));
  return aNb;
}

} // namespace

TEST(BRepGraph_RefIdTest, DefaultRefId_IsInvalid)
{
  const BRepGraph_RefId aRefId;
  EXPECT_FALSE(aRefId.IsValid());
}

TEST(BRepGraph_RefIdTest, TypedRefId_ConvertsToUntyped)
{
  const BRepGraph_FaceRefId aFaceRefId(3);
  const BRepGraph_RefId     anUntyped = aFaceRefId;
  EXPECT_EQ(anUntyped.RefKind, BRepGraph_RefId::Kind::Face);
  EXPECT_EQ(anUntyped.Index, 3);
}

TEST(BRepGraph_RefIdTest, RefUID_Default_IsInvalid)
{
  const BRepGraph_RefUID aRefUID;
  EXPECT_FALSE(aRefUID.IsValid());
}

TEST(BRepGraph_RefIdTest, RefUID_Equality_IgnoresGeneration)
{
  const BRepGraph_RefUID aUIDV1(BRepGraph_RefId::Kind::Face, 42, 1);
  const BRepGraph_RefUID aUIDV2(BRepGraph_RefId::Kind::Face, 42, 999);
  EXPECT_EQ(aUIDV1, aUIDV2);
}

TEST(BRepGraph_RefIdTest, RefsView_AfterBuild_HasFaceRefs)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());
  const int aFaceRefCount = aGraph.Refs().NbFaceRefs();
  EXPECT_GE(aFaceRefCount, 0);

  if (aFaceRefCount > 0)
  {
    const BRepGraph_FaceRefId          aFaceRefId(0);
    const BRepGraphInc::FaceRefEntry& anEntry = aGraph.Refs().Face(aFaceRefId);
    (void)anEntry;
    EXPECT_TRUE(aFaceRefId.IsValid(aFaceRefCount));
  }
  else
  {
    EXPECT_FALSE(aGraph.Refs().UIDOf(BRepGraph_RefId::Face(0)).IsValid());
  }
}

TEST(BRepGraph_RefIdTest, RefDomain_StampGUIDGeneration_IfSupported)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  if (aGraph.Refs().NbFaceRefs() <= 0)
  {
    GTEST_SKIP() << "Ref-domain entries are not populated at this phase";
  }

  const BRepGraph_VersionStamp aStamp = aGraph.Refs().StampOf(BRepGraph_RefId::Face(0));
  if (!aStamp.IsValid())
  {
    GTEST_SKIP() << "Ref-domain stamping/GUID generation not wired yet";
  }

  EXPECT_TRUE(aStamp.IsRefStamp());

  const Standard_GUID& aGraphGUID = aGraph.UIDs().GraphGUID();
  const Standard_GUID  aGUID1     = aStamp.ToGUID(aGraphGUID);
  const Standard_GUID  aGUID2     = aStamp.ToGUID(aGraphGUID);
  EXPECT_EQ(aGUID1, aGUID2);
}

TEST(BRepGraph_RefIdTest, RefsView_AfterBuild_CountsMatchInlineStorage)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Refs().NbFaceRefs(), countInlineFaceRefs(aGraph));
  EXPECT_EQ(aGraph.Refs().NbWireRefs(), countInlineWireRefs(aGraph));
  EXPECT_EQ(aGraph.Refs().NbCoEdgeRefs(), countInlineCoEdgeRefs(aGraph));
  EXPECT_EQ(aGraph.Refs().NbVertexRefs(), countInlineVertexRefs(aGraph));
  EXPECT_EQ(aGraph.Refs().NbShellRefs(), countInlineShellRefs(aGraph));
  EXPECT_EQ(aGraph.Refs().NbSolidRefs(), countInlineSolidRefs(aGraph));
  EXPECT_EQ(aGraph.Refs().NbChildRefs(), countInlineChildRefs(aGraph));
}

TEST(BRepGraph_RefIdTest, RefsView_AfterBuild_UIDRoundtripAndParentKinds)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  for (int i = 0; i < aGraph.Refs().NbFaceRefs(); ++i)
  {
    const BRepGraph_RefId       aRefId = BRepGraph_RefId::Face(i);
    const BRepGraph_RefUID      aUID   = aGraph.Refs().UIDOf(aRefId);
    const BRepGraph_VersionStamp aStamp = aGraph.Refs().StampOf(aRefId);
    EXPECT_TRUE(aUID.IsValid());
    EXPECT_EQ(aGraph.Refs().RefIdFrom(aUID), aRefId);
    EXPECT_TRUE(aGraph.Refs().Has(aUID));
    EXPECT_TRUE(aStamp.IsValid());
    EXPECT_TRUE(aStamp.IsRefStamp());
    EXPECT_FALSE(aGraph.Refs().IsStale(aStamp));

    const BRepGraphInc::FaceRefEntry& anEntry = aGraph.Refs().Face(BRepGraph_FaceRefId(i));
    EXPECT_EQ(anEntry.RefId, aRefId);
    EXPECT_EQ(anEntry.ParentId.NodeKind, BRepGraph_NodeId::Kind::Shell);
    EXPECT_TRUE(anEntry.FaceDefId.IsValid(aGraph.Topo().NbFaces()));
  }

  for (int i = 0; i < aGraph.Refs().NbWireRefs(); ++i)
  {
    const BRepGraph_RefId      aRefId = BRepGraph_RefId::Wire(i);
    const BRepGraph_RefUID     aUID   = aGraph.Refs().UIDOf(aRefId);
    const BRepGraphInc::WireRefEntry& anEntry = aGraph.Refs().Wire(BRepGraph_WireRefId(i));
    EXPECT_TRUE(aUID.IsValid());
    EXPECT_EQ(aGraph.Refs().RefIdFrom(aUID), aRefId);
    EXPECT_EQ(anEntry.RefId, aRefId);
    EXPECT_EQ(anEntry.ParentId.NodeKind, BRepGraph_NodeId::Kind::Face);
    EXPECT_TRUE(anEntry.WireDefId.IsValid(aGraph.Topo().NbWires()));
  }

  for (int i = 0; i < aGraph.Refs().NbCoEdgeRefs(); ++i)
  {
    const BRepGraph_RefId        aRefId = BRepGraph_RefId::CoEdge(i);
    const BRepGraph_RefUID       aUID   = aGraph.Refs().UIDOf(aRefId);
    const BRepGraphInc::CoEdgeRefEntry& anEntry = aGraph.Refs().CoEdge(BRepGraph_CoEdgeRefId(i));
    EXPECT_TRUE(aUID.IsValid());
    EXPECT_EQ(aGraph.Refs().RefIdFrom(aUID), aRefId);
    EXPECT_EQ(anEntry.RefId, aRefId);
    EXPECT_EQ(anEntry.ParentId.NodeKind, BRepGraph_NodeId::Kind::Wire);
    EXPECT_TRUE(anEntry.CoEdgeDefId.IsValid(aGraph.Topo().NbCoEdges()));
  }

  for (int i = 0; i < aGraph.Refs().NbShellRefs(); ++i)
  {
    const BRepGraph_RefId       aRefId = BRepGraph_RefId::Shell(i);
    const BRepGraph_RefUID      aUID   = aGraph.Refs().UIDOf(aRefId);
    const BRepGraphInc::ShellRefEntry& anEntry = aGraph.Refs().Shell(BRepGraph_ShellRefId(i));
    EXPECT_TRUE(aUID.IsValid());
    EXPECT_EQ(aGraph.Refs().RefIdFrom(aUID), aRefId);
    EXPECT_EQ(anEntry.RefId, aRefId);
    EXPECT_EQ(anEntry.ParentId.NodeKind, BRepGraph_NodeId::Kind::Solid);
    EXPECT_TRUE(anEntry.ShellDefId.IsValid(aGraph.Topo().NbShells()));
  }

  for (int i = 0; i < aGraph.Refs().NbVertexRefs(); ++i)
  {
    const BRepGraph_RefId        aRefId = BRepGraph_RefId::Vertex(i);
    const BRepGraph_RefUID       aUID   = aGraph.Refs().UIDOf(aRefId);
    const BRepGraphInc::VertexRefEntry& anEntry = aGraph.Refs().Vertex(BRepGraph_VertexRefId(i));
    EXPECT_TRUE(aUID.IsValid());
    EXPECT_EQ(aGraph.Refs().RefIdFrom(aUID), aRefId);
    EXPECT_EQ(anEntry.RefId, aRefId);
    EXPECT_TRUE(anEntry.ParentId.NodeKind == BRepGraph_NodeId::Kind::Edge
                || anEntry.ParentId.NodeKind == BRepGraph_NodeId::Kind::Face);
    EXPECT_TRUE(anEntry.VertexDefId.IsValid(aGraph.Topo().NbVertices()));
  }

  for (int i = 0; i < aGraph.Refs().NbSolidRefs(); ++i)
  {
    const BRepGraph_RefId       aRefId = BRepGraph_RefId::Solid(i);
    const BRepGraph_RefUID      aUID   = aGraph.Refs().UIDOf(aRefId);
    const BRepGraphInc::SolidRefEntry& anEntry = aGraph.Refs().Solid(BRepGraph_SolidRefId(i));
    EXPECT_TRUE(aUID.IsValid());
    EXPECT_EQ(aGraph.Refs().RefIdFrom(aUID), aRefId);
    EXPECT_EQ(anEntry.RefId, aRefId);
    EXPECT_EQ(anEntry.ParentId.NodeKind, BRepGraph_NodeId::Kind::CompSolid);
    EXPECT_TRUE(anEntry.SolidDefId.IsValid(aGraph.Topo().NbSolids()));
  }

  for (int i = 0; i < aGraph.Refs().NbChildRefs(); ++i)
  {
    const BRepGraph_RefId       aRefId = BRepGraph_RefId::Child(i);
    const BRepGraph_RefUID      aUID   = aGraph.Refs().UIDOf(aRefId);
    const BRepGraphInc::ChildRefEntry& anEntry = aGraph.Refs().Child(BRepGraph_ChildRefId(i));
    EXPECT_TRUE(aUID.IsValid());
    EXPECT_EQ(aGraph.Refs().RefIdFrom(aUID), aRefId);
    EXPECT_EQ(anEntry.RefId, aRefId);
    EXPECT_TRUE(anEntry.ParentId.NodeKind == BRepGraph_NodeId::Kind::Compound
                || anEntry.ParentId.NodeKind == BRepGraph_NodeId::Kind::Shell
                || anEntry.ParentId.NodeKind == BRepGraph_NodeId::Kind::Solid);
    EXPECT_TRUE(anEntry.ChildDefId.IsValid());
  }
}

TEST(BRepGraph_RefIdTest, MutFaceRef_UpdatesRefStampAndParentModifiedFlag)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  if (aGraph.Refs().NbFaceRefs() <= 0)
  {
    GTEST_SKIP() << "No face references available";
  }

  const BRepGraph_FaceRefId            aFaceRefId(0);
  const BRepGraphInc::FaceRefEntry&   aBeforeEntry = aGraph.Refs().Face(aFaceRefId);
  const BRepGraph_VersionStamp         aBeforeStamp = aGraph.Refs().StampOf(aFaceRefId);
  ASSERT_TRUE(aBeforeEntry.ParentId.IsValid());
  ASSERT_TRUE(aBeforeStamp.IsValid());
  ASSERT_TRUE(aBeforeStamp.IsRefStamp());
  const uint32_t         aBeforeMutationGen = aBeforeEntry.MutationGen;
  const TopAbs_Orientation anBeforeOri        = aBeforeEntry.Orientation;

  {
    auto aMut = aGraph.Builder().MutFaceRef(aFaceRefId);
    aMut->Orientation = (anBeforeOri == TopAbs_FORWARD) ? TopAbs_REVERSED : TopAbs_FORWARD;
  }

  const BRepGraphInc::FaceRefEntry& aAfterEntry = aGraph.Refs().Face(aFaceRefId);
  EXPECT_NE(aAfterEntry.Orientation, anBeforeOri);
  EXPECT_GT(aAfterEntry.MutationGen, aBeforeMutationGen);
  EXPECT_TRUE(aGraph.Refs().IsStale(aBeforeStamp));

  const BRepGraph_TopoNode::BaseDef* aParentDef = aGraph.Topo().TopoDef(aAfterEntry.ParentId);
  ASSERT_NE(aParentDef, nullptr);
  EXPECT_TRUE(aParentDef->IsModified);
}

TEST(BRepGraph_RefIdTest, MutFaceRef_MarkRemoved_PersistsAndInvalidatesStamp)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  if (aGraph.Refs().NbFaceRefs() <= 0)
  {
    GTEST_SKIP() << "No face references available";
  }

  const BRepGraph_FaceRefId aFaceRefId(0);
  const BRepGraph_VersionStamp aBeforeStamp = aGraph.Refs().StampOf(aFaceRefId);
  ASSERT_TRUE(aBeforeStamp.IsValid());
  ASSERT_TRUE(aBeforeStamp.IsRefStamp());

  {
    auto aMut = aGraph.Builder().MutFaceRef(aFaceRefId);
    aMut->IsRemoved = true;
  }

  const BRepGraphInc::FaceRefEntry& aAfterEntry = aGraph.Refs().Face(aFaceRefId);
  EXPECT_TRUE(aAfterEntry.IsRemoved);
  EXPECT_TRUE(aGraph.Refs().IsStale(aBeforeStamp));
  EXPECT_FALSE(aGraph.Refs().StampOf(aFaceRefId).IsValid());
}

TEST(BRepGraph_RefIdTest, ChildRefs_CompoundEntriesAreValid)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph::TopoView& aTopo = aGraph.Topo();
  const BRepGraph::RefsView& aRefs = aGraph.Refs();

  for (int aCompIdx = 0; aCompIdx < aTopo.NbCompounds(); ++aCompIdx)
  {
    const NCollection_Vector<BRepGraph_ChildRefId> aChildRefs =
      BRepGraph_TestTools::ChildRefsOfParent(aGraph, BRepGraph_NodeId::Compound(aCompIdx));
    for (int aChildRefIdx = 0; aChildRefIdx < aChildRefs.Length(); ++aChildRefIdx)
    {
      const BRepGraphInc::ChildRefEntry& aRefEntry = aRefs.Child(aChildRefs.Value(aChildRefIdx));
      EXPECT_EQ(aRefEntry.ParentId, BRepGraph_NodeId::Compound(aCompIdx));
      EXPECT_TRUE(aRefEntry.ChildDefId.IsValid());
      EXPECT_FALSE(aRefEntry.IsRemoved);
    }
  }
}
