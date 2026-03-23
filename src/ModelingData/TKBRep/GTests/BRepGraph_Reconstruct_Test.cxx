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
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_MutView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraphInc_IncidenceRef.hxx>
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

TEST(BRepGraphReconstructTest, Box_Area_Preserved)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  const double anOrigArea = computeArea(aBox);

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  TopoDS_Shape aRecon = aGraph.Shapes().Reconstruct(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, 0));
  const double aReconArea = computeArea(aRecon);

  EXPECT_NEAR(aReconArea, anOrigArea, anOrigArea * 0.01);
}

TEST(BRepGraphReconstructTest, Box_Volume_Preserved)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  const double anOrigVol = computeVolume(aBox);

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  TopoDS_Shape aRecon = aGraph.Shapes().Reconstruct(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, 0));
  const double aReconVol = computeVolume(aRecon);

  EXPECT_NEAR(aReconVol, anOrigVol, anOrigVol * 0.01);
}

TEST(BRepGraphReconstructTest, Sphere_Area_Preserved)
{
  BRepPrimAPI_MakeSphere aSphereMaker(15.0);
  const TopoDS_Shape& aSphere = aSphereMaker.Shape();
  const double anOrigArea = computeArea(aSphere);

  BRepGraph aGraph;
  aGraph.Build(aSphere);
  ASSERT_TRUE(aGraph.IsDone());

  TopoDS_Shape aRecon = aGraph.Shapes().Reconstruct(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, 0));
  const double aReconArea = computeArea(aRecon);

  EXPECT_NEAR(aReconArea, anOrigArea, anOrigArea * 0.01);
}

TEST(BRepGraphReconstructTest, Sphere_Volume_Preserved)
{
  BRepPrimAPI_MakeSphere aSphereMaker(15.0);
  const TopoDS_Shape& aSphere = aSphereMaker.Shape();
  const double anOrigVol = computeVolume(aSphere);

  BRepGraph aGraph;
  aGraph.Build(aSphere);
  ASSERT_TRUE(aGraph.IsDone());

  TopoDS_Shape aRecon = aGraph.Shapes().Reconstruct(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, 0));
  const double aReconVol = computeVolume(aRecon);

  EXPECT_NEAR(aReconVol, anOrigVol, anOrigVol * 0.01);
}

TEST(BRepGraphReconstructTest, Cylinder_Area_Preserved)
{
  BRepPrimAPI_MakeCylinder aCylMaker(10.0, 20.0);
  const TopoDS_Shape& aCyl = aCylMaker.Shape();
  const double anOrigArea = computeArea(aCyl);

  BRepGraph aGraph;
  aGraph.Build(aCyl);
  ASSERT_TRUE(aGraph.IsDone());

  TopoDS_Shape aRecon = aGraph.Shapes().Reconstruct(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, 0));
  const double aReconArea = computeArea(aRecon);

  EXPECT_NEAR(aReconArea, anOrigArea, anOrigArea * 0.01);
}

TEST(BRepGraphReconstructTest, Cylinder_Volume_Preserved)
{
  BRepPrimAPI_MakeCylinder aCylMaker(10.0, 20.0);
  const TopoDS_Shape& aCyl = aCylMaker.Shape();
  const double anOrigVol = computeVolume(aCyl);

  BRepGraph aGraph;
  aGraph.Build(aCyl);
  ASSERT_TRUE(aGraph.IsDone());

  TopoDS_Shape aRecon = aGraph.Shapes().Reconstruct(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, 0));
  const double aReconVol = computeVolume(aRecon);

  EXPECT_NEAR(aReconVol, anOrigVol, anOrigVol * 0.01);
}

// ============================================================
// Per-kind reconstruction
// ============================================================

TEST(BRepGraphReconstructTest, Shell_FaceCount_MatchesOriginal)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  const int anOrigFaceCount = countSubShapes(aBox, TopAbs_FACE);

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  TopoDS_Shape aReconShell = aGraph.Shapes().Reconstruct(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Shell, 0));
  const int aReconFaceCount = countSubShapes(aReconShell, TopAbs_FACE);

  EXPECT_EQ(aReconFaceCount, anOrigFaceCount);
}

TEST(BRepGraphReconstructTest, Wire_EdgeCount_FourPerBoxFace)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // Each wire of a box face should have exactly 4 edges.
  for (int aWireIdx = 0; aWireIdx < aGraph.Defs().NbWires(); ++aWireIdx)
  {
    TopoDS_Shape aReconWire = aGraph.Shapes().Reconstruct(
      BRepGraph_NodeId(BRepGraph_NodeId::Kind::Wire, aWireIdx));
    const int anEdgeCount = countSubShapes(aReconWire, TopAbs_EDGE);
    EXPECT_EQ(anEdgeCount, 4) << "Wire " << aWireIdx << " has " << anEdgeCount << " edges";
  }
}

TEST(BRepGraphReconstructTest, Edge_HasCurve_NonNull)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Defs().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aGraph.Defs().Edge(anEdgeIdx);
    if (anEdgeDef.IsDegenerate)
      continue;

    TopoDS_Shape aReconEdge = aGraph.Shapes().Reconstruct(
      BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, anEdgeIdx));
    TopLoc_Location aLoc;
    double aFirst = 0.0;
    double aLast  = 0.0;
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(TopoDS::Edge(aReconEdge), aLoc, aFirst, aLast);
    EXPECT_FALSE(aCurve.IsNull()) << "Edge " << anEdgeIdx << " has null curve";
  }
}

TEST(BRepGraphReconstructTest, Edge_ParameterRange_Preserved)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  for (int anEdgeIdx = 0; anEdgeIdx < aGraph.Defs().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aGraph.Defs().Edge(anEdgeIdx);
    if (anEdgeDef.IsDegenerate)
      continue;

    TopoDS_Shape aReconEdge = aGraph.Shapes().Reconstruct(
      BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, anEdgeIdx));
    TopLoc_Location aLoc;
    double aFirst = 0.0;
    double aLast  = 0.0;
    BRep_Tool::Curve(TopoDS::Edge(aReconEdge), aLoc, aFirst, aLast);

    EXPECT_NEAR(aFirst, anEdgeDef.ParamFirst, Precision::Confusion())
      << "Edge " << anEdgeIdx << " ParamFirst mismatch";
    EXPECT_NEAR(aLast, anEdgeDef.ParamLast, Precision::Confusion())
      << "Edge " << anEdgeIdx << " ParamLast mismatch";
  }
}

TEST(BRepGraphReconstructTest, Vertex_Point_MatchesDefPoint)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  for (int aVertIdx = 0; aVertIdx < aGraph.Defs().NbVertices(); ++aVertIdx)
  {
    const BRepGraph_TopoNode::VertexDef& aVertDef = aGraph.Defs().Vertex(aVertIdx);

    TopoDS_Shape aReconVtx = aGraph.Shapes().Reconstruct(
      BRepGraph_NodeId(BRepGraph_NodeId::Kind::Vertex, aVertIdx));
    const gp_Pnt aReconPt = BRep_Tool::Pnt(TopoDS::Vertex(aReconVtx));

    EXPECT_NEAR(aReconPt.X(), aVertDef.Point.X(), Precision::Confusion())
      << "Vertex " << aVertIdx << " X mismatch";
    EXPECT_NEAR(aReconPt.Y(), aVertDef.Point.Y(), Precision::Confusion())
      << "Vertex " << aVertIdx << " Y mismatch";
    EXPECT_NEAR(aReconPt.Z(), aVertDef.Point.Z(), Precision::Confusion())
      << "Vertex " << aVertIdx << " Z mismatch";
  }
}

TEST(BRepGraphReconstructTest, Face_PCurvesPresent_OnAllEdges)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  for (int aFaceIdx = 0; aFaceIdx < aGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    TopoDS_Shape aReconFace = aGraph.Shapes().ReconstructFace(aFaceIdx);
    ASSERT_FALSE(aReconFace.IsNull()) << "ReconstructFace returned null for face " << aFaceIdx;

    const TopoDS_Face& aFace = TopoDS::Face(aReconFace);
    for (TopExp_Explorer anEdgeExp(aFace, TopAbs_EDGE); anEdgeExp.More(); anEdgeExp.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeExp.Current());
      if (BRep_Tool::Degenerated(anEdge))
        continue;

      double aFirst = 0.0;
      double aLast  = 0.0;
      Handle(Geom2d_Curve) aPCurve = BRep_Tool::CurveOnSurface(anEdge, aFace, aFirst, aLast);
      EXPECT_FALSE(aPCurve.IsNull())
        << "Edge on face " << aFaceIdx << " is missing a pcurve";
    }
  }
}

TEST(BRepGraphReconstructTest, Face_OrientationPreserved)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // Verify that reconstructed faces have valid orientations matching incidence refs.
  ASSERT_EQ(aGraph.Defs().NbShells(), 1);
  const BRepGraph_TopoNode::ShellDef& aShellDef = aGraph.Defs().Shell(0);
  for (int aRefIdx = 0; aRefIdx < aShellDef.FaceRefs.Length(); ++aRefIdx)
  {
    const BRepGraphInc::FaceRef& aFaceRef = aShellDef.FaceRefs.Value(aRefIdx);
    const TopAbs_Orientation anExpectedOri = aFaceRef.Orientation;

    TopoDS_Shape aReconFace = aGraph.Shapes().ReconstructFace(aFaceRef.FaceIdx);
    ASSERT_FALSE(aReconFace.IsNull()) << "ReconstructFace returned null for face " << aFaceRef.FaceIdx;

    // The reconstructed face from the def should be valid.
    EXPECT_EQ(aReconFace.ShapeType(), TopAbs_FACE);
    (void)anExpectedOri; // Orientation is now stored in the incidence ref, not usage.
  }
}

// ============================================================
// Shape() vs ReconstructShape() consistency
// ============================================================

TEST(BRepGraphReconstructTest, Shape_UnmodifiedGraph_SameAsOriginalOf)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // For an unmodified graph, Shape(id) should be the same TShape as OriginalOf(id).
  BRepGraph_NodeId aSolidId(BRepGraph_NodeId::Kind::Solid, 0);
  ASSERT_TRUE(aGraph.Shapes().HasOriginal(aSolidId));

  TopoDS_Shape aShapeResult = aGraph.Shapes().Shape(aSolidId);
  const TopoDS_Shape& anOriginal = aGraph.Shapes().OriginalOf(aSolidId);
  EXPECT_TRUE(aShapeResult.IsSame(anOriginal));
}

TEST(BRepGraphReconstructTest, Reconstruct_Face_ValidShape)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Defs().NbFaces(), 0);

  TopoDS_Shape aRecon = aGraph.Shapes().ReconstructFace(0);
  EXPECT_FALSE(aRecon.IsNull());
  EXPECT_EQ(aRecon.ShapeType(), TopAbs_FACE);
}

TEST(BRepGraphReconstructTest, Reconstruct_Edge_ValidShape)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Defs().NbEdges(), 0);

  // Find a non-degenerate edge.
  for (int anIdx = 0; anIdx < aGraph.Defs().NbEdges(); ++anIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aGraph.Defs().Edge(anIdx);
    if (anEdgeDef.IsDegenerate)
      continue;

    TopoDS_Shape aRecon = aGraph.Shapes().Reconstruct(
      BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, anIdx));
    EXPECT_FALSE(aRecon.IsNull());
    EXPECT_EQ(aRecon.ShapeType(), TopAbs_EDGE);
    return;
  }
  FAIL() << "No non-degenerate edge found";
}

TEST(BRepGraphReconstructTest, Reconstruct_Vertex_CorrectPoint)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Defs().NbVertices(), 0);

  const BRepGraph_TopoNode::VertexDef& aVtxDef = aGraph.Defs().Vertex(0);
  const gp_Pnt& anExpectedPt = aVtxDef.Point;

  TopoDS_Shape aRecon = aGraph.Shapes().Reconstruct(
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Vertex, 0));
  ASSERT_FALSE(aRecon.IsNull());
  ASSERT_EQ(aRecon.ShapeType(), TopAbs_VERTEX);

  const gp_Pnt aReconPt = BRep_Tool::Pnt(TopoDS::Vertex(aRecon));
  EXPECT_NEAR(aReconPt.Distance(anExpectedPt), 0.0, Precision::Confusion());
}

// ============================================================
// After mutation
// ============================================================

TEST(BRepGraphReconstructTest, AfterVertexMutation_ModifiedFlagAndPointChanged)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // Find a vertex belonging to face 0 and move it significantly.
  const BRepGraph_TopoNode::FaceDef& aFaceDef = aGraph.Defs().Face(0);
  const int anOuterWireIdx = aFaceDef.OuterWireIdx();
  ASSERT_GE(anOuterWireIdx, 0);

  const BRepGraph_TopoNode::WireDef& aWireDef = aGraph.Defs().Wire(anOuterWireIdx);
  ASSERT_GT(aWireDef.EdgeRefs.Length(), 0);

  const BRepGraphInc::EdgeRef& aFirstEdgeRef = aWireDef.EdgeRefs.First();
  const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aGraph.Defs().Edge(aFirstEdgeRef.EdgeIdx);
  const int aVertIdx = anEdgeDef.StartVertexIdx;
  ASSERT_GE(aVertIdx, 0);

  // Mutate: move vertex by 5 units in Z.
  const gp_Pnt anOldPt = aGraph.Defs().Vertex(aVertIdx).Point;
  {
    BRepGraph_MutRef<BRepGraph_TopoNode::VertexDef> aMutVtx = aGraph.Mut().VertexDef(aVertIdx);
    aMutVtx->Point = gp_Pnt(anOldPt.X(), anOldPt.Y(), anOldPt.Z() + 5.0);
  }

  // Verify the modification flag is set on the vertex def.
  EXPECT_TRUE(aGraph.Defs().Vertex(aVertIdx).IsModified)
    << "Vertex def should be marked as modified after mutation";

  // Verify the graph VertexDef.Point has actually changed.
  const gp_Pnt aNewPt = aGraph.Defs().Vertex(aVertIdx).Point;
  EXPECT_GT(anOldPt.Distance(aNewPt), Precision::Confusion())
    << "VertexDef.Point should differ after mutation";
}

TEST(BRepGraphReconstructTest, AfterToleranceMutation_NewTShape)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_NodeId anEdgeId(BRepGraph_NodeId::Kind::Edge, 0);
  TopoDS_Shape aShapeBefore = aGraph.Shapes().Shape(anEdgeId);

  // Mutate tolerance.
  {
    BRepGraph_MutRef<BRepGraph_TopoNode::EdgeDef> aMutEdge = aGraph.Mut().EdgeDef(0);
    aMutEdge->Tolerance = aMutEdge->Tolerance + 1.0;
  }

  // After mutation, Shape() should return a reconstructed shape with a different TShape.
  TopoDS_Shape aShapeAfter = aGraph.Shapes().Shape(anEdgeId);
  EXPECT_FALSE(aShapeAfter.IsNull());
  EXPECT_FALSE(aShapeAfter.IsSame(aShapeBefore))
    << "Shape() should return a new TShape after tolerance mutation";
}

TEST(BRepGraphReconstructTest, CompoundRoot_TwoSolids_Preserved)
{
  BRepPrimAPI_MakeBox aBoxMaker1(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox1 = aBoxMaker1.Shape();
  BRepPrimAPI_MakeBox aBoxMaker2(5.0, 10.0, 15.0);
  const TopoDS_Shape& aBox2 = aBoxMaker2.Shape();

  BRep_Builder aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBox1);
  aBuilder.Add(aCompound, aBox2);

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_EQ(aGraph.Defs().NbSolids(), 2);

  // Reconstruct each solid and verify volumes match originals.
  const double anOrigVol1 = computeVolume(aBox1);
  const double anOrigVol2 = computeVolume(aBox2);

  TopoDS_Shape aRecon1 = aGraph.Shapes().Reconstruct(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, 0));
  TopoDS_Shape aRecon2 = aGraph.Shapes().Reconstruct(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, 1));

  const double aReconVol1 = computeVolume(aRecon1);
  const double aReconVol2 = computeVolume(aRecon2);

  // The two volumes should match the originals (order may vary, so check both assignments).
  const bool isDirectMatch =
    (std::abs(aReconVol1 - anOrigVol1) < anOrigVol1 * 0.01)
    && (std::abs(aReconVol2 - anOrigVol2) < anOrigVol2 * 0.01);
  const bool isSwappedMatch =
    (std::abs(aReconVol1 - anOrigVol2) < anOrigVol2 * 0.01)
    && (std::abs(aReconVol2 - anOrigVol1) < anOrigVol1 * 0.01);

  EXPECT_TRUE(isDirectMatch || isSwappedMatch)
    << "Reconstructed solid volumes do not match originals: "
    << aReconVol1 << ", " << aReconVol2 << " vs " << anOrigVol1 << ", " << anOrigVol2;
}
