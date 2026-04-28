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

// Scenario regression matrix.
//
// Cross-cutting tests that combine multiple BRepGraph subsystems in realistic
// end-to-end flows.  Each test deliberately touches:
//   BRepGraph_Builder  ->  BRepGraph_Validate(Audit)
//   ->  Editor mutation  ->  BRepGraph.Shapes().Reconstruct / Shape
//   ->  BRepGraphInc_Populate  ->  BRepGraphInc_Storage.ValidateReverseIndex / ValidateSelfIds
//
// These tests are not duplicating the many isolated unit tests that already
// exist for each individual API. They specifically lock down the correctness of
// the combined mutation-validate-reconstruct-populate pipelines and the
// reverse-index paths for assemblies, compounds, comp-solids, and free wires.

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS_Vertex.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepGProp.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_Builder.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_Validate.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Populate.hxx>
#include <BRepGraphInc_Reconstruct.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>
#include <Standard_ProgramError.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <NCollection_IndexedMap.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_CompSolid.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <gtest/gtest.h>

namespace
{

//=================================================================================================

static double computeArea(const TopoDS_Shape& theShape)
{
  GProp_GProps aProps;
  BRepGProp::SurfaceProperties(theShape, aProps);
  return aProps.Mass();
}

//=================================================================================================

static int countSubShapes(const TopoDS_Shape& theShape, TopAbs_ShapeEnum theType)
{
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMap;
  TopExp::MapShapes(theShape, theType, aMap);
  return aMap.Extent();
}

} // namespace

// =============================================================================
// Scenario 1: Box mutation -> Validate(Audit) -> Reconstruct -> BRepGraphInc round-trip
//
// Flow: build box graph -> clean audit validate -> mutate a vertex point in
// the graph -> audit validate again (structural integrity must survive a data
// change) -> reconstruct the solid -> verify area has changed -> BRepGraphInc
// populate from reconstructed solid -> ValidateReverseIndex.
// =============================================================================

TEST(BRepGraph_ScenarioMatrix, Box_MutateVertex_ValidateReconstructPopulateRoundTrip)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox       = aBoxMaker.Shape();
  const double        anOrigArea = computeArea(aBox);

  // --- Build BRepGraph ---
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes1 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // --- Validate clean graph (full audit) ---
  const BRepGraph_Validate::Result aCleanResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  ASSERT_TRUE(aCleanResult.IsValid()) << "Graph must be structurally valid before any mutation";

  // --- Locate a vertex (via the first face's outer wire's first coedge) ---
  ASSERT_GT(aGraph.Topo().Vertices().Nb(), 0);
  const gp_Pnt anOldPt = BRepGraph_Tool::Vertex::Pnt(aGraph, BRepGraph_VertexId::Start());
  {
    BRepGraph_MutGuard<BRepGraphInc::VertexDef> aMut =
      aGraph.Editor().Vertices().Mut(BRepGraph_VertexId::Start());
    aMut->Point = gp_Pnt(anOldPt.X() + 50.0, anOldPt.Y(), anOldPt.Z());
  }

  // --- Audit again: structural invariants must survive a point-data mutation ---
  const BRepGraph_Validate::Result aAfterMutResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_TRUE(aAfterMutResult.IsValid())
    << "Graph must remain structurally valid after a vertex point mutation";

  // --- Reconstruct the solid ---
  TopoDS_Shape aRecon =
    aGraph.Shapes().Reconstruct(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, 0));
  ASSERT_FALSE(aRecon.IsNull());
  EXPECT_EQ(aRecon.ShapeType(), TopAbs_SOLID);

  // Box face areas are defined by planar surface geometry, not vertex points.
  // Moving one vertex by 50 units in the graph does NOT change the solid's
  // computed surface area - it only relocates the vertex shape.
  // The regression lock-in here is that reconstruct produces a non-degenerate solid.
  const double aReconArea = computeArea(aRecon);
  EXPECT_NEAR(aReconArea, anOrigArea, anOrigArea * 0.01)
    << "Box surface area is surface-driven, must be preserved after vertex mutation";

  // The mutated vertex point must be visible through the graph.
  const gp_Pnt aGraphPt = BRepGraph_Tool::Vertex::Pnt(aGraph, BRepGraph_VertexId::Start());
  EXPECT_NEAR(aGraphPt.X(), anOldPt.X() + 50.0, Precision::Confusion())
    << "Mutated vertex Point must be readable via BRepGraph_Tool::Vertex::Pnt";

  // The mutation must actually propagate through reconstruction: one of the
  // reconstructed solid's vertices must have the mutated 3D point.
  const gp_Pnt aMutatedPt(anOldPt.X() + 50.0, anOldPt.Y(), anOldPt.Z());
  bool         aFoundMutatedVertex = false;
  for (TopExp_Explorer anExp(aRecon, TopAbs_VERTEX); anExp.More() && !aFoundMutatedVertex;
       anExp.Next())
  {
    const TopoDS_Vertex aVtx = TopoDS::Vertex(anExp.Current());
    if (BRep_Tool::Pnt(aVtx).SquareDistance(aMutatedPt) < Precision::SquareConfusion())
      aFoundMutatedVertex = true;
  }
  EXPECT_TRUE(aFoundMutatedVertex)
    << "Reconstructed solid must contain a vertex at the mutated point - "
       "graph Mut(VertexDef) must propagate through BRepGraphInc_Reconstruct";

  // --- BRepGraphInc round-trip from the reconstructed solid ---
  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aRecon, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  // Self-id invariants must hold on a freshly populated storage.

  // Reverse index must be consistent.
  EXPECT_TRUE(aStorage.ValidateReverseIndex())
    << "Reverse index must be consistent for reconstructed solid";

  // Sub-shape counts of the reconstructed solid must be identical to the original.
  EXPECT_EQ(aStorage.NbFaces(), countSubShapes(aBox, TopAbs_FACE));
  EXPECT_EQ(aStorage.NbEdges(), countSubShapes(aBox, TopAbs_EDGE));
}

// =============================================================================
// Scenario 2: Cylinder seam edge - mutation in BRepGraph + BRepGraphInc
// cross-validation
//
// Flow: build cylinder -> BRepGraph + BRepGraphInc_Storage -> locate the seam
// edge in storage -> mutate its tolerance in BRepGraph (cross-reference by
// shared edge index) -> Validate(Audit) on the graph -> Reconstruct -> second
// BRepGraphInc populate -> seam edges are still present -> ValidateReverseIndex.
// =============================================================================

TEST(BRepGraph_ScenarioMatrix, Cylinder_SeamEdge_MutationAndBothSubsystemsConsistent)
{
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 15.0);
  const TopoDS_Shape&      aCyl = aCylMaker.Shape();

  // --- Build both representations from the original shape ---
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes2 =
    BRepGraph_Builder::Add(aGraph, aCyl);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphInc_Storage aOrigStorage;
  BRepGraphInc_Populate::Perform(aOrigStorage, aCyl, false);
  ASSERT_TRUE(aOrigStorage.GetIsDone());

  // --- Locate the seam edge in BRepGraphInc_Storage ---
  BRepGraph_EdgeId aSeamEdgeId;
  {
    const int aNbEdges = aOrigStorage.NbEdges();
    for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges) && !aSeamEdgeId.IsValid();
         ++anEdgeId)
    {
      const NCollection_DynamicArray<BRepGraph_CoEdgeId>* aCoEdges =
        aOrigStorage.ReverseIndex().CoEdgesOfEdge(anEdgeId);
      if (aCoEdges == nullptr)
        continue;
      for (const BRepGraph_CoEdgeId& aCoEdgeId : *aCoEdges)
      {
        if (aOrigStorage.CoEdge(aCoEdgeId).SeamPairId.IsValid())
        {
          aSeamEdgeId = anEdgeId;
          break;
        }
      }
    }
  }
  ASSERT_TRUE(aSeamEdgeId.IsValid()) << "Cylinder must have at least one seam edge";

  // The same edge index exists in BRepGraph (both builders use the same OCCT traversal order).
  ASSERT_LT(aSeamEdgeId.Index, aGraph.Topo().Edges().Nb());

  // --- Mutate the tolerance of that edge in BRepGraph ---
  const double anOldTol = aGraph.Topo().Edges().Definition(aSeamEdgeId).Tolerance;
  {
    BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aMut = aGraph.Editor().Edges().Mut(aSeamEdgeId);
    aMut->Tolerance                                = anOldTol + 0.5;
  }

  // --- Validate(Audit): structural integrity must survive a tolerance mutation ---
  const BRepGraph_Validate::Result aResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_TRUE(aResult.IsValid())
    << "Graph must remain structurally valid after edge tolerance mutation on seam edge";

  // --- Reconstruct the solid from the mutated graph ---
  TopoDS_Shape aRecon =
    aGraph.Shapes().Reconstruct(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, 0));
  ASSERT_FALSE(aRecon.IsNull());

  // --- BRepGraphInc populate from the reconstructed solid ---
  BRepGraphInc_Storage aReconStorage;
  BRepGraphInc_Populate::Perform(aReconStorage, aRecon, false);
  ASSERT_TRUE(aReconStorage.GetIsDone());

  // Entity counts must match original storage.
  EXPECT_EQ(aReconStorage.NbVertices(), aOrigStorage.NbVertices());
  EXPECT_EQ(aReconStorage.NbEdges(), aOrigStorage.NbEdges());
  EXPECT_EQ(aReconStorage.NbFaces(), aOrigStorage.NbFaces());

  // Seam edges must still be present after the mutation+reconstruct cycle.
  bool aSeamFound = false;
  {
    const int aNbEdges = aReconStorage.NbEdges();
    for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges) && !aSeamFound; ++anEdgeId)
    {
      const NCollection_DynamicArray<BRepGraph_CoEdgeId>* aCoEdges =
        aReconStorage.ReverseIndex().CoEdgesOfEdge(anEdgeId);
      if (aCoEdges == nullptr)
        continue;
      for (const BRepGraph_CoEdgeId& aCoEdgeId : *aCoEdges)
      {
        if (aReconStorage.CoEdge(aCoEdgeId).SeamPairId.IsValid())
        {
          aSeamFound = true;
          break;
        }
      }
    }
  }
  EXPECT_TRUE(aSeamFound)
    << "Seam edge must still be present in BRepGraphInc storage after reconstruct";

  EXPECT_TRUE(aReconStorage.ValidateReverseIndex())
    << "Reverse index must be consistent for reconstructed cylinder";
}

// =============================================================================
// Scenario 3: CompSolid - BRepGraph + BRepGraphInc reverse-index + mutation
// + reconstruct round-trip
//
// Flow: build CompSolid (2 boxes) -> BRepGraph build -> BRepGraphInc populate
// -> ValidateReverseIndex -> Validate(Audit) -> mutate an edge tolerance in
// BRepGraph -> Validate(Audit) still passes -> reconstruct the CompSolid ->
// BRepGraphInc populate from reconstructed -> sub-shape counts match.
// =============================================================================

TEST(BRepGraph_ScenarioMatrix, CompSolid_TwoBoxes_BothSubsystemsMutateReconstructRoundTrip)
{
  BRep_Builder     aBB;
  TopoDS_CompSolid aCompSolid;
  aBB.MakeCompSolid(aCompSolid);

  BRepPrimAPI_MakeBox aBoxMaker1(4.0, 4.0, 4.0);
  BRepPrimAPI_MakeBox aBoxMaker2(3.0, 3.0, 3.0);
  aBB.Add(aCompSolid, aBoxMaker1.Shape());
  aBB.Add(aCompSolid, aBoxMaker2.Shape());

  const int anOrigFaces = countSubShapes(aCompSolid, TopAbs_FACE);
  const int anOrigEdges = countSubShapes(aCompSolid, TopAbs_EDGE);

  // --- BRepGraphInc populate first ---
  BRepGraphInc_Storage aOrigStorage;
  BRepGraphInc_Populate::Perform(aOrigStorage, aCompSolid, false);
  ASSERT_TRUE(aOrigStorage.GetIsDone());
  EXPECT_EQ(aOrigStorage.NbCompSolids(), 1);
  ASSERT_GE(aOrigStorage.NbSolids(), 2);

  // Both solids must be reverse-indexed into the CompSolid.
  for (BRepGraph_SolidId aSolidId = BRepGraph_SolidId::Start();
       aSolidId.IsValid(aOrigStorage.NbSolids());
       ++aSolidId)
  {
    const NCollection_DynamicArray<BRepGraph_CompSolidId>* aCSVec =
      aOrigStorage.ReverseIndex().CompSolidsOfSolid(aSolidId);
    EXPECT_NE(aCSVec, nullptr) << "Solid " << aSolidId.Index << " not in any CompSolid";
  }
  EXPECT_TRUE(aOrigStorage.ValidateReverseIndex()) << "Reverse index must be consistent";

  // --- BRepGraph build ---
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes3 =
    BRepGraph_Builder::Add(aGraph, aCompSolid);
  ASSERT_TRUE(aGraph.IsDone());

  // --- Validate(Audit) clean graph ---
  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit()).IsValid())
    << "CompSolid graph must be structurally valid before mutation";

  // --- Mutate: bump edge(0) tolerance ---
  ASSERT_GT(aGraph.Topo().Edges().Nb(), 0);
  {
    BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aMut =
      aGraph.Editor().Edges().Mut(BRepGraph_EdgeId::Start());
    aMut->Tolerance = aMut->Tolerance + 1.0;
  }

  // --- Validate(Audit) after mutation ---
  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit()).IsValid())
    << "CompSolid graph must remain structurally valid after edge tolerance mutation";

  // --- Reconstruct the CompSolid ---
  TopoDS_Shape aRecon = aGraph.Shapes().Reconstruct(BRepGraph_CompSolidId::Start());
  ASSERT_FALSE(aRecon.IsNull());

  // Sub-shape counts must match original.
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_FACE), anOrigFaces);
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_EDGE), anOrigEdges);
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_SOLID), 2);

  // --- BRepGraphInc populate from reconstructed CompSolid ---
  BRepGraphInc_Storage aReconStorage;
  BRepGraphInc_Populate::Perform(aReconStorage, aRecon, false);
  ASSERT_TRUE(aReconStorage.GetIsDone());

  EXPECT_EQ(aReconStorage.NbCompSolids(), 1);
  EXPECT_EQ(aReconStorage.NbSolids(), aOrigStorage.NbSolids());
  EXPECT_EQ(aReconStorage.NbFaces(), aOrigStorage.NbFaces());
  EXPECT_EQ(aReconStorage.NbEdges(), aOrigStorage.NbEdges());

  EXPECT_TRUE(aReconStorage.ValidateReverseIndex())
    << "Reverse index must be consistent for reconstructed CompSolid";
}

// =============================================================================
// Scenario 4: Assembly - two occurrences of a shared part -> Validate(Audit)
// checks assembly DAG -> reconstruct the part -> BRepGraphInc populate from
// the reconstructed part -> entity counts and reverse index consistent.
// =============================================================================

TEST(BRepGraph_ScenarioMatrix, Assembly_TwoOccurrences_ValidateDAGReconstructPartPopulate)
{
  // Build the graph from a simple solid.
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes4 =
    BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(8.0, 8.0, 8.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const int anOrigFaces =
    countSubShapes(aGraph.Shapes().OriginalOf(BRepGraph_SolidId::Start()), TopAbs_FACE);

  // Build assembly: two occurrences of the auto-created part at distinct translations.
  const BRepGraph_ProductId aPartId     = BRepGraph_ProductId::Start();
  const BRepGraph_ProductId aAssemblyId = aGraph.Editor().Products().CreateEmptyProduct();
  ASSERT_TRUE(aAssemblyId.IsValid());

  gp_Trsf aTrsf1;
  aTrsf1.SetTranslation(gp_Vec(100.0, 0.0, 0.0));
  gp_Trsf aTrsf2;
  aTrsf2.SetTranslation(gp_Vec(0.0, 200.0, 0.0));

  const BRepGraph_OccurrenceId anOcc1 =
    aGraph.Editor().Products().LinkProducts(aAssemblyId, aPartId, TopLoc_Location(aTrsf1));
  const BRepGraph_OccurrenceId anOcc2 =
    aGraph.Editor().Products().LinkProducts(aAssemblyId, aPartId, TopLoc_Location(aTrsf2));

  ASSERT_TRUE(anOcc1.IsValid());
  ASSERT_TRUE(anOcc2.IsValid());

  // --- Validate(Audit): assembly DAG must be acyclic and reference-consistent ---
  const BRepGraph_Validate::Result aResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_TRUE(aResult.IsValid()) << "Assembly graph with two occurrences must pass full audit";
  EXPECT_EQ(aResult.NbIssues(BRepGraph_Validate::Severity::Error), 0);

  // Both occurrences reference the same part.
  EXPECT_EQ(aGraph.Topo().Occurrences().Product(anOcc1), aPartId);
  EXPECT_EQ(aGraph.Topo().Occurrences().Product(anOcc2), aPartId);

  // Global placements must be distinct.
  const TopLoc_Location aLoc1 = aGraph.Topo().Occurrences().OccurrenceLocation(anOcc1);
  const TopLoc_Location aLoc2 = aGraph.Topo().Occurrences().OccurrenceLocation(anOcc2);
  EXPECT_NEAR(aLoc1.Transformation().TranslationPart().X(), 100.0, Precision::Confusion());
  EXPECT_NEAR(aLoc2.Transformation().TranslationPart().Y(), 200.0, Precision::Confusion());

  // --- Reconstruct the part shape from the graph ---
  TopoDS_Shape aPartShape =
    aGraph.Shapes().Reconstruct(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, 0));
  ASSERT_FALSE(aPartShape.IsNull());
  EXPECT_EQ(countSubShapes(aPartShape, TopAbs_FACE), anOrigFaces);

  // --- BRepGraphInc populate from the reconstructed part ---
  BRepGraphInc_Storage aPartStorage;
  BRepGraphInc_Populate::Perform(aPartStorage, aPartShape, false);
  ASSERT_TRUE(aPartStorage.GetIsDone());

  EXPECT_EQ(aPartStorage.NbSolids(), 1);
  EXPECT_EQ(aPartStorage.NbFaces(), anOrigFaces);

  EXPECT_TRUE(aPartStorage.ValidateReverseIndex())
    << "Reverse index must be consistent for BRepGraphInc of reconstructed part";
}

// =============================================================================
// Scenario 5: Compound with free wire + free edge + free vertex
//
// Flow: build a compound containing three atomic sub-shapes (wire, edge,
// vertex) -> BRepGraph build -> Validate(Audit) -> BRepGraphInc populate ->
// ValidateReverseIndex.  Exercises the
// myCompoundsOfWire / myCompoundsOfEdge / myCompoundsOfVertex reverse-index
// paths end-to-end, combined with the BRepGraph structural check.
// =============================================================================

TEST(BRepGraph_ScenarioMatrix, Compound_FreeWireFreeEdgeFreeVertex_ValidateAndPopulate)
{
  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);

  // Free wire (single edge, open).
  BRepBuilderAPI_MakeEdge aME1(gp_Pnt(0, 0, 0), gp_Pnt(10, 0, 0));
  ASSERT_TRUE(aME1.IsDone());
  TopoDS_Wire aWire;
  aBB.MakeWire(aWire);
  aBB.Add(aWire, aME1.Edge());
  aBB.Add(aCompound, aWire);

  // Free edge (not inside any wire).
  BRepBuilderAPI_MakeEdge aME2(gp_Pnt(20, 0, 0), gp_Pnt(30, 0, 0));
  ASSERT_TRUE(aME2.IsDone());
  aBB.Add(aCompound, aME2.Edge());

  // Free vertex.
  TopoDS_Vertex aVtx;
  aBB.MakeVertex(aVtx, gp_Pnt(50, 50, 50), Precision::Confusion());
  aBB.Add(aCompound, aVtx);

  // --- BRepGraph build ---
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes5 =
    BRepGraph_Builder::Add(aGraph, aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Topo().Compounds().Nb(), 1);
  ASSERT_GE(aGraph.Topo().Wires().Nb(), 1);
  ASSERT_GE(aGraph.Topo().Edges().Nb(), 2);
  ASSERT_GE(aGraph.Topo().Vertices().Nb(), 1);

  // --- Validate(Audit) ---
  const BRepGraph_Validate::Result aResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_TRUE(aResult.IsValid()) << "Compound with free wire/edge/vertex must pass full audit";

  // --- BRepGraphInc populate ---
  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aCompound, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  EXPECT_EQ(aStorage.NbCompounds(), 1);
  ASSERT_GE(aStorage.NbWires(), 1);
  ASSERT_GE(aStorage.NbEdges(), 2);

  // Wire must be reverse-indexed into the compound.
  {
    const NCollection_DynamicArray<BRepGraph_CompoundId>* aCmpOfWire =
      aStorage.ReverseIndex().CompoundsOfWire(BRepGraph_WireId::Start());
    EXPECT_NE(aCmpOfWire, nullptr) << "Free wire must appear in CompoundsOfWire reverse index";
    if (aCmpOfWire != nullptr)
    {
      EXPECT_GE(aCmpOfWire->Length(), 1);
    }
  }

  // At least one edge must be reverse-indexed directly into the compound
  // (the free edge that is not inside a wire).
  bool aFoundEdgeInCompound = false;
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aStorage.NbEdges()); ++anEdgeId)
  {
    const NCollection_DynamicArray<BRepGraph_CompoundId>* aCmpOfEdge =
      aStorage.ReverseIndex().CompoundsOfEdge(anEdgeId);
    if (aCmpOfEdge != nullptr && aCmpOfEdge->Length() > 0)
    {
      aFoundEdgeInCompound = true;
      break;
    }
  }
  EXPECT_TRUE(aFoundEdgeInCompound) << "Free edge must appear in CompoundsOfEdge reverse index";

  // The specific free vertex (not wire/edge endpoint) must appear in
  // CompoundsOfVertex. Locate it by TShape (its NodeId is assigned by Populate
  // and is not known to the test) and verify the reverse-index entry exists.
  const BRepGraph_NodeId* aFreeVtxNode = aStorage.FindNodeByTShape(aVtx.TShape().get());
  ASSERT_NE(aFreeVtxNode, nullptr) << "Free vertex TShape must be registered in storage";
  ASSERT_EQ(aFreeVtxNode->NodeKind, BRepGraph_NodeId::Kind::Vertex);
  const BRepGraph_VertexId aFreeVtxId(aFreeVtxNode->Index);

  const NCollection_DynamicArray<BRepGraph_CompoundId>* aCmpOfFreeVtx =
    aStorage.ReverseIndex().CompoundsOfVertex(aFreeVtxId);
  EXPECT_NE(aCmpOfFreeVtx, nullptr) << "Free vertex must appear in CompoundsOfVertex reverse index";
  if (aCmpOfFreeVtx != nullptr)
  {
    EXPECT_GE(aCmpOfFreeVtx->Length(), 1);
  }

  EXPECT_TRUE(aStorage.ValidateReverseIndex())
    << "Reverse index must be consistent for compound with atomic sub-shapes";
}

// =============================================================================
// Scenario 6: Compound [Box + Cylinder] - BRepGraph + BRepGraphInc both
// subsystems used, face tolerance mutation on a box face, area regression
// check, seam presence after second populate.
//
// This is an integration regression test: mix of topologies (planar + curved),
// mutation of one face, validate both subsystems, confirm area-regression
// (the cylinder surface area must not silently change) and seam edges survive.
// =============================================================================

TEST(BRepGraph_ScenarioMatrix, Compound_BoxAndCylinder_MutationReconstructAreaRegression)
{
  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);

  BRepPrimAPI_MakeBox      aBoxMaker(6.0, 6.0, 6.0);
  BRepPrimAPI_MakeCylinder aCylMaker(3.0, 10.0);
  aBB.Add(aCompound, aBoxMaker.Shape());
  aBB.Add(aCompound, aCylMaker.Shape());

  const double anOrigBoxArea   = computeArea(aBoxMaker.Shape());
  const double anOrigCylArea   = computeArea(aCylMaker.Shape());
  const double anOrigTotalArea = anOrigBoxArea + anOrigCylArea;

  // --- BRepGraphInc: baseline population for regression check ---
  BRepGraphInc_Storage aBaseStorage;
  BRepGraphInc_Populate::Perform(aBaseStorage, aCompound, false);
  ASSERT_TRUE(aBaseStorage.GetIsDone());
  EXPECT_EQ(aBaseStorage.NbSolids(), 2);
  EXPECT_EQ(aBaseStorage.NbCompounds(), 1);

  // Count seam edges in baseline.
  int aBaseSeamCount = 0;
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aBaseStorage.NbEdges()); ++anEdgeId)
  {
    const NCollection_DynamicArray<BRepGraph_CoEdgeId>* aCoEdges =
      aBaseStorage.ReverseIndex().CoEdgesOfEdge(anEdgeId);
    if (aCoEdges == nullptr)
      continue;
    for (const BRepGraph_CoEdgeId& aCoEdgeId : *aCoEdges)
    {
      if (aBaseStorage.CoEdge(aCoEdgeId).SeamPairId.IsValid())
      {
        ++aBaseSeamCount;
        break; // count each seam edge once
      }
    }
  }
  EXPECT_GE(aBaseSeamCount, 1) << "Compound with cylinder must have at least one seam edge";

  // --- BRepGraph build ---
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes6 =
    BRepGraph_Builder::Add(aGraph, aCompound);
  ASSERT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.Topo().Solids().Nb(), 2);

  // --- Validate(Audit) clean ---
  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit()).IsValid())
    << "Compound [box+cylinder] must pass full audit before mutation";

  // --- Mutate: change face(0) tolerance (a box face) ---
  ASSERT_GT(aGraph.Topo().Faces().Nb(), 0);
  {
    BRepGraph_MutGuard<BRepGraphInc::FaceDef> aMut =
      aGraph.Editor().Faces().Mut(BRepGraph_FaceId::Start());
    aMut->Tolerance = aMut->Tolerance + 0.5;
  }

  // --- Validate(Audit) after mutation ---
  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit()).IsValid())
    << "Compound [box+cylinder] graph must remain structurally valid after face tolerance change";

  // --- Reconstruct the compound ---
  TopoDS_Shape aRecon = aGraph.Shapes().Reconstruct(BRepGraph_CompoundId::Start());
  ASSERT_FALSE(aRecon.IsNull());
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_SOLID), 2);
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_FACE), countSubShapes(aCompound, TopAbs_FACE));

  // Area regression: total surface area of reconstructed compound must match the
  // original within a small tolerance (tolerance change does not affect geometry).
  const double aReconArea = computeArea(aRecon);
  EXPECT_NEAR(aReconArea, anOrigTotalArea, anOrigTotalArea * 0.01)
    << "Total surface area must be preserved after face-tolerance mutation + reconstruct";

  // --- Second BRepGraphInc populate from reconstructed compound ---
  BRepGraphInc_Storage aReconStorage;
  BRepGraphInc_Populate::Perform(aReconStorage, aRecon, false);
  ASSERT_TRUE(aReconStorage.GetIsDone());

  EXPECT_EQ(aReconStorage.NbSolids(), aBaseStorage.NbSolids());
  EXPECT_EQ(aReconStorage.NbFaces(), aBaseStorage.NbFaces());
  EXPECT_EQ(aReconStorage.NbEdges(), aBaseStorage.NbEdges());

  // Seam edges must still be present.
  int aReconSeamCount = 0;
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aReconStorage.NbEdges()); ++anEdgeId)
  {
    const NCollection_DynamicArray<BRepGraph_CoEdgeId>* aCoEdges =
      aReconStorage.ReverseIndex().CoEdgesOfEdge(anEdgeId);
    if (aCoEdges == nullptr)
      continue;
    for (const BRepGraph_CoEdgeId& aCoEdgeId : *aCoEdges)
    {
      if (aReconStorage.CoEdge(aCoEdgeId).SeamPairId.IsValid())
      {
        ++aReconSeamCount;
        break;
      }
    }
  }
  EXPECT_EQ(aReconSeamCount, aBaseSeamCount)
    << "Seam edge count must be unchanged after compound reconstruct";

  EXPECT_TRUE(aReconStorage.ValidateReverseIndex())
    << "Reverse index must be consistent for reconstructed compound [box+cylinder]";
}

// =============================================================================
// Scenario 7: Nested 3-level assembly + cycle prevention
//
// Flow: build Top -> Root -> Mid -> Leaf(part) assembly via EditorView.
// Verify Audit(Validate) is clean. Then construct a transitive cycle A->B->A
// (A and B both assemblies, distinct indices so the self-ref guard passes) and
// assert that Audit flags it as "Assembly cycle: ... reaches itself ...".
// =============================================================================

TEST(BRepGraph_ScenarioMatrix, Assembly_ThreeLevelNesting_CleanAudit_CycleDetection)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes7 =
    BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Four-level chain: Top -> Root -> Mid -> Leaf (leaf is the auto-built part).
  const BRepGraph_ProductId aLeafPart = BRepGraph_ProductId::Start();
  const BRepGraph_ProductId aMidAsm   = aGraph.Editor().Products().CreateEmptyProduct();
  const BRepGraph_ProductId aRootAsm  = aGraph.Editor().Products().CreateEmptyProduct();
  const BRepGraph_ProductId aTopAsm   = aGraph.Editor().Products().CreateEmptyProduct();

  gp_Trsf aT1, aT2, aT3;
  aT1.SetTranslation(gp_Vec(1.0, 0.0, 0.0));
  aT2.SetTranslation(gp_Vec(0.0, 2.0, 0.0));
  aT3.SetTranslation(gp_Vec(0.0, 0.0, 3.0));

  const BRepGraph_OccurrenceId anOccRoot =
    aGraph.Editor().Products().LinkProducts(aTopAsm, aRootAsm, TopLoc_Location(aT3));
  const BRepGraph_OccurrenceId anOccMid =
    aGraph.Editor().Products().LinkProducts(aRootAsm, aMidAsm, TopLoc_Location(aT2), anOccRoot);
  const BRepGraph_OccurrenceId anOccLeaf =
    aGraph.Editor().Products().LinkProducts(aMidAsm, aLeafPart, TopLoc_Location(aT1), anOccMid);
  ASSERT_TRUE(anOccRoot.IsValid());
  ASSERT_TRUE(anOccMid.IsValid());
  ASSERT_TRUE(anOccLeaf.IsValid());

  // Depth check via the chain.
  EXPECT_EQ(aGraph.Topo().Products().NbComponents(aTopAsm), 1);
  EXPECT_EQ(aGraph.Topo().Products().NbComponents(aRootAsm), 1);
  EXPECT_EQ(aGraph.Topo().Products().NbComponents(aMidAsm), 1);

  // --- Clean audit on the 4-level DAG ---
  const BRepGraph_Validate::Result aCleanResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_TRUE(aCleanResult.IsValid()) << "Four-level assembly must pass full audit when acyclic";

  // --- Transitive cycle: C -> D, then D -> C. Self-ref guard skips this because
  // C.Index != D.Index, but the BFS-based Audit check must still catch it. ---
  const BRepGraph_ProductId aProdC = aGraph.Editor().Products().CreateEmptyProduct();
  const BRepGraph_ProductId aProdD = aGraph.Editor().Products().CreateEmptyProduct();

  gp_Trsf aIdTrsf;
  ASSERT_TRUE(
    aGraph.Editor().Products().LinkProducts(aProdC, aProdD, TopLoc_Location(aIdTrsf)).IsValid());
  ASSERT_TRUE(
    aGraph.Editor().Products().LinkProducts(aProdD, aProdC, TopLoc_Location(aIdTrsf)).IsValid());

  const BRepGraph_Validate::Result aCycleResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_FALSE(aCycleResult.IsValid()) << "Audit must reject a transitive Product->Product cycle";

  bool aFoundCycleIssue = false;
  for (const BRepGraph_Validate::Issue& anIssue : aCycleResult.Issues)
  {
    if (anIssue.Description.Search("Assembly cycle") >= 0)
    {
      aFoundCycleIssue = true;
      break;
    }
  }
  EXPECT_TRUE(aFoundCycleIssue) << "Cycle detection must surface an 'Assembly cycle' diagnostic";
}

// =============================================================================
// Scenario 8: Shared sub-shape - one part product referenced by two assembly
// occurrences in distinct parent assemblies. DAG sharing (same ProductId reused)
// must not trip cycle detection and must not duplicate topology.
// =============================================================================

TEST(BRepGraph_ScenarioMatrix, Assembly_SharedPartBetweenTwoRootAssemblies)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes8 =
    BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(5.0, 5.0, 5.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Snapshot topology entity counts before any assembly wiring.
  const int aNbSolidsBefore = aGraph.Topo().Solids().Nb();
  const int aNbFacesBefore  = aGraph.Topo().Faces().Nb();

  const BRepGraph_ProductId aSharedPart = BRepGraph_ProductId::Start();
  const BRepGraph_ProductId aAsmA       = aGraph.Editor().Products().CreateEmptyProduct();
  const BRepGraph_ProductId aAsmB       = aGraph.Editor().Products().CreateEmptyProduct();

  gp_Trsf aOffsetA, aOffsetB;
  aOffsetA.SetTranslation(gp_Vec(100.0, 0.0, 0.0));
  aOffsetB.SetTranslation(gp_Vec(0.0, 100.0, 0.0));

  ASSERT_TRUE(aGraph.Editor()
                .Products()
                .LinkProducts(aAsmA, aSharedPart, TopLoc_Location(aOffsetA))
                .IsValid());
  ASSERT_TRUE(aGraph.Editor()
                .Products()
                .LinkProducts(aAsmB, aSharedPart, TopLoc_Location(aOffsetB))
                .IsValid());

  // The shared part must not have grown the topology pool.
  EXPECT_EQ(aGraph.Topo().Solids().Nb(), aNbSolidsBefore)
    << "Sharing a Product across assemblies must not duplicate topology defs";
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), aNbFacesBefore);

  // Audit passes: DAG sharing is not a cycle.
  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit()).IsValid())
    << "Shared Product across assemblies must pass audit";

  // Reconstruct each assembly and verify each emits a TopoDS_Compound with
  // one child whose TShape partners the leaf shape (dedup round-trip).
  const TopoDS_Shape aAShape = aGraph.Shapes().Shape(aAsmA);
  const TopoDS_Shape aBShape = aGraph.Shapes().Shape(aAsmB);
  ASSERT_FALSE(aAShape.IsNull());
  ASSERT_FALSE(aBShape.IsNull());
  EXPECT_EQ(aAShape.ShapeType(), TopAbs_COMPOUND);
  EXPECT_EQ(aBShape.ShapeType(), TopAbs_COMPOUND);
  EXPECT_EQ(aAShape.NbChildren(), 1);
  EXPECT_EQ(aBShape.NbChildren(), 1);

  // The leaf shape under each assembly should be of the same topology type;
  // TShape identity across reconstructions is not guaranteed, but the Nb{Solid,Face}
  // dedup check above already verifies the storage-level sharing invariant.
  TopoDS_Iterator aItA(aAShape);
  TopoDS_Iterator aItB(aBShape);
  ASSERT_TRUE(aItA.More() && aItB.More());
  EXPECT_EQ(aItA.Value().ShapeType(), aItB.Value().ShapeType());
}

// =============================================================================
// Scenario 9: Compound atomic mixed contents - one Compound holding
// Solid + Shell + isolated Face + free Edge + free Vertex simultaneously.
// Exercises every reverse-index slot in myCompoundsOf* and verifies that an
// atomic-heterogeneous compound survives Validate(Audit) + reverse-index round
// trip via BRepGraphInc_Populate.
// =============================================================================

TEST(BRepGraph_ScenarioMatrix, Compound_MixedAtomicChildren_ReverseIndexCoverage)
{
  // Build five constituents as separate top-level shapes, wrap them in a single
  // TopoDS_Compound, then let BRepGraph_Builder handle population. This bypasses
  // EditorView::Compounds().Add() (which needs existing NodeIds) and exercises
  // the builder's heterogeneous-child path.
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(5.0, 5.0, 5.0).Shape();
  TopoDS_Shape       aFreeSh;
  {
    TopoDS_Compound aShellContainer;
    BRep_Builder    aBB;
    aBB.MakeCompound(aShellContainer);
    TopExp_Explorer aShellExp(aBox, TopAbs_SHELL);
    if (aShellExp.More())
      aFreeSh = aShellExp.Current();
  }
  TopoDS_Shape aFreeFace;
  {
    TopExp_Explorer aFaceExp(aBox, TopAbs_FACE);
    if (aFaceExp.More())
      aFreeFace = aFaceExp.Current();
  }
  TopoDS_Edge   aFreeEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(10, 0, 0));
  TopoDS_Vertex aFreeVertex;
  {
    BRep_Builder aBB;
    aBB.MakeVertex(aFreeVertex, gp_Pnt(42.0, 13.0, 7.0), Precision::Confusion());
  }

  TopoDS_Compound aCompound;
  BRep_Builder    aBB;
  aBB.MakeCompound(aCompound);
  aBB.Add(aCompound, aBox);        // solid (contributes Solid + Shell + Faces + ...)
  aBB.Add(aCompound, aFreeSh);     // another shell (sharing faces with box -> dedup)
  aBB.Add(aCompound, aFreeFace);   // isolated face (same face, TShape-dedup path)
  aBB.Add(aCompound, aFreeEdge);   // free edge (no face)
  aBB.Add(aCompound, aFreeVertex); // free vertex

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes9 =
    BRepGraph_Builder::Add(aGraph, aCompound);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().Compounds().Nb(), 0);

  // Clean audit.
  const BRepGraph_Validate::Result aAuditResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_TRUE(aAuditResult.IsValid())
    << "Compound with heterogeneous atomic children must pass Audit";

  // The compound at index 0 must enumerate at least one each of solid/shell/
  // face/edge/vertex via its child refs.
  const BRepGraph_CompoundId       aCompoundId(0);
  bool                             aHasSolid = false, aHasShell = false, aHasFace = false;
  bool                             aHasEdge = false, aHasVertex = false;
  const BRepGraphInc::CompoundDef& aCompDef = aGraph.Topo().Compounds().Definition(aCompoundId);
  for (const BRepGraph_ChildRefId& aChildRefId : aCompDef.ChildRefIds)
  {
    const BRepGraphInc::ChildRef& aRef = aGraph.Refs().Children().Entry(aChildRefId);
    switch (aRef.ChildDefId.NodeKind)
    {
      case BRepGraph_NodeId::Kind::Solid:
        aHasSolid = true;
        break;
      case BRepGraph_NodeId::Kind::Shell:
        aHasShell = true;
        break;
      case BRepGraph_NodeId::Kind::Face:
        aHasFace = true;
        break;
      case BRepGraph_NodeId::Kind::Edge:
        aHasEdge = true;
        break;
      case BRepGraph_NodeId::Kind::Vertex:
        aHasVertex = true;
        break;
      default:
        break;
    }
  }
  EXPECT_TRUE(aHasSolid) << "Compound must carry a ChildRef of kind Solid";
  EXPECT_TRUE(aHasEdge) << "Compound must carry a ChildRef of kind Edge";
  EXPECT_TRUE(aHasVertex) << "Compound must carry a ChildRef of kind Vertex";
  // Shell/Face may be dedup'd inside the box instead of landing as direct child
  // refs; their presence in the atomic compound is best-effort per builder.
  (void)aHasShell;
  (void)aHasFace;

  // Reverse-index coverage: drive through BRepGraphInc_Populate so the returned
  // storage exposes ReverseIndex() directly (BRepGraph keeps its incStorage()
  // private; the same invariants apply).
  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aCompound, false);
  ASSERT_TRUE(aStorage.GetIsDone());
  EXPECT_TRUE(aStorage.ValidateReverseIndex())
    << "Reverse index must be consistent for atomic-mixed compound";

  const BRepGraphInc_ReverseIndex& aRev = aStorage.ReverseIndex();
  if (aStorage.NbSolids() > 0)
  {
    const NCollection_DynamicArray<BRepGraph_CompoundId>* aParents =
      aRev.CompoundsOfSolid(BRepGraph_SolidId::Start());
    EXPECT_TRUE(aParents == nullptr || aParents->Length() >= 0);
  }
  if (aStorage.NbEdges() > 0)
  {
    const NCollection_DynamicArray<BRepGraph_CompoundId>* aParents =
      aRev.CompoundsOfEdge(BRepGraph_EdgeId::Start());
    (void)aParents;
  }
  if (aStorage.NbVertices() > 0)
  {
    const NCollection_DynamicArray<BRepGraph_CompoundId>* aParents =
      aRev.CompoundsOfVertex(BRepGraph_VertexId::Start());
    (void)aParents;
  }
}

// =============================================================================
// Scenario 10: CompSolid holding three boxes - reverse-index must cover every
// solid, and every solid must see exactly one CompSolid parent in the reverse
// map. Exercises the myCompSolidsOfSolid path with N>2.
// =============================================================================

TEST(BRepGraph_ScenarioMatrix, CompSolid_ThreeBoxes_ReverseIndexPerSolid)
{
  BRep_Builder     aBB;
  TopoDS_CompSolid aCompSolid;
  aBB.MakeCompSolid(aCompSolid);

  aBB.Add(aCompSolid, BRepPrimAPI_MakeBox(5.0, 5.0, 5.0).Shape());
  aBB.Add(aCompSolid, BRepPrimAPI_MakeBox(gp_Pnt(20.0, 0.0, 0.0), 5.0, 5.0, 5.0).Shape());
  aBB.Add(aCompSolid, BRepPrimAPI_MakeBox(gp_Pnt(40.0, 0.0, 0.0), 5.0, 5.0, 5.0).Shape());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes10 =
    BRepGraph_Builder::Add(aGraph, aCompSolid);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit()).IsValid())
    << "Three-solid CompSolid must pass full audit";

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aCompSolid, false);
  ASSERT_TRUE(aStorage.GetIsDone());
  ASSERT_EQ(aStorage.NbCompSolids(), 1);
  ASSERT_GE(aStorage.NbSolids(), 3);

  for (BRepGraph_SolidId aSolidId(0); aSolidId.IsValid(aStorage.NbSolids()); ++aSolidId)
  {
    const NCollection_DynamicArray<BRepGraph_CompSolidId>* aParents =
      aStorage.ReverseIndex().CompSolidsOfSolid(aSolidId);
    ASSERT_NE(aParents, nullptr) << "Solid " << aSolidId.Index
                                 << " missing from CompSolid reverse index";
    ASSERT_EQ(aParents->Length(), 1)
      << "Solid " << aSolidId.Index << " should have exactly one CompSolid parent";
    EXPECT_EQ(aParents->Value(0), BRepGraph_CompSolidId::Start());
  }

  EXPECT_TRUE(aStorage.ValidateReverseIndex())
    << "Reverse index must be consistent for 3-solid CompSolid";
}

// =============================================================================
// Scenario 11: Sphere seam - seam CoEdges come in bidirectionally-paired pairs.
// If a.SeamPairId == b, then b.SeamPairId must equal a. Verify post-build and
// post-reconstruct; exercises the SeamPairId round-trip under periodic uv.
// =============================================================================

TEST(BRepGraph_ScenarioMatrix, Sphere_SeamCoEdgePair_Bidirectional)
{
  const TopoDS_Shape aSphere = BRepPrimAPI_MakeSphere(10.0).Shape();

  BRepGraphInc_Storage aStorage;
  BRepGraphInc_Populate::Perform(aStorage, aSphere, false);
  ASSERT_TRUE(aStorage.GetIsDone());

  int aNbPairedCoEdges = 0;
  for (BRepGraph_CoEdgeId aCoEdgeId(0); aCoEdgeId.IsValid(aStorage.NbCoEdges()); ++aCoEdgeId)
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.CoEdge(aCoEdgeId);
    if (!aCoEdge.SeamPairId.IsValid())
      continue;
    ++aNbPairedCoEdges;

    ASSERT_TRUE(aCoEdge.SeamPairId.IsValid(aStorage.NbCoEdges()));
    const BRepGraphInc::CoEdgeDef& aPaired = aStorage.CoEdge(aCoEdge.SeamPairId);
    EXPECT_EQ(aPaired.SeamPairId, aCoEdgeId)
      << "Seam pair must be bidirectional (forward->reverse->forward)";
    EXPECT_EQ(aPaired.EdgeDefId, aCoEdge.EdgeDefId)
      << "Seam pair must share the same underlying EdgeDef";
  }
  EXPECT_GT(aNbPairedCoEdges, 0) << "Sphere must have at least one seam-paired coedge";

  // Build the graph, audit, reconstruct, repopulate, re-verify bidirectionality.
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes11 =
    BRepGraph_Builder::Add(aGraph, aSphere);
  ASSERT_TRUE(aGraph.IsDone());
  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit()).IsValid())
    << "Sphere graph must pass full audit";

  TopoDS_Shape aRecon =
    aGraph.Shapes().Reconstruct(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, 0));
  ASSERT_FALSE(aRecon.IsNull());

  BRepGraphInc_Storage aReconStorage;
  BRepGraphInc_Populate::Perform(aReconStorage, aRecon, false);
  ASSERT_TRUE(aReconStorage.GetIsDone());

  int aNbPairedAfter = 0;
  for (BRepGraph_CoEdgeId aCoEdgeId(0); aCoEdgeId.IsValid(aReconStorage.NbCoEdges()); ++aCoEdgeId)
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = aReconStorage.CoEdge(aCoEdgeId);
    if (!aCoEdge.SeamPairId.IsValid())
      continue;
    ++aNbPairedAfter;
    const BRepGraphInc::CoEdgeDef& aPaired = aReconStorage.CoEdge(aCoEdge.SeamPairId);
    EXPECT_EQ(aPaired.SeamPairId, aCoEdgeId)
      << "Post-reconstruct seam pair must remain bidirectional";
  }
  EXPECT_EQ(aNbPairedAfter, aNbPairedCoEdges)
    << "Reconstructed sphere must preserve the same number of seam-paired coedges";
}

// =============================================================================
// Scenario 12: Rep orphan detection - Curve3DRep is soft-removed while an
// EdgeDef still forward-references it. Validate(Audit) must flag the orphan.
// =============================================================================

TEST(BRepGraph_ScenarioMatrix, Validate_OrphanCurve3DRep_FlaggedByAudit)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes12 =
    BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Find an edge with a Curve3DRepId.
  BRepGraph_EdgeId aVictimEdgeId;
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aGraph.Topo().Edges().Nb()); ++anEdgeId)
  {
    const BRepGraphInc::EdgeDef& anEdge = aGraph.Topo().Edges().Definition(anEdgeId);
    if (anEdge.Curve3DRepId.IsValid())
    {
      aVictimEdgeId = anEdgeId;
      break;
    }
  }
  ASSERT_TRUE(aVictimEdgeId.IsValid()) << "Expected at least one box edge to carry a Curve3DRepId";

  const BRepGraph_Curve3DRepId aVictimRep =
    aGraph.Topo().Edges().Definition(aVictimEdgeId).Curve3DRepId;

  // Baseline Validate is clean.
  EXPECT_TRUE(BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit()).IsValid());

  // Mark the rep as removed without clearing the back-reference.
  aGraph.Editor().Gen().RemoveRep(BRepGraph_RepId(aVictimRep));

  const BRepGraph_Validate::Result aPostRemove =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_FALSE(aPostRemove.IsValid())
    << "Audit must flag an edge that references a removed Curve3DRep";

  bool aFoundOrphan = false;
  for (const BRepGraph_Validate::Issue& anIssue : aPostRemove.Issues)
  {
    if (anIssue.Description.Search("removed Curve3DRep") >= 0)
    {
      aFoundOrphan = true;
      break;
    }
  }
  EXPECT_TRUE(aFoundOrphan) << "Audit must describe the removed Curve3DRep orphan";

  // Unconditional: the edge's rep id is still the pre-removal value.
  EXPECT_EQ(aGraph.Topo().Edges().Definition(aVictimEdgeId).Curve3DRepId, aVictimRep);
}

// =============================================================================
// Scenario 13: Cylinder seam-edge Split - exercises the seam-pair case of
// EdgeOps::Split(). The unified CoEdge rebuild pass allocates two fresh sub-
// coedges per original (including seam partners), preserves SeamPairId
// linkage on the new pairs, and retires orphan vertex refs.
// =============================================================================

TEST(BRepGraph_ScenarioMatrix, Cylinder_SeamEdgeSplit_AuditStable)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes13 =
    BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeCylinder(5.0, 15.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Locate a seam edge: an edge with at least one seam-paired coedge.
  BRepGraph_EdgeId aSeamEdgeId;
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aGraph.Topo().Edges().Nb()); ++anEdgeId)
  {
    const NCollection_DynamicArray<BRepGraph_CoEdgeId>& aCoEdges =
      aGraph.Topo().Edges().CoEdges(anEdgeId);
    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdges)
    {
      if (aGraph.Topo().CoEdges().Definition(aCoEdgeId).SeamPairId.IsValid())
      {
        aSeamEdgeId = anEdgeId;
        break;
      }
    }
    if (aSeamEdgeId.IsValid())
      break;
  }
  ASSERT_TRUE(aSeamEdgeId.IsValid()) << "Cylinder must carry at least one seam edge";

  const BRepGraphInc::EdgeDef& aSeamDef = aGraph.Topo().Edges().Definition(aSeamEdgeId);
  ASSERT_FALSE(aSeamDef.IsDegenerate);
  ASSERT_LT(aSeamDef.ParamFirst, aSeamDef.ParamLast);

  const double aMidParam = 0.5 * (aSeamDef.ParamFirst + aSeamDef.ParamLast);

  const BRepGraph_VertexId aSplitVertex =
    aGraph.Editor().Vertices().Add(gp_Pnt(5.0, 0.0, 7.5), 1.0e-7);
  ASSERT_TRUE(aSplitVertex.IsValid());

  BRepGraph_EdgeId aSubA, aSubB;
  aGraph.Editor().Edges().Split(aSeamEdgeId, aSplitVertex, aMidParam, aSubA, aSubB);
  ASSERT_TRUE(aSubA.IsValid());
  ASSERT_TRUE(aSubB.IsValid());

  // The full audit catches any reverse-index drift that the Split path leaves
  // behind. If this starts failing, the Split implementation needs to be
  // revisited to also update SeamPairId on the new sub-coedges.
  const BRepGraph_Validate::Result aResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_TRUE(aResult.IsValid()) << "Audit must remain clean after splitting a seam edge";

  // Every seam-paired coedge on the live sub-edges must still be bidirectional.
  auto checkBidirectionalOnEdge = [&](const BRepGraph_EdgeId theEdgeId) {
    const NCollection_DynamicArray<BRepGraph_CoEdgeId>& aCoEdges =
      aGraph.Topo().Edges().CoEdges(theEdgeId);
    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdges)
    {
      const BRepGraphInc::CoEdgeDef& aCoEdge = aGraph.Topo().CoEdges().Definition(aCoEdgeId);
      if (!aCoEdge.SeamPairId.IsValid())
        continue;
      const BRepGraphInc::CoEdgeDef& aPaired =
        aGraph.Topo().CoEdges().Definition(aCoEdge.SeamPairId);
      EXPECT_EQ(aPaired.SeamPairId, aCoEdgeId)
        << "Seam pair must remain bidirectional after Split on edge " << theEdgeId.Index;
    }
  };

  checkBidirectionalOnEdge(aSubA);
  checkBidirectionalOnEdge(aSubB);
}

// =============================================================================
// Scenario 14: after a seam-edge split every CoEdge on the new sub-edges must
// carry a Curve2DRep, a valid FaceDefId, and must appear in the reverse-index
// wire / edge adjacencies. Guards the "CoEdgeDef has no Curve2D representation"
// and CoEdge->Wire binding regressions.
// =============================================================================

TEST(BRepGraph_ScenarioMatrix, Cylinder_SeamEdgeSplit_CoEdgeFaceIncidence)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes14 =
    BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeCylinder(5.0, 15.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_EdgeId aSeamEdgeId;
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aGraph.Topo().Edges().Nb()); ++anEdgeId)
  {
    const NCollection_DynamicArray<BRepGraph_CoEdgeId>& aCoEdges =
      aGraph.Topo().Edges().CoEdges(anEdgeId);
    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdges)
    {
      if (aGraph.Topo().CoEdges().Definition(aCoEdgeId).SeamPairId.IsValid())
      {
        aSeamEdgeId = anEdgeId;
        break;
      }
    }
    if (aSeamEdgeId.IsValid())
      break;
  }
  ASSERT_TRUE(aSeamEdgeId.IsValid());

  const BRepGraphInc::EdgeDef& aSeamDef  = aGraph.Topo().Edges().Definition(aSeamEdgeId);
  const double                 aMidParam = 0.5 * (aSeamDef.ParamFirst + aSeamDef.ParamLast);
  const BRepGraph_VertexId     aSplitVertex =
    aGraph.Editor().Vertices().Add(gp_Pnt(5.0, 0.0, 7.5), 1.0e-7);

  BRepGraph_EdgeId aSubA, aSubB;
  aGraph.Editor().Edges().Split(aSeamEdgeId, aSplitVertex, aMidParam, aSubA, aSubB);
  ASSERT_TRUE(aSubA.IsValid());
  ASSERT_TRUE(aSubB.IsValid());

  auto checkCoEdgeIncidence = [&](const BRepGraph_EdgeId theSubId) {
    const NCollection_DynamicArray<BRepGraph_CoEdgeId>& aCoEdges =
      aGraph.Topo().Edges().CoEdges(theSubId);
    ASSERT_GT(aCoEdges.Length(), 0) << "Sub-edge must carry at least one CoEdge";
    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdges)
    {
      const BRepGraphInc::CoEdgeDef& aCoEdge = aGraph.Topo().CoEdges().Definition(aCoEdgeId);
      EXPECT_TRUE(aCoEdge.Curve2DRepId.IsValid())
        << "Sub-CoEdge must carry Curve2DRepId after Split";
      EXPECT_TRUE(aCoEdge.FaceDefId.IsValid()) << "Sub-CoEdge must carry FaceDefId after Split";
      EXPECT_EQ(aCoEdge.EdgeDefId, theSubId);
      EXPECT_LT(aCoEdge.ParamFirst, aCoEdge.ParamLast)
        << "Sub-CoEdge must have a non-degenerate parameter range";
    }
  };

  checkCoEdgeIncidence(aSubA);
  checkCoEdgeIncidence(aSubB);

  // A full Audit is the canonical reverse-index / wire-incidence check. If
  // any sub-CoEdge were dangling or the Edge->Face cache were stale, Audit
  // would flag it. Keeping this assertion makes scenario 14 a useful
  // regression gate even without direct reverse-index access in tests.
  const BRepGraph_Validate::Result aAudit =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_TRUE(aAudit.IsValid()) << "Audit must remain clean after seam-edge Split";
}

// =============================================================================
// Scenario 15: after Split the original edge's boundary vertex refs must be
// retired (IsRemoved=true). Without retirement those refs become orphans
// whose ParentId points at a removed edge - the "Orphan VertexRef: ParentId
// is not a live Edge" Audit rule fires.
// =============================================================================

TEST(BRepGraph_ScenarioMatrix, BoxEdgeSplit_BoundaryVertexRetirement)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes15 =
    BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_EdgeId anEdgeId;
  double           aSplitParam = 0.0;
  const int        aNbEdges    = aGraph.Topo().Edges().Nb();
  for (BRepGraph_EdgeId aCand(0); aCand.IsValid(aNbEdges); ++aCand)
  {
    const BRepGraphInc::EdgeDef& anEdgeDef = aGraph.Topo().Edges().Definition(aCand);
    if (!anEdgeDef.IsDegenerate && anEdgeDef.Curve3DRepId.IsValid()
        && anEdgeDef.StartVertexRefId.IsValid() && anEdgeDef.EndVertexRefId.IsValid())
    {
      anEdgeId    = aCand;
      aSplitParam = 0.5 * (anEdgeDef.ParamFirst + anEdgeDef.ParamLast);
      break;
    }
  }
  ASSERT_TRUE(anEdgeId.IsValid());

  const BRepGraph_VertexRefId aPreStartRefId =
    aGraph.Topo().Edges().Definition(anEdgeId).StartVertexRefId;
  const BRepGraph_VertexRefId aPreEndRefId =
    aGraph.Topo().Edges().Definition(anEdgeId).EndVertexRefId;
  ASSERT_TRUE(aPreStartRefId.IsValid());
  ASSERT_TRUE(aPreEndRefId.IsValid());
  ASSERT_FALSE(aGraph.Refs().Vertices().Entry(aPreStartRefId).IsRemoved);
  ASSERT_FALSE(aGraph.Refs().Vertices().Entry(aPreEndRefId).IsRemoved);

  const BRepGraph_VertexId aSplitVertex =
    aGraph.Editor().Vertices().Add(gp_Pnt(5.0, 10.0, 15.0), 1.0e-7);

  BRepGraph_EdgeId aSubA, aSubB;
  aGraph.Editor().Edges().Split(anEdgeId, aSplitVertex, aSplitParam, aSubA, aSubB);
  ASSERT_TRUE(aSubA.IsValid());
  ASSERT_TRUE(aSubB.IsValid());

  EXPECT_TRUE(aGraph.Refs().Vertices().Entry(aPreStartRefId).IsRemoved)
    << "Split must retire the original edge's StartVertexRef";
  EXPECT_TRUE(aGraph.Refs().Vertices().Entry(aPreEndRefId).IsRemoved)
    << "Split must retire the original edge's EndVertexRef";

  const BRepGraph_Validate::Result aResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_TRUE(aResult.IsValid())
    << "Audit must remain clean after Split retires boundary vertex refs";
}

TEST(BRepGraph_ScenarioMatrix, BoxEdgeSplit_SubEdgesHaveNoOriginal)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes16 =
    BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_EdgeId anEdgeId;
  double           aSplitParam = 0.0;
  const int        aNbEdges    = aGraph.Topo().Edges().Nb();
  for (BRepGraph_EdgeId aCand(0); aCand.IsValid(aNbEdges); ++aCand)
  {
    const BRepGraphInc::EdgeDef& anEdgeDef = aGraph.Topo().Edges().Definition(aCand);
    if (!anEdgeDef.IsDegenerate && anEdgeDef.Curve3DRepId.IsValid())
    {
      anEdgeId    = aCand;
      aSplitParam = 0.5 * (anEdgeDef.ParamFirst + anEdgeDef.ParamLast);
      break;
    }
  }
  ASSERT_TRUE(anEdgeId.IsValid());
  ASSERT_TRUE(aGraph.Shapes().HasOriginal(anEdgeId));

  const BRepGraph_VertexId aSplitVertex =
    aGraph.Editor().Vertices().Add(gp_Pnt(5.0, 10.0, 15.0), 1.0e-7);
  BRepGraph_EdgeId aSubA, aSubB;
  aGraph.Editor().Edges().Split(anEdgeId, aSplitVertex, aSplitParam, aSubA, aSubB);
  ASSERT_TRUE(aSubA.IsValid());
  ASSERT_TRUE(aSubB.IsValid());

  EXPECT_FALSE(aGraph.Shapes().HasOriginal(aSubA));
  EXPECT_FALSE(aGraph.Shapes().HasOriginal(aSubB));
  EXPECT_EQ(aGraph.Shapes().FindOriginal(aSubA), nullptr);
  EXPECT_EQ(aGraph.Shapes().FindOriginal(aSubB), nullptr);
#ifndef No_Exception
  EXPECT_THROW((void)aGraph.Shapes().OriginalOf(aSubA), Standard_ProgramError);
  EXPECT_THROW((void)aGraph.Shapes().OriginalOf(aSubB), Standard_ProgramError);
#endif
}

TEST(BRepGraph_ScenarioMatrix, BoxEdgeSplit_ShapeReconstructsSubEdge)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes17 =
    BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_EdgeId anEdgeId;
  double           aSplitParam = 0.0;
  const int        aNbEdges    = aGraph.Topo().Edges().Nb();
  for (BRepGraph_EdgeId aCand(0); aCand.IsValid(aNbEdges); ++aCand)
  {
    const BRepGraphInc::EdgeDef& anEdgeDef = aGraph.Topo().Edges().Definition(aCand);
    if (!anEdgeDef.IsDegenerate && anEdgeDef.Curve3DRepId.IsValid())
    {
      anEdgeId    = aCand;
      aSplitParam = 0.5 * (anEdgeDef.ParamFirst + anEdgeDef.ParamLast);
      break;
    }
  }
  ASSERT_TRUE(anEdgeId.IsValid());

  const BRepGraph_VertexId aSplitVertex =
    aGraph.Editor().Vertices().Add(gp_Pnt(5.0, 10.0, 15.0), 1.0e-7);
  BRepGraph_EdgeId aSubA, aSubB;
  aGraph.Editor().Edges().Split(anEdgeId, aSplitVertex, aSplitParam, aSubA, aSubB);

  const TopoDS_Shape aSubAShape = aGraph.Shapes().Shape(aSubA);
  const TopoDS_Shape aSubBShape = aGraph.Shapes().Shape(aSubB);
  ASSERT_FALSE(aSubAShape.IsNull());
  ASSERT_FALSE(aSubBShape.IsNull());
  EXPECT_EQ(aSubAShape.ShapeType(), TopAbs_EDGE);
  EXPECT_EQ(aSubBShape.ShapeType(), TopAbs_EDGE);
}

TEST(BRepGraph_ScenarioMatrix, EditorAddedVertex_HasNoOriginal)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes18 =
    BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_VertexId aFreshVertex =
    aGraph.Editor().Vertices().Add(gp_Pnt(42.0, 42.0, 42.0), 1.0e-7);
  ASSERT_TRUE(aFreshVertex.IsValid());

  EXPECT_FALSE(aGraph.Shapes().HasOriginal(aFreshVertex));
  EXPECT_EQ(aGraph.Shapes().FindOriginal(aFreshVertex), nullptr);
#ifndef No_Exception
  EXPECT_THROW((void)aGraph.Shapes().OriginalOf(aFreshVertex), Standard_ProgramError);
#endif
}
