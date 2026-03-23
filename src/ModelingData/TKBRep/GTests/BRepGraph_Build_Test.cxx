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
#include <BRepGraph.hxx>
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_GeomView.hxx>
#include <BRepGraph_UsagesView.hxx>
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

// =============================================================================
// Helper: count total (non-unique) sub-shape occurrences via TopExp_Explorer
// =============================================================================

static int countNonUnique(const TopoDS_Shape& theShape, TopAbs_ShapeEnum theType)
{
  int aCount = 0;
  for (TopExp_Explorer anExp(theShape, theType); anExp.More(); anExp.Next())
  {
    ++aCount;
  }
  return aCount;
}

// =============================================================================
// Sphere tests
// =============================================================================

TEST(BRepGraphBuildTest, Sphere_IsDone)
{
  BRepPrimAPI_MakeSphere aMaker(10.0);
  const TopoDS_Shape aShape = aMaker.Shape();
  ASSERT_TRUE(aMaker.IsDone());

  BRepGraph aGraph;
  aGraph.Build(aShape);
  EXPECT_TRUE(aGraph.IsDone());
}

TEST(BRepGraphBuildTest, Sphere_DefCounts_MatchTopExp)
{
  BRepPrimAPI_MakeSphere aMaker(10.0);
  const TopoDS_Shape aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Defs().NbSolids(),  countUnique(aShape, TopAbs_SOLID));
  EXPECT_EQ(aGraph.Defs().NbShells(),  countUnique(aShape, TopAbs_SHELL));
  EXPECT_EQ(aGraph.Defs().NbFaces(),   countUnique(aShape, TopAbs_FACE));
  EXPECT_EQ(aGraph.Defs().NbWires(),   countUnique(aShape, TopAbs_WIRE));
  EXPECT_EQ(aGraph.Defs().NbEdges(),   countUnique(aShape, TopAbs_EDGE));
  EXPECT_EQ(aGraph.Defs().NbVertices(), countUnique(aShape, TopAbs_VERTEX));
}

TEST(BRepGraphBuildTest, Sphere_SurfaceType)
{
  BRepPrimAPI_MakeSphere aMaker(5.0);
  const TopoDS_Shape aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GE(aGraph.Geom().NbSurfaces(), 1);

  bool aHasSpherical = false;
  for (int anIdx = 0; anIdx < aGraph.Geom().NbSurfaces(); ++anIdx)
  {
    const Handle(Geom_Surface)& aSurf = aGraph.Geom().Surface(anIdx).Surface;
    if (!aSurf.IsNull() && aSurf->DynamicType() == STANDARD_TYPE(Geom_SphericalSurface))
    {
      aHasSpherical = true;
    }
  }
  EXPECT_TRUE(aHasSpherical);
}

TEST(BRepGraphBuildTest, Sphere_HasDegenerateEdges)
{
  BRepPrimAPI_MakeSphere aMaker(8.0);
  const TopoDS_Shape aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);
  ASSERT_TRUE(aGraph.IsDone());

  // A sphere has degenerate edges at poles.
  int aDegCount = 0;
  for (int anIdx = 0; anIdx < aGraph.Defs().NbEdges(); ++anIdx)
  {
    if (aGraph.Defs().Edge(anIdx).IsDegenerate)
    {
      ++aDegCount;
    }
  }
  EXPECT_GE(aDegCount, 1);
}

// =============================================================================
// Cylinder tests
// =============================================================================

TEST(BRepGraphBuildTest, Cylinder_IsDone)
{
  BRepPrimAPI_MakeCylinder aMaker(5.0, 20.0);
  const TopoDS_Shape aShape = aMaker.Shape();
  ASSERT_TRUE(aMaker.IsDone());

  BRepGraph aGraph;
  aGraph.Build(aShape);
  EXPECT_TRUE(aGraph.IsDone());
}

TEST(BRepGraphBuildTest, Cylinder_DefCounts_MatchTopExp)
{
  BRepPrimAPI_MakeCylinder aMaker(5.0, 20.0);
  const TopoDS_Shape aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Defs().NbFaces(),   countUnique(aShape, TopAbs_FACE));
  EXPECT_EQ(aGraph.Defs().NbEdges(),   countUnique(aShape, TopAbs_EDGE));
  EXPECT_EQ(aGraph.Defs().NbVertices(), countUnique(aShape, TopAbs_VERTEX));
}

TEST(BRepGraphBuildTest, Cylinder_SurfaceType)
{
  BRepPrimAPI_MakeCylinder aMaker(5.0, 20.0);
  const TopoDS_Shape aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);
  ASSERT_TRUE(aGraph.IsDone());

  bool aHasCylindrical = false;
  for (int anIdx = 0; anIdx < aGraph.Geom().NbSurfaces(); ++anIdx)
  {
    const Handle(Geom_Surface)& aSurf = aGraph.Geom().Surface(anIdx).Surface;
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

TEST(BRepGraphBuildTest, Cone_IsDone)
{
  BRepPrimAPI_MakeCone aMaker(10.0, 0.0, 15.0);
  const TopoDS_Shape aShape = aMaker.Shape();
  ASSERT_TRUE(aMaker.IsDone());

  BRepGraph aGraph;
  aGraph.Build(aShape);
  EXPECT_TRUE(aGraph.IsDone());
}

TEST(BRepGraphBuildTest, Cone_DefCounts_MatchTopExp)
{
  BRepPrimAPI_MakeCone aMaker(10.0, 0.0, 15.0);
  const TopoDS_Shape aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Defs().NbFaces(),   countUnique(aShape, TopAbs_FACE));
  EXPECT_EQ(aGraph.Defs().NbEdges(),   countUnique(aShape, TopAbs_EDGE));
  EXPECT_EQ(aGraph.Defs().NbVertices(), countUnique(aShape, TopAbs_VERTEX));
}

TEST(BRepGraphBuildTest, Cone_SurfaceType)
{
  BRepPrimAPI_MakeCone aMaker(10.0, 5.0, 15.0);
  const TopoDS_Shape aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);
  ASSERT_TRUE(aGraph.IsDone());

  bool aHasConical = false;
  for (int anIdx = 0; anIdx < aGraph.Geom().NbSurfaces(); ++anIdx)
  {
    const Handle(Geom_Surface)& aSurf = aGraph.Geom().Surface(anIdx).Surface;
    if (!aSurf.IsNull() && aSurf->DynamicType() == STANDARD_TYPE(Geom_ConicalSurface))
    {
      aHasConical = true;
    }
  }
  EXPECT_TRUE(aHasConical);
}

TEST(BRepGraphBuildTest, Cone_HasDegenerateEdge)
{
  // Cone with top radius 0 => apex degenerate edge.
  BRepPrimAPI_MakeCone aMaker(10.0, 0.0, 15.0);
  const TopoDS_Shape aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);
  ASSERT_TRUE(aGraph.IsDone());

  int aDegCount = 0;
  for (int anIdx = 0; anIdx < aGraph.Defs().NbEdges(); ++anIdx)
  {
    if (aGraph.Defs().Edge(anIdx).IsDegenerate)
    {
      ++aDegCount;
    }
  }
  EXPECT_GE(aDegCount, 1);
}

// =============================================================================
// Torus tests
// =============================================================================

TEST(BRepGraphBuildTest, Torus_IsDone)
{
  BRepPrimAPI_MakeTorus aMaker(20.0, 5.0);
  const TopoDS_Shape aShape = aMaker.Shape();
  ASSERT_TRUE(aMaker.IsDone());

  BRepGraph aGraph;
  aGraph.Build(aShape);
  EXPECT_TRUE(aGraph.IsDone());
}

TEST(BRepGraphBuildTest, Torus_DefCounts_MatchTopExp)
{
  BRepPrimAPI_MakeTorus aMaker(20.0, 5.0);
  const TopoDS_Shape aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Defs().NbFaces(),   countUnique(aShape, TopAbs_FACE));
  EXPECT_EQ(aGraph.Defs().NbEdges(),   countUnique(aShape, TopAbs_EDGE));
  EXPECT_EQ(aGraph.Defs().NbVertices(), countUnique(aShape, TopAbs_VERTEX));
}

TEST(BRepGraphBuildTest, Torus_SurfaceType)
{
  BRepPrimAPI_MakeTorus aMaker(20.0, 5.0);
  const TopoDS_Shape aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);
  ASSERT_TRUE(aGraph.IsDone());

  bool aHasToroidal = false;
  for (int anIdx = 0; anIdx < aGraph.Geom().NbSurfaces(); ++anIdx)
  {
    const Handle(Geom_Surface)& aSurf = aGraph.Geom().Surface(anIdx).Surface;
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

TEST(BRepGraphBuildTest, Wedge_IsDone)
{
  BRepPrimAPI_MakeWedge aMaker(10.0, 10.0, 10.0, 5.0);
  const TopoDS_Shape aShape = aMaker.Shape();
  ASSERT_TRUE(aMaker.IsDone());

  BRepGraph aGraph;
  aGraph.Build(aShape);
  EXPECT_TRUE(aGraph.IsDone());
}

TEST(BRepGraphBuildTest, Wedge_DefCounts_MatchTopExp)
{
  BRepPrimAPI_MakeWedge aMaker(10.0, 10.0, 10.0, 5.0);
  const TopoDS_Shape aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Defs().NbSolids(),  countUnique(aShape, TopAbs_SOLID));
  EXPECT_EQ(aGraph.Defs().NbShells(),  countUnique(aShape, TopAbs_SHELL));
  EXPECT_EQ(aGraph.Defs().NbFaces(),   countUnique(aShape, TopAbs_FACE));
  EXPECT_EQ(aGraph.Defs().NbWires(),   countUnique(aShape, TopAbs_WIRE));
  EXPECT_EQ(aGraph.Defs().NbEdges(),   countUnique(aShape, TopAbs_EDGE));
  EXPECT_EQ(aGraph.Defs().NbVertices(), countUnique(aShape, TopAbs_VERTEX));
}

TEST(BRepGraphBuildTest, Wedge_AllPlanarSurfaces)
{
  BRepPrimAPI_MakeWedge aMaker(10.0, 10.0, 10.0, 5.0);
  const TopoDS_Shape aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);
  ASSERT_TRUE(aGraph.IsDone());

  for (int anIdx = 0; anIdx < aGraph.Geom().NbSurfaces(); ++anIdx)
  {
    const Handle(Geom_Surface)& aSurf = aGraph.Geom().Surface(anIdx).Surface;
    ASSERT_FALSE(aSurf.IsNull());
    EXPECT_TRUE(aSurf->DynamicType() == STANDARD_TYPE(Geom_Plane))
      << "Surface index " << anIdx << " is not a Geom_Plane";
  }
}

// =============================================================================
// Usage count tests for primitives
// =============================================================================

TEST(BRepGraphBuildTest, Sphere_UsageCounts_MatchDefCounts)
{
  BRepPrimAPI_MakeSphere aMaker(10.0);
  const TopoDS_Shape aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);
  ASSERT_TRUE(aGraph.IsDone());

  // For a single solid, usages >= defs (usages count occurrences in hierarchy).
  EXPECT_GE(aGraph.Usages().NbFaces(),   aGraph.Defs().NbFaces());
  EXPECT_GE(aGraph.Usages().NbEdges(),   aGraph.Defs().NbEdges());
  EXPECT_GE(aGraph.Usages().NbVertices(), aGraph.Defs().NbVertices());
}

TEST(BRepGraphBuildTest, Cylinder_UsageCounts_MatchNonUniqueTopExp)
{
  BRepPrimAPI_MakeCylinder aMaker(5.0, 20.0);
  const TopoDS_Shape aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Usages().NbFaces(),   countNonUnique(aShape, TopAbs_FACE));
  EXPECT_EQ(aGraph.Usages().NbEdges(),   countNonUnique(aShape, TopAbs_EDGE));
  EXPECT_EQ(aGraph.Usages().NbVertices(), countNonUnique(aShape, TopAbs_VERTEX));
}

// =============================================================================
// Compound builds
// =============================================================================

TEST(BRepGraphBuildTest, Compound_TwoPrimitives_IsDone)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 10.0, 10.0);
  const TopoDS_Shape aBox = aBoxMaker.Shape();

  BRepPrimAPI_MakeSphere aSphereMaker(5.0);
  const TopoDS_Shape aSphere = aSphereMaker.Shape();

  BRep_Builder aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBox);
  aBuilder.Add(aCompound, aSphere);

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  EXPECT_TRUE(aGraph.IsDone());
}

TEST(BRepGraphBuildTest, Compound_TwoPrimitives_DefCountsAddUp)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 10.0, 10.0);
  const TopoDS_Shape aBox = aBoxMaker.Shape();

  BRepPrimAPI_MakeSphere aSphereMaker(5.0);
  const TopoDS_Shape aSphere = aSphereMaker.Shape();

  BRep_Builder aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBox);
  aBuilder.Add(aCompound, aSphere);

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Defs().NbFaces(),   countUnique(aCompound, TopAbs_FACE));
  EXPECT_EQ(aGraph.Defs().NbEdges(),   countUnique(aCompound, TopAbs_EDGE));
  EXPECT_EQ(aGraph.Defs().NbVertices(), countUnique(aCompound, TopAbs_VERTEX));
}

TEST(BRepGraphBuildTest, Compound_ThreeBoxes_DefCounts)
{
  BRep_Builder aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  for (int anIdx = 0; anIdx < 3; ++anIdx)
  {
    BRepPrimAPI_MakeBox aMaker(5.0 * (anIdx + 1), 10.0, 10.0);
    const TopoDS_Shape aBox = aMaker.Shape();
    aBuilder.Add(aCompound, aBox);
  }

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Defs().NbSolids(), countUnique(aCompound, TopAbs_SOLID));
  EXPECT_EQ(aGraph.Defs().NbFaces(),  countUnique(aCompound, TopAbs_FACE));
  EXPECT_EQ(aGraph.Defs().NbEdges(),  countUnique(aCompound, TopAbs_EDGE));
}

TEST(BRepGraphBuildTest, Compound_Nested_DefCounts)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 10.0, 10.0);
  const TopoDS_Shape aBox = aBoxMaker.Shape();

  BRepPrimAPI_MakeCylinder aCylMaker(3.0, 10.0);
  const TopoDS_Shape aCyl = aCylMaker.Shape();

  BRep_Builder aBuilder;
  TopoDS_Compound anInner;
  aBuilder.MakeCompound(anInner);
  aBuilder.Add(anInner, aBox);

  TopoDS_Compound anOuter;
  aBuilder.MakeCompound(anOuter);
  aBuilder.Add(anOuter, anInner);
  aBuilder.Add(anOuter, aCyl);

  BRepGraph aGraph;
  aGraph.Build(anOuter);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Defs().NbFaces(),   countUnique(anOuter, TopAbs_FACE));
  EXPECT_EQ(aGraph.Defs().NbEdges(),   countUnique(anOuter, TopAbs_EDGE));
  EXPECT_EQ(aGraph.Defs().NbVertices(), countUnique(anOuter, TopAbs_VERTEX));
}

// =============================================================================
// Minimal shapes
// =============================================================================

TEST(BRepGraphBuildTest, SinglePlanarFace_IsDone)
{
  gp_Pln aPln;
  BRepBuilderAPI_MakeFace aFaceMaker(aPln);
  const TopoDS_Shape aShape = aFaceMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);
  EXPECT_TRUE(aGraph.IsDone());
}

TEST(BRepGraphBuildTest, SinglePlanarFace_Counts)
{
  gp_Pln aPln;
  BRepBuilderAPI_MakeFace aFaceMaker(aPln);
  const TopoDS_Shape aShape = aFaceMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Defs().NbFaces(), 1);
  EXPECT_EQ(aGraph.Defs().NbSolids(), 0);
  EXPECT_EQ(aGraph.Defs().NbShells(), 0);
  EXPECT_GE(aGraph.Geom().NbSurfaces(), 1);

  // Verify surface is a plane.
  const Handle(Geom_Surface)& aSurf = aGraph.Geom().Surface(0).Surface;
  ASSERT_FALSE(aSurf.IsNull());
  EXPECT_TRUE(aSurf->DynamicType() == STANDARD_TYPE(Geom_Plane));
}

TEST(BRepGraphBuildTest, SingleEdge_HandlesGracefully)
{
  BRepBuilderAPI_MakeEdge anEdgeMaker(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0));
  ASSERT_TRUE(anEdgeMaker.IsDone());
  const TopoDS_Shape aShape = anEdgeMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);

  // BRepGraph is face-level; standalone edges may produce zero counts.
  // Verify it does not crash and returns consistent state.
  EXPECT_EQ(aGraph.Defs().NbFaces(), 0);
  EXPECT_EQ(aGraph.Defs().NbSolids(), 0);
}

TEST(BRepGraphBuildTest, SingleVertex_HandlesGracefully)
{
  BRepBuilderAPI_MakeVertex aVertexMaker(gp_Pnt(1.0, 2.0, 3.0));
  const TopoDS_Shape aShape = aVertexMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);

  // BRepGraph is face-level; standalone vertices may produce zero counts.
  EXPECT_EQ(aGraph.Defs().NbFaces(), 0);
  EXPECT_EQ(aGraph.Defs().NbEdges(), 0);
}

// =============================================================================
// Box cross-validation with TopExp
// =============================================================================

TEST(BRepGraphBuildTest, Box_FaceDefCount_MatchesTopExp)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Defs().NbFaces(), countUnique(aBox, TopAbs_FACE));
  EXPECT_EQ(aGraph.Defs().NbFaces(), 6);
}

TEST(BRepGraphBuildTest, Box_EdgeDefCount_MatchesTopExp)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Defs().NbEdges(), countUnique(aBox, TopAbs_EDGE));
  EXPECT_EQ(aGraph.Defs().NbEdges(), 12);
}

TEST(BRepGraphBuildTest, Box_VertexDefCount_MatchesTopExp)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Defs().NbVertices(), countUnique(aBox, TopAbs_VERTEX));
  EXPECT_EQ(aGraph.Defs().NbVertices(), 8);
}

TEST(BRepGraphBuildTest, Box_VertexPoints_MatchBRepTool)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // Collect all vertex points from TopExp.
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aVertexMap;
  TopExp::MapShapes(aBox, TopAbs_VERTEX, aVertexMap);

  ASSERT_EQ(aGraph.Defs().NbVertices(), aVertexMap.Extent());

  // For each graph vertex, verify that a matching TopExp vertex exists.
  for (int anIdx = 0; anIdx < aGraph.Defs().NbVertices(); ++anIdx)
  {
    const gp_Pnt& aGraphPnt = aGraph.Defs().Vertex(anIdx).Point;
    bool aFound = false;
    for (int aMapIdx = 1; aMapIdx <= aVertexMap.Extent(); ++aMapIdx)
    {
      const TopoDS_Vertex& aVertex = TopoDS::Vertex(aVertexMap(aMapIdx));
      const gp_Pnt aTopExpPnt = BRep_Tool::Pnt(aVertex);
      if (aGraphPnt.Distance(aTopExpPnt) < Precision::Confusion())
      {
        aFound = true;
        break;
      }
    }
    EXPECT_TRUE(aFound) << "Graph vertex " << anIdx << " at ("
      << aGraphPnt.X() << ", " << aGraphPnt.Y() << ", " << aGraphPnt.Z()
      << ") has no matching TopExp vertex";
  }
}

TEST(BRepGraphBuildTest, Box_FaceTolerances_MatchBRepTool)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aFaceMap;
  TopExp::MapShapes(aBox, TopAbs_FACE, aFaceMap);

  // Verify that each graph face tolerance appears in the TopExp set.
  for (int anIdx = 0; anIdx < aGraph.Defs().NbFaces(); ++anIdx)
  {
    const double aGraphTol = aGraph.Defs().Face(anIdx).Tolerance;
    bool aFound = false;
    for (int aMapIdx = 1; aMapIdx <= aFaceMap.Extent(); ++aMapIdx)
    {
      const TopoDS_Face& aFace = TopoDS::Face(aFaceMap(aMapIdx));
      const double aTopExpTol = BRep_Tool::Tolerance(aFace);
      if (std::abs(aGraphTol - aTopExpTol) < Precision::Confusion())
      {
        aFound = true;
        break;
      }
    }
    EXPECT_TRUE(aFound) << "Graph face " << anIdx << " tolerance " << aGraphTol
      << " has no matching TopExp face tolerance";
  }
}

TEST(BRepGraphBuildTest, Box_EdgeTolerances_MatchBRepTool)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> anEdgeMap;
  TopExp::MapShapes(aBox, TopAbs_EDGE, anEdgeMap);

  for (int anIdx = 0; anIdx < aGraph.Defs().NbEdges(); ++anIdx)
  {
    const double aGraphTol = aGraph.Defs().Edge(anIdx).Tolerance;
    bool aFound = false;
    for (int aMapIdx = 1; aMapIdx <= anEdgeMap.Extent(); ++aMapIdx)
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeMap(aMapIdx));
      const double aTopExpTol = BRep_Tool::Tolerance(anEdge);
      if (std::abs(aGraphTol - aTopExpTol) < Precision::Confusion())
      {
        aFound = true;
        break;
      }
    }
    EXPECT_TRUE(aFound) << "Graph edge " << anIdx << " tolerance " << aGraphTol
      << " has no matching TopExp edge tolerance";
  }
}

TEST(BRepGraphBuildTest, Box_EdgeUsageCount_MatchesNonUniqueTopExp)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // Count total non-unique edge occurrences across all wires in all faces.
  const int aNonUniqueEdges = countNonUnique(aBox, TopAbs_EDGE);

  EXPECT_EQ(aGraph.Usages().NbEdges(), aNonUniqueEdges);
}

TEST(BRepGraphBuildTest, Box_VertexUsageCount_MatchesNonUniqueTopExp)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  const int aNonUniqueVertices = countNonUnique(aBox, TopAbs_VERTEX);

  EXPECT_EQ(aGraph.Usages().NbVertices(), aNonUniqueVertices);
}

TEST(BRepGraphBuildTest, Box_AllSurfacesArePlanes)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  ASSERT_EQ(aGraph.Geom().NbSurfaces(), 6);
  for (int anIdx = 0; anIdx < aGraph.Geom().NbSurfaces(); ++anIdx)
  {
    const Handle(Geom_Surface)& aSurf = aGraph.Geom().Surface(anIdx).Surface;
    ASSERT_FALSE(aSurf.IsNull());
    EXPECT_TRUE(aSurf->DynamicType() == STANDARD_TYPE(Geom_Plane))
      << "Surface " << anIdx << " is not Geom_Plane";
  }
}

TEST(BRepGraphBuildTest, Box_NoDegenerateEdges)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  for (int anIdx = 0; anIdx < aGraph.Defs().NbEdges(); ++anIdx)
  {
    EXPECT_FALSE(aGraph.Defs().Edge(anIdx).IsDegenerate)
      << "Box edge " << anIdx << " is degenerate unexpectedly";
  }
}

TEST(BRepGraphBuildTest, Box_EdgeVertexDefsAreValid)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  for (int anIdx = 0; anIdx < aGraph.Defs().NbEdges(); ++anIdx)
  {
    const auto& anEdge = aGraph.Defs().Edge(anIdx);
    EXPECT_TRUE(anEdge.StartVertexDefId.IsValid())
      << "Edge " << anIdx << " has invalid start vertex";
    EXPECT_TRUE(anEdge.EndVertexDefId.IsValid())
      << "Edge " << anIdx << " has invalid end vertex";
    EXPECT_EQ(anEdge.StartVertexDefId.NodeKind, BRepGraph_NodeId::Kind::Vertex);
    EXPECT_EQ(anEdge.EndVertexDefId.NodeKind, BRepGraph_NodeId::Kind::Vertex);
  }
}

TEST(BRepGraphBuildTest, Box_FaceSurfNodeIdsAreValid)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  for (int anIdx = 0; anIdx < aGraph.Defs().NbFaces(); ++anIdx)
  {
    const auto& aFace = aGraph.Defs().Face(anIdx);
    EXPECT_TRUE(aFace.SurfNodeId.IsValid())
      << "Face " << anIdx << " has invalid SurfNodeId";
    EXPECT_EQ(aFace.SurfNodeId.NodeKind, BRepGraph_NodeId::Kind::Surface);
    EXPECT_GE(aFace.SurfNodeId.Index, 0);
    EXPECT_LT(aFace.SurfNodeId.Index, aGraph.Geom().NbSurfaces());
  }
}

TEST(BRepGraphBuildTest, Box_EdgeParamRange_IsNonDegenerate)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  for (int anIdx = 0; anIdx < aGraph.Defs().NbEdges(); ++anIdx)
  {
    const auto& anEdge = aGraph.Defs().Edge(anIdx);
    EXPECT_LT(anEdge.ParamFirst, anEdge.ParamLast)
      << "Edge " << anIdx << " has invalid parameter range ["
      << anEdge.ParamFirst << ", " << anEdge.ParamLast << "]";
  }
}

TEST(BRepGraphBuildTest, AppendShape_OnEmptyGraph_BuildsFaceLevelGraph)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Builder().AppendShape(aBox);

  EXPECT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.Defs().NbSolids(), 0);
  EXPECT_EQ(aGraph.Defs().NbShells(), 0);
  EXPECT_EQ(aGraph.Defs().NbFaces(), 6);
}

TEST(BRepGraphBuildTest, AppendShape_SameFaceTwice_DedupsDefinition)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  const TopoDS_Face aFace = TopoDS::Face(anExp.Current());

  BRepGraph aGraph;
  aGraph.Builder().AppendShape(aFace);
  aGraph.Builder().AppendShape(aFace);

  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_EQ(aGraph.Defs().NbFaces(), 1);
  EXPECT_EQ(aGraph.Usages().NbFaces(), 2);

  const BRepGraph_TopoNode::FaceDef& aFaceDef = aGraph.Defs().Face(0);
  EXPECT_EQ(aFaceDef.Usages.Length(), 2);
}

TEST(BRepGraphBuildTest, AppendShape_AfterBuild_DoesNotCreateNewSolidDefs)
{
  BRepPrimAPI_MakeBox aBox1Maker(10.0, 20.0, 30.0);
  BRepPrimAPI_MakeBox aBox2Maker(15.0, 25.0, 35.0);

  BRepGraph aGraph;
  aGraph.Build(aBox1Maker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const int aNbSolidsBefore = aGraph.Defs().NbSolids();
  const int aNbFacesBefore  = aGraph.Defs().NbFaces();

  aGraph.Builder().AppendShape(aBox2Maker.Shape());

  EXPECT_EQ(aGraph.Defs().NbSolids(), aNbSolidsBefore);
  EXPECT_EQ(aGraph.Defs().NbFaces(), aNbFacesBefore + 6);
}

TEST(BRepGraphBuildTest, AppendShape_AppendedFaceHasNoParentShellUsage)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  const TopoDS_Face aFace = TopoDS::Face(anExp.Current());

  BRepGraph aGraph;
  aGraph.Builder().AppendShape(aFace);

  ASSERT_EQ(aGraph.Usages().NbFaces(), 1);
  const BRepGraph_TopoNode::FaceUsage& aFaceUsage = aGraph.Usages().Face(0);
  EXPECT_FALSE(aFaceUsage.ParentUsage.IsValid());
}
