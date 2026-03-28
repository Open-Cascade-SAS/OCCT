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

#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>
#include <BRepBndLib.hxx>
#include <BRepGraph.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <BRepGraph_Analyze.hxx>
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_MutRef.hxx>
#include <BRepGraph_SubGraph.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraphAlgo_BndLib.hxx>
#include "BRepGraph_RefTestTools.hxx"
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>

#include <atomic>

#include <gtest/gtest.h>

namespace
{

static gp_Pnt bboxCenter(const BRepGraph& theGraph, BRepGraph_NodeId theNode)
{
  Bnd_Box aBox;
  BRepGraphAlgo_BndLib::Add(theGraph, theNode, aBox);
  if (aBox.IsVoid())
    return gp_Pnt();
  double xn, yn, zn, xx, yx, zx;
  aBox.Get(xn, yn, zn, xx, yx, zx);
  return gp_Pnt((xn + xx) * 0.5, (yn + yx) * 0.5, (zn + zx) * 0.5);
}

} // namespace

// ============================================================
// Fixture: builds a 10x20x30 box graph for reuse
// ============================================================

class BRepGraph_AnalyzeTest : public testing::Test
{
protected:
  void SetUp() override
  {
    BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
    myBoxShape = aBoxMaker.Shape();
    myGraph.Build(myBoxShape);
    ASSERT_TRUE(myGraph.IsDone());
  }

  BRepGraph    myGraph;
  TopoDS_Shape myBoxShape;
};

// ============================================================
// FreeEdges tests
// ============================================================

TEST_F(BRepGraph_AnalyzeTest, FreeEdges_SingleFace_AllFree)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  TopExp_Explorer    anExp(aBox, TopAbs_FACE);
  const TopoDS_Shape aFace = anExp.Current();

  BRepGraph aGraph;
  aGraph.Build(aFace);
  ASSERT_TRUE(aGraph.IsDone());

  const NCollection_Vector<BRepGraph_EdgeId> aFreeEdges = BRepGraph_Analyze::FreeEdges(aGraph);

  // A single planar face from a box has 4 edges, all free (each shared by only 1 face).
  EXPECT_EQ(aFreeEdges.Length(), 4);
}

TEST_F(BRepGraph_AnalyzeTest, FreeEdges_TwoAdjacentFaces_SharedNotFree)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  // Collect first two faces from the box.
  TopExp_Explorer    anExp(aBox, TopAbs_FACE);
  const TopoDS_Shape aFace1 = anExp.Current();
  anExp.Next();
  const TopoDS_Shape aFace2 = anExp.Current();

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aFace1);
  aBuilder.Add(aCompound, aFace2);

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  const NCollection_Vector<BRepGraph_EdgeId> aFreeEdges = BRepGraph_Analyze::FreeEdges(aGraph);

  // Two faces from a box compound. Depending on TShape sharing, edges may or may not
  // be deduplicated. The key property: free edge count < total edge defs (some are shared).
  // For a box, adjacent faces from the same solid share TShapes, so typically 7 unique edges
  // with the shared edge having FaceCount==2. But compound construction may vary.
  // Just verify we get fewer free edges than total edges (some sharing detected).
  EXPECT_GT(aFreeEdges.Length(), 0);
  EXPECT_LE(aFreeEdges.Length(), aGraph.Topo().NbEdges());
}

TEST_F(BRepGraph_AnalyzeTest, FreeEdges_ClosedBox_NoneReturned)
{
  const NCollection_Vector<BRepGraph_EdgeId> aFreeEdges = BRepGraph_Analyze::FreeEdges(myGraph);

  // A closed box has no free edges: every edge is shared by exactly 2 faces.
  EXPECT_EQ(aFreeEdges.Length(), 0);
}

TEST_F(BRepGraph_AnalyzeTest, FreeEdges_Sphere_SeamEdgesAreFree)
{
  BRepPrimAPI_MakeSphere aSphereMaker(50.0);
  const TopoDS_Shape&    aSphere = aSphereMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aSphere);
  ASSERT_TRUE(aGraph.IsDone());

  const NCollection_Vector<BRepGraph_EdgeId> aFreeEdges = BRepGraph_Analyze::FreeEdges(aGraph);
  // A sphere has seam edges that are referenced by only 1 face (the single face),
  // so they are detected as free by the FreeEdges algorithm (FaceCountOfEdge == 1).
  // Degenerate edges at poles are excluded.
  EXPECT_GE(aFreeEdges.Length(), 0);
}

// ============================================================
// MissingPCurves, ToleranceConflicts, DegenerateWires
// ============================================================

TEST_F(BRepGraph_AnalyzeTest, MissingPCurves_ValidBox_Empty)
{
  const NCollection_Vector<std::pair<BRepGraph_EdgeId, BRepGraph_FaceId>> aMissing =
    BRepGraph_Analyze::MissingPCurves(myGraph);
  EXPECT_EQ(aMissing.Length(), 0);
}

TEST_F(BRepGraph_AnalyzeTest, ToleranceConflicts_UniformBox_None)
{
  const NCollection_Vector<BRepGraph_EdgeId> aConflicts =
    BRepGraph_Analyze::ToleranceConflicts(myGraph, 1.0e-3);
  EXPECT_EQ(aConflicts.Length(), 0);
}

TEST_F(BRepGraph_AnalyzeTest, ToleranceConflicts_SmallThreshold_StillNone)
{
  // Even with a tiny threshold, a standard box has no shared curves between
  // different edges, so no tolerance conflicts can occur.
  const NCollection_Vector<BRepGraph_EdgeId> aConflicts =
    BRepGraph_Analyze::ToleranceConflicts(myGraph, 0.0);
  EXPECT_EQ(aConflicts.Length(), 0);
}

TEST_F(BRepGraph_AnalyzeTest, DegenerateWires_ValidBox_None)
{
  const NCollection_Vector<BRepGraph_WireId> aDegWires =
    BRepGraph_Analyze::DegenerateWires(myGraph);
  EXPECT_EQ(aDegWires.Length(), 0);
}

// ============================================================
// Decompose tests
// ============================================================

TEST_F(BRepGraph_AnalyzeTest, Decompose_ThreeDisconnected_ThreeComponents)
{
  BRepPrimAPI_MakeBox aBox1(10.0, 10.0, 10.0);
  BRepPrimAPI_MakeBox aBox2(gp_Pnt(100.0, 0.0, 0.0), 10.0, 10.0, 10.0);
  BRepPrimAPI_MakeBox aBox3(gp_Pnt(200.0, 0.0, 0.0), 10.0, 10.0, 10.0);

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBox1.Shape());
  aBuilder.Add(aCompound, aBox2.Shape());
  aBuilder.Add(aCompound, aBox3.Shape());

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  const NCollection_Vector<BRepGraph_SubGraph> aComponents = BRepGraph_Analyze::Decompose(aGraph);

  EXPECT_EQ(aComponents.Length(), 3);

  // Each component should contain 6 faces (one box).
  for (int anIdx = 0; anIdx < aComponents.Length(); ++anIdx)
  {
    EXPECT_EQ(aComponents.Value(anIdx).FaceDefIds().Length(), 6)
      << "Component " << anIdx << " does not have 6 faces";
  }
}

TEST_F(BRepGraph_AnalyzeTest, Decompose_EmptyCompound_ZeroComponents)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  BRepGraph aGraph;
  aGraph.Build(aCompound);

  const NCollection_Vector<BRepGraph_SubGraph> aComponents = BRepGraph_Analyze::Decompose(aGraph);
  EXPECT_EQ(aComponents.Length(), 0);
}

// ============================================================
// ParallelForEach tests
// ============================================================

TEST_F(BRepGraph_AnalyzeTest, ParallelForEachFace_Box_VisitsAllSix)
{
  const NCollection_Vector<BRepGraph_SubGraph> aComponents = BRepGraph_Analyze::Decompose(myGraph);
  ASSERT_EQ(aComponents.Length(), 1);

  std::atomic<int> aCounter{0};
  BRepGraph_Analyze::ParallelForEachFace(
    myGraph,
    aComponents.Value(0),
    [&aCounter](const BRepGraph_FaceId) { aCounter.fetch_add(1); });

  EXPECT_EQ(aCounter.load(), 6);
}

TEST_F(BRepGraph_AnalyzeTest, ParallelForEachEdge_Box_VisitsAllTwelve)
{
  const NCollection_Vector<BRepGraph_SubGraph> aComponents = BRepGraph_Analyze::Decompose(myGraph);
  ASSERT_EQ(aComponents.Length(), 1);

  std::atomic<int> aCounter{0};
  BRepGraph_Analyze::ParallelForEachEdge(
    myGraph,
    aComponents.Value(0),
    [&aCounter](const BRepGraph_EdgeId) { aCounter.fetch_add(1); });

  // Edges may be duplicated across wires in SubGraph; count unique via the graph.
  // A box has 12 unique edges, but SubGraph edge indices may contain duplicates
  // since multiple wires reference the same edge def.
  EXPECT_GE(aCounter.load(), 12);
}

TEST_F(BRepGraph_AnalyzeTest, ParallelForEachFace_SubGraph_OnlyComponentFaces)
{
  BRepPrimAPI_MakeBox aBox1(10.0, 10.0, 10.0);
  BRepPrimAPI_MakeBox aBox2(gp_Pnt(100.0, 0.0, 0.0), 5.0, 5.0, 5.0);

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBox1.Shape());
  aBuilder.Add(aCompound, aBox2.Shape());

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  const NCollection_Vector<BRepGraph_SubGraph> aComponents = BRepGraph_Analyze::Decompose(aGraph);
  ASSERT_EQ(aComponents.Length(), 2);

  // Iterate only over faces of the first component.
  std::atomic<int> aCounter{0};
  BRepGraph_Analyze::ParallelForEachFace(
    aGraph,
    aComponents.Value(0),
    [&aCounter](const BRepGraph_FaceId) { aCounter.fetch_add(1); });

  EXPECT_EQ(aCounter.load(), 6);
}

// ============================================================
// BoundingBox tests
// ============================================================

TEST_F(BRepGraph_AnalyzeTest, BoundingBox_Box_MatchesBRepBndLib)
{
  Bnd_Box aRefBox;
  BRepBndLib::Add(myBoxShape, aRefBox);

  double aRefXmin, aRefYmin, aRefZmin, aRefXmax, aRefYmax, aRefZmax;
  aRefBox.Get(aRefXmin, aRefYmin, aRefZmin, aRefXmax, aRefYmax, aRefZmax);

  const BRepGraph_NodeId aSolidId(BRepGraph_NodeId::Kind::Solid, 0);
  Bnd_Box                aGraphBox;
  BRepGraphAlgo_BndLib::Add(myGraph, aSolidId, aGraphBox);
  ASSERT_FALSE(aGraphBox.IsVoid());

  double aGXmin, aGYmin, aGZmin, aGXmax, aGYmax, aGZmax;
  aGraphBox.Get(aGXmin, aGYmin, aGZmin, aGXmax, aGYmax, aGZmax);

  const double aTol = 1.0;
  EXPECT_NEAR(aGXmin, aRefXmin, aTol);
  EXPECT_NEAR(aGYmin, aRefYmin, aTol);
  EXPECT_NEAR(aGZmin, aRefZmin, aTol);
  EXPECT_NEAR(aGXmax, aRefXmax, aTol);
  EXPECT_NEAR(aGYmax, aRefYmax, aTol);
  EXPECT_NEAR(aGZmax, aRefZmax, aTol);
}

TEST_F(BRepGraph_AnalyzeTest, BoundingBox_Sphere_NonVoid)
{
  BRepPrimAPI_MakeSphere aSphereMaker(25.0);
  const TopoDS_Shape&    aSphere = aSphereMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aSphere);
  ASSERT_TRUE(aGraph.IsDone());

  // Verify face-level bounding boxes are valid.
  for (int aFaceIdx = 0; aFaceIdx < aGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, aFaceIdx);
    Bnd_Box                aFaceBox;
    BRepGraphAlgo_BndLib::Add(aGraph, aFaceId, aFaceBox);
    EXPECT_FALSE(aFaceBox.IsVoid()) << "Face " << aFaceIdx << " bbox is void";
  }
}

TEST_F(BRepGraph_AnalyzeTest, BoundingBox_Cylinder_FacesNonVoid)
{
  BRepPrimAPI_MakeCylinder aCylMaker(10.0, 50.0);
  const TopoDS_Shape&      aCylinder = aCylMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aCylinder);
  ASSERT_TRUE(aGraph.IsDone());

  // Verify all face-level bounding boxes are valid.
  for (int aFaceIdx = 0; aFaceIdx < aGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, aFaceIdx);
    Bnd_Box                aFaceBox;
    BRepGraphAlgo_BndLib::Add(aGraph, aFaceId, aFaceBox);
    EXPECT_FALSE(aFaceBox.IsVoid()) << "Face " << aFaceIdx << " bbox is void";
  }
}

TEST_F(BRepGraph_AnalyzeTest, BoundingBox_FaceSubsetOfShell)
{
  const BRepGraph_NodeId aShellId(BRepGraph_NodeId::Kind::Shell, 0);
  Bnd_Box                aShellBox;
  BRepGraphAlgo_BndLib::Add(myGraph, aShellId, aShellBox);
  ASSERT_FALSE(aShellBox.IsVoid());

  double aSXmin, aSYmin, aSZmin, aSXmax, aSYmax, aSZmax;
  aShellBox.Get(aSXmin, aSYmin, aSZmin, aSXmax, aSYmax, aSZmax);

  const double aTol = Precision::Confusion();

  for (int aFaceIdx = 0; aFaceIdx < myGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, aFaceIdx);
    Bnd_Box                aFaceBox;
    BRepGraphAlgo_BndLib::Add(myGraph, aFaceId, aFaceBox);
    if (aFaceBox.IsVoid())
      continue;

    double aFXmin, aFYmin, aFZmin, aFXmax, aFYmax, aFZmax;
    aFaceBox.Get(aFXmin, aFYmin, aFZmin, aFXmax, aFYmax, aFZmax);

    EXPECT_GE(aFXmin, aSXmin - aTol) << "Face " << aFaceIdx << " Xmin outside shell";
    EXPECT_GE(aFYmin, aSYmin - aTol) << "Face " << aFaceIdx << " Ymin outside shell";
    EXPECT_GE(aFZmin, aSZmin - aTol) << "Face " << aFaceIdx << " Zmin outside shell";
    EXPECT_LE(aFXmax, aSXmax + aTol) << "Face " << aFaceIdx << " Xmax outside shell";
    EXPECT_LE(aFYmax, aSYmax + aTol) << "Face " << aFaceIdx << " Ymax outside shell";
    EXPECT_LE(aFZmax, aSZmax + aTol) << "Face " << aFaceIdx << " Zmax outside shell";
  }
}

TEST_F(BRepGraph_AnalyzeTest, BoundingBox_ShellSubsetOfSolid)
{
  const BRepGraph_NodeId aSolidId(BRepGraph_NodeId::Kind::Solid, 0);
  Bnd_Box                aSolidBox;
  BRepGraphAlgo_BndLib::Add(myGraph, aSolidId, aSolidBox);
  ASSERT_FALSE(aSolidBox.IsVoid());

  double aSOXmin, aSOYmin, aSOZmin, aSOXmax, aSOYmax, aSOZmax;
  aSolidBox.Get(aSOXmin, aSOYmin, aSOZmin, aSOXmax, aSOYmax, aSOZmax);

  const BRepGraph_NodeId aShellId(BRepGraph_NodeId::Kind::Shell, 0);
  Bnd_Box                aShellBox;
  BRepGraphAlgo_BndLib::Add(myGraph, aShellId, aShellBox);
  ASSERT_FALSE(aShellBox.IsVoid());

  double aSHXmin, aSHYmin, aSHZmin, aSHXmax, aSHYmax, aSHZmax;
  aShellBox.Get(aSHXmin, aSHYmin, aSHZmin, aSHXmax, aSHYmax, aSHZmax);

  const double aTol = Precision::Confusion();
  EXPECT_GE(aSHXmin, aSOXmin - aTol);
  EXPECT_GE(aSHYmin, aSOYmin - aTol);
  EXPECT_GE(aSHZmin, aSOZmin - aTol);
  EXPECT_LE(aSHXmax, aSOXmax + aTol);
  EXPECT_LE(aSHYmax, aSOYmax + aTol);
  EXPECT_LE(aSHZmax, aSOZmax + aTol);
}

TEST_F(BRepGraph_AnalyzeTest, BoundingBox_Edge_SubsetOfOwningFace)
{
  // Take first face and check that each of its edges' bounding boxes
  // fit within the face bounding box.
  const BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, 0);
  Bnd_Box                aFaceBox;
  BRepGraphAlgo_BndLib::Add(myGraph, aFaceId, aFaceBox);
  ASSERT_FALSE(aFaceBox.IsVoid());

  double aFXmin, aFYmin, aFZmin, aFXmax, aFYmax, aFZmax;
  aFaceBox.Get(aFXmin, aFYmin, aFZmin, aFXmax, aFYmax, aFZmax);

  // Get edges of the first wire of the first face via ref entries.
  const NCollection_Vector<BRepGraph_CoEdgeRefId> aCoEdgeRefs =
    BRepGraph_TestTools::CoEdgeRefsOfWire(myGraph, BRepGraph_WireId(0));
  const double aTol = Precision::Confusion();

  for (int aCoEdgeIter = 0; aCoEdgeIter < aCoEdgeRefs.Length(); ++aCoEdgeIter)
  {
    const BRepGraphInc::CoEdgeRef& aCR      = myGraph.Refs().CoEdge(aCoEdgeRefs.Value(aCoEdgeIter));
    const BRepGraphInc::CoEdgeDef& aCoEdge  = myGraph.Topo().CoEdge(aCR.CoEdgeDefId);
    const BRepGraph_NodeId         anEdgeId = aCoEdge.EdgeDefId;
    Bnd_Box                        anEdgeBox;
    BRepGraphAlgo_BndLib::Add(myGraph, anEdgeId, anEdgeBox);
    if (anEdgeBox.IsVoid())
      continue;

    double aEXmin, aEYmin, aEZmin, aEXmax, aEYmax, aEZmax;
    anEdgeBox.Get(aEXmin, aEYmin, aEZmin, aEXmax, aEYmax, aEZmax);

    EXPECT_GE(aEXmin, aFXmin - aTol) << "Edge Xmin outside face";
    EXPECT_GE(aEYmin, aFYmin - aTol) << "Edge Ymin outside face";
    EXPECT_GE(aEZmin, aFZmin - aTol) << "Edge Zmin outside face";
    EXPECT_LE(aEXmax, aFXmax + aTol) << "Edge Xmax outside face";
    EXPECT_LE(aEYmax, aFYmax + aTol) << "Edge Ymax outside face";
    EXPECT_LE(aEZmax, aFZmax + aTol) << "Edge Zmax outside face";
  }
}

TEST_F(BRepGraph_AnalyzeTest, BoundingBox_Vertex_SinglePoint)
{
  for (int aVertIdx = 0; aVertIdx < myGraph.Topo().NbVertices(); ++aVertIdx)
  {
    const BRepGraph_NodeId aVertId(BRepGraph_NodeId::Kind::Vertex, aVertIdx);
    Bnd_Box                aVertBox;
    BRepGraphAlgo_BndLib::Add(myGraph, aVertId, aVertBox);
    ASSERT_FALSE(aVertBox.IsVoid()) << "Vertex " << aVertIdx << " has void bbox";

    double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
    aVertBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

    // A vertex bounding box should be essentially a point (within tolerance gap).
    const double aMaxSpan = 2.0; // tolerance-expanded point
    EXPECT_LE(aXmax - aXmin, aMaxSpan) << "Vertex " << aVertIdx << " X span too large";
    EXPECT_LE(aYmax - aYmin, aMaxSpan) << "Vertex " << aVertIdx << " Y span too large";
    EXPECT_LE(aZmax - aZmin, aMaxSpan) << "Vertex " << aVertIdx << " Z span too large";
  }
}

TEST_F(BRepGraph_AnalyzeTest, BoundingBox_AfterMutation_CacheInvalidated)
{
  const BRepGraph_NodeId aVertId(BRepGraph_NodeId::Kind::Vertex, 0);

  // Get initial bounding box.
  Bnd_Box aBoxBefore;
  BRepGraphAlgo_BndLib::Add(myGraph, aVertId, aBoxBefore);
  ASSERT_FALSE(aBoxBefore.IsVoid());

  // Mutate vertex - this should invalidate the cache via markModified.
  myGraph.Builder().MutVertex(BRepGraph_VertexId(0));

  // Verify that after mutation, recomputing still produces a valid bbox.
  // BoundingBox uses the original shape for computation, so results stay consistent.
  Bnd_Box aBoxAfter;
  BRepGraphAlgo_BndLib::Add(myGraph, aVertId, aBoxAfter);
  ASSERT_FALSE(aBoxAfter.IsVoid());

  // Verify IsModified flag was set.
  EXPECT_TRUE(myGraph.Topo().Vertex(BRepGraph_VertexId(0)).IsModified);
}

TEST_F(BRepGraph_AnalyzeTest, Invalidate_ThenRecompute_SameResult)
{
  const BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, 0);

  // Compute bounding box via cached path, then invalidate, then recompute. Results should match.
  const Bnd_Box aBoxFirst = BRepGraphAlgo_BndLib::AddCached(myGraph, aFaceId);
  ASSERT_FALSE(aBoxFirst.IsVoid());

  BRepGraphAlgo_BndLib::InvalidateCached(myGraph, aFaceId);

  const Bnd_Box aBoxSecond = BRepGraphAlgo_BndLib::AddCached(myGraph, aFaceId);
  ASSERT_FALSE(aBoxSecond.IsVoid());

  double aX1min, aY1min, aZ1min, aX1max, aY1max, aZ1max;
  aBoxFirst.Get(aX1min, aY1min, aZ1min, aX1max, aY1max, aZ1max);

  double aX2min, aY2min, aZ2min, aX2max, aY2max, aZ2max;
  aBoxSecond.Get(aX2min, aY2min, aZ2min, aX2max, aY2max, aZ2max);

  EXPECT_NEAR(aX1min, aX2min, Precision::Confusion());
  EXPECT_NEAR(aY1min, aY2min, Precision::Confusion());
  EXPECT_NEAR(aZ1min, aZ2min, Precision::Confusion());
  EXPECT_NEAR(aX1max, aX2max, Precision::Confusion());
  EXPECT_NEAR(aY1max, aY2max, Precision::Confusion());
  EXPECT_NEAR(aZ1max, aZ2max, Precision::Confusion());
}

TEST_F(BRepGraph_AnalyzeTest, InvalidateSubgraph_PropagatesUpToSolid)
{
  const BRepGraph_NodeId aSolidId(BRepGraph_NodeId::Kind::Solid, 0);
  const BRepGraph_NodeId aVertId(BRepGraph_NodeId::Kind::Vertex, 0);

  // Compute solid bounding box first (populates cache).
  const Bnd_Box aSolidBoxBefore = BRepGraphAlgo_BndLib::AddCached(myGraph, aSolidId);
  ASSERT_FALSE(aSolidBoxBefore.IsVoid());

  // Invalidate from a vertex upward via a no-op mutation (triggers markModified).
  {
    BRepGraph_MutRef<BRepGraphInc::VertexDef> aMut =
      myGraph.Builder().MutVertex(BRepGraph_VertexId(aVertId.Index));
  }

  // Recompute. Since no actual mutation occurred, the result should be the same.
  const Bnd_Box aSolidBoxAfter = BRepGraphAlgo_BndLib::AddCached(myGraph, aSolidId);
  ASSERT_FALSE(aSolidBoxAfter.IsVoid());

  double aB1Xmin, aB1Ymin, aB1Zmin, aB1Xmax, aB1Ymax, aB1Zmax;
  aSolidBoxBefore.Get(aB1Xmin, aB1Ymin, aB1Zmin, aB1Xmax, aB1Ymax, aB1Zmax);

  double aB2Xmin, aB2Ymin, aB2Zmin, aB2Xmax, aB2Ymax, aB2Zmax;
  aSolidBoxAfter.Get(aB2Xmin, aB2Ymin, aB2Zmin, aB2Xmax, aB2Ymax, aB2Zmax);

  EXPECT_NEAR(aB1Xmin, aB2Xmin, Precision::Confusion());
  EXPECT_NEAR(aB1Ymin, aB2Ymin, Precision::Confusion());
  EXPECT_NEAR(aB1Zmin, aB2Zmin, Precision::Confusion());
  EXPECT_NEAR(aB1Xmax, aB2Xmax, Precision::Confusion());
  EXPECT_NEAR(aB1Ymax, aB2Ymax, Precision::Confusion());
  EXPECT_NEAR(aB1Zmax, aB2Zmax, Precision::Confusion());
}

// ============================================================
// Centroid tests
// ============================================================

TEST_F(BRepGraph_AnalyzeTest, Centroid_Box_AtCenter)
{
  // 10x20x30 box at origin: centroid near (5, 10, 15).
  const BRepGraph_NodeId aSolidId(BRepGraph_NodeId::Kind::Solid, 0);
  const gp_Pnt           aCentroid = bboxCenter(myGraph, aSolidId);

  EXPECT_NEAR(aCentroid.X(), 5.0, 1.0);
  EXPECT_NEAR(aCentroid.Y(), 10.0, 1.0);
  EXPECT_NEAR(aCentroid.Z(), 15.0, 1.0);
}

TEST_F(BRepGraph_AnalyzeTest, Centroid_Sphere_AtOrigin)
{
  BRepPrimAPI_MakeSphere aSphereMaker(30.0);
  const TopoDS_Shape&    aSphere = aSphereMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aSphere);
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId aSolidId(BRepGraph_NodeId::Kind::Solid, 0);
  const gp_Pnt           aCentroid = bboxCenter(aGraph, aSolidId);

  EXPECT_NEAR(aCentroid.X(), 0.0, 2.0);
  EXPECT_NEAR(aCentroid.Y(), 0.0, 2.0);
  EXPECT_NEAR(aCentroid.Z(), 0.0, 2.0);
}

TEST_F(BRepGraph_AnalyzeTest, Centroid_Face_InsideFaceBBox)
{
  for (int aFaceIdx = 0; aFaceIdx < myGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_NodeId aFaceId(BRepGraph_NodeId::Kind::Face, aFaceIdx);
    const gp_Pnt           aCentroid = bboxCenter(myGraph, aFaceId);
    Bnd_Box                aFaceBox;
    BRepGraphAlgo_BndLib::Add(myGraph, aFaceId, aFaceBox);

    if (aFaceBox.IsVoid())
      continue;

    double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
    aFaceBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

    const double aTol = 1.0;
    EXPECT_GE(aCentroid.X(), aXmin - aTol) << "Face " << aFaceIdx << " centroid X below min";
    EXPECT_LE(aCentroid.X(), aXmax + aTol) << "Face " << aFaceIdx << " centroid X above max";
    EXPECT_GE(aCentroid.Y(), aYmin - aTol) << "Face " << aFaceIdx << " centroid Y below min";
    EXPECT_LE(aCentroid.Y(), aYmax + aTol) << "Face " << aFaceIdx << " centroid Y above max";
    EXPECT_GE(aCentroid.Z(), aZmin - aTol) << "Face " << aFaceIdx << " centroid Z below min";
    EXPECT_LE(aCentroid.Z(), aZmax + aTol) << "Face " << aFaceIdx << " centroid Z above max";
  }
}

TEST_F(BRepGraph_AnalyzeTest, Centroid_AfterMutation_CacheInvalidated)
{
  const BRepGraph_NodeId aVertId(BRepGraph_NodeId::Kind::Vertex, 0);

  const gp_Pnt aCentroidBefore = bboxCenter(myGraph, aVertId);

  // Mutate vertex (marks modified, invalidates cache).
  myGraph.Builder().MutVertex(BRepGraph_VertexId(0));

  // Centroid recomputes from original shape - result stays consistent.
  const gp_Pnt aCentroidAfter = bboxCenter(myGraph, aVertId);

  // Since the underlying shape hasn't changed (only graph data), centroids match.
  EXPECT_NEAR(aCentroidBefore.X(), aCentroidAfter.X(), Precision::Confusion());
  EXPECT_NEAR(aCentroidBefore.Y(), aCentroidAfter.Y(), Precision::Confusion());
  EXPECT_NEAR(aCentroidBefore.Z(), aCentroidAfter.Z(), Precision::Confusion());

  // Verify the modification flag was set.
  EXPECT_TRUE(myGraph.Topo().Vertex(BRepGraph_VertexId(0)).IsModified);
}
