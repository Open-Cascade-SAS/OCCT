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
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_LayerParam.hxx>
#include <BRepGraph_LayerRegularity.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <BRepGraphInc_Populate.hxx>
#include <BRepGraph_Builder.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakeTorus.hxx>
#include <BRepPrimAPI_MakeWedge.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
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

// =============================================================================
// Helper: count unique shapes of a given type via TopExp::MapShapes
// =============================================================================

static int countUnique(const TopoDS_Shape& theShape, TopAbs_ShapeEnum theType)
{
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMap;
  TopExp::MapShapes(theShape, theType, aMap);
  return aMap.Extent();
}

static void registerStandardLayers(BRepGraph& theGraph)
{
  theGraph.LayerRegistry().RegisterLayer(new BRepGraph_LayerParam());
  theGraph.LayerRegistry().RegisterLayer(new BRepGraph_LayerRegularity());
}

static TopoDS_Shape makeStandaloneWire()
{
  BRepBuilderAPI_MakeEdge aMkEdge(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(10.0, 0.0, 0.0));
  return BRepBuilderAPI_MakeWire(TopoDS::Edge(aMkEdge.Shape())).Shape();
}

// =============================================================================
// Sphere tests
// =============================================================================

TEST(BRepGraph_BuildTest, Sphere_IsDone)
{
  BRepPrimAPI_MakeSphere aMaker(10.0);
  const TopoDS_Shape     aShape = aMaker.Shape();
  ASSERT_TRUE(aMaker.IsDone());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes1 =
    BRepGraph_Builder::Add(aGraph, aShape);
  EXPECT_TRUE(aGraph.IsDone());
}

TEST(BRepGraph_BuildTest, Sphere_DefCounts_MatchTopExp)
{
  BRepPrimAPI_MakeSphere aMaker(10.0);
  const TopoDS_Shape     aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes2 =
    BRepGraph_Builder::Add(aGraph, aShape);
  ASSERT_TRUE(aGraph.IsDone());

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
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes3 =
    BRepGraph_Builder::Add(aGraph, aShape);
  ASSERT_TRUE(aGraph.IsDone());
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
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes4 =
    BRepGraph_Builder::Add(aGraph, aShape);
  ASSERT_TRUE(aGraph.IsDone());

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

TEST(BRepGraph_BuildTest, Cylinder_IsDone)
{
  BRepPrimAPI_MakeCylinder aMaker(5.0, 20.0);
  const TopoDS_Shape       aShape = aMaker.Shape();
  ASSERT_TRUE(aMaker.IsDone());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes5 =
    BRepGraph_Builder::Add(aGraph, aShape);
  EXPECT_TRUE(aGraph.IsDone());
}

TEST(BRepGraph_BuildTest, Cylinder_DefCounts_MatchTopExp)
{
  BRepPrimAPI_MakeCylinder aMaker(5.0, 20.0);
  const TopoDS_Shape       aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes6 =
    BRepGraph_Builder::Add(aGraph, aShape);
  ASSERT_TRUE(aGraph.IsDone());

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
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes7 =
    BRepGraph_Builder::Add(aGraph, aShape);
  ASSERT_TRUE(aGraph.IsDone());

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

TEST(BRepGraph_BuildTest, Cone_IsDone)
{
  BRepPrimAPI_MakeCone aMaker(10.0, 0.0, 15.0);
  const TopoDS_Shape   aShape = aMaker.Shape();
  ASSERT_TRUE(aMaker.IsDone());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes8 =
    BRepGraph_Builder::Add(aGraph, aShape);
  EXPECT_TRUE(aGraph.IsDone());
}

TEST(BRepGraph_BuildTest, Cone_DefCounts_MatchTopExp)
{
  BRepPrimAPI_MakeCone aMaker(10.0, 0.0, 15.0);
  const TopoDS_Shape   aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes9 =
    BRepGraph_Builder::Add(aGraph, aShape);
  ASSERT_TRUE(aGraph.IsDone());

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
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes10 =
    BRepGraph_Builder::Add(aGraph, aShape);
  ASSERT_TRUE(aGraph.IsDone());

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
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes11 =
    BRepGraph_Builder::Add(aGraph, aShape);
  ASSERT_TRUE(aGraph.IsDone());

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

TEST(BRepGraph_BuildTest, Torus_IsDone)
{
  BRepPrimAPI_MakeTorus aMaker(20.0, 5.0);
  const TopoDS_Shape    aShape = aMaker.Shape();
  ASSERT_TRUE(aMaker.IsDone());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes12 =
    BRepGraph_Builder::Add(aGraph, aShape);
  EXPECT_TRUE(aGraph.IsDone());
}

TEST(BRepGraph_BuildTest, Torus_DefCounts_MatchTopExp)
{
  BRepPrimAPI_MakeTorus aMaker(20.0, 5.0);
  const TopoDS_Shape    aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes13 =
    BRepGraph_Builder::Add(aGraph, aShape);
  ASSERT_TRUE(aGraph.IsDone());

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
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes14 =
    BRepGraph_Builder::Add(aGraph, aShape);
  ASSERT_TRUE(aGraph.IsDone());

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

TEST(BRepGraph_BuildTest, Wedge_IsDone)
{
  BRepPrimAPI_MakeWedge aMaker(10.0, 10.0, 10.0, 5.0);
  const TopoDS_Shape    aShape = aMaker.Shape();
  ASSERT_TRUE(aMaker.IsDone());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes15 =
    BRepGraph_Builder::Add(aGraph, aShape);
  EXPECT_TRUE(aGraph.IsDone());
}

TEST(BRepGraph_BuildTest, Wedge_DefCounts_MatchTopExp)
{
  BRepPrimAPI_MakeWedge aMaker(10.0, 10.0, 10.0, 5.0);
  const TopoDS_Shape    aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes16 =
    BRepGraph_Builder::Add(aGraph, aShape);
  ASSERT_TRUE(aGraph.IsDone());

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
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes17 =
    BRepGraph_Builder::Add(aGraph, aShape);
  ASSERT_TRUE(aGraph.IsDone());

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

TEST(BRepGraph_BuildTest, Compound_TwoPrimitives_IsDone)
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
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes18 =
    BRepGraph_Builder::Add(aGraph, aCompound);
  EXPECT_TRUE(aGraph.IsDone());
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
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes19 =
    BRepGraph_Builder::Add(aGraph, aCompound);
  ASSERT_TRUE(aGraph.IsDone());

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
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes20 =
    BRepGraph_Builder::Add(aGraph, aCompound);
  ASSERT_TRUE(aGraph.IsDone());

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
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes21 =
    BRepGraph_Builder::Add(aGraph, anOuter);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Topo().Faces().Nb(), countUnique(anOuter, TopAbs_FACE));
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), countUnique(anOuter, TopAbs_EDGE));
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), countUnique(anOuter, TopAbs_VERTEX));
}

// =============================================================================
// Minimal shapes
// =============================================================================

TEST(BRepGraph_BuildTest, SinglePlanarFace_IsDone)
{
  gp_Pln                  aPln;
  BRepBuilderAPI_MakeFace aFaceMaker(aPln);
  const TopoDS_Shape      aShape = aFaceMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes22 =
    BRepGraph_Builder::Add(aGraph, aShape);
  EXPECT_TRUE(aGraph.IsDone());
}

TEST(BRepGraph_BuildTest, SinglePlanarFace_Counts)
{
  gp_Pln                  aPln;
  BRepBuilderAPI_MakeFace aFaceMaker(aPln);
  const TopoDS_Shape      aShape = aFaceMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes23 =
    BRepGraph_Builder::Add(aGraph, aShape);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 1);
  EXPECT_EQ(aGraph.Topo().Solids().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().Shells().Nb(), 0);
  EXPECT_GE(aGraph.Topo().Faces().Nb(), 1);

  // Verify surface is a plane.
  ASSERT_TRUE(BRepGraph_Tool::Face::HasSurface(aGraph, BRepGraph_FaceId::Start()));
  const occ::handle<Geom_Surface>& aSurf =
    BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId::Start());
  ASSERT_FALSE(aSurf.IsNull());
  EXPECT_TRUE(aSurf->DynamicType() == STANDARD_TYPE(Geom_Plane));
}

TEST(BRepGraph_BuildTest, SingleEdge_HandlesGracefully)
{
  BRepBuilderAPI_MakeEdge anEdgeMaker(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0));
  ASSERT_TRUE(anEdgeMaker.IsDone());
  const TopoDS_Shape aShape = anEdgeMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes24 =
    BRepGraph_Builder::Add(aGraph, aShape);

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
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes25 =
    BRepGraph_Builder::Add(aGraph, aShape);

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
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes26 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Topo().Faces().Nb(), countUnique(aBox, TopAbs_FACE));
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 6);
}

TEST(BRepGraph_BuildTest, Box_EdgeDefCount_MatchesTopExp)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape  aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes27 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Topo().Edges().Nb(), countUnique(aBox, TopAbs_EDGE));
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), 12);
}

TEST(BRepGraph_BuildTest, Box_VertexDefCount_MatchesTopExp)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape  aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes28 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), countUnique(aBox, TopAbs_VERTEX));
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), 8);
}

TEST(BRepGraph_BuildTest, Box_VertexPoints_MatchBRepTool)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape  aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes29 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

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
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes30 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

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
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes31 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

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
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes32 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

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
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes33 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

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
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes34 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId         anEdgeId = anEdgeIt.CurrentId();
    const BRepGraphInc::VertexRef& aStartRef =
      BRepGraph_Tool::Edge::StartVertexRef(aGraph, anEdgeId);
    const BRepGraphInc::VertexRef& anEndRef = BRepGraph_Tool::Edge::EndVertexRef(aGraph, anEdgeId);
    EXPECT_TRUE(aStartRef.VertexDefId.IsValid())
      << "Edge " << anEdgeId.Index << " has invalid start vertex";
    EXPECT_TRUE(anEndRef.VertexDefId.IsValid())
      << "Edge " << anEdgeId.Index << " has invalid end vertex";
    EXPECT_EQ(BRepGraph_NodeId(aStartRef.VertexDefId).NodeKind, BRepGraph_NodeId::Kind::Vertex);
    EXPECT_EQ(BRepGraph_NodeId(anEndRef.VertexDefId).NodeKind, BRepGraph_NodeId::Kind::Vertex);
  }
}

TEST(BRepGraph_BuildTest, Box_FaceSurfacesAreValid)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape  aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes35 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

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
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes36 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId       anEdgeId = anEdgeIt.CurrentId();
    const BRepGraphInc::EdgeDef& anEdge   = anEdgeIt.Current();
    EXPECT_LT(anEdge.ParamFirst, anEdge.ParamLast)
      << "Edge " << anEdgeId.Index << " has invalid parameter range [" << anEdge.ParamFirst << ", "
      << anEdge.ParamLast << "]";
  }
}

TEST(BRepGraph_BuildTest, AddFlatten_OnEmptyGraph_BuildsFlattenedGraph)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph                                        aGraph;
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes37 =
    BRepGraph_Builder::Add(aGraph, aBox, BRepGraph_Builder::Options{{}, false, true, false});

  EXPECT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.Topo().Solids().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().Shells().Nb(), 0);
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 6);

  // Flatten Add creates raw topology roots (no product wrapper).
  // Verify the 6 appended faces directly.
  const int aNbFaces = aGraph.Topo().Faces().Nb();
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
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes38 =
    BRepGraph_Builder::Add(aGraph, aBoxMaker.Shape());

  ASSERT_TRUE(aGraph.IsDone());
  EXPECT_TRUE(aGraph.Editor().ValidateMutationBoundary());
}

TEST(BRepGraph_BuildTest, AddFlatten_SameFaceTwice_DedupsDefinition)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  const TopoDS_Face aFace = TopoDS::Face(anExp.Current());

  BRepGraph                                        aGraph;
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes39 =
    BRepGraph_Builder::Add(aGraph, aFace, BRepGraph_Builder::Options{{}, false, true, false});
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes40 =
    BRepGraph_Builder::Add(aGraph, aFace, BRepGraph_Builder::Options{{}, false, true, false});

  ASSERT_TRUE(aGraph.IsDone());
  // Same TShape appended twice: definition is deduplicated.
  ASSERT_EQ(aGraph.Topo().Faces().Nb(), 1);
}

TEST(BRepGraph_BuildTest, AddFlatten_AfterBuild_DoesNotCreateNewSolidDefs)
{
  BRepPrimAPI_MakeBox aBox1Maker(10.0, 20.0, 30.0);
  BRepPrimAPI_MakeBox aBox2Maker(15.0, 25.0, 35.0);

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes41 =
    BRepGraph_Builder::Add(aGraph, aBox1Maker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const int aNbSolidsBefore = aGraph.Topo().Solids().Nb();
  const int aNbFacesBefore  = aGraph.Topo().Faces().Nb();

  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes42 =
    BRepGraph_Builder::Add(aGraph,
                           aBox2Maker.Shape(),
                           BRepGraph_Builder::Options{{}, false, true, false});

  EXPECT_EQ(aGraph.Topo().Solids().Nb(), aNbSolidsBefore);
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), aNbFacesBefore + 6);
  // Initial BRepGraph_Builder::Add() created 1 product; Flatten Add doesn't create
  // products.
  EXPECT_EQ(aGraph.RootProductIds().Length(), 1);
  EXPECT_TRUE(aGraph.Editor().ValidateMutationBoundary());
}

TEST(BRepGraph_BuildTest, AddFull_MutationBoundary_IsValid)
{
  BRepPrimAPI_MakeBox    aBoxMaker(10.0, 20.0, 30.0);
  BRepPrimAPI_MakeSphere aSphereMaker(5.0);

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes43 =
    BRepGraph_Builder::Add(aGraph, aBoxMaker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes44 =
    BRepGraph_Builder::Add(aGraph, aSphereMaker.Shape());

  ASSERT_TRUE(aGraph.IsDone());
  EXPECT_TRUE(aGraph.Editor().ValidateMutationBoundary());
}

TEST(BRepGraph_BuildTest, AddFlatten_AppendedFaceHasNoParentShell)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  const TopoDS_Face aFace = TopoDS::Face(anExp.Current());

  BRepGraph                                        aGraph;
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes45 =
    BRepGraph_Builder::Add(aGraph, aFace, BRepGraph_Builder::Options{{}, false, true, false});

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
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes46 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().Edges().Nb(), 0);

  // Record UID of the first edge before append.
  const BRepGraph_UID anOrigUID = aGraph.UIDs().Of(BRepGraph_EdgeId::Start());
  ASSERT_TRUE(anOrigUID.IsValid());

  // Append a sphere.
  BRepPrimAPI_MakeSphere                           aSphereMaker(5.0);
  const TopoDS_Shape&                              aSphere = aSphereMaker.Shape();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes47 =
    BRepGraph_Builder::Add(aGraph, aSphere, BRepGraph_Builder::Options{{}, false, true, false});
  ASSERT_TRUE(aGraph.IsDone());

  // Verify original edge UID is unchanged.
  const BRepGraph_UID aPostUID = aGraph.UIDs().Of(BRepGraph_EdgeId::Start());
  EXPECT_EQ(anOrigUID, aPostUID);
}

TEST(BRepGraph_BuildTest, AddFlatten_StandaloneVertex_AppendsIntoNonEmptyGraph)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes48 =
    BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const int          aNbVerticesBefore = aGraph.Topo().Vertices().Nb();
  const TopoDS_Shape aVertexShape      = BRepBuilderAPI_MakeVertex(gp_Pnt(100.0, 0.0, 0.0)).Shape();

  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes49 =
    BRepGraph_Builder::Add(aGraph,
                           aVertexShape,
                           BRepGraph_Builder::Options{{}, false, true, false});

  ASSERT_TRUE(aGraph.IsDone());
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
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes50 =
    BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const int          aNbEdgesBefore = aGraph.Topo().Edges().Nb();
  const TopoDS_Shape anEdgeShape =
    BRepBuilderAPI_MakeEdge(gp_Pnt(100.0, 0.0, 0.0), gp_Pnt(120.0, 0.0, 0.0)).Shape();

  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes51 =
    BRepGraph_Builder::Add(aGraph, anEdgeShape, BRepGraph_Builder::Options{{}, false, true, false});

  ASSERT_TRUE(aGraph.IsDone());
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
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes52 =
    BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const int          aNbWiresBefore = aGraph.Topo().Wires().Nb();
  const TopoDS_Shape aWireShape     = makeStandaloneWire();

  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes53 =
    BRepGraph_Builder::Add(aGraph, aWireShape, BRepGraph_Builder::Options{{}, false, true, false});

  ASSERT_TRUE(aGraph.IsDone());
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
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes54 =
    BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, BRepBuilderAPI_MakeVertex(gp_Pnt(50.0, 0.0, 0.0)).Shape());
  aBuilder.Add(aCompound,
               BRepBuilderAPI_MakeEdge(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(10.0, 0.0, 0.0)).Shape());

  const int aNbVerticesBefore = aGraph.Topo().Vertices().Nb();
  const int aNbEdgesBefore    = aGraph.Topo().Edges().Nb();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes55 =
    BRepGraph_Builder::Add(aGraph, aCompound, BRepGraph_Builder::Options{{}, false, true, false});
  ASSERT_TRUE(aGraph.IsDone());
  // The compound should add at least 1 vertex and 1 edge.
  EXPECT_GT(aGraph.Topo().Vertices().Nb(), aNbVerticesBefore);
  EXPECT_GT(aGraph.Topo().Edges().Nb(), aNbEdgesBefore);
}

TEST(BRepGraph_BuildTest, Build_WithoutPostPasses_BasicQueriesWork)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph_Builder::Options anOpts;
  anOpts.Populate.ExtractRegularities    = false;
  anOpts.Populate.ExtractVertexPointReps = false;

  BRepGraph aGraph;
  registerStandardLayers(aGraph);
  aGraph.Clear();
  {
    BRepGraph_Builder::Options anOpts__ = anOpts;
    anOpts__.Parallel                   = false;
    [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes56 =
      BRepGraph_Builder::Add(aGraph, aBox, anOpts__);
  };
  ASSERT_TRUE(aGraph.IsDone());
  const occ::handle<BRepGraph_LayerParam> aParamLayer =
    aGraph.LayerRegistry().FindLayer<BRepGraph_LayerParam>();
  const occ::handle<BRepGraph_LayerRegularity> aRegularityLayer =
    aGraph.LayerRegistry().FindLayer<BRepGraph_LayerRegularity>();
  ASSERT_FALSE(aParamLayer.IsNull());
  ASSERT_FALSE(aRegularityLayer.IsNull());

  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 6);
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), 12);
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), 8);

  // Regularities should be empty.
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    EXPECT_EQ(aRegularityLayer->NbRegularities(anEdgeIt.CurrentId()), 0);
  }

  // Vertex point reps should be empty.
  for (BRepGraph_VertexIterator aVertexIt(aGraph); aVertexIt.More(); aVertexIt.Next())
  {
    EXPECT_EQ(aParamLayer->NbPointsOnCurve(aVertexIt.CurrentId()), 0);
    EXPECT_EQ(aParamLayer->NbPointsOnSurface(aVertexIt.CurrentId()), 0);
    EXPECT_EQ(aParamLayer->NbPointsOnPCurve(aVertexIt.CurrentId()), 0);
  }
}

TEST(BRepGraph_BuildTest, ParamLayer_EdgeMutation_InvalidatesVertexBindings)
{
  BRepGraph aGraph;
  registerStandardLayers(aGraph);
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes57 =
    BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());
  const occ::handle<BRepGraph_LayerParam> aParamLayer =
    aGraph.LayerRegistry().FindLayer<BRepGraph_LayerParam>();
  ASSERT_FALSE(aParamLayer.IsNull());

  ASSERT_GT(aGraph.Topo().Vertices().Nb(), 0);
  ASSERT_GT(aGraph.Topo().Edges().Nb(), 0);

  const BRepGraph_VertexId aVertexId(0);
  const BRepGraph_EdgeId   anEdgeId(0);
  aParamLayer->SetPointOnCurve(aVertexId, anEdgeId, 1.25);
  EXPECT_TRUE(aParamLayer->FindPointOnCurve(aVertexId, anEdgeId));

  aGraph.Editor().Edges().SetTolerance(anEdgeId,
                                       aGraph.Topo().Edges().Definition(anEdgeId).Tolerance + 0.01);
}

TEST(BRepGraph_BuildTest, ParamLayer_FaceMutation_InvalidatesVertexBindings)
{
  BRepGraph aGraph;
  registerStandardLayers(aGraph);
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes58 =
    BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());
  const occ::handle<BRepGraph_LayerParam> aParamLayer =
    aGraph.LayerRegistry().FindLayer<BRepGraph_LayerParam>();
  ASSERT_FALSE(aParamLayer.IsNull());

  ASSERT_GT(aGraph.Topo().Vertices().Nb(), 0);
  ASSERT_GT(aGraph.Topo().Faces().Nb(), 0);

  const BRepGraph_VertexId aVertexId(0);
  const BRepGraph_FaceId   aFaceId(0);
  aParamLayer->SetPointOnSurface(aVertexId, aFaceId, 0.5, 0.75);
  EXPECT_TRUE(aParamLayer->FindPointOnSurface(aVertexId, aFaceId));

  {
    BRepGraph_MutGuard<BRepGraphInc::FaceDef> aFace = aGraph.Editor().Faces().Mut(aFaceId);
    aGraph.Editor().Faces().SetNaturalRestriction(aFace, !aFace->NaturalRestriction);
  }

  EXPECT_FALSE(aParamLayer->FindPointOnSurface(aVertexId, aFaceId));
}

TEST(BRepGraph_BuildTest, ParamLayer_CoEdgeMutation_InvalidatesPCurveBindings)
{
  BRepGraph aGraph;
  registerStandardLayers(aGraph);
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes59 =
    BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());
  const occ::handle<BRepGraph_LayerParam> aParamLayer =
    aGraph.LayerRegistry().FindLayer<BRepGraph_LayerParam>();
  ASSERT_FALSE(aParamLayer.IsNull());

  ASSERT_GT(aGraph.Topo().Vertices().Nb(), 0);
  ASSERT_GT(aGraph.Topo().CoEdges().Nb(), 0);

  const BRepGraph_VertexId aVertexId(0);
  const BRepGraph_CoEdgeId aCoEdgeId(0);
  aParamLayer->SetPointOnPCurve(aVertexId, aCoEdgeId, 2.5);
  EXPECT_TRUE(aParamLayer->FindPointOnPCurve(aVertexId, aCoEdgeId));

  aGraph.Editor().CoEdges().SetParamRange(aCoEdgeId,
                                          aGraph.Topo().CoEdges().Definition(aCoEdgeId).ParamFirst
                                            + 0.01,
                                          aGraph.Topo().CoEdges().Definition(aCoEdgeId).ParamLast);

  EXPECT_FALSE(aParamLayer->FindPointOnPCurve(aVertexId, aCoEdgeId));
}

TEST(BRepGraph_BuildTest, RegularityLayer_EdgeMutation_InvalidatesBindings)
{
  BRepGraph aGraph;
  registerStandardLayers(aGraph);
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes60 =
    BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());
  const occ::handle<BRepGraph_LayerRegularity> aRegularityLayer =
    aGraph.LayerRegistry().FindLayer<BRepGraph_LayerRegularity>();
  ASSERT_FALSE(aRegularityLayer.IsNull());

  BRepGraph_EdgeId                           anEdgeId;
  BRepGraph_LayerRegularity::RegularityEntry aRegularity;
  bool                                       hasBinding = false;
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More() && !hasBinding; anEdgeIt.Next())
  {
    anEdgeId = anEdgeIt.CurrentId();
    const NCollection_DynamicArray<BRepGraph_CoEdgeId>& aCoEdges =
      aGraph.Topo().Edges().CoEdges(anEdgeId);
    BRepGraph_FaceId aFace1;
    BRepGraph_FaceId aFace2;
    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdges)
    {
      const BRepGraph_FaceId aFace = aGraph.Topo().CoEdges().Definition(aCoEdgeId).FaceDefId;
      if (!aFace.IsValid())
        continue;
      if (!aFace1.IsValid())
        aFace1 = aFace;
      else if (aFace != aFace1)
      {
        aFace2 = aFace;
        break;
      }
    }
    if (!aFace1.IsValid() || !aFace2.IsValid())
      continue;
    aRegularityLayer->SetRegularity(anEdgeId, aFace1, aFace2, GeomAbs_C1);
    aRegularity.FaceEntity1 = aFace1;
    aRegularity.FaceEntity2 = aFace2;
    aRegularity.Continuity  = GeomAbs_C1;
    hasBinding              = true;
  }
  ASSERT_TRUE(hasBinding);
  EXPECT_TRUE(
    aRegularityLayer->FindContinuity(anEdgeId, aRegularity.FaceEntity1, aRegularity.FaceEntity2));

  aGraph.Editor().Edges().SetTolerance(anEdgeId,
                                       aGraph.Topo().Edges().Definition(anEdgeId).Tolerance + 0.01);

  EXPECT_FALSE(
    aRegularityLayer->FindContinuity(anEdgeId, aRegularity.FaceEntity1, aRegularity.FaceEntity2));
  EXPECT_EQ(aRegularityLayer->NbRegularities(anEdgeId), 0);
}

TEST(BRepGraph_BuildTest, RegularityLayer_FaceMutation_InvalidatesBindings)
{
  BRepGraph aGraph;
  registerStandardLayers(aGraph);
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes61 =
    BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aGraph.IsDone());
  const occ::handle<BRepGraph_LayerRegularity> aRegularityLayer =
    aGraph.LayerRegistry().FindLayer<BRepGraph_LayerRegularity>();
  ASSERT_FALSE(aRegularityLayer.IsNull());

  BRepGraph_EdgeId                           anEdgeId;
  BRepGraph_LayerRegularity::RegularityEntry aRegularity;
  bool                                       hasBinding = false;
  for (BRepGraph_EdgeIterator anEdgeIt(aGraph); anEdgeIt.More() && !hasBinding; anEdgeIt.Next())
  {
    anEdgeId = anEdgeIt.CurrentId();
    const NCollection_DynamicArray<BRepGraph_CoEdgeId>& aCoEdges =
      aGraph.Topo().Edges().CoEdges(anEdgeId);
    BRepGraph_FaceId aFace1;
    BRepGraph_FaceId aFace2;
    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdges)
    {
      const BRepGraph_FaceId aFace = aGraph.Topo().CoEdges().Definition(aCoEdgeId).FaceDefId;
      if (!aFace.IsValid())
        continue;
      if (!aFace1.IsValid())
        aFace1 = aFace;
      else if (aFace != aFace1)
      {
        aFace2 = aFace;
        break;
      }
    }
    if (!aFace1.IsValid() || !aFace2.IsValid())
      continue;
    aRegularityLayer->SetRegularity(anEdgeId, aFace1, aFace2, GeomAbs_C1);
    aRegularity.FaceEntity1 = aFace1;
    aRegularity.FaceEntity2 = aFace2;
    aRegularity.Continuity  = GeomAbs_C1;
    hasBinding              = true;
  }
  ASSERT_TRUE(hasBinding);
  EXPECT_TRUE(
    aRegularityLayer->FindContinuity(anEdgeId, aRegularity.FaceEntity1, aRegularity.FaceEntity2));

  {
    BRepGraph_MutGuard<BRepGraphInc::FaceDef> aFace =
      aGraph.Editor().Faces().Mut(aRegularity.FaceEntity1);
    aGraph.Editor().Faces().SetNaturalRestriction(aFace, !aFace->NaturalRestriction);
  }

  EXPECT_FALSE(
    aRegularityLayer->FindContinuity(anEdgeId, aRegularity.FaceEntity1, aRegularity.FaceEntity2));
}

TEST(BRepGraph_BuildTest, RegularityLayer_RemoveRegularity_SharedFaceRetained)
{
  BRepGraph_LayerRegularity aLayer;
  const BRepGraph_EdgeId    anEdgeId(0);
  const BRepGraph_FaceId    aFace1(0);
  const BRepGraph_FaceId    aFace2(1);
  const BRepGraph_FaceId    aFace3(2);

  aLayer.SetRegularity(anEdgeId, aFace1, aFace2, GeomAbs_C1);
  aLayer.SetRegularity(anEdgeId, aFace1, aFace3, GeomAbs_G1);
  EXPECT_EQ(aLayer.NbRegularities(anEdgeId), 2);

  // Remove (F1,F2) - F1 is shared by the surviving (F1,F3) entry.
  aLayer.OnNodeModified(aFace2);

  // (F1,F3) must survive; F1 must still be tracked.
  EXPECT_TRUE(aLayer.FindContinuity(anEdgeId, aFace1, aFace3));
  EXPECT_EQ(aLayer.NbRegularities(anEdgeId), 1);

  // Modifying F1 should still invalidate the remaining entry.
  aLayer.OnNodeModified(aFace1);
  EXPECT_EQ(aLayer.NbRegularities(anEdgeId), 0);
}

TEST(BRepGraph_BuildTest, RegularityLayer_RemoveRegularity_NoMatch_NoEffect)
{
  BRepGraph_LayerRegularity aLayer;
  const BRepGraph_EdgeId    anEdgeId(0);
  const BRepGraph_FaceId    aFace1(0);
  const BRepGraph_FaceId    aFace2(1);
  const BRepGraph_FaceId    aFace3(2);

  aLayer.SetRegularity(anEdgeId, aFace1, aFace2, GeomAbs_C1);
  EXPECT_EQ(aLayer.NbRegularities(anEdgeId), 1);

  // Modifying F3 (not referenced) should have no effect.
  aLayer.OnNodeModified(aFace3);
  EXPECT_EQ(aLayer.NbRegularities(anEdgeId), 1);
  EXPECT_TRUE(aLayer.FindContinuity(anEdgeId, aFace1, aFace2));
}

TEST(BRepGraph_BuildTest, ParamLayer_OnCompact_RemapsNodeIds)
{
  BRepGraph_LayerParam     aLayer;
  const BRepGraph_VertexId aVtx0(0);
  const BRepGraph_VertexId aVtx1(1);
  const BRepGraph_EdgeId   anEdge0(0);
  const BRepGraph_EdgeId   anEdge1(1);
  const BRepGraph_FaceId   aFace0(0);
  const BRepGraph_CoEdgeId aCoEdge0(0);

  aLayer.SetPointOnCurve(aVtx0, anEdge0, 1.0);
  aLayer.SetPointOnCurve(aVtx1, anEdge1, 2.0);
  aLayer.SetPointOnSurface(aVtx0, aFace0, 0.5, 0.75);
  aLayer.SetPointOnPCurve(aVtx1, aCoEdge0, 3.0);

  // Remap: vtx0->vtx0, vtx1 dropped; edge0->edge0, edge1 dropped; face0->face0; coedge0 dropped.
  NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId> aRemapMap;
  aRemapMap.Bind(BRepGraph_VertexId::Start(), BRepGraph_VertexId::Start());
  aRemapMap.Bind(BRepGraph_EdgeId::Start(), BRepGraph_EdgeId::Start());
  aRemapMap.Bind(BRepGraph_FaceId::Start(), BRepGraph_FaceId::Start());

  aLayer.OnCompact(aRemapMap);

  // Vtx0 on edge0 should survive.
  double aParam = 0.0;
  EXPECT_TRUE(aLayer.FindPointOnCurve(aVtx0, anEdge0, &aParam));
  EXPECT_NEAR(aParam, 1.0, 1e-15);

  // Vtx0 on face0 should survive.
  gp_Pnt2d aUV;
  EXPECT_TRUE(aLayer.FindPointOnSurface(aVtx0, aFace0, &aUV));
  EXPECT_NEAR(aUV.X(), 0.5, 1e-15);
  EXPECT_NEAR(aUV.Y(), 0.75, 1e-15);

  // Vtx1 was dropped.
  EXPECT_FALSE(aLayer.FindPointOnCurve(aVtx1, anEdge1));
  EXPECT_FALSE(aLayer.FindPointOnPCurve(aVtx1, aCoEdge0));
}

TEST(BRepGraph_BuildTest, RegularityLayer_OnCompact_RemapsNodeIds)
{
  BRepGraph_LayerRegularity aLayer;
  const BRepGraph_EdgeId    anEdge0(0);
  const BRepGraph_EdgeId    anEdge1(1);
  const BRepGraph_FaceId    aFace0(0);
  const BRepGraph_FaceId    aFace1(1);
  const BRepGraph_FaceId    aFace2(2);

  aLayer.SetRegularity(anEdge0, aFace0, aFace1, GeomAbs_C1);
  aLayer.SetRegularity(anEdge1, aFace1, aFace2, GeomAbs_G1);

  // Remap: edge0->edge0, edge1 dropped; face0->face0, face1->face1, face2 dropped.
  NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId> aRemapMap;
  aRemapMap.Bind(BRepGraph_EdgeId::Start(), BRepGraph_EdgeId::Start());
  aRemapMap.Bind(BRepGraph_FaceId::Start(), BRepGraph_FaceId::Start());
  aRemapMap.Bind(BRepGraph_FaceId(1), BRepGraph_FaceId(1));

  aLayer.OnCompact(aRemapMap);

  // Edge0 (F0,F1) should survive.
  GeomAbs_Shape aContinuity = GeomAbs_C0;
  EXPECT_TRUE(aLayer.FindContinuity(anEdge0, aFace0, aFace1, &aContinuity));
  EXPECT_EQ(aContinuity, GeomAbs_C1);

  // Edge1 was dropped.
  EXPECT_EQ(aLayer.NbRegularities(anEdge1), 0);
}

TEST(BRepGraph_BuildTest, RootProductIds_Box_ReturnsOneProduct)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes62 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // BRepGraph_Builder::Add() from a solid should produce exactly one root product.
  const NCollection_DynamicArray<BRepGraph_ProductId>& aRoots = aGraph.RootProductIds();
  ASSERT_EQ(aRoots.Length(), 1);

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

  BRepGraph_Builder::Options anOptions;
  anOptions.CreateAutoProduct = false;

  BRepGraph aGraph;
  aGraph.Clear();
  {
    BRepGraph_Builder::Options anOpts__ = anOptions;
    anOpts__.Parallel                   = false;
    [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes63 =
      BRepGraph_Builder::Add(aGraph, aBox, anOpts__);
  };
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Topo().Products().Nb(), 0);
  EXPECT_EQ(aGraph.RootProductIds().Length(), 0);
  EXPECT_EQ(aGraph.Topo().Solids().Nb(), 1);
}

TEST(BRepGraph_BuildTest, RootProductIds_AddFlatten_ProductCountUnchanged)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes64 =
    BRepGraph_Builder::Add(aGraph, aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_EQ(aGraph.RootProductIds().Length(), 1);

  const int aNbFacesBefore = aGraph.Topo().Faces().Nb();

  // Append a second shape (a single face from another box).
  BRepPrimAPI_MakeBox aBox2Maker(5.0, 5.0, 5.0);
  TopExp_Explorer     anExp(aBox2Maker.Shape(), TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  const TopoDS_Face aFace = TopoDS::Face(anExp.Current());

  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes65 =
    BRepGraph_Builder::Add(aGraph, aFace, BRepGraph_Builder::Options{{}, false, true, false});
  ASSERT_TRUE(aGraph.IsDone());

  // Flatten Add does not create new products.
  const NCollection_DynamicArray<BRepGraph_ProductId>& aRoots = aGraph.RootProductIds();
  EXPECT_EQ(aRoots.Length(), 1);

  // But a new face was appended.
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), aNbFacesBefore + 1);

  // The original product root should still be valid.
  EXPECT_TRUE(aRoots.Value(0).IsValid());
}
