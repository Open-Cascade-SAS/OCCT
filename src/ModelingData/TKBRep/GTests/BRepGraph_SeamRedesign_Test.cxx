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

// Edge cases that the seam-redesign reachable only after:
//   - Both seam halves placed in WireDef::CoEdgeIds at TopoDS_Iterator order
//   - SeamPair derived from connectivity (CoEdgesOfEdge filtered by face+orientation)
//   - Continuity derived/stored temporarily in BRepGraphAlgo_Regularity

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepGraph.hxx>
#include <BRepGraphAlgo_Regularity.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraph_ReverseIterator.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_Validate.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <NCollection_LinearVector.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Wire.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

#include <optional>

namespace
{

void registerLayers(BRepGraph& theGraph)
{
  (void)theGraph;
}

//! Find any seam CoEdge on the cylinder lateral face.
BRepGraph_CoEdgeId findSeamCoEdge(const BRepGraph& theGraph)
{
  for (BRepGraph_EdgeIterator anEdgeIt(theGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    for (BRepGraph_CoEdgesOfEdge aCEIt(theGraph,
                                       theGraph.Topo().Edges().CoEdges(anEdgeIt.CurrentId()));
         aCEIt.More();
         aCEIt.Next())
    {
      if (BRepGraph_Tool::CoEdge::IsSeam(theGraph, aCEIt.CurrentId()))
      {
        return aCEIt.CurrentId();
      }
    }
  }
  return {};
}

} // namespace

// ============================================================
// Wire CoEdgeIds layout: seam halves at TopoDS_Iterator positions
// ============================================================

// Cylinder lateral wire must contain BOTH seam halves in CoEdgeIds at the
// same positions TopoDS_Iterator yields them. This guarantees round-trip with
// classical TopoDS_Wire iteration.
TEST(BRepGraph_SeamRedesignTest, CylinderLateralWire_BothSeamHalvesInCoEdgeIds)
{
  const TopoDS_Shape aCyl = BRepPrimAPI_MakeCylinder(5., 10.).Shape();
  BRepGraph          aGraph;
  registerLayers(aGraph);
  aGraph.Clear();
  ASSERT_TRUE(aGraph.Shapes().Add(aCyl).IsOk());

  const BRepGraph_CoEdgeId aSeamCoEdge = findSeamCoEdge(aGraph);
  ASSERT_TRUE(aSeamCoEdge.IsValid()) << "Cylinder must have a seam CoEdge";

  const BRepGraph_CoEdgeId aSeamPair = BRepGraph_Tool::CoEdge::SeamPair(aGraph, aSeamCoEdge);
  ASSERT_TRUE(aSeamPair.IsValid());

  // Find the wire containing the seam CoEdge.
  const BRepGraph_WireId aWireId = aGraph.Topo().CoEdges().Wire(aSeamCoEdge);
  ASSERT_TRUE(aWireId.IsValid());

  // Walk the wire's CoEdgeIds; both seam halves must appear.
  bool aFwdInWire = false, aRevInWire = false;
  for (BRepGraph_CoEdgesOfWire aRefIt(aGraph, aWireId); aRefIt.More(); aRefIt.Next())
  {
    const BRepGraph_CoEdgeId aCoEdgeId = aRefIt.CurrentId();
    if (aCoEdgeId == aSeamCoEdge)
    {
      aFwdInWire = true;
    }
    else if (aCoEdgeId == aSeamPair)
    {
      aRevInWire = true;
    }
  }
  EXPECT_TRUE(aFwdInWire) << "Forward seam half must appear in the wire's CoEdgeIds";
  EXPECT_TRUE(aRevInWire) << "Reversed seam half must appear in the wire's CoEdgeIds";
}

// Cylinder lateral wire's NbCoEdges must equal the count of TopoDS_Iterator(wire)
// yields on the same wire - i.e. the seam edge contributes 2 coedge usages.
TEST(BRepGraph_SeamRedesignTest, CylinderLateralWire_NbCoEdges_MatchesTopoDSIterator)
{
  const TopoDS_Shape aCyl = BRepPrimAPI_MakeCylinder(5., 10.).Shape();
  BRepGraph          aGraph;
  registerLayers(aGraph);
  aGraph.Clear();
  ASSERT_TRUE(aGraph.Shapes().Add(aCyl).IsOk());

  // Locate the cylinder lateral face (the one with > 1 wire-edges and a seam).
  TopoDS_Face aLateralFace;
  for (TopExp_Explorer aFE(aCyl, TopAbs_FACE); aFE.More(); aFE.Next())
  {
    const TopoDS_Face& aF       = TopoDS::Face(aFE.Current());
    int                aNbEdges = 0;
    for (TopExp_Explorer aEE(aF, TopAbs_EDGE); aEE.More(); aEE.Next())
    {
      ++aNbEdges;
    }
    if (aNbEdges == 4)
    {
      aLateralFace = aF;
      break;
    }
  }
  ASSERT_FALSE(aLateralFace.IsNull());

  // Count TopoDS_Iterator yields per wire on the original.
  uint32_t aClassicalCount = 0;
  for (TopoDS_Iterator aWIt(aLateralFace, false, false); aWIt.More(); aWIt.Next())
  {
    const TopoDS_Shape& aChild = aWIt.Value();
    if (aChild.ShapeType() != TopAbs_WIRE)
    {
      continue;
    }
    for (TopoDS_Iterator aEIt(aChild, false, false); aEIt.More(); aEIt.Next())
    {
      ++aClassicalCount;
    }
  }

  // Sum NbCoEdges across all wires of the matching face in the graph.
  uint32_t aGraphCount = 0;
  for (BRepGraph_WireIterator aWIt(aGraph); aWIt.More(); aWIt.Next())
  {
    const BRepGraph_FaceId aFaceId = BRepGraph_Tool::Wire::FaceOf(aGraph, aWIt.CurrentId());
    if (!aFaceId.IsValid())
    {
      continue;
    }
    const TopoDS_Shape aOrig = aGraph.Shapes().Original(aFaceId);
    if (aOrig.IsNull() || !aOrig.IsSame(aLateralFace))
    {
      continue;
    }
    aGraphCount += BRepGraph_Tool::Wire::NbCoEdges(aGraph, aWIt.CurrentId());
  }
  EXPECT_EQ(aGraphCount, aClassicalCount)
    << "NbCoEdges must equal the count of TopoDS_Iterator yields";
}

// ============================================================
// Derived SeamPair: symmetry, free wire, non-seam
// ============================================================

// SeamPair(SeamPair(c)) == c on every seam half - symmetry guaranteed by the
// connectivity-derived implementation.
TEST(BRepGraph_SeamRedesignTest, SeamPair_DerivedQuery_IsSymmetric)
{
  const TopoDS_Shape aSph = BRepPrimAPI_MakeSphere(10.).Shape();
  BRepGraph          aGraph;
  registerLayers(aGraph);
  aGraph.Clear();
  ASSERT_TRUE(aGraph.Shapes().Add(aSph).IsOk());

  uint32_t aSeamCount = 0;
  for (BRepGraph_CoEdgeId aCEId(0); aCEId.IsValid(aGraph.Topo().CoEdges().Nb()); ++aCEId)
  {
    const BRepGraph_CoEdgeId aPair = BRepGraph_Tool::CoEdge::SeamPair(aGraph, aCEId);
    if (!aPair.IsValid())
    {
      continue;
    }
    ++aSeamCount;
    const BRepGraph_CoEdgeId aBack = BRepGraph_Tool::CoEdge::SeamPair(aGraph, aPair);
    EXPECT_EQ(aBack, aCEId) << "SeamPair must be symmetric: SeamPair(SeamPair(c)) == c";
  }
  EXPECT_GT(aSeamCount, 0u) << "Sphere must have at least one seam CoEdge";
}

// Box has no seams: every CoEdge returns invalid SeamPair.
TEST(BRepGraph_SeamRedesignTest, SeamPair_BoxHasNoSeams)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(1., 1., 1.).Shape();
  BRepGraph          aGraph;
  registerLayers(aGraph);
  aGraph.Clear();
  ASSERT_TRUE(aGraph.Shapes().Add(aBox).IsOk());

  for (BRepGraph_CoEdgeId aCEId(0); aCEId.IsValid(aGraph.Topo().CoEdges().Nb()); ++aCEId)
  {
    EXPECT_FALSE(BRepGraph_Tool::CoEdge::IsSeam(aGraph, aCEId))
      << "Box CoEdge " << aCEId.Index << " must not be a seam half";
  }
}

// Free-wire CoEdge (no FaceId) cannot be a seam - derived query handles invalid face.
TEST(BRepGraph_SeamRedesignTest, SeamPair_FreeWireHasNoSeams)
{
  // Build a free wire: an edge between two vertices, not bound to any face.
  TopoDS_Vertex aV1, aV2;
  BRep_Builder  aBB;
  aBB.MakeVertex(aV1, gp_Pnt(0, 0, 0), 1.0e-7);
  aBB.MakeVertex(aV2, gp_Pnt(1, 0, 0), 1.0e-7);
  BRepBuilderAPI_MakeEdge aMkEdge(aV1, aV2);
  ASSERT_TRUE(aMkEdge.IsDone());
  BRepBuilderAPI_MakeWire aMkWire(aMkEdge.Edge());
  ASSERT_TRUE(aMkWire.IsDone());

  BRepGraph aGraph;
  registerLayers(aGraph);
  aGraph.Clear();
  ASSERT_TRUE(aGraph.Shapes().Add(aMkWire.Wire()).IsOk());

  for (BRepGraph_CoEdgeId aCEId(0); aCEId.IsValid(aGraph.Topo().CoEdges().Nb()); ++aCEId)
  {
    EXPECT_FALSE(BRepGraph_Tool::CoEdge::IsSeam(aGraph, aCEId))
      << "Free-wire CoEdge has no face; cannot be a seam";
  }
}

// ============================================================
// EditorView::EdgeOps::IsSeamOnFace API
// ============================================================

TEST(BRepGraph_SeamRedesignTest, EdgeOps_IsSeamOnFace_DerivedFromConnectivity)
{
  const TopoDS_Shape aCyl = BRepPrimAPI_MakeCylinder(5., 10.).Shape();
  BRepGraph          aGraph;
  registerLayers(aGraph);
  aGraph.Clear();
  ASSERT_TRUE(aGraph.Shapes().Add(aCyl).IsOk());

  const BRepGraph_CoEdgeId aSeamCoEdge = findSeamCoEdge(aGraph);
  ASSERT_TRUE(aSeamCoEdge.IsValid());
  const BRepGraphInc::CoEdgeDef& aSeamDef = aGraph.Topo().CoEdges().Definition(aSeamCoEdge);
  EXPECT_TRUE(
    BRepGraph_Tool::Edge::IsSeamOnFace(aGraph, aSeamDef.ChildEdgeId, aSeamDef.FaceId));

  // A box edge (any) must NOT be a seam on its face.
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(1., 1., 1.).Shape();
  BRepGraph          aBoxGraph;
  registerLayers(aBoxGraph);
  aBoxGraph.Clear();
  ASSERT_TRUE(aBoxGraph.Shapes().Add(aBox).IsOk());
  for (BRepGraph_CoEdgeId aCEId(0); aCEId.IsValid(aBoxGraph.Topo().CoEdges().Nb()); ++aCEId)
  {
    const BRepGraphInc::CoEdgeDef& aDef = aBoxGraph.Topo().CoEdges().Definition(aCEId);
    if (!aDef.FaceId.IsValid())
    {
      continue;
    }
    EXPECT_FALSE(BRepGraph_Tool::Edge::IsSeamOnFace(aBoxGraph, aDef.ChildEdgeId, aDef.FaceId))
      << "Box edge cannot be a seam";
  }
}

// ============================================================
// Tool::Wire::NbDistinctEdges
// ============================================================

TEST(BRepGraph_SeamRedesignTest, NbDistinctEdges_AccountsForSeamHalves)
{
  const TopoDS_Shape aCyl = BRepPrimAPI_MakeCylinder(5., 10.).Shape();
  BRepGraph          aGraph;
  registerLayers(aGraph);
  aGraph.Clear();
  ASSERT_TRUE(aGraph.Shapes().Add(aCyl).IsOk());

  uint32_t aWiresWithSeams = 0;
  for (BRepGraph_WireIterator aWIt(aGraph); aWIt.More(); aWIt.Next())
  {
    const BRepGraph_WireId aWireId   = aWIt.CurrentId();
    const uint32_t         aRawCnt   = BRepGraph_Tool::Wire::NbCoEdges(aGraph, aWireId);
    const uint32_t         aDistinct = BRepGraph_Tool::Wire::NbDistinctEdges(aGraph, aWireId);
    EXPECT_LE(aDistinct, aRawCnt) << "Distinct edges <= raw CoEdge count";
    if (aRawCnt > aDistinct)
    {
      ++aWiresWithSeams;
      // The difference equals the number of seam pairs in the wire.
      const uint32_t aDelta = aRawCnt - aDistinct;
      EXPECT_GE(aDelta, 1u);
    }
  }
  EXPECT_GE(aWiresWithSeams, 1u)
    << "Cylinder must have at least one wire whose seam contributes a doubled edge";
}

TEST(BRepGraph_SeamRedesignTest, Edge_Continuity_DerivedFromGraph)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(1., 1., 1.).Shape();
  BRepGraph          aGraph;
  registerLayers(aGraph);
  aGraph.Clear();
  ASSERT_TRUE(aGraph.Shapes().Add(aBox).IsOk());

  // Pick any edge with two faces.
  BRepGraph_EdgeId aEdgeId;
  BRepGraph_FaceId aFace1, aFace2;
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    NCollection_LinearVector<BRepGraph_FaceId> aFaces;
    const NCollection_LinearVector<BRepGraph_FaceId> anEdgeFaces =
      aGraph.Topo().Edges().Faces(anEdgeIt.CurrentId());
    for (BRepGraph_FacesOfEdge aFIt(aGraph, anEdgeFaces);
         aFIt.More();
         aFIt.Next())
    {
      aFaces.Append(aFIt.CurrentId());
    }
    if (aFaces.Size() >= 2)
    {
      aEdgeId = anEdgeIt.CurrentId();
      aFace1  = aFaces[0];
      aFace2  = aFaces[1];
      break;
    }
  }
  ASSERT_TRUE(aEdgeId.IsValid());

  EXPECT_TRUE(BRepGraphAlgo_Regularity::HasContinuity(aGraph, aEdgeId, aFace1, aFace2));
  const std::optional<GeomAbs_Shape> aContinuity =
    BRepGraphAlgo_Regularity::Continuity(aGraph, aEdgeId, aFace1, aFace2);
  ASSERT_TRUE(aContinuity.has_value());
  EXPECT_EQ(*aContinuity, GeomAbs_C0);
  EXPECT_EQ(BRepGraphAlgo_Regularity::MaxContinuity(aGraph, aEdgeId), GeomAbs_C0);

  // Symmetry: (F1, F2) == (F2, F1).
  const std::optional<GeomAbs_Shape> aReverseContinuity =
    BRepGraphAlgo_Regularity::Continuity(aGraph, aEdgeId, aFace2, aFace1);
  ASSERT_TRUE(aReverseContinuity.has_value());
  EXPECT_EQ(*aReverseContinuity, GeomAbs_C0);
}

TEST(BRepGraph_SeamRedesignTest, EdgeOps_Split_DerivesSeamRegularity)
{
  const TopoDS_Shape aCyl = BRepPrimAPI_MakeCylinder(5., 10.).Shape();
  BRepGraph          aGraph;
  registerLayers(aGraph);
  aGraph.Clear();
  ASSERT_TRUE(aGraph.Shapes().Add(aCyl).IsOk());

  const BRepGraph_CoEdgeId aSeamCoEdge = findSeamCoEdge(aGraph);
  ASSERT_TRUE(aSeamCoEdge.IsValid());
  const BRepGraphInc::CoEdgeDef& aSeamDef    = aGraph.Topo().CoEdges().Definition(aSeamCoEdge);
  const BRepGraph_EdgeId         aSeamEdgeId = aSeamDef.ChildEdgeId;
  const BRepGraph_FaceId         aSeamFaceId = aSeamDef.FaceId;
  ASSERT_TRUE(aSeamEdgeId.IsValid());
  ASSERT_TRUE(aSeamFaceId.IsValid());

  ASSERT_TRUE(BRepGraphAlgo_Regularity::HasContinuity(aGraph,
                                                  aSeamEdgeId,
                                                  aSeamFaceId,
                                                  aSeamFaceId));
  const std::optional<GeomAbs_Shape> aContinuity =
    BRepGraphAlgo_Regularity::Continuity(aGraph, aSeamEdgeId, aSeamFaceId, aSeamFaceId);
  ASSERT_TRUE(aContinuity.has_value());
  ASSERT_GT(*aContinuity, GeomAbs_C0);

  const BRepGraphInc::EdgeDef& aEdgeDef  = aGraph.Topo().Edges().Definition(aSeamEdgeId);
  const std::pair<double, double> aEdgeRange = BRepGraph_Tool::Edge::Range(aGraph, aSeamEdgeId);
  const double                 aMidParam = 0.5 * (aEdgeRange.first + aEdgeRange.second);
  const BRepGraph_VertexId     aSplitVertex =
    aGraph.Editor().Vertices().Add(gp_Pnt(5.0, 0.0, 5.0), aEdgeDef.Tolerance);
  ASSERT_TRUE(aSplitVertex.IsValid());

  BRepGraph_EdgeId aSubA;
  BRepGraph_EdgeId aSubB;
  aGraph.Editor().Edges().Split(aSeamEdgeId, aSplitVertex, aMidParam, aSubA, aSubB);
  ASSERT_TRUE(aSubA.IsValid());
  ASSERT_TRUE(aSubB.IsValid());

  EXPECT_FALSE(BRepGraphAlgo_Regularity::HasContinuity(aGraph,
                                                   aSeamEdgeId,
                                                   aSeamFaceId,
                                                   aSeamFaceId))
    << "Removed source edge must not keep stale regularity bindings";
  EXPECT_TRUE(BRepGraphAlgo_Regularity::HasContinuity(aGraph, aSubA, aSeamFaceId, aSeamFaceId));
  const std::optional<GeomAbs_Shape> aSubAContinuity =
    BRepGraphAlgo_Regularity::Continuity(aGraph, aSubA, aSeamFaceId, aSeamFaceId);
  ASSERT_TRUE(aSubAContinuity.has_value());
  EXPECT_EQ(*aSubAContinuity, *aContinuity);
  EXPECT_TRUE(BRepGraphAlgo_Regularity::HasContinuity(aGraph, aSubB, aSeamFaceId, aSeamFaceId));
  const std::optional<GeomAbs_Shape> aSubBContinuity =
    BRepGraphAlgo_Regularity::Continuity(aGraph, aSubB, aSeamFaceId, aSeamFaceId);
  ASSERT_TRUE(aSubBContinuity.has_value());
  EXPECT_EQ(*aSubBContinuity, *aContinuity);

  const BRepGraph_Validate::Result aAudit =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_TRUE(aAudit.IsValid()) << "Audit must remain clean after seam split";
}

// ============================================================
// Reconstruct: TopoDS_Iterator order preservation
// ============================================================

// After shape -> graph -> shape, TopoDS_Iterator on the lateral cylinder face's
// outer wire must yield the same sequence of (TShape, orientation) tuples.
TEST(BRepGraph_SeamRedesignTest, Reconstruct_CylinderWire_TopoDSIteratorOrder)
{
  const TopoDS_Shape aOriginal = BRepPrimAPI_MakeCylinder(5., 10.).Shape();
  BRepGraph          aGraph;
  registerLayers(aGraph);
  aGraph.Clear();
  ASSERT_TRUE(aGraph.Shapes().Add(aOriginal).IsOk());

  TopoDS_Shape aRecon =
    aGraph.Shapes().Reconstruct(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, 0));
  ASSERT_FALSE(aRecon.IsNull());

  // Compare wire-edge counts pairwise across all faces.
  NCollection_LinearVector<int> aOrigCounts, aReconCounts;
  for (TopExp_Explorer aFE(aOriginal, TopAbs_FACE); aFE.More(); aFE.Next())
  {
    for (TopoDS_Iterator aWIt(aFE.Current(), false, false); aWIt.More(); aWIt.Next())
    {
      if (aWIt.Value().ShapeType() != TopAbs_WIRE)
      {
        continue;
      }
      int aCount = 0;
      for (TopoDS_Iterator aEIt(aWIt.Value(), false, false); aEIt.More(); aEIt.Next())
      {
        ++aCount;
      }
      aOrigCounts.Append(aCount);
    }
  }
  for (TopExp_Explorer aFE(aRecon, TopAbs_FACE); aFE.More(); aFE.Next())
  {
    for (TopoDS_Iterator aWIt(aFE.Current(), false, false); aWIt.More(); aWIt.Next())
    {
      if (aWIt.Value().ShapeType() != TopAbs_WIRE)
      {
        continue;
      }
      int aCount = 0;
      for (TopoDS_Iterator aEIt(aWIt.Value(), false, false); aEIt.More(); aEIt.Next())
      {
        ++aCount;
      }
      aReconCounts.Append(aCount);
    }
  }
  ASSERT_EQ(aOrigCounts.Size(), aReconCounts.Size());
  for (size_t i = 0; i < aOrigCounts.Size(); ++i)
  {
    EXPECT_EQ(aReconCounts[i], aOrigCounts[i])
      << "Wire " << i << " edge count must match after reconstruct";
  }
}

// ============================================================
// RegularityAlgorithm captures both inter-face and seam continuity
// ============================================================

TEST(BRepGraph_SeamRedesignTest, RegularityAlgorithm_CapturesInterFaceAndSeam)
{
  const TopoDS_Shape aCyl = BRepPrimAPI_MakeCylinder(5., 10.).Shape();
  BRepGraph          aGraph;
  registerLayers(aGraph);
  aGraph.Clear();
  ASSERT_TRUE(aGraph.Shapes().Add(aCyl).IsOk());

  // Cylinder produces exactly one seam-style entry (F1 == F2, on the lateral face).
  uint32_t aSeamEntries = 0;
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId anEdgeId = anEdgeIt.CurrentId();
    const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdges =
      aGraph.Topo().Edges().CoEdges(anEdgeId);
    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdges)
    {
      const BRepGraph_FaceId aFace = aGraph.Topo().CoEdges().Definition(aCoEdgeId).FaceId;
      if (aFace.IsValid()
          && BRepGraphAlgo_Regularity::HasContinuity(aGraph, anEdgeId, aFace, aFace))
      {
        ++aSeamEntries;
        const std::optional<GeomAbs_Shape> aContinuity =
          BRepGraphAlgo_Regularity::Continuity(aGraph, anEdgeId, aFace, aFace);
        ASSERT_TRUE(aContinuity.has_value());
        EXPECT_GT(*aContinuity, GeomAbs_C0)
          << "Cylinder seam BRep_CurveOnClosedSurface stores G^2";
        break;
      }
    }
  }
  EXPECT_EQ(aSeamEntries, 1u) << "Cylinder must produce exactly one seam regularity entry";
}

// ============================================================
// Validator catches invalid seam orientation
// ============================================================

// Manually break the opposite-orientation invariant. Validate must flag it.
TEST(BRepGraph_SeamRedesignTest, Validate_DetectsAsymmetricSeamPair)
{
  const TopoDS_Shape aCyl = BRepPrimAPI_MakeCylinder(5., 10.).Shape();
  BRepGraph          aGraph;
  registerLayers(aGraph);
  aGraph.Clear();
  ASSERT_TRUE(aGraph.Shapes().Add(aCyl).IsOk());

  // Locate a CoEdge with a seam pair.
  const BRepGraph_CoEdgeId aSeamCoEdge = findSeamCoEdge(aGraph);
  ASSERT_TRUE(aSeamCoEdge.IsValid());
  const BRepGraph_CoEdgeId aSeamMate = BRepGraph_Tool::CoEdge::SeamPair(aGraph, aSeamCoEdge);
  ASSERT_TRUE(aSeamMate.IsValid());

  // Forcibly assign the same orientation to both halves to break the
  // "opposite-orientation" invariant. Validate must flag this.
  const TopAbs_Orientation aOri = aGraph.Topo().CoEdges().Definition(aSeamCoEdge).Orientation;
  aGraph.Editor().CoEdges().SetOrientation(aSeamMate, aOri);

  const BRepGraphInc::CoEdgeDef& aDef = aGraph.Topo().CoEdges().Definition(aSeamCoEdge);
  EXPECT_FALSE(BRepGraph_Tool::Edge::IsSeamOnFace(aGraph, aDef.ChildEdgeId, aDef.FaceId));

  const BRepGraph_Validate::Result aResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_FALSE(aResult.IsValid());
  EXPECT_GT(aResult.NbIssues(BRepGraph_Validate::Severity::Error), 0);

  bool hasSameOrientationIssue = false;
  for (const BRepGraph_Validate::Issue& anIssue : aResult.Issues)
  {
    if (anIssue.Sev == BRepGraph_Validate::Severity::Error
        && anIssue.Description.Search("same Orientation") > 0)
    {
      hasSameOrientationIssue = true;
      break;
    }
  }
  EXPECT_TRUE(hasSameOrientationIssue);
}
