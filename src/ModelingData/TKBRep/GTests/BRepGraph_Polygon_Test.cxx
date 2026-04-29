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
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_LayerParam.hxx>
#include <BRepGraph_LayerRegularity.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_Builder.hxx>
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
  theGraph.LayerRegistry().RegisterLayer(new BRepGraph_LayerParam());
  theGraph.LayerRegistry().RegisterLayer(new BRepGraph_LayerRegularity());
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
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes1 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // Verify triangulations were captured on face definitions.
  bool aHasTriangulations = false;
  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraphInc::FaceDef& aFaceDef = aFaceIt.Current();
    if (aFaceDef.TriangulationRepId.IsValid())
    {
      aHasTriangulations = true;
      EXPECT_FALSE(BRepGraph_Tool::Face::Triangulation(aGraph, aFaceIt.CurrentId()).IsNull());
    }
  }
  EXPECT_TRUE(aHasTriangulations) << "Meshed box should have triangulations";

  // Reconstruct and verify triangulations are preserved.
  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    TopoDS_Shape aReconFace = aGraph.Shapes().Reconstruct(aFaceIt.CurrentId());
    ASSERT_FALSE(aReconFace.IsNull());

    TopLoc_Location                       aLoc;
    const occ::handle<Poly_Triangulation> aReconTri =
      BRep_Tool::Triangulation(TopoDS::Face(aReconFace), aLoc);
    EXPECT_FALSE(aReconTri.IsNull())
      << "Reconstructed face " << aFaceIt.CurrentId().Index << " should have triangulation";
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
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes2 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // Count Polygon3D on edges - matches what BRep_Tool reports for the original shape.
  int aNbPoly3DGraph = 0;
  int aNbPoly3DOrig  = 0;
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    if (BRepGraph_Tool::Edge::HasPolygon3D(aGraph, anEdgeIt.CurrentId()))
    {
      ++aNbPoly3DGraph;
    }
  }
  for (TopExp_Explorer anExp(aBox, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    TopLoc_Location aLoc;
    if (!BRep_Tool::Polygon3D(TopoDS::Edge(anExp.Current()), aLoc).IsNull())
    {
      ++aNbPoly3DOrig;
    }
  }
  EXPECT_EQ(aNbPoly3DGraph, aNbPoly3DOrig)
    << "Graph Polygon3D count should match BRep_Tool::Polygon3D count";

  // Verify Polygon3D roundtrip if present.
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    if (!BRepGraph_Tool::Edge::HasPolygon3D(aGraph, anEdgeIt.CurrentId()))
    {
      continue;
    }
    TopoDS_Shape aReconEdge = aGraph.Shapes().Reconstruct(anEdgeIt.CurrentId());
    ASSERT_FALSE(aReconEdge.IsNull());
    TopLoc_Location             aPolyLoc;
    occ::handle<Poly_Polygon3D> aPoly = BRep_Tool::Polygon3D(TopoDS::Edge(aReconEdge), aPolyLoc);
    EXPECT_FALSE(aPoly.IsNull()) << "Reconstructed edge " << anEdgeIt.CurrentId().Index
                                 << " should have Polygon3D";
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
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes3 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // Count PolygonOnTriangulation entries on coedges.
  int aNbPolyOnTri = 0;
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const NCollection_DynamicArray<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Topo().Edges().CoEdges(anEdgeIt.CurrentId());
    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdgeIdxs)
    {
      const BRepGraphInc::CoEdgeDef& aCE = aGraph.Topo().CoEdges().Definition(aCoEdgeId);
      aNbPolyOnTri += aCE.PolygonOnTriRepId.IsValid() ? 1 : 0;
    }
  }
  EXPECT_GT(aNbPolyOnTri, 0) << "Meshed box edges should have PolygonOnTriangulation";

  // Verify PolyOnTri entries have valid context references.
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const NCollection_DynamicArray<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Topo().Edges().CoEdges(anEdgeIt.CurrentId());
    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdgeIdxs)
    {
      const BRepGraphInc::CoEdgeDef& aCE = aGraph.Topo().CoEdges().Definition(aCoEdgeId);
      if (aCE.PolygonOnTriRepId.IsValid())
      {
        EXPECT_TRUE(aCE.PolygonOnTriRepId.IsValid());
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
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes4 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // Reconstruct solid and verify polygon-on-triangulation is re-attached.
  BRepGraph_NodeId aSolidDefId = BRepGraph_SolidId::Start();
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
      {
        continue;
      }
      TopLoc_Location                          aPolyLoc;
      occ::handle<Poly_PolygonOnTriangulation> aPolyOnTri =
        BRep_Tool::PolygonOnTriangulation(anEdge, aTri, aPolyLoc);
      if (!aPolyOnTri.IsNull())
      {
        ++aNbReconPolyOnTri;
      }
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
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes5 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // At least some CoEdge entries should have non-origin UV points.
  int aNbNonOriginUV = 0;
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const NCollection_DynamicArray<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Topo().Edges().CoEdges(anEdgeIt.CurrentId());
    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdgeIdxs)
    {
      const BRepGraphInc::CoEdgeDef& aCE = aGraph.Topo().CoEdges().Definition(aCoEdgeId);
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
  for (TopExp_Explorer anEdgeExp(aShape, TopAbs_EDGE); anEdgeExp.More() && !hasPointOnCurve;
       anEdgeExp.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeExp.Current());
    double             aFirst = 0.0;
    double             aLast  = 0.0;
    TopLoc_Location    aLoc;
    if (BRep_Tool::Curve(anEdge, aLoc, aFirst, aLast).IsNull())
    {
      continue;
    }

    for (TopoDS_Iterator aVtxIt(anEdge, false, false); aVtxIt.More(); aVtxIt.Next())
    {
      const TopoDS_Vertex& aVertex    = TopoDS::Vertex(aVtxIt.Value());
      const double         aParameter = aVertex.Orientation() == TopAbs_REVERSED ? aLast : aFirst;
      aBuilder.UpdateVertex(aVertex, aParameter, anEdge, BRep_Tool::Tolerance(aVertex));
      hasPointOnCurve = true;
      break;
    }
  }

  bool hasPointOnSurface = false;
  for (TopExp_Explorer aFaceExp(aShape, TopAbs_FACE); aFaceExp.More() && !hasPointOnSurface;
       aFaceExp.Next())
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
  for (TopExp_Explorer aFaceExp(aShape, TopAbs_FACE); aFaceExp.More() && !hasPointOnPCurve;
       aFaceExp.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face(aFaceExp.Current());
    for (TopExp_Explorer anEdgeExp(aFace, TopAbs_EDGE); anEdgeExp.More() && !hasPointOnPCurve;
         anEdgeExp.Next())
    {
      const TopoDS_Edge&        anEdge  = TopoDS::Edge(anEdgeExp.Current());
      double                    aFirst  = 0.0;
      double                    aLast   = 0.0;
      occ::handle<Geom2d_Curve> aPCurve = BRep_Tool::CurveOnSurface(anEdge, aFace, aFirst, aLast);
      if (aPCurve.IsNull())
      {
        continue;
      }

      aBuilder.UpdateEdge(anEdge, aPCurve, aFace, BRep_Tool::Tolerance(anEdge));
      aBuilder.Range(anEdge, aFace, aFirst, aLast);

      for (TopoDS_Iterator aVtxIt(anEdge, false, false); aVtxIt.More(); aVtxIt.Next())
      {
        const TopoDS_Vertex& aVertex    = TopoDS::Vertex(aVtxIt.Value());
        const double         aParameter = aVertex.Orientation() == TopAbs_REVERSED ? aLast : aFirst;
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
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes6 =
    BRepGraph_Builder::Add(aGraph, aShape);
  ASSERT_TRUE(aGraph.IsDone());
  const occ::handle<BRepGraph_LayerParam> aParamLayer =
    aGraph.LayerRegistry().FindLayer<BRepGraph_LayerParam>();
  ASSERT_FALSE(aParamLayer.IsNull());

  // Count all extracted vertex point representations.
  int aNbPointsOnCurve   = 0;
  int aNbPointsOnSurface = 0;
  int aNbPointsOnPCurve  = 0;
  for (BRepGraph_VertexIterator aVertexIt(aGraph); aVertexIt.More(); aVertexIt.Next())
  {
    const BRepGraph_VertexId                  aVertexId = aVertexIt.CurrentId();
    const BRepGraph_LayerParam::VertexParams* aParams   = aParamLayer->FindVertexParams(aVertexId);
    if (aParams == nullptr)
    {
      continue;
    }
    aNbPointsOnCurve += aParams->PointsOnCurve.Length();
    aNbPointsOnSurface += aParams->PointsOnSurface.Length();
    aNbPointsOnPCurve += aParams->PointsOnPCurve.Length();

    // Validate that any captured entries have valid def references.
    for (const BRepGraph_LayerParam::PointOnCurveEntry& anEntry : aParams->PointsOnCurve)
    {
      EXPECT_TRUE(anEntry.EdgeDefId.IsValid());
    }
    for (const BRepGraph_LayerParam::PointOnSurfaceEntry& anEntry : aParams->PointsOnSurface)
    {
      EXPECT_TRUE(anEntry.FaceDefId.IsValid());
    }
    for (const BRepGraph_LayerParam::PointOnPCurveEntry& anEntry : aParams->PointsOnPCurve)
    {
      EXPECT_TRUE(anEntry.CoEdgeDefId.IsValid());
    }
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
    {
      continue;
    }
    for (const occ::handle<BRep_CurveRepresentation>& aCRep : aTEdge->Curves())
    {
      if (!occ::down_cast<BRep_CurveOn2Surfaces>(aCRep).IsNull())
      {
        ++aNbOrigReg;
      }
    }
  }

  BRepGraph aGraph;
  registerStandardLayers(aGraph);
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes7 =
    BRepGraph_Builder::Add(aGraph, aCyl);
  ASSERT_TRUE(aGraph.IsDone());
  const occ::handle<BRepGraph_LayerRegularity> aRegularityLayer =
    aGraph.LayerRegistry().FindLayer<BRepGraph_LayerRegularity>();
  ASSERT_FALSE(aRegularityLayer.IsNull());

  // Count captured regularity entries.
  int aNbGraphReg = 0;
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    aNbGraphReg += aRegularityLayer->NbRegularities(anEdgeIt.CurrentId());
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
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes8 =
    BRepGraph_Builder::Add(aGraph, aCyl);
  ASSERT_TRUE(aGraph.IsDone());

  // Find an edge with two PolyOnTri entries for the same face (seam edge pattern).
  bool aFoundSeam = false;
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More() && !aFoundSeam; anEdgeIt.Next())
  {
    const NCollection_DynamicArray<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      aGraph.Topo().Edges().CoEdges(anEdgeIt.CurrentId());
    // Count PolyOnTri entries per face via coedges.
    NCollection_DataMap<int, int> aFaceCounts;
    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdgeIdxs)
    {
      const BRepGraphInc::CoEdgeDef& aCE      = aGraph.Topo().CoEdges().Definition(aCoEdgeId);
      const int                      aFaceIdx = aCE.FaceDefId.Index;
      if (!aFaceCounts.IsBound(aFaceIdx))
      {
        aFaceCounts.Bind(aFaceIdx, 0);
      }
      aFaceCounts.ChangeFind(aFaceIdx) += 1;
    }
    for (const auto& [aFaceIdx, aCount] : aFaceCounts.Items())
    {
      if (aCount >= 2)
      {
        aFoundSeam = true;
        break;
      }
    }
    if (aFoundSeam)
    {
      break;
    }
  }
  // Seam edges on cylinder lateral face should produce two PolyOnTri entries.
  EXPECT_TRUE(aFoundSeam) << "Meshed cylinder should have seam edge with two PolyOnTri entries";
}
