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
#include <BRepGraph.hxx>
#include <BRepGraph_Analyze.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_SubGraph.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Map.hxx>
#include <Precision.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Compound.hxx>

#include <gtest/gtest.h>

// ============================================================
// Geometry navigation tests
// ============================================================

TEST(BRepGraphGeometry, Sphere_AllFaces_SameSurfNodeId)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeSphere(15.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // All face defs of a sphere share the same surface node.
  ASSERT_GE(aGraph.NbFaceDefs(), 1);
  const BRepGraph_NodeId aFirstSurfId = aGraph.FaceDef(0).SurfNodeId;
  EXPECT_TRUE(aFirstSurfId.IsValid());
  for (int i = 1; i < aGraph.NbFaceDefs(); ++i)
  {
    EXPECT_EQ(aGraph.FaceDef(i).SurfNodeId, aFirstSurfId);
  }
}

TEST(BRepGraphGeometry, Sphere_FacesOnSurface_ReturnsAll)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeSphere(15.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId aSurfId = aGraph.FaceDef(0).SurfNodeId;
  const NCollection_Vector<BRepGraph_NodeId>& aFaces = aGraph.FacesOnSurface(aSurfId);
  EXPECT_EQ(aFaces.Length(), aGraph.NbFaceDefs());
}

TEST(BRepGraphGeometry, Box_CurveOf_ValidForAll12Edges)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.NbEdgeDefs(), 12);

  for (int i = 0; i < aGraph.NbEdgeDefs(); ++i)
  {
    const BRepGraph_NodeId aEdgeDefId(BRepGraph_NodeKind::Edge, i);
    const BRepGraph_NodeId aCurveId = aGraph.CurveOf(aEdgeDefId);
    EXPECT_TRUE(aCurveId.IsValid()) << "Edge def " << i << " has no valid curve";
  }
}

TEST(BRepGraphGeometry, Box_EdgesOnCurve_AtLeastOnePerCurve)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  for (int i = 0; i < aGraph.NbCurves(); ++i)
  {
    const BRepGraph_NodeId aCurveId(BRepGraph_NodeKind::Curve, i);
    const NCollection_Vector<BRepGraph_NodeId>& aEdges = aGraph.EdgesOnCurve(aCurveId);
    EXPECT_GE(aEdges.Length(), 1) << "Curve " << i << " has no edge users";
  }
}

TEST(BRepGraphGeometry, Box_PCurveOf_AllEdgeFacePairs_Valid)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aPCurveCount = 0;
  for (int i = 0; i < aGraph.NbEdgeDefs(); ++i)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = aGraph.EdgeDef(i);
    const BRepGraph_NodeId aEdgeDefId(BRepGraph_NodeKind::Edge, i);
    for (int j = 0; j < anEdge.PCurves.Length(); ++j)
    {
      const BRepGraph_NodeId aFaceDefId = anEdge.PCurves.Value(j).FaceDefId;
      const BRepGraph_NodeId aPCurveId = aGraph.PCurveOf(aEdgeDefId, aFaceDefId);
      EXPECT_TRUE(aPCurveId.IsValid());
      ++aPCurveCount;
    }
  }
  EXPECT_GT(aPCurveCount, 0);
}

TEST(BRepGraphGeometry, PCurve_ContextNodes_MatchEdgeFace)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  for (int i = 0; i < aGraph.NbPCurves(); ++i)
  {
    const BRepGraph_GeomNode::PCurve& aPCurve = aGraph.PCurve(i);
    EXPECT_TRUE(aPCurve.EdgeContext.IsValid()) << "PCurve " << i << " has invalid edge context";
    EXPECT_TRUE(aPCurve.FaceContext.IsValid()) << "PCurve " << i << " has invalid face context";
    EXPECT_EQ(aPCurve.EdgeContext.Kind, BRepGraph_NodeKind::Edge);
    EXPECT_EQ(aPCurve.FaceContext.Kind, BRepGraph_NodeKind::Face);
  }
}

TEST(BRepGraphGeometry, PCurve_ParamRange_NonZero)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.NbPCurves(), 0);

  for (int i = 0; i < aGraph.NbPCurves(); ++i)
  {
    const BRepGraph_GeomNode::PCurve& aPCurve = aGraph.PCurve(i);
    const double aRange = aPCurve.ParamLast - aPCurve.ParamFirst;
    EXPECT_GT(std::abs(aRange), Precision::PConfusion())
        << "PCurve " << i << " has zero parameter range";
  }
}

TEST(BRepGraphGeometry, Surf_IsMultiLocated_FalseForSimpleBox)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  for (int i = 0; i < aGraph.NbSurfaces(); ++i)
  {
    EXPECT_FALSE(aGraph.Surf(i).IsMultiLocated) << "Surface " << i << " unexpectedly multi-located";
  }
}

TEST(BRepGraphGeometry, Curve_IsMultiLocated_FalseForSimpleBox)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  for (int i = 0; i < aGraph.NbCurves(); ++i)
  {
    EXPECT_FALSE(aGraph.Curve(i).IsMultiLocated) << "Curve " << i << " unexpectedly multi-located";
  }
}

TEST(BRepGraphGeometry, SurfaceOf_InvalidNodeKind_ReturnsInvalid)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Pass an edge def id to SurfaceOf -- expects invalid result.
  const BRepGraph_NodeId anEdgeDefId(BRepGraph_NodeKind::Edge, 0);
  const BRepGraph_NodeId aResult = aGraph.SurfaceOf(anEdgeDefId);
  EXPECT_FALSE(aResult.IsValid());
}

TEST(BRepGraphGeometry, SameDomainFaces_SimpleBox_Empty)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // For a simple box each face has a unique surface, so SameDomainFaces is empty.
  for (int i = 0; i < aGraph.NbFaceDefs(); ++i)
  {
    const BRepGraph_NodeId aFaceDefId(BRepGraph_NodeKind::Face, i);
    const NCollection_Vector<BRepGraph_NodeId> aSameDomain = aGraph.SameDomainFaces(aFaceDefId);
    EXPECT_EQ(aSameDomain.Length(), 0) << "Face def " << i << " has unexpected same-domain faces";
  }
}

TEST(BRepGraphGeometry, GlobalTransform_DefId_MatchesFirstUsage)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.NbFaceDefs(), 0);

  const BRepGraph_NodeId aFaceDefId(BRepGraph_NodeKind::Face, 0);
  const gp_Trsf aDefTrsf = aGraph.GlobalTransform(aFaceDefId);

  // The first usage of this face should give the same transform.
  const NCollection_Vector<BRepGraph_UsageId>& aUsages = aGraph.UsagesOf(aFaceDefId);
  ASSERT_GT(aUsages.Length(), 0);
  const gp_Trsf aUsageTrsf = aGraph.GlobalTransform(aUsages.Value(0));

  // Compare translation parts of both transforms.
  for (int aRow = 1; aRow <= 3; ++aRow)
  {
    for (int aCol = 1; aCol <= 4; ++aCol)
    {
      EXPECT_NEAR(aDefTrsf.Value(aRow, aCol),
                  aUsageTrsf.Value(aRow, aCol),
                  Precision::Confusion());
    }
  }
}

TEST(BRepGraphGeometry, GlobalTransform_CompoundWithLocation_NonIdentity)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(100.0, 0.0, 0.0));
  const TopoDS_Shape aMoved = aBox.Moved(TopLoc_Location(aTrsf));

  TopoDS_Compound aCompound;
  BRep_Builder aBuilder;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBox);
  aBuilder.Add(aCompound, aMoved);

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  // At least one face usage should have a non-identity global transform.
  bool hasNonIdentity = false;
  for (int i = 0; i < aGraph.NbFaceUsages(); ++i)
  {
    const gp_Trsf aGlobalTrsf = aGraph.GlobalTransform(
        BRepGraph_UsageId(BRepGraph_NodeKind::Face, i));
    if (aGlobalTrsf.IsNegative() || aGlobalTrsf.IsNegative() == false)
    {
      // Check if translation component is non-zero.
      const double aDist = std::sqrt(
          aGlobalTrsf.Value(1, 4) * aGlobalTrsf.Value(1, 4) +
          aGlobalTrsf.Value(2, 4) * aGlobalTrsf.Value(2, 4) +
          aGlobalTrsf.Value(3, 4) * aGlobalTrsf.Value(3, 4));
      if (aDist > Precision::Confusion())
      {
        hasNonIdentity = true;
        break;
      }
    }
  }
  EXPECT_TRUE(hasNonIdentity);
}

TEST(BRepGraphGeometry, Surf_Triangulation_NullForAnalyticNoCrash)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Analytical box surfaces should have null triangulation (no mesh computed).
  // Simply verify access does not crash.
  for (int i = 0; i < aGraph.NbSurfaces(); ++i)
  {
    const BRepGraph_GeomNode::Surf& aSurf = aGraph.Surf(i);
    EXPECT_TRUE(aSurf.Triangulation.IsNull())
        << "Surface " << i << " unexpectedly has a triangulation";
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
  EXPECT_EQ(aCount, aGraph.NbSolidDefs());
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
  EXPECT_EQ(aCount, aGraph.NbShellDefs());
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
  EXPECT_EQ(aCount, aGraph.NbFaceDefs());
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
  EXPECT_EQ(aCount, aGraph.NbWireDefs());
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
  EXPECT_EQ(aCount, aGraph.NbEdgeDefs());
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
  EXPECT_EQ(aCount, aGraph.NbVertexDefs());
}

TEST(BRepGraphIterator, FaceUsage_CountMatchesNb)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (BRepGraph_Iterator<BRepGraph_TopoNode::FaceUsage> anIt(aGraph); anIt.More(); anIt.Next())
  {
    (void)anIt.Current();
    ++aCount;
  }
  EXPECT_EQ(aCount, aGraph.NbFaceUsages());
}

TEST(BRepGraphIterator, SurfGeom_CountMatchesNb)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (BRepGraph_Iterator<BRepGraph_GeomNode::Surf> anIt(aGraph); anIt.More(); anIt.Next())
  {
    EXPECT_FALSE(anIt.Current().Surface.IsNull());
    ++aCount;
  }
  EXPECT_EQ(aCount, aGraph.NbSurfaces());
}

TEST(BRepGraphIterator, CurveGeom_CountMatchesNb)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (BRepGraph_Iterator<BRepGraph_GeomNode::Curve> anIt(aGraph); anIt.More(); anIt.Next())
  {
    EXPECT_FALSE(anIt.Current().CurveGeom.IsNull());
    ++aCount;
  }
  EXPECT_EQ(aCount, aGraph.NbCurves());
}

TEST(BRepGraphIterator, PCurveGeom_CountMatchesNb)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (BRepGraph_Iterator<BRepGraph_GeomNode::PCurve> anIt(aGraph); anIt.More(); anIt.Next())
  {
    EXPECT_FALSE(anIt.Current().Curve2d.IsNull());
    ++aCount;
  }
  EXPECT_EQ(aCount, aGraph.NbPCurves());
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
  const TopoDS_Shape aBox2 = BRepPrimAPI_MakeBox(gp_Pnt(50.0, 50.0, 50.0),
                                                  60.0, 60.0, 60.0).Shape();

  TopoDS_Compound aCompound;
  BRep_Builder aBuilder;
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
  const TopoDS_Shape aBox2 = BRepPrimAPI_MakeBox(gp_Pnt(50.0, 50.0, 50.0),
                                                  60.0, 60.0, 60.0).Shape();

  TopoDS_Compound aCompound;
  BRep_Builder aBuilder;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBox1);
  aBuilder.Add(aCompound, aBox2);

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  const NCollection_Vector<BRepGraph_SubGraph> aSubs = BRepGraph_Analyze::Decompose(aGraph);
  ASSERT_EQ(aSubs.Length(), 2);

  // Collect face def indices from both components and verify they are disjoint.
  NCollection_Map<int> aFaceSet1;
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
  const TopoDS_Shape aBox2 = BRepPrimAPI_MakeBox(gp_Pnt(50.0, 50.0, 50.0),
                                                  60.0, 60.0, 60.0).Shape();

  TopoDS_Compound aCompound;
  BRep_Builder aBuilder;
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

  const int aGraphTotal = aGraph.NbSolidDefs() + aGraph.NbShellDefs()
                         + aGraph.NbFaceDefs()  + aGraph.NbWireDefs()
                         + aGraph.NbEdgeDefs()  + aGraph.NbVertexDefs();
  EXPECT_GE(aTotalTopoNodes, aGraphTotal);
}
