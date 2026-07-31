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

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_CacheDerivedState.hxx>
#include <BRepGraph_CacheRegistry.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_RepId.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_ParentExplorer.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_MeshView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Plane.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_Map.hxx>
#include <Precision.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>

#include <tuple>

#include <gtest/gtest.h>

namespace
{

bool edgeSameParameter(const BRepGraph& theGraph, BRepGraph_EdgeId theEdge)
{
  const auto& aRel = theGraph.Topo().Edges().Relations(theEdge);
  for (const auto& aCoEdgeId : aRel.CoEdgeIds)
  {
    if (!BRepGraph_Tool::CoEdge::SameParameter(theGraph, aCoEdgeId))
    {
      return false;
    }
  }
  return true;
}

bool edgeSameRange(const BRepGraph& theGraph, BRepGraph_EdgeId theEdge)
{
  const auto& aRel = theGraph.Topo().Edges().Relations(theEdge);
  for (const auto& aCoEdgeId : aRel.CoEdgeIds)
  {
    if (!BRepGraph_Tool::CoEdge::SameRange(theGraph, aCoEdgeId))
    {
      return false;
    }
  }
  return true;
}

static int componentKey(const BRepGraph_NodeId theNode)
{
  return theNode.Index * BRepGraph_NodeId::THE_KIND_COUNT + static_cast<int>(theNode.NodeKind);
}

static BRepGraph_NodeId componentRootOfFace(const BRepGraph&       theGraph,
                                            const BRepGraph_FaceId theFaceId)
{
  for (BRepGraph_ParentExplorer aSolidExp(theGraph, theFaceId, BRepGraph_NodeId::Kind::Solid);
       aSolidExp.More();
       aSolidExp.Next())
  {
    return aSolidExp.Current().DefId;
  }

  for (BRepGraph_ParentExplorer aShellExp(theGraph, theFaceId, BRepGraph_NodeId::Kind::Shell);
       aShellExp.More();
       aShellExp.Next())
  {
    return aShellExp.Current().DefId;
  }

  return theFaceId;
}

static NCollection_DataMap<int, int> faceCountsByComponent(const BRepGraph& theGraph)
{
  NCollection_DataMap<int, int> aCounts;
  for (BRepGraph_FaceIterator aFaceIt(theGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const int aKey = componentKey(componentRootOfFace(theGraph, aFaceIt.CurrentId()));
    if (!aCounts.IsBound(aKey))
    {
      aCounts.Bind(aKey, 0);
    }
    aCounts.ChangeFind(aKey) += 1;
  }
  return aCounts;
}

static NCollection_LinearVector<BRepGraph_FaceId> collectSameDomainFaces(
  const BRepGraph&       theGraph,
  const BRepGraph_FaceId theFace)
{
  NCollection_LinearVector<BRepGraph_FaceId> aFaces;
  const occ::handle<Geom_Surface>& aSurface = BRepGraph_Tool::Face::Surface(theGraph, theFace);
  if (aSurface.IsNull())
  {
    return aFaces;
  }

  for (BRepGraph_FaceIterator aFaceIt(theGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId anOtherFace = aFaceIt.CurrentId();
    if (anOtherFace != theFace && BRepGraph_Tool::Face::Surface(theGraph, anOtherFace) == aSurface)
    {
      aFaces.Append(anOtherFace);
    }
  }
  return aFaces;
}

} // namespace

// ============================================================
// Geometry navigation tests
// ============================================================

TEST(BRepGraph_GeometryTest, Sphere_AllFaces_SameSurface)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes1 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeSphere(15.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  // All face defs of a sphere share the same surface handle.
  ASSERT_GE(aGraph.Topo().Faces().Nb(), 1);
  ASSERT_TRUE(BRepGraph_Tool::Face::HasSurface(aGraph, BRepGraph_FaceId::Start()));
  const occ::handle<Geom_Surface>& aFirstSurf =
    BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId::Start());
  EXPECT_FALSE(aFirstSurf.IsNull());
  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId aFaceId = aFaceIt.CurrentId();
    if (aFaceId.Index == 0)
    {
      continue;
    }
    ASSERT_TRUE(BRepGraph_Tool::Face::HasSurface(aGraph, aFaceId));
    EXPECT_EQ(BRepGraph_Tool::Face::Surface(aGraph, aFaceId).get(), aFirstSurf.get());
  }
}

TEST(BRepGraph_GeometryTest, Sphere_AllFacesShareSurface)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes2 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeSphere(15.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  // All faces of a sphere share the same surface pointer.
  ASSERT_TRUE(BRepGraph_Tool::Face::HasSurface(aGraph, BRepGraph_FaceId::Start()));
  const occ::handle<Geom_Surface>& aFirstSurf =
    BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId::Start());
  EXPECT_FALSE(aFirstSurf.IsNull());
  int aSameCount = 0;
  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId aFaceId = aFaceIt.CurrentId();
    if (BRepGraph_Tool::Face::HasSurface(aGraph, aFaceId)
        && BRepGraph_Tool::Face::Surface(aGraph, aFaceId).get() == aFirstSurf.get())
    {
      ++aSameCount;
    }
  }
  EXPECT_EQ(aSameCount, aGraph.Topo().Faces().Nb());
}

TEST(BRepGraph_GeometryTest, Box_Curve3d_ValidForAll12Edges)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes3 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), 12);

  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId anEdgeId = anEdgeIt.CurrentId();
    EXPECT_TRUE(BRepGraph_Tool::Edge::HasCurve(aGraph, anEdgeId))
      << "Edge def " << anEdgeId.Index << " has no valid curve";
  }
}

TEST(BRepGraph_GeometryTest, Box_AllEdgesHaveCurve3d)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes4 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId anEdgeId = anEdgeIt.CurrentId();
    EXPECT_TRUE(BRepGraph_Tool::Edge::HasCurve(aGraph, anEdgeId))
      << "Edge " << anEdgeId.Index << " has no curve";
  }
}

TEST(BRepGraph_GeometryTest, Box_FindPCurveCoEdgeId_AllEdgeFacePairs_Valid)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes5 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  int aPCurveCount = 0;
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId                              anEdgeId = anEdgeIt.CurrentId();
    const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Topo().Edges().CoEdges(anEdgeId);
    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdgeIdxs)
    {
      const BRepGraphInc::CoEdgeDef& aCE = aGraph.Topo().CoEdges().Definition(aCoEdgeId);
      const BRepGraph_CoEdgeId       aPCurveId =
        BRepGraph_Tool::Edge::FindPCurveCoEdgeId(aGraph, anEdgeId, BRepGraph_FaceId(aCE.FaceId));
      EXPECT_TRUE(aPCurveId.IsValid());
      ++aPCurveCount;
    }
  }
  EXPECT_GT(aPCurveCount, 0);
}

TEST(BRepGraph_GeometryTest, CoEdge_FaceIdValid)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes6 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Topo().Edges().CoEdges(anEdgeIt.CurrentId());
    for (size_t j = 0; j < aCoEdgeIdxs.Size(); ++j)
    {
      const BRepGraphInc::CoEdgeDef& aCE = aGraph.Topo().CoEdges().Definition(aCoEdgeIdxs.Value(j));
      EXPECT_TRUE(aCE.FaceId.IsValid())
        << "Edge " << anEdgeIt.CurrentId().Index << " CoEdge " << j << " has invalid FaceId";
      EXPECT_EQ(BRepGraph_NodeId(aCE.FaceId).NodeKind, BRepGraph_NodeId::Kind::Face);
    }
  }
}

TEST(BRepGraph_GeometryTest, CoEdge_ParamRange_NonZero)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes7 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  int aCoEdgeCount = 0;
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Topo().Edges().CoEdges(anEdgeIt.CurrentId());
    for (size_t j = 0; j < aCoEdgeIdxs.Size(); ++j)
    {
      const std::pair<double, double> aRange =
        BRepGraph_Tool::CoEdge::Range(aGraph, aCoEdgeIdxs.Value(j));
      const double aRangeVal = aRange.second - aRange.first;
      EXPECT_GT(std::abs(aRangeVal), Precision::PConfusion())
        << "Edge " << anEdgeIt.CurrentId().Index << " CoEdge " << j << " has zero parameter range";
      ++aCoEdgeCount;
    }
  }
  EXPECT_GT(aCoEdgeCount, 0);
}

TEST(BRepGraph_GeometryTest, SetPCurveTwoArgPreservesExistingRange)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph_CoEdgeId aCoEdgeId;
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdges =
      aGraph.Topo().Edges().CoEdges(anEdgeIt.CurrentId());
    if (!aCoEdges.IsEmpty())
    {
      aCoEdgeId = aCoEdges.First();
      break;
    }
  }
  ASSERT_TRUE(aCoEdgeId.IsValid());
  const std::pair<double, double> aRangeBefore = BRepGraph_Tool::CoEdge::Range(aGraph, aCoEdgeId);

  occ::handle<Geom2d_Curve> aReplacement =
    new Geom2d_Line(gp_Pnt2d(100.0, 200.0), gp_Dir2d(1.0, 0.0));
  aGraph.Editor().CoEdges().SetPCurve(aCoEdgeId, aReplacement);

  const std::pair<double, double> aRangeAfter = BRepGraph_Tool::CoEdge::Range(aGraph, aCoEdgeId);
  EXPECT_NEAR(aRangeAfter.first, aRangeBefore.first, Precision::PConfusion());
  EXPECT_NEAR(aRangeAfter.second, aRangeBefore.second, Precision::PConfusion());
  EXPECT_EQ(BRepGraph_Tool::CoEdge::PCurve(aGraph, aCoEdgeId).get(), aReplacement.get());
}

TEST(BRepGraph_GeometryTest, FaceDef_Surface_IsNotNull)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes9 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    EXPECT_TRUE(BRepGraph_Tool::Face::HasSurface(aGraph, aFaceIt.CurrentId()))
      << "Face " << aFaceIt.CurrentId().Index << " has null Surface handle";
  }
}

TEST(BRepGraph_GeometryTest, EdgeDef_Curve3d_IsNotNull)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes10 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    EXPECT_TRUE(BRepGraph_Tool::Edge::HasCurve(aGraph, anEdgeIt.CurrentId()))
      << "Edge " << anEdgeIt.CurrentId().Index << " has null Curve3d handle";
  }
}

TEST(BRepGraph_GeometryTest, SameDomainFaces_SimpleBox_Empty)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes11 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  // For a simple box each face has a unique surface, so SameDomainFaces is empty.
  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId                           aFaceId = aFaceIt.CurrentId();
    const NCollection_LinearVector<BRepGraph_FaceId> aSameDomain =
      collectSameDomainFaces(aGraph, aFaceId);
    EXPECT_EQ(aSameDomain.Size(), 0)
      << "Face def " << aFaceId.Index << " has unexpected same-domain faces";
  }
}

TEST(BRepGraph_GeometryTest, SameDomainFaces_SharedSurfaceAcrossOwnedUses)
{
  BRepGraph aGraph;
  aGraph.Clear();

  const occ::handle<Geom_Surface>            aSurface = new Geom_Plane(gp_Pln());
  NCollection_LinearVector<BRepGraph_WireId> anInnerWires;
  const BRepGraph_FaceId                     aFaceA =
    aGraph.Editor().Faces().Add(aSurface, BRepGraph_WireId(), anInnerWires.ToArray1(), 1.e-7);
  const BRepGraph_FaceId aFaceB =
    aGraph.Editor().Faces().Add(aSurface, BRepGraph_WireId(), anInnerWires.ToArray1(), 1.e-7);
  ASSERT_TRUE(aFaceA.IsValid());
  ASSERT_TRUE(aFaceB.IsValid());
  ASSERT_NE(aGraph.Topo().Faces().Definition(aFaceA).SurfaceRepId,
            aGraph.Topo().Faces().Definition(aFaceB).SurfaceRepId);

  const NCollection_LinearVector<BRepGraph_FaceId> aSameAsA =
    collectSameDomainFaces(aGraph, aFaceA);
  ASSERT_EQ(aSameAsA.Size(), 1u);
  EXPECT_EQ(aSameAsA.Value(0), aFaceB);

  const NCollection_LinearVector<BRepGraph_FaceId> aSameAsB =
    collectSameDomainFaces(aGraph, aFaceB);
  ASSERT_EQ(aSameAsB.Size(), 1u);
  EXPECT_EQ(aSameAsB.Value(0), aFaceA);
}

TEST(BRepGraph_GeometryTest, CompoundWithMovedChild_SharedSolidDef)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(100.0, 0.0, 0.0));
  const TopoDS_Shape aMoved = aBox.Moved(TopLoc_Location(aTrsf));

  TopoDS_Compound aCompound;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBox);
  aBuilder.Add(aCompound, aMoved);

  BRepGraph aGraph;
  aGraph.Clear();
  const BRepGraph::ShapesView::Result aBuildRes12 = aGraph.Shapes().Add(aCompound);
  ASSERT_TRUE(aBuildRes12.IsOk());
  ASSERT_FALSE(aGraph.IsEmpty());

  // Moved() preserves TShape. The solid definition is shared and the moved
  // usage keeps its placement on the compound child reference.
  EXPECT_EQ(aGraph.Topo().Solids().Nb(), 1);
  const BRepGraph_CompoundId                            aCompoundId(aBuildRes12.TopologyRoot);
  const NCollection_LinearVector<BRepGraph_ChildRefId>& aChildRefs =
    aGraph.Topo().Compounds().Relations(aCompoundId).ChildRefIds;
  ASSERT_EQ(aChildRefs.Size(), 2u);
  EXPECT_TRUE(aGraph.Refs().Gen().LocalLocation(aChildRefs.Value(0)).IsIdentity());
  EXPECT_FALSE(aGraph.Refs().Gen().LocalLocation(aChildRefs.Value(1)).IsIdentity());
}

TEST(BRepGraph_GeometryTest, FaceDef_Triangulation_NullForAnalyticNoCrash)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes13 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  // Analytical box faces should have null triangulation (no mesh computed).
  // Simply verify access does not crash.
  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const bool hasActiveTri = aGraph.Mesh().Effective().Faces().Has(aFaceIt.CurrentId());
    EXPECT_FALSE(hasActiveTri) << "Face " << aFaceIt.CurrentId().Index
                               << " unexpectedly has a triangulation";
  }
}

// ============================================================
// Definition traversal tests
// ============================================================

TEST(BRepGraph_GeometryTest, SolidDef_CountMatchesNb)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes14 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  int aCount = 0;
  for (BRepGraph_SolidIterator aSolidIt(aGraph); aSolidIt.More(); aSolidIt.Next())
  {
    std::ignore = aSolidIt.Current();
    ++aCount;
  }
  EXPECT_EQ(aCount, aGraph.Topo().Solids().Nb());
}

TEST(BRepGraph_GeometryTest, ShellDef_CountMatchesNb)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes15 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  int aCount = 0;
  for (BRepGraph_ShellIterator aShellIt(aGraph); aShellIt.More(); aShellIt.Next())
  {
    std::ignore = aShellIt.Current();
    ++aCount;
  }
  EXPECT_EQ(aCount, aGraph.Topo().Shells().Nb());
}

TEST(BRepGraph_GeometryTest, FaceDef_CountMatchesNb)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes16 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  int aCount = 0;
  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    std::ignore = aFaceIt.Current();
    ++aCount;
  }
  EXPECT_EQ(aCount, aGraph.Topo().Faces().Nb());
}

TEST(BRepGraph_GeometryTest, WireDef_CountMatchesNb)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes17 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  int aCount = 0;
  for (BRepGraph_WireIterator aWireIt(aGraph); aWireIt.More(); aWireIt.Next())
  {
    std::ignore = aWireIt.Current();
    ++aCount;
  }
  EXPECT_EQ(aCount, aGraph.Topo().Wires().Nb());
}

TEST(BRepGraph_GeometryTest, EdgeDef_CountMatchesNb)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes18 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  int aCount = 0;
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    std::ignore = anEdgeIt.Current();
    ++aCount;
  }
  EXPECT_EQ(aCount, aGraph.Topo().Edges().Nb());
}

TEST(BRepGraph_GeometryTest, VertexDef_CountMatchesNb)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes19 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  int aCount = 0;
  for (BRepGraph_VertexIterator aVertexIt(aGraph); aVertexIt.More(); aVertexIt.Next())
  {
    std::ignore = aVertexIt.Current();
    ++aCount;
  }
  EXPECT_EQ(aCount, aGraph.Topo().Vertices().Nb());
}

TEST(BRepGraph_GeometryTest, FaceDef_CountViaIterator_MatchesNb)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes20 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  int aCount = 0;
  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    std::ignore = aFaceIt.Current();
    ++aCount;
  }
  EXPECT_EQ(aCount, aGraph.Topo().Faces().Nb());
}

TEST(BRepGraph_GeometryTest, FaceDef_AllSurfacesNonNull)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes21 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  int aCount = 0;
  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    EXPECT_TRUE(BRepGraph_Tool::Face::HasSurface(aGraph, aFaceIt.CurrentId()));
    ++aCount;
  }
  EXPECT_EQ(aCount, aGraph.Topo().Faces().Nb());
}

TEST(BRepGraph_GeometryTest, EdgeDef_AllCurves3dNonNull)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes22 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  int aCount = 0;
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    if (!BRepGraph_Tool::Edge::Degenerated(aGraph, anEdgeIt.CurrentId()))
    {
      EXPECT_TRUE(BRepGraph_Tool::Edge::HasCurve(aGraph, anEdgeIt.CurrentId()));
    }
    ++aCount;
  }
  EXPECT_EQ(aCount, aGraph.Topo().Edges().Nb());
}

TEST(BRepGraph_GeometryTest, AllCoEdgesHaveCurve2d)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes23 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  int aCount = 0;
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Topo().Edges().CoEdges(anEdgeIt.CurrentId());
    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdgeIdxs)
    {
      EXPECT_TRUE(BRepGraph_Tool::CoEdge::HasPCurve(aGraph, aCoEdgeId));
      ++aCount;
    }
  }
  EXPECT_GT(aCount, 0);
}

TEST(BRepGraph_GeometryTest, CoEdgePCurveAdaptor_FallsBackOnPlaneWhenStoredPCurveRemoved)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes24 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph_CoEdgeId aCoEdgeId;
  BRepGraph_EdgeId   anEdgeId;
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdges =
      aGraph.Topo().Edges().CoEdges(anEdgeIt.CurrentId());
    if (aCoEdges.Size() == 0)
    {
      continue;
    }

    aCoEdgeId = aCoEdges(0);
    anEdgeId  = anEdgeIt.CurrentId();
    break;
  }

  ASSERT_TRUE(aCoEdgeId.IsValid());
  ASSERT_FALSE(BRepGraph_Tool::CoEdge::PCurve(aGraph, aCoEdgeId).IsNull());

  aGraph.Editor().CoEdges().SetPCurve(aCoEdgeId, occ::handle<Geom2d_Curve>());
  EXPECT_TRUE(BRepGraph_Tool::CoEdge::PCurve(aGraph, aCoEdgeId).IsNull());

  const Geom2dAdaptor_Curve aPCurve = BRepGraph_Tool::CoEdge::PCurveAdaptor(aGraph, aCoEdgeId);
  ASSERT_TRUE(aPCurve.IsInitialized());
  EXPECT_FALSE(BRepGraph_Tool::CoEdge::HasPCurve(aGraph, aCoEdgeId));

  const std::pair<double, double> aEdgeRange = BRepGraph_Tool::Edge::Range(aGraph, anEdgeId);
  EXPECT_NEAR(aPCurve.FirstParameter(), aEdgeRange.first, Precision::PConfusion());
  EXPECT_NEAR(aPCurve.LastParameter(), aEdgeRange.second, Precision::PConfusion());
}

// ============================================================
// Connected component grouping via ParentExplorer
// ============================================================

TEST(BRepGraph_GeometryTest, ConnectedComponents_SingleBox_OneComponent)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes25 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const NCollection_DataMap<int, int> aFaceCounts = faceCountsByComponent(aGraph);
  ASSERT_EQ(aFaceCounts.Extent(), 1);
  int aComponentFaces = 0;
  for (const auto& [aComponent, aCount] : aFaceCounts.Items())
  {
    aComponentFaces = aCount;
  }
  EXPECT_EQ(aComponentFaces, 6);
}

TEST(BRepGraph_GeometryTest, ConnectedComponents_TwoBoxCompound_TwoComponents)
{
  const TopoDS_Shape aBox1 = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  const TopoDS_Shape aBox2 =
    BRepPrimAPI_MakeBox(gp_Pnt(50.0, 50.0, 50.0), 60.0, 60.0, 60.0).Shape();

  TopoDS_Compound aCompound;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBox1);
  aBuilder.Add(aCompound, aBox2);

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes26 = aGraph.Shapes().Add(aCompound);
  ASSERT_FALSE(aGraph.IsEmpty());

  const NCollection_DataMap<int, int> aFaceCounts = faceCountsByComponent(aGraph);
  EXPECT_EQ(aFaceCounts.Extent(), 2);
}

TEST(BRepGraph_GeometryTest, ConnectedComponents_TwoBoxCompound_FacesGroupedPerRoot)
{
  const TopoDS_Shape aBox1 = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  const TopoDS_Shape aBox2 =
    BRepPrimAPI_MakeBox(gp_Pnt(50.0, 50.0, 50.0), 60.0, 60.0, 60.0).Shape();

  TopoDS_Compound aCompound;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBox1);
  aBuilder.Add(aCompound, aBox2);

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes27 = aGraph.Shapes().Add(aCompound);
  ASSERT_FALSE(aGraph.IsEmpty());

  const NCollection_DataMap<int, int> aFaceCounts = faceCountsByComponent(aGraph);
  ASSERT_EQ(aFaceCounts.Extent(), 2);
  for (const auto& [aComponent, aCount] : aFaceCounts.Items())
  {
    EXPECT_EQ(aCount, 6);
  }
}

TEST(BRepGraph_GeometryTest, ConnectedComponents_TwoBoxCompound_CoverAllFaces)
{
  const TopoDS_Shape aBox1 = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  const TopoDS_Shape aBox2 =
    BRepPrimAPI_MakeBox(gp_Pnt(50.0, 50.0, 50.0), 60.0, 60.0, 60.0).Shape();

  TopoDS_Compound aCompound;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBox1);
  aBuilder.Add(aCompound, aBox2);

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes28 = aGraph.Shapes().Add(aCompound);
  ASSERT_FALSE(aGraph.IsEmpty());

  const NCollection_DataMap<int, int> aFaceCounts = faceCountsByComponent(aGraph);
  int                                 aTotalFaces = 0;
  for (const auto& [aComponent, aCount] : aFaceCounts.Items())
  {
    aTotalFaces += aCount;
  }

  EXPECT_EQ(aTotalFaces, aGraph.Topo().Faces().Nb());
}

// ============================================================
// SameParameter / SameRange round-trip tests
// ============================================================

TEST(BRepGraph_GeometryTest, Box_EdgeDef_SameParameter_IsSet)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes29 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GT(aGraph.Topo().Edges().Nb(), 0);

  // Box edges are well-formed; SameParameter should be true for all.
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    EXPECT_TRUE(edgeSameParameter(aGraph, anEdgeIt.CurrentId()))
      << "Edge def " << anEdgeIt.CurrentId().Index << " has SameParameter=false";
  }
}

TEST(BRepGraph_GeometryTest, Box_EdgeDef_SameRange_IsSet)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes30 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GT(aGraph.Topo().Edges().Nb(), 0);

  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    EXPECT_TRUE(edgeSameRange(aGraph, anEdgeIt.CurrentId()))
      << "Edge def " << anEdgeIt.CurrentId().Index << " has SameRange=false";
  }
}

TEST(BRepGraph_GeometryTest, DerivedStateCache_LazyAndFreshAfterPCurveRangeMutation)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes31 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());
  EXPECT_TRUE(aGraph.CacheRegistry().Find<BRepGraph_CacheDerivedState>().IsNull());

  BRepGraph_EdgeId   anEdgeId;
  BRepGraph_CoEdgeId aCoEdgeId;
  for (BRepGraph_CoEdgeIterator aCoEdgeIt(aGraph); aCoEdgeIt.More(); aCoEdgeIt.Next())
  {
    const BRepGraph_CoEdgeId aCandidateCoEdge = aCoEdgeIt.CurrentId();
    const BRepGraph_EdgeId   aCandidateEdge =
      aGraph.Topo().CoEdges().Definition(aCandidateCoEdge).ChildEdgeId;
    if (aCandidateEdge.IsValid(aGraph.Topo().Edges().Nb()) && !aCandidateEdge.IsRemoved(aGraph)
        && !BRepGraph_Tool::Edge::Curve(aGraph, aCandidateEdge).IsNull()
        && !BRepGraph_Tool::CoEdge::PCurve(aGraph, aCandidateCoEdge).IsNull())
    {
      anEdgeId  = aCandidateEdge;
      aCoEdgeId = aCandidateCoEdge;
      break;
    }
  }
  ASSERT_TRUE(anEdgeId.IsValid(aGraph.Topo().Edges().Nb()));
  ASSERT_TRUE(aCoEdgeId.IsValid(aGraph.Topo().CoEdges().Nb()));

  ASSERT_TRUE(edgeSameRange(aGraph, anEdgeId));
  EXPECT_FALSE(aGraph.CacheRegistry().Find<BRepGraph_CacheDerivedState>().IsNull());

  const std::pair<double, double> anEdgeRange = BRepGraph_Tool::Edge::Range(aGraph, anEdgeId);
  aGraph.Editor().CoEdges().SetParamRange(aCoEdgeId, anEdgeRange.first + 1.0, anEdgeRange.second);

  EXPECT_FALSE(edgeSameRange(aGraph, anEdgeId));
  EXPECT_FALSE(edgeSameParameter(aGraph, anEdgeId));
}

// ============================================================
// Seam edge PCurve validation tests
// ============================================================

TEST(BRepGraph_GeometryTest, Cylinder_SeamEdge_HasTwoCoEdges)
{
  // A cylinder has a seam edge on its lateral face.
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes32 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeCylinder(5.0, 20.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  // Find a seam edge via the connectivity-derived BRepGraph_Tool::CoEdge::SeamPair query.
  bool aFoundSeam = false;
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More() && !aFoundSeam; anEdgeIt.Next())
  {
    const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Topo().Edges().CoEdges(anEdgeIt.CurrentId());
    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdgeIdxs)
    {
      const BRepGraph_CoEdgeId aPairId = BRepGraph_Tool::CoEdge::SeamPair(aGraph, aCoEdgeId);
      if (aPairId.IsValid())
      {
        const BRepGraphInc::CoEdgeDef& aCE   = aGraph.Topo().CoEdges().Definition(aCoEdgeId);
        const BRepGraphInc::CoEdgeDef& aPair = aGraph.Topo().CoEdges().Definition(aPairId);
        EXPECT_NE(aCE.Orientation, aPair.Orientation)
          << "Seam coedges should have opposite orientations";
        EXPECT_EQ(aCE.FaceId, aPair.FaceId) << "Seam coedges should share the same face";
        aFoundSeam = true;
        break;
      }
    }
  }
  EXPECT_TRUE(aFoundSeam) << "No seam edge found in cylinder";
}

TEST(BRepGraph_GeometryTest, Cylinder_SeamEdge_FindPCurveCoEdgeId_WithOrientation)
{
  // Verify FindPCurveCoEdgeId(edge, face, orientation) returns different entries for FORWARD vs
  // REVERSED.
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes32 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeCylinder(5.0, 20.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId                              anEdgeId = anEdgeIt.CurrentId();
    const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Topo().Edges().CoEdges(anEdgeId);

    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdgeIdxs)
    {
      const BRepGraphInc::CoEdgeDef& aCE = aGraph.Topo().CoEdges().Definition(aCoEdgeId);
      if (!BRepGraph_Tool::CoEdge::IsSeam(aGraph, aCoEdgeId))
      {
        continue;
      }

      // This is a seam edge - test oriented overload.
      const BRepGraph_FaceId   aFaceId(aCE.FaceId.Index);
      const BRepGraph_CoEdgeId aPC_Fwd =
        BRepGraph_Tool::Edge::FindPCurveCoEdgeId(aGraph, anEdgeId, aFaceId, TopAbs_FORWARD);
      const BRepGraph_CoEdgeId aPC_Rev =
        BRepGraph_Tool::Edge::FindPCurveCoEdgeId(aGraph, anEdgeId, aFaceId, TopAbs_REVERSED);

      EXPECT_TRUE(aPC_Fwd.IsValid()) << "FindPCurveCoEdgeId FORWARD returned invalid for seam edge";
      EXPECT_TRUE(aPC_Rev.IsValid())
        << "FindPCurveCoEdgeId REVERSED returned invalid for seam edge";
      EXPECT_NE(aPC_Fwd, aPC_Rev) << "FORWARD and REVERSED PCurves should be different entries";
      return; // one seam is enough
    }
  }
  GTEST_SKIP() << "No seam edge found; test inconclusive";
}

TEST(BRepGraph_GeometryTest, Box_FindPCurveCoEdgeId_MatchesCoEdge)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes33 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId                              anEdgeId = anEdgeIt.CurrentId();
    const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Topo().Edges().CoEdges(anEdgeId);

    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdgeIdxs)
    {
      const BRepGraphInc::CoEdgeDef& aCE = aGraph.Topo().CoEdges().Definition(aCoEdgeId);
      const BRepGraph_CoEdgeId       aFoundId =
        BRepGraph_Tool::Edge::FindPCurveCoEdgeId(aGraph, anEdgeId, aCE.FaceId, aCE.Orientation);

      EXPECT_EQ(aFoundId, aCoEdgeId);
    }
  }
}

TEST(BRepGraph_GeometryTest, Cylinder_SeamEdge_FindPCurveCoEdgeId_DistinguishesOrientation)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes34 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeCylinder(5.0, 20.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Topo().Edges().CoEdges(anEdgeIt.CurrentId());

    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdgeIdxs)
    {
      const BRepGraphInc::CoEdgeDef& aCE = aGraph.Topo().CoEdges().Definition(aCoEdgeId);
      if (!BRepGraph_Tool::CoEdge::IsSeam(aGraph, aCoEdgeId))
      {
        continue;
      }

      // Seam edge: same face, two orientations.
      const BRepGraph_FaceId aFaceId  = aCE.FaceId;
      const BRepGraph_EdgeId anEdgeId = anEdgeIt.CurrentId();

      const BRepGraph_CoEdgeId aPCFwd =
        BRepGraph_Tool::Edge::FindPCurveCoEdgeId(aGraph, anEdgeId, aFaceId, TopAbs_FORWARD);
      const BRepGraph_CoEdgeId aPCRev =
        BRepGraph_Tool::Edge::FindPCurveCoEdgeId(aGraph, anEdgeId, aFaceId, TopAbs_REVERSED);

      EXPECT_TRUE(aPCFwd.IsValid());
      EXPECT_TRUE(aPCRev.IsValid());
      EXPECT_NE(aPCFwd, aPCRev);
      return;
    }
  }
  GTEST_SKIP() << "No seam edge found; test inconclusive";
}

// ============================================================
// Representation entity layer tests
// ============================================================

TEST(BRepGraph_GeometryTest, Box_RepCounts_MatchTopology)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes35 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  EXPECT_GT(aGraph.Topo().Geometry().NbFaceSurfaces(), 0);
  EXPECT_GT(aGraph.Topo().Geometry().NbEdgeCurves3D(), 0);
  EXPECT_GT(aGraph.Topo().Geometry().NbCoEdgeCurves2D(), 0);

  // Every face has a valid surface.
  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    EXPECT_TRUE(BRepGraph_Tool::Face::HasSurface(aGraph, aFaceIt.CurrentId()))
      << "Face " << aFaceIt.CurrentId().Index << " has no surface";
    EXPECT_FALSE(BRepGraph_Tool::Face::Surface(aGraph, aFaceIt.CurrentId()).IsNull());
  }

  // Every non-degenerate edge has a valid 3D curve.
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    if (!BRepGraph_Tool::Edge::Degenerated(aGraph, anEdgeIt.CurrentId())
        && BRepGraph_Tool::Edge::HasCurve(aGraph, anEdgeIt.CurrentId()))
    {
      EXPECT_FALSE(BRepGraph_Tool::Edge::Curve(aGraph, anEdgeIt.CurrentId()).IsNull());
    }
  }

  // Every coedge with a PCurve has a valid Curve2DRepId.
  for (BRepGraph_CoEdgeIterator aCoEdgeIt(aGraph); aCoEdgeIt.More(); aCoEdgeIt.Next())
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = aCoEdgeIt.Current();
    if (aCoEdge.Curve2DRepId.IsValid())
    {
      const occ::handle<Geom2d_Curve>& aPCurve =
        BRepGraph_Tool::CoEdge::PCurve(aGraph, aCoEdgeIt.CurrentId());
      EXPECT_FALSE(aPCurve.IsNull());
    }
  }
}

TEST(BRepGraph_GeometryTest, Sphere_SurfaceDedup_SharedHandle)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes36 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeSphere(15.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  // All faces of a sphere share the same TShape -> same entity -> same surface.
  if (aGraph.Topo().Faces().Nb() > 1)
  {
    const Geom_Surface* aFirstSurfPtr =
      BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId::Start()).get();
    for (BRepGraph_FaceId aFaceId(1); aFaceId.IsValid(aGraph.Topo().Faces().Nb()); ++aFaceId)
    {
      EXPECT_EQ(BRepGraph_Tool::Face::Surface(aGraph, aFaceId).get(), aFirstSurfPtr)
        << "Faces sharing same surface should share the same surface pointer";
    }
  }
}

TEST(BRepGraph_GeometryTest, Cylinder_TriangulationReps_Populated)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes37 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeCylinder(5.0, 10.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraphInc::FaceDef& aFace = aFaceIt.Current();
    if (aFace.TriangulationRepId.IsValid())
    {
      EXPECT_TRUE(aFace.TriangulationRepId.IsValid());
      // Verify active triangulation is non-null via BRepGraph_Tool.
      if (aGraph.Mesh().Effective().Faces().Has(aFaceIt.CurrentId()))
      {
        EXPECT_FALSE(aGraph.Mesh().Effective().Faces().Triangulation(aFaceIt.CurrentId()).IsNull());
      }
    }
  }
}

TEST(BRepGraph_GeometryTest, RepId_FactoryMethods)
{
  const BRepGraph_FaceSurfaceRepId aSurfId(42);
  EXPECT_EQ(BRepGraph_RepId(aSurfId).RepKind, BRepGraph_RepId::Kind::FaceSurface);
  EXPECT_EQ(aSurfId.Index, 42u);
  EXPECT_TRUE(aSurfId.IsValid());

  const BRepGraph_EdgeCurve3DRepId aCurve3DId(7);
  EXPECT_EQ(BRepGraph_RepId(aCurve3DId).RepKind, BRepGraph_RepId::Kind::EdgeCurve3D);
  EXPECT_EQ(aCurve3DId.Index, 7u);

  const BRepGraph_RepId aDefaultId;
  EXPECT_FALSE(aDefaultId.IsValid());

  EXPECT_EQ(aSurfId, BRepGraph_FaceSurfaceRepId(42));
  EXPECT_NE(BRepGraph_RepId(aSurfId), BRepGraph_RepId(aCurve3DId));
}

TEST(BRepGraph_GeometryTest, RepId_UntypedArithmetic_PreservesKindAndIndex)
{
  const BRepGraph_RepId aBase(BRepGraph_RepId::Kind::EdgeCurve3D, 5);
  EXPECT_EQ(aBase.RepKind, BRepGraph_RepId::Kind::EdgeCurve3D);
  EXPECT_EQ(aBase.Index, 5u);

  const BRepGraph_RepId aSame(BRepGraph_RepId::Kind::EdgeCurve3D, 5);
  EXPECT_EQ(aBase, aSame);

  const BRepGraph_RepId aDifferent(BRepGraph_RepId::Kind::EdgeCurve3D, 9);
  EXPECT_NE(aBase, aDifferent);
  EXPECT_LT(aBase, aDifferent);

  const BRepGraph_RepId aOtherKind(BRepGraph_RepId::Kind::FaceSurface, 5);
  EXPECT_NE(aBase, aOtherKind);
}

TEST(BRepGraph_GeometryTest, Compound_TwoBoxes_SurfaceDedup)
{
  TopoDS_Shape aBox1 = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  TopoDS_Shape aBox2 = BRepPrimAPI_MakeBox(20.0, 20.0, 20.0).Shape();

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBox1);
  aBuilder.Add(aCompound, aBox2);

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes38 = aGraph.Shapes().Add(aCompound);
  ASSERT_FALSE(aGraph.IsEmpty());

  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 12);
  EXPECT_GT(aGraph.Topo().Geometry().NbFaceSurfaces(), 0);
  EXPECT_LE(aGraph.Topo().Geometry().NbFaceSurfaces(), 12);

  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    EXPECT_TRUE(BRepGraph_Tool::Face::HasSurface(aGraph, aFaceIt.CurrentId()));
  }
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    if (!BRepGraph_Tool::Edge::Degenerated(aGraph, anEdgeIt.CurrentId())
        && BRepGraph_Tool::Edge::HasCurve(aGraph, anEdgeIt.CurrentId()))
    {
      EXPECT_FALSE(BRepGraph_Tool::Edge::Curve(aGraph, anEdgeIt.CurrentId()).IsNull());
    }
  }
}

TEST(BRepGraph_GeometryTest, Box_Polygon2DRep_MatchesInline)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes39 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  // Every coedge with a Polygon2DRepId has a valid polygon rep.
  for (BRepGraph_CoEdgeIterator aCoEdgeIt(aGraph); aCoEdgeIt.More(); aCoEdgeIt.Next())
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = aCoEdgeIt.Current();
    if (aCoEdge.Polygon2DRepId.IsValid())
    {
      const occ::handle<Poly_Polygon2D>& aPoly =
        aGraph.Mesh().Persistent().CoEdges().PolygonOnSurface(aCoEdgeIt.CurrentId());
      EXPECT_FALSE(aPoly.IsNull())
        << "CoEdge " << aCoEdgeIt.CurrentId().Index << " has Polygon2DRepId but null polygon";
    }
    // PolygonOnTriRepId should have valid rep entry if set.
    if (aCoEdge.PolygonOnTriRepId.IsValid())
    {
      EXPECT_TRUE(aCoEdge.PolygonOnTriRepId.IsValid());
    }
  }
}

TEST(BRepGraph_GeometryTest, ClearUseOwnersMarksRecordsRemovedAndSetReusesSlots)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes40 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_FaceId aFaceId = BRepGraph_FaceId::Start();
  ASSERT_TRUE(aFaceId.IsValid(aGraph.Topo().Faces().Nb()));
  const BRepGraph_FaceSurfaceRepId aSurfaceRepId =
    aGraph.Topo().Faces().Definition(aFaceId).SurfaceRepId;
  ASSERT_TRUE(aSurfaceRepId.IsValid());
  const occ::handle<Geom_Surface> aSurface = BRepGraph_Tool::Face::Surface(aGraph, aFaceId);
  ASSERT_FALSE(aSurface.IsNull());

  BRepGraph_EdgeId anEdgeId;
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    if (anEdgeIt.Current().Curve3DRepId.IsValid())
    {
      anEdgeId = anEdgeIt.CurrentId();
      break;
    }
  }
  ASSERT_TRUE(anEdgeId.IsValid(aGraph.Topo().Edges().Nb()));
  const BRepGraph_EdgeCurve3DRepId aCurveRepId =
    aGraph.Topo().Edges().Definition(anEdgeId).Curve3DRepId;
  const occ::handle<Geom_Curve> aCurve3d = BRepGraph_Tool::Edge::Curve(aGraph, anEdgeId);
  ASSERT_FALSE(aCurve3d.IsNull());

  BRepGraph_CoEdgeId aCoEdgeId;
  for (BRepGraph_CoEdgeIterator aCoEdgeIt(aGraph); aCoEdgeIt.More(); aCoEdgeIt.Next())
  {
    if (aCoEdgeIt.Current().Curve2DRepId.IsValid())
    {
      aCoEdgeId = aCoEdgeIt.CurrentId();
      break;
    }
  }
  ASSERT_TRUE(aCoEdgeId.IsValid(aGraph.Topo().CoEdges().Nb()));
  const BRepGraph_CoEdgeCurve2DRepId aPCurveRepId =
    aGraph.Topo().CoEdges().Definition(aCoEdgeId).Curve2DRepId;
  const occ::handle<Geom2d_Curve> aPCurve = BRepGraph_Tool::CoEdge::PCurve(aGraph, aCoEdgeId);
  ASSERT_FALSE(aPCurve.IsNull());

  const uint32_t aNbActiveSurfaces = aGraph.Topo().Geometry().NbActiveFaceSurfaces();
  ASSERT_GT(aNbActiveSurfaces, 0u);
  aGraph.Editor().Faces().ClearSurface(aFaceId);
  EXPECT_EQ(aGraph.Topo().Faces().Definition(aFaceId).SurfaceRepId, aSurfaceRepId);
  EXPECT_FALSE(BRepGraph_Tool::Face::HasSurface(aGraph, aFaceId));
  EXPECT_EQ(aGraph.Topo().Geometry().NbActiveFaceSurfaces(), aNbActiveSurfaces - 1u);
  aGraph.Editor().Faces().SetSurface(aFaceId, aSurface);
  EXPECT_EQ(aGraph.Topo().Faces().Definition(aFaceId).SurfaceRepId, aSurfaceRepId);
  EXPECT_EQ(aGraph.Topo().Geometry().NbActiveFaceSurfaces(), aNbActiveSurfaces);
  aGraph.Editor().Faces().ClearSurface(aFaceId);
  EXPECT_EQ(aGraph.Topo().Geometry().NbActiveFaceSurfaces(), aNbActiveSurfaces - 1u);

  const uint32_t aNbActiveCurves3D = aGraph.Topo().Geometry().NbActiveEdgeCurves3D();
  ASSERT_GT(aNbActiveCurves3D, 0u);
  aGraph.Editor().Edges().ClearCurve(anEdgeId);
  EXPECT_EQ(aGraph.Topo().Edges().Definition(anEdgeId).Curve3DRepId, aCurveRepId);
  EXPECT_FALSE(BRepGraph_Tool::Edge::HasCurve(aGraph, anEdgeId));
  EXPECT_EQ(aGraph.Topo().Geometry().NbActiveEdgeCurves3D(), aNbActiveCurves3D - 1u);
  aGraph.Editor().Edges().SetCurve(anEdgeId, aCurve3d, 0.0, 1.0);
  EXPECT_EQ(aGraph.Topo().Edges().Definition(anEdgeId).Curve3DRepId, aCurveRepId);
  EXPECT_EQ(aGraph.Topo().Geometry().NbActiveEdgeCurves3D(), aNbActiveCurves3D);
  aGraph.Editor().Edges().ClearCurve(anEdgeId);
  EXPECT_EQ(aGraph.Topo().Geometry().NbActiveEdgeCurves3D(), aNbActiveCurves3D - 1u);

  const uint32_t aNbActiveCurves2D = aGraph.Topo().Geometry().NbActiveCoEdgeCurves2D();
  ASSERT_GT(aNbActiveCurves2D, 0u);
  aGraph.Editor().CoEdges().ClearPCurve(aCoEdgeId);
  EXPECT_EQ(aGraph.Topo().CoEdges().Definition(aCoEdgeId).Curve2DRepId, aPCurveRepId);
  EXPECT_FALSE(BRepGraph_Tool::CoEdge::HasPCurve(aGraph, aCoEdgeId));
  EXPECT_EQ(aGraph.Topo().Geometry().NbActiveCoEdgeCurves2D(), aNbActiveCurves2D - 1u);
  aGraph.Editor().CoEdges().SetPCurve(aCoEdgeId, aPCurve, 0.0, 1.0);
  EXPECT_EQ(aGraph.Topo().CoEdges().Definition(aCoEdgeId).Curve2DRepId, aPCurveRepId);
  EXPECT_EQ(aGraph.Topo().Geometry().NbActiveCoEdgeCurves2D(), aNbActiveCurves2D);
  aGraph.Editor().CoEdges().ClearPCurve(aCoEdgeId);
  EXPECT_EQ(aGraph.Topo().Geometry().NbActiveCoEdgeCurves2D(), aNbActiveCurves2D - 1u);

  const occ::handle<Poly_Triangulation> aTriangulation = new Poly_Triangulation(3, 1, false);
  const occ::handle<Poly_Polygon3D>     aPolygon3D     = new Poly_Polygon3D(2, false);
  const occ::handle<Poly_Polygon2D>     aPolygon2D     = new Poly_Polygon2D(2);
  const occ::handle<Poly_PolygonOnTriangulation> aPolygonOnTri =
    new Poly_PolygonOnTriangulation(2, false);
  aGraph.Editor().Faces().SetPersistentTriangulation(aFaceId, aTriangulation);
  aGraph.Editor().Edges().SetPersistentPolygon3D(anEdgeId, aPolygon3D);
  aGraph.Editor().CoEdges().SetPersistentPolygon2D(aCoEdgeId, aPolygon2D);
  aGraph.Editor().CoEdges().SetPersistentPolygonOnTri(aCoEdgeId, aPolygonOnTri);
  const BRepGraph_FaceTriangulationRepId aTriangulationRepId =
    aGraph.Topo().Faces().Definition(aFaceId).TriangulationRepId;
  const BRepGraph_EdgePolygon3DRepId aPolygon3DRepId =
    aGraph.Topo().Edges().Definition(anEdgeId).Polygon3DRepId;
  const BRepGraph_CoEdgePolygon2DRepId aPolygon2DRepId =
    aGraph.Topo().CoEdges().Definition(aCoEdgeId).Polygon2DRepId;
  const BRepGraph_CoEdgePolygonOnTriRepId aPolygonOnTriRepId =
    aGraph.Topo().CoEdges().Definition(aCoEdgeId).PolygonOnTriRepId;

  const uint32_t aNbActiveTriangulations = aGraph.Mesh().Poly().NbActiveTriangulations();
  ASSERT_GT(aNbActiveTriangulations, 0u);
  aGraph.Editor().Faces().ClearPersistentTriangulation(aFaceId);
  EXPECT_EQ(aGraph.Topo().Faces().Definition(aFaceId).TriangulationRepId, aTriangulationRepId);
  EXPECT_FALSE(aGraph.Mesh().Persistent().Faces().Has(aFaceId));
  EXPECT_EQ(aGraph.Mesh().Poly().NbActiveTriangulations(), aNbActiveTriangulations - 1u);
  aGraph.Editor().Faces().SetPersistentTriangulation(aFaceId, aTriangulation);
  EXPECT_EQ(aGraph.Topo().Faces().Definition(aFaceId).TriangulationRepId, aTriangulationRepId);
  EXPECT_EQ(aGraph.Mesh().Poly().NbActiveTriangulations(), aNbActiveTriangulations);
  aGraph.Editor().Faces().ClearPersistentTriangulation(aFaceId);

  const uint32_t aNbActivePolygons3D = aGraph.Mesh().Poly().NbActivePolygons3D();
  ASSERT_GT(aNbActivePolygons3D, 0u);
  aGraph.Editor().Edges().ClearPersistentPolygon3D(anEdgeId);
  EXPECT_EQ(aGraph.Topo().Edges().Definition(anEdgeId).Polygon3DRepId, aPolygon3DRepId);
  EXPECT_FALSE(aGraph.Mesh().Persistent().Edges().Has(anEdgeId));
  EXPECT_EQ(aGraph.Mesh().Poly().NbActivePolygons3D(), aNbActivePolygons3D - 1u);
  aGraph.Editor().Edges().SetPersistentPolygon3D(anEdgeId, aPolygon3D);
  EXPECT_EQ(aGraph.Topo().Edges().Definition(anEdgeId).Polygon3DRepId, aPolygon3DRepId);
  EXPECT_EQ(aGraph.Mesh().Poly().NbActivePolygons3D(), aNbActivePolygons3D);
  aGraph.Editor().Edges().ClearPersistentPolygon3D(anEdgeId);

  const uint32_t aNbActivePolygons2D = aGraph.Mesh().Poly().NbActivePolygons2D();
  ASSERT_GT(aNbActivePolygons2D, 0u);
  aGraph.Editor().CoEdges().SetPersistentPolygon2D(aCoEdgeId, occ::handle<Poly_Polygon2D>());
  EXPECT_EQ(aGraph.Topo().CoEdges().Definition(aCoEdgeId).Polygon2DRepId, aPolygon2DRepId);
  EXPECT_FALSE(aGraph.Mesh().Persistent().CoEdges().Has(aCoEdgeId));
  EXPECT_EQ(aGraph.Mesh().Poly().NbActivePolygons2D(), aNbActivePolygons2D - 1u);
  aGraph.Editor().CoEdges().SetPersistentPolygon2D(aCoEdgeId, aPolygon2D);
  EXPECT_EQ(aGraph.Topo().CoEdges().Definition(aCoEdgeId).Polygon2DRepId, aPolygon2DRepId);
  EXPECT_EQ(aGraph.Mesh().Poly().NbActivePolygons2D(), aNbActivePolygons2D);
  aGraph.Editor().CoEdges().SetPersistentPolygon2D(aCoEdgeId, occ::handle<Poly_Polygon2D>());

  const uint32_t aNbActivePolygonsOnTri = aGraph.Mesh().Poly().NbActivePolygonsOnTri();
  ASSERT_GT(aNbActivePolygonsOnTri, 0u);
  aGraph.Editor().CoEdges().SetPersistentPolygonOnTri(aCoEdgeId,
                                                      occ::handle<Poly_PolygonOnTriangulation>());
  EXPECT_EQ(aGraph.Topo().CoEdges().Definition(aCoEdgeId).PolygonOnTriRepId, aPolygonOnTriRepId);
  EXPECT_FALSE(aGraph.Mesh().Persistent().CoEdges().HasPolygonOnTriangulation(aCoEdgeId));
  EXPECT_EQ(aGraph.Mesh().Poly().NbActivePolygonsOnTri(), aNbActivePolygonsOnTri - 1u);
  aGraph.Editor().CoEdges().SetPersistentPolygonOnTri(aCoEdgeId, aPolygonOnTri);
  EXPECT_EQ(aGraph.Topo().CoEdges().Definition(aCoEdgeId).PolygonOnTriRepId, aPolygonOnTriRepId);
  EXPECT_EQ(aGraph.Mesh().Poly().NbActivePolygonsOnTri(), aNbActivePolygonsOnTri);
}
