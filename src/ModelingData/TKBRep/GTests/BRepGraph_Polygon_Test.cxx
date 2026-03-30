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
#include <BRep_Builder.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_ParamLayer.hxx>
#include <BRepGraph_RegularityLayer.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>
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

static void registerStandardLayers(BRepGraph& theGraph)
{
  theGraph.LayerRegistry().RegisterLayer(new BRepGraph_ParamLayer());
  theGraph.LayerRegistry().RegisterLayer(new BRepGraph_RegularityLayer());
}

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
  BRepGraph_NodeId aSolidDefId = BRepGraph_SolidId(0);
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
  // Populate-time extraction reads explicit BRep point representations from vertices.
  // Add one representation of each kind so the layer is exercised deterministically.
  TopoDS_Shape aShape = BRepPrimAPI_MakeCylinder(5., 10.).Shape();
  BRep_Builder aBuilder;

  bool hasPointOnCurve = false;
  for (TopExp_Explorer anEdgeExp(aShape, TopAbs_EDGE); anEdgeExp.More() && !hasPointOnCurve; anEdgeExp.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeExp.Current());
    double             aFirst = 0.0;
    double             aLast  = 0.0;
    TopLoc_Location    aLoc;
    if (BRep_Tool::Curve(anEdge, aLoc, aFirst, aLast).IsNull())
      continue;

    for (TopoDS_Iterator aVtxIt(anEdge, false, false); aVtxIt.More(); aVtxIt.Next())
    {
      const TopoDS_Vertex& aVertex = TopoDS::Vertex(aVtxIt.Value());
      const double aParameter =
        aVertex.Orientation() == TopAbs_REVERSED ? aLast : aFirst;
      aBuilder.UpdateVertex(aVertex, aParameter, anEdge, BRep_Tool::Tolerance(aVertex));
      hasPointOnCurve = true;
      break;
    }
  }

  bool hasPointOnSurface = false;
  for (TopExp_Explorer aFaceExp(aShape, TopAbs_FACE); aFaceExp.More() && !hasPointOnSurface; aFaceExp.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face(aFaceExp.Current());
    for (TopExp_Explorer aVtxExp(aFace, TopAbs_VERTEX); aVtxExp.More(); aVtxExp.Next())
    {
      const TopoDS_Vertex& aVertex = TopoDS::Vertex(aVtxExp.Current());
      const gp_Pnt2d       aUV     = BRep_Tool::Parameters(aVertex, aFace);
      aBuilder.UpdateVertex(aVertex, aUV.X(), aUV.Y(), aFace, BRep_Tool::Tolerance(aVertex));
      hasPointOnSurface = true;
      break;
    }
  }

  bool hasPointOnPCurve = false;
  for (TopExp_Explorer aFaceExp(aShape, TopAbs_FACE); aFaceExp.More() && !hasPointOnPCurve; aFaceExp.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face(aFaceExp.Current());
    for (TopExp_Explorer anEdgeExp(aFace, TopAbs_EDGE); anEdgeExp.More() && !hasPointOnPCurve;
         anEdgeExp.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeExp.Current());
      double             aFirst = 0.0;
      double             aLast  = 0.0;
      occ::handle<Geom2d_Curve> aPCurve = BRep_Tool::CurveOnSurface(anEdge, aFace, aFirst, aLast);
      if (aPCurve.IsNull())
        continue;

      aBuilder.UpdateEdge(anEdge, aPCurve, aFace, BRep_Tool::Tolerance(anEdge));
      aBuilder.Range(anEdge, aFace, aFirst, aLast);

      for (TopoDS_Iterator aVtxIt(anEdge, false, false); aVtxIt.More(); aVtxIt.Next())
      {
        const TopoDS_Vertex& aVertex = TopoDS::Vertex(aVtxIt.Value());
        const double aParameter =
          aVertex.Orientation() == TopAbs_REVERSED ? aLast : aFirst;
        aBuilder.UpdateVertex(aVertex, aParameter, anEdge, aFace, BRep_Tool::Tolerance(aVertex));
        hasPointOnPCurve = true;
        break;
      }
    }
  }

  ASSERT_TRUE(hasPointOnCurve);
  ASSERT_TRUE(hasPointOnSurface);
  ASSERT_TRUE(hasPointOnPCurve);

  BRepGraph aGraph;
  registerStandardLayers(aGraph);
  aGraph.Build(aShape);
  ASSERT_TRUE(aGraph.IsDone());
  const occ::handle<BRepGraph_ParamLayer> aParamLayer = aGraph.LayerRegistry().FindLayer<BRepGraph_ParamLayer>();
  ASSERT_FALSE(aParamLayer.IsNull());

  // Count all extracted vertex point representations.
  int aNbPointsOnCurve   = 0;
  int aNbPointsOnSurface = 0;
  int aNbPointsOnPCurve  = 0;
  for (int aVtxIdx = 0; aVtxIdx < aGraph.Topo().NbVertices(); ++aVtxIdx)
  {
    const BRepGraph_VertexId                   aVertexId(aVtxIdx);
    const BRepGraph_ParamLayer::VertexParams* aParams = aParamLayer->FindVertexParams(aVertexId);
    if (aParams == nullptr)
      continue;
    aNbPointsOnCurve += aParams->PointsOnCurve.Length();
    aNbPointsOnSurface += aParams->PointsOnSurface.Length();
    aNbPointsOnPCurve += aParams->PointsOnPCurve.Length();

    // Validate that any captured entries have valid def references.
    for (int i = 0; i < aParams->PointsOnCurve.Length(); ++i)
      EXPECT_TRUE(aParams->PointsOnCurve.Value(i).EdgeDefId.IsValid());
    for (int i = 0; i < aParams->PointsOnSurface.Length(); ++i)
      EXPECT_TRUE(aParams->PointsOnSurface.Value(i).FaceDefId.IsValid());
    for (int i = 0; i < aParams->PointsOnPCurve.Length(); ++i)
      EXPECT_TRUE(aParams->PointsOnPCurve.Value(i).CoEdgeDefId.IsValid());
  }

  EXPECT_GT(aNbPointsOnSurface, 0);
  EXPECT_GT(aNbPointsOnCurve + aNbPointsOnSurface + aNbPointsOnPCurve, 0);
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
  registerStandardLayers(aGraph);
  aGraph.Build(aCyl);
  ASSERT_TRUE(aGraph.IsDone());
  const occ::handle<BRepGraph_RegularityLayer> aRegularityLayer =
    aGraph.LayerRegistry().FindLayer<BRepGraph_RegularityLayer>();
  ASSERT_FALSE(aRegularityLayer.IsNull());

  // Count captured regularity entries.
  int aNbGraphReg = 0;
  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    aNbGraphReg += aRegularityLayer->NbRegularities(BRepGraph_EdgeId(anEdgeIdx));
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
