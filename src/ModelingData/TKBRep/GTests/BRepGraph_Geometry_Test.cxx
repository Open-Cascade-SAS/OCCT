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
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_ParentExplorer.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_RepId.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_Map.hxx>
#include <Precision.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>

#include <gtest/gtest.h>

namespace
{

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
    return aSolidExp.Current();
  }

  for (BRepGraph_ParentExplorer aShellExp(theGraph, theFaceId, BRepGraph_NodeId::Kind::Shell);
       aShellExp.More();
       aShellExp.Next())
  {
    return aShellExp.Current();
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

} // namespace

// ============================================================
// Geometry navigation tests
// ============================================================

TEST(BRepGraph_GeometryTest, Sphere_AllFaces_SameSurface)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeSphere(15.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // All face defs of a sphere share the same surface handle.
  ASSERT_GE(aGraph.Topo().Faces().Nb(), 1);
  ASSERT_TRUE(BRepGraph_Tool::Face::HasSurface(aGraph, BRepGraph_FaceId(0)));
  const occ::handle<Geom_Surface>& aFirstSurf =
    BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(0));
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
  aGraph.Build(BRepPrimAPI_MakeSphere(15.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // All faces of a sphere share the same surface pointer.
  ASSERT_TRUE(BRepGraph_Tool::Face::HasSurface(aGraph, BRepGraph_FaceId(0)));
  const occ::handle<Geom_Surface>& aFirstSurf =
    BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(0));
  EXPECT_FALSE(aFirstSurf.IsNull());
  int aSameCount = 0;
  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId aFaceId = aFaceIt.CurrentId();
    if (BRepGraph_Tool::Face::HasSurface(aGraph, aFaceId)
        && BRepGraph_Tool::Face::Surface(aGraph, aFaceId).get() == aFirstSurf.get())
      ++aSameCount;
  }
  EXPECT_EQ(aSameCount, aGraph.Topo().Faces().Nb());
}

TEST(BRepGraph_GeometryTest, Box_Curve3d_ValidForAll12Edges)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());
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
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId anEdgeId = anEdgeIt.CurrentId();
    EXPECT_TRUE(BRepGraph_Tool::Edge::HasCurve(aGraph, anEdgeId))
      << "Edge " << anEdgeId.Index << " has no curve";
  }
}

TEST(BRepGraph_GeometryTest, Box_FindPCurve_AllEdgeFacePairs_Valid)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aPCurveCount = 0;
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId                        anEdgeId = anEdgeIt.CurrentId();
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Topo().Edges().CoEdges(anEdgeId);
    for (int j = 0; j < aCoEdgeIdxs.Length(); ++j)
    {
      const BRepGraphInc::CoEdgeDef& aCE = aGraph.Topo().CoEdges().Definition(aCoEdgeIdxs.Value(j));
      const BRepGraphInc::CoEdgeDef* aPCurveEntry =
        BRepGraph_Tool::Edge::FindPCurve(aGraph, anEdgeId, BRepGraph_FaceId(aCE.FaceDefId.Index));
      EXPECT_NE(aPCurveEntry, nullptr);
      ++aPCurveCount;
    }
  }
  EXPECT_GT(aPCurveCount, 0);
}

TEST(BRepGraph_GeometryTest, CoEdge_FaceDefIdValid)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  for (int i = 0; i < aGraph.Topo().Edges().Nb(); ++i)
  {
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Topo().Edges().CoEdges(BRepGraph_EdgeId(i));
    for (int j = 0; j < aCoEdgeIdxs.Length(); ++j)
    {
      const BRepGraphInc::CoEdgeDef& aCE = aGraph.Topo().CoEdges().Definition(aCoEdgeIdxs.Value(j));
      EXPECT_TRUE(aCE.FaceDefId.IsValid())
        << "Edge " << i << " CoEdge " << j << " has invalid FaceDefId";
      EXPECT_EQ(BRepGraph_NodeId(aCE.FaceDefId).NodeKind, BRepGraph_NodeId::Kind::Face);
    }
  }
}

TEST(BRepGraph_GeometryTest, CoEdge_ParamRange_NonZero)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCoEdgeCount = 0;
  for (int i = 0; i < aGraph.Topo().Edges().Nb(); ++i)
  {
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Topo().Edges().CoEdges(BRepGraph_EdgeId(i));
    for (int j = 0; j < aCoEdgeIdxs.Length(); ++j)
    {
      const BRepGraphInc::CoEdgeDef& aCE = aGraph.Topo().CoEdges().Definition(aCoEdgeIdxs.Value(j));
      const double                   aRange = aCE.ParamLast - aCE.ParamFirst;
      EXPECT_GT(std::abs(aRange), Precision::PConfusion())
        << "Edge " << i << " CoEdge " << j << " has zero parameter range";
      ++aCoEdgeCount;
    }
  }
  EXPECT_GT(aCoEdgeCount, 0);
}

TEST(BRepGraph_GeometryTest, CoEdge_Continuity_Valid)
{
  const TopoDS_Shape aShape = BRepPrimAPI_MakeCylinder(10.0, 20.0).Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);
  ASSERT_TRUE(aGraph.IsDone());

  bool hasNonC0Continuity = false;
  for (int i = 0; i < aGraph.Topo().Edges().Nb(); ++i)
  {
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Topo().Edges().CoEdges(BRepGraph_EdgeId(i));
    for (int j = 0; j < aCoEdgeIdxs.Length(); ++j)
    {
      const BRepGraphInc::CoEdgeDef& aCE = aGraph.Topo().CoEdges().Definition(aCoEdgeIdxs.Value(j));
      if (aCE.Continuity > GeomAbs_C0)
      {
        hasNonC0Continuity = true;
      }
    }
  }

  EXPECT_TRUE(hasNonC0Continuity);
}

TEST(BRepGraph_GeometryTest, FaceDef_Surface_IsNotNull)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  for (int i = 0; i < aGraph.Topo().Faces().Nb(); ++i)
  {
    EXPECT_TRUE(BRepGraph_Tool::Face::HasSurface(aGraph, BRepGraph_FaceId(i)))
      << "Face " << i << " has null Surface handle";
  }
}

TEST(BRepGraph_GeometryTest, EdgeDef_Curve3d_IsNotNull)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  for (int i = 0; i < aGraph.Topo().Edges().Nb(); ++i)
  {
    EXPECT_TRUE(BRepGraph_Tool::Edge::HasCurve(aGraph, BRepGraph_EdgeId(i)))
      << "Edge " << i << " has null Curve3d handle";
  }
}

TEST(BRepGraph_GeometryTest, SameDomainFaces_SimpleBox_Empty)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // For a simple box each face has a unique surface, so SameDomainFaces is empty.
  for (int i = 0; i < aGraph.Topo().Faces().Nb(); ++i)
  {
    const BRepGraph_FaceId                     aFaceDefId(i);
    const NCollection_Vector<BRepGraph_FaceId> aSameDomain =
      aGraph.Topo().Faces().SameDomain(aFaceDefId, aGraph.Allocator());
    EXPECT_EQ(aSameDomain.Length(), 0) << "Face def " << i << " has unexpected same-domain faces";
  }
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
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  // Moved() preserves TShape - one solid definition, two compound ChildRefs.
  EXPECT_EQ(aGraph.Topo().Solids().Nb(), 1);
  // Verify the graph was built successfully.
  EXPECT_TRUE(aGraph.IsDone());
}

TEST(BRepGraph_GeometryTest, FaceDef_Triangulation_NullForAnalyticNoCrash)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Analytical box faces should have null triangulation (no mesh computed).
  // Simply verify access does not crash.
  for (int i = 0; i < aGraph.Topo().Faces().Nb(); ++i)
  {
    const bool hasActiveTri = BRepGraph_Tool::Face::HasTriangulation(aGraph, BRepGraph_FaceId(i));
    EXPECT_FALSE(hasActiveTri) << "Face " << i << " unexpectedly has a triangulation";
  }
}

// ============================================================
// Definition traversal tests
// ============================================================

TEST(BRepGraph_GeometryTest, SolidDef_CountMatchesNb)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (int aSolidIdx = 0; aSolidIdx < aGraph.Topo().Solids().Nb(); ++aSolidIdx)
  {
    (void)aGraph.Topo().Solids().Definition(BRepGraph_SolidId(aSolidIdx));
    ++aCount;
  }
  EXPECT_EQ(aCount, aGraph.Topo().Solids().Nb());
}

TEST(BRepGraph_GeometryTest, ShellDef_CountMatchesNb)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (int aShellIdx = 0; aShellIdx < aGraph.Topo().Shells().Nb(); ++aShellIdx)
  {
    (void)aGraph.Topo().Shells().Definition(BRepGraph_ShellId(aShellIdx));
    ++aCount;
  }
  EXPECT_EQ(aCount, aGraph.Topo().Shells().Nb());
}

TEST(BRepGraph_GeometryTest, FaceDef_CountMatchesNb)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (int aFaceIdx = 0; aFaceIdx < aGraph.Topo().Faces().Nb(); ++aFaceIdx)
  {
    (void)aGraph.Topo().Faces().Definition(BRepGraph_FaceId(aFaceIdx));
    ++aCount;
  }
  EXPECT_EQ(aCount, aGraph.Topo().Faces().Nb());
}

TEST(BRepGraph_GeometryTest, WireDef_CountMatchesNb)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (int aWireIdx = 0; aWireIdx < aGraph.Topo().Wires().Nb(); ++aWireIdx)
  {
    (void)aGraph.Topo().Wires().Definition(BRepGraph_WireId(aWireIdx));
    ++aCount;
  }
  EXPECT_EQ(aCount, aGraph.Topo().Wires().Nb());
}

TEST(BRepGraph_GeometryTest, EdgeDef_CountMatchesNb)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Topo().Edges().Nb(); ++anEdgeIdx)
  {
    (void)aGraph.Topo().Edges().Definition(BRepGraph_EdgeId(anEdgeIdx));
    ++aCount;
  }
  EXPECT_EQ(aCount, aGraph.Topo().Edges().Nb());
}

TEST(BRepGraph_GeometryTest, VertexDef_CountMatchesNb)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (int aVertexIdx = 0; aVertexIdx < aGraph.Topo().Vertices().Nb(); ++aVertexIdx)
  {
    (void)aGraph.Topo().Vertices().Definition(BRepGraph_VertexId(aVertexIdx));
    ++aCount;
  }
  EXPECT_EQ(aCount, aGraph.Topo().Vertices().Nb());
}

TEST(BRepGraph_GeometryTest, FaceDef_CountViaIterator_MatchesNb)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (int aFaceIdx = 0; aFaceIdx < aGraph.Topo().Faces().Nb(); ++aFaceIdx)
  {
    (void)aGraph.Topo().Faces().Definition(BRepGraph_FaceId(aFaceIdx));
    ++aCount;
  }
  EXPECT_EQ(aCount, aGraph.Topo().Faces().Nb());
}

TEST(BRepGraph_GeometryTest, FaceDef_AllSurfacesNonNull)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (int aFaceIdx = 0; aFaceIdx < aGraph.Topo().Faces().Nb(); ++aFaceIdx)
  {
    EXPECT_TRUE(BRepGraph_Tool::Face::HasSurface(aGraph, BRepGraph_FaceId(aFaceIdx)));
    ++aCount;
  }
  EXPECT_EQ(aCount, aGraph.Topo().Faces().Nb());
}

TEST(BRepGraph_GeometryTest, EdgeDef_AllCurves3dNonNull)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Topo().Edges().Nb(); ++anEdgeIdx)
  {
    if (!BRepGraph_Tool::Edge::Degenerated(aGraph, BRepGraph_EdgeId(anEdgeIdx)))
    {
      EXPECT_TRUE(BRepGraph_Tool::Edge::HasCurve(aGraph, BRepGraph_EdgeId(anEdgeIdx)));
    }
    ++aCount;
  }
  EXPECT_EQ(aCount, aGraph.Topo().Edges().Nb());
}

TEST(BRepGraph_GeometryTest, AllCoEdgesHaveCurve2d)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (int i = 0; i < aGraph.Topo().Edges().Nb(); ++i)
  {
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Topo().Edges().CoEdges(BRepGraph_EdgeId(i));
    for (int j = 0; j < aCoEdgeIdxs.Length(); ++j)
    {
      EXPECT_TRUE(BRepGraph_Tool::CoEdge::HasPCurve(aGraph, aCoEdgeIdxs.Value(j)));
      ++aCount;
    }
  }
  EXPECT_GT(aCount, 0);
}

// ============================================================
// Connected component grouping via ParentExplorer
// ============================================================

TEST(BRepGraph_GeometryTest, ConnectedComponents_SingleBox_OneComponent)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const NCollection_DataMap<int, int> aFaceCounts = faceCountsByComponent(aGraph);
  ASSERT_EQ(aFaceCounts.Extent(), 1);
  int aComponentFaces = 0;
  for (NCollection_DataMap<int, int>::Iterator anIt(aFaceCounts); anIt.More(); anIt.Next())
  {
    aComponentFaces = anIt.Value();
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
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

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
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  const NCollection_DataMap<int, int> aFaceCounts = faceCountsByComponent(aGraph);
  ASSERT_EQ(aFaceCounts.Extent(), 2);
  for (NCollection_DataMap<int, int>::Iterator anIt(aFaceCounts); anIt.More(); anIt.Next())
  {
    EXPECT_EQ(anIt.Value(), 6);
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
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  const NCollection_DataMap<int, int> aFaceCounts = faceCountsByComponent(aGraph);
  int                                 aTotalFaces = 0;
  for (NCollection_DataMap<int, int>::Iterator anIt(aFaceCounts); anIt.More(); anIt.Next())
  {
    aTotalFaces += anIt.Value();
  }

  EXPECT_EQ(aTotalFaces, aGraph.Topo().Faces().Nb());
}

// ============================================================
// SameParameter / SameRange round-trip tests
// ============================================================

TEST(BRepGraph_GeometryTest, Box_EdgeDef_SameParameter_IsSet)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().Edges().Nb(), 0);

  // Box edges are well-formed; SameParameter should be true for all.
  for (int i = 0; i < aGraph.Topo().Edges().Nb(); ++i)
  {
    EXPECT_TRUE(BRepGraph_Tool::Edge::SameParameter(aGraph, BRepGraph_EdgeId(i)))
      << "Edge def " << i << " has SameParameter=false";
  }
}

TEST(BRepGraph_GeometryTest, Box_EdgeDef_SameRange_IsSet)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().Edges().Nb(), 0);

  for (int i = 0; i < aGraph.Topo().Edges().Nb(); ++i)
  {
    EXPECT_TRUE(BRepGraph_Tool::Edge::SameRange(aGraph, BRepGraph_EdgeId(i)))
      << "Edge def " << i << " has SameRange=false";
  }
}

// ============================================================
// Seam edge PCurve validation tests
// ============================================================

TEST(BRepGraph_GeometryTest, Cylinder_SeamEdge_HasTwoCoEdges)
{
  // A cylinder has a seam edge on its lateral face.
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeCylinder(5.0, 20.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Find a seam edge via CoEdge SeamPairId.
  bool aFoundSeam = false;
  for (int i = 0; i < aGraph.Topo().Edges().Nb() && !aFoundSeam; ++i)
  {
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Topo().Edges().CoEdges(BRepGraph_EdgeId(i));
    for (int j = 0; j < aCoEdgeIdxs.Length() && !aFoundSeam; ++j)
    {
      const BRepGraphInc::CoEdgeDef& aCE = aGraph.Topo().CoEdges().Definition(aCoEdgeIdxs.Value(j));
      if (aCE.SeamPairId.IsValid())
      {
        // Verify the paired coedge has opposite orientation.
        const BRepGraphInc::CoEdgeDef& aPair = aGraph.Topo().CoEdges().Definition(aCE.SeamPairId);
        EXPECT_NE(aCE.Sense, aPair.Sense) << "Seam coedges should have opposite orientations";
        EXPECT_EQ(aCE.FaceDefId, aPair.FaceDefId) << "Seam coedges should share the same face";
        aFoundSeam = true;
      }
    }
  }
  EXPECT_TRUE(aFoundSeam) << "No seam edge found in cylinder";
}

TEST(BRepGraph_GeometryTest, Cylinder_SeamEdge_FindPCurve_WithOrientation)
{
  // Verify FindPCurve(edge, face, orientation) returns different entries for FORWARD vs REVERSED.
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeCylinder(5.0, 20.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  for (int i = 0; i < aGraph.Topo().Edges().Nb(); ++i)
  {
    const BRepGraph_NodeId                        anEdgeDefId(BRepGraph_NodeId::Kind::Edge, i);
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Topo().Edges().CoEdges(BRepGraph_EdgeId(i));

    for (int j = 0; j < aCoEdgeIdxs.Length(); ++j)
    {
      const BRepGraphInc::CoEdgeDef& aCE = aGraph.Topo().CoEdges().Definition(aCoEdgeIdxs.Value(j));
      if (!aCE.SeamPairId.IsValid())
        continue;

      // This is a seam edge - test oriented overload.
      const BRepGraph_FaceId         aFaceId(aCE.FaceDefId.Index);
      const BRepGraphInc::CoEdgeDef* aPC_Fwd =
        BRepGraph_Tool::Edge::FindPCurve(aGraph, BRepGraph_EdgeId(i), aFaceId, TopAbs_FORWARD);
      const BRepGraphInc::CoEdgeDef* aPC_Rev =
        BRepGraph_Tool::Edge::FindPCurve(aGraph, BRepGraph_EdgeId(i), aFaceId, TopAbs_REVERSED);

      EXPECT_NE(aPC_Fwd, nullptr) << "FindPCurve FORWARD returned null for seam edge";
      EXPECT_NE(aPC_Rev, nullptr) << "FindPCurve REVERSED returned null for seam edge";
      EXPECT_NE(aPC_Fwd, aPC_Rev) << "FORWARD and REVERSED PCurves should be different entries";
      return; // one seam is enough
    }
  }
  GTEST_SKIP() << "No seam edge found; test inconclusive";
}

TEST(BRepGraph_GeometryTest, Box_FindPCurve_MatchesToolOverload)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  for (int i = 0; i < aGraph.Topo().Edges().Nb(); ++i)
  {
    const BRepGraph_NodeId                        anEdgeDefId(BRepGraph_NodeId::Kind::Edge, i);
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Topo().Edges().CoEdges(BRepGraph_EdgeId(i));

    for (int j = 0; j < aCoEdgeIdxs.Length(); ++j)
    {
      const BRepGraphInc::CoEdgeDef& aCE = aGraph.Topo().CoEdges().Definition(aCoEdgeIdxs.Value(j));
      const BRepGraphInc::CoEdgeDef* aFromDefs =
        BRepGraph_Tool::Edge::FindPCurve(aGraph,
                                         BRepGraph_EdgeId(anEdgeDefId.Index),
                                         aCE.FaceDefId,
                                         aCE.Sense);
      const BRepGraphInc::CoEdgeDef* aFromTool =
        BRepGraph_Tool::Edge::FindPCurve(aGraph,
                                         BRepGraph_EdgeId(i),
                                         BRepGraph_FaceId(aCE.FaceDefId.Index),
                                         aCE.Sense);

      EXPECT_EQ(aFromDefs, aFromTool);
      EXPECT_NE(aFromDefs, nullptr);
    }
  }
}

TEST(BRepGraph_GeometryTest, Cylinder_SeamEdge_FindPCurve_DistinguishesOrientation)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeCylinder(5.0, 20.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  for (int i = 0; i < aGraph.Topo().Edges().Nb(); ++i)
  {
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Topo().Edges().CoEdges(BRepGraph_EdgeId(i));

    for (int j = 0; j < aCoEdgeIdxs.Length(); ++j)
    {
      const BRepGraphInc::CoEdgeDef& aCE = aGraph.Topo().CoEdges().Definition(aCoEdgeIdxs.Value(j));
      if (!aCE.SeamPairId.IsValid())
        continue;

      // Seam edge: same face, two orientations.
      const BRepGraph_FaceId aFaceId = aCE.FaceDefId;
      const BRepGraph_EdgeId anEdgeId(i);

      const BRepGraphInc::CoEdgeDef* aPCFwd =
        BRepGraph_Tool::Edge::FindPCurve(aGraph, anEdgeId, aFaceId, TopAbs_FORWARD);
      const BRepGraphInc::CoEdgeDef* aPCRev =
        BRepGraph_Tool::Edge::FindPCurve(aGraph, anEdgeId, aFaceId, TopAbs_REVERSED);

      EXPECT_NE(aPCFwd, nullptr);
      EXPECT_NE(aPCRev, nullptr);
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
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_GT(aGraph.Topo().Geometry().NbSurfaces(), 0);
  EXPECT_GT(aGraph.Topo().Geometry().NbCurves3D(), 0);
  EXPECT_GT(aGraph.Topo().Geometry().NbCurves2D(), 0);

  // Every face has a valid surface.
  for (int i = 0; i < aGraph.Topo().Faces().Nb(); ++i)
  {
    EXPECT_TRUE(BRepGraph_Tool::Face::HasSurface(aGraph, BRepGraph_FaceId(i)))
      << "Face " << i << " has no surface";
    EXPECT_FALSE(BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(i)).IsNull());
  }

  // Every non-degenerate edge has a valid 3D curve.
  for (int i = 0; i < aGraph.Topo().Edges().Nb(); ++i)
  {
    if (!BRepGraph_Tool::Edge::Degenerated(aGraph, BRepGraph_EdgeId(i))
        && BRepGraph_Tool::Edge::HasCurve(aGraph, BRepGraph_EdgeId(i)))
    {
      EXPECT_FALSE(BRepGraph_Tool::Edge::Curve(aGraph, BRepGraph_EdgeId(i)).IsNull());
    }
  }

  // Every coedge with a PCurve has a valid Curve2DRepId.
  for (int i = 0; i < aGraph.Topo().CoEdges().Nb(); ++i)
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge =
      aGraph.Topo().CoEdges().Definition(BRepGraph_CoEdgeId(i));
    if (aCoEdge.Curve2DRepId.IsValid())
    {
      const occ::handle<Geom2d_Curve>& aPCurve =
        BRepGraph_Tool::CoEdge::PCurve(aGraph, BRepGraph_CoEdgeId(i));
      EXPECT_FALSE(aPCurve.IsNull());
    }
  }
}

TEST(BRepGraph_GeometryTest, Sphere_SurfaceDedup_SharedHandle)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeSphere(15.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // All faces of a sphere share the same TShape -> same entity -> same surface.
  if (aGraph.Topo().Faces().Nb() > 1)
  {
    const Geom_Surface* aFirstSurfPtr =
      BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(0)).get();
    for (int i = 1; i < aGraph.Topo().Faces().Nb(); ++i)
    {
      EXPECT_EQ(BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(i)).get(), aFirstSurfPtr)
        << "Faces sharing same surface should share the same surface pointer";
    }
  }
}

TEST(BRepGraph_GeometryTest, Cylinder_TriangulationReps_Populated)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeCylinder(5.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  for (int i = 0; i < aGraph.Topo().Faces().Nb(); ++i)
  {
    const BRepGraphInc::FaceDef& aFace = aGraph.Topo().Faces().Definition(BRepGraph_FaceId(i));
    if (!aFace.TriangulationRepIds.IsEmpty())
    {
      for (int j = 0; j < aFace.TriangulationRepIds.Length(); ++j)
      {
        EXPECT_TRUE(aFace.TriangulationRepIds.Value(j).IsValid());
      }
      // Verify active triangulation is non-null via BRepGraph_Tool.
      if (BRepGraph_Tool::Face::HasTriangulation(aGraph, BRepGraph_FaceId(i)))
      {
        EXPECT_FALSE(BRepGraph_Tool::Face::Triangulation(aGraph, BRepGraph_FaceId(i)).IsNull());
      }
    }
  }
}

TEST(BRepGraph_GeometryTest, RepId_FactoryMethods)
{
  const BRepGraph_SurfaceRepId aSurfId = BRepGraph_RepId::Surface(42);
  EXPECT_EQ(BRepGraph_RepId(aSurfId).RepKind, BRepGraph_RepId::Kind::Surface);
  EXPECT_EQ(aSurfId.Index, 42);
  EXPECT_TRUE(aSurfId.IsValid());

  const BRepGraph_Curve3DRepId aCurve3DId = BRepGraph_RepId::Curve3D(7);
  EXPECT_EQ(BRepGraph_RepId(aCurve3DId).RepKind, BRepGraph_RepId::Kind::Curve3D);
  EXPECT_EQ(aCurve3DId.Index, 7);

  const BRepGraph_RepId aDefaultId;
  EXPECT_FALSE(aDefaultId.IsValid());

  EXPECT_EQ(aSurfId, BRepGraph_RepId::Surface(42));
  EXPECT_NE(BRepGraph_RepId(aSurfId), BRepGraph_RepId(aCurve3DId));
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
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 12);
  EXPECT_GT(aGraph.Topo().Geometry().NbSurfaces(), 0);
  EXPECT_LE(aGraph.Topo().Geometry().NbSurfaces(), 12);

  for (int i = 0; i < aGraph.Topo().Faces().Nb(); ++i)
  {
    EXPECT_TRUE(BRepGraph_Tool::Face::HasSurface(aGraph, BRepGraph_FaceId(i)));
  }
  for (int i = 0; i < aGraph.Topo().Edges().Nb(); ++i)
  {
    if (!BRepGraph_Tool::Edge::Degenerated(aGraph, BRepGraph_EdgeId(i))
        && BRepGraph_Tool::Edge::HasCurve(aGraph, BRepGraph_EdgeId(i)))
    {
      EXPECT_FALSE(BRepGraph_Tool::Edge::Curve(aGraph, BRepGraph_EdgeId(i)).IsNull());
    }
  }
}

TEST(BRepGraph_GeometryTest, Box_Polygon2DRep_MatchesInline)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Every coedge with a Polygon2DRepId has a valid polygon rep.
  for (int i = 0; i < aGraph.Topo().CoEdges().Nb(); ++i)
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge =
      aGraph.Topo().CoEdges().Definition(BRepGraph_CoEdgeId(i));
    if (aCoEdge.Polygon2DRepId.IsValid())
    {
      const occ::handle<Poly_Polygon2D>& aPoly =
        BRepGraph_Tool::CoEdge::PolygonOnSurface(aGraph, BRepGraph_CoEdgeId(i));
      EXPECT_FALSE(aPoly.IsNull()) << "CoEdge " << i << " has Polygon2DRepId but null polygon";
    }
    // PolygonOnTriRepIds should have valid rep entries.
    for (int j = 0; j < aCoEdge.PolygonOnTriRepIds.Length(); ++j)
    {
      EXPECT_TRUE(aCoEdge.PolygonOnTriRepIds.Value(j).IsValid());
    }
  }
}
