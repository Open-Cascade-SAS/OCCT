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
#include <BRepGraphInc_Representation.hxx>
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_RefId.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_RefUID.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <BRepGraph_VersionStamp.hxx>
#include "BRepGraph_RefTestTools.hxx"
#include <BRepPrimAPI_MakeBox.hxx>
#include <Standard_GUID.hxx>

#include <gtest/gtest.h>

namespace
{

int countInlineFaceRefs(const BRepGraph& theGraph)
{
  int aNb = 0;
  for (BRepGraph_ShellIterator aShellIt(theGraph); aShellIt.More(); aShellIt.Next())
    aNb += BRepGraph_TestTools::CountFaceRefsOfShell(theGraph, aShellIt.CurrentId());
  return aNb;
}

int countInlineWireRefs(const BRepGraph& theGraph)
{
  int aNb = 0;
  for (BRepGraph_FaceIterator aFaceIt(theGraph); aFaceIt.More(); aFaceIt.Next())
    aNb += BRepGraph_TestTools::CountWireRefsOfFace(theGraph, aFaceIt.CurrentId());
  return aNb;
}

int countInlineCoEdgeRefs(const BRepGraph& theGraph)
{
  int aNb = 0;
  for (BRepGraph_WireIterator aWireIt(theGraph); aWireIt.More(); aWireIt.Next())
    aNb += BRepGraph_TestTools::CountCoEdgeRefsOfWire(theGraph, aWireIt.CurrentId());
  return aNb;
}

int countInlineVertexRefs(const BRepGraph& theGraph)
{
  int aNb = 0;
  for (BRepGraph_EdgeIterator anEdgeIt(theGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraphInc::EdgeDef& anEdge = anEdgeIt.Current();
    if (anEdge.StartVertexRefId.IsValid())
      ++aNb;
    if (anEdge.EndVertexRefId.IsValid())
      ++aNb;
    aNb += anEdge.InternalVertexRefIds.Length();
  }
  for (BRepGraph_FaceIterator aFaceIt(theGraph); aFaceIt.More(); aFaceIt.Next())
    aNb += aFaceIt.Current().VertexRefIds.Length();
  return aNb;
}

int countInlineShellRefs(const BRepGraph& theGraph)
{
  int aNb = 0;
  for (BRepGraph_SolidIterator aSolidIt(theGraph); aSolidIt.More(); aSolidIt.Next())
    aNb += BRepGraph_TestTools::CountShellRefsOfSolid(theGraph, aSolidIt.CurrentId());
  return aNb;
}

int countInlineSolidRefs(const BRepGraph& theGraph)
{
  int aNb = 0;
  for (BRepGraph_CompSolidIterator aCSIt(theGraph); aCSIt.More(); aCSIt.Next())
    aNb += BRepGraph_TestTools::CountSolidRefsOfCompSolid(theGraph, aCSIt.CurrentId());
  return aNb;
}

int countInlineChildRefs(const BRepGraph& theGraph)
{
  int aNb = 0;
  for (BRepGraph_CompoundIterator aCompIt(theGraph); aCompIt.More(); aCompIt.Next())
    aNb += BRepGraph_TestTools::CountChildRefsOfParent(theGraph, aCompIt.CurrentId());
  for (BRepGraph_ShellIterator aShellIt(theGraph); aShellIt.More(); aShellIt.Next())
    aNb += BRepGraph_TestTools::CountChildRefsOfParent(theGraph, aShellIt.CurrentId());
  for (BRepGraph_SolidIterator aSolidIt(theGraph); aSolidIt.More(); aSolidIt.Next())
    aNb += BRepGraph_TestTools::CountChildRefsOfParent(theGraph, aSolidIt.CurrentId());
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

TEST(BRepGraph_RefIdTest, UntypedArithmetic_PreservesKindAndIndex)
{
  BRepGraph_RefId aRefId(BRepGraph_RefId::Kind::Face, 1);

  const BRepGraph_RefId aPrev = aRefId++;
  EXPECT_EQ(aPrev.RefKind, BRepGraph_RefId::Kind::Face);
  EXPECT_EQ(aPrev.Index, 1);
  EXPECT_EQ(aRefId.Index, 2);

  ++aRefId;
  EXPECT_EQ(aRefId.Index, 3);

  const BRepGraph_RefId anAdvanced = aRefId + 4;
  EXPECT_EQ(anAdvanced.RefKind, BRepGraph_RefId::Kind::Face);
  EXPECT_EQ(anAdvanced.Index, 7);

  const BRepGraph_RefId aRetreated = anAdvanced - 6;
  EXPECT_EQ(aRetreated.RefKind, BRepGraph_RefId::Kind::Face);
  EXPECT_EQ(aRetreated.Index, 1);
}

TEST(BRepGraph_RefIdTest, TypedArithmetic_PreservesKindAndIndex)
{
  BRepGraph_FaceRefId aFaceRefId(1);

  const BRepGraph_FaceRefId aPrev = aFaceRefId++;
  EXPECT_EQ(aPrev.Index, 1);
  EXPECT_EQ(aFaceRefId.Index, 2);

  ++aFaceRefId;
  EXPECT_EQ(aFaceRefId.Index, 3);

  const BRepGraph_FaceRefId anAdvanced = aFaceRefId + 4;
  EXPECT_EQ(anAdvanced.Index, 7);

  const BRepGraph_FaceRefId aRetreated = anAdvanced - 6;
  EXPECT_EQ(aRetreated.Index, 1);

  // Verify kind is preserved through implicit conversion.
  const BRepGraph_RefId aRefId = anAdvanced;
  EXPECT_EQ(aRefId.RefKind, BRepGraph_RefId::Kind::Face);
  EXPECT_EQ(aRefId.Index, 7);
}

TEST(BRepGraph_RefIdTest, TypedArithmetic_IndexZeroBoundary)
{
  BRepGraph_CoEdgeRefId aCoEdge(0);
  EXPECT_TRUE(aCoEdge.IsValid());

  // Increment from zero.
  ++aCoEdge;
  EXPECT_EQ(aCoEdge.Index, 1);

  // Retreat back to zero  still valid.
  const BRepGraph_CoEdgeRefId aZero = aCoEdge - 1;
  EXPECT_EQ(aZero.Index, 0);
  EXPECT_TRUE(aZero.IsValid());

  // Subtract to -1  produces invalid id (allowed by constructor).
  const BRepGraph_CoEdgeRefId anInvalid = aZero - 1;
  EXPECT_EQ(anInvalid.Index, -1);
  EXPECT_FALSE(anInvalid.IsValid());
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
  const int aFaceRefCount = aGraph.Refs().Faces().Nb();
  EXPECT_GE(aFaceRefCount, 0);

  if (aFaceRefCount > 0)
  {
    const BRepGraph_FaceRefId    aFaceRefId(0);
    const BRepGraphInc::FaceRef& anEntry = aGraph.Refs().Faces().Entry(aFaceRefId);
    (void)anEntry;
    EXPECT_TRUE(aFaceRefId.IsValid(aFaceRefCount));
  }
  else
  {
    EXPECT_FALSE(aGraph.UIDs().Of(BRepGraph_FaceRefId(0)).IsValid());
  }
}

TEST(BRepGraph_RefIdTest, RefDomain_StampGUIDGeneration_IfSupported)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  if (aGraph.Refs().Faces().Nb() <= 0)
  {
    GTEST_SKIP() << "Ref-domain entries are not populated at this phase";
  }

  const BRepGraph_VersionStamp aStamp = aGraph.UIDs().StampOf(BRepGraph_FaceRefId(0));
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

  EXPECT_EQ(aGraph.Refs().Faces().Nb(), countInlineFaceRefs(aGraph));
  EXPECT_EQ(aGraph.Refs().Wires().Nb(), countInlineWireRefs(aGraph));
  EXPECT_EQ(aGraph.Refs().CoEdges().Nb(), countInlineCoEdgeRefs(aGraph));
  EXPECT_EQ(aGraph.Refs().Vertices().Nb(), countInlineVertexRefs(aGraph));
  EXPECT_EQ(aGraph.Refs().Shells().Nb(), countInlineShellRefs(aGraph));
  EXPECT_EQ(aGraph.Refs().Solids().Nb(), countInlineSolidRefs(aGraph));
  EXPECT_EQ(aGraph.Refs().Children().Nb(), countInlineChildRefs(aGraph));
}

TEST(BRepGraph_RefIdTest, RefsView_AfterBuild_UIDRoundtripAndParentKinds)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const int aNbFaceRefs = aGraph.Refs().Faces().Nb();
  for (BRepGraph_FaceRefId aFaceRefId(0); aFaceRefId.IsValid(aNbFaceRefs); ++aFaceRefId)
  {
    const BRepGraph_RefId        aRefId = aFaceRefId;
    const BRepGraph_RefUID       aUID   = aGraph.UIDs().Of(aRefId);
    const BRepGraph_VersionStamp aStamp = aGraph.UIDs().StampOf(aRefId);
    EXPECT_TRUE(aUID.IsValid());
    EXPECT_EQ(aGraph.UIDs().RefIdFrom(aUID), aRefId);
    EXPECT_TRUE(aGraph.UIDs().Has(aUID));
    EXPECT_TRUE(aStamp.IsValid());
    EXPECT_TRUE(aStamp.IsRefStamp());
    EXPECT_FALSE(aGraph.UIDs().IsStale(aStamp));

    const BRepGraphInc::FaceRef& anEntry = aGraph.Refs().Faces().Entry(aFaceRefId);
    EXPECT_EQ(anEntry.RefId, aRefId);
    EXPECT_EQ(anEntry.ParentId.NodeKind, BRepGraph_NodeId::Kind::Shell);
    EXPECT_TRUE(anEntry.FaceDefId.IsValid(aGraph.Topo().Faces().Nb()));
  }

  const int aNbWireRefs = aGraph.Refs().Wires().Nb();
  for (BRepGraph_WireRefId aWireRefId(0); aWireRefId.IsValid(aNbWireRefs); ++aWireRefId)
  {
    const BRepGraph_RefId        aRefId  = aWireRefId;
    const BRepGraph_RefUID       aUID    = aGraph.UIDs().Of(aRefId);
    const BRepGraphInc::WireRef& anEntry = aGraph.Refs().Wires().Entry(aWireRefId);
    EXPECT_TRUE(aUID.IsValid());
    EXPECT_EQ(aGraph.UIDs().RefIdFrom(aUID), aRefId);
    EXPECT_EQ(anEntry.RefId, aRefId);
    EXPECT_EQ(anEntry.ParentId.NodeKind, BRepGraph_NodeId::Kind::Face);
    EXPECT_TRUE(anEntry.WireDefId.IsValid(aGraph.Topo().Wires().Nb()));
  }

  const int aNbCoEdgeRefs = aGraph.Refs().CoEdges().Nb();
  for (BRepGraph_CoEdgeRefId aCoEdgeRefId(0); aCoEdgeRefId.IsValid(aNbCoEdgeRefs); ++aCoEdgeRefId)
  {
    const BRepGraph_RefId          aRefId  = aCoEdgeRefId;
    const BRepGraph_RefUID         aUID    = aGraph.UIDs().Of(aRefId);
    const BRepGraphInc::CoEdgeRef& anEntry = aGraph.Refs().CoEdges().Entry(aCoEdgeRefId);
    EXPECT_TRUE(aUID.IsValid());
    EXPECT_EQ(aGraph.UIDs().RefIdFrom(aUID), aRefId);
    EXPECT_EQ(anEntry.RefId, aRefId);
    EXPECT_EQ(anEntry.ParentId.NodeKind, BRepGraph_NodeId::Kind::Wire);
    EXPECT_TRUE(anEntry.CoEdgeDefId.IsValid(aGraph.Topo().CoEdges().Nb()));
  }

  const int aNbShellRefs = aGraph.Refs().Shells().Nb();
  for (BRepGraph_ShellRefId aShellRefId(0); aShellRefId.IsValid(aNbShellRefs); ++aShellRefId)
  {
    const BRepGraph_RefId         aRefId  = aShellRefId;
    const BRepGraph_RefUID        aUID    = aGraph.UIDs().Of(aRefId);
    const BRepGraphInc::ShellRef& anEntry = aGraph.Refs().Shells().Entry(aShellRefId);
    EXPECT_TRUE(aUID.IsValid());
    EXPECT_EQ(aGraph.UIDs().RefIdFrom(aUID), aRefId);
    EXPECT_EQ(anEntry.RefId, aRefId);
    EXPECT_EQ(anEntry.ParentId.NodeKind, BRepGraph_NodeId::Kind::Solid);
    EXPECT_TRUE(anEntry.ShellDefId.IsValid(aGraph.Topo().Shells().Nb()));
  }

  const int aNbVertexRefs = aGraph.Refs().Vertices().Nb();
  for (BRepGraph_VertexRefId aVertexRefId(0); aVertexRefId.IsValid(aNbVertexRefs); ++aVertexRefId)
  {
    const BRepGraph_RefId          aRefId  = aVertexRefId;
    const BRepGraph_RefUID         aUID    = aGraph.UIDs().Of(aRefId);
    const BRepGraphInc::VertexRef& anEntry = aGraph.Refs().Vertices().Entry(aVertexRefId);
    EXPECT_TRUE(aUID.IsValid());
    EXPECT_EQ(aGraph.UIDs().RefIdFrom(aUID), aRefId);
    EXPECT_EQ(anEntry.RefId, aRefId);
    EXPECT_TRUE(anEntry.ParentId.NodeKind == BRepGraph_NodeId::Kind::Edge
                || anEntry.ParentId.NodeKind == BRepGraph_NodeId::Kind::Face);
    EXPECT_TRUE(anEntry.VertexDefId.IsValid(aGraph.Topo().Vertices().Nb()));
  }

  const int aNbSolidRefs = aGraph.Refs().Solids().Nb();
  for (BRepGraph_SolidRefId aSolidRefId(0); aSolidRefId.IsValid(aNbSolidRefs); ++aSolidRefId)
  {
    const BRepGraph_RefId         aRefId  = aSolidRefId;
    const BRepGraph_RefUID        aUID    = aGraph.UIDs().Of(aRefId);
    const BRepGraphInc::SolidRef& anEntry = aGraph.Refs().Solids().Entry(aSolidRefId);
    EXPECT_TRUE(aUID.IsValid());
    EXPECT_EQ(aGraph.UIDs().RefIdFrom(aUID), aRefId);
    EXPECT_EQ(anEntry.RefId, aRefId);
    EXPECT_EQ(anEntry.ParentId.NodeKind, BRepGraph_NodeId::Kind::CompSolid);
    EXPECT_TRUE(anEntry.SolidDefId.IsValid(aGraph.Topo().Solids().Nb()));
  }

  const int aNbChildRefs = aGraph.Refs().Children().Nb();
  for (BRepGraph_ChildRefId aChildRefId(0); aChildRefId.IsValid(aNbChildRefs); ++aChildRefId)
  {
    const BRepGraph_RefId         aRefId  = aChildRefId;
    const BRepGraph_RefUID        aUID    = aGraph.UIDs().Of(aRefId);
    const BRepGraphInc::ChildRef& anEntry = aGraph.Refs().Children().Entry(aChildRefId);
    EXPECT_TRUE(aUID.IsValid());
    EXPECT_EQ(aGraph.UIDs().RefIdFrom(aUID), aRefId);
    EXPECT_EQ(anEntry.RefId, aRefId);
    EXPECT_TRUE(anEntry.ParentId.NodeKind == BRepGraph_NodeId::Kind::Compound
                || anEntry.ParentId.NodeKind == BRepGraph_NodeId::Kind::Shell
                || anEntry.ParentId.NodeKind == BRepGraph_NodeId::Kind::Solid);
    EXPECT_TRUE(anEntry.ChildDefId.IsValid());
  }
}

TEST(BRepGraph_RefIdTest, RefUIDReverseLookupStaysCurrentAfterProgrammaticAdd)
{
  BRepGraph aGraph;

  const BRepGraph_VertexId aV0 = aGraph.Builder().AddVertex(gp_Pnt(0.0, 0.0, 0.0), 0.001);
  const BRepGraph_VertexId aV1 = aGraph.Builder().AddVertex(gp_Pnt(1.0, 0.0, 0.0), 0.001);
  (void)aGraph.Builder().AddEdge(aV0, aV1, occ::handle<Geom_Curve>(), 0.0, 1.0, 0.001);

  const BRepGraph_RefId  aFirstRefId = BRepGraph_VertexRefId(0);
  const BRepGraph_RefUID aFirstUID   = aGraph.UIDs().Of(aFirstRefId);
  ASSERT_TRUE(aFirstUID.IsValid());
  ASSERT_EQ(aGraph.UIDs().RefIdFrom(aFirstUID), aFirstRefId);

  const BRepGraph_VertexId aV2 = aGraph.Builder().AddVertex(gp_Pnt(2.0, 0.0, 0.0), 0.001);
  const BRepGraph_VertexId aV3 = aGraph.Builder().AddVertex(gp_Pnt(3.0, 0.0, 0.0), 0.001);
  (void)aGraph.Builder().AddEdge(aV2, aV3, occ::handle<Geom_Curve>(), 0.0, 1.0, 0.001);

  const BRepGraph_RefId  aSecondRefId = BRepGraph_VertexRefId(2);
  const BRepGraph_RefUID aSecondUID   = aGraph.UIDs().Of(aSecondRefId);
  ASSERT_TRUE(aSecondUID.IsValid());

  EXPECT_EQ(aGraph.UIDs().RefIdFrom(aFirstUID), aFirstRefId);
  EXPECT_EQ(aGraph.UIDs().RefIdFrom(aSecondUID), aSecondRefId);
  EXPECT_TRUE(aGraph.UIDs().Has(aFirstUID));
  EXPECT_TRUE(aGraph.UIDs().Has(aSecondUID));
}

TEST(BRepGraph_RefIdTest, StaleRefUID_HasReturnsFalseAndLookupBecomesInvalidAfterRebuild)
{
  BRepPrimAPI_MakeBox aBoxMaker1(10.0, 20.0, 30.0);
  BRepPrimAPI_MakeBox aBoxMaker2(11.0, 21.0, 31.0);

  BRepGraph aGraph;
  aGraph.Build(aBoxMaker1.Shape());
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Refs().Faces().Nb(), 0);

  const BRepGraph_RefUID anOldUID = aGraph.UIDs().Of(BRepGraph_FaceRefId(0));
  ASSERT_TRUE(anOldUID.IsValid());
  ASSERT_TRUE(aGraph.UIDs().Has(anOldUID));

  aGraph.Build(aBoxMaker2.Shape());

  EXPECT_FALSE(aGraph.UIDs().Has(anOldUID));
  EXPECT_FALSE(aGraph.UIDs().RefIdFrom(anOldUID).IsValid());
}

TEST(BRepGraph_RefIdTest, MutFaceRef_UpdatesRefStampAndParentModifiedFlag)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  if (aGraph.Refs().Faces().Nb() <= 0)
  {
    GTEST_SKIP() << "No face references available";
  }

  const BRepGraph_FaceRefId    aFaceRefId(0);
  const BRepGraphInc::FaceRef& aBeforeEntry = aGraph.Refs().Faces().Entry(aFaceRefId);
  const BRepGraph_VersionStamp aBeforeStamp = aGraph.UIDs().StampOf(aFaceRefId);
  ASSERT_TRUE(aBeforeEntry.ParentId.IsValid());
  ASSERT_TRUE(aBeforeStamp.IsValid());
  ASSERT_TRUE(aBeforeStamp.IsRefStamp());
  const uint32_t           aBeforeOwnGen = aBeforeEntry.OwnGen;
  const TopAbs_Orientation anBeforeOri   = aBeforeEntry.Orientation;

  {
    BRepGraph_MutGuard<BRepGraphInc::FaceRef> aMut = aGraph.Builder().MutFaceRef(aFaceRefId);
    aMut->Orientation = (anBeforeOri == TopAbs_FORWARD) ? TopAbs_REVERSED : TopAbs_FORWARD;
  }

  const BRepGraphInc::FaceRef& aAfterEntry = aGraph.Refs().Faces().Entry(aFaceRefId);
  EXPECT_NE(aAfterEntry.Orientation, anBeforeOri);
  EXPECT_GT(aAfterEntry.OwnGen, aBeforeOwnGen);
  EXPECT_TRUE(aGraph.UIDs().IsStale(aBeforeStamp));

  const BRepGraphInc::BaseDef* aParentDef = aGraph.Topo().Gen().TopoEntity(aAfterEntry.ParentId);
  ASSERT_NE(aParentDef, nullptr);
  EXPECT_GT(aParentDef->SubtreeGen, 0u);
}

TEST(BRepGraph_RefIdTest, MutFaceRef_MarkRemoved_PersistsAndInvalidatesStamp)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  if (aGraph.Refs().Faces().Nb() <= 0)
  {
    GTEST_SKIP() << "No face references available";
  }

  const BRepGraph_FaceRefId    aFaceRefId(0);
  const BRepGraph_VersionStamp aBeforeStamp = aGraph.UIDs().StampOf(aFaceRefId);
  ASSERT_TRUE(aBeforeStamp.IsValid());
  ASSERT_TRUE(aBeforeStamp.IsRefStamp());

  {
    BRepGraph_MutGuard<BRepGraphInc::FaceRef> aMut = aGraph.Builder().MutFaceRef(aFaceRefId);
    aMut->IsRemoved                                = true;
  }

  const BRepGraphInc::FaceRef& aAfterEntry = aGraph.Refs().Faces().Entry(aFaceRefId);
  EXPECT_TRUE(aAfterEntry.IsRemoved);
  EXPECT_TRUE(aGraph.UIDs().IsStale(aBeforeStamp));
  EXPECT_FALSE(aGraph.UIDs().StampOf(aFaceRefId).IsValid());
}

TEST(BRepGraph_RefIdTest, ChildRefs_CompoundEntriesAreValid)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph::RefsView& aRefs = aGraph.Refs();

  for (BRepGraph_CompoundIterator aCompIt(aGraph); aCompIt.More(); aCompIt.Next())
  {
    const NCollection_Vector<BRepGraph_ChildRefId> aChildRefs =
      BRepGraph_TestTools::ChildRefsOfParent(aGraph, aCompIt.CurrentId());
    for (const BRepGraph_ChildRefId& aChildRefId : aChildRefs)
    {
      const BRepGraphInc::ChildRef& aRef = aRefs.Children().Entry(aChildRefId);
      EXPECT_EQ(aRef.ParentId, aCompIt.CurrentId());
      EXPECT_TRUE(aRef.ChildDefId.IsValid());
      EXPECT_FALSE(aRef.IsRemoved);
    }
  }
}
