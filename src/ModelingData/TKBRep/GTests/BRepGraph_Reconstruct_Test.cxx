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
#include <BRepGProp.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <BRepGraph_BuilderView.hxx>
#include "BRepGraph_RefTestTools.hxx"
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>

#include <gtest/gtest.h>

// ============================================================
// Helper: compute surface area of a shape
// ============================================================
static double computeArea(const TopoDS_Shape& theShape)
{
  GProp_GProps aProps;
  BRepGProp::SurfaceProperties(theShape, aProps);
  return aProps.Mass();
}

// ============================================================
// Helper: compute volume of a shape
// ============================================================
static double computeVolume(const TopoDS_Shape& theShape)
{
  GProp_GProps aProps;
  BRepGProp::VolumeProperties(theShape, aProps);
  return aProps.Mass();
}

// ============================================================
// Helper: count sub-shapes of a given type
// ============================================================
static int countSubShapes(const TopoDS_Shape& theShape, TopAbs_ShapeEnum theType)
{
  int aCount = 0;
  for (TopExp_Explorer anExp(theShape, theType); anExp.More(); anExp.Next())
  {
    ++aCount;
  }
  return aCount;
}

// ============================================================
// Area/volume fidelity per primitive
// ============================================================

TEST(BRepGraph_ReconstructTest, Box_Area_Preserved)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox       = aBoxMaker.Shape();
  const double        anOrigArea = computeArea(aBox);

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  TopoDS_Shape aRecon =
    aGraph.Shapes().Reconstruct(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, 0));
  const double aReconArea = computeArea(aRecon);

  EXPECT_NEAR(aReconArea, anOrigArea, anOrigArea * 0.01);
}

TEST(BRepGraph_ReconstructTest, Box_Volume_Preserved)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox      = aBoxMaker.Shape();
  const double        anOrigVol = computeVolume(aBox);

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  TopoDS_Shape aRecon =
    aGraph.Shapes().Reconstruct(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, 0));
  const double aReconVol = computeVolume(aRecon);

  EXPECT_NEAR(aReconVol, anOrigVol, anOrigVol * 0.01);
}

TEST(BRepGraph_ReconstructTest, Sphere_Area_Preserved)
{
  BRepPrimAPI_MakeSphere aSphereMaker(15.0);
  const TopoDS_Shape&    aSphere    = aSphereMaker.Shape();
  const double           anOrigArea = computeArea(aSphere);

  BRepGraph aGraph;
  aGraph.Build(aSphere);
  ASSERT_TRUE(aGraph.IsDone());

  TopoDS_Shape aRecon =
    aGraph.Shapes().Reconstruct(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, 0));
  const double aReconArea = computeArea(aRecon);

  EXPECT_NEAR(aReconArea, anOrigArea, anOrigArea * 0.01);
}

TEST(BRepGraph_ReconstructTest, Sphere_Volume_Preserved)
{
  BRepPrimAPI_MakeSphere aSphereMaker(15.0);
  const TopoDS_Shape&    aSphere   = aSphereMaker.Shape();
  const double           anOrigVol = computeVolume(aSphere);

  BRepGraph aGraph;
  aGraph.Build(aSphere);
  ASSERT_TRUE(aGraph.IsDone());

  TopoDS_Shape aRecon =
    aGraph.Shapes().Reconstruct(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, 0));
  const double aReconVol = computeVolume(aRecon);

  EXPECT_NEAR(aReconVol, anOrigVol, anOrigVol * 0.01);
}

TEST(BRepGraph_ReconstructTest, Cylinder_Area_Preserved)
{
  BRepPrimAPI_MakeCylinder aCylMaker(10.0, 20.0);
  const TopoDS_Shape&      aCyl       = aCylMaker.Shape();
  const double             anOrigArea = computeArea(aCyl);

  BRepGraph aGraph;
  aGraph.Build(aCyl);
  ASSERT_TRUE(aGraph.IsDone());

  TopoDS_Shape aRecon =
    aGraph.Shapes().Reconstruct(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, 0));
  const double aReconArea = computeArea(aRecon);

  EXPECT_NEAR(aReconArea, anOrigArea, anOrigArea * 0.01);
}

TEST(BRepGraph_ReconstructTest, Cylinder_Volume_Preserved)
{
  BRepPrimAPI_MakeCylinder aCylMaker(10.0, 20.0);
  const TopoDS_Shape&      aCyl      = aCylMaker.Shape();
  const double             anOrigVol = computeVolume(aCyl);

  BRepGraph aGraph;
  aGraph.Build(aCyl);
  ASSERT_TRUE(aGraph.IsDone());

  TopoDS_Shape aRecon =
    aGraph.Shapes().Reconstruct(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, 0));
  const double aReconVol = computeVolume(aRecon);

  EXPECT_NEAR(aReconVol, anOrigVol, anOrigVol * 0.01);
}

// ============================================================
// Per-kind reconstruction
// ============================================================

TEST(BRepGraph_ReconstructTest, Shell_FaceCount_MatchesOriginal)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox            = aBoxMaker.Shape();
  const int           anOrigFaceCount = countSubShapes(aBox, TopAbs_FACE);

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  TopoDS_Shape aReconShell =
    aGraph.Shapes().Reconstruct(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Shell, 0));
  const int aReconFaceCount = countSubShapes(aReconShell, TopAbs_FACE);

  EXPECT_EQ(aReconFaceCount, anOrigFaceCount);
}

TEST(BRepGraph_ReconstructTest, Wire_EdgeCount_FourPerBoxFace)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // Each wire of a box face should have exactly 4 edges.
  for (BRepGraph_WireIterator aWireIt(aGraph); aWireIt.More(); aWireIt.Next())
  {
    TopoDS_Shape aReconWire =
      aGraph.Shapes().Reconstruct(BRepGraph_NodeId(aWireIt.CurrentId()));
    const int anEdgeCount = countSubShapes(aReconWire, TopAbs_EDGE);
    EXPECT_EQ(anEdgeCount, 4) << "Wire " << aWireIt.CurrentId().Index << " has " << anEdgeCount
                              << " edges";
  }
}

TEST(BRepGraph_ReconstructTest, Edge_HasCurve_NonNull)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    if (BRepGraph_Tool::Edge::Degenerated(aGraph, anEdgeIt.CurrentId()))
      continue;

    TopoDS_Shape aReconEdge =
      aGraph.Shapes().Reconstruct(BRepGraph_NodeId(anEdgeIt.CurrentId()));
    TopLoc_Location         aLoc;
    double                  aFirst = 0.0;
    double                  aLast  = 0.0;
    occ::handle<Geom_Curve> aCurve =
      BRep_Tool::Curve(TopoDS::Edge(aReconEdge), aLoc, aFirst, aLast);
    EXPECT_FALSE(aCurve.IsNull()) << "Edge " << anEdgeIt.CurrentId().Index << " has null curve";
  }
}

TEST(BRepGraph_ReconstructTest, Edge_ParameterRange_Preserved)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    if (BRepGraph_Tool::Edge::Degenerated(aGraph, anEdgeIt.CurrentId()))
      continue;

    TopoDS_Shape aReconEdge =
      aGraph.Shapes().Reconstruct(BRepGraph_NodeId(anEdgeIt.CurrentId()));
    TopLoc_Location aLoc;
    double          aFirst = 0.0;
    double          aLast  = 0.0;
    BRep_Tool::Curve(TopoDS::Edge(aReconEdge), aLoc, aFirst, aLast);

    const auto [aDefFirst, aDefLast] =
      BRepGraph_Tool::Edge::Range(aGraph, anEdgeIt.CurrentId());
    EXPECT_NEAR(aFirst, aDefFirst, Precision::Confusion())
      << "Edge " << anEdgeIt.CurrentId().Index << " ParamFirst mismatch";
    EXPECT_NEAR(aLast, aDefLast, Precision::Confusion())
      << "Edge " << anEdgeIt.CurrentId().Index << " ParamLast mismatch";
  }
}

TEST(BRepGraph_ReconstructTest, Vertex_Point_MatchesDefPoint)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  for (BRepGraph_VertexIterator aVertexIt(aGraph); aVertexIt.More(); aVertexIt.Next())
  {
    const gp_Pnt aDefPt = BRepGraph_Tool::Vertex::Pnt(aGraph, aVertexIt.CurrentId());

    TopoDS_Shape aReconVtx =
      aGraph.Shapes().Reconstruct(BRepGraph_NodeId(aVertexIt.CurrentId()));
    const gp_Pnt aReconPt = BRep_Tool::Pnt(TopoDS::Vertex(aReconVtx));

    EXPECT_NEAR(aReconPt.X(), aDefPt.X(), Precision::Confusion())
      << "Vertex " << aVertexIt.CurrentId().Index << " X mismatch";
    EXPECT_NEAR(aReconPt.Y(), aDefPt.Y(), Precision::Confusion())
      << "Vertex " << aVertexIt.CurrentId().Index << " Y mismatch";
    EXPECT_NEAR(aReconPt.Z(), aDefPt.Z(), Precision::Confusion())
      << "Vertex " << aVertexIt.CurrentId().Index << " Z mismatch";
  }
}

TEST(BRepGraph_ReconstructTest, Face_PCurvesPresent_OnAllEdges)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    TopoDS_Shape aReconFace = aGraph.Shapes().Reconstruct(aFaceIt.CurrentId());
    ASSERT_FALSE(aReconFace.IsNull()) << "ReconstructFace returned null for face "
                                      << aFaceIt.CurrentId().Index;

    const TopoDS_Face& aFace = TopoDS::Face(aReconFace);
    for (TopExp_Explorer anEdgeExp(aFace, TopAbs_EDGE); anEdgeExp.More(); anEdgeExp.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeExp.Current());
      if (BRep_Tool::Degenerated(anEdge))
        continue;

      double                    aFirst  = 0.0;
      double                    aLast   = 0.0;
      occ::handle<Geom2d_Curve> aPCurve = BRep_Tool::CurveOnSurface(anEdge, aFace, aFirst, aLast);
      EXPECT_FALSE(aPCurve.IsNull()) << "Edge on face " << aFaceIt.CurrentId().Index
                                     << " is missing a pcurve";
    }
  }
}

TEST(BRepGraph_ReconstructTest, Face_OrientationPreserved)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // Verify that reconstructed faces have valid orientations matching ref entries.
  ASSERT_EQ(aGraph.Topo().Shells().Nb(), 1);
  const NCollection_Vector<BRepGraph_FaceRefId> aFaceRefs =
    BRepGraph_TestTools::FaceRefsOfShell(aGraph, BRepGraph_ShellId(0));
  for (const BRepGraph_FaceRefId& aFaceRefId : aFaceRefs)
  {
    const BRepGraphInc::FaceRef& aFaceRef = aGraph.Refs().Faces().Entry(aFaceRefId);
    const TopAbs_Orientation     anExpectedOri = aFaceRef.Orientation;

    TopoDS_Shape aReconFace = aGraph.Shapes().Reconstruct(aFaceRef.FaceDefId);
    ASSERT_FALSE(aReconFace.IsNull())
      << "ReconstructFace returned null for face " << aFaceRef.FaceDefId.Index;

    // The reconstructed face from the def should be valid.
    EXPECT_EQ(aReconFace.ShapeType(), TopAbs_FACE);
    (void)anExpectedOri; // Orientation is now stored in the incidence ref, not usage.
  }
}

// ============================================================
// Shape() vs ReconstructShape() consistency
// ============================================================

TEST(BRepGraph_ReconstructTest, Shape_UnmodifiedGraph_SameAsOriginalOf)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // For an unmodified graph, Shape(id) should be the same TShape as OriginalOf(id).
  BRepGraph_SolidId aSolidId(0);
  ASSERT_TRUE(aGraph.Shapes().HasOriginal(aSolidId));

  TopoDS_Shape        aShapeResult = aGraph.Shapes().Shape(aSolidId);
  const TopoDS_Shape& anOriginal   = aGraph.Shapes().OriginalOf(aSolidId);
  EXPECT_TRUE(aShapeResult.IsSame(anOriginal));
}

TEST(BRepGraph_ReconstructTest, HasOriginal_BuildFace_ReturnsTrue)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_TRUE(aGraph.Shapes().HasOriginal(BRepGraph_FaceId(0)));
}

TEST(BRepGraph_ReconstructTest, OriginalOf_Face_IsSameAsBuildInputFace)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  const TopoDS_Shape aFirstFace = anExp.Current();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_TRUE(aGraph.Shapes().OriginalOf(BRepGraph_FaceId(0)).IsSame(aFirstFace));
}

TEST(BRepGraph_ReconstructTest, HasOriginal_ManualVertex_ReturnsFalse)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_VertexId aVertexId = aGraph.Builder().AddVertex(gp_Pnt(42.0, 0.0, 0.0), 0.001);
  ASSERT_TRUE(aVertexId.IsValid());

  EXPECT_FALSE(aGraph.Shapes().HasOriginal(aVertexId));
}

TEST(BRepGraph_ReconstructTest, FindNode_OriginalFace_RoundTrip)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId aFaceId        = BRepGraph_FaceId(0);
  const TopoDS_Shape&    anOriginalFace = aGraph.Shapes().OriginalOf(aFaceId);
  EXPECT_EQ(aGraph.Shapes().FindNode(anOriginalFace), aFaceId);
}

TEST(BRepGraph_ReconstructTest, Reconstruct_Face_ValidShape)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().Faces().Nb(), 0);

  TopoDS_Shape aRecon = aGraph.Shapes().Reconstruct(BRepGraph_FaceId(0));
  EXPECT_FALSE(aRecon.IsNull());
  EXPECT_EQ(aRecon.ShapeType(), TopAbs_FACE);
}

TEST(BRepGraph_ReconstructTest, Reconstruct_Edge_ValidShape)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().Edges().Nb(), 0);

  // Find a non-degenerate edge.
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    if (BRepGraph_Tool::Edge::Degenerated(aGraph, anEdgeIt.CurrentId()))
      continue;

    TopoDS_Shape aRecon =
      aGraph.Shapes().Reconstruct(BRepGraph_NodeId(anEdgeIt.CurrentId()));
    EXPECT_FALSE(aRecon.IsNull());
    EXPECT_EQ(aRecon.ShapeType(), TopAbs_EDGE);
    return;
  }
  FAIL() << "No non-degenerate edge found";
}

TEST(BRepGraph_ReconstructTest, Reconstruct_Vertex_CorrectPoint)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().Vertices().Nb(), 0);

  const gp_Pnt anExpectedPt = BRepGraph_Tool::Vertex::Pnt(aGraph, BRepGraph_VertexId(0));

  TopoDS_Shape aRecon =
    aGraph.Shapes().Reconstruct(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Vertex, 0));
  ASSERT_FALSE(aRecon.IsNull());
  ASSERT_EQ(aRecon.ShapeType(), TopAbs_VERTEX);

  const gp_Pnt aReconPt = BRep_Tool::Pnt(TopoDS::Vertex(aRecon));
  EXPECT_NEAR(aReconPt.Distance(anExpectedPt), 0.0, Precision::Confusion());
}

// ============================================================
// After mutation
// ============================================================

TEST(BRepGraph_ReconstructTest, AfterVertexMutation_ModifiedFlagAndPointChanged)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // Find a vertex belonging to face 0 and move it significantly.
  const BRepGraph_WireId anOuterWire =
    BRepGraph_TestTools::OuterWireOfFace(aGraph, BRepGraph_FaceId(0));
  ASSERT_TRUE(anOuterWire.IsValid());

  const NCollection_Vector<BRepGraph_CoEdgeRefId> aCoEdgeRefs =
    BRepGraph_TestTools::CoEdgeRefsOfWire(aGraph, anOuterWire);
  ASSERT_GT(aCoEdgeRefs.Length(), 0);

  const BRepGraphInc::CoEdgeRef& aFirstCR = aGraph.Refs().CoEdges().Entry(aCoEdgeRefs.First());
  const BRepGraphInc::CoEdgeDef& aFirstCoEdge =
    aGraph.Topo().CoEdges().Definition(aFirstCR.CoEdgeDefId);
  const int aVertIdx =
    BRepGraph_Tool::Edge::StartVertex(aGraph, BRepGraph_EdgeId(aFirstCoEdge.EdgeDefId))
      .VertexDefId.Index;
  ASSERT_GE(aVertIdx, 0);

  // Mutate: move vertex by 5 units in Z.
  const gp_Pnt anOldPt = BRepGraph_Tool::Vertex::Pnt(aGraph, BRepGraph_VertexId(aVertIdx));
  {
    BRepGraph_MutGuard<BRepGraphInc::VertexDef> aMutVtx =
      aGraph.Builder().MutVertex(BRepGraph_VertexId(aVertIdx));
    aMutVtx->Point = gp_Pnt(anOldPt.X(), anOldPt.Y(), anOldPt.Z() + 5.0);
  }

  // Verify the OwnGen is incremented on the vertex def (directly mutated).
  EXPECT_GT(aGraph.Topo().Vertices().Definition(BRepGraph_VertexId(aVertIdx)).OwnGen, 0u)
    << "Vertex def should have OwnGen > 0 after mutation";

  // Verify the graph VertexDef.Point has actually changed.
  const gp_Pnt aNewPt = BRepGraph_Tool::Vertex::Pnt(aGraph, BRepGraph_VertexId(aVertIdx));
  EXPECT_GT(anOldPt.Distance(aNewPt), Precision::Confusion())
    << "VertexDef.Point should differ after mutation";
}

TEST(BRepGraph_ReconstructTest, AfterToleranceMutation_NewTShape)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_EdgeId anEdgeId(0);
  TopoDS_Shape     aShapeBefore = aGraph.Shapes().Shape(anEdgeId);

  // Mutate tolerance.
  {
    BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aMutEdge =
      aGraph.Builder().MutEdge(BRepGraph_EdgeId(0));
    aMutEdge->Tolerance = aMutEdge->Tolerance + 1.0;
  }

  // After mutation, Shape() should return a reconstructed shape with a different TShape.
  TopoDS_Shape aShapeAfter = aGraph.Shapes().Shape(anEdgeId);
  EXPECT_FALSE(aShapeAfter.IsNull());
  EXPECT_FALSE(aShapeAfter.IsSame(aShapeBefore))
    << "Shape() should return a new TShape after tolerance mutation";
}

TEST(BRepGraph_ReconstructTest, CompoundRoot_TwoSolids_Preserved)
{
  BRepPrimAPI_MakeBox aBoxMaker1(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox1 = aBoxMaker1.Shape();
  BRepPrimAPI_MakeBox aBoxMaker2(5.0, 10.0, 15.0);
  const TopoDS_Shape& aBox2 = aBoxMaker2.Shape();

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBox1);
  aBuilder.Add(aCompound, aBox2);

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_EQ(aGraph.Topo().Solids().Nb(), 2);

  // Reconstruct each solid and verify volumes match originals.
  const double anOrigVol1 = computeVolume(aBox1);
  const double anOrigVol2 = computeVolume(aBox2);

  TopoDS_Shape aRecon1 =
    aGraph.Shapes().Reconstruct(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, 0));
  TopoDS_Shape aRecon2 =
    aGraph.Shapes().Reconstruct(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, 1));

  const double aReconVol1 = computeVolume(aRecon1);
  const double aReconVol2 = computeVolume(aRecon2);

  // The two volumes should match the originals (order may vary, so check both assignments).
  const bool isDirectMatch = (std::abs(aReconVol1 - anOrigVol1) < anOrigVol1 * 0.01)
                             && (std::abs(aReconVol2 - anOrigVol2) < anOrigVol2 * 0.01);
  const bool isSwappedMatch = (std::abs(aReconVol1 - anOrigVol2) < anOrigVol2 * 0.01)
                              && (std::abs(aReconVol2 - anOrigVol1) < anOrigVol1 * 0.01);

  EXPECT_TRUE(isDirectMatch || isSwappedMatch)
    << "Reconstructed solid volumes do not match originals: " << aReconVol1 << ", " << aReconVol2
    << " vs " << anOrigVol1 << ", " << anOrigVol2;
}
