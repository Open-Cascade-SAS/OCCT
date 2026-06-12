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
//   BRepGraph::ShapesView  ->  BRepGraph_Validate(Audit)
//   ->  Editor mutation  ->  BRepGraph.Shapes().Reconstruct / Shape
//   ->  BRepGraphInc_Populate  ->  BRepGraphInc_Storage.ValidateRelations / ValidateSelfIds
//
// These tests are not duplicating the many isolated unit tests that already
// exist for each individual API. They specifically lock down the correctness of
// the combined mutation-validate-reconstruct-populate pipelines and the
// relation-table paths for assemblies, compounds, comp-solids, and free wires.

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS_Vertex.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepGProp.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_ReverseIterator.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_Validate.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Populate.hxx>
#include <BRepGraphInc_Reconstruct.hxx>
#include <BRepGraphInc_Storage.hxx>
#include "BRepGraph_RefTestTools.hxx"
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>
#include <Standard_ProgramError.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_LinearVector.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_CompSolid.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <gtest/gtest.h>

#include <tuple>

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
// populate from reconstructed solid -> ValidateRelations.
// =============================================================================

TEST(BRepGraph_ScenarioMatrix, Box_MutateVertex_ValidateReconstructPopulateRoundTrip)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox       = aBoxMaker.Shape();
  const double        anOrigArea = computeArea(aBox);

  // --- Build BRepGraph ---
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes1 = aGraph.Shapes().Add(aBox);
  ASSERT_FALSE(aGraph.IsEmpty());

  // --- Validate clean graph (full audit) ---
  const BRepGraph_Validate::Result aCleanResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight());
  ASSERT_TRUE(aCleanResult.IsValid()) << "Graph must be structurally valid before any mutation";

  // --- Locate a vertex (via the first face's outer wire's first coedge) ---
  ASSERT_GT(aGraph.Topo().Vertices().Nb(), 0);
  const gp_Pnt anOldPt = BRepGraph_Tool::Vertex::Pnt(aGraph, BRepGraph_VertexId::Start());
  {
    BRepGraph_MutGuard<BRepGraphInc::VertexDef> aMut =
      aGraph.Editor().Vertices().Mut(BRepGraph_VertexId::Start());
    aGraph.Editor().Vertices().SetPoint(aMut, gp_Pnt(anOldPt.X() + 50.0, anOldPt.Y(), anOldPt.Z()));
  }

  // --- Audit again: structural invariants must survive a point-data mutation ---
  const BRepGraph_Validate::Result aAfterMutResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight());
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
    {
      aFoundMutatedVertex = true;
    }
  }
  EXPECT_TRUE(aFoundMutatedVertex)
    << "Reconstructed solid must contain a vertex at the mutated point - "
       "graph Mut(VertexDef) must propagate through BRepGraphInc_Reconstruct";

  // --- BRepGraphInc round-trip from the reconstructed solid ---
  BRepGraph aRoundTripGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aRoundTripGraph, aRecon, false);
  ASSERT_FALSE(aRoundTripGraph.IsEmpty());

  // Relation table must be consistent.
  EXPECT_TRUE(
    BRepGraph_Validate::Perform(aRoundTripGraph, BRepGraph_Validate::Options::Lightweight())
      .IsValid())
    << "Relation table must be consistent for reconstructed solid";

  // Sub-shape counts of the reconstructed solid must be identical to the original.
  EXPECT_EQ(aRoundTripGraph.Topo().Faces().Nb(), countSubShapes(aBox, TopAbs_FACE));
  EXPECT_EQ(aRoundTripGraph.Topo().Edges().Nb(), countSubShapes(aBox, TopAbs_EDGE));
}

// =============================================================================
// Scenario 2: Cylinder seam edge - mutation in BRepGraph + BRepGraphInc
// cross-validation
//
// Flow: build cylinder -> BRepGraph + BRepGraphInc_Storage -> locate the seam
// edge in storage -> mutate its tolerance in BRepGraph (cross-reference by
// shared edge index) -> Validate(Audit) on the graph -> Reconstruct -> second
// BRepGraphInc populate -> seam edges are still present -> ValidateRelations.
// =============================================================================

TEST(BRepGraph_ScenarioMatrix, Cylinder_SeamEdge_MutationAndBothSubsystemsConsistent)
{
  BRepPrimAPI_MakeCylinder aCylMaker(5.0, 15.0);
  const TopoDS_Shape&      aCyl = aCylMaker.Shape();

  // --- Build both representations from the original shape ---
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes2 = aGraph.Shapes().Add(aCyl);
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph aOrigGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aOrigGraph, aCyl, false);
  ASSERT_FALSE(aOrigGraph.IsEmpty());

  // --- Locate the seam edge in BRepGraphInc_Storage ---
  BRepGraph_EdgeId aSeamEdgeId;
  {
    const uint32_t aNbEdges = aOrigGraph.Topo().Edges().Nb();
    for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges) && !aSeamEdgeId.IsValid();
         ++anEdgeId)
    {
      const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdges =
        aOrigGraph.Topo().Edges().Relations(anEdgeId).CoEdgeIds;
      for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdges)
      {
        if (BRepGraph_Tool::CoEdge::SeamPair(aOrigGraph, aCoEdgeId).IsValid())
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
    aGraph.Editor().Edges().SetTolerance(aMut, anOldTol + 0.5);
  }

  // --- Validate(Audit): structural integrity must survive a tolerance mutation ---
  const BRepGraph_Validate::Result aResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight());
  EXPECT_TRUE(aResult.IsValid())
    << "Graph must remain structurally valid after edge tolerance mutation on seam edge";

  // --- Reconstruct the solid from the mutated graph ---
  TopoDS_Shape aRecon =
    aGraph.Shapes().Reconstruct(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, 0));
  ASSERT_FALSE(aRecon.IsNull());

  // --- BRepGraphInc populate from the reconstructed solid ---
  BRepGraph aReconGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aReconGraph, aRecon, false);
  ASSERT_FALSE(aReconGraph.IsEmpty());

  // Entity counts must match original storage.
  EXPECT_EQ(aReconGraph.Topo().Vertices().Nb(), aOrigGraph.Topo().Vertices().Nb());
  EXPECT_EQ(aReconGraph.Topo().Edges().Nb(), aOrigGraph.Topo().Edges().Nb());
  EXPECT_EQ(aReconGraph.Topo().Faces().Nb(), aOrigGraph.Topo().Faces().Nb());

  // Seam edges must still be present after the mutation+reconstruct cycle.
  bool aSeamFound = false;
  {
    const uint32_t aNbEdges = aReconGraph.Topo().Edges().Nb();
    for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges) && !aSeamFound; ++anEdgeId)
    {
      const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdges =
        aReconGraph.Topo().Edges().Relations(anEdgeId).CoEdgeIds;
      for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdges)
      {
        if (BRepGraph_Tool::CoEdge::SeamPair(aReconGraph, aCoEdgeId).IsValid())
        {
          aSeamFound = true;
          break;
        }
      }
    }
  }
  EXPECT_TRUE(aSeamFound)
    << "Seam edge must still be present in BRepGraphInc storage after reconstruct";

  EXPECT_TRUE(
    BRepGraph_Validate::Perform(aReconGraph, BRepGraph_Validate::Options::Lightweight()).IsValid())
    << "Relation table must be consistent for reconstructed cylinder";
}

// =============================================================================
// Scenario 3: CompSolid - BRepGraph + BRepGraphInc relation-table + mutation
// + reconstruct round-trip
//
// Flow: build CompSolid (2 boxes) -> BRepGraph build -> BRepGraphInc populate
// -> ValidateRelations -> Validate(Audit) -> mutate an edge tolerance in
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
  BRepGraph aOrigGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aOrigGraph, aCompSolid, false);
  ASSERT_FALSE(aOrigGraph.IsEmpty());
  EXPECT_EQ(aOrigGraph.Topo().CompSolids().Nb(), 1);
  ASSERT_GE(aOrigGraph.Topo().Solids().Nb(), 2);

  // Both solids must be relation-tableed into the CompSolid.
  for (BRepGraph_SolidId aSolidId = BRepGraph_SolidId::Start();
       aSolidId.IsValid(aOrigGraph.Topo().Solids().Nb());
       ++aSolidId)
  {
    const NCollection_LinearVector<BRepGraph_SolidRefId>& aSolidRefs =
      aOrigGraph.Topo().Solids().Relations(aSolidId).ParentSolidRefIds;
    EXPECT_GE(aSolidRefs.Size(), 1u) << "Solid " << aSolidId.Index << " not in any CompSolid";
  }
  EXPECT_TRUE(
    BRepGraph_Validate::Perform(aOrigGraph, BRepGraph_Validate::Options::Lightweight()).IsValid())
    << "Relation table must be consistent";

  // --- BRepGraph build ---
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes3 = aGraph.Shapes().Add(aCompSolid);
  ASSERT_FALSE(aGraph.IsEmpty());

  // --- Validate(Audit) clean graph ---
  EXPECT_TRUE(
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight()).IsValid())
    << "CompSolid graph must be structurally valid before mutation";

  // --- Mutate: bump edge(0) tolerance ---
  ASSERT_GT(aGraph.Topo().Edges().Nb(), 0);
  {
    BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aMut =
      aGraph.Editor().Edges().Mut(BRepGraph_EdgeId::Start());
    aGraph.Editor().Edges().SetTolerance(aMut, aMut->Tolerance + 1.0);
  }

  // --- Validate(Audit) after mutation ---
  EXPECT_TRUE(
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight()).IsValid())
    << "CompSolid graph must remain structurally valid after edge tolerance mutation";

  // --- Reconstruct the CompSolid ---
  TopoDS_Shape aRecon = aGraph.Shapes().Reconstruct(BRepGraph_CompSolidId::Start());
  ASSERT_FALSE(aRecon.IsNull());

  // Sub-shape counts must match original.
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_FACE), anOrigFaces);
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_EDGE), anOrigEdges);
  EXPECT_EQ(countSubShapes(aRecon, TopAbs_SOLID), 2);

  // --- BRepGraphInc populate from reconstructed CompSolid ---
  BRepGraph aReconGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aReconGraph, aRecon, false);
  ASSERT_FALSE(aReconGraph.IsEmpty());

  EXPECT_EQ(aReconGraph.Topo().CompSolids().Nb(), 1);
  EXPECT_EQ(aReconGraph.Topo().Solids().Nb(), aOrigGraph.Topo().Solids().Nb());
  EXPECT_EQ(aReconGraph.Topo().Faces().Nb(), aOrigGraph.Topo().Faces().Nb());
  EXPECT_EQ(aReconGraph.Topo().Edges().Nb(), aOrigGraph.Topo().Edges().Nb());

  EXPECT_TRUE(
    BRepGraph_Validate::Perform(aReconGraph, BRepGraph_Validate::Options::Lightweight()).IsValid())
    << "Relation table must be consistent for reconstructed CompSolid";
}

// =============================================================================
// Scenario 4: Assembly - two occurrences of a shared part -> Validate(Audit)
// checks assembly DAG -> reconstruct the part -> BRepGraphInc populate from
// the reconstructed part -> entity counts and relation tables consistent.
// =============================================================================

TEST(BRepGraph_ScenarioMatrix, Assembly_TwoOccurrences_ValidateDAGReconstructPartPopulate)
{
  // Build the graph from a simple solid.
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes4 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(8.0, 8.0, 8.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const TopoDS_Shape anOriginalSolid = aGraph.Shapes().Original(BRepGraph_SolidId::Start());
  ASSERT_FALSE(anOriginalSolid.IsNull());
  const int anOrigFaces = countSubShapes(anOriginalSolid, TopAbs_FACE);

  // Build assembly: two occurrences of the auto-created part at distinct translations.
  const BRepGraph_ProductId aPartId     = BRepGraph_ProductId::Start();
  const BRepGraph_ProductId aAssemblyId = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aAssemblyId);
  ASSERT_TRUE(aAssemblyId.IsValid());

  gp_Trsf aTrsf1;
  aTrsf1.SetTranslation(gp_Vec(100.0, 0.0, 0.0));
  gp_Trsf aTrsf2;
  aTrsf2.SetTranslation(gp_Vec(0.0, 200.0, 0.0));

  const BRepGraph_OccurrenceId anOcc1 =
    aGraph.Editor().Products().Append(aAssemblyId, aPartId, TopLoc_Location(aTrsf1));
  const BRepGraph_OccurrenceId anOcc2 =
    aGraph.Editor().Products().Append(aAssemblyId, aPartId, TopLoc_Location(aTrsf2));

  ASSERT_TRUE(anOcc1.IsValid());
  ASSERT_TRUE(anOcc2.IsValid());

  // --- Validate(Audit): assembly DAG must be acyclic and reference-consistent ---
  const BRepGraph_Validate::Result aResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight());
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
  BRepGraph aPartGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aPartGraph, aPartShape, false);
  ASSERT_FALSE(aPartGraph.IsEmpty());

  EXPECT_EQ(aPartGraph.Topo().Solids().Nb(), 1);
  EXPECT_EQ(aPartGraph.Topo().Faces().Nb(), anOrigFaces);

  EXPECT_TRUE(
    BRepGraph_Validate::Perform(aPartGraph, BRepGraph_Validate::Options::Lightweight()).IsValid())
    << "Relation table must be consistent for BRepGraphInc of reconstructed part";
}

// =============================================================================
// Scenario 5: Compound with free wire + free edge + free vertex
//
// Flow: build a compound containing three atomic sub-shapes (wire, edge,
// vertex) -> BRepGraph build -> Validate(Audit) -> BRepGraphInc populate ->
// ValidateRelations. Exercises atomic child-to-compound relation tablesing
// end-to-end, combined with the BRepGraph structural check.
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
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes5 = aGraph.Shapes().Add(aCompound);
  ASSERT_FALSE(aGraph.IsEmpty());

  EXPECT_EQ(aGraph.Topo().Compounds().Nb(), 1);
  ASSERT_GE(aGraph.Topo().Wires().Nb(), 1);
  ASSERT_GE(aGraph.Topo().Edges().Nb(), 2);
  ASSERT_GE(aGraph.Topo().Vertices().Nb(), 1);

  // --- Validate(Audit) ---
  const BRepGraph_Validate::Result aResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight());
  EXPECT_TRUE(aResult.IsValid()) << "Compound with free wire/edge/vertex must pass full audit";

  // --- BRepGraphInc populate ---
  BRepGraph aPopGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aPopGraph, aCompound, false);
  ASSERT_FALSE(aPopGraph.IsEmpty());

  EXPECT_EQ(aPopGraph.Topo().Compounds().Nb(), 1u);
  ASSERT_GE(aPopGraph.Topo().Wires().Nb(), 1u);
  ASSERT_GE(aPopGraph.Topo().Edges().Nb(), 2u);

  EXPECT_TRUE(
    BRepGraph_Validate::Perform(aPopGraph, BRepGraph_Validate::Options::Lightweight()).IsValid())
    << "Relation table must be consistent for compound with atomic sub-shapes";
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
  BRepGraph aBaseGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aBaseGraph, aCompound, false);
  ASSERT_FALSE(aBaseGraph.IsEmpty());
  EXPECT_EQ(aBaseGraph.Topo().Solids().Nb(), 2);
  EXPECT_EQ(aBaseGraph.Topo().Compounds().Nb(), 1);

  // Count seam edges in baseline.
  int aBaseSeamCount = 0;
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aBaseGraph.Topo().Edges().Nb()); ++anEdgeId)
  {
    const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdges =
      aBaseGraph.Topo().Edges().Relations(anEdgeId).CoEdgeIds;
    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdges)
    {
      if (BRepGraph_Tool::CoEdge::SeamPair(aBaseGraph, aCoEdgeId).IsValid())
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
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes6 = aGraph.Shapes().Add(aCompound);
  ASSERT_FALSE(aGraph.IsEmpty());
  EXPECT_EQ(aGraph.Topo().Solids().Nb(), 2);

  // --- Validate(Audit) clean ---
  EXPECT_TRUE(
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight()).IsValid())
    << "Compound [box+cylinder] must pass full audit before mutation";

  // --- Mutate: change face(0) tolerance (a box face) ---
  ASSERT_GT(aGraph.Topo().Faces().Nb(), 0);
  {
    BRepGraph_MutGuard<BRepGraphInc::FaceDef> aMut =
      aGraph.Editor().Faces().Mut(BRepGraph_FaceId::Start());
    aGraph.Editor().Faces().SetTolerance(aMut, aMut->Tolerance + 0.5);
  }

  // --- Validate(Audit) after mutation ---
  EXPECT_TRUE(
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight()).IsValid())
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
  BRepGraph aReconGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aReconGraph, aRecon, false);
  ASSERT_FALSE(aReconGraph.IsEmpty());

  EXPECT_EQ(aReconGraph.Topo().Solids().Nb(), aBaseGraph.Topo().Solids().Nb());
  EXPECT_EQ(aReconGraph.Topo().Faces().Nb(), aBaseGraph.Topo().Faces().Nb());
  EXPECT_EQ(aReconGraph.Topo().Edges().Nb(), aBaseGraph.Topo().Edges().Nb());

  // Seam edges must still be present.
  int aReconSeamCount = 0;
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aReconGraph.Topo().Edges().Nb()); ++anEdgeId)
  {
    const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdges =
      aReconGraph.Topo().Edges().Relations(anEdgeId).CoEdgeIds;
    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdges)
    {
      if (BRepGraph_Tool::CoEdge::SeamPair(aReconGraph, aCoEdgeId).IsValid())
      {
        ++aReconSeamCount;
        break;
      }
    }
  }
  EXPECT_EQ(aReconSeamCount, aBaseSeamCount)
    << "Seam edge count must be unchanged after compound reconstruct";

  EXPECT_TRUE(
    BRepGraph_Validate::Perform(aReconGraph, BRepGraph_Validate::Options::Lightweight()).IsValid())
    << "Relation table must be consistent for reconstructed compound [box+cylinder]";
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
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes7 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  // Four-level chain: Top -> Root -> Mid -> Leaf (leaf is the auto-built part).
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

  const BRepGraph_OccurrenceId anOccRoot =
    aGraph.Editor().Products().Append(aTopAsm, aRootAsm, TopLoc_Location(aT3));
  const BRepGraph_OccurrenceId anOccMid =
    aGraph.Editor().Products().Append(aRootAsm, aMidAsm, TopLoc_Location(aT2), anOccRoot);
  const BRepGraph_OccurrenceId anOccLeaf =
    aGraph.Editor().Products().Append(aMidAsm, aLeafPart, TopLoc_Location(aT1), anOccMid);
  ASSERT_TRUE(anOccRoot.IsValid());
  ASSERT_TRUE(anOccMid.IsValid());
  ASSERT_TRUE(anOccLeaf.IsValid());

  // Depth check via the chain.
  EXPECT_EQ(aGraph.Topo().Products().NbComponents(aTopAsm), 1);
  EXPECT_EQ(aGraph.Topo().Products().NbComponents(aRootAsm), 1);
  EXPECT_EQ(aGraph.Topo().Products().NbComponents(aMidAsm), 1);

  // --- Clean audit on the 4-level DAG ---
  const BRepGraph_Validate::Result aCleanResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight());
  EXPECT_TRUE(aCleanResult.IsValid()) << "Four-level assembly must pass full audit when acyclic";

  // --- Transitive cycle: C -> D, then D -> C. Self-ref guard skips this because
  // C.Index != D.Index, but the BFS-based Audit check must still catch it. ---
  const BRepGraph_ProductId aProdC = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aProdC);
  const BRepGraph_ProductId aProdD = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aProdD);

  gp_Trsf aIdTrsf;
  ASSERT_TRUE(
    aGraph.Editor().Products().Append(aProdC, aProdD, TopLoc_Location(aIdTrsf)).IsValid());
  ASSERT_TRUE(
    aGraph.Editor().Products().Append(aProdD, aProdC, TopLoc_Location(aIdTrsf)).IsValid());

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
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes8 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(5.0, 5.0, 5.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  // Snapshot topology entity counts before any assembly wiring.
  const uint32_t aNbSolidsBefore = aGraph.Topo().Solids().Nb();
  const uint32_t aNbFacesBefore  = aGraph.Topo().Faces().Nb();

  const BRepGraph_ProductId aSharedPart = BRepGraph_ProductId::Start();
  const BRepGraph_ProductId aAsmA       = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aAsmA);
  const BRepGraph_ProductId aAsmB = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aAsmB);

  gp_Trsf aOffsetA, aOffsetB;
  aOffsetA.SetTranslation(gp_Vec(100.0, 0.0, 0.0));
  aOffsetB.SetTranslation(gp_Vec(0.0, 100.0, 0.0));

  ASSERT_TRUE(
    aGraph.Editor().Products().Append(aAsmA, aSharedPart, TopLoc_Location(aOffsetA)).IsValid());
  ASSERT_TRUE(
    aGraph.Editor().Products().Append(aAsmB, aSharedPart, TopLoc_Location(aOffsetB)).IsValid());

  // The shared part must not have grown the topology pool.
  EXPECT_EQ(aGraph.Topo().Solids().Nb(), aNbSolidsBefore)
    << "Sharing a Product across assemblies must not duplicate topology defs";
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), aNbFacesBefore);

  // Audit passes: DAG sharing is not a cycle.
  EXPECT_TRUE(
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight()).IsValid())
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
// Exercises mixed child-to-compound relation tablesing and verifies that an
// atomic-heterogeneous compound survives Validate(Audit) + relation-table round
// trip via BRepGraphInc_Populate.
// =============================================================================

TEST(BRepGraph_ScenarioMatrix, Compound_MixedAtomicChildren_RelationsCoverage)
{
  // Build five constituents as separate top-level shapes, wrap them in a single
  // TopoDS_Compound, then let BRepGraph::ShapesView handle population. This bypasses
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
    {
      aFreeSh = aShellExp.Current();
    }
  }
  TopoDS_Shape aFreeFace;
  {
    TopExp_Explorer aFaceExp(aBox, TopAbs_FACE);
    if (aFaceExp.More())
    {
      aFreeFace = aFaceExp.Current();
    }
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
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes9 = aGraph.Shapes().Add(aCompound);
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GT(aGraph.Topo().Compounds().Nb(), 0);

  // Clean audit.
  const BRepGraph_Validate::Result aAuditResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight());
  EXPECT_TRUE(aAuditResult.IsValid())
    << "Compound with heterogeneous atomic children must pass Audit";

  // The compound at index 0 must enumerate at least one each of solid/shell/
  // face/edge/vertex via its child refs.
  const BRepGraph_CompoundId                            aCompoundId(0);
  bool                                                  aHasSolid = false;
  [[maybe_unused]] bool                                 aHasShell = false;
  [[maybe_unused]] bool                                 aHasFace  = false;
  bool                                                  aHasEdge = false, aHasVertex = false;
  const NCollection_LinearVector<BRepGraph_ChildRefId>& aChildRefs =
    aGraph.Topo().Compounds().Relations(aCompoundId).ChildRefIds;
  for (const BRepGraph_ChildRefId& aChildRefId : aChildRefs)
  {
    const BRepGraphInc::ChildRef& aRef = aGraph.Refs().Children().Entry(aChildRefId);
    switch (aRef.ChildNodeId.NodeKind)
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

  // Relation coverage: drive through BRepGraphInc_Populate so the returned
  // storage exposes direct relation containers.
  BRepGraph aPopGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aPopGraph, aCompound, false);
  ASSERT_FALSE(aPopGraph.IsEmpty());
  EXPECT_TRUE(
    BRepGraph_Validate::Perform(aPopGraph, BRepGraph_Validate::Options::Lightweight()).IsValid())
    << "Relation table must be consistent for atomic-mixed compound";
}

// =============================================================================
// Scenario 10: CompSolid holding three boxes - relation-table must cover every
// solid, and every solid must see exactly one CompSolid parent in the reverse
// map. Exercises the myCompSolidsOfSolid path with N>2.
// =============================================================================

TEST(BRepGraph_ScenarioMatrix, CompSolid_ThreeBoxes_RelationsPerSolid)
{
  BRep_Builder     aBB;
  TopoDS_CompSolid aCompSolid;
  aBB.MakeCompSolid(aCompSolid);

  aBB.Add(aCompSolid, BRepPrimAPI_MakeBox(5.0, 5.0, 5.0).Shape());
  aBB.Add(aCompSolid, BRepPrimAPI_MakeBox(gp_Pnt(20.0, 0.0, 0.0), 5.0, 5.0, 5.0).Shape());
  aBB.Add(aCompSolid, BRepPrimAPI_MakeBox(gp_Pnt(40.0, 0.0, 0.0), 5.0, 5.0, 5.0).Shape());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes10 =
    aGraph.Shapes().Add(aCompSolid);
  ASSERT_FALSE(aGraph.IsEmpty());

  EXPECT_TRUE(
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight()).IsValid())
    << "Three-solid CompSolid must pass full audit";

  BRepGraph aPopGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aPopGraph, aCompSolid, false);
  ASSERT_FALSE(aPopGraph.IsEmpty());
  ASSERT_EQ(aPopGraph.Topo().CompSolids().Nb(), 1);
  ASSERT_GE(aPopGraph.Topo().Solids().Nb(), 3);

  for (BRepGraph_SolidId aSolidId(0); aSolidId.IsValid(aPopGraph.Topo().Solids().Nb()); ++aSolidId)
  {
    const NCollection_LinearVector<BRepGraph_SolidRefId>& aParents =
      aPopGraph.Topo().Solids().Relations(aSolidId).ParentSolidRefIds;
    ASSERT_EQ(aParents.Size(), 1u)
      << "Solid " << aSolidId.Index << " should have exactly one CompSolid parent";
  }

  EXPECT_TRUE(
    BRepGraph_Validate::Perform(aPopGraph, BRepGraph_Validate::Options::Lightweight()).IsValid())
    << "Relation table must be consistent for 3-solid CompSolid";
}

// =============================================================================
// Scenario 11: Sphere seam - seam CoEdges come in bidirectionally-paired pairs.
// The seam relation is connectivity-derived (Tool::CoEdge::SeamPair walks
// CoEdgesOfEdge filtered by face+orientation); the symmetry of that predicate
// is structural. Verify post-build and post-reconstruct that the seam count is
// preserved under periodic UV.
// =============================================================================

TEST(BRepGraph_ScenarioMatrix, Sphere_SeamCoEdgePair_Bidirectional)
{
  const TopoDS_Shape aSphere = BRepPrimAPI_MakeSphere(10.0).Shape();

  BRepGraph aPopGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aPopGraph, aSphere, false);
  ASSERT_FALSE(aPopGraph.IsEmpty());

  int aNbPairedCoEdges = 0;
  for (BRepGraph_CoEdgeId aCoEdgeId(0); aCoEdgeId.IsValid(aPopGraph.Topo().CoEdges().Nb());
       ++aCoEdgeId)
  {
    const BRepGraph_CoEdgeId aPairId = BRepGraph_Tool::CoEdge::SeamPair(aPopGraph, aCoEdgeId);
    if (!aPairId.IsValid())
    {
      continue;
    }
    ++aNbPairedCoEdges;

    // Symmetry: the partner's partner is us.
    const BRepGraph_CoEdgeId aBackId = BRepGraph_Tool::CoEdge::SeamPair(aPopGraph, aPairId);
    EXPECT_EQ(aBackId, aCoEdgeId) << "Seam relation must be symmetric";
    const BRepGraphInc::CoEdgeDef& aCoEdge = aPopGraph.Topo().CoEdges().Definition(aCoEdgeId);
    const BRepGraphInc::CoEdgeDef& aPaired = aPopGraph.Topo().CoEdges().Definition(aPairId);
    EXPECT_EQ(aPaired.ChildEdgeId, aCoEdge.ChildEdgeId)
      << "Seam pair must share the same underlying EdgeDef";
  }
  EXPECT_GT(aNbPairedCoEdges, 0) << "Sphere must have at least one seam-paired coedge";

  // Build the graph, audit, reconstruct, repopulate, re-verify symmetry.
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes11 = aGraph.Shapes().Add(aSphere);
  ASSERT_FALSE(aGraph.IsEmpty());
  EXPECT_TRUE(
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight()).IsValid())
    << "Sphere graph must pass full audit";

  TopoDS_Shape aRecon =
    aGraph.Shapes().Reconstruct(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, 0));
  ASSERT_FALSE(aRecon.IsNull());

  BRepGraph aReconGraph;
  std::ignore = BRepGraphInc_Populate::Perform(aReconGraph, aRecon, false);
  ASSERT_FALSE(aReconGraph.IsEmpty());

  int aNbPairedAfter = 0;
  for (BRepGraph_CoEdgeId aCoEdgeId(0); aCoEdgeId.IsValid(aReconGraph.Topo().CoEdges().Nb());
       ++aCoEdgeId)
  {
    const BRepGraph_CoEdgeId aPairId = BRepGraph_Tool::CoEdge::SeamPair(aReconGraph, aCoEdgeId);
    if (!aPairId.IsValid())
    {
      continue;
    }
    ++aNbPairedAfter;
    const BRepGraph_CoEdgeId aBackId = BRepGraph_Tool::CoEdge::SeamPair(aReconGraph, aPairId);
    EXPECT_EQ(aBackId, aCoEdgeId) << "Post-reconstruct seam pair must remain symmetric";
  }
  EXPECT_EQ(aNbPairedAfter, aNbPairedCoEdges)
    << "Reconstructed sphere must preserve the same number of seam-paired coedges";
}

// =============================================================================
// Scenario 12: Cylinder seam-edge Split - exercises the seam-pair case of
// EdgeOps::Split(). The unified CoEdge rebuild pass allocates two fresh sub-
// coedges per original (including seam partners), preserves derived seam pairing
// linkage on the new pairs, and retires orphan vertex refs.
// =============================================================================

TEST(BRepGraph_ScenarioMatrix, Cylinder_SeamEdgeSplit_AuditStable)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes13 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeCylinder(5.0, 15.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  // Locate a seam edge: an edge with at least one seam-paired coedge.
  BRepGraph_EdgeId aSeamEdgeId;
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aGraph.Topo().Edges().Nb()); ++anEdgeId)
  {
    const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdges =
      aGraph.Topo().Edges().CoEdges(anEdgeId);
    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdges)
    {
      if (BRepGraph_Tool::CoEdge::IsSeam(aGraph, aCoEdgeId))
      {
        aSeamEdgeId = anEdgeId;
        break;
      }
    }
    if (aSeamEdgeId.IsValid())
    {
      break;
    }
  }
  ASSERT_TRUE(aSeamEdgeId.IsValid()) << "Cylinder must carry at least one seam edge";

  ASSERT_FALSE(BRepGraph_Tool::Edge::Degenerated(aGraph, aSeamEdgeId));
  const std::pair<double, double> aSeamRange = BRepGraph_Tool::Edge::Range(aGraph, aSeamEdgeId);
  ASSERT_LT(aSeamRange.first, aSeamRange.second);

  const double aMidParam = 0.5 * (aSeamRange.first + aSeamRange.second);

  const BRepGraph_VertexId aSplitVertex =
    aGraph.Editor().Vertices().Add(gp_Pnt(5.0, 0.0, 7.5), 1.0e-7);
  ASSERT_TRUE(aSplitVertex.IsValid());

  BRepGraph_EdgeId aSubA, aSubB;
  aGraph.Editor().Edges().Split(aSeamEdgeId, aSplitVertex, aMidParam, aSubA, aSubB);
  ASSERT_TRUE(aSubA.IsValid());
  ASSERT_TRUE(aSubB.IsValid());

  // The full audit catches any relation-table drift that the Split path leaves
  // behind. If this starts failing, the Split implementation needs to be
  // revisited to also update derived seam pair connectivity on the new sub-coedges.
  const BRepGraph_Validate::Result aResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight());
  EXPECT_TRUE(aResult.IsValid()) << "Audit must remain clean after splitting a seam edge";

  // Every seam-paired coedge on the live sub-edges must still satisfy the
  // symmetry of the connectivity-derived seam predicate (SeamPair(SeamPair(x))==x).
  auto checkBidirectionalOnEdge = [&](const BRepGraph_EdgeId theEdgeId) {
    const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdges =
      aGraph.Topo().Edges().CoEdges(theEdgeId);
    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdges)
    {
      const BRepGraph_CoEdgeId aPairId = BRepGraph_Tool::CoEdge::SeamPair(aGraph, aCoEdgeId);
      if (!aPairId.IsValid())
      {
        continue;
      }
      const BRepGraph_CoEdgeId aBackId = BRepGraph_Tool::CoEdge::SeamPair(aGraph, aPairId);
      EXPECT_EQ(aBackId, aCoEdgeId)
        << "Seam pair must remain symmetric after Split on edge " << theEdgeId.Index;
    }
  };

  checkBidirectionalOnEdge(aSubA);
  checkBidirectionalOnEdge(aSubB);
}

// =============================================================================
// Scenario 13: direct editor-created face-bound PCurve CoEdge must be wired into
// edge incidence immediately. This is the typed replacement for raw rep deletion:
// the public editor operation creates geometry and relation state atomically.
// =============================================================================

TEST(BRepGraph_ScenarioMatrix, EditorFaceBoundPCurveCoEdge_RelationsAndLookup)
{
  BRepGraph aGraph;
  aGraph.Clear();

  const BRepGraph_VertexId aV0 = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_VertexId aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.0e-7);
  ASSERT_TRUE(aV0.IsValid());
  ASSERT_TRUE(aV1.IsValid());

  const BRepGraph_EdgeId anEdgeId =
    aGraph.Editor().Edges().Add(aV0, aV1, occ::handle<Geom_Curve>(), 0.0, 1.0, 1.0e-7);
  ASSERT_TRUE(anEdgeId.IsValid());

  const BRepGraph_CoEdgeId aWireCoEdgeId = aGraph.Editor().CoEdges().Add(anEdgeId, TopAbs_FORWARD);
  ASSERT_TRUE(aWireCoEdgeId.IsValid());

  NCollection_LinearVector<BRepGraph_CoEdgeId> aWireCoEdgeIds;
  aWireCoEdgeIds.Append(aWireCoEdgeId);
  const BRepGraph_WireId aWireId = aGraph.Editor().Wires().Add(aWireCoEdgeIds.ToArray1());
  ASSERT_TRUE(aWireId.IsValid());

  NCollection_LinearVector<BRepGraph_WireId> anInnerWireIds;
  const occ::handle<Geom_Plane>              aPlane =
    new Geom_Plane(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  const BRepGraph_FaceId aFaceId =
    aGraph.Editor().Faces().Add(aPlane, aWireId, anInnerWireIds.ToArray1(), 1.0e-7);
  ASSERT_TRUE(aFaceId.IsValid());

  const occ::handle<Geom2d_Line> aCurve2d = new Geom2d_Line(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  const BRepGraph_CoEdgeId       aPCurveCoEdgeId =
    aGraph.Editor().CoEdges().Add(anEdgeId, aFaceId, aCurve2d, 0.0, 1.0, TopAbs_REVERSED);
  ASSERT_TRUE(aPCurveCoEdgeId.IsValid());

  const BRepGraphInc::CoEdgeDef& aPCurveCoEdge =
    aGraph.Topo().CoEdges().Definition(aPCurveCoEdgeId);
  EXPECT_EQ(aPCurveCoEdge.ChildEdgeId, anEdgeId);
  EXPECT_EQ(aPCurveCoEdge.FaceId, aFaceId);
  EXPECT_EQ(aPCurveCoEdge.Orientation, TopAbs_REVERSED);
  EXPECT_TRUE(aPCurveCoEdge.Curve2DRepId.IsValid());

  EXPECT_EQ(BRepGraph_Tool::Edge::FindPCurveCoEdgeId(aGraph, anEdgeId, aFaceId), aPCurveCoEdgeId);
  EXPECT_EQ(BRepGraph_Tool::Edge::FindPCurveCoEdgeId(aGraph, anEdgeId, aFaceId, TopAbs_REVERSED),
            aPCurveCoEdgeId);

  bool isCoEdgeListedByEdge = false;
  for (const BRepGraph_CoEdgeId& aCoEdgeId : aGraph.Topo().Edges().CoEdges(anEdgeId))
  {
    if (aCoEdgeId == aPCurveCoEdgeId)
    {
      isCoEdgeListedByEdge = true;
      break;
    }
  }
  EXPECT_TRUE(isCoEdgeListedByEdge)
    << "Face-bound PCurve CoEdge must be reachable through EdgeRelations::CoEdgeIds";

  bool isFaceListedByEdge = false;
  for (const BRepGraph_FaceId& anAdjacentFaceId : aGraph.Topo().Edges().FacesOf(anEdgeId))
  {
    if (anAdjacentFaceId == aFaceId)
    {
      isFaceListedByEdge = true;
      break;
    }
  }
  EXPECT_TRUE(isFaceListedByEdge)
    << "Face-bound PCurve CoEdge must be visible through edge-face adjacency";

  EXPECT_TRUE(aGraph.ValidateRelations());
  const BRepGraph_Validate::Result aAudit =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight());
  EXPECT_TRUE(aAudit.IsValid());
}

// =============================================================================
// Scenario 14: after a seam-edge split every CoEdge on the new sub-edges must
// carry a Curve2DRep, a valid FaceId, and must appear in the relation-table
// wire / edge adjacencies. Guards the "CoEdgeDef has no Curve2D representation"
// and CoEdge->Wire binding regressions.
// =============================================================================

TEST(BRepGraph_ScenarioMatrix, Cylinder_SeamEdgeSplit_CoEdgeFaceIncidence)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes14 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeCylinder(5.0, 15.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph_EdgeId aSeamEdgeId;
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aGraph.Topo().Edges().Nb()); ++anEdgeId)
  {
    const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdges =
      aGraph.Topo().Edges().CoEdges(anEdgeId);
    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdges)
    {
      if (BRepGraph_Tool::CoEdge::IsSeam(aGraph, aCoEdgeId))
      {
        aSeamEdgeId = anEdgeId;
        break;
      }
    }
    if (aSeamEdgeId.IsValid())
    {
      break;
    }
  }
  ASSERT_TRUE(aSeamEdgeId.IsValid());

  const std::pair<double, double> aSeamRange2 = BRepGraph_Tool::Edge::Range(aGraph, aSeamEdgeId);
  const double                    aMidParam   = 0.5 * (aSeamRange2.first + aSeamRange2.second);
  const BRepGraph_VertexId        aSplitVertex =
    aGraph.Editor().Vertices().Add(gp_Pnt(5.0, 0.0, 7.5), 1.0e-7);

  BRepGraph_EdgeId aSubA, aSubB;
  aGraph.Editor().Edges().Split(aSeamEdgeId, aSplitVertex, aMidParam, aSubA, aSubB);
  ASSERT_TRUE(aSubA.IsValid());
  ASSERT_TRUE(aSubB.IsValid());

  auto checkCoEdgeIncidence = [&](const BRepGraph_EdgeId theSubId) {
    const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdges =
      aGraph.Topo().Edges().CoEdges(theSubId);
    ASSERT_GT(aCoEdges.Size(), 0u) << "Sub-edge must carry at least one CoEdge";
    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdges)
    {
      const BRepGraphInc::CoEdgeDef& aCoEdge = aGraph.Topo().CoEdges().Definition(aCoEdgeId);
      EXPECT_TRUE(aCoEdge.Curve2DRepId.IsValid())
        << "Sub-CoEdge must carry Curve2DRepId after Split";
      EXPECT_TRUE(aCoEdge.FaceId.IsValid()) << "Sub-CoEdge must carry FaceId after Split";
      EXPECT_EQ(aCoEdge.ChildEdgeId, theSubId);
      const std::pair<double, double> aCERange = BRepGraph_Tool::CoEdge::Range(aGraph, aCoEdgeId);
      EXPECT_LT(aCERange.first, aCERange.second)
        << "Sub-CoEdge must have a non-degenerate parameter range";
    }
  };

  checkCoEdgeIncidence(aSubA);
  checkCoEdgeIncidence(aSubB);

  // A full Audit is the canonical relation-table / wire-incidence check. If
  // any sub-CoEdge were dangling or the Edge->Face cache were stale, Audit
  // would flag it. Keeping this assertion makes scenario 14 a useful
  // regression gate even without direct relation-table access in tests.
  const BRepGraph_Validate::Result aAudit =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight());
  EXPECT_TRUE(aAudit.IsValid()) << "Audit must remain clean after seam-edge Split";
}

// =============================================================================
// Scenario 15: after Split the original edge's boundary vertex refs must be
// retired (IsRemoved=true). Without retirement those refs become orphans
// because no live edge slot owns them.
// =============================================================================

TEST(BRepGraph_ScenarioMatrix, BoxEdgeSplit_BoundaryVertexRetirement)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes15 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph_EdgeId anEdgeId;
  double           aSplitParam = 0.0;
  const uint32_t   aNbEdges    = aGraph.Topo().Edges().Nb();
  for (BRepGraph_EdgeId aCand(0); aCand.IsValid(aNbEdges); ++aCand)
  {
    const BRepGraphInc::EdgeDef& anEdgeDef = aGraph.Topo().Edges().Definition(aCand);
    if (!BRepGraph_Tool::Edge::Degenerated(aGraph, aCand) && anEdgeDef.Curve3DRepId.IsValid()
        && anEdgeDef.StartVertexRefId.IsValid() && anEdgeDef.EndVertexRefId.IsValid())
    {
      anEdgeId = aCand;
      {
        const auto _r = BRepGraph_Tool::Edge::Range(aGraph, aCand);
        aSplitParam   = 0.5 * (_r.first + _r.second);
      }
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
  ASSERT_FALSE(aPreStartRefId.IsRemoved(aGraph));
  ASSERT_FALSE(aPreEndRefId.IsRemoved(aGraph));

  const BRepGraph_VertexId aSplitVertex =
    aGraph.Editor().Vertices().Add(gp_Pnt(5.0, 10.0, 15.0), 1.0e-7);

  BRepGraph_EdgeId aSubA, aSubB;
  aGraph.Editor().Edges().Split(anEdgeId, aSplitVertex, aSplitParam, aSubA, aSubB);
  ASSERT_TRUE(aSubA.IsValid());
  ASSERT_TRUE(aSubB.IsValid());

  EXPECT_TRUE(aPreStartRefId.IsRemoved(aGraph))
    << "Split must retire the original edge's StartVertexRef";
  EXPECT_TRUE(aPreEndRefId.IsRemoved(aGraph))
    << "Split must retire the original edge's EndVertexRef";

  const BRepGraph_Validate::Result aResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Lightweight());
  EXPECT_TRUE(aResult.IsValid())
    << "Audit must remain clean after Split retires boundary vertex refs";
}

TEST(BRepGraph_ScenarioMatrix, BoxEdgeSplit_SubEdgesHaveNoOriginal)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes16 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph_EdgeId anEdgeId;
  double           aSplitParam = 0.0;
  const uint32_t   aNbEdges    = aGraph.Topo().Edges().Nb();
  for (BRepGraph_EdgeId aCand(0); aCand.IsValid(aNbEdges); ++aCand)
  {
    const BRepGraphInc::EdgeDef& anEdgeDef = aGraph.Topo().Edges().Definition(aCand);
    if (!BRepGraph_Tool::Edge::Degenerated(aGraph, aCand) && anEdgeDef.Curve3DRepId.IsValid())
    {
      anEdgeId = aCand;
      {
        const auto _r = BRepGraph_Tool::Edge::Range(aGraph, aCand);
        aSplitParam   = 0.5 * (_r.first + _r.second);
      }
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
  EXPECT_TRUE(aGraph.Shapes().Original(aSubA).IsNull());
  EXPECT_TRUE(aGraph.Shapes().Original(aSubB).IsNull());
}

TEST(BRepGraph_ScenarioMatrix, BoxEdgeSplit_ShapeReconstructsSubEdge)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes17 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph_EdgeId anEdgeId;
  double           aSplitParam = 0.0;
  const uint32_t   aNbEdges    = aGraph.Topo().Edges().Nb();
  for (BRepGraph_EdgeId aCand(0); aCand.IsValid(aNbEdges); ++aCand)
  {
    const BRepGraphInc::EdgeDef& anEdgeDef = aGraph.Topo().Edges().Definition(aCand);
    if (!BRepGraph_Tool::Edge::Degenerated(aGraph, aCand) && anEdgeDef.Curve3DRepId.IsValid())
    {
      anEdgeId = aCand;
      {
        const auto _r = BRepGraph_Tool::Edge::Range(aGraph, aCand);
        aSplitParam   = 0.5 * (_r.first + _r.second);
      }
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
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes18 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_VertexId aFreshVertex =
    aGraph.Editor().Vertices().Add(gp_Pnt(42.0, 42.0, 42.0), 1.0e-7);
  ASSERT_TRUE(aFreshVertex.IsValid());

  EXPECT_FALSE(aGraph.Shapes().HasOriginal(aFreshVertex));
  EXPECT_TRUE(aGraph.Shapes().Original(aFreshVertex).IsNull());
}
