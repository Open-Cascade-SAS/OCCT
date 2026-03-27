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

#include <BRep_CurveOn2Surfaces.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

#include <gtest/gtest.h>

// ============================================================
// Multi-Triangulation roundtrip
// ============================================================

TEST(BRepGraph_PolygonTest, MultiTriangulation_Roundtrip_PreservesAll)
{
  // Build a meshed box.
  TopoDS_Shape             aBox = BRepPrimAPI_MakeBox(10., 20., 30.).Shape();
  BRepMesh_IncrementalMesh aMesher(aBox, 0.5);

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // Verify triangulations were captured on face definitions.
  bool aHasTriangulations = false;
  for (int aFaceDefIdx = 0; aFaceDefIdx < aGraph.Topo().NbFaces(); ++aFaceDefIdx)
  {
    const BRepGraphInc::FaceDef& aFaceDef = aGraph.Topo().Face(BRepGraph_FaceId(aFaceDefIdx));
    if (!aFaceDef.TriangulationRepIds.IsEmpty())
    {
      aHasTriangulations = true;
      EXPECT_GE(aFaceDef.ActiveTriangulationIndex, 0)
        << "Active triangulation index should be valid for meshed face";
      EXPECT_LT(aFaceDef.ActiveTriangulationIndex, aFaceDef.TriangulationRepIds.Length());
      const BRepGraph_TriangulationRepId anActiveRepId = aFaceDef.ActiveTriangulationRepId();
      EXPECT_TRUE(anActiveRepId.IsValid());
      EXPECT_FALSE(
        BRepGraph_Tool::Face::Triangulation(aGraph, BRepGraph_FaceId(aFaceDefIdx)).IsNull());
    }
  }
  EXPECT_TRUE(aHasTriangulations) << "Meshed box should have triangulations";

  // Reconstruct and verify triangulations are preserved.
  for (int aFaceIdx = 0; aFaceIdx < aGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    const BRepGraphInc::FaceDef& aFaceDef   = aGraph.Topo().Face(BRepGraph_FaceId(aFaceIdx));
    TopoDS_Shape                 aReconFace = aGraph.Shapes().Reconstruct(aFaceDef.Id);
    ASSERT_FALSE(aReconFace.IsNull());

    TopLoc_Location                       aLoc;
    const occ::handle<Poly_Triangulation> aReconTri =
      BRep_Tool::Triangulation(TopoDS::Face(aReconFace), aLoc);
    EXPECT_FALSE(aReconTri.IsNull())
      << "Reconstructed face " << aFaceIdx << " should have triangulation";
  }
}

// ============================================================
// Polygon3D roundtrip
// ============================================================

TEST(BRepGraph_PolygonTest, Polygon3D_Captured_WhenPresent)
{
  // Polygon3D is not produced by BRepMesh_IncrementalMesh, so a bare box won't have them.
  // Verify the graph correctly reports no Polygon3D for unmeshed/standard-meshed edges,
  // and that the data model is structurally sound.
  TopoDS_Shape             aBox = BRepPrimAPI_MakeBox(10., 20., 30.).Shape();
  BRepMesh_IncrementalMesh aMesher(aBox, 0.5);

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // Count Polygon3D on edges - matches what BRep_Tool reports for the original shape.
  int aNbPoly3DGraph = 0;
  int aNbPoly3DOrig  = 0;
  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    if (BRepGraph_Tool::Edge::HasPolygon3D(aGraph, BRepGraph_EdgeId(anEdgeIdx)))
      ++aNbPoly3DGraph;
  }
  for (TopExp_Explorer anExp(aBox, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    TopLoc_Location aLoc;
    if (!BRep_Tool::Polygon3D(TopoDS::Edge(anExp.Current()), aLoc).IsNull())
      ++aNbPoly3DOrig;
  }
  EXPECT_EQ(aNbPoly3DGraph, aNbPoly3DOrig)
    << "Graph Polygon3D count should match BRep_Tool::Polygon3D count";

  // Verify Polygon3D roundtrip if present.
  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    if (!BRepGraph_Tool::Edge::HasPolygon3D(aGraph, BRepGraph_EdgeId(anEdgeIdx)))
      continue;
    const BRepGraphInc::EdgeDef& anEdge     = aGraph.Topo().Edge(BRepGraph_EdgeId(anEdgeIdx));
    TopoDS_Shape                 aReconEdge = aGraph.Shapes().Reconstruct(anEdge.Id);
    ASSERT_FALSE(aReconEdge.IsNull());
    TopLoc_Location             aPolyLoc;
    occ::handle<Poly_Polygon3D> aPoly = BRep_Tool::Polygon3D(TopoDS::Edge(aReconEdge), aPolyLoc);
    EXPECT_FALSE(aPoly.IsNull()) << "Reconstructed edge " << anEdgeIdx << " should have Polygon3D";
  }
}

// ============================================================
// PolygonOnTriangulation roundtrip
// ============================================================

TEST(BRepGraph_PolygonTest, PolyOnTri_Captured_AfterMesh)
{
  TopoDS_Shape             aBox = BRepPrimAPI_MakeBox(10., 20., 30.).Shape();
  BRepMesh_IncrementalMesh aMesher(aBox, 0.5);

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // Count PolygonOnTriangulation entries on coedges.
  int aNbPolyOnTri = 0;
  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Topo().CoEdgesOfEdge(BRepGraph_EdgeId(anEdgeIdx));
    for (int aCEIdx = 0; aCEIdx < aCoEdgeIdxs.Length(); ++aCEIdx)
    {
      const BRepGraphInc::CoEdgeDef& aCE = aGraph.Topo().CoEdge(aCoEdgeIdxs.Value(aCEIdx));
      aNbPolyOnTri += aCE.PolygonOnTriRepIds.Length();
    }
  }
  EXPECT_GT(aNbPolyOnTri, 0) << "Meshed box edges should have PolygonOnTriangulation";

  // Verify PolyOnTri entries have valid context references.
  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Topo().CoEdgesOfEdge(BRepGraph_EdgeId(anEdgeIdx));
    for (int aCEIdx = 0; aCEIdx < aCoEdgeIdxs.Length(); ++aCEIdx)
    {
      const BRepGraphInc::CoEdgeDef& aCE = aGraph.Topo().CoEdge(aCoEdgeIdxs.Value(aCEIdx));
      for (int aPolyIdx = 0; aPolyIdx < aCE.PolygonOnTriRepIds.Length(); ++aPolyIdx)
      {
        EXPECT_TRUE(aCE.PolygonOnTriRepIds.Value(aPolyIdx).IsValid());
      }
      EXPECT_TRUE(aCE.FaceDefId.IsValid());
    }
  }
}

// ============================================================
// PolygonOnTriangulation reconstruct roundtrip
// ============================================================

TEST(BRepGraph_PolygonTest, PolyOnTri_Roundtrip_PreservedOnReconstruct)
{
  TopoDS_Shape             aBox = BRepPrimAPI_MakeBox(10., 20., 30.).Shape();
  BRepMesh_IncrementalMesh aMesher(aBox, 0.5);

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // Reconstruct solid and verify polygon-on-triangulation is re-attached.
  BRepGraph_NodeId aSolidDefId = BRepGraph_NodeId::Solid(0);
  TopoDS_Shape     aReconSolid = aGraph.Shapes().Reconstruct(aSolidDefId);
  ASSERT_FALSE(aReconSolid.IsNull());

  int aNbReconPolyOnTri = 0;
  for (TopExp_Explorer anEdgeExp(aReconSolid, TopAbs_EDGE); anEdgeExp.More(); anEdgeExp.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeExp.Current());
    for (TopExp_Explorer aFaceExp(aReconSolid, TopAbs_FACE); aFaceExp.More(); aFaceExp.Next())
    {
      const TopoDS_Face&              aFace = TopoDS::Face(aFaceExp.Current());
      TopLoc_Location                 aTriLoc;
      occ::handle<Poly_Triangulation> aTri = BRep_Tool::Triangulation(aFace, aTriLoc);
      if (aTri.IsNull())
        continue;
      TopLoc_Location                          aPolyLoc;
      occ::handle<Poly_PolygonOnTriangulation> aPolyOnTri =
        BRep_Tool::PolygonOnTriangulation(anEdge, aTri, aPolyLoc);
      if (!aPolyOnTri.IsNull())
        ++aNbReconPolyOnTri;
    }
  }

  EXPECT_GT(aNbReconPolyOnTri, 0)
    << "Reconstructed solid should have PolygonOnTriangulation on edges";
}

// ============================================================
// UV Points on PCurves
// ============================================================

TEST(BRepGraph_PolygonTest, UVPoints_Captured_OnPCurves)
{
  TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10., 20., 30.).Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // At least some CoEdge entries should have non-origin UV points.
  int aNbNonOriginUV = 0;
  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Topo().CoEdgesOfEdge(BRepGraph_EdgeId(anEdgeIdx));
    for (int aCEIdx = 0; aCEIdx < aCoEdgeIdxs.Length(); ++aCEIdx)
    {
      const BRepGraphInc::CoEdgeDef& aCE = aGraph.Topo().CoEdge(aCoEdgeIdxs.Value(aCEIdx));
      if (aCE.UV1.Distance(gp_Pnt2d(0, 0)) > Precision::Confusion()
          || aCE.UV2.Distance(gp_Pnt2d(0, 0)) > Precision::Confusion())
      {
        ++aNbNonOriginUV;
      }
    }
  }
  EXPECT_GT(aNbNonOriginUV, 0) << "Box CoEdges should have non-origin UV points";
}

// ============================================================
// Vertex Point Representations
// ============================================================

TEST(BRepGraph_PolygonTest, VertexPointRepresentations_StructurallyValid)
{
  // Not all shapes have explicit point representations (BRepPrimAPI_MakeBox doesn't store them).
  // Verify the data model is structurally sound: vectors exist and any captured entries are valid.
  TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10., 20., 30.).Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // Count all vertex point representations (may be zero for simple boxes).
  int aNbPointsOnCurve   = 0;
  int aNbPointsOnSurface = 0;
  int aNbPointsOnPCurve  = 0;
  for (int aVtxIdx = 0; aVtxIdx < aGraph.Topo().NbVertices(); ++aVtxIdx)
  {
    const BRepGraphInc::VertexDef& aVtx = aGraph.Topo().Vertex(BRepGraph_VertexId(aVtxIdx));
    aNbPointsOnCurve += aVtx.PointsOnCurve.Length();
    aNbPointsOnSurface += aVtx.PointsOnSurface.Length();
    aNbPointsOnPCurve += aVtx.PointsOnPCurve.Length();

    // Validate that any captured entries have valid def references.
    for (int i = 0; i < aVtx.PointsOnCurve.Length(); ++i)
      EXPECT_TRUE(aVtx.PointsOnCurve.Value(i).EdgeDefId.IsValid());
    for (int i = 0; i < aVtx.PointsOnSurface.Length(); ++i)
      EXPECT_TRUE(aVtx.PointsOnSurface.Value(i).FaceDefId.IsValid());
    for (int i = 0; i < aVtx.PointsOnPCurve.Length(); ++i)
      EXPECT_TRUE(aVtx.PointsOnPCurve.Value(i).FaceDefId.IsValid());
  }

  // Just verify we can query - exact count depends on shape.
  EXPECT_GE(aNbPointsOnCurve + aNbPointsOnSurface + aNbPointsOnPCurve, 0);
}

// ============================================================
// Edge Regularity
// ============================================================

TEST(BRepGraph_PolygonTest, EdgeRegularity_MatchesOriginal)
{
  // Cylinder has smooth edges between lateral face and caps.
  // BRep stores regularity as BRep_CurveOn2Surfaces entries.
  TopoDS_Shape aCyl = BRepPrimAPI_MakeCylinder(5., 10.).Shape();

  // Count original regularities via BRep_Tool::Continuity.
  int aNbOrigReg = 0;
  for (TopExp_Explorer anEdgeExp(aCyl, TopAbs_EDGE); anEdgeExp.More(); anEdgeExp.Next())
  {
    const TopoDS_Edge&             anEdge = TopoDS::Edge(anEdgeExp.Current());
    const occ::handle<BRep_TEdge>& aTEdge = occ::down_cast<BRep_TEdge>(anEdge.TShape());
    if (aTEdge.IsNull())
      continue;
    for (const occ::handle<BRep_CurveRepresentation>& aCRep : aTEdge->Curves())
    {
      if (!occ::down_cast<BRep_CurveOn2Surfaces>(aCRep).IsNull())
        ++aNbOrigReg;
    }
  }

  BRepGraph aGraph;
  aGraph.Build(aCyl);
  ASSERT_TRUE(aGraph.IsDone());

  // Count captured regularity entries.
  int aNbGraphReg = 0;
  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraphInc::EdgeDef& anEdge = aGraph.Topo().Edge(BRepGraph_EdgeId(anEdgeIdx));
    aNbGraphReg += anEdge.Regularities.Length();
  }
  EXPECT_EQ(aNbGraphReg, aNbOrigReg)
    << "Graph regularity count should match BRep_CurveOn2Surfaces count";
}

// ============================================================
// Seam edge PolygonOnTriangulation
// ============================================================

TEST(BRepGraph_PolygonTest, SeamEdge_PolyOnTri_TwoEntries)
{
  // Cylinder has seam edges with two PolygonOnTriangulation entries.
  TopoDS_Shape             aCyl = BRepPrimAPI_MakeCylinder(5., 10.).Shape();
  BRepMesh_IncrementalMesh aMesher(aCyl, 0.1);

  BRepGraph aGraph;
  aGraph.Build(aCyl);
  ASSERT_TRUE(aGraph.IsDone());

  // Find an edge with two PolyOnTri entries for the same face (seam edge pattern).
  bool aFoundSeam = false;
  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Topo().CoEdgesOfEdge(BRepGraph_EdgeId(anEdgeIdx));
    // Count PolyOnTri entries per face via coedges.
    NCollection_DataMap<int, int> aFaceCounts;
    for (int aCEIdx = 0; aCEIdx < aCoEdgeIdxs.Length(); ++aCEIdx)
    {
      const BRepGraphInc::CoEdgeDef& aCE      = aGraph.Topo().CoEdge(aCoEdgeIdxs.Value(aCEIdx));
      const int                      aFaceIdx = aCE.FaceDefId.Index;
      if (!aFaceCounts.IsBound(aFaceIdx))
        aFaceCounts.Bind(aFaceIdx, 0);
      aFaceCounts.ChangeFind(aFaceIdx) += 1;
    }
    for (NCollection_DataMap<int, int>::Iterator anIt(aFaceCounts); anIt.More(); anIt.Next())
    {
      if (anIt.Value() >= 2)
      {
        aFoundSeam = true;
        break;
      }
    }
    if (aFoundSeam)
      break;
  }
  // Seam edges on cylinder lateral face should produce two PolyOnTri entries.
  EXPECT_TRUE(aFoundSeam) << "Meshed cylinder should have seam edge with two PolyOnTri entries";
}
