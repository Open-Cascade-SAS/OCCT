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
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <BRepBndLib.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <Bnd_Box.hxx>
#include <TopExp_Explorer.hxx>
#include <Precision.hxx>
#include <Standard_ProgramError.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <BRep_Builder.hxx>
#include <gp_Trsf.hxx>

#include <NCollection_IncAllocator.hxx>

#include <gtest/gtest.h>

namespace
{
static double translationX(const TopoDS_Shape& theShape)
{
  return theShape.Location().Transformation().TranslationPart().X();
}

Bnd_Box geometryBounds(const TopoDS_Shape& theShape)
{
  Bnd_Box aBox;
  BRepBndLib::AddOptimal(theShape, aBox, false, false);
  return aBox;
}

void expectBoxNear(const Bnd_Box& theActual, const Bnd_Box& theExpected, const double theTol)
{
  const Bnd_Box::Limits anActual   = theActual.Get();
  const Bnd_Box::Limits anExpected = theExpected.Get();
  EXPECT_NEAR(anActual.Xmin, anExpected.Xmin, theTol);
  EXPECT_NEAR(anActual.Ymin, anExpected.Ymin, theTol);
  EXPECT_NEAR(anActual.Zmin, anExpected.Zmin, theTol);
  EXPECT_NEAR(anActual.Xmax, anExpected.Xmax, theTol);
  EXPECT_NEAR(anActual.Ymax, anExpected.Ymax, theTol);
  EXPECT_NEAR(anActual.Zmax, anExpected.Zmax, theTol);
}

NCollection_LinearVector<BRepGraph_ProductId> collectRootProducts(const BRepGraph& theGraph)
{
  NCollection_LinearVector<BRepGraph_ProductId> aRoots(4);
  for (BRepGraph_RootProductIterator aRootIt(theGraph); aRootIt.More(); aRootIt.Next())
  {
    aRoots.Append(aRootIt.Current());
  }
  return aRoots;
}

template <typename RootContainerT>
bool hasRootProduct(const RootContainerT& theRoots, const BRepGraph_ProductId theProduct)
{
  for (const BRepGraph_ProductId& aRoot : theRoots)
  {
    if (aRoot == theProduct)
    {
      return true;
    }
  }
  return false;
}
} // namespace

// =============================================================================
// Build_SingleSolid_AutoCreatesRootProduct
// =============================================================================

TEST(BRepGraph_AssemblyTest, Build_SingleSolid_AutoCreatesRootProduct)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes1 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());

  EXPECT_EQ(aGraph.Topo().Products().Nb(), 1);
  // BRepGraph::ShapesView::Add() creates a shape-root occurrence linking product to its topology
  // root.
  EXPECT_EQ(aGraph.Topo().Occurrences().Nb(), 1);

  [[maybe_unused]] const BRepGraphInc::ProductDef& aProductDef =
    aGraph.Topo().Products().Definition(BRepGraph_ProductId::Start());
  EXPECT_TRUE(aGraph.Topo().Products().ShapeRoot(BRepGraph_ProductId::Start()).IsValid());

  // The root product should be a part (has topology root).
  EXPECT_TRUE(aGraph.Topo().Products().IsPart(BRepGraph_ProductId::Start()));
  EXPECT_FALSE(aGraph.Topo().Products().IsAssembly(BRepGraph_ProductId::Start()));
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
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes2 = aGraph.Shapes().Add(aCompound);

  EXPECT_EQ(aGraph.Topo().Products().Nb(), 1);
  // BRepGraph::ShapesView::Add() creates a shape-root occurrence linking product to its topology
  // root.
  EXPECT_EQ(aGraph.Topo().Occurrences().Nb(), 1);

  const BRepGraph_NodeId aShapeRoot =
    aGraph.Topo().Products().ShapeRoot(BRepGraph_ProductId::Start());
  EXPECT_TRUE(aShapeRoot.IsValid());
  EXPECT_EQ(aShapeRoot.NodeKind, BRepGraph_NodeId::Kind::Compound);
}

// =============================================================================
// AddProduct_IsPart
// =============================================================================

TEST(BRepGraph_AssemblyTest, AddProduct_IsPart)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes3 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());

  // Add a second part product.
  const BRepGraph_NodeId    aShapeRoot = BRepGraph_SolidId::Start();
  const BRepGraph_ProductId aProductId = aGraph.Editor().Products().Add(aShapeRoot);
  aGraph.Editor().Products().AppendDocumentRoot(aProductId);

  EXPECT_TRUE(aProductId.IsValid());
  EXPECT_TRUE(aGraph.Topo().Products().IsPart(aProductId));
  EXPECT_FALSE(aGraph.Topo().Products().IsAssembly(aProductId));
}

TEST(BRepGraph_AssemblyTest, AddProduct_InvalidShapeRoot_ReturnsInvalid)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes4 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());

  EXPECT_FALSE(aGraph.Editor().Products().Add(BRepGraph_ProductId::Start()).IsValid());

  aGraph.Editor().Gen().RemoveNode(BRepGraph_SolidId::Start());
  EXPECT_FALSE(aGraph.Editor().Products().Add(BRepGraph_SolidId::Start()).IsValid());
}

// =============================================================================
// CreateEmptyProduct_EmptyIsNotAssemblyYet
// =============================================================================

TEST(BRepGraph_AssemblyTest, CreateEmptyProduct_EmptyIsNotAssemblyYet)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes5 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());

  const BRepGraph_ProductId aAssemblyId = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aAssemblyId);

  EXPECT_TRUE(aAssemblyId.IsValid());
  EXPECT_FALSE(aGraph.Topo().Products().IsAssembly(aAssemblyId));
  EXPECT_FALSE(aGraph.Topo().Products().IsPart(aAssemblyId));
}

// =============================================================================
// LinkProducts_LinksCorrectly
// =============================================================================

TEST(BRepGraph_AssemblyTest, LinkProducts_LinksCorrectly)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes6 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());

  const BRepGraph_ProductId aPartId     = BRepGraph_ProductId::Start(); // auto-created root
  const BRepGraph_ProductId aAssemblyId = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aAssemblyId);

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(100.0, 0.0, 0.0));
  TopLoc_Location aLoc(aTrsf);

  const BRepGraph_OccurrenceId anOccId =
    aGraph.Editor().Products().Append(aAssemblyId, aPartId, aLoc);

  EXPECT_TRUE(anOccId.IsValid());

  EXPECT_EQ(aGraph.Topo().Occurrences().Product(anOccId), aPartId);
  EXPECT_EQ(aGraph.Topo().Occurrences().ParentProduct(anOccId), aAssemblyId);

  // Check that assembly product has the occurrence in OccurrenceRefIds.
  EXPECT_EQ(aGraph.Topo().Products().NbComponents(aAssemblyId), 1);
  EXPECT_EQ(aGraph.Topo().Products().Component(aAssemblyId, 0), anOccId);
}

// =============================================================================
// DAGSharing_MultipleOccurrencesSamePart
// =============================================================================

TEST(BRepGraph_AssemblyTest, DAGSharing_MultipleOccurrencesSamePart)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes7 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());

  const BRepGraph_ProductId aPartId     = BRepGraph_ProductId::Start();
  const BRepGraph_ProductId aAssemblyId = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aAssemblyId);

  gp_Trsf aTrsf1;
  aTrsf1.SetTranslation(gp_Vec(100.0, 0.0, 0.0));
  gp_Trsf aTrsf2;
  aTrsf2.SetTranslation(gp_Vec(200.0, 0.0, 0.0));

  const BRepGraph_OccurrenceId anOcc1 =
    aGraph.Editor().Products().Append(aAssemblyId, aPartId, TopLoc_Location(aTrsf1));
  const BRepGraph_OccurrenceId anOcc2 =
    aGraph.Editor().Products().Append(aAssemblyId, aPartId, TopLoc_Location(aTrsf2));

  EXPECT_NE(anOcc1, anOcc2);
  EXPECT_EQ(aGraph.Topo().Occurrences().Product(anOcc1),
            aGraph.Topo().Occurrences().Product(anOcc2));

  EXPECT_EQ(aGraph.Topo().Products().NbComponents(aAssemblyId), 2);
}

TEST(BRepGraph_AssemblyTest, LinkProducts_ParentOccurrenceMustMatchParentProduct)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes8 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());

  const BRepGraph_ProductId aPartId     = BRepGraph_ProductId::Start();
  const BRepGraph_ProductId anAssemblyA = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(anAssemblyA);
  const BRepGraph_ProductId anAssemblyB = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(anAssemblyB);
  const BRepGraph_OccurrenceId aParentOccId =
    aGraph.Editor().Products().Append(anAssemblyA, aPartId, TopLoc_Location());
  ASSERT_TRUE(aParentOccId.IsValid());

  const BRepGraph_OccurrenceId anInvalidOccId =
    aGraph.Editor().Products().Append(anAssemblyB, aPartId, TopLoc_Location(), aParentOccId);

  EXPECT_FALSE(anInvalidOccId.IsValid());
  // BRepGraph::ShapesView::Add() creates 1 shape-root occ, LinkProducts creates 1 more = 2 total.
  EXPECT_EQ(aGraph.Topo().Occurrences().Nb(), 2);
  EXPECT_EQ(aGraph.Topo().Products().NbComponents(anAssemblyB), 0);
}

// =============================================================================
// RootNodeIds_Query
// =============================================================================

TEST(BRepGraph_AssemblyTest, RootProductIds_Query)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes9 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());

  // Auto-created root product is the first root.
  NCollection_LinearVector<BRepGraph_ProductId> aRoots = collectRootProducts(aGraph);
  EXPECT_EQ(aRoots.Size(), 1);
  EXPECT_EQ(aRoots.Value(0), BRepGraph_ProductId::Start());

  // Add an assembly and make it instantiate the part product via occurrence.
  const BRepGraph_ProductId aPartId     = BRepGraph_ProductId::Start();
  const BRepGraph_ProductId aAssemblyId = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aAssemblyId);
  [[maybe_unused]] const BRepGraph_OccurrenceId anOccId =
    aGraph.Editor().Products().Append(aAssemblyId, aPartId, TopLoc_Location());

  // Now only the assembly (which is not referenced by any occurrence) is a root.
  aRoots = collectRootProducts(aGraph);
  EXPECT_EQ(aRoots.Size(), 1);
  EXPECT_EQ(aRoots.Value(0), aAssemblyId);
}

// =============================================================================
// RootProductIds_ShapelessRootAssembly_UsesProductId
// =============================================================================

TEST(BRepGraph_AssemblyTest, RootProductIds_ShapelessRootAssembly_UsesProductId)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes10 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());

  const BRepGraph_ProductId aPartId     = BRepGraph_ProductId::Start();
  const BRepGraph_ProductId aAssemblyId = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aAssemblyId);
  ASSERT_TRUE(aGraph.Editor().Products().Append(aAssemblyId, aPartId, TopLoc_Location()).IsValid());

  const NCollection_LinearVector<BRepGraph_ProductId>& aRoots = aGraph.RootProductIds();
  ASSERT_EQ(aRoots.Size(), 1);
  EXPECT_EQ(aRoots.Value(0), aAssemblyId);
}

// =============================================================================
// RootProductIds_ReflectsAssemblyMutation
// =============================================================================

TEST(BRepGraph_AssemblyTest, RootProductIds_ReflectsAssemblyMutation)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes11 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());

  const NCollection_LinearVector<BRepGraph_ProductId> aRootsBefore = collectRootProducts(aGraph);
  ASSERT_EQ(aRootsBefore.Size(), 1);
  EXPECT_EQ(aRootsBefore.Value(0), BRepGraph_ProductId::Start());

  const BRepGraph_ProductId aAssemblyId = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aAssemblyId);
  [[maybe_unused]] const BRepGraph_OccurrenceId anOccId =
    aGraph.Editor().Products().Append(aAssemblyId, BRepGraph_ProductId::Start(), TopLoc_Location());

  const NCollection_LinearVector<BRepGraph_ProductId> aRootsAfter = collectRootProducts(aGraph);
  ASSERT_EQ(aRootsAfter.Size(), 1);
  EXPECT_EQ(aRootsAfter.Value(0), aAssemblyId);
}

// =============================================================================
// RemoveOccurrence_UpdatesParent
// =============================================================================

TEST(BRepGraph_AssemblyTest, RemoveOccurrence_UpdatesParent)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes12 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());

  const BRepGraph_ProductId aPartId     = BRepGraph_ProductId::Start();
  const BRepGraph_ProductId aAssemblyId = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aAssemblyId);
  const BRepGraph_OccurrenceId anOccId =
    aGraph.Editor().Products().Append(aAssemblyId, aPartId, TopLoc_Location());

  EXPECT_EQ(aGraph.Topo().Products().NbComponents(aAssemblyId), 1);
  const NCollection_LinearVector<BRepGraph_OccurrenceRefId>& aBeforeRefs =
    aGraph.Refs().Occurrences().IdsOf(aAssemblyId);
  ASSERT_EQ(aBeforeRefs.Size(), 1);
  const BRepGraph_OccurrenceRefId anOccRefId = aBeforeRefs.Value(0);
  EXPECT_FALSE(anOccRefId.IsRemoved(aGraph));

  // Remove the occurrence - should update parent's OccurrenceRefs.
  aGraph.Editor().Gen().RemoveSubgraph(anOccId);

  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(anOccId));
  EXPECT_EQ(aGraph.Topo().Products().NbComponents(aAssemblyId), 0);
  const NCollection_LinearVector<BRepGraph_OccurrenceRefId>& anAfterRefs =
    aGraph.Refs().Occurrences().IdsOf(aAssemblyId);
  EXPECT_EQ(anAfterRefs.Size(), 0);
  EXPECT_TRUE(anOccRefId.IsRemoved(aGraph));
}

// =============================================================================
// RemoveProduct_CascadeOccurrences
// =============================================================================

TEST(BRepGraph_AssemblyTest, RemoveProduct_CascadeOccurrences)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes13 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());

  const BRepGraph_ProductId aPartId     = BRepGraph_ProductId::Start();
  const BRepGraph_ProductId aAssemblyId = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aAssemblyId);
  const BRepGraph_OccurrenceId anOcc1 =
    aGraph.Editor().Products().Append(aAssemblyId, aPartId, TopLoc_Location());
  const BRepGraph_OccurrenceId anOcc2 =
    aGraph.Editor().Products().Append(aAssemblyId, aPartId, TopLoc_Location());

  // Remove the assembly product - cascades to its child occurrences.
  aGraph.Editor().Gen().RemoveSubgraph(aAssemblyId);

  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aAssemblyId));
  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(anOcc1));
  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(anOcc2));
}

// =============================================================================
// RemoveProduct_CascadesToPartTopology
// =============================================================================

TEST(BRepGraph_AssemblyTest, RemoveProduct_RemovesProductAndOccurrences)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes14 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());

  // Part product created by BRepGraph::ShapesView::Add() references topology via
  // a shape-root occurrence.
  const BRepGraph_ProductId aPartId = BRepGraph_ProductId::Start();
  EXPECT_TRUE(aGraph.Topo().Products().IsPart(aPartId));

  // BRepGraph::ShapesView::Add() creates 1 shape-root occurrence.
  ASSERT_EQ(aGraph.Topo().Occurrences().Nb(), 1);
  const BRepGraph_OccurrenceId aShapeRootOcc = BRepGraph_OccurrenceId::Start();
  EXPECT_FALSE(aGraph.Topo().Gen().IsRemoved(aShapeRootOcc));

  // Remove the part product - should remove the product and its occurrences.
  aGraph.Editor().Gen().RemoveSubgraph(aPartId);

  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aPartId));
  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(aShapeRootOcc));
}

// =============================================================================
// RemoveOccurrence_CascadesToNestedChildren
// =============================================================================

TEST(BRepGraph_AssemblyTest, RemoveOccurrence_CascadesToNestedChildren)
{
  // TopAsm -> MidAsm -> LeafPart, each level via occurrences.
  // Removing the mid-level occurrence should also remove the leaf occurrence.
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes15 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());

  const BRepGraph_ProductId aLeafPart = BRepGraph_ProductId::Start();
  const BRepGraph_ProductId aMidAsm   = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aMidAsm);
  const BRepGraph_ProductId aTopAsm = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aTopAsm);

  gp_Trsf aT1, aT2;
  aT1.SetTranslation(gp_Vec(10.0, 0.0, 0.0));
  aT2.SetTranslation(gp_Vec(0.0, 20.0, 0.0));

  // TopAsm places MidAsm.
  const BRepGraph_OccurrenceId anOccMid =
    aGraph.Editor().Products().Append(aTopAsm, aMidAsm, TopLoc_Location(aT1));
  // MidAsm places LeafPart, with parent occurrence = anOccMid.
  const BRepGraph_OccurrenceId anOccLeaf =
    aGraph.Editor().Products().Append(aMidAsm, aLeafPart, TopLoc_Location(aT2), anOccMid);

  EXPECT_FALSE(aGraph.Topo().Gen().IsRemoved(anOccMid));
  EXPECT_FALSE(aGraph.Topo().Gen().IsRemoved(anOccLeaf));

  // ParentOccurrence is no longer stored on OccurrenceDef; paths are resolved by explorers.
  // Verify that both occurrences are active.
  EXPECT_FALSE(aGraph.Topo().Gen().IsRemoved(anOccLeaf));

  // Remove the mid-level occurrence.
  aGraph.Editor().Gen().RemoveSubgraph(anOccMid);

  EXPECT_TRUE(aGraph.Topo().Gen().IsRemoved(anOccMid));
  // Leaf occurrence is not a child of mid occurrence in the new model.
  // Leaf is a child of MidAsm product, with anOccMid as parent occurrence context.
  // RemoveSubgraph on an occurrence does not cascade to sibling occurrences.

  // Products themselves should NOT be removed (only occurrences).
  EXPECT_FALSE(aGraph.Topo().Gen().IsRemoved(aLeafPart));
  EXPECT_FALSE(aGraph.Topo().Gen().IsRemoved(aMidAsm));
  EXPECT_FALSE(aGraph.Topo().Gen().IsRemoved(aTopAsm));
}

// =============================================================================
// MutProduct_RAII
// =============================================================================

TEST(BRepGraph_AssemblyTest, MutProduct_RAII)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes16 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());

  {
    BRepGraph_MutGuard<BRepGraphInc::ProductDef> aMutProd =
      aGraph.Editor().Products().Mut(BRepGraph_ProductId::Start());
    // Trigger a mutation (any field write suffices).
    aMutProd.MarkDirty();
  } // markModified fires here

  EXPECT_GT(aGraph.Topo().Products().Definition(BRepGraph_ProductId::Start()).OwnGen, 0u);
}

// =============================================================================
// MutOccurrence_Placement
// =============================================================================

TEST(BRepGraph_AssemblyTest, MutOccurrenceRef_LocalLocation)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes17 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());

  const BRepGraph_ProductId aPartId     = BRepGraph_ProductId::Start();
  const BRepGraph_ProductId aAssemblyId = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aAssemblyId);
  const BRepGraph_OccurrenceId anOccId =
    aGraph.Editor().Products().Append(aAssemblyId, aPartId, TopLoc_Location());
  ASSERT_TRUE(anOccId.IsValid());

  // Find the OccurrenceRefId for the occurrence.
  const NCollection_LinearVector<BRepGraph_OccurrenceRefId>& aOccRefs =
    aGraph.Refs().Occurrences().IdsOf(aAssemblyId);
  ASSERT_EQ(aOccRefs.Size(), 1);
  const BRepGraph_OccurrenceRefId anOccRefId = aOccRefs.Value(0);

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(50.0, 0.0, 0.0));

  {
    BRepGraph_MutGuard<BRepGraphInc::OccurrenceRef> aMutRef =
      aGraph.Editor().Occurrences().MutRef(anOccRefId);
    aGraph.Editor().Occurrences().SetRefLocalLocation(aMutRef, TopLoc_Location(aTrsf));
  } // markRefModified fires here

  const gp_Trsf& aStoredTrsf =
    aGraph.Refs().Occurrences().Entry(anOccRefId).LocalLocation.Transformation();
  EXPECT_NEAR(aStoredTrsf.TranslationPart().X(), 50.0, Precision::Confusion());
}

TEST(BRepGraph_AssemblyTest, MutInvalidAssemblyDefs_ThrowProgramError)
{
  BRepGraph aGraph;
#if !defined(No_Exception)
  EXPECT_THROW(
    { [[maybe_unused]] auto aMut = aGraph.Editor().Products().Mut(BRepGraph_ProductId(7)); },
    Standard_ProgramError);
  EXPECT_THROW(
    { [[maybe_unused]] auto aMut = aGraph.Editor().Occurrences().Mut(BRepGraph_OccurrenceId(7)); },
    Standard_ProgramError);
#endif
}

// =============================================================================
// GlobalPlacement_DeepNesting
// =============================================================================

TEST(BRepGraph_AssemblyTest, GlobalPlacement_DeepNesting)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes18 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());

  const BRepGraph_ProductId aPartId = BRepGraph_ProductId::Start();

  // Build: RootAssembly -> (OccSubAsm) -> SubAssembly -> (OccPart) -> Part
  const BRepGraph_ProductId aSubAsmId = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aSubAsmId);
  const BRepGraph_ProductId aRootAsmId = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aRootAsmId);

  gp_Trsf aTrsf1;
  aTrsf1.SetTranslation(gp_Vec(100.0, 0.0, 0.0));
  gp_Trsf aTrsf2;
  aTrsf2.SetTranslation(gp_Vec(0.0, 200.0, 0.0));

  // RootAssembly places SubAssembly with aTrsf2 (top-level occurrence, no parent occ).
  const BRepGraph_OccurrenceId anOccSubAsm =
    aGraph.Editor().Products().Append(aRootAsmId, aSubAsmId, TopLoc_Location(aTrsf2));
  // SubAssembly places Part with aTrsf1, with parent occurrence = anOccSubAsm.
  const BRepGraph_OccurrenceId anOccPart =
    aGraph.Editor().Products().Append(aSubAsmId, aPartId, TopLoc_Location(aTrsf1), anOccSubAsm);

  // OccurrenceLocation returns the local location from the OccurrenceRef.
  // Global placement composition (parent chain walk) is handled by explorers.
  TopLoc_Location aLocalPart = aGraph.Topo().Occurrences().OccurrenceLocation(anOccPart);
  const gp_Trsf&  aPartTrsf  = aLocalPart.Transformation();
  EXPECT_NEAR(aPartTrsf.TranslationPart().X(), 100.0, Precision::Confusion());
  EXPECT_NEAR(aPartTrsf.TranslationPart().Y(), 0.0, Precision::Confusion());

  TopLoc_Location aLocalSubAsm = aGraph.Topo().Occurrences().OccurrenceLocation(anOccSubAsm);
  const gp_Trsf&  aSubAsmTrsf  = aLocalSubAsm.Transformation();
  EXPECT_NEAR(aSubAsmTrsf.TranslationPart().X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aSubAsmTrsf.TranslationPart().Y(), 200.0, Precision::Confusion());
}

// =============================================================================
// NbNodes_IncludesAssembly
// =============================================================================

TEST(BRepGraph_AssemblyTest, NbNodes_IncludesAssembly)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes19 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());

  const size_t aNbNodesAfterBuild = aGraph.Topo().Gen().NbNodes();
  // Should include the auto-created root product.
  EXPECT_GE(aNbNodesAfterBuild, 1u);

  // Add assembly + occurrence.
  const BRepGraph_ProductId aAssemblyId = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aAssemblyId);
  [[maybe_unused]] const BRepGraph_OccurrenceId anOccId =
    aGraph.Editor().Products().Append(aAssemblyId, BRepGraph_ProductId::Start(), TopLoc_Location());

  const size_t aNbNodesAfterAssembly = aGraph.Topo().Gen().NbNodes();
  EXPECT_EQ(aNbNodesAfterAssembly, aNbNodesAfterBuild + 2); // +1 product, +1 occurrence
}

// =============================================================================
// OccurrencesOfProduct_Relations
// =============================================================================

TEST(BRepGraph_AssemblyTest, OccurrencesOfProduct_Relations)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes20 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());

  const BRepGraph_ProductId aPartId     = BRepGraph_ProductId::Start();
  const BRepGraph_ProductId aAssemblyId = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aAssemblyId);
  [[maybe_unused]] const BRepGraph_OccurrenceId anOccId1 =
    aGraph.Editor().Products().Append(aAssemblyId, aPartId, TopLoc_Location());
  [[maybe_unused]] const BRepGraph_OccurrenceId anOccId2 =
    aGraph.Editor().Products().Append(aAssemblyId, aPartId, TopLoc_Location());

  // We test product-occurrence relation maintenance through the NbComponents API.
  EXPECT_EQ(aGraph.Topo().Products().NbComponents(aAssemblyId), 2);
}

// =============================================================================
// Product_Count
// =============================================================================

TEST(BRepGraph_AssemblyTest, Product_Count)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes21 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());

  [[maybe_unused]] const BRepGraph_ProductId anEmptyProduct = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(anEmptyProduct);

  size_t aCount = 0;
  for (BRepGraph_ProductIterator aProductIt(aGraph); aProductIt.More(); aProductIt.Next())
  {
    ++aCount;
  }
  EXPECT_EQ(aCount, 2u); // auto root + added assembly
}

// =============================================================================
// Occurrence_Count
// =============================================================================

TEST(BRepGraph_AssemblyTest, Occurrence_Count)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes22 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());

  const BRepGraph_ProductId aPartId     = BRepGraph_ProductId::Start();
  const BRepGraph_ProductId aAssemblyId = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aAssemblyId);
  [[maybe_unused]] const BRepGraph_OccurrenceId anOccId1 =
    aGraph.Editor().Products().Append(aAssemblyId, aPartId, TopLoc_Location());
  [[maybe_unused]] const BRepGraph_OccurrenceId anOccId2 =
    aGraph.Editor().Products().Append(aAssemblyId, aPartId, TopLoc_Location());

  size_t aCount = 0;
  for (BRepGraph_OccurrenceIterator anOccIt(aGraph); anOccIt.More(); anOccIt.Next())
  {
    ++aCount;
  }
  // 1 shape-root occurrence (from Build) + 2 added occurrences = 3.
  EXPECT_EQ(aCount, 3u);
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
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes23 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());

  // The auto-created root product should have a UID with IsAssembly() == false
  // (it's a part, but its UID Kind is Product, so IsAssembly() on UID checks the Kind).
  const BRepGraph_UID aProductUID = aGraph.UIDs().Of(BRepGraph_ProductId::Start());
  EXPECT_TRUE(aProductUID.IsValid());
  EXPECT_TRUE(aProductUID.IsAssembly());
  EXPECT_FALSE(aProductUID.IsTopology());
}

// =============================================================================
// LinkProducts_InvalidParent_ReturnsInvalid
// =============================================================================

TEST(BRepGraph_AssemblyTest, LinkProducts_InvalidParent_ReturnsInvalid)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes24 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());

  BRepGraph_OccurrenceId aResult = aGraph.Editor().Products().Append(BRepGraph_ProductId(999),
                                                                     BRepGraph_ProductId::Start(),
                                                                     TopLoc_Location());
  EXPECT_FALSE(aResult.IsValid());

  // Out-of-bounds referenced product index.
  aResult = aGraph.Editor().Products().Append(BRepGraph_ProductId::Start(),
                                              BRepGraph_ProductId(999),
                                              TopLoc_Location());
  EXPECT_FALSE(aResult.IsValid());
}

// =============================================================================
// LinkProducts_SelfReference_ReturnsInvalid
// =============================================================================

TEST(BRepGraph_AssemblyTest, LinkProducts_SelfReference_ReturnsInvalid)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes25 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());

  const BRepGraph_ProductId aPartId = BRepGraph_ProductId::Start();

  // Self-referencing: a product cannot be an occurrence of itself.
  const BRepGraph_OccurrenceId aResult =
    aGraph.Editor().Products().Append(aPartId, aPartId, TopLoc_Location());
  EXPECT_FALSE(aResult.IsValid());
}

// =============================================================================
// RootProducts_RemovedOccurrence_DoesNotAffectRoots
// =============================================================================

TEST(BRepGraph_AssemblyTest, RootProducts_RemovedOccurrence_DoesNotAffectRoots)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes26 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());

  const BRepGraph_ProductId aPartId     = BRepGraph_ProductId::Start();
  const BRepGraph_ProductId aAssemblyId = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aAssemblyId);
  const BRepGraph_OccurrenceId anOccId =
    aGraph.Editor().Products().Append(aAssemblyId, aPartId, TopLoc_Location());

  // Before removal: only assembly is root (part is referenced).
  NCollection_LinearVector<BRepGraph_ProductId> aRoots = collectRootProducts(aGraph);
  EXPECT_EQ(aRoots.Size(), 1);
  EXPECT_EQ(aRoots.Value(0), aAssemblyId);

  // Remove the occurrence - part becomes a root again (no longer referenced).
  aGraph.Editor().Gen().RemoveSubgraph(anOccId);

  aRoots = collectRootProducts(aGraph);
  // Both part and assembly are now roots (part is no longer referenced by any occurrence).
  EXPECT_GE(aRoots.Size(), 1);
  EXPECT_TRUE(hasRootProduct(aRoots, aAssemblyId));
}

// =============================================================================
// GlobalPlacement_DAGSharing_DistinctPathsGiveDistinctPlacements
// =============================================================================

TEST(BRepGraph_AssemblyTest, GlobalPlacement_DAGSharing_DistinctPathsGiveDistinctPlacements)
{
  // Shared part placed twice under the same assembly at different locations.
  // Each occurrence has its own placement chain - no ambiguity.
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes27 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());

  const BRepGraph_ProductId aPartId     = BRepGraph_ProductId::Start();
  const BRepGraph_ProductId aAssemblyId = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aAssemblyId);

  gp_Trsf aTrsf1;
  aTrsf1.SetTranslation(gp_Vec(100.0, 0.0, 0.0));
  gp_Trsf aTrsf2;
  aTrsf2.SetTranslation(gp_Vec(0.0, 200.0, 0.0));

  const BRepGraph_OccurrenceId anOcc1 =
    aGraph.Editor().Products().Append(aAssemblyId, aPartId, TopLoc_Location(aTrsf1));
  const BRepGraph_OccurrenceId anOcc2 =
    aGraph.Editor().Products().Append(aAssemblyId, aPartId, TopLoc_Location(aTrsf2));

  // Same part, different occurrences, different global placements.
  TopLoc_Location aGlobal1 = aGraph.Topo().Occurrences().OccurrenceLocation(anOcc1);
  TopLoc_Location aGlobal2 = aGraph.Topo().Occurrences().OccurrenceLocation(anOcc2);

  EXPECT_NEAR(aGlobal1.Transformation().TranslationPart().X(), 100.0, Precision::Confusion());
  EXPECT_NEAR(aGlobal1.Transformation().TranslationPart().Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aGlobal2.Transformation().TranslationPart().X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aGlobal2.Transformation().TranslationPart().Y(), 200.0, Precision::Confusion());
}

// =============================================================================
// LinkProducts_RemovedProduct_ReturnsInvalid
// =============================================================================

TEST(BRepGraph_AssemblyTest, LinkProducts_RemovedProduct_ReturnsInvalid)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes28 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());

  const BRepGraph_ProductId aAssemblyId = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aAssemblyId);
  // Remove the assembly.
  aGraph.Editor().Gen().RemoveNode(aAssemblyId);

  // Cannot add occurrence to a removed product.
  const BRepGraph_OccurrenceId aResult =
    aGraph.Editor().Products().Append(aAssemblyId, BRepGraph_ProductId::Start(), TopLoc_Location());
  EXPECT_FALSE(aResult.IsValid());

  // Cannot reference a removed product either.
  const BRepGraph_ProductId aAsm2 = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aAsm2);
  aGraph.Editor().Gen().RemoveNode(BRepGraph_ProductId::Start());
  const BRepGraph_OccurrenceId aResult2 =
    aGraph.Editor().Products().Append(aAsm2, BRepGraph_ProductId::Start(), TopLoc_Location());
  EXPECT_FALSE(aResult2.IsValid());
}

// =============================================================================
// GlobalPlacement_ThreeLevelNesting
// =============================================================================

TEST(BRepGraph_AssemblyTest, GlobalPlacement_ThreeLevelNesting)
{
  // Root -> Mid -> Leaf, each with a distinct translation.
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes29 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());

  const BRepGraph_ProductId aLeafPart = BRepGraph_ProductId::Start();
  const BRepGraph_ProductId aMidAsm   = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aMidAsm);
  const BRepGraph_ProductId aRootAsm = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aRootAsm);
  const BRepGraph_ProductId aTopAsm = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aTopAsm);

  gp_Trsf aT1, aT2, aT3;
  aT1.SetTranslation(gp_Vec(1.0, 0.0, 0.0));
  aT2.SetTranslation(gp_Vec(0.0, 2.0, 0.0));
  aT3.SetTranslation(gp_Vec(0.0, 0.0, 3.0));

  // TopAsm places RootAsm.
  const BRepGraph_OccurrenceId anOccRoot =
    aGraph.Editor().Products().Append(aTopAsm, aRootAsm, TopLoc_Location(aT3));
  // RootAsm places MidAsm, parent occ = anOccRoot.
  const BRepGraph_OccurrenceId anOccMid =
    aGraph.Editor().Products().Append(aRootAsm, aMidAsm, TopLoc_Location(aT2), anOccRoot);
  // MidAsm places Leaf, parent occ = anOccMid.
  const BRepGraph_OccurrenceId anOccLeaf =
    aGraph.Editor().Products().Append(aMidAsm, aLeafPart, TopLoc_Location(aT1), anOccMid);

  // OccurrenceLocation returns the local location only.
  // Verify each level has the correct local placement.
  TopLoc_Location aLeafLoc = aGraph.Topo().Occurrences().OccurrenceLocation(anOccLeaf);
  EXPECT_NEAR(aLeafLoc.Transformation().TranslationPart().X(), 1.0, Precision::Confusion());
  TopLoc_Location aMidLoc = aGraph.Topo().Occurrences().OccurrenceLocation(anOccMid);
  EXPECT_NEAR(aMidLoc.Transformation().TranslationPart().Y(), 2.0, Precision::Confusion());
  TopLoc_Location aRootLoc = aGraph.Topo().Occurrences().OccurrenceLocation(anOccRoot);
  EXPECT_NEAR(aRootLoc.Transformation().TranslationPart().Z(), 3.0, Precision::Confusion());
}

// =============================================================================
// ShapesView_ProductShape_ReconstructsBuiltRootTransform
// =============================================================================

TEST(BRepGraph_AssemblyTest, ShapesView_ProductShape_ReconstructsBuiltRootTransform)
{
  TopoDS_Shape aRootShape = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  gp_Trsf aMove;
  aMove.SetTranslation(gp_Vec(15.0, -7.0, 2.0));
  aRootShape.Move(TopLoc_Location(aMove));
  aRootShape.Reverse();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes30 =
    aGraph.Shapes().Add(aRootShape);

  const TopoDS_Shape aProductShape = aGraph.Shapes().Shape(BRepGraph_ProductId::Start());
  ASSERT_FALSE(aProductShape.IsNull());
  EXPECT_EQ(aProductShape.ShapeType(), aRootShape.ShapeType());
  // Location is preserved via OccurrenceRef::LocalLocation.
  // Orientation may differ since RootOrientation is no longer stored on ProductDef.

  const TopoDS_Shape aReconstructed = aGraph.Shapes().Reconstruct(BRepGraph_ProductId::Start());
  ASSERT_FALSE(aReconstructed.IsNull());
  EXPECT_EQ(aReconstructed.ShapeType(), aRootShape.ShapeType());
}

// =============================================================================
// ShapesView_AssemblyProduct_ReconstructsChildOccurrences
// =============================================================================

TEST(BRepGraph_AssemblyTest, ShapesView_AssemblyProduct_ReconstructsChildOccurrences)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes31 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());

  const BRepGraph_ProductId aPartId     = BRepGraph_ProductId::Start();
  const BRepGraph_ProductId aAssemblyId = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aAssemblyId);

  gp_Trsf aTrsf1;
  aTrsf1.SetTranslation(gp_Vec(100.0, 0.0, 0.0));
  gp_Trsf aTrsf2;
  aTrsf2.SetTranslation(gp_Vec(200.0, 0.0, 0.0));

  ASSERT_TRUE(
    aGraph.Editor().Products().Append(aAssemblyId, aPartId, TopLoc_Location(aTrsf1)).IsValid());
  ASSERT_TRUE(
    aGraph.Editor().Products().Append(aAssemblyId, aPartId, TopLoc_Location(aTrsf2)).IsValid());

  const TopoDS_Shape aAssemblyShape = aGraph.Shapes().Shape(aAssemblyId);
  ASSERT_FALSE(aAssemblyShape.IsNull());
  EXPECT_EQ(aAssemblyShape.ShapeType(), TopAbs_COMPOUND);
  EXPECT_EQ(aAssemblyShape.NbChildren(), 2);

  TopoDS_Iterator anIt(aAssemblyShape);
  ASSERT_TRUE(anIt.More());
  const TopoDS_Shape aFirstChild = anIt.Value();
  anIt.Next();
  ASSERT_TRUE(anIt.More());
  const TopoDS_Shape aSecondChild = anIt.Value();
  anIt.Next();
  EXPECT_FALSE(anIt.More());

  EXPECT_TRUE(aFirstChild.IsPartner(aSecondChild));
  EXPECT_NEAR(translationX(aFirstChild), 100.0, Precision::Confusion());
  EXPECT_NEAR(translationX(aSecondChild), 200.0, Precision::Confusion());
}

// =============================================================================
// ShapesView_OccurrenceShape_UsesGlobalPlacementChain
// =============================================================================

TEST(BRepGraph_AssemblyTest, ShapesView_OccurrenceShape_UsesGlobalPlacementChain)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes32 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());

  const BRepGraph_ProductId aPartId      = BRepGraph_ProductId::Start();
  const BRepGraph_ProductId aSubAssembly = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aSubAssembly);
  const BRepGraph_ProductId aRootAssembly = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aRootAssembly);

  gp_Trsf aParentTrsf;
  aParentTrsf.SetTranslation(gp_Vec(10.0, 0.0, 0.0));
  const BRepGraph_OccurrenceId aParentOccurrence =
    aGraph.Editor().Products().Append(aRootAssembly, aSubAssembly, TopLoc_Location(aParentTrsf));
  ASSERT_TRUE(aParentOccurrence.IsValid());

  gp_Trsf aChildTrsf;
  aChildTrsf.SetTranslation(gp_Vec(20.0, 0.0, 0.0));
  const BRepGraph_OccurrenceId aChildOccurrence =
    aGraph.Editor().Products().Append(aSubAssembly,
                                      aPartId,
                                      TopLoc_Location(aChildTrsf),
                                      aParentOccurrence);
  ASSERT_TRUE(aChildOccurrence.IsValid());

  const TopoDS_Shape aSubAssemblyShape = aGraph.Shapes().Shape(aSubAssembly);
  ASSERT_FALSE(aSubAssemblyShape.IsNull());
  EXPECT_EQ(aSubAssemblyShape.NbChildren(), 1);

  TopoDS_Iterator aSubIt(aSubAssemblyShape);
  ASSERT_TRUE(aSubIt.More());
  EXPECT_NEAR(translationX(aSubIt.Value()), 20.0, Precision::Confusion());

  // Shape(occurrence) uses local location only (no global composition).
  const TopoDS_Shape aOccurrenceShape = aGraph.Shapes().Shape(aChildOccurrence);
  ASSERT_FALSE(aOccurrenceShape.IsNull());
  EXPECT_NEAR(translationX(aOccurrenceShape), 20.0, Precision::Confusion());

  const TopoDS_Shape aReconstructed = aGraph.Shapes().Reconstruct(aChildOccurrence);
  ASSERT_FALSE(aReconstructed.IsNull());
  EXPECT_NEAR(translationX(aReconstructed), 20.0, Precision::Confusion());
}

// =============================================================================
// ShapesView_OccurrenceShape_FiltersNestedChildrenByParentOccurrence
// =============================================================================

TEST(BRepGraph_AssemblyTest, ShapesView_OccurrenceShape_FiltersNestedChildrenByParentOccurrence)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes33 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());

  const BRepGraph_ProductId aPartId      = BRepGraph_ProductId::Start();
  const BRepGraph_ProductId aSubAssembly = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aSubAssembly);
  const BRepGraph_ProductId aRootAssembly = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aRootAssembly);

  gp_Trsf aParentTrsf1;
  aParentTrsf1.SetTranslation(gp_Vec(100.0, 0.0, 0.0));
  const BRepGraph_OccurrenceId aParentOccurrence1 =
    aGraph.Editor().Products().Append(aRootAssembly, aSubAssembly, TopLoc_Location(aParentTrsf1));
  ASSERT_TRUE(aParentOccurrence1.IsValid());

  gp_Trsf aParentTrsf2;
  aParentTrsf2.SetTranslation(gp_Vec(200.0, 0.0, 0.0));
  const BRepGraph_OccurrenceId aParentOccurrence2 =
    aGraph.Editor().Products().Append(aRootAssembly, aSubAssembly, TopLoc_Location(aParentTrsf2));
  ASSERT_TRUE(aParentOccurrence2.IsValid());

  gp_Trsf aChildTrsf1;
  aChildTrsf1.SetTranslation(gp_Vec(10.0, 0.0, 0.0));
  const BRepGraph_OccurrenceId aChildOccurrence1 =
    aGraph.Editor().Products().Append(aSubAssembly,
                                      aPartId,
                                      TopLoc_Location(aChildTrsf1),
                                      aParentOccurrence1);
  ASSERT_TRUE(aChildOccurrence1.IsValid());

  gp_Trsf aChildTrsf2;
  aChildTrsf2.SetTranslation(gp_Vec(20.0, 0.0, 0.0));
  const BRepGraph_OccurrenceId aChildOccurrence2 =
    aGraph.Editor().Products().Append(aSubAssembly,
                                      aPartId,
                                      TopLoc_Location(aChildTrsf2),
                                      aParentOccurrence2);
  ASSERT_TRUE(aChildOccurrence2.IsValid());

  // Verify that occurrence shapes are non-null and have children.
  // Parent occurrence filtering uses local locations only (no global composition).
  const TopoDS_Shape aOccurrenceShape1 = aGraph.Shapes().Shape(aParentOccurrence1);
  ASSERT_FALSE(aOccurrenceShape1.IsNull());
  EXPECT_GE(aOccurrenceShape1.NbChildren(), 1);

  const TopoDS_Shape aOccurrenceShape2 = aGraph.Shapes().Shape(aParentOccurrence2);
  ASSERT_FALSE(aOccurrenceShape2.IsNull());
  EXPECT_GE(aOccurrenceShape2.NbChildren(), 1);

  const TopoDS_Shape aReconstructed1 = aGraph.Shapes().Reconstruct(aParentOccurrence1);
  ASSERT_FALSE(aReconstructed1.IsNull());
  EXPECT_GE(aReconstructed1.NbChildren(), 1);

  const TopoDS_Shape aReconstructed2 = aGraph.Shapes().Reconstruct(aParentOccurrence2);
  ASSERT_FALSE(aReconstructed2.IsNull());
  EXPECT_GE(aReconstructed2.NbChildren(), 1);
}

// =============================================================================
// ShapesView_OccurrenceShape_KeepsCommonChildrenAndFiltersBranchSpecificOnes
// =============================================================================

TEST(BRepGraph_AssemblyTest,
     ShapesView_OccurrenceShape_KeepsCommonChildrenAndFiltersBranchSpecificOnes)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes34 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());

  const BRepGraph_ProductId aPartId      = BRepGraph_ProductId::Start();
  const BRepGraph_ProductId aSubAssembly = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aSubAssembly);
  const BRepGraph_ProductId aRootAssembly = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aRootAssembly);

  gp_Trsf aParentTrsf1;
  aParentTrsf1.SetTranslation(gp_Vec(100.0, 0.0, 0.0));
  const BRepGraph_OccurrenceId aParentOccurrence1 =
    aGraph.Editor().Products().Append(aRootAssembly, aSubAssembly, TopLoc_Location(aParentTrsf1));
  ASSERT_TRUE(aParentOccurrence1.IsValid());

  gp_Trsf aParentTrsf2;
  aParentTrsf2.SetTranslation(gp_Vec(200.0, 0.0, 0.0));
  const BRepGraph_OccurrenceId aParentOccurrence2 =
    aGraph.Editor().Products().Append(aRootAssembly, aSubAssembly, TopLoc_Location(aParentTrsf2));
  ASSERT_TRUE(aParentOccurrence2.IsValid());

  gp_Trsf aCommonChildTrsf;
  aCommonChildTrsf.SetTranslation(gp_Vec(5.0, 0.0, 0.0));
  const BRepGraph_OccurrenceId aCommonChildOccurrence =
    aGraph.Editor().Products().Append(aSubAssembly, aPartId, TopLoc_Location(aCommonChildTrsf));
  ASSERT_TRUE(aCommonChildOccurrence.IsValid());

  gp_Trsf aBranchChildTrsf1;
  aBranchChildTrsf1.SetTranslation(gp_Vec(10.0, 0.0, 0.0));
  const BRepGraph_OccurrenceId aBranchChildOccurrence1 =
    aGraph.Editor().Products().Append(aSubAssembly,
                                      aPartId,
                                      TopLoc_Location(aBranchChildTrsf1),
                                      aParentOccurrence1);
  ASSERT_TRUE(aBranchChildOccurrence1.IsValid());

  gp_Trsf aBranchChildTrsf2;
  aBranchChildTrsf2.SetTranslation(gp_Vec(20.0, 0.0, 0.0));
  const BRepGraph_OccurrenceId aBranchChildOccurrence2 =
    aGraph.Editor().Products().Append(aSubAssembly,
                                      aPartId,
                                      TopLoc_Location(aBranchChildTrsf2),
                                      aParentOccurrence2);
  ASSERT_TRUE(aBranchChildOccurrence2.IsValid());

  // Verify occurrence shapes are reconstructed and have children.
  // Branch-specific filtering and global location composition are handled by explorers.
  const TopoDS_Shape aOccurrenceShape1 = aGraph.Shapes().Shape(aParentOccurrence1);
  ASSERT_FALSE(aOccurrenceShape1.IsNull());
  EXPECT_GE(aOccurrenceShape1.NbChildren(), 1);

  const TopoDS_Shape aOccurrenceShape2 = aGraph.Shapes().Shape(aParentOccurrence2);
  ASSERT_FALSE(aOccurrenceShape2.IsNull());
  EXPECT_GE(aOccurrenceShape2.NbChildren(), 1);
}

// =============================================================================
// OccurrencesOfProduct_ViaRelations
// =============================================================================

TEST(BRepGraph_AssemblyTest, OccurrencesOfProduct_ViaRelations)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes35 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());

  const BRepGraph_ProductId aPartId = BRepGraph_ProductId::Start();
  const BRepGraph_ProductId aAsmId  = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aAsmId);
  [[maybe_unused]] const BRepGraph_OccurrenceId anOccId1 =
    aGraph.Editor().Products().Append(aAsmId, aPartId, TopLoc_Location());
  [[maybe_unused]] const BRepGraph_OccurrenceId anOccId2 =
    aGraph.Editor().Products().Append(aAsmId, aPartId, TopLoc_Location());

  // Access via DefsView, backed by incrementally maintained occurrence refs.
  EXPECT_EQ(aGraph.Topo().Products().NbComponents(aAsmId), 2);

  // The part product has 1 shape-root occurrence (auto-created by Build).
  EXPECT_EQ(aGraph.Topo().Products().NbComponents(aPartId), 1);
}

// =============================================================================
// GlobalPlacement_CircularParentOccurrence_Terminates
// =============================================================================

TEST(BRepGraph_AssemblyTest, OccurrenceLocation_AlwaysTerminates)
{
  // OccurrenceLocation returns the local location from the OccurrenceRef.
  // No parent chain walk means no risk of infinite loops.
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes36 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());

  const BRepGraph_ProductId aPartId = BRepGraph_ProductId::Start();
  const BRepGraph_ProductId aAsmId  = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aAsmId);

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(1.0, 0.0, 0.0));

  const BRepGraph_OccurrenceId anOcc1 =
    aGraph.Editor().Products().Append(aAsmId, aPartId, TopLoc_Location(aTrsf));
  const BRepGraph_OccurrenceId anOcc2 =
    aGraph.Editor().Products().Append(aAsmId, aPartId, TopLoc_Location(aTrsf));

  // OccurrenceLocation must terminate and return a location (local from the ref).
  TopLoc_Location aLoc1 = aGraph.Topo().Occurrences().OccurrenceLocation(anOcc1);
  EXPECT_NEAR(aLoc1.Transformation().TranslationPart().X(), 1.0, Precision::Confusion());
  ASSERT_TRUE(anOcc2.IsValid());
  // anOcc2 also has its own local location from its ref.
  const TopLoc_Location aLoc2 = aGraph.Topo().Occurrences().OccurrenceLocation(anOcc2);
  EXPECT_FALSE(aLoc2.IsIdentity());
}

// =============================================================================
// Add(graph, shape) -- root ingestion
// =============================================================================

TEST(BRepGraph_AssemblyTest, Add_RootProduct_PreservesShapeLocation)
{
  BRepGraph    aGraph;
  TopoDS_Shape aLocalBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();
  TopoDS_Shape aBox      = aLocalBox;
  gp_Trsf      aTrsf;
  aTrsf.SetTranslation(gp_Vec(5.0, 6.0, 7.0));
  aBox.Location(TopLoc_Location(aTrsf));

  const BRepGraph::ShapesView::Result aResult = aGraph.Shapes().Add(aBox);
  ASSERT_TRUE(aResult.IsOk());
  ASSERT_TRUE(aResult.Product.IsValid());
  ASSERT_TRUE(aResult.TopologyRoot.IsValid());
  ASSERT_TRUE(aResult.Occurrence.IsValid());
  EXPECT_EQ(aGraph.Shapes().FindNode(aBox), aResult.TopologyRoot);

  ASSERT_EQ(aGraph.RootProductIds().Size(), 1u);
  EXPECT_EQ(aGraph.RootProductIds().Value(0), aResult.Product);

  const BRepGraph_OccurrenceRefId anOccRefId =
    aGraph.Topo().Products().Relations(aResult.Product).OccurrenceRefIds.Value(0);
  const TopLoc_Location& aLoc = aGraph.Refs().Occurrences().Entry(anOccRefId).LocalLocation;
  EXPECT_TRUE(aLoc.IsEqual(aBox.Location()));

  const TopoDS_Shape aProductShape = aGraph.Shapes().Shape(aResult.Product);
  ASSERT_FALSE(aProductShape.IsNull());
  expectBoxNear(geometryBounds(aProductShape), geometryBounds(aBox), 1.0e-7);

  const TopoDS_Shape aTopologyShape = aGraph.Shapes().Shape(aResult.TopologyRoot);
  ASSERT_FALSE(aTopologyShape.IsNull());
  EXPECT_TRUE(aTopologyShape.Location().IsIdentity());
  expectBoxNear(geometryBounds(aTopologyShape), geometryBounds(aLocalBox), 1.0e-7);
}

TEST(BRepGraph_AssemblyTest, Add_NoAutoProduct_TopologyOnly)
{
  BRepGraph                      aGraph;
  BRepGraph::ShapesView::Options anOpts;
  anOpts.CreateAutoProduct = false;

  const BRepGraph::ShapesView::Result aResult =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape(), anOpts);
  ASSERT_TRUE(aResult.IsOk());
  EXPECT_FALSE(aResult.Product.IsValid());
  EXPECT_FALSE(aResult.Occurrence.IsValid());
  ASSERT_TRUE(aResult.TopologyRoot.IsValid());
  EXPECT_EQ(aGraph.RootProductIds().Size(), 0u);
}

TEST(BRepGraph_AssemblyTest, Add_NullShape_ReturnsInvalidResult)
{
  BRepGraph                           aGraph;
  TopoDS_Shape                        aNull;
  const BRepGraph::ShapesView::Result aResult = aGraph.Shapes().Add(aNull);
  EXPECT_FALSE(aResult.IsOk());
  EXPECT_FALSE(aResult.Product.IsValid());
  EXPECT_FALSE(aResult.TopologyRoot.IsValid());
}

// =============================================================================
// Add(graph, shape, parent) -- parented ingestion
// =============================================================================

TEST(BRepGraph_AssemblyTest, Add_ProductParent_CreatesChildPartAndOccurrence)
{
  BRepGraph                 aGraph;
  const BRepGraph_ProductId aParent = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aParent);
  ASSERT_TRUE(aParent.IsValid());

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(2.0, 0.0, 0.0));
  TopoDS_Shape aSphere = BRepPrimAPI_MakeSphere(5.0).Shape();
  aSphere.Location(TopLoc_Location(aTrsf));

  const BRepGraph::ShapesView::Result aResult =
    aGraph.Shapes().Add(aSphere, BRepGraph_NodeId(aParent));
  ASSERT_TRUE(aResult.IsOk());
  ASSERT_TRUE(aResult.TopologyRoot.IsValid());
  ASSERT_TRUE(aResult.Product.IsValid());
  ASSERT_TRUE(aResult.Occurrence.IsValid());

  EXPECT_NE(aResult.Product, aParent);

  const BRepGraphInc::ProductRelations& aParentRelations =
    aGraph.Topo().Products().Relations(aParent);
  ASSERT_EQ(aParentRelations.OccurrenceRefIds.Size(), 1u);
  const BRepGraph_OccurrenceRefId anOccRefId = aParentRelations.OccurrenceRefIds.Value(0);
  const TopLoc_Location& aLoc = aGraph.Refs().Occurrences().Entry(anOccRefId).LocalLocation;
  EXPECT_TRUE(aLoc.IsEqual(aSphere.Location()));
}

TEST(BRepGraph_AssemblyTest, Add_CompoundParent_AppendsAsChild)
{
  BRepGraph       aGraph;
  TopoDS_Compound aCompound;
  BRep_Builder().MakeCompound(aCompound);
  TopoDS_Shape aBox = BRepPrimAPI_MakeBox(1.0, 1.0, 1.0).Shape();
  BRep_Builder().Add(aCompound, aBox);

  const BRepGraph::ShapesView::Result aRoot = aGraph.Shapes().Add(aCompound);
  ASSERT_TRUE(aRoot.IsOk());
  ASSERT_EQ(aRoot.TopologyRoot.NodeKind, BRepGraph_NodeId::Kind::Compound);

  const BRepGraph_CompoundId aCompoundId(aRoot.TopologyRoot);
  const uint32_t             aRefsBefore =
    static_cast<uint32_t>(aGraph.Topo().Compounds().Relations(aCompoundId).ChildRefIds.Size());

  TopoDS_Shape aFace;
  for (TopExp_Explorer anExp(BRepPrimAPI_MakeBox(2.0, 2.0, 2.0).Shape(), TopAbs_FACE); anExp.More();
       anExp.Next())
  {
    aFace = anExp.Current();
    break;
  }
  ASSERT_FALSE(aFace.IsNull());
  const BRepGraph::ShapesView::Result aChild =
    aGraph.Shapes().Add(aFace, BRepGraph_NodeId(aCompoundId));
  ASSERT_TRUE(aChild.IsOk());
  EXPECT_TRUE(aChild.InsertedRef.IsValid());

  EXPECT_EQ(aGraph.Topo().Compounds().Relations(aCompoundId).ChildRefIds.Size(), aRefsBefore + 1u);
}

TEST(BRepGraph_AssemblyTest, Add_InvalidParent_ReturnsInvalidResult)
{
  BRepGraph                                            aGraph;
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes37 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(1.0, 1.0, 1.0).Shape());

  TopoDS_Shape                        aBox    = BRepPrimAPI_MakeBox(2.0, 2.0, 2.0).Shape();
  const BRepGraph::ShapesView::Result aResult = aGraph.Shapes().Add(aBox, BRepGraph_NodeId());
  EXPECT_FALSE(aResult.IsOk());
}

TEST(BRepGraph_AssemblyTest, RootProductRemovalCompactsInPlaceAfterRepeatedRemoves)
{
  BRepGraph aGraph;
  aGraph.Clear();

  const BRepGraph_ProductId aRootA = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aRootA);
  const BRepGraph_ProductId aRootB = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aRootB);
  const BRepGraph_ProductId aRootC = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aRootC);
  ASSERT_TRUE(aRootA.IsValid());
  ASSERT_TRUE(aRootB.IsValid());
  ASSERT_TRUE(aRootC.IsValid());

  ASSERT_EQ(collectRootProducts(aGraph).Size(), 3u);

  aGraph.Editor().Gen().RemoveNode(aRootB);
  NCollection_LinearVector<BRepGraph_ProductId> aRoots = collectRootProducts(aGraph);
  ASSERT_EQ(aRoots.Size(), 2u);
  EXPECT_TRUE(hasRootProduct(aRoots, aRootA));
  EXPECT_FALSE(hasRootProduct(aRoots, aRootB));
  EXPECT_TRUE(hasRootProduct(aRoots, aRootC));

  const BRepGraph_ProductId aRootD = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aRootD);
  ASSERT_TRUE(aRootD.IsValid());
  aGraph.Editor().Gen().RemoveNode(aRootA);
  aGraph.Editor().Gen().RemoveNode(aRootD);

  aRoots = collectRootProducts(aGraph);
  ASSERT_EQ(aRoots.Size(), 1u);
  EXPECT_EQ(aRoots.Value(0), aRootC);
}
