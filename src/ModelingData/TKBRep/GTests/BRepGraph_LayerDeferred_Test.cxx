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
#include <BRepGraph_LayerDeferred.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_ShapesView.hxx>

#include <BRepPrimAPI_MakeBox.hxx>
#include <gtest/gtest.h>

namespace
{
using Representation     = BRepGraph_LayerDeferred::Representation;
using RepresentationKind = BRepGraph_LayerDeferred::RepresentationKind;

BRepGraph makeBoxGraph()
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  return aGraph;
}

Representation makeRepresentation(const RepresentationKind       theKind,
                                  const TCollection_AsciiString& theName,
                                  const uint32_t                 theSourceIndex,
                                  const uint32_t                 theRole = 0)
{
  Representation aRepresentation;
  aRepresentation.Kind        = theKind;
  aRepresentation.Role        = theRole;
  aRepresentation.Name        = theName;
  aRepresentation.SourceIndex = theSourceIndex;
  return aRepresentation;
}

bool isSameRepresentation(const Representation& theLeft, const Representation& theRight)
{
  return theLeft.Kind == theRight.Kind && theLeft.Role == theRight.Role
         && theLeft.Name == theRight.Name && theLeft.SourceIndex == theRight.SourceIndex;
}

uint32_t countRepresentation(const BRepGraph_LayerDeferred::Entry& theEntry,
                             const Representation&                 theRepresentation)
{
  uint32_t aCount = 0;
  for (size_t anIdx = 0; anIdx < theEntry.Representations.Size(); ++anIdx)
  {
    if (isSameRepresentation(theEntry.Representations.Value(anIdx), theRepresentation))
    {
      ++aCount;
    }
  }
  return aCount;
}
} // namespace

TEST(BRepGraph_LayerDeferredTest, OnNodeReplacedMergesOverlappingRepresentations)
{
  const BRepGraph_FaceId anOldFace = BRepGraph_FaceId::Start();
  const BRepGraph_FaceId aNewFace(1);
  const BRepGraph_NodeId anOldNode(anOldFace);
  const BRepGraph_NodeId aNewNode(aNewFace);
  const BRepGraph_ItemId anOldItem(anOldNode);
  const BRepGraph_ItemId aNewItem(aNewNode);

  occ::handle<BRepGraph_LayerDeferred> aLayer = new BRepGraph_LayerDeferred();

  const Representation aShared =
    makeRepresentation(RepresentationKind::Geometry, "surface", 42);
  const Representation anOldOnly =
    makeRepresentation(RepresentationKind::Mesh, "triangulation", 7);
  const Representation aNewOnly =
    makeRepresentation(RepresentationKind::Topology, "face-topology", 9);

  const Representation anOldReps[] = {aShared, anOldOnly};
  const Representation aNewReps[]  = {aShared, aNewOnly};
  aLayer->RegisterDeferredRepresentations(anOldItem, "Provider", "old-source", anOldReps, 2);
  aLayer->RegisterDeferredRepresentations(aNewItem, "Provider", "new-source", aNewReps, 2);

  ASSERT_TRUE(aLayer->HasDeferred(anOldItem));
  ASSERT_TRUE(aLayer->HasDeferred(aNewItem));

  aLayer->OnNodeReplaced(anOldNode, aNewNode);

  EXPECT_FALSE(aLayer->HasDeferred(anOldItem));

  const BRepGraph_LayerDeferred::Entry* anEntry = aLayer->FindDeferred(aNewItem);
  ASSERT_NE(anEntry, nullptr);
  EXPECT_EQ(anEntry->Provider, TCollection_AsciiString("Provider"));
  EXPECT_EQ(anEntry->SourceKey, TCollection_AsciiString("new-source"));
  EXPECT_EQ(anEntry->Representations.Size(), 3u);
  EXPECT_EQ(countRepresentation(*anEntry, aShared), 1u);
  EXPECT_EQ(countRepresentation(*anEntry, anOldOnly), 1u);
  EXPECT_EQ(countRepresentation(*anEntry, aNewOnly), 1u);
}

TEST(BRepGraph_LayerDeferredTest, NestedBulkRegistrationTouchesOnceAtOuterEnd)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  occ::handle<BRepGraph_LayerDeferred> aLayer =
    aGraph.LayerRegistry().Ensure<BRepGraph_LayerDeferred>();

  const uint64_t aBaseGeneration = aLayer->Generation();
  aLayer->BeginBulkRegistration();
  aLayer->BeginBulkRegistration();
  aLayer->RegisterDeferred(BRepGraph_FaceId::Start(),
                           "Provider",
                           "source",
                           RepresentationKind::Geometry,
                           "surface",
                           42);

  EXPECT_EQ(aLayer->Generation(), aBaseGeneration);
  aLayer->EndBulkRegistration();
  EXPECT_EQ(aLayer->Generation(), aBaseGeneration);
  aLayer->EndBulkRegistration();
  EXPECT_EQ(aLayer->Generation(), aBaseGeneration + 1u);

  const uint64_t aGenerationAfterChange = aLayer->Generation();
  aLayer->BeginBulkRegistration();
  aLayer->BeginBulkRegistration();
  aLayer->EndBulkRegistration();
  aLayer->EndBulkRegistration();
  EXPECT_EQ(aLayer->Generation(), aGenerationAfterChange);
}
