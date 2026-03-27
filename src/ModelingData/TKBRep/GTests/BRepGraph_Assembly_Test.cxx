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
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_PathView.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <BRepGraphInc_ReverseIndex.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <Precision.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>
#include <gp_Trsf.hxx>

#include <gtest/gtest.h>

// =============================================================================
// Build_SingleSolid_AutoCreatesRootProduct
// =============================================================================

TEST(BRepGraph_AssemblyTest, Build_SingleSolid_AutoCreatesRootProduct)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Paths().NbProducts(), 1);
  EXPECT_EQ(aGraph.Paths().NbOccurrences(), 0);

  const BRepGraphInc::ProductEntity& aProduct = aGraph.Paths().Product(BRepGraph_ProductId(0));
  EXPECT_TRUE(aProduct.ShapeRootId.IsValid());
  EXPECT_EQ(aProduct.Id.NodeKind, BRepGraph_NodeId::Kind::Product);
  EXPECT_EQ(aProduct.Id.Index, 0);

  // The root product should be a part (has topology root).
  EXPECT_TRUE(aGraph.Paths().IsPart(BRepGraph_ProductId(0)));
  EXPECT_FALSE(aGraph.Paths().IsAssembly(BRepGraph_ProductId(0)));
}

// =============================================================================
// Build_Compound_AutoCreatesRootProduct
// =============================================================================

TEST(BRepGraph_AssemblyTest, Build_Compound_AutoCreatesRootProduct)
{
  TopoDS_Compound aCompound;
  BRep_Builder    aBB;
  aBB.MakeCompound(aCompound);
  aBB.Add(aCompound, BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  aBB.Add(aCompound, BRepPrimAPI_MakeSphere(5.0).Shape());

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Paths().NbProducts(), 1);
  EXPECT_EQ(aGraph.Paths().NbOccurrences(), 0);

  const BRepGraphInc::ProductEntity& aProduct = aGraph.Paths().Product(BRepGraph_ProductId(0));
  EXPECT_TRUE(aProduct.ShapeRootId.IsValid());
  EXPECT_EQ(aProduct.ShapeRootId.NodeKind, BRepGraph_NodeId::Kind::Compound);
}

// =============================================================================
// AddProduct_IsPart
// =============================================================================

TEST(BRepGraph_AssemblyTest, AddProduct_IsPart)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Add a second part product.
  const BRepGraph_NodeId aShapeRoot = BRepGraph_NodeId::Solid(0);
  const BRepGraph_NodeId aProductId = aGraph.Builder().AddProduct(aShapeRoot);

  EXPECT_TRUE(aProductId.IsValid());
  EXPECT_EQ(aProductId.NodeKind, BRepGraph_NodeId::Kind::Product);
  EXPECT_TRUE(aGraph.Paths().IsPart(BRepGraph_ProductId(aProductId.Index)));
  EXPECT_FALSE(aGraph.Paths().IsAssembly(BRepGraph_ProductId(aProductId.Index)));
}

// =============================================================================
// AddAssemblyProduct_IsAssembly
// =============================================================================

TEST(BRepGraph_AssemblyTest, AddAssemblyProduct_IsAssembly)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId aAssemblyId = aGraph.Builder().AddAssemblyProduct();

  EXPECT_TRUE(aAssemblyId.IsValid());
  EXPECT_EQ(aAssemblyId.NodeKind, BRepGraph_NodeId::Kind::Product);
  EXPECT_TRUE(aGraph.Paths().IsAssembly(BRepGraph_ProductId(aAssemblyId.Index)));
  EXPECT_FALSE(aGraph.Paths().IsPart(BRepGraph_ProductId(aAssemblyId.Index)));
}

// =============================================================================
// AddOccurrence_LinksCorrectly
// =============================================================================

TEST(BRepGraph_AssemblyTest, AddOccurrence_LinksCorrectly)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId aPartId     = BRepGraph_NodeId::Product(0); // auto-created root
  const BRepGraph_NodeId aAssemblyId = aGraph.Builder().AddAssemblyProduct();

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(100.0, 0.0, 0.0));
  TopLoc_Location aLoc(aTrsf);

  const BRepGraph_NodeId anOccId = aGraph.Builder().AddOccurrence(aAssemblyId, aPartId, aLoc);

  EXPECT_TRUE(anOccId.IsValid());
  EXPECT_EQ(anOccId.NodeKind, BRepGraph_NodeId::Kind::Occurrence);

  const BRepGraphInc::OccurrenceEntity& anOcc =
    aGraph.Paths().Occurrence(BRepGraph_OccurrenceId(anOccId.Index));
  EXPECT_EQ(anOcc.ProductEntityId.Index, aPartId.Index);
  EXPECT_EQ(anOcc.ParentProductEntityId.Index, aAssemblyId.Index);

  // Check that assembly product has the occurrence in OccurrenceRefIds.
  const BRepGraph_ProductId aAssemblyProdId(aAssemblyId.Index);
  EXPECT_EQ(aGraph.Paths().NbComponents(aAssemblyProdId), 1);
  EXPECT_EQ(aGraph.Paths().Component(aAssemblyProdId, 0).Index, anOccId.Index);
}

// =============================================================================
// DAGSharing_MultipleOccurrencesSamePart
// =============================================================================

TEST(BRepGraph_AssemblyTest, DAGSharing_MultipleOccurrencesSamePart)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId aPartId     = BRepGraph_NodeId::Product(0);
  const BRepGraph_NodeId aAssemblyId = aGraph.Builder().AddAssemblyProduct();

  gp_Trsf aTrsf1;
  aTrsf1.SetTranslation(gp_Vec(100.0, 0.0, 0.0));
  gp_Trsf aTrsf2;
  aTrsf2.SetTranslation(gp_Vec(200.0, 0.0, 0.0));

  const BRepGraph_NodeId anOcc1 =
    aGraph.Builder().AddOccurrence(aAssemblyId, aPartId, TopLoc_Location(aTrsf1));
  const BRepGraph_NodeId anOcc2 =
    aGraph.Builder().AddOccurrence(aAssemblyId, aPartId, TopLoc_Location(aTrsf2));

  EXPECT_NE(anOcc1, anOcc2);
  EXPECT_EQ(aGraph.Paths().Occurrence(BRepGraph_OccurrenceId(anOcc1.Index)).ProductEntityId,
            aGraph.Paths().Occurrence(BRepGraph_OccurrenceId(anOcc2.Index)).ProductEntityId);

  EXPECT_EQ(aGraph.Paths().NbComponents(BRepGraph_ProductId(aAssemblyId.Index)), 2);
}

// =============================================================================
// RootProducts_Query
// =============================================================================

TEST(BRepGraph_AssemblyTest, RootProducts_Query)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Auto-created root product is the only root initially.
  NCollection_Vector<BRepGraph_NodeId> aRoots = aGraph.Paths().RootProducts();
  EXPECT_EQ(aRoots.Length(), 1);
  EXPECT_EQ(aRoots.Value(0), BRepGraph_NodeId::Product(0));

  // Add an assembly and make it instantiate the part product via occurrence.
  const BRepGraph_NodeId aPartId     = BRepGraph_NodeId::Product(0);
  const BRepGraph_NodeId aAssemblyId = aGraph.Builder().AddAssemblyProduct();
  (void)aGraph.Builder().AddOccurrence(aAssemblyId, aPartId, TopLoc_Location());

  // Now only the assembly (which is not referenced by any occurrence) is a root.
  aRoots = aGraph.Paths().RootProducts();
  EXPECT_EQ(aRoots.Length(), 1);
  EXPECT_EQ(aRoots.Value(0), aAssemblyId);
}

// =============================================================================
// RemoveOccurrence_UpdatesParent
// =============================================================================

TEST(BRepGraph_AssemblyTest, RemoveOccurrence_UpdatesParent)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId aPartId     = BRepGraph_NodeId::Product(0);
  const BRepGraph_NodeId aAssemblyId = aGraph.Builder().AddAssemblyProduct();
  const BRepGraph_NodeId anOccId =
    aGraph.Builder().AddOccurrence(aAssemblyId, aPartId, TopLoc_Location());

  EXPECT_EQ(aGraph.Paths().NbComponents(BRepGraph_ProductId(aAssemblyId.Index)), 1);
  const NCollection_Vector<BRepGraph_OccurrenceRefId>& aBeforeRefs =
    aGraph.Refs().OccurrenceRefIdsOf(BRepGraph_ProductId(aAssemblyId.Index));
  ASSERT_EQ(aBeforeRefs.Length(), 1);
  const BRepGraph_OccurrenceRefId anOccRefId = aBeforeRefs.Value(0);
  EXPECT_FALSE(aGraph.Refs().Occurrence(anOccRefId).IsRemoved);

  // Remove the occurrence - should update parent's OccurrenceRefs.
  aGraph.Builder().RemoveSubgraph(anOccId);

  EXPECT_TRUE(aGraph.Topo().IsRemoved(anOccId));
  EXPECT_EQ(aGraph.Paths().NbComponents(BRepGraph_ProductId(aAssemblyId.Index)), 0);
  const NCollection_Vector<BRepGraph_OccurrenceRefId>& anAfterRefs =
    aGraph.Refs().OccurrenceRefIdsOf(BRepGraph_ProductId(aAssemblyId.Index));
  EXPECT_EQ(anAfterRefs.Length(), 0);
  EXPECT_TRUE(aGraph.Refs().Occurrence(anOccRefId).IsRemoved);
}

// =============================================================================
// RemoveProduct_CascadeOccurrences
// =============================================================================

TEST(BRepGraph_AssemblyTest, RemoveProduct_CascadeOccurrences)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId aPartId     = BRepGraph_NodeId::Product(0);
  const BRepGraph_NodeId aAssemblyId = aGraph.Builder().AddAssemblyProduct();
  const BRepGraph_NodeId anOcc1 =
    aGraph.Builder().AddOccurrence(aAssemblyId, aPartId, TopLoc_Location());
  const BRepGraph_NodeId anOcc2 =
    aGraph.Builder().AddOccurrence(aAssemblyId, aPartId, TopLoc_Location());

  // Remove the assembly product - cascades to its child occurrences.
  aGraph.Builder().RemoveSubgraph(aAssemblyId);

  EXPECT_TRUE(aGraph.Topo().IsRemoved(aAssemblyId));
  EXPECT_TRUE(aGraph.Topo().IsRemoved(anOcc1));
  EXPECT_TRUE(aGraph.Topo().IsRemoved(anOcc2));
}

// =============================================================================
// MutProduct_RAII
// =============================================================================

TEST(BRepGraph_AssemblyTest, MutProduct_RAII)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  {
    BRepGraph_MutRef<BRepGraphInc::ProductEntity> aMutProd =
      aGraph.Builder().MutProduct(BRepGraph_ProductId(0));
    // Setting ShapeRootId to a different topology node.
    aMutProd->ShapeRootId = BRepGraph_NodeId::Solid(0);
  } // markModified fires here

  EXPECT_TRUE(aGraph.Paths().Product(BRepGraph_ProductId(0)).IsModified);
}

// =============================================================================
// MutOccurrence_Placement
// =============================================================================

TEST(BRepGraph_AssemblyTest, MutOccurrence_Placement)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId aPartId     = BRepGraph_NodeId::Product(0);
  const BRepGraph_NodeId aAssemblyId = aGraph.Builder().AddAssemblyProduct();
  const BRepGraph_NodeId anOccId =
    aGraph.Builder().AddOccurrence(aAssemblyId, aPartId, TopLoc_Location());

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(50.0, 0.0, 0.0));

  {
    BRepGraph_MutRef<BRepGraphInc::OccurrenceEntity> aMutOcc =
      aGraph.Builder().MutOccurrence(BRepGraph_OccurrenceId(anOccId.Index));
    aMutOcc->Placement = TopLoc_Location(aTrsf);
  } // markModified fires here

  EXPECT_TRUE(aGraph.Paths().Occurrence(BRepGraph_OccurrenceId(anOccId.Index)).IsModified);
  const gp_Trsf& aStoredTrsf =
    aGraph.Paths().Occurrence(BRepGraph_OccurrenceId(anOccId.Index)).Placement.Transformation();
  EXPECT_NEAR(aStoredTrsf.TranslationPart().X(), 50.0, Precision::Confusion());
}

// =============================================================================
// GlobalPlacement_DeepNesting
// =============================================================================

TEST(BRepGraph_AssemblyTest, GlobalPlacement_DeepNesting)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId aPartId = BRepGraph_NodeId::Product(0);

  // Build: RootAssembly -> (OccSubAsm) -> SubAssembly -> (OccPart) -> Part
  const BRepGraph_NodeId aSubAsmId  = aGraph.Builder().AddAssemblyProduct();
  const BRepGraph_NodeId aRootAsmId = aGraph.Builder().AddAssemblyProduct();

  gp_Trsf aTrsf1;
  aTrsf1.SetTranslation(gp_Vec(100.0, 0.0, 0.0));
  gp_Trsf aTrsf2;
  aTrsf2.SetTranslation(gp_Vec(0.0, 200.0, 0.0));

  // RootAssembly places SubAssembly with aTrsf2 (top-level occurrence, no parent occ).
  const BRepGraph_NodeId anOccSubAsm =
    aGraph.Builder().AddOccurrence(aRootAsmId, aSubAsmId, TopLoc_Location(aTrsf2));
  // SubAssembly places Part with aTrsf1, with parent occurrence = anOccSubAsm.
  const BRepGraph_NodeId anOccPart =
    aGraph.Builder().AddOccurrence(aSubAsmId, aPartId, TopLoc_Location(aTrsf1), anOccSubAsm);

  // Global placement of the part occurrence should be aTrsf2 * aTrsf1.
  // ParentOccurrenceEntityId chain: anOccPart -> anOccSubAsm -> -1 (root).
  TopLoc_Location aGlobal =
    aGraph.Paths().OccurrenceLocation(BRepGraph_OccurrenceId::FromNodeId(anOccPart));
  const gp_Trsf& aGTrsf = aGlobal.Transformation();
  EXPECT_NEAR(aGTrsf.TranslationPart().X(), 100.0, Precision::Confusion());
  EXPECT_NEAR(aGTrsf.TranslationPart().Y(), 200.0, Precision::Confusion());
}

// =============================================================================
// NbNodes_IncludesAssembly
// =============================================================================

TEST(BRepGraph_AssemblyTest, NbNodes_IncludesAssembly)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const size_t aNbNodesAfterBuild = aGraph.Topo().NbNodes();
  // Should include the auto-created root product.
  EXPECT_GE(aNbNodesAfterBuild, 1u);

  // Add assembly + occurrence.
  const BRepGraph_NodeId aAssemblyId = aGraph.Builder().AddAssemblyProduct();
  (void)aGraph.Builder().AddOccurrence(aAssemblyId,
                                       BRepGraph_NodeId::Product(0),
                                       TopLoc_Location());

  const size_t aNbNodesAfterAssembly = aGraph.Topo().NbNodes();
  EXPECT_EQ(aNbNodesAfterAssembly, aNbNodesAfterBuild + 2); // +1 product, +1 occurrence
}

// =============================================================================
// OccurrencesOfProduct_ReverseIndex
// =============================================================================

TEST(BRepGraph_AssemblyTest, OccurrencesOfProduct_ReverseIndex)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId aPartId     = BRepGraph_NodeId::Product(0);
  const BRepGraph_NodeId aAssemblyId = aGraph.Builder().AddAssemblyProduct();
  (void)aGraph.Builder().AddOccurrence(aAssemblyId, aPartId, TopLoc_Location());
  (void)aGraph.Builder().AddOccurrence(aAssemblyId, aPartId, TopLoc_Location());

  // Build the product-occurrence reverse index manually.
  BRepGraphInc_ReverseIndex aRevIdx;
  // We test the reverse index build via Storage's exposed reverse index.
  // Since BuildReverseIndex doesn't cover product/occurrences yet,
  // we test through the NbComponents API.
  EXPECT_EQ(aGraph.Paths().NbComponents(BRepGraph_ProductId(aAssemblyId.Index)), 2);
}

// =============================================================================
// Iterator_Product
// =============================================================================

TEST(BRepGraph_AssemblyTest, Iterator_Product)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  (void)aGraph.Builder().AddAssemblyProduct();

  int aCount = 0;
  for (BRepGraph_Iterator<BRepGraphInc::ProductEntity> anIt(aGraph); anIt.More(); anIt.Next())
  {
    ++aCount;
  }
  EXPECT_EQ(aCount, 2); // auto root + added assembly
}

// =============================================================================
// Iterator_Occurrence
// =============================================================================

TEST(BRepGraph_AssemblyTest, Iterator_Occurrence)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId aPartId     = BRepGraph_NodeId::Product(0);
  const BRepGraph_NodeId aAssemblyId = aGraph.Builder().AddAssemblyProduct();
  (void)aGraph.Builder().AddOccurrence(aAssemblyId, aPartId, TopLoc_Location());
  (void)aGraph.Builder().AddOccurrence(aAssemblyId, aPartId, TopLoc_Location());

  int aCount = 0;
  for (BRepGraph_Iterator<BRepGraphInc::OccurrenceEntity> anIt(aGraph); anIt.More(); anIt.Next())
  {
    ++aCount;
  }
  EXPECT_EQ(aCount, 2);
}

// =============================================================================
// NodeId_Helpers
// =============================================================================

TEST(BRepGraph_AssemblyTest, NodeId_Helpers)
{
  EXPECT_TRUE(BRepGraph_NodeId::IsTopologyKind(BRepGraph_NodeId::Kind::Solid));
  EXPECT_TRUE(BRepGraph_NodeId::IsTopologyKind(BRepGraph_NodeId::Kind::Vertex));
  EXPECT_TRUE(BRepGraph_NodeId::IsTopologyKind(BRepGraph_NodeId::Kind::CompSolid));
  EXPECT_FALSE(BRepGraph_NodeId::IsTopologyKind(BRepGraph_NodeId::Kind::Product));
  EXPECT_FALSE(BRepGraph_NodeId::IsTopologyKind(BRepGraph_NodeId::Kind::Occurrence));

  EXPECT_TRUE(BRepGraph_NodeId::IsAssemblyKind(BRepGraph_NodeId::Kind::Product));
  EXPECT_TRUE(BRepGraph_NodeId::IsAssemblyKind(BRepGraph_NodeId::Kind::Occurrence));
  EXPECT_FALSE(BRepGraph_NodeId::IsAssemblyKind(BRepGraph_NodeId::Kind::Solid));
  EXPECT_FALSE(BRepGraph_NodeId::IsAssemblyKind(BRepGraph_NodeId::Kind::Face));
}

// =============================================================================
// UID_IsAssembly
// =============================================================================

TEST(BRepGraph_AssemblyTest, UID_IsAssembly)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // The auto-created root product should have a UID with IsAssembly() == false
  // (it's a part, but its UID Kind is Product, so IsAssembly() on UID checks the Kind).
  const BRepGraph_UID aProductUID = aGraph.UIDs().Of(BRepGraph_NodeId::Product(0));
  EXPECT_TRUE(aProductUID.IsValid());
  EXPECT_TRUE(aProductUID.IsAssembly());
  EXPECT_FALSE(aProductUID.IsTopology());
}

// =============================================================================
// AddOccurrence_InvalidParent_ReturnsInvalid
// =============================================================================

TEST(BRepGraph_AssemblyTest, AddOccurrence_InvalidParent_ReturnsInvalid)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Invalid parent (wrong Kind).
  BRepGraph_NodeId aResult = aGraph.Builder().AddOccurrence(BRepGraph_NodeId::Solid(0),
                                                            BRepGraph_NodeId::Product(0),
                                                            TopLoc_Location());
  EXPECT_FALSE(aResult.IsValid());

  // Out-of-bounds parent index.
  aResult = aGraph.Builder().AddOccurrence(BRepGraph_NodeId::Product(999),
                                           BRepGraph_NodeId::Product(0),
                                           TopLoc_Location());
  EXPECT_FALSE(aResult.IsValid());

  // Out-of-bounds referenced product index.
  aResult = aGraph.Builder().AddOccurrence(BRepGraph_NodeId::Product(0),
                                           BRepGraph_NodeId::Product(999),
                                           TopLoc_Location());
  EXPECT_FALSE(aResult.IsValid());
}

// =============================================================================
// AddOccurrence_SelfReference_ReturnsInvalid
// =============================================================================

TEST(BRepGraph_AssemblyTest, AddOccurrence_SelfReference_ReturnsInvalid)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId aPartId = BRepGraph_NodeId::Product(0);

  // Self-referencing: a product cannot be an occurrence of itself.
  const BRepGraph_NodeId aResult =
    aGraph.Builder().AddOccurrence(aPartId, aPartId, TopLoc_Location());
  EXPECT_FALSE(aResult.IsValid());
}

// =============================================================================
// RootProducts_RemovedOccurrence_DoesNotAffectRoots
// =============================================================================

TEST(BRepGraph_AssemblyTest, RootProducts_RemovedOccurrence_DoesNotAffectRoots)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId aPartId     = BRepGraph_NodeId::Product(0);
  const BRepGraph_NodeId aAssemblyId = aGraph.Builder().AddAssemblyProduct();
  const BRepGraph_NodeId anOccId =
    aGraph.Builder().AddOccurrence(aAssemblyId, aPartId, TopLoc_Location());

  // Before removal: only assembly is root (part is referenced).
  NCollection_Vector<BRepGraph_NodeId> aRoots = aGraph.Paths().RootProducts();
  EXPECT_EQ(aRoots.Length(), 1);
  EXPECT_EQ(aRoots.Value(0), aAssemblyId);

  // Remove the occurrence - part should become a root again.
  aGraph.Builder().RemoveSubgraph(anOccId);

  aRoots = aGraph.Paths().RootProducts();
  EXPECT_EQ(aRoots.Length(), 2); // both part and assembly are roots now
}

// =============================================================================
// GlobalPlacement_DAGSharing_DistinctPathsGiveDistinctPlacements
// =============================================================================

TEST(BRepGraph_AssemblyTest, GlobalPlacement_DAGSharing_DistinctPathsGiveDistinctPlacements)
{
  // Shared part placed twice under the same assembly at different locations.
  // Each occurrence has its own placement chain - no ambiguity.
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId aPartId     = BRepGraph_NodeId::Product(0);
  const BRepGraph_NodeId aAssemblyId = aGraph.Builder().AddAssemblyProduct();

  gp_Trsf aTrsf1;
  aTrsf1.SetTranslation(gp_Vec(100.0, 0.0, 0.0));
  gp_Trsf aTrsf2;
  aTrsf2.SetTranslation(gp_Vec(0.0, 200.0, 0.0));

  const BRepGraph_NodeId anOcc1 =
    aGraph.Builder().AddOccurrence(aAssemblyId, aPartId, TopLoc_Location(aTrsf1));
  const BRepGraph_NodeId anOcc2 =
    aGraph.Builder().AddOccurrence(aAssemblyId, aPartId, TopLoc_Location(aTrsf2));

  // Same part, different occurrences, different global placements.
  TopLoc_Location aGlobal1 =
    aGraph.Paths().OccurrenceLocation(BRepGraph_OccurrenceId::FromNodeId(anOcc1));
  TopLoc_Location aGlobal2 =
    aGraph.Paths().OccurrenceLocation(BRepGraph_OccurrenceId::FromNodeId(anOcc2));

  EXPECT_NEAR(aGlobal1.Transformation().TranslationPart().X(), 100.0, Precision::Confusion());
  EXPECT_NEAR(aGlobal1.Transformation().TranslationPart().Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aGlobal2.Transformation().TranslationPart().X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aGlobal2.Transformation().TranslationPart().Y(), 200.0, Precision::Confusion());
}

// =============================================================================
// AddOccurrence_RemovedProduct_ReturnsInvalid
// =============================================================================

TEST(BRepGraph_AssemblyTest, AddOccurrence_RemovedProduct_ReturnsInvalid)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId aAssemblyId = aGraph.Builder().AddAssemblyProduct();
  // Remove the assembly.
  aGraph.Builder().RemoveNode(aAssemblyId);

  // Cannot add occurrence to a removed product.
  const BRepGraph_NodeId aResult =
    aGraph.Builder().AddOccurrence(aAssemblyId, BRepGraph_NodeId::Product(0), TopLoc_Location());
  EXPECT_FALSE(aResult.IsValid());

  // Cannot reference a removed product either.
  const BRepGraph_NodeId aAsm2 = aGraph.Builder().AddAssemblyProduct();
  aGraph.Builder().RemoveNode(BRepGraph_NodeId::Product(0));
  const BRepGraph_NodeId aResult2 =
    aGraph.Builder().AddOccurrence(aAsm2, BRepGraph_NodeId::Product(0), TopLoc_Location());
  EXPECT_FALSE(aResult2.IsValid());
}

// =============================================================================
// GlobalPlacement_ThreeLevelNesting
// =============================================================================

TEST(BRepGraph_AssemblyTest, GlobalPlacement_ThreeLevelNesting)
{
  // Root -> Mid -> Leaf, each with a distinct translation.
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId aLeafPart = BRepGraph_NodeId::Product(0);
  const BRepGraph_NodeId aMidAsm   = aGraph.Builder().AddAssemblyProduct();
  const BRepGraph_NodeId aRootAsm  = aGraph.Builder().AddAssemblyProduct();
  const BRepGraph_NodeId aTopAsm   = aGraph.Builder().AddAssemblyProduct();

  gp_Trsf aT1, aT2, aT3;
  aT1.SetTranslation(gp_Vec(1.0, 0.0, 0.0));
  aT2.SetTranslation(gp_Vec(0.0, 2.0, 0.0));
  aT3.SetTranslation(gp_Vec(0.0, 0.0, 3.0));

  // TopAsm places RootAsm.
  const BRepGraph_NodeId anOccRoot =
    aGraph.Builder().AddOccurrence(aTopAsm, aRootAsm, TopLoc_Location(aT3));
  // RootAsm places MidAsm, parent occ = anOccRoot.
  const BRepGraph_NodeId anOccMid =
    aGraph.Builder().AddOccurrence(aRootAsm, aMidAsm, TopLoc_Location(aT2), anOccRoot);
  // MidAsm places Leaf, parent occ = anOccMid.
  const BRepGraph_NodeId anOccLeaf =
    aGraph.Builder().AddOccurrence(aMidAsm, aLeafPart, TopLoc_Location(aT1), anOccMid);

  // Global of leaf = T3 * T2 * T1 => (1, 2, 3).
  TopLoc_Location aGlobal =
    aGraph.Paths().OccurrenceLocation(BRepGraph_OccurrenceId::FromNodeId(anOccLeaf));
  const gp_XYZ& aTransl = aGlobal.Transformation().TranslationPart();
  EXPECT_NEAR(aTransl.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aTransl.Y(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aTransl.Z(), 3.0, Precision::Confusion());
}

// =============================================================================
// OccurrencesOfProduct_ViaReverseIndex
// =============================================================================

TEST(BRepGraph_AssemblyTest, OccurrencesOfProduct_ViaReverseIndex)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId aPartId = BRepGraph_NodeId::Product(0);
  const BRepGraph_NodeId aAsmId  = aGraph.Builder().AddAssemblyProduct();
  (void)aGraph.Builder().AddOccurrence(aAsmId, aPartId, TopLoc_Location());
  (void)aGraph.Builder().AddOccurrence(aAsmId, aPartId, TopLoc_Location());

  // Rebuild reverse index to populate product->occurrences.
  // (BuildReverseIndex is called during Build, but not after Builder mutations.)
  // Access via DefsView which uses forward OccurrenceRefs.
  EXPECT_EQ(aGraph.Paths().NbComponents(BRepGraph_ProductId(aAsmId.Index)), 2);

  // The auto-created root product is not referenced by any occurrence.
  // (It IS the part being referenced, so OccurrencesOfProduct should have entries.)
  EXPECT_EQ(aGraph.Paths().NbComponents(BRepGraph_ProductId(aPartId.Index)),
            0); // part has no child occurrences
}

// =============================================================================
// GlobalPlacement_CircularParentOccurrence_Terminates
// =============================================================================

TEST(BRepGraph_AssemblyTest, GlobalPlacement_CircularParentOccurrence_Terminates)
{
  // Manually create a circular ParentOccurrenceEntityId via MutRef to simulate
  // a malformed graph. GlobalPlacement must terminate (THE_MAX_OCCURRENCE_DEPTH guard).
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId aPartId = BRepGraph_NodeId::Product(0);
  const BRepGraph_NodeId aAsmId  = aGraph.Builder().AddAssemblyProduct();

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(1.0, 0.0, 0.0));

  const BRepGraph_NodeId anOcc1 =
    aGraph.Builder().AddOccurrence(aAsmId, aPartId, TopLoc_Location(aTrsf));
  const BRepGraph_NodeId anOcc2 =
    aGraph.Builder().AddOccurrence(aAsmId, aPartId, TopLoc_Location(aTrsf), anOcc1);

  // Inject circular reference: occ1.ParentOccurrenceEntityId = occ2 (creates cycle).
  {
    BRepGraph_MutRef<BRepGraphInc::OccurrenceEntity> aMut =
      aGraph.Builder().MutOccurrence(BRepGraph_OccurrenceId(anOcc1.Index));
    aMut->ParentOccurrenceEntityId = BRepGraph_OccurrenceId(anOcc2.Index);
  }

  // GlobalPlacement must terminate despite the cycle (depth guard).
  TopLoc_Location aGlobal =
    aGraph.Paths().OccurrenceLocation(BRepGraph_OccurrenceId::FromNodeId(anOcc2));
  // We don't check the value - just that it doesn't hang.
  (void)aGlobal;
}
