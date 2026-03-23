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
#include <BRepGraph_BackRefManager.hxx>
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_GeomView.hxx>
#include <BRepGraph_MutView.hxx>
#include <BRepGraph_Mutator.hxx>
#include <BRepGraph_RelEdgesView.hxx>

#include <BRepPrimAPI_MakeBox.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Precision.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>

#include <gtest/gtest.h>

namespace
{

//! Build a graph from a box and return it.
BRepGraph buildBoxGraph()
{
  BRepPrimAPI_MakeBox aMakeBox(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aMakeBox.Shape();
  BRepGraph aGraph;
  aGraph.Build(aBox);
  return aGraph;
}

} // namespace

// Verify that after Build(), back-refs match forward links for surfaces.
TEST(BRepGraphBackRefManagerTest, Build_SurfaceBackRefsMatchForwardLinks)
{
  BRepGraph aGraph = buildBoxGraph();
  ASSERT_TRUE(aGraph.IsDone());

  for (int aFaceIdx = 0; aFaceIdx < aGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFaceDef = aGraph.Defs().Face(aFaceIdx);
    if (!aFaceDef.SurfNodeId.IsValid())
      continue;

    const BRepGraph_GeomNode::Surf& aSurf = aGraph.Geom().Surface(aFaceDef.SurfNodeId.Index);
    const NCollection_Vector<BRepGraph_NodeId> aFaceIds = aGraph.Geom().FacesOnSurface(aSurf.Id);
    bool aFound = false;
    for (int aUserIdx = 0; aUserIdx < aFaceIds.Length(); ++aUserIdx)
    {
      if (aFaceIds.Value(aUserIdx) == aFaceDef.Id)
      {
        aFound = true;
        break;
      }
    }
    EXPECT_TRUE(aFound) << "FaceDef " << aFaceIdx << " not found in FacesOnSurface";
  }
}

// Verify that after Build(), back-refs match forward links for curves.
TEST(BRepGraphBackRefManagerTest, Build_CurveBackRefsMatchForwardLinks)
{
  BRepGraph aGraph = buildBoxGraph();
  ASSERT_TRUE(aGraph.IsDone());

  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Defs().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aGraph.Defs().Edge(anEdgeIdx);
    if (!anEdgeDef.CurveNodeId.IsValid())
      continue;

    const BRepGraph_GeomNode::Curve& aCurve = aGraph.Geom().Curve(anEdgeDef.CurveNodeId.Index);
    const NCollection_Vector<BRepGraph_NodeId> anEdgeIds = aGraph.Geom().EdgesOnCurve(aCurve.Id);
    bool aFound = false;
    for (int aUserIdx = 0; aUserIdx < anEdgeIds.Length(); ++aUserIdx)
    {
      if (anEdgeIds.Value(aUserIdx) == anEdgeDef.Id)
      {
        aFound = true;
        break;
      }
    }
    EXPECT_TRUE(aFound) << "EdgeDef " << anEdgeIdx << " not found in EdgesOnCurve";
  }
}

// Verify that after Build(), edge-to-wire reverse index matches wire OrderedEdges.
TEST(BRepGraphBackRefManagerTest, Build_EdgeToWiresMatchForwardLinks)
{
  BRepGraph aGraph = buildBoxGraph();
  ASSERT_TRUE(aGraph.IsDone());

  for (int aWireIdx = 0; aWireIdx < aGraph.Defs().NbWires(); ++aWireIdx)
  {
    const BRepGraph_TopoNode::WireDef& aWireDef = aGraph.Defs().Wire(aWireIdx);
    for (int anEdgeIdx = 0; anEdgeIdx < aWireDef.OrderedEdges.Length(); ++anEdgeIdx)
    {
      const int anEdgeDefIdx = aWireDef.OrderedEdges.Value(anEdgeIdx).EdgeDefId.Index;
      const NCollection_Vector<int>& aWires = aGraph.RelEdges().WiresOfEdge(anEdgeDefIdx);
      bool aFound = false;
      for (int aW = 0; aW < aWires.Length(); ++aW)
      {
        if (aWires.Value(aW) == aWireIdx)
        {
          aFound = true;
          break;
        }
      }
      EXPECT_TRUE(aFound) << "Wire " << aWireIdx << " not found in WiresOfEdge(" << anEdgeDefIdx << ")";
    }
  }
}

// RebuildAll produces identical back-refs.
TEST(BRepGraphBackRefManagerTest, RebuildAll_ProducesIdenticalResults)
{
  BRepGraph aGraph = buildBoxGraph();
  ASSERT_TRUE(aGraph.IsDone());

  // Collect original surface back-ref counts.
  NCollection_Vector<int> aOrigSurfUserCounts;
  for (int aSIdx = 0; aSIdx < aGraph.Geom().NbSurfaces(); ++aSIdx)
  {
    const BRepGraph_GeomNode::Surf& aSurf = aGraph.Geom().Surface(aSIdx);
    aOrigSurfUserCounts.Append(aGraph.Geom().FacesOnSurface(aSurf.Id).Length());
  }

  NCollection_Vector<int> aOrigCurveUserCounts;
  for (int aCIdx = 0; aCIdx < aGraph.Geom().NbCurves(); ++aCIdx)
  {
    const BRepGraph_GeomNode::Curve& aCurve = aGraph.Geom().Curve(aCIdx);
    aOrigCurveUserCounts.Append(aGraph.Geom().EdgesOnCurve(aCurve.Id).Length());
  }

  // Rebuild.
  BRepGraph_BackRefManager::RebuildAll(aGraph);

  // Verify surface forward-ref counts match.
  for (int aSIdx = 0; aSIdx < aGraph.Geom().NbSurfaces(); ++aSIdx)
  {
    const BRepGraph_GeomNode::Surf& aSurf = aGraph.Geom().Surface(aSIdx);
    EXPECT_EQ(aGraph.Geom().FacesOnSurface(aSurf.Id).Length(),
              aOrigSurfUserCounts.Value(aSIdx))
      << "Surface " << aSIdx << " user count mismatch after RebuildAll";
  }

  // Verify curve forward-ref counts match.
  for (int aCIdx = 0; aCIdx < aGraph.Geom().NbCurves(); ++aCIdx)
  {
    const BRepGraph_GeomNode::Curve& aCurve = aGraph.Geom().Curve(aCIdx);
    EXPECT_EQ(aGraph.Geom().EdgesOnCurve(aCurve.Id).Length(),
              aOrigCurveUserCounts.Value(aCIdx))
      << "Curve " << aCIdx << " user count mismatch after RebuildAll";
  }
}

// Note: RewriteFaceSurface was removed when back-references were
// moved out of geometry nodes. Forward-reference consistency is
// validated through FacesOnSurface / EdgesOnCurve scanning instead.

// SplitEdge: curve gets both sub-edge refs, edge-to-wires updated.
TEST(BRepGraphBackRefManagerTest, SplitEdge_CurveGetsSubEdgeRefs)
{
  BRepGraph aGraph = buildBoxGraph();
  ASSERT_TRUE(aGraph.IsDone());

  // Find first edge with a valid curve.
  int aTargetEdgeIdx = -1;
  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Defs().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = aGraph.Defs().Edge(anEdgeIdx);
    if (anEdge.CurveNodeId.IsValid() && !anEdge.IsDegenerate
        && anEdge.StartVertexDefId.IsValid() && anEdge.EndVertexDefId.IsValid())
    {
      aTargetEdgeIdx = anEdgeIdx;
      break;
    }
  }
  ASSERT_GE(aTargetEdgeIdx, 0) << "No suitable edge found for splitting";

  const BRepGraph_TopoNode::EdgeDef& anOrigEdge = aGraph.Defs().Edge(aTargetEdgeIdx);
  const int aCurveIdx = anOrigEdge.CurveNodeId.Index;
  const BRepGraph_GeomNode::Curve& aCurveRef = aGraph.Geom().Curve(aCurveIdx);
  const int anOrigCurveUserCount = aGraph.Geom().EdgesOnCurve(aCurveRef.Id).Length();

  // Create split vertex.
  const double aMidParam = 0.5 * (anOrigEdge.ParamFirst + anOrigEdge.ParamLast);
  const BRepGraph_TopoNode::VertexDef& aStartVtx =
    aGraph.Defs().Vertex(anOrigEdge.StartVertexDefId.Index);
  const BRepGraph_TopoNode::VertexDef& aEndVtx =
    aGraph.Defs().Vertex(anOrigEdge.EndVertexDefId.Index);
  gp_Pnt aMidPoint(0.5 * (aStartVtx.Point.X() + aEndVtx.Point.X()),
                    0.5 * (aStartVtx.Point.Y() + aEndVtx.Point.Y()),
                    0.5 * (aStartVtx.Point.Z() + aEndVtx.Point.Z()));
  BRepGraph_NodeId aSplitVtx = aGraph.Builder().AddVertexDef(aMidPoint, Precision::Confusion());

  BRepGraph_NodeId aSubA, aSubB;
  BRepGraph_Mutator::SplitEdge(aGraph,
                                BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, aTargetEdgeIdx),
                                aSplitVtx, aMidParam, aSubA, aSubB);

  // Curve should have gained 2 new users (SubA and SubB).
  const BRepGraph_GeomNode::Curve& aCurve = aGraph.Geom().Curve(aCurveIdx);
  const NCollection_Vector<BRepGraph_NodeId> aEdgeIds = aGraph.Geom().EdgesOnCurve(aCurve.Id);
  const int aNewCurveUserCount = aEdgeIds.Length();
  EXPECT_EQ(aNewCurveUserCount, anOrigCurveUserCount + 2);

  // Verify SubA and SubB are in the curve's user list.
  bool aFoundA = false, aFoundB = false;
  for (int anIdx = 0; anIdx < aEdgeIds.Length(); ++anIdx)
  {
    if (aEdgeIds.Value(anIdx) == aSubA)
      aFoundA = true;
    if (aEdgeIds.Value(anIdx) == aSubB)
      aFoundB = true;
  }
  EXPECT_TRUE(aFoundA) << "SubA not found in EdgesOnCurve";
  EXPECT_TRUE(aFoundB) << "SubB not found in EdgesOnCurve";

  // Verify edge-to-wire was updated: SubA and SubB should be in wires.
  const NCollection_Vector<int>& aWiresA = aGraph.RelEdges().WiresOfEdge(aSubA.Index);
  const NCollection_Vector<int>& aWiresB = aGraph.RelEdges().WiresOfEdge(aSubB.Index);
  EXPECT_GT(aWiresA.Length(), 0) << "SubA has no wire refs";
  EXPECT_GT(aWiresB.Length(), 0) << "SubB has no wire refs";
}

// ReplaceEdgeInWireMap: old edge loses wire ref, new edge gains it.
TEST(BRepGraphBackRefManagerTest, ReplaceEdgeInWireMap_UpdatesCorrectly)
{
  BRepGraph aGraph = buildBoxGraph();
  ASSERT_TRUE(aGraph.IsDone());

  // Find an edge that belongs to at least one wire.
  int anOldEdgeIdx = -1;
  int aWireDefIdx  = -1;
  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Defs().NbEdges(); ++anEdgeIdx)
  {
    const NCollection_Vector<int>& aWires = aGraph.RelEdges().WiresOfEdge(anEdgeIdx);
    if (!aWires.IsEmpty())
    {
      anOldEdgeIdx = anEdgeIdx;
      aWireDefIdx  = aWires.Value(0);
      break;
    }
  }
  ASSERT_GE(anOldEdgeIdx, 0);
  ASSERT_GE(aWireDefIdx, 0);

  // Create a new edge to replace the old one.
  const BRepGraph_TopoNode::EdgeDef& anOldEdge = aGraph.Defs().Edge(anOldEdgeIdx);
  BRepGraph_NodeId aNewEdgeId = aGraph.Builder().AddEdgeDef(
    anOldEdge.StartVertexDefId, anOldEdge.EndVertexDefId,
    Handle(Geom_Curve)(), anOldEdge.ParamFirst, anOldEdge.ParamLast, anOldEdge.Tolerance);
  ASSERT_TRUE(aNewEdgeId.IsValid());

  // Replace in wire map.
  BRepGraph_BackRefManager::ReplaceEdgeInWireMap(aGraph, anOldEdgeIdx, aNewEdgeId.Index, aWireDefIdx);

  // Old edge should no longer reference this wire.
  const NCollection_Vector<int>& anOldWires = aGraph.RelEdges().WiresOfEdge(anOldEdgeIdx);
  bool aOldFound = false;
  for (int aW = 0; aW < anOldWires.Length(); ++aW)
  {
    if (anOldWires.Value(aW) == aWireDefIdx)
    {
      aOldFound = true;
      break;
    }
  }
  EXPECT_FALSE(aOldFound) << "Old edge still references wire after replacement";

  // New edge should reference this wire.
  const NCollection_Vector<int>& aNewWires = aGraph.RelEdges().WiresOfEdge(aNewEdgeId.Index);
  bool aNewFound = false;
  for (int aW = 0; aW < aNewWires.Length(); ++aW)
  {
    if (aNewWires.Value(aW) == aWireDefIdx)
    {
      aNewFound = true;
      break;
    }
  }
  EXPECT_TRUE(aNewFound) << "New edge does not reference wire after replacement";
}

// AddRelEdge creates entries in both outgoing and incoming maps.
TEST(BRepGraphBackRefManagerTest, AddRelEdge_CreatesOutAndInEntries)
{
  BRepGraph aGraph = buildBoxGraph();
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GE(aGraph.Defs().NbFaces(), 2);

  const BRepGraph_NodeId aFace0 = aGraph.Defs().Face(0).Id;
  const BRepGraph_NodeId aFace1 = aGraph.Defs().Face(1).Id;

  const int anIdx = BRepGraph_BackRefManager::AddRelEdge(
    aGraph, aFace0, aFace1, BRepGraph_RelEdge::Kind::SameDomain);
  EXPECT_GE(anIdx, 0);

  // Verify outgoing from Face0.
  const NCollection_Vector<BRepGraph_RelEdge>* anOutEdges = aGraph.RelEdges().OutOf(aFace0);
  ASSERT_NE(anOutEdges, nullptr);
  bool aFoundOut = false;
  for (int anI = 0; anI < anOutEdges->Length(); ++anI)
  {
    const BRepGraph_RelEdge& anEdge = anOutEdges->Value(anI);
    if (anEdge.RelKind == BRepGraph_RelEdge::Kind::SameDomain && anEdge.Target == aFace1)
    {
      aFoundOut = true;
      break;
    }
  }
  EXPECT_TRUE(aFoundOut) << "Outgoing SameDomain edge not found";

  // Verify incoming to Face1.
  const NCollection_Vector<BRepGraph_RelEdge>* anInEdges = aGraph.RelEdges().InOf(aFace1);
  ASSERT_NE(anInEdges, nullptr);
  bool aFoundIn = false;
  for (int anI = 0; anI < anInEdges->Length(); ++anI)
  {
    const BRepGraph_RelEdge& anEdge = anInEdges->Value(anI);
    if (anEdge.RelKind == BRepGraph_RelEdge::Kind::SameDomain && anEdge.Source == aFace0)
    {
      aFoundIn = true;
      break;
    }
  }
  EXPECT_TRUE(aFoundIn) << "Incoming SameDomain edge not found";
}

// RemoveRelEdges cleans up both directions.
TEST(BRepGraphBackRefManagerTest, RemoveRelEdges_CleansUpBothDirections)
{
  BRepGraph aGraph = buildBoxGraph();
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GE(aGraph.Defs().NbFaces(), 2);

  const BRepGraph_NodeId aFace0 = aGraph.Defs().Face(0).Id;
  const BRepGraph_NodeId aFace1 = aGraph.Defs().Face(1).Id;

  // Add two edges of different kinds.
  BRepGraph_BackRefManager::AddRelEdge(aGraph, aFace0, aFace1,
                                       BRepGraph_RelEdge::Kind::SameDomain);
  BRepGraph_BackRefManager::AddRelEdge(aGraph, aFace0, aFace1,
                                       BRepGraph_RelEdge::Kind::UserDefined);

  // Remove only SameDomain.
  BRepGraph_BackRefManager::RemoveRelEdges(aGraph, aFace0, aFace1,
                                           BRepGraph_RelEdge::Kind::SameDomain);

  // SameDomain should be gone, UserDefined should remain.
  const NCollection_Vector<BRepGraph_RelEdge>* anOutEdges = aGraph.RelEdges().OutOf(aFace0);
  ASSERT_NE(anOutEdges, nullptr);
  bool aFoundSD = false;
  bool aFoundUD = false;
  for (int anI = 0; anI < anOutEdges->Length(); ++anI)
  {
    if (anOutEdges->Value(anI).RelKind == BRepGraph_RelEdge::Kind::SameDomain
        && anOutEdges->Value(anI).Target == aFace1)
      aFoundSD = true;
    if (anOutEdges->Value(anI).RelKind == BRepGraph_RelEdge::Kind::UserDefined
        && anOutEdges->Value(anI).Target == aFace1)
      aFoundUD = true;
  }
  EXPECT_FALSE(aFoundSD) << "SameDomain edge should have been removed";
  EXPECT_TRUE(aFoundUD) << "UserDefined edge should still be present";

  // Check incoming side too.
  const NCollection_Vector<BRepGraph_RelEdge>* anInEdges = aGraph.RelEdges().InOf(aFace1);
  ASSERT_NE(anInEdges, nullptr);
  bool aFoundInSD = false;
  for (int anI = 0; anI < anInEdges->Length(); ++anI)
  {
    if (anInEdges->Value(anI).RelKind == BRepGraph_RelEdge::Kind::SameDomain
        && anInEdges->Value(anI).Source == aFace0)
      aFoundInSD = true;
  }
  EXPECT_FALSE(aFoundInSD) << "Incoming SameDomain edge should have been removed";
}

// ClearRelEdges unbinds all edges for a node.
TEST(BRepGraphBackRefManagerTest, ClearRelEdges_UnbindsNode)
{
  BRepGraph aGraph = buildBoxGraph();
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GE(aGraph.Defs().NbFaces(), 3);

  const BRepGraph_NodeId aFace0 = aGraph.Defs().Face(0).Id;
  const BRepGraph_NodeId aFace1 = aGraph.Defs().Face(1).Id;
  const BRepGraph_NodeId aFace2 = aGraph.Defs().Face(2).Id;

  BRepGraph_BackRefManager::AddRelEdge(aGraph, aFace0, aFace1,
                                       BRepGraph_RelEdge::Kind::SameDomain);
  BRepGraph_BackRefManager::AddRelEdge(aGraph, aFace2, aFace0,
                                       BRepGraph_RelEdge::Kind::UserDefined);

  // Clear all rel-edges for Face0.
  BRepGraph_BackRefManager::ClearRelEdges(aGraph, aFace0);

  // Face0 should have no outgoing edges.
  EXPECT_EQ(aGraph.RelEdges().OutOf(aFace0), nullptr);
  // Face0 should have no incoming edges.
  EXPECT_EQ(aGraph.RelEdges().InOf(aFace0), nullptr);
}

// ClearAll empties all back-ref containers.
TEST(BRepGraphBackRefManagerTest, ClearAll_EmptiesAllBackRefs)
{
  BRepGraph aGraph = buildBoxGraph();
  ASSERT_TRUE(aGraph.IsDone());

  // Verify we have data before clearing.
  ASSERT_GT(aGraph.Defs().NbFaces(), 0);
  ASSERT_GT(aGraph.Defs().NbEdges(), 0);

  // Add a RelEdge so we can verify it gets cleared.
  const BRepGraph_NodeId aFace0 = aGraph.Defs().Face(0).Id;
  const BRepGraph_NodeId aFace1 = aGraph.Defs().Face(1).Id;
  BRepGraph_BackRefManager::AddRelEdge(aGraph, aFace0, aFace1,
                                       BRepGraph_RelEdge::Kind::SameDomain);

  BRepGraph_BackRefManager::ClearAll(aGraph);

  // FacesOnSurface/EdgesOnCurve are now computed by scanning defs,
  // not stored as back-refs, so ClearAll does not affect them.

  // Edge-to-wire map should be empty (verify via RelEdges view).
  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Defs().NbEdges(); ++anEdgeIdx)
    EXPECT_TRUE(aGraph.RelEdges().WiresOfEdge(anEdgeIdx).IsEmpty())
      << "Edge " << anEdgeIdx << " still has wire refs";

  // RelEdges should be gone.
  EXPECT_EQ(aGraph.RelEdges().OutOf(aFace0), nullptr);
  EXPECT_EQ(aGraph.RelEdges().InOf(aFace1), nullptr);
}
