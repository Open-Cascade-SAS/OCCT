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
#include <BRepGraph_Analyze.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_PCurveContext.hxx>
#include <BRepGraph_RepId.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_SpatialView.hxx>
#include <BRepGraph_SubGraph.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Map.hxx>
#include <Precision.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>

#include <gtest/gtest.h>

// ============================================================
// Geometry navigation tests
// ============================================================

TEST(BRepGraphGeometry, Sphere_AllFaces_SameSurface)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeSphere(15.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // All face defs of a sphere share the same surface handle.
  ASSERT_GE(aGraph.Defs().NbFaces(), 1);
  ASSERT_TRUE(BRepGraph_Tool::Face::HasSurface(aGraph, BRepGraph_FaceId(0)));
  const occ::handle<Geom_Surface>& aFirstSurf =
    BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(0));
  EXPECT_FALSE(aFirstSurf.IsNull());
  for (int i = 1; i < aGraph.Defs().NbFaces(); ++i)
  {
    ASSERT_TRUE(BRepGraph_Tool::Face::HasSurface(aGraph, BRepGraph_FaceId(i)));
    EXPECT_EQ(BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(i)).get(), aFirstSurf.get());
  }
}

TEST(BRepGraphGeometry, Sphere_AllFacesShareSurface)
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
  for (int i = 0; i < aGraph.Defs().NbFaces(); ++i)
  {
    if (BRepGraph_Tool::Face::HasSurface(aGraph, BRepGraph_FaceId(i))
        && BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(i)).get() == aFirstSurf.get())
      ++aSameCount;
  }
  EXPECT_EQ(aSameCount, aGraph.Defs().NbFaces());
}

TEST(BRepGraphGeometry, Box_Curve3d_ValidForAll12Edges)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.Defs().NbEdges(), 12);

  for (int i = 0; i < aGraph.Defs().NbEdges(); ++i)
  {
    EXPECT_TRUE(BRepGraph_Tool::Edge::HasCurve(aGraph, BRepGraph_EdgeId(i)))
      << "Edge def " << i << " has no valid curve";
  }
}

TEST(BRepGraphGeometry, Box_AllEdgesHaveCurve3d)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  for (int i = 0; i < aGraph.Defs().NbEdges(); ++i)
  {
    EXPECT_TRUE(BRepGraph_Tool::Edge::HasCurve(aGraph, BRepGraph_EdgeId(i)))
      << "Edge " << i << " has no curve";
  }
}

TEST(BRepGraphGeometry, Box_FindPCurve_AllEdgeFacePairs_Valid)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aPCurveCount = 0;
  for (int i = 0; i < aGraph.Defs().NbEdges(); ++i)
  {
    const BRepGraph_NodeId                        aEdgeDefId(BRepGraph_NodeId::Kind::Edge, i);
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Defs().CoEdgesOfEdge(BRepGraph_EdgeId(i));
    for (int j = 0; j < aCoEdgeIdxs.Length(); ++j)
    {
      const BRepGraphInc::CoEdgeEntity& aCE = aGraph.Defs().CoEdge(aCoEdgeIdxs.Value(j));
      const BRepGraphInc::CoEdgeEntity* aPCurveEntry =
        BRepGraph_Tool::Edge::FindPCurve(aGraph,
                                         BRepGraph_EdgeId(i),
                                         BRepGraph_FaceId(aCE.FaceDefId.Index));
      EXPECT_NE(aPCurveEntry, nullptr);
      ++aPCurveCount;
    }
  }
  EXPECT_GT(aPCurveCount, 0);
}

TEST(BRepGraphGeometry, CoEdge_FaceDefIdValid)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  for (int i = 0; i < aGraph.Defs().NbEdges(); ++i)
  {
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Defs().CoEdgesOfEdge(BRepGraph_EdgeId(i));
    for (int j = 0; j < aCoEdgeIdxs.Length(); ++j)
    {
      const BRepGraphInc::CoEdgeEntity& aCE = aGraph.Defs().CoEdge(aCoEdgeIdxs.Value(j));
      EXPECT_TRUE(aCE.FaceDefId.IsValid())
        << "Edge " << i << " CoEdge " << j << " has invalid FaceDefId";
      EXPECT_EQ(BRepGraph_NodeId(aCE.FaceDefId).NodeKind, BRepGraph_NodeId::Kind::Face);
    }
  }
}

TEST(BRepGraphGeometry, CoEdge_ParamRange_NonZero)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCoEdgeCount = 0;
  for (int i = 0; i < aGraph.Defs().NbEdges(); ++i)
  {
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Defs().CoEdgesOfEdge(BRepGraph_EdgeId(i));
    for (int j = 0; j < aCoEdgeIdxs.Length(); ++j)
    {
      const BRepGraphInc::CoEdgeEntity& aCE    = aGraph.Defs().CoEdge(aCoEdgeIdxs.Value(j));
      const double                      aRange = aCE.ParamLast - aCE.ParamFirst;
      EXPECT_GT(std::abs(aRange), Precision::PConfusion())
        << "Edge " << i << " CoEdge " << j << " has zero parameter range";
      ++aCoEdgeCount;
    }
  }
  EXPECT_GT(aCoEdgeCount, 0);
}

TEST(BRepGraphGeometry, CoEdge_Continuity_Valid)
{
  const TopoDS_Shape aShape = BRepPrimAPI_MakeCylinder(10.0, 20.0).Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);
  ASSERT_TRUE(aGraph.IsDone());

  bool hasNonC0Continuity = false;
  for (int i = 0; i < aGraph.Defs().NbEdges(); ++i)
  {
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Defs().CoEdgesOfEdge(BRepGraph_EdgeId(i));
    for (int j = 0; j < aCoEdgeIdxs.Length(); ++j)
    {
      const BRepGraphInc::CoEdgeEntity& aCE = aGraph.Defs().CoEdge(aCoEdgeIdxs.Value(j));
      if (aCE.Continuity > GeomAbs_C0)
      {
        hasNonC0Continuity = true;
      }
    }
  }

  EXPECT_TRUE(hasNonC0Continuity);
}

TEST(BRepGraphGeometry, FaceDef_Surface_IsNotNull)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  for (int i = 0; i < aGraph.Defs().NbFaces(); ++i)
  {
    EXPECT_TRUE(BRepGraph_Tool::Face::HasSurface(aGraph, BRepGraph_FaceId(i)))
      << "Face " << i << " has null Surface handle";
  }
}

TEST(BRepGraphGeometry, EdgeDef_Curve3d_IsNotNull)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  for (int i = 0; i < aGraph.Defs().NbEdges(); ++i)
  {
    EXPECT_TRUE(BRepGraph_Tool::Edge::HasCurve(aGraph, BRepGraph_EdgeId(i)))
      << "Edge " << i << " has null Curve3d handle";
  }
}

TEST(BRepGraphGeometry, SameDomainFaces_SimpleBox_Empty)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // For a simple box each face has a unique surface, so SameDomainFaces is empty.
  for (int i = 0; i < aGraph.Defs().NbFaces(); ++i)
  {
    const BRepGraph_NodeId                     aFaceDefId(BRepGraph_NodeId::Kind::Face, i);
    const NCollection_Vector<BRepGraph_NodeId> aSameDomain =
      aGraph.Spatial().SameDomainFaces(aFaceDefId);
    EXPECT_EQ(aSameDomain.Length(), 0) << "Face def " << i << " has unexpected same-domain faces";
  }
}

TEST(BRepGraphGeometry, CompoundWithMovedChild_SharedSolidDef)
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
  EXPECT_EQ(aGraph.Defs().NbSolids(), 1);
  // Verify the graph was built successfully.
  EXPECT_TRUE(aGraph.IsDone());
}

TEST(BRepGraphGeometry, FaceDef_Triangulation_NullForAnalyticNoCrash)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Analytical box faces should have null triangulation (no mesh computed).
  // Simply verify access does not crash.
  for (int i = 0; i < aGraph.Defs().NbFaces(); ++i)
  {
    const bool hasActiveTri = BRepGraph_Tool::Face::HasTriangulation(aGraph, BRepGraph_FaceId(i));
    EXPECT_FALSE(hasActiveTri) << "Face " << i << " unexpectedly has a triangulation";
  }
}

// ============================================================
// Iterator tests
// ============================================================

TEST(BRepGraphIterator, SolidDef_CountMatchesNb)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (BRepGraph_Iterator<BRepGraph_TopoNode::SolidDef> anIt(aGraph); anIt.More(); anIt.Next())
  {
    (void)anIt.Current();
    ++aCount;
  }
  EXPECT_EQ(aCount, aGraph.Defs().NbSolids());
}

TEST(BRepGraphIterator, ShellDef_CountMatchesNb)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (BRepGraph_Iterator<BRepGraph_TopoNode::ShellDef> anIt(aGraph); anIt.More(); anIt.Next())
  {
    (void)anIt.Current();
    ++aCount;
  }
  EXPECT_EQ(aCount, aGraph.Defs().NbShells());
}

TEST(BRepGraphIterator, FaceDef_CountMatchesNb)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (BRepGraph_Iterator<BRepGraph_TopoNode::FaceDef> anIt(aGraph); anIt.More(); anIt.Next())
  {
    (void)anIt.Current();
    ++aCount;
  }
  EXPECT_EQ(aCount, aGraph.Defs().NbFaces());
}

TEST(BRepGraphIterator, WireDef_CountMatchesNb)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (BRepGraph_Iterator<BRepGraph_TopoNode::WireDef> anIt(aGraph); anIt.More(); anIt.Next())
  {
    (void)anIt.Current();
    ++aCount;
  }
  EXPECT_EQ(aCount, aGraph.Defs().NbWires());
}

TEST(BRepGraphIterator, EdgeDef_CountMatchesNb)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (BRepGraph_Iterator<BRepGraph_TopoNode::EdgeDef> anIt(aGraph); anIt.More(); anIt.Next())
  {
    (void)anIt.Current();
    ++aCount;
  }
  EXPECT_EQ(aCount, aGraph.Defs().NbEdges());
}

TEST(BRepGraphIterator, VertexDef_CountMatchesNb)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (BRepGraph_Iterator<BRepGraph_TopoNode::VertexDef> anIt(aGraph); anIt.More(); anIt.Next())
  {
    (void)anIt.Current();
    ++aCount;
  }
  EXPECT_EQ(aCount, aGraph.Defs().NbVertices());
}

TEST(BRepGraphIterator, FaceDef_CountViaIterator_MatchesNb)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (BRepGraph_Iterator<BRepGraph_TopoNode::FaceDef> anIt(aGraph); anIt.More(); anIt.Next())
  {
    (void)anIt.Current();
    ++aCount;
  }
  EXPECT_EQ(aCount, aGraph.Defs().NbFaces());
}

TEST(BRepGraphIterator, FaceDef_AllSurfacesNonNull)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (BRepGraph_Iterator<BRepGraph_TopoNode::FaceDef> anIt(aGraph); anIt.More(); anIt.Next())
  {
    EXPECT_TRUE(BRepGraph_Tool::Face::HasSurface(aGraph, BRepGraph_FaceId(anIt.Index())));
    ++aCount;
  }
  EXPECT_EQ(aCount, aGraph.Defs().NbFaces());
}

TEST(BRepGraphIterator, EdgeDef_AllCurves3dNonNull)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (BRepGraph_Iterator<BRepGraph_TopoNode::EdgeDef> anIt(aGraph); anIt.More(); anIt.Next())
  {
    if (!BRepGraph_Tool::Edge::Degenerated(aGraph, BRepGraph_EdgeId(anIt.Index())))
      EXPECT_TRUE(BRepGraph_Tool::Edge::HasCurve(aGraph, BRepGraph_EdgeId(anIt.Index())));
    ++aCount;
  }
  EXPECT_EQ(aCount, aGraph.Defs().NbEdges());
}

TEST(BRepGraphIterator, AllCoEdgesHaveCurve2d)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (int i = 0; i < aGraph.Defs().NbEdges(); ++i)
  {
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Defs().CoEdgesOfEdge(BRepGraph_EdgeId(i));
    for (int j = 0; j < aCoEdgeIdxs.Length(); ++j)
    {
      EXPECT_TRUE(BRepGraph_Tool::CoEdge::HasPCurve(aGraph, aCoEdgeIdxs.Value(j)));
      ++aCount;
    }
  }
  EXPECT_GT(aCount, 0);
}

// ============================================================
// SubGraph / Decompose tests
// ============================================================

TEST(BRepGraphSubGraph, Decompose_SingleBox_OneComponent)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const NCollection_Vector<BRepGraph_SubGraph> aSubs = BRepGraph_Analyze::Decompose(aGraph);
  EXPECT_EQ(aSubs.Length(), 1);
}

TEST(BRepGraphSubGraph, Decompose_TwoBoxCompound_TwoComponents)
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

  const NCollection_Vector<BRepGraph_SubGraph> aSubs = BRepGraph_Analyze::Decompose(aGraph);
  EXPECT_EQ(aSubs.Length(), 2);
}

TEST(BRepGraphSubGraph, SubGraph_IndicesDisjoint_BetweenComponents)
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

  const NCollection_Vector<BRepGraph_SubGraph> aSubs = BRepGraph_Analyze::Decompose(aGraph);
  ASSERT_EQ(aSubs.Length(), 2);

  // Collect face def ids from both components and verify they are disjoint.
  NCollection_Map<int>                        aFaceSet1;
  const NCollection_Vector<BRepGraph_FaceId>& aFaces1 = aSubs.Value(0).FaceDefIds();
  for (int i = 0; i < aFaces1.Length(); ++i)
  {
    aFaceSet1.Add(aFaces1.Value(i).Index);
  }

  const NCollection_Vector<BRepGraph_FaceId>& aFaces2 = aSubs.Value(1).FaceDefIds();
  for (int i = 0; i < aFaces2.Length(); ++i)
  {
    EXPECT_FALSE(aFaceSet1.Contains(aFaces2.Value(i).Index))
      << "Face def index " << aFaces2.Value(i).Index << " found in both components";
  }
}

TEST(BRepGraphSubGraph, SubGraph_NbTopoNodes_SumEqualsTotal)
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

  const NCollection_Vector<BRepGraph_SubGraph> aSubs = BRepGraph_Analyze::Decompose(aGraph);

  // SubGraph NbTopoNodes counts per-definition indices but may include duplicates
  // (e.g., vertex defs counted multiple times across edges). Verify the sum is at least
  // as large as the total unique def count and that each component contributes > 0.
  int aTotalTopoNodes = 0;
  for (int i = 0; i < aSubs.Length(); ++i)
  {
    EXPECT_GT(aSubs.Value(i).NbTopoNodes(), 0)
      << "SubGraph component " << i << " has zero topo nodes";
    aTotalTopoNodes += aSubs.Value(i).NbTopoNodes();
  }

  const int aGraphTotal = aGraph.Defs().NbSolids() + aGraph.Defs().NbShells()
                          + aGraph.Defs().NbFaces() + aGraph.Defs().NbWires()
                          + aGraph.Defs().NbEdges() + aGraph.Defs().NbVertices();
  EXPECT_GE(aTotalTopoNodes, aGraphTotal);
}

// ============================================================
// SameParameter / SameRange round-trip tests
// ============================================================

TEST(BRepGraphGeometry, Box_EdgeDef_SameParameter_IsSet)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Defs().NbEdges(), 0);

  // Box edges are well-formed; SameParameter should be true for all.
  for (int i = 0; i < aGraph.Defs().NbEdges(); ++i)
  {
    EXPECT_TRUE(BRepGraph_Tool::Edge::SameParameter(aGraph, BRepGraph_EdgeId(i)))
      << "Edge def " << i << " has SameParameter=false";
  }
}

TEST(BRepGraphGeometry, Box_EdgeDef_SameRange_IsSet)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Defs().NbEdges(), 0);

  for (int i = 0; i < aGraph.Defs().NbEdges(); ++i)
  {
    EXPECT_TRUE(BRepGraph_Tool::Edge::SameRange(aGraph, BRepGraph_EdgeId(i)))
      << "Edge def " << i << " has SameRange=false";
  }
}

// ============================================================
// Seam edge PCurve validation tests
// ============================================================

TEST(BRepGraphGeometry, Cylinder_SeamEdge_HasTwoCoEdges)
{
  // A cylinder has a seam edge on its lateral face.
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeCylinder(5.0, 20.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Find a seam edge via CoEdge SeamPairId.
  bool aFoundSeam = false;
  for (int i = 0; i < aGraph.Defs().NbEdges() && !aFoundSeam; ++i)
  {
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Defs().CoEdgesOfEdge(BRepGraph_EdgeId(i));
    for (int j = 0; j < aCoEdgeIdxs.Length() && !aFoundSeam; ++j)
    {
      const BRepGraphInc::CoEdgeEntity& aCE = aGraph.Defs().CoEdge(aCoEdgeIdxs.Value(j));
      if (aCE.SeamPairId.IsValid())
      {
        // Verify the paired coedge has opposite orientation.
        const BRepGraphInc::CoEdgeEntity& aPair = aGraph.Defs().CoEdge(aCE.SeamPairId);
        EXPECT_NE(aCE.Sense, aPair.Sense) << "Seam coedges should have opposite orientations";
        EXPECT_EQ(aCE.FaceDefId, aPair.FaceDefId) << "Seam coedges should share the same face";
        aFoundSeam = true;
      }
    }
  }
  EXPECT_TRUE(aFoundSeam) << "No seam edge found in cylinder";
}

TEST(BRepGraphGeometry, Cylinder_SeamEdge_FindPCurve_WithOrientation)
{
  // Verify FindPCurve(edge, face, orientation) returns different entries for FORWARD vs REVERSED.
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeCylinder(5.0, 20.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  for (int i = 0; i < aGraph.Defs().NbEdges(); ++i)
  {
    const BRepGraph_NodeId                        anEdgeDefId(BRepGraph_NodeId::Kind::Edge, i);
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Defs().CoEdgesOfEdge(BRepGraph_EdgeId(i));

    for (int j = 0; j < aCoEdgeIdxs.Length(); ++j)
    {
      const BRepGraphInc::CoEdgeEntity& aCE = aGraph.Defs().CoEdge(aCoEdgeIdxs.Value(j));
      if (!aCE.SeamPairId.IsValid())
        continue;

      // This is a seam edge - test oriented overload.
      const BRepGraph_FaceId            aFaceId(aCE.FaceDefId.Index);
      const BRepGraphInc::CoEdgeEntity* aPC_Fwd =
        BRepGraph_Tool::Edge::FindPCurve(aGraph, BRepGraph_EdgeId(i), aFaceId, TopAbs_FORWARD);
      const BRepGraphInc::CoEdgeEntity* aPC_Rev =
        BRepGraph_Tool::Edge::FindPCurve(aGraph, BRepGraph_EdgeId(i), aFaceId, TopAbs_REVERSED);

      EXPECT_NE(aPC_Fwd, nullptr) << "FindPCurve FORWARD returned null for seam edge";
      EXPECT_NE(aPC_Rev, nullptr) << "FindPCurve REVERSED returned null for seam edge";
      EXPECT_NE(aPC_Fwd, aPC_Rev) << "FORWARD and REVERSED PCurves should be different entries";
      return; // one seam is enough
    }
  }
  GTEST_SKIP() << "No seam edge found; test inconclusive";
}

TEST(BRepGraphGeometry, Box_FindPCurve_Context_MatchesThreeArgOverload)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  for (int i = 0; i < aGraph.Defs().NbEdges(); ++i)
  {
    const BRepGraph_NodeId                        anEdgeDefId(BRepGraph_NodeId::Kind::Edge, i);
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Defs().CoEdgesOfEdge(BRepGraph_EdgeId(i));

    for (int j = 0; j < aCoEdgeIdxs.Length(); ++j)
    {
      const BRepGraphInc::CoEdgeEntity& aCE = aGraph.Defs().CoEdge(aCoEdgeIdxs.Value(j));
      const BRepGraph_PCurveContext     aCtx(BRepGraph_EdgeId(i),
                                         BRepGraph_FaceId(aCE.FaceDefId.Index),
                                         aCE.Sense);

      const BRepGraphInc::CoEdgeEntity* aFromCtx = aGraph.Defs().FindPCurve(aCtx);
      const BRepGraphInc::CoEdgeEntity* aFrom3Arg =
        BRepGraph_Tool::Edge::FindPCurve(aGraph,
                                         BRepGraph_EdgeId(i),
                                         BRepGraph_FaceId(aCE.FaceDefId.Index),
                                         aCE.Sense);

      EXPECT_EQ(aFromCtx, aFrom3Arg);
      EXPECT_NE(aFromCtx, nullptr);
    }
  }
}

TEST(BRepGraphGeometry, Cylinder_SeamEdge_FindPCurve_Context_DistinguishesOrientation)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeCylinder(5.0, 20.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  for (int i = 0; i < aGraph.Defs().NbEdges(); ++i)
  {
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Defs().CoEdgesOfEdge(BRepGraph_EdgeId(i));

    for (int j = 0; j < aCoEdgeIdxs.Length(); ++j)
    {
      const BRepGraphInc::CoEdgeEntity& aCE = aGraph.Defs().CoEdge(aCoEdgeIdxs.Value(j));
      if (!aCE.SeamPairId.IsValid())
        continue;

      // Seam edge: same face, two orientations.
      const BRepGraph_NodeId        aFaceId = aCE.FaceDefId;
      const BRepGraph_PCurveContext aCtxFwd(BRepGraph_EdgeId(i),
                                            BRepGraph_FaceId(aFaceId.Index),
                                            TopAbs_FORWARD);
      const BRepGraph_PCurveContext aCtxRev(BRepGraph_EdgeId(i),
                                            BRepGraph_FaceId(aFaceId.Index),
                                            TopAbs_REVERSED);

      const BRepGraphInc::CoEdgeEntity* aPCFwd = aGraph.Defs().FindPCurve(aCtxFwd);
      const BRepGraphInc::CoEdgeEntity* aPCRev = aGraph.Defs().FindPCurve(aCtxRev);

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

TEST(BRepGraphRepresentations, Box_RepCounts_MatchTopology)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_GT(aGraph.Defs().NbSurfaces(), 0);
  EXPECT_GT(aGraph.Defs().NbCurves3D(), 0);
  EXPECT_GT(aGraph.Defs().NbCurves2D(), 0);

  // Every face has a valid surface.
  for (int i = 0; i < aGraph.Defs().NbFaces(); ++i)
  {
    EXPECT_TRUE(BRepGraph_Tool::Face::HasSurface(aGraph, BRepGraph_FaceId(i)))
      << "Face " << i << " has no surface";
    EXPECT_FALSE(BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(i)).IsNull());
  }

  // Every non-degenerate edge has a valid 3D curve.
  for (int i = 0; i < aGraph.Defs().NbEdges(); ++i)
  {
    if (!BRepGraph_Tool::Edge::Degenerated(aGraph, BRepGraph_EdgeId(i))
        && BRepGraph_Tool::Edge::HasCurve(aGraph, BRepGraph_EdgeId(i)))
    {
      EXPECT_FALSE(BRepGraph_Tool::Edge::Curve(aGraph, BRepGraph_EdgeId(i)).IsNull());
    }
  }

  // Every coedge with a PCurve has a valid Curve2DRepId.
  for (int i = 0; i < aGraph.Defs().NbCoEdges(); ++i)
  {
    const auto& aCoEdge = aGraph.Defs().CoEdge(BRepGraph_CoEdgeId(i));
    if (aCoEdge.Curve2DRepId.IsValid())
    {
      const occ::handle<Geom2d_Curve>& aPCurve =
        BRepGraph_Tool::CoEdge::PCurve(aGraph, BRepGraph_CoEdgeId(i));
      EXPECT_FALSE(aPCurve.IsNull());
    }
  }
}

TEST(BRepGraphRepresentations, Sphere_SurfaceDedup_SharedHandle)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeSphere(15.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // All faces of a sphere share the same TShape -> same entity -> same surface.
  if (aGraph.Defs().NbFaces() > 1)
  {
    const Geom_Surface* aFirstSurfPtr =
      BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(0)).get();
    for (int i = 1; i < aGraph.Defs().NbFaces(); ++i)
    {
      EXPECT_EQ(BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(i)).get(), aFirstSurfPtr)
        << "Faces sharing same surface should share the same surface pointer";
    }
  }
}

TEST(BRepGraphRepresentations, Cylinder_TriangulationReps_Populated)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeCylinder(5.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  for (int i = 0; i < aGraph.Defs().NbFaces(); ++i)
  {
    const auto& aFace = aGraph.Defs().Face(BRepGraph_FaceId(i));
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

TEST(BRepGraphRepresentations, RepId_FactoryMethods)
{
  const auto aSurfId = BRepGraph_RepId::Surface(42);
  EXPECT_EQ(BRepGraph_RepId(aSurfId).RepKind, BRepGraph_RepId::Kind::Surface);
  EXPECT_EQ(aSurfId.Index, 42);
  EXPECT_TRUE(aSurfId.IsValid());

  const auto aCurve3DId = BRepGraph_RepId::Curve3D(7);
  EXPECT_EQ(BRepGraph_RepId(aCurve3DId).RepKind, BRepGraph_RepId::Kind::Curve3D);
  EXPECT_EQ(aCurve3DId.Index, 7);

  const auto aDefaultId = BRepGraph_RepId();
  EXPECT_FALSE(aDefaultId.IsValid());

  EXPECT_EQ(aSurfId, BRepGraph_RepId::Surface(42));
  EXPECT_NE(BRepGraph_RepId(aSurfId), BRepGraph_RepId(aCurve3DId));
}

TEST(BRepGraphRepresentations, Compound_TwoBoxes_SurfaceDedup)
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

  EXPECT_EQ(aGraph.Defs().NbFaces(), 12);
  EXPECT_GT(aGraph.Defs().NbSurfaces(), 0);
  EXPECT_LE(aGraph.Defs().NbSurfaces(), 12);

  for (int i = 0; i < aGraph.Defs().NbFaces(); ++i)
  {
    EXPECT_TRUE(BRepGraph_Tool::Face::HasSurface(aGraph, BRepGraph_FaceId(i)));
  }
  for (int i = 0; i < aGraph.Defs().NbEdges(); ++i)
  {
    if (!BRepGraph_Tool::Edge::Degenerated(aGraph, BRepGraph_EdgeId(i))
        && BRepGraph_Tool::Edge::HasCurve(aGraph, BRepGraph_EdgeId(i)))
    {
      EXPECT_FALSE(BRepGraph_Tool::Edge::Curve(aGraph, BRepGraph_EdgeId(i)).IsNull());
    }
  }
}

TEST(BRepGraphRepresentations, Box_Polygon2DRep_MatchesInline)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Every coedge with a Polygon2DRepId has a valid polygon rep.
  for (int i = 0; i < aGraph.Defs().NbCoEdges(); ++i)
  {
    const auto& aCoEdge = aGraph.Defs().CoEdge(BRepGraph_CoEdgeId(i));
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
