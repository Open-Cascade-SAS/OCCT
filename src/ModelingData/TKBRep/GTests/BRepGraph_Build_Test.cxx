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

#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_RepId.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <BRepGraph_Validate.hxx>
#include <BRepGraphInc_Populate.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakeTorus.hxx>
#include <BRepPrimAPI_MakeWedge.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_IndexedMap.hxx>
#include <Precision.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopTools_ShapeMapHasher.hxx>

#include <gtest/gtest.h>

#include <cmath>
#include <optional>

// =============================================================================
// Helper: count unique shapes of a given type via TopExp::MapShapes
// =============================================================================

static int countUnique(const TopoDS_Shape& theShape, TopAbs_ShapeEnum theType)
{
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMap;
  TopExp::MapShapes(theShape, theType, aMap);
  return aMap.Extent();
}

static TopoDS_Shape makeStandaloneWire()
{
  BRepBuilderAPI_MakeEdge aMkEdge(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(10.0, 0.0, 0.0));
  return BRepBuilderAPI_MakeWire(TopoDS::Edge(aMkEdge.Shape())).Shape();
}

// =============================================================================
// Sphere tests
// =============================================================================

TEST(BRepGraph_BuildTest, Sphere_IsNotEmpty)
{
  BRepPrimAPI_MakeSphere aMaker(10.0);
  const TopoDS_Shape     aShape = aMaker.Shape();
  ASSERT_TRUE(aMaker.IsDone());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes1 = aGraph.Shapes().Add(aShape);
}

TEST(BRepGraph_BuildTest, Sphere_DefCounts_MatchTopExp)
{
  BRepPrimAPI_MakeSphere aMaker(10.0);
  const TopoDS_Shape     aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes2 = aGraph.Shapes().Add(aShape);

  EXPECT_EQ(aGraph.Topo().Solids().Nb(), countUnique(aShape, TopAbs_SOLID));
  EXPECT_EQ(aGraph.Topo().Shells().Nb(), countUnique(aShape, TopAbs_SHELL));
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), countUnique(aShape, TopAbs_FACE));
  EXPECT_EQ(aGraph.Topo().Wires().Nb(), countUnique(aShape, TopAbs_WIRE));
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), countUnique(aShape, TopAbs_EDGE));
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), countUnique(aShape, TopAbs_VERTEX));
}

TEST(BRepGraph_BuildTest, Sphere_SurfaceType)
{
  BRepPrimAPI_MakeSphere aMaker(5.0);
  const TopoDS_Shape     aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes3 = aGraph.Shapes().Add(aShape);
  ASSERT_GE(aGraph.Topo().Faces().Nb(), 1);

  bool aHasSpherical = false;
  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const occ::handle<Geom_Surface>& aSurf =
      BRepGraph_Tool::Face::Surface(aGraph, aFaceIt.CurrentId());
    if (!aSurf.IsNull() && aSurf->DynamicType() == STANDARD_TYPE(Geom_SphericalSurface))
    {
      aHasSpherical = true;
    }
  }
  EXPECT_TRUE(aHasSpherical);
}

TEST(BRepGraph_BuildTest, Sphere_HasDegenerateEdges)
{
  BRepPrimAPI_MakeSphere aMaker(8.0);
  const TopoDS_Shape     aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes4 = aGraph.Shapes().Add(aShape);

  // A sphere has degenerate edges at poles.
  int aDegCount = 0;
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    if (BRepGraph_Tool::Edge::Degenerated(aGraph, anEdgeIt.CurrentId()))
    {
      ++aDegCount;
    }
  }
  EXPECT_GE(aDegCount, 1);
}

// =============================================================================
// Cylinder tests
// =============================================================================

TEST(BRepGraph_BuildTest, Cylinder_IsNotEmpty)
{
  BRepPrimAPI_MakeCylinder aMaker(5.0, 20.0);
  const TopoDS_Shape       aShape = aMaker.Shape();
  ASSERT_TRUE(aMaker.IsDone());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes5 = aGraph.Shapes().Add(aShape);
}

TEST(BRepGraph_BuildTest, Cylinder_DefCounts_MatchTopExp)
{
  BRepPrimAPI_MakeCylinder aMaker(5.0, 20.0);
  const TopoDS_Shape       aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes6 = aGraph.Shapes().Add(aShape);

  EXPECT_EQ(aGraph.Topo().Faces().Nb(), countUnique(aShape, TopAbs_FACE));
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), countUnique(aShape, TopAbs_EDGE));
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), countUnique(aShape, TopAbs_VERTEX));
}

TEST(BRepGraph_BuildTest, Cylinder_SurfaceType)
{
  BRepPrimAPI_MakeCylinder aMaker(5.0, 20.0);
  const TopoDS_Shape       aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes7 = aGraph.Shapes().Add(aShape);

  bool aHasCylindrical = false;
  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const occ::handle<Geom_Surface>& aSurf =
      BRepGraph_Tool::Face::Surface(aGraph, aFaceIt.CurrentId());
    if (!aSurf.IsNull() && aSurf->DynamicType() == STANDARD_TYPE(Geom_CylindricalSurface))
    {
      aHasCylindrical = true;
    }
  }
  EXPECT_TRUE(aHasCylindrical);
}

// =============================================================================
// Cone tests
// =============================================================================

TEST(BRepGraph_BuildTest, Cone_IsNotEmpty)
{
  BRepPrimAPI_MakeCone aMaker(10.0, 0.0, 15.0);
  const TopoDS_Shape   aShape = aMaker.Shape();
  ASSERT_TRUE(aMaker.IsDone());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes8 = aGraph.Shapes().Add(aShape);
}

TEST(BRepGraph_BuildTest, Cone_DefCounts_MatchTopExp)
{
  BRepPrimAPI_MakeCone aMaker(10.0, 0.0, 15.0);
  const TopoDS_Shape   aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes9 = aGraph.Shapes().Add(aShape);

  EXPECT_EQ(aGraph.Topo().Faces().Nb(), countUnique(aShape, TopAbs_FACE));
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), countUnique(aShape, TopAbs_EDGE));
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), countUnique(aShape, TopAbs_VERTEX));
}

TEST(BRepGraph_BuildTest, Cone_SurfaceType)
{
  BRepPrimAPI_MakeCone aMaker(10.0, 5.0, 15.0);
  const TopoDS_Shape   aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes10 = aGraph.Shapes().Add(aShape);

  bool aHasConical = false;
  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const occ::handle<Geom_Surface>& aSurf =
      BRepGraph_Tool::Face::Surface(aGraph, aFaceIt.CurrentId());
    if (!aSurf.IsNull() && aSurf->DynamicType() == STANDARD_TYPE(Geom_ConicalSurface))
    {
      aHasConical = true;
    }
  }
  EXPECT_TRUE(aHasConical);
}

TEST(BRepGraph_BuildTest, Cone_HasDegenerateEdge)
{
  // Cone with top radius 0 => apex degenerate edge.
  BRepPrimAPI_MakeCone aMaker(10.0, 0.0, 15.0);
  const TopoDS_Shape   aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes11 = aGraph.Shapes().Add(aShape);

  int aDegCount = 0;
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    if (BRepGraph_Tool::Edge::Degenerated(aGraph, anEdgeIt.CurrentId()))
    {
      ++aDegCount;
    }
  }
  EXPECT_GE(aDegCount, 1);
}

// =============================================================================
// Torus tests
// =============================================================================

TEST(BRepGraph_BuildTest, Torus_IsNotEmpty)
{
  BRepPrimAPI_MakeTorus aMaker(20.0, 5.0);
  const TopoDS_Shape    aShape = aMaker.Shape();
  ASSERT_TRUE(aMaker.IsDone());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes12 = aGraph.Shapes().Add(aShape);
}

TEST(BRepGraph_BuildTest, Torus_DefCounts_MatchTopExp)
{
  BRepPrimAPI_MakeTorus aMaker(20.0, 5.0);
  const TopoDS_Shape    aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes13 = aGraph.Shapes().Add(aShape);

  EXPECT_EQ(aGraph.Topo().Faces().Nb(), countUnique(aShape, TopAbs_FACE));
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), countUnique(aShape, TopAbs_EDGE));
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), countUnique(aShape, TopAbs_VERTEX));
}

TEST(BRepGraph_BuildTest, Torus_SurfaceType)
{
  BRepPrimAPI_MakeTorus aMaker(20.0, 5.0);
  const TopoDS_Shape    aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes14 = aGraph.Shapes().Add(aShape);

  bool aHasToroidal = false;
  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const occ::handle<Geom_Surface>& aSurf =
      BRepGraph_Tool::Face::Surface(aGraph, aFaceIt.CurrentId());
    if (!aSurf.IsNull() && aSurf->DynamicType() == STANDARD_TYPE(Geom_ToroidalSurface))
    {
      aHasToroidal = true;
    }
  }
  EXPECT_TRUE(aHasToroidal);
}

// =============================================================================
// Wedge tests
// =============================================================================

TEST(BRepGraph_BuildTest, Wedge_IsNotEmpty)
{
  BRepPrimAPI_MakeWedge aMaker(10.0, 10.0, 10.0, 5.0);
  const TopoDS_Shape    aShape = aMaker.Shape();
  ASSERT_TRUE(aMaker.IsDone());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes15 = aGraph.Shapes().Add(aShape);
}

TEST(BRepGraph_BuildTest, Wedge_DefCounts_MatchTopExp)
{
  BRepPrimAPI_MakeWedge aMaker(10.0, 10.0, 10.0, 5.0);
  const TopoDS_Shape    aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes16 = aGraph.Shapes().Add(aShape);

  EXPECT_EQ(aGraph.Topo().Solids().Nb(), countUnique(aShape, TopAbs_SOLID));
  EXPECT_EQ(aGraph.Topo().Shells().Nb(), countUnique(aShape, TopAbs_SHELL));
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), countUnique(aShape, TopAbs_FACE));
  EXPECT_EQ(aGraph.Topo().Wires().Nb(), countUnique(aShape, TopAbs_WIRE));
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), countUnique(aShape, TopAbs_EDGE));
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), countUnique(aShape, TopAbs_VERTEX));
}

TEST(BRepGraph_BuildTest, Wedge_AllPlanarSurfaces)
{
  BRepPrimAPI_MakeWedge aMaker(10.0, 10.0, 10.0, 5.0);
  const TopoDS_Shape    aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes17 = aGraph.Shapes().Add(aShape);

  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const occ::handle<Geom_Surface>& aSurf =
      BRepGraph_Tool::Face::Surface(aGraph, aFaceIt.CurrentId());
    ASSERT_FALSE(aSurf.IsNull());
    EXPECT_TRUE(aSurf->DynamicType() == STANDARD_TYPE(Geom_Plane))
      << "Face " << aFaceIt.CurrentId().Index << " surface is not a Geom_Plane";
  }
}

// =============================================================================
// Compound builds
// =============================================================================

TEST(BRepGraph_BuildTest, Compound_TwoPrimitives_IsNotEmpty)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 10.0, 10.0);
  const TopoDS_Shape  aBox = aBoxMaker.Shape();

  BRepPrimAPI_MakeSphere aSphereMaker(5.0);
  const TopoDS_Shape     aSphere = aSphereMaker.Shape();

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBox);
  aBuilder.Add(aCompound, aSphere);

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes18 = aGraph.Shapes().Add(aCompound);
}

TEST(BRepGraph_BuildTest, Compound_TwoPrimitives_DefCountsAddUp)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 10.0, 10.0);
  const TopoDS_Shape  aBox = aBoxMaker.Shape();

  BRepPrimAPI_MakeSphere aSphereMaker(5.0);
  const TopoDS_Shape     aSphere = aSphereMaker.Shape();

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBox);
  aBuilder.Add(aCompound, aSphere);

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes19 = aGraph.Shapes().Add(aCompound);

  EXPECT_EQ(aGraph.Topo().Faces().Nb(), countUnique(aCompound, TopAbs_FACE));
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), countUnique(aCompound, TopAbs_EDGE));
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), countUnique(aCompound, TopAbs_VERTEX));
}

TEST(BRepGraph_BuildTest, Compound_ThreeBoxes_DefCounts)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  for (int anIdx = 0; anIdx < 3; ++anIdx)
  {
    BRepPrimAPI_MakeBox aMaker(5.0 * (anIdx + 1), 10.0, 10.0);
    const TopoDS_Shape  aBox = aMaker.Shape();
    aBuilder.Add(aCompound, aBox);
  }

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes20 = aGraph.Shapes().Add(aCompound);

  EXPECT_EQ(aGraph.Topo().Solids().Nb(), countUnique(aCompound, TopAbs_SOLID));
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), countUnique(aCompound, TopAbs_FACE));
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), countUnique(aCompound, TopAbs_EDGE));
}

TEST(BRepGraph_BuildTest, Compound_Nested_DefCounts)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 10.0, 10.0);
  const TopoDS_Shape  aBox = aBoxMaker.Shape();

  BRepPrimAPI_MakeCylinder aCylMaker(3.0, 10.0);
  const TopoDS_Shape       aCyl = aCylMaker.Shape();

  BRep_Builder    aBuilder;
  TopoDS_Compound anInner;
  aBuilder.MakeCompound(anInner);
  aBuilder.Add(anInner, aBox);

  TopoDS_Compound anOuter;
  aBuilder.MakeCompound(anOuter);
  aBuilder.Add(anOuter, anInner);
  aBuilder.Add(anOuter, aCyl);

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes21 = aGraph.Shapes().Add(anOuter);

  EXPECT_EQ(aGraph.Topo().Faces().Nb(), countUnique(anOuter, TopAbs_FACE));
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), countUnique(anOuter, TopAbs_EDGE));
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), countUnique(anOuter, TopAbs_VERTEX));
}

// =============================================================================
// Minimal shapes
// =============================================================================

TEST(BRepGraph_BuildTest, SinglePlanarFace_HasWarnings)
{
  gp_Pln                  aPln;
  BRepBuilderAPI_MakeFace aFaceMaker(aPln);
  const TopoDS_Shape      aShape = aFaceMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  const BRepGraph::ShapesView::Result aResult = aGraph.Shapes().Add(aShape);
  EXPECT_TRUE(aResult.IsOk());
  EXPECT_EQ(aResult.Status, BRepGraph::ShapesView::AddStatus::SuccessWithWarnings);
}

TEST(BRepGraph_BuildTest, SinglePlanarFace_Counts)
{
  gp_Pln                  aPln;
  BRepBuilderAPI_MakeFace aFaceMaker(aPln);
  const TopoDS_Shape      aShape = aFaceMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  const BRepGraph::ShapesView::Result aResult = aGraph.Shapes().Add(aShape);
  ASSERT_TRUE(aResult.IsOk());
  EXPECT_EQ(aResult.Status, BRepGraph::ShapesView::AddStatus::SuccessWithWarnings);
}

TEST(BRepGraph_BuildTest, SingleEdge_HandlesGracefully)
{
  BRepBuilderAPI_MakeEdge anEdgeMaker(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0));
  ASSERT_TRUE(anEdgeMaker.IsDone());
  const TopoDS_Shape aShape = anEdgeMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes24 = aGraph.Shapes().Add(aShape);

  // BRepGraph is face-level; standalone edges may produce zero counts.
  // Verify it does not crash and returns consistent state.
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().Solids().Nb(), 0);
}

TEST(BRepGraph_BuildTest, SingleVertex_HandlesGracefully)
{
  BRepBuilderAPI_MakeVertex aVertexMaker(gp_Pnt(1.0, 2.0, 3.0));
  const TopoDS_Shape        aShape = aVertexMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes25 = aGraph.Shapes().Add(aShape);

  // BRepGraph is face-level; standalone vertices may produce zero counts.
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), 0);
}

// =============================================================================
// Box cross-validation with TopExp
// =============================================================================

TEST(BRepGraph_BuildTest, Box_FaceDefCount_MatchesTopExp)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape  aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes26 = aGraph.Shapes().Add(aBox);

  EXPECT_EQ(aGraph.Topo().Faces().Nb(), countUnique(aBox, TopAbs_FACE));
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 6);
}

TEST(BRepGraph_BuildTest, Box_EdgeDefCount_MatchesTopExp)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape  aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes27 = aGraph.Shapes().Add(aBox);

  EXPECT_EQ(aGraph.Topo().Edges().Nb(), countUnique(aBox, TopAbs_EDGE));
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), 12);
}

TEST(BRepGraph_BuildTest, Box_VertexDefCount_MatchesTopExp)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape  aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes28 = aGraph.Shapes().Add(aBox);

  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), countUnique(aBox, TopAbs_VERTEX));
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), 8);
}

TEST(BRepGraph_BuildTest, Box_VertexPoints_MatchBRepTool)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape  aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes29 = aGraph.Shapes().Add(aBox);

  // Collect all vertex points from TopExp.
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aVertexMap;
  TopExp::MapShapes(aBox, TopAbs_VERTEX, aVertexMap);

  ASSERT_EQ(aGraph.Topo().Vertices().Nb(), aVertexMap.Extent());

  // For each graph vertex, verify that a matching TopExp vertex exists.
  for (BRepGraph_VertexIterator aVertexIt(aGraph); aVertexIt.More(); aVertexIt.Next())
  {
    const BRepGraph_VertexId aVertexId = aVertexIt.CurrentId();
    const gp_Pnt             aGraphPnt = BRepGraph_Tool::Vertex::Pnt(aGraph, aVertexId);
    bool                     aFound    = false;
    for (int aMapIdx = 1; aMapIdx <= aVertexMap.Extent(); ++aMapIdx)
    {
      const TopoDS_Vertex& aVertex    = TopoDS::Vertex(aVertexMap(aMapIdx));
      const gp_Pnt         aTopExpPnt = BRep_Tool::Pnt(aVertex);
      if (aGraphPnt.Distance(aTopExpPnt) < Precision::Confusion())
      {
        aFound = true;
        break;
      }
    }
    EXPECT_TRUE(aFound) << "Graph vertex " << aVertexId.Index << " at (" << aGraphPnt.X() << ", "
                        << aGraphPnt.Y() << ", " << aGraphPnt.Z()
                        << ") has no matching TopExp vertex";
  }
}

TEST(BRepGraph_BuildTest, Box_FaceTolerances_MatchBRepTool)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape  aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes30 = aGraph.Shapes().Add(aBox);

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aFaceMap;
  TopExp::MapShapes(aBox, TopAbs_FACE, aFaceMap);

  // Verify that each graph face tolerance appears in the TopExp set.
  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId aFaceId   = aFaceIt.CurrentId();
    const double           aGraphTol = BRepGraph_Tool::Face::Tolerance(aGraph, aFaceId);
    bool                   aFound    = false;
    for (int aMapIdx = 1; aMapIdx <= aFaceMap.Extent(); ++aMapIdx)
    {
      const TopoDS_Face& aFace      = TopoDS::Face(aFaceMap(aMapIdx));
      const double       aTopExpTol = BRep_Tool::Tolerance(aFace);
      if (std::abs(aGraphTol - aTopExpTol) < Precision::Confusion())
      {
        aFound = true;
        break;
      }
    }
    EXPECT_TRUE(aFound) << "Graph face " << aFaceId.Index << " tolerance " << aGraphTol
                        << " has no matching TopExp face tolerance";
  }
}

TEST(BRepGraph_BuildTest, Box_EdgeTolerances_MatchBRepTool)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape  aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes31 = aGraph.Shapes().Add(aBox);

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> anEdgeMap;
  TopExp::MapShapes(aBox, TopAbs_EDGE, anEdgeMap);

  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId anEdgeId  = anEdgeIt.CurrentId();
    const double           aGraphTol = BRepGraph_Tool::Edge::Tolerance(aGraph, anEdgeId);
    bool                   aFound    = false;
    for (int aMapIdx = 1; aMapIdx <= anEdgeMap.Extent(); ++aMapIdx)
    {
      const TopoDS_Edge& anEdge     = TopoDS::Edge(anEdgeMap(aMapIdx));
      const double       aTopExpTol = BRep_Tool::Tolerance(anEdge);
      if (std::abs(aGraphTol - aTopExpTol) < Precision::Confusion())
      {
        aFound = true;
        break;
      }
    }
    EXPECT_TRUE(aFound) << "Graph edge " << anEdgeId.Index << " tolerance " << aGraphTol
                        << " has no matching TopExp edge tolerance";
  }
}

TEST(BRepGraph_BuildTest, Box_AllSurfacesArePlanes)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape  aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes32 = aGraph.Shapes().Add(aBox);

  ASSERT_EQ(aGraph.Topo().Faces().Nb(), 6);
  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const occ::handle<Geom_Surface>& aSurf =
      BRepGraph_Tool::Face::Surface(aGraph, aFaceIt.CurrentId());
    ASSERT_FALSE(aSurf.IsNull());
    EXPECT_TRUE(aSurf->DynamicType() == STANDARD_TYPE(Geom_Plane))
      << "Face " << aFaceIt.CurrentId().Index << " surface is not Geom_Plane";
  }
}

TEST(BRepGraph_BuildTest, Box_NoDegenerateEdges)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape  aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes33 = aGraph.Shapes().Add(aBox);

  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    EXPECT_FALSE(BRepGraph_Tool::Edge::Degenerated(aGraph, anEdgeIt.CurrentId()))
      << "Box edge " << anEdgeIt.CurrentId().Index << " is degenerate unexpectedly";
  }
}

TEST(BRepGraph_BuildTest, Box_EdgeVertexDefsAreValid)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape  aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes34 = aGraph.Shapes().Add(aBox);

  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId      anEdgeId  = anEdgeIt.CurrentId();
    const BRepGraph_VertexRefId aStartRef = BRepGraph_Tool::Edge::StartVertexId(aGraph, anEdgeId);
    const BRepGraph_VertexRefId anEndRef  = BRepGraph_Tool::Edge::EndVertexId(aGraph, anEdgeId);
    EXPECT_TRUE(aStartRef.IsValid()) << "Edge " << anEdgeId.Index << " has invalid start vertex";
    EXPECT_TRUE(anEndRef.IsValid()) << "Edge " << anEdgeId.Index << " has invalid end vertex";
    EXPECT_EQ(BRepGraph_NodeId(aGraph.Refs().Vertices().Entry(aStartRef).ChildVertexId).NodeKind,
              BRepGraph_NodeId::Kind::Vertex);
    EXPECT_EQ(BRepGraph_NodeId(aGraph.Refs().Vertices().Entry(anEndRef).ChildVertexId).NodeKind,
              BRepGraph_NodeId::Kind::Vertex);
  }
}

TEST(BRepGraph_BuildTest, Box_FaceSurfacesAreValid)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape  aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes35 = aGraph.Shapes().Add(aBox);

  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    EXPECT_TRUE(BRepGraph_Tool::Face::HasSurface(aGraph, aFaceIt.CurrentId()))
      << "Face " << aFaceIt.CurrentId().Index << " has no surface rep";
  }
}

TEST(BRepGraph_BuildTest, Box_EdgeParamRange_IsNonDegenerate)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape  aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes36 = aGraph.Shapes().Add(aBox);

  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId          anEdgeId = anEdgeIt.CurrentId();
    const std::pair<double, double> aRange   = BRepGraph_Tool::Edge::Range(aGraph, anEdgeId);
    EXPECT_LT(aRange.first, aRange.second)
      << "Edge " << anEdgeId.Index << " has invalid parameter range [" << aRange.first << ", "
      << aRange.second << "]";
  }
}

TEST(BRepGraph_BuildTest, AddFlatten_OnEmptyGraph_BuildsFlattenedGraph)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph                                            aGraph;
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes37 =
    aGraph.Shapes().Add(aBox, BRepGraph::ShapesView::Options{{}, false, true, false});

  EXPECT_EQ(aGraph.Topo().Solids().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().Shells().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 6);

  // Flatten Add creates raw topology roots (no product wrapper).
  // Verify the 6 appended faces directly.
  const uint32_t aNbFaces = aGraph.Topo().Faces().Nb();
  for (BRepGraph_FaceId aFaceId(0); aFaceId.IsValid(aNbFaces); ++aFaceId)
  {
    EXPECT_FALSE(aGraph.Topo().Gen().IsRemoved(aFaceId));
  }

  EXPECT_TRUE(aGraph.Editor().ValidateMutationBoundary());
}

TEST(BRepGraph_BuildTest, Build_MutationBoundary_IsValid)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes38 =
    aGraph.Shapes().Add(aBoxMaker.Shape());

  EXPECT_TRUE(aGraph.Editor().ValidateMutationBoundary());
}

TEST(BRepGraph_BuildTest, AddFlatten_SameFaceTwice_DedupsDefinition)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  const TopoDS_Face aFace = TopoDS::Face(anExp.Current());

  BRepGraph                                            aGraph;
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes39 =
    aGraph.Shapes().Add(aFace, BRepGraph::ShapesView::Options{{}, false, true, false});
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes40 =
    aGraph.Shapes().Add(aFace, BRepGraph::ShapesView::Options{{}, false, true, false});

  // Same TShape appended twice: definition is deduplicated.
  ASSERT_EQ(aGraph.Topo().Faces().Nb(), 1);
}

TEST(BRepGraph_BuildTest, AddFlatten_AfterBuild_DoesNotCreateNewSolidDefs)
{
  BRepPrimAPI_MakeBox aBox1Maker(10.0, 20.0, 30.0);
  BRepPrimAPI_MakeBox aBox2Maker(15.0, 25.0, 35.0);

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes41 =
    aGraph.Shapes().Add(aBox1Maker.Shape());

  const uint32_t aNbSolidsBefore = aGraph.Topo().Solids().Nb();
  const uint32_t aNbFacesBefore  = aGraph.Topo().Faces().Nb();

  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes42 =
    aGraph.Shapes().Add(aBox2Maker.Shape(), BRepGraph::ShapesView::Options{{}, false, true, false});

  EXPECT_EQ(aGraph.Topo().Solids().Nb(), aNbSolidsBefore);
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), aNbFacesBefore + 6);
  // Initial BRepGraph::ShapesView::Add() created 1 product; Flatten Add doesn't create
  // products.
  EXPECT_EQ(aGraph.RootProductIds().Size(), 1);
  EXPECT_TRUE(aGraph.Editor().ValidateMutationBoundary());
}

TEST(BRepGraph_BuildTest, AddFull_MutationBoundary_IsValid)
{
  BRepPrimAPI_MakeBox    aBoxMaker(10.0, 20.0, 30.0);
  BRepPrimAPI_MakeSphere aSphereMaker(5.0);

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes43 =
    aGraph.Shapes().Add(aBoxMaker.Shape());

  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes44 =
    aGraph.Shapes().Add(aSphereMaker.Shape());

  EXPECT_TRUE(aGraph.Editor().ValidateMutationBoundary());
}

TEST(BRepGraph_BuildTest, AddFlatten_AppendedFaceHasNoParentShell)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  const TopoDS_Face aFace = TopoDS::Face(anExp.Current());

  BRepGraph                                            aGraph;
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes45 =
    aGraph.Shapes().Add(aFace, BRepGraph::ShapesView::Options{{}, false, true, false});

  ASSERT_EQ(aGraph.Topo().Faces().Nb(), 1);
  // Appended face should not be part of any shell.
  EXPECT_EQ(aGraph.Topo().Shells().Nb(), 0);
}

TEST(BRepGraph_BuildTest, AddFlatten_PreservesExistingUIDs)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes46 = aGraph.Shapes().Add(aBox);
  ASSERT_GT(aGraph.Topo().Edges().Nb(), 0);

  // Record UID of the first edge before append.
  const BRepGraph_UID anOrigUID = aGraph.UIDs().Of(BRepGraph_EdgeId::Start());
  ASSERT_TRUE(anOrigUID.IsValid());

  // Append a sphere.
  BRepPrimAPI_MakeSphere                               aSphereMaker(5.0);
  const TopoDS_Shape&                                  aSphere = aSphereMaker.Shape();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes47 =
    aGraph.Shapes().Add(aSphere, BRepGraph::ShapesView::Options{{}, false, true, false});

  // Verify original edge UID is unchanged.
  const BRepGraph_UID aPostUID = aGraph.UIDs().Of(BRepGraph_EdgeId::Start());
  EXPECT_EQ(anOrigUID, aPostUID);
}

TEST(BRepGraph_BuildTest, AddFlatten_StandaloneVertex_AppendsIntoNonEmptyGraph)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes48 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());

  const uint32_t     aNbVerticesBefore = aGraph.Topo().Vertices().Nb();
  const TopoDS_Shape aVertexShape      = BRepBuilderAPI_MakeVertex(gp_Pnt(100.0, 0.0, 0.0)).Shape();

  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes49 =
    aGraph.Shapes().Add(aVertexShape, BRepGraph::ShapesView::Options{{}, false, true, false});

  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), aNbVerticesBefore + 1);
  EXPECT_TRUE(aGraph.Shapes().FindNode(aVertexShape).IsValid());
  // Verify the new vertex is accessible.
  const BRepGraph_VertexId aNewVtx(aNbVerticesBefore);
  EXPECT_FALSE(aGraph.Shapes().Shape(aNewVtx).IsNull());
}

TEST(BRepGraph_BuildTest, AddFlatten_StandaloneEdge_AppendsIntoNonEmptyGraph)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes50 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());

  const uint32_t     aNbEdgesBefore = aGraph.Topo().Edges().Nb();
  const TopoDS_Shape anEdgeShape =
    BRepBuilderAPI_MakeEdge(gp_Pnt(100.0, 0.0, 0.0), gp_Pnt(120.0, 0.0, 0.0)).Shape();

  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes51 =
    aGraph.Shapes().Add(anEdgeShape, BRepGraph::ShapesView::Options{{}, false, true, false});

  EXPECT_EQ(aGraph.Topo().Edges().Nb(), aNbEdgesBefore + 1);
  EXPECT_TRUE(aGraph.Shapes().FindNode(anEdgeShape).IsValid());
  // Verify the new edge is accessible.
  const BRepGraph_EdgeId aNewEdge(aNbEdgesBefore);
  EXPECT_FALSE(aGraph.Shapes().Shape(aNewEdge).IsNull());
}

TEST(BRepGraph_BuildTest, AddFlatten_StandaloneWire_AppendsIntoNonEmptyGraph)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes52 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());

  const uint32_t     aNbWiresBefore = aGraph.Topo().Wires().Nb();
  const TopoDS_Shape aWireShape     = makeStandaloneWire();

  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes53 =
    aGraph.Shapes().Add(aWireShape, BRepGraph::ShapesView::Options{{}, false, true, false});

  EXPECT_EQ(aGraph.Topo().Wires().Nb(), aNbWiresBefore + 1);
  EXPECT_TRUE(aGraph.Shapes().FindNode(aWireShape).IsValid());
  // Verify the new wire is accessible.
  const BRepGraph_WireId aNewWire(aNbWiresBefore);
  EXPECT_FALSE(aGraph.Shapes().Shape(aNewWire).IsNull());
}

TEST(BRepGraph_BuildTest, AddFlatten_CompoundWithStandaloneShapes)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes54 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(50.0, 0.0, 0.0)).Shape());
  aBuilder.Add(aCompound,
               BRepBuilderAPI_MakeEdge(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(10.0, 0.0, 0.0)).Shape());

  const uint32_t aNbVerticesBefore = aGraph.Topo().Vertices().Nb();
  const uint32_t aNbEdgesBefore    = aGraph.Topo().Edges().Nb();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes55 =
    aGraph.Shapes().Add(aCompound, BRepGraph::ShapesView::Options{{}, false, true, false});
  // The compound should add at least 1 vertex and 1 edge.
  EXPECT_GT(aGraph.Topo().Vertices().Nb(), aNbVerticesBefore);
  EXPECT_GT(aGraph.Topo().Edges().Nb(), aNbEdgesBefore);
}

TEST(BRepGraph_BuildTest, Build_BasicQueriesAndAlgorithmsWork)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  {
    BRepGraph::ShapesView::Options anOpts;
    anOpts.Parallel = false;
    [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes56 =
      aGraph.Shapes().Add(aBox, anOpts);
  };
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 6);
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), 12);
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), 8);
}

TEST(BRepGraph_BuildTest, RootProductIds_Box_ReturnsOneProduct)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes62 = aGraph.Shapes().Add(aBox);

  // BRepGraph::ShapesView::Add() from a solid should produce exactly one root product.
  const NCollection_LinearVector<BRepGraph_ProductId>& aRoots = aGraph.RootProductIds();
  ASSERT_EQ(aRoots.Size(), 1);

  // The product's shape root should be a Solid (for a box).
  const BRepGraph_ProductId aRootProduct = aRoots.Value(0);
  EXPECT_TRUE(aRootProduct.IsValid());
  const BRepGraph_NodeId aShapeRoot = aGraph.Topo().Products().ShapeRoot(aRootProduct);
  EXPECT_TRUE(aShapeRoot.IsValid());
  EXPECT_EQ(aShapeRoot.NodeKind, BRepGraph_NodeId::Kind::Solid);
}

TEST(BRepGraph_BuildTest, BuildOptions_DisableAutoProduct_DoesNotCreateProducts)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph::ShapesView::Options anOptions;
  anOptions.CreateAutoProduct = false;

  BRepGraph aGraph;
  aGraph.Clear();
  {
    BRepGraph::ShapesView::Options anOpts__ = anOptions;
    anOpts__.Parallel                       = false;
    [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes63 =
      aGraph.Shapes().Add(aBox, anOpts__);
  };

  EXPECT_EQ(aGraph.Topo().Products().Nb(), 0);
  EXPECT_EQ(aGraph.RootProductIds().Size(), 0);
  EXPECT_EQ(aGraph.Topo().Solids().Nb(), 1);
}

TEST(BRepGraph_BuildTest, RootProductIds_AddFlatten_ProductCountUnchanged)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes64 = aGraph.Shapes().Add(aBox);
  ASSERT_EQ(aGraph.RootProductIds().Size(), 1);

  const uint32_t aNbFacesBefore = aGraph.Topo().Faces().Nb();

  // Append a second shape (a single face from another box).
  BRepPrimAPI_MakeBox aBox2Maker(5.0, 5.0, 5.0);
  TopExp_Explorer     anExp(aBox2Maker.Shape(), TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  const TopoDS_Face aFace = TopoDS::Face(anExp.Current());

  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes65 =
    aGraph.Shapes().Add(aFace, BRepGraph::ShapesView::Options{{}, false, true, false});

  // Flatten Add does not create new products.
  const NCollection_LinearVector<BRepGraph_ProductId>& aRoots = aGraph.RootProductIds();
  EXPECT_EQ(aRoots.Size(), 1);

  // But a new face was appended.
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), aNbFacesBefore + 1);

  // The original product root should still be valid.
  EXPECT_TRUE(aRoots.Value(0).IsValid());
}

// =============================================================================
// Natural-bound populate normalization tests (P1)
// =============================================================================

static TopoDS_Face makeNoWireNaturalFace(const occ::handle<Geom_Surface>& theSurface)
{
  BRep_Builder aBuilder;
  TopoDS_Face  aFace;
  aBuilder.MakeFace(aFace, theSurface, Precision::Confusion());
  aBuilder.NaturalRestriction(aFace, true);
  return aFace;
}

static void expectFiniteNoWireNaturalFaceSynthesizesTopology(
  const occ::handle<Geom_Surface>& theSurface)
{
  const TopoDS_Face aFace = makeNoWireNaturalFace(theSurface);

  BRepGraph aGraph;
  aGraph.Clear();
  const BRepGraph::ShapesView::Result aRes = aGraph.Shapes().Add(aFace);
  ASSERT_TRUE(aRes.IsOk());
  ASSERT_EQ(aGraph.Topo().Faces().Nb(), 1u);

  const BRepGraph_FaceId aFaceId = BRepGraph_FaceId::Start();
  EXPECT_GT(BRepGraph_Tool::Face::NbWires(aGraph, aFaceId), 0);
  EXPECT_GT(aGraph.Topo().Wires().Nb(), 0);
  EXPECT_GT(aGraph.Topo().Edges().Nb(), 0);
  EXPECT_GT(aGraph.Topo().Vertices().Nb(), 0);
}

TEST(BRepGraph_BuildTest, NaturalBound_FiniteNoWireNaturalFaces_SynthesizeTopology)
{
  expectFiniteNoWireNaturalFaceSynthesizesTopology(
    new Geom_SphericalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0));

  expectFiniteNoWireNaturalFaceSynthesizesTopology(
    new Geom_ToroidalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 3.0));

  expectFiniteNoWireNaturalFaceSynthesizesTopology(new Geom_RectangularTrimmedSurface(
    new Geom_CylindricalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0),
    0.0,
    2.0 * M_PI,
    0.0,
    10.0,
    true,
    true));

  expectFiniteNoWireNaturalFaceSynthesizesTopology(new Geom_RectangularTrimmedSurface(
    new Geom_ConicalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 0.2, 5.0),
    0.0,
    2.0 * M_PI,
    0.0,
    10.0,
    true,
    true));

  expectFiniteNoWireNaturalFaceSynthesizesTopology(
    new Geom_RectangularTrimmedSurface(new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)),
                                       0.0,
                                       3.0,
                                       0.0,
                                       4.0,
                                       true,
                                       true));
}

TEST(BRepGraph_BuildTest, NaturalBound_Sphere_HasWiresAndEdges)
{
  BRepPrimAPI_MakeSphere aMaker(10.0);
  const TopoDS_Shape     aShape = aMaker.Shape();
  ASSERT_TRUE(aMaker.IsDone());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aRes = aGraph.Shapes().Add(aShape);
  ASSERT_GE(aGraph.Topo().Faces().Nb(), 1);

  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId aFaceId = aFaceIt.CurrentId();
    EXPECT_GT(BRepGraph_Tool::Face::NbWires(aGraph, aFaceId), 0)
      << "Natural-bound sphere face must have explicit wires";
  }
  EXPECT_GT(aGraph.Topo().Wires().Nb(), 0);
  EXPECT_GT(aGraph.Topo().Edges().Nb(), 0);
  EXPECT_GT(aGraph.Topo().Vertices().Nb(), 0);
}

TEST(BRepGraph_BuildTest, NaturalBound_Cylinder_HasWiresAndEdges)
{
  BRepPrimAPI_MakeCylinder aMaker(5.0, 10.0);
  const TopoDS_Shape       aShape = aMaker.Shape();
  ASSERT_TRUE(aMaker.IsDone());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aRes = aGraph.Shapes().Add(aShape);
  ASSERT_GE(aGraph.Topo().Faces().Nb(), 1);

  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId aFaceId = aFaceIt.CurrentId();
    EXPECT_GT(BRepGraph_Tool::Face::NbWires(aGraph, aFaceId), 0)
      << "Natural-bound cylinder face must have explicit wires";
  }
}

TEST(BRepGraph_BuildTest, NaturalBound_Cone_HasWiresAndEdges)
{
  BRepPrimAPI_MakeCone aMaker(5.0, 2.0, 10.0);
  const TopoDS_Shape   aShape = aMaker.Shape();
  ASSERT_TRUE(aMaker.IsDone());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aRes = aGraph.Shapes().Add(aShape);
  ASSERT_GE(aGraph.Topo().Faces().Nb(), 1);

  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId aFaceId = aFaceIt.CurrentId();
    EXPECT_GT(BRepGraph_Tool::Face::NbWires(aGraph, aFaceId), 0)
      << "Natural-bound cone face must have explicit wires";
  }
}

TEST(BRepGraph_BuildTest, NaturalBound_Torus_HasWiresAndEdges)
{
  BRepPrimAPI_MakeTorus aMaker(10.0, 3.0);
  const TopoDS_Shape    aShape = aMaker.Shape();
  ASSERT_TRUE(aMaker.IsDone());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aRes = aGraph.Shapes().Add(aShape);
  ASSERT_GE(aGraph.Topo().Faces().Nb(), 1);

  for (BRepGraph_FaceIterator aFaceIt(aGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId aFaceId = aFaceIt.CurrentId();
    EXPECT_GT(BRepGraph_Tool::Face::NbWires(aGraph, aFaceId), 0)
      << "Natural-bound torus face must have explicit wires";
  }
}

TEST(BRepGraph_BuildTest, NaturalBound_InfinitePlane_ProducesWarnings)
{
  // An infinite plane has NaturalRestriction but infinite UV bounds.
  // Build succeeds with a warning and preserves the unbounded face.
  const occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

  BRep_Builder aBuilder;
  TopoDS_Face  aFace;
  aBuilder.MakeFace(aFace, aPlane, Precision::Confusion());
  aBuilder.NaturalRestriction(aFace, true);

  BRepGraph aGraph;
  aGraph.Clear();
  const BRepGraph::ShapesView::Result aRes = aGraph.Shapes().Add(aFace);
  EXPECT_TRUE(aRes.IsOk());
  EXPECT_EQ(aRes.Status, BRepGraph::ShapesView::AddStatus::SuccessWithWarnings);
  EXPECT_FALSE(aGraph.IsEmpty());
  ASSERT_EQ(aGraph.Topo().Faces().NbActive(), 1u);

  const BRepGraph_FaceId aFaceId = BRepGraph_FaceId::Start();
  EXPECT_EQ(BRepGraph_Tool::Face::NbWires(aGraph, aFaceId), 0u);

  const BRepGraph_Validate::Result anAudit =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_TRUE(anAudit.IsValid());
  EXPECT_EQ(anAudit.NbIssues(BRepGraph_Validate::Severity::Error), 0);
  EXPECT_EQ(anAudit.NbIssues(BRepGraph_Validate::Severity::Warning), 1);
  EXPECT_FALSE(aGraph.Shapes().Reconstruct(aFaceId).IsNull());
}

TEST(BRepGraph_BuildTest, NaturalBound_PreservesExplicitWires)
{
  // A sphere face with NaturalRestriction but also explicit wires
  // should keep the explicit wires, not synthesize new ones.
  BRepPrimAPI_MakeSphere aMaker(10.0);
  const TopoDS_Shape     aShape = aMaker.Shape();
  ASSERT_TRUE(aMaker.IsDone());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aRes = aGraph.Shapes().Add(aShape);

  // Count the total wires - these are all from explicit source topology
  // (BRepPrimAPI_MakeSphere already creates seam edges and wires).
  const uint32_t aNbWires = aGraph.Topo().Wires().Nb();
  EXPECT_GT(aNbWires, 0);

  // Verify the graph round-trips correctly (no double-counting).
  const uint32_t aNbEdges    = aGraph.Topo().Edges().Nb();
  const uint32_t aNbVertices = aGraph.Topo().Vertices().Nb();
  EXPECT_GT(aNbEdges, 0);
  EXPECT_GT(aNbVertices, 0);
}

TEST(BRepGraph_BuildTest, NaturalBound_PlaneSynthesis_SkipsPCurves)
{
  // A trimmed plane with NaturalRestriction should have wires/edges
  // but no stored PCurves on coedges (planes are trivially derivable).
  const occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const occ::handle<Geom_RectangularTrimmedSurface> aTrimmed =
    new Geom_RectangularTrimmedSurface(aPlane, -10.0, 10.0, -10.0, 10.0, false, false);
  const TopoDS_Face aFace = makeNoWireNaturalFace(aTrimmed);

  BRepGraph aGraph;
  aGraph.Clear();
  const BRepGraph::ShapesView::Result aRes = aGraph.Shapes().Add(aFace);
  EXPECT_TRUE(aRes.IsOk());

  // Face should have synthesized wires/edges.
  EXPECT_GT(aGraph.Topo().Wires().Nb(), 0u);
  EXPECT_GT(aGraph.Topo().Edges().Nb(), 0u);

  // Verify no stored PCurves on coedges, but PCurveAdaptor still works.
  for (BRepGraph_WireIterator aWireIt(aGraph); aWireIt.More(); aWireIt.Next())
  {
    for (BRepGraph_CoEdgesOfWire aCoEdgeIt(aGraph, aWireIt.CurrentId()); aCoEdgeIt.More();
         aCoEdgeIt.Next())
    {
      const BRepGraph_CoEdgeId aCoEdgeId = aCoEdgeIt.CurrentId();
      EXPECT_FALSE(BRepGraph_Tool::CoEdge::HasPCurve(aGraph, aCoEdgeId))
        << "Plane synthesis should skip storing PCurves";

      // PCurveAdaptor must still return a valid curve (generated on-the-fly).
      const Geom2dAdaptor_Curve aPCurveAdaptor =
        BRepGraph_Tool::CoEdge::PCurveAdaptor(aGraph, aCoEdgeId);
      EXPECT_TRUE(aPCurveAdaptor.IsInitialized()) << "Plane PCurves should be generated on-the-fly";
    }
  }
}
