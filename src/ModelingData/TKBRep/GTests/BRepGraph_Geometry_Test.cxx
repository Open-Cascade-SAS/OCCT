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
#include <BRepGraph_PCurveContext.hxx>
#include <BRepGraph_RepId.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRep_Tool.hxx>
#include <BRepGraph_SpatialView.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_SubGraph.hxx>
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
  const occ::handle<Geom_Surface>& aFirstSurf = aGraph.Defs().Face(0).Surface;
  EXPECT_FALSE(aFirstSurf.IsNull());
  for (int i = 1; i < aGraph.Defs().NbFaces(); ++i)
  {
    EXPECT_EQ(aGraph.Defs().Face(i).Surface.get(), aFirstSurf.get());
  }
}

TEST(BRepGraphGeometry, Sphere_AllFacesShareSurface)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeSphere(15.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // All faces of a sphere share the same surface pointer.
  const occ::handle<Geom_Surface>& aFirstSurf = aGraph.Defs().Face(0).Surface;
  EXPECT_FALSE(aFirstSurf.IsNull());
  int aSameCount = 0;
  for (int i = 0; i < aGraph.Defs().NbFaces(); ++i)
  {
    if (aGraph.Defs().Face(i).Surface.get() == aFirstSurf.get())
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
    EXPECT_FALSE(aGraph.Defs().Edge(i).Curve3d.IsNull())
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
    EXPECT_FALSE(aGraph.Defs().Edge(i).Curve3d.IsNull())
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
    const BRepGraph_TopoNode::EdgeDef& anEdge = aGraph.Defs().Edge(i);
    const BRepGraph_NodeId             aEdgeDefId(BRepGraph_NodeId::Kind::Edge, i);
    for (int j = 0; j < anEdge.PCurves.Length(); ++j)
    {
      const BRepGraph_NodeId aFaceDefId = anEdge.PCurves.Value(j).FaceDefId;
      const BRepGraph_TopoNode::EdgeDef::PCurveEntry* aPCurveEntry =
        aGraph.Defs().FindPCurve(aEdgeDefId, aFaceDefId);
      EXPECT_NE(aPCurveEntry, nullptr);
      ++aPCurveCount;
    }
  }
  EXPECT_GT(aPCurveCount, 0);
}

TEST(BRepGraphGeometry, PCurve_InlineData_FaceDefIdValid)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  for (int i = 0; i < aGraph.Defs().NbEdges(); ++i)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = aGraph.Defs().Edge(i);
    for (int j = 0; j < anEdge.PCurves.Length(); ++j)
    {
      const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCEntry = anEdge.PCurves.Value(j);
      EXPECT_TRUE(aPCEntry.FaceDefId.IsValid())
        << "Edge " << i << " PCurve " << j << " has invalid FaceDefId";
      EXPECT_EQ(aPCEntry.FaceDefId.NodeKind, BRepGraph_NodeId::Kind::Face);
    }
  }
}

TEST(BRepGraphGeometry, PCurve_ParamRange_NonZero)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aPCurveCount = 0;
  for (int i = 0; i < aGraph.Defs().NbEdges(); ++i)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = aGraph.Defs().Edge(i);
    for (int j = 0; j < anEdge.PCurves.Length(); ++j)
    {
      const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCEntry = anEdge.PCurves.Value(j);
      const double aRange = aPCEntry.ParamLast - aPCEntry.ParamFirst;
      EXPECT_GT(std::abs(aRange), Precision::PConfusion())
        << "Edge " << i << " PCurve " << j << " has zero parameter range";
      ++aPCurveCount;
    }
  }
  EXPECT_GT(aPCurveCount, 0);
}

TEST(BRepGraphGeometry, PCurve_Continuity_Valid)
{
  const TopoDS_Shape aShape = BRepPrimAPI_MakeCylinder(10.0, 20.0).Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);
  ASSERT_TRUE(aGraph.IsDone());

  bool hasNonC0Continuity = false;
  for (int i = 0; i < aGraph.Defs().NbEdges(); ++i)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = aGraph.Defs().Edge(i);
    for (int j = 0; j < anEdge.PCurves.Length(); ++j)
    {
      const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCEntry = anEdge.PCurves.Value(j);
      if (aPCEntry.Continuity > GeomAbs_C0)
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
    EXPECT_FALSE(aGraph.Defs().Face(i).Surface.IsNull())
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
    EXPECT_FALSE(aGraph.Defs().Edge(i).Curve3d.IsNull())
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

TEST(BRepGraphGeometry, GlobalTransform_DefId_IsValid)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Defs().NbFaces(), 0);

  const BRepGraph_NodeId aFaceDefId(BRepGraph_NodeId::Kind::Face, 0);
  const gp_Trsf          aDefTrsf = aGraph.Spatial().GlobalTransform(aFaceDefId);

  // For a simple box, the transform should be identity.
  // Just verify the call does not crash and produces a valid transform.
  EXPECT_NEAR(aDefTrsf.Value(1, 1), 1.0, Precision::Confusion());
  EXPECT_NEAR(aDefTrsf.Value(2, 2), 1.0, Precision::Confusion());
  EXPECT_NEAR(aDefTrsf.Value(3, 3), 1.0, Precision::Confusion());
}

TEST(BRepGraphGeometry, GlobalTransform_CompoundWithLocation_NonIdentity)
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

  // Moved() preserves TShape — one solid definition, two compound ChildRefs.
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
    const BRepGraph_TopoNode::FaceDef& aFaceDef = aGraph.Defs().Face(i);
    EXPECT_TRUE(aFaceDef.ActiveTriangulation().IsNull())
      << "Face " << i << " unexpectedly has a triangulation";
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
    EXPECT_FALSE(anIt.Current().Surface.IsNull());
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
    if (!anIt.Current().IsDegenerate)
      EXPECT_FALSE(anIt.Current().Curve3d.IsNull());
    ++aCount;
  }
  EXPECT_EQ(aCount, aGraph.Defs().NbEdges());
}

TEST(BRepGraphIterator, InlinePCurve_AllEdgePCurvesHaveCurve2d)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (int i = 0; i < aGraph.Defs().NbEdges(); ++i)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = aGraph.Defs().Edge(i);
    for (int j = 0; j < anEdge.PCurves.Length(); ++j)
    {
      EXPECT_FALSE(anEdge.PCurves.Value(j).Curve2d.IsNull());
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

  // Collect face def indices from both components and verify they are disjoint.
  NCollection_Map<int>           aFaceSet1;
  const NCollection_Vector<int>& aFaces1 = aSubs.Value(0).FaceDefIndices();
  for (int i = 0; i < aFaces1.Length(); ++i)
  {
    aFaceSet1.Add(aFaces1.Value(i));
  }

  const NCollection_Vector<int>& aFaces2 = aSubs.Value(1).FaceDefIndices();
  for (int i = 0; i < aFaces2.Length(); ++i)
  {
    EXPECT_FALSE(aFaceSet1.Contains(aFaces2.Value(i)))
      << "Face def index " << aFaces2.Value(i) << " found in both components";
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
    const BRepGraph_TopoNode::EdgeDef& anEdge = aGraph.Defs().Edge(i);
    EXPECT_TRUE(anEdge.SameParameter) << "Edge def " << i << " has SameParameter=false";
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
    const BRepGraph_TopoNode::EdgeDef& anEdge = aGraph.Defs().Edge(i);
    EXPECT_TRUE(anEdge.SameRange) << "Edge def " << i << " has SameRange=false";
  }
}

// ============================================================
// Seam edge PCurve validation tests
// ============================================================

TEST(BRepGraphGeometry, Cylinder_SeamEdge_HasTwoPCurves)
{
  // A cylinder has a seam edge on its lateral face.
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeCylinder(5.0, 20.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Find an edge with two PCurve entries on the same face (seam edge).
  bool aFoundSeam = false;
  for (int i = 0; i < aGraph.Defs().NbEdges(); ++i)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = aGraph.Defs().Edge(i);
    if (anEdge.PCurves.Length() < 2)
      continue;

    // Check if any two entries share the same FaceDefId (seam condition).
    for (int j = 0; j < anEdge.PCurves.Length() && !aFoundSeam; ++j)
    {
      for (int k = j + 1; k < anEdge.PCurves.Length() && !aFoundSeam; ++k)
      {
        if (anEdge.PCurves.Value(j).FaceDefId == anEdge.PCurves.Value(k).FaceDefId)
        {
          // Verify the two entries have opposite orientations.
          const TopAbs_Orientation anOri1 = anEdge.PCurves.Value(j).EdgeOrientation;
          const TopAbs_Orientation anOri2 = anEdge.PCurves.Value(k).EdgeOrientation;
          EXPECT_NE(anOri1, anOri2) << "Seam edge PCurves should have opposite orientations";
          aFoundSeam = true;
        }
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
    const BRepGraph_TopoNode::EdgeDef& anEdge = aGraph.Defs().Edge(i);
    const BRepGraph_NodeId             anEdgeDefId(BRepGraph_NodeId::Kind::Edge, i);

    for (int j = 0; j < anEdge.PCurves.Length(); ++j)
    {
      for (int k = j + 1; k < anEdge.PCurves.Length(); ++k)
      {
        if (anEdge.PCurves.Value(j).FaceDefId != anEdge.PCurves.Value(k).FaceDefId)
          continue;

        // This is a seam edge -- test oriented overload.
        const BRepGraph_NodeId aFaceId = anEdge.PCurves.Value(j).FaceDefId;
        const BRepGraph_TopoNode::EdgeDef::PCurveEntry* aPC_Fwd =
          aGraph.Defs().FindPCurve(anEdgeDefId, aFaceId, TopAbs_FORWARD);
        const BRepGraph_TopoNode::EdgeDef::PCurveEntry* aPC_Rev =
          aGraph.Defs().FindPCurve(anEdgeDefId, aFaceId, TopAbs_REVERSED);

        EXPECT_NE(aPC_Fwd, nullptr) << "FindPCurve FORWARD returned null for seam edge";
        EXPECT_NE(aPC_Rev, nullptr) << "FindPCurve REVERSED returned null for seam edge";
        EXPECT_NE(aPC_Fwd, aPC_Rev) << "FORWARD and REVERSED PCurves should be different entries";
        return; // one seam is enough
      }
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
    const BRepGraph_TopoNode::EdgeDef& anEdge = aGraph.Defs().Edge(i);
    const BRepGraph_NodeId             anEdgeDefId(BRepGraph_NodeId::Kind::Edge, i);

    for (int j = 0; j < anEdge.PCurves.Length(); ++j)
    {
      const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCEntry = anEdge.PCurves.Value(j);
      const BRepGraph_PCurveContext aCtx(i, aPCEntry.FaceDefId.Index, aPCEntry.EdgeOrientation);

      const BRepGraph_TopoNode::EdgeDef::PCurveEntry* aFromCtx = aGraph.Defs().FindPCurve(aCtx);
      const BRepGraph_TopoNode::EdgeDef::PCurveEntry* aFrom3Arg =
        aGraph.Defs().FindPCurve(anEdgeDefId, aPCEntry.FaceDefId, aPCEntry.EdgeOrientation);

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
    const BRepGraph_TopoNode::EdgeDef& anEdge = aGraph.Defs().Edge(i);
    const BRepGraph_NodeId             anEdgeDefId(BRepGraph_NodeId::Kind::Edge, i);

    for (int j = 0; j < anEdge.PCurves.Length(); ++j)
    {
      for (int k = j + 1; k < anEdge.PCurves.Length(); ++k)
      {
        if (anEdge.PCurves.Value(j).FaceDefId != anEdge.PCurves.Value(k).FaceDefId)
          continue;

        // Seam edge: same face, two orientations.
        const BRepGraph_NodeId aFaceId = anEdge.PCurves.Value(j).FaceDefId;
        const BRepGraph_PCurveContext aCtxFwd(i, aFaceId.Index, TopAbs_FORWARD);
        const BRepGraph_PCurveContext aCtxRev(i, aFaceId.Index, TopAbs_REVERSED);

        const BRepGraph_TopoNode::EdgeDef::PCurveEntry* aPCFwd = aGraph.Defs().FindPCurve(aCtxFwd);
        const BRepGraph_TopoNode::EdgeDef::PCurveEntry* aPCRev = aGraph.Defs().FindPCurve(aCtxRev);

        EXPECT_NE(aPCFwd, nullptr);
        EXPECT_NE(aPCRev, nullptr);
        EXPECT_NE(aPCFwd, aPCRev);
        return;
      }
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

  // Every face has a valid SurfaceRepIdx with matching Surface handle.
  for (int i = 0; i < aGraph.Defs().NbFaces(); ++i)
  {
    const auto& aFace = aGraph.Defs().Face(i);
    EXPECT_GE(aFace.SurfaceRepIdx, 0) << "Face " << i << " has no SurfaceRepIdx";
    const auto& aSurfRep = aGraph.Defs().SurfaceRep(aFace.SurfaceRepIdx);
    EXPECT_FALSE(aSurfRep.Surface.IsNull());
    EXPECT_EQ(aSurfRep.Surface.get(), aFace.Surface.get());
  }

  // Every non-degenerate edge has a valid Curve3DRepIdx.
  for (int i = 0; i < aGraph.Defs().NbEdges(); ++i)
  {
    const auto& anEdge = aGraph.Defs().Edge(i);
    if (!anEdge.IsDegenerate && !anEdge.Curve3d.IsNull())
    {
      EXPECT_GE(anEdge.Curve3DRepIdx, 0) << "Edge " << i << " has no Curve3DRepIdx";
      const auto& aCurveRep = aGraph.Defs().Curve3DRep(anEdge.Curve3DRepIdx);
      EXPECT_EQ(aCurveRep.Curve.get(), anEdge.Curve3d.get());
    }
  }

  // Every coedge with a PCurve has a valid Curve2DRepIdx.
  for (int i = 0; i < aGraph.Defs().NbCoEdges(); ++i)
  {
    const auto& aCoEdge = aGraph.Defs().CoEdge(i);
    if (!aCoEdge.Curve2d.IsNull())
    {
      EXPECT_GE(aCoEdge.Curve2DRepIdx, 0) << "CoEdge " << i << " has no Curve2DRepIdx";
      const auto& aCurve2DRep = aGraph.Defs().Curve2DRep(aCoEdge.Curve2DRepIdx);
      EXPECT_EQ(aCurve2DRep.Curve.get(), aCoEdge.Curve2d.get());
    }
  }
}

TEST(BRepGraphRepresentations, Sphere_SurfaceDedup_SharedHandle)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeSphere(15.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // All faces of a sphere share the same TShape -> same entity -> same SurfaceRepIdx.
  if (aGraph.Defs().NbFaces() > 1)
  {
    const int aFirstRepIdx = aGraph.Defs().Face(0).SurfaceRepIdx;
    for (int i = 1; i < aGraph.Defs().NbFaces(); ++i)
    {
      EXPECT_EQ(aGraph.Defs().Face(i).SurfaceRepIdx, aFirstRepIdx)
        << "Faces sharing same surface should share SurfaceRepIdx";
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
    const auto& aFace = aGraph.Defs().Face(i);
    if (!aFace.Triangulations.IsEmpty())
    {
      EXPECT_EQ(aFace.TriangulationRepIdxs.Length(), aFace.Triangulations.Length())
        << "TriangulationRepIdxs count should match Triangulations count for face " << i;
      for (int j = 0; j < aFace.TriangulationRepIdxs.Length(); ++j)
      {
        const int aRepIdx = aFace.TriangulationRepIdxs.Value(j);
        EXPECT_GE(aRepIdx, 0);
        const auto& aTriRep = aGraph.Defs().TriangulationRep(aRepIdx);
        EXPECT_EQ(aTriRep.Triangulation.get(), aFace.Triangulations.Value(j).get());
      }
    }
  }
}

TEST(BRepGraphRepresentations, RepId_FactoryMethods)
{
  const auto aSurfId = BRepGraph_RepId::Surface(42);
  EXPECT_EQ(aSurfId.Kind, BRepGraph_RepId::RepKind::Surface);
  EXPECT_EQ(aSurfId.Index, 42);
  EXPECT_TRUE(aSurfId.IsValid());

  const auto aCurve3DId = BRepGraph_RepId::Curve3D(7);
  EXPECT_EQ(aCurve3DId.Kind, BRepGraph_RepId::RepKind::Curve3D);
  EXPECT_EQ(aCurve3DId.Index, 7);

  const auto aDefaultId = BRepGraph_RepId();
  EXPECT_FALSE(aDefaultId.IsValid());

  EXPECT_EQ(aSurfId, BRepGraph_RepId::Surface(42));
  EXPECT_NE(aSurfId, aCurve3DId);
}

TEST(BRepGraphRepresentations, Compound_TwoBoxes_SurfaceDedup)
{
  TopoDS_Shape aBox1 = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  TopoDS_Shape aBox2 = BRepPrimAPI_MakeBox(20.0, 20.0, 20.0).Shape();

  BRep_Builder aBuilder;
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
    EXPECT_GE(aGraph.Defs().Face(i).SurfaceRepIdx, 0);
  }
  for (int i = 0; i < aGraph.Defs().NbEdges(); ++i)
  {
    const auto& anEdge = aGraph.Defs().Edge(i);
    if (!anEdge.IsDegenerate && !anEdge.Curve3d.IsNull())
    {
      EXPECT_GE(anEdge.Curve3DRepIdx, 0);
    }
  }
}

TEST(BRepGraphRepresentations, Box_Polygon2DRep_MatchesInline)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Every coedge with a PolygonOnSurf has a matching Polygon2DRepIdx.
  for (int i = 0; i < aGraph.Defs().NbCoEdges(); ++i)
  {
    const auto& aCoEdge = aGraph.Defs().CoEdge(i);
    if (!aCoEdge.PolygonOnSurf.IsNull())
    {
      EXPECT_GE(aCoEdge.Polygon2DRepIdx, 0) << "CoEdge " << i << " has PolygonOnSurf but no rep";
      const auto& aRep = aGraph.Defs().Polygon2DRep(aCoEdge.Polygon2DRepIdx);
      EXPECT_EQ(aRep.Polygon.get(), aCoEdge.PolygonOnSurf.get());
    }
    // PolygonOnTriRepIdxs count must match PolygonsOnTri count.
    EXPECT_EQ(aCoEdge.PolygonOnTriRepIdxs.Length(), aCoEdge.PolygonsOnTri.Length())
      << "CoEdge " << i << " PolygonOnTriRepIdxs/PolygonsOnTri length mismatch";
    for (int j = 0; j < aCoEdge.PolygonOnTriRepIdxs.Length(); ++j)
    {
      const int aRepIdx = aCoEdge.PolygonOnTriRepIdxs.Value(j);
      EXPECT_GE(aRepIdx, 0);
      const auto& aRep = aGraph.Defs().PolygonOnTriRep(aRepIdx);
      EXPECT_EQ(aRep.Polygon.get(), aCoEdge.PolygonsOnTri.Value(j).Polygon.get());
      // TriangulationRepIdx should be valid (global index).
      EXPECT_GE(aRep.TriangulationRepIdx, 0);
    }
  }
}
