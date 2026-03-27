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
#include <BRepGraphInc_Entity.hxx>
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_UIDsView.hxx>
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
// Sphere tests
// =============================================================================

TEST(BRepGraph_BuildTest, Sphere_IsDone)
{
  BRepPrimAPI_MakeSphere aMaker(10.0);
  const TopoDS_Shape     aShape = aMaker.Shape();
  ASSERT_TRUE(aMaker.IsDone());

  BRepGraph aGraph;
  aGraph.Build(aShape);
  EXPECT_TRUE(aGraph.IsDone());
}

TEST(BRepGraph_BuildTest, Sphere_DefCounts_MatchTopExp)
{
  BRepPrimAPI_MakeSphere aMaker(10.0);
  const TopoDS_Shape     aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Topo().NbSolids(), countUnique(aShape, TopAbs_SOLID));
  EXPECT_EQ(aGraph.Topo().NbShells(), countUnique(aShape, TopAbs_SHELL));
  EXPECT_EQ(aGraph.Topo().NbFaces(), countUnique(aShape, TopAbs_FACE));
  EXPECT_EQ(aGraph.Topo().NbWires(), countUnique(aShape, TopAbs_WIRE));
  EXPECT_EQ(aGraph.Topo().NbEdges(), countUnique(aShape, TopAbs_EDGE));
  EXPECT_EQ(aGraph.Topo().NbVertices(), countUnique(aShape, TopAbs_VERTEX));
}

TEST(BRepGraph_BuildTest, Sphere_SurfaceType)
{
  BRepPrimAPI_MakeSphere aMaker(5.0);
  const TopoDS_Shape     aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GE(aGraph.Topo().NbFaces(), 1);

  bool aHasSpherical = false;
  for (int anIdx = 0; anIdx < aGraph.Topo().NbFaces(); ++anIdx)
  {
    const occ::handle<Geom_Surface>& aSurf =
      BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(anIdx));
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
  aGraph.Build(aShape);
  ASSERT_TRUE(aGraph.IsDone());

  // A sphere has degenerate edges at poles.
  int aDegCount = 0;
  for (int anIdx = 0; anIdx < aGraph.Topo().NbEdges(); ++anIdx)
  {
    if (BRepGraph_Tool::Edge::Degenerated(aGraph, BRepGraph_EdgeId(anIdx)))
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
  aGraph.Build(aShape);
  EXPECT_TRUE(aGraph.IsDone());
}

TEST(BRepGraph_BuildTest, Cylinder_DefCounts_MatchTopExp)
{
  BRepPrimAPI_MakeCylinder aMaker(5.0, 20.0);
  const TopoDS_Shape       aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Topo().NbFaces(), countUnique(aShape, TopAbs_FACE));
  EXPECT_EQ(aGraph.Topo().NbEdges(), countUnique(aShape, TopAbs_EDGE));
  EXPECT_EQ(aGraph.Topo().NbVertices(), countUnique(aShape, TopAbs_VERTEX));
}

TEST(BRepGraph_BuildTest, Cylinder_SurfaceType)
{
  BRepPrimAPI_MakeCylinder aMaker(5.0, 20.0);
  const TopoDS_Shape       aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);
  ASSERT_TRUE(aGraph.IsDone());

  bool aHasCylindrical = false;
  for (int anIdx = 0; anIdx < aGraph.Topo().NbFaces(); ++anIdx)
  {
    const occ::handle<Geom_Surface>& aSurf =
      BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(anIdx));
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
  aGraph.Build(aShape);
  EXPECT_TRUE(aGraph.IsDone());
}

TEST(BRepGraph_BuildTest, Cone_DefCounts_MatchTopExp)
{
  BRepPrimAPI_MakeCone aMaker(10.0, 0.0, 15.0);
  const TopoDS_Shape   aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Topo().NbFaces(), countUnique(aShape, TopAbs_FACE));
  EXPECT_EQ(aGraph.Topo().NbEdges(), countUnique(aShape, TopAbs_EDGE));
  EXPECT_EQ(aGraph.Topo().NbVertices(), countUnique(aShape, TopAbs_VERTEX));
}

TEST(BRepGraph_BuildTest, Cone_SurfaceType)
{
  BRepPrimAPI_MakeCone aMaker(10.0, 5.0, 15.0);
  const TopoDS_Shape   aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);
  ASSERT_TRUE(aGraph.IsDone());

  bool aHasConical = false;
  for (int anIdx = 0; anIdx < aGraph.Topo().NbFaces(); ++anIdx)
  {
    const occ::handle<Geom_Surface>& aSurf =
      BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(anIdx));
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
  aGraph.Build(aShape);
  ASSERT_TRUE(aGraph.IsDone());

  int aDegCount = 0;
  for (int anIdx = 0; anIdx < aGraph.Topo().NbEdges(); ++anIdx)
  {
    if (BRepGraph_Tool::Edge::Degenerated(aGraph, BRepGraph_EdgeId(anIdx)))
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
  aGraph.Build(aShape);
  EXPECT_TRUE(aGraph.IsDone());
}

TEST(BRepGraph_BuildTest, Torus_DefCounts_MatchTopExp)
{
  BRepPrimAPI_MakeTorus aMaker(20.0, 5.0);
  const TopoDS_Shape    aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Topo().NbFaces(), countUnique(aShape, TopAbs_FACE));
  EXPECT_EQ(aGraph.Topo().NbEdges(), countUnique(aShape, TopAbs_EDGE));
  EXPECT_EQ(aGraph.Topo().NbVertices(), countUnique(aShape, TopAbs_VERTEX));
}

TEST(BRepGraph_BuildTest, Torus_SurfaceType)
{
  BRepPrimAPI_MakeTorus aMaker(20.0, 5.0);
  const TopoDS_Shape    aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);
  ASSERT_TRUE(aGraph.IsDone());

  bool aHasToroidal = false;
  for (int anIdx = 0; anIdx < aGraph.Topo().NbFaces(); ++anIdx)
  {
    const occ::handle<Geom_Surface>& aSurf =
      BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(anIdx));
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
  aGraph.Build(aShape);
  EXPECT_TRUE(aGraph.IsDone());
}

TEST(BRepGraph_BuildTest, Wedge_DefCounts_MatchTopExp)
{
  BRepPrimAPI_MakeWedge aMaker(10.0, 10.0, 10.0, 5.0);
  const TopoDS_Shape    aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Topo().NbSolids(), countUnique(aShape, TopAbs_SOLID));
  EXPECT_EQ(aGraph.Topo().NbShells(), countUnique(aShape, TopAbs_SHELL));
  EXPECT_EQ(aGraph.Topo().NbFaces(), countUnique(aShape, TopAbs_FACE));
  EXPECT_EQ(aGraph.Topo().NbWires(), countUnique(aShape, TopAbs_WIRE));
  EXPECT_EQ(aGraph.Topo().NbEdges(), countUnique(aShape, TopAbs_EDGE));
  EXPECT_EQ(aGraph.Topo().NbVertices(), countUnique(aShape, TopAbs_VERTEX));
}

TEST(BRepGraph_BuildTest, Wedge_AllPlanarSurfaces)
{
  BRepPrimAPI_MakeWedge aMaker(10.0, 10.0, 10.0, 5.0);
  const TopoDS_Shape    aShape = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);
  ASSERT_TRUE(aGraph.IsDone());

  for (int anIdx = 0; anIdx < aGraph.Topo().NbFaces(); ++anIdx)
  {
    const occ::handle<Geom_Surface>& aSurf =
      BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(anIdx));
    ASSERT_FALSE(aSurf.IsNull());
    EXPECT_TRUE(aSurf->DynamicType() == STANDARD_TYPE(Geom_Plane))
      << "Face " << anIdx << " surface is not a Geom_Plane";
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
  aGraph.Build(aCompound);
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
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Topo().NbFaces(), countUnique(aCompound, TopAbs_FACE));
  EXPECT_EQ(aGraph.Topo().NbEdges(), countUnique(aCompound, TopAbs_EDGE));
  EXPECT_EQ(aGraph.Topo().NbVertices(), countUnique(aCompound, TopAbs_VERTEX));
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
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Topo().NbSolids(), countUnique(aCompound, TopAbs_SOLID));
  EXPECT_EQ(aGraph.Topo().NbFaces(), countUnique(aCompound, TopAbs_FACE));
  EXPECT_EQ(aGraph.Topo().NbEdges(), countUnique(aCompound, TopAbs_EDGE));
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
  aGraph.Build(anOuter);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Topo().NbFaces(), countUnique(anOuter, TopAbs_FACE));
  EXPECT_EQ(aGraph.Topo().NbEdges(), countUnique(anOuter, TopAbs_EDGE));
  EXPECT_EQ(aGraph.Topo().NbVertices(), countUnique(anOuter, TopAbs_VERTEX));
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
  aGraph.Build(aShape);
  EXPECT_TRUE(aGraph.IsDone());
}

TEST(BRepGraph_BuildTest, SinglePlanarFace_Counts)
{
  gp_Pln                  aPln;
  BRepBuilderAPI_MakeFace aFaceMaker(aPln);
  const TopoDS_Shape      aShape = aFaceMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Topo().NbFaces(), 1);
  EXPECT_EQ(aGraph.Topo().NbSolids(), 0);
  EXPECT_EQ(aGraph.Topo().NbShells(), 0);
  EXPECT_GE(aGraph.Topo().NbFaces(), 1);

  // Verify surface is a plane.
  ASSERT_TRUE(BRepGraph_Tool::Face::HasSurface(aGraph, BRepGraph_FaceId(0)));
  const occ::handle<Geom_Surface>& aSurf =
    BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(0));
  ASSERT_FALSE(aSurf.IsNull());
  EXPECT_TRUE(aSurf->DynamicType() == STANDARD_TYPE(Geom_Plane));
}

TEST(BRepGraph_BuildTest, SingleEdge_HandlesGracefully)
{
  BRepBuilderAPI_MakeEdge anEdgeMaker(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0));
  ASSERT_TRUE(anEdgeMaker.IsDone());
  const TopoDS_Shape aShape = anEdgeMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);

  // BRepGraph is face-level; standalone edges may produce zero counts.
  // Verify it does not crash and returns consistent state.
  EXPECT_EQ(aGraph.Topo().NbFaces(), 0);
  EXPECT_EQ(aGraph.Topo().NbSolids(), 0);
}

TEST(BRepGraph_BuildTest, SingleVertex_HandlesGracefully)
{
  BRepBuilderAPI_MakeVertex aVertexMaker(gp_Pnt(1.0, 2.0, 3.0));
  const TopoDS_Shape        aShape = aVertexMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aShape);

  // BRepGraph is face-level; standalone vertices may produce zero counts.
  EXPECT_EQ(aGraph.Topo().NbFaces(), 0);
  EXPECT_EQ(aGraph.Topo().NbEdges(), 0);
}

// =============================================================================
// Box cross-validation with TopExp
// =============================================================================

TEST(BRepGraph_BuildTest, Box_FaceDefCount_MatchesTopExp)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape  aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Topo().NbFaces(), countUnique(aBox, TopAbs_FACE));
  EXPECT_EQ(aGraph.Topo().NbFaces(), 6);
}

TEST(BRepGraph_BuildTest, Box_EdgeDefCount_MatchesTopExp)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape  aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Topo().NbEdges(), countUnique(aBox, TopAbs_EDGE));
  EXPECT_EQ(aGraph.Topo().NbEdges(), 12);
}

TEST(BRepGraph_BuildTest, Box_VertexDefCount_MatchesTopExp)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape  aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Topo().NbVertices(), countUnique(aBox, TopAbs_VERTEX));
  EXPECT_EQ(aGraph.Topo().NbVertices(), 8);
}

TEST(BRepGraph_BuildTest, Box_VertexPoints_MatchBRepTool)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape  aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // Collect all vertex points from TopExp.
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aVertexMap;
  TopExp::MapShapes(aBox, TopAbs_VERTEX, aVertexMap);

  ASSERT_EQ(aGraph.Topo().NbVertices(), aVertexMap.Extent());

  // For each graph vertex, verify that a matching TopExp vertex exists.
  for (int anIdx = 0; anIdx < aGraph.Topo().NbVertices(); ++anIdx)
  {
    const gp_Pnt aGraphPnt = BRepGraph_Tool::Vertex::Pnt(aGraph, BRepGraph_VertexId(anIdx));
    bool         aFound    = false;
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
    EXPECT_TRUE(aFound) << "Graph vertex " << anIdx << " at (" << aGraphPnt.X() << ", "
                        << aGraphPnt.Y() << ", " << aGraphPnt.Z()
                        << ") has no matching TopExp vertex";
  }
}

TEST(BRepGraph_BuildTest, Box_FaceTolerances_MatchBRepTool)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape  aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aFaceMap;
  TopExp::MapShapes(aBox, TopAbs_FACE, aFaceMap);

  // Verify that each graph face tolerance appears in the TopExp set.
  for (int anIdx = 0; anIdx < aGraph.Topo().NbFaces(); ++anIdx)
  {
    const double aGraphTol = BRepGraph_Tool::Face::Tolerance(aGraph, BRepGraph_FaceId(anIdx));
    bool         aFound    = false;
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
    EXPECT_TRUE(aFound) << "Graph face " << anIdx << " tolerance " << aGraphTol
                        << " has no matching TopExp face tolerance";
  }
}

TEST(BRepGraph_BuildTest, Box_EdgeTolerances_MatchBRepTool)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape  aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> anEdgeMap;
  TopExp::MapShapes(aBox, TopAbs_EDGE, anEdgeMap);

  for (int anIdx = 0; anIdx < aGraph.Topo().NbEdges(); ++anIdx)
  {
    const double aGraphTol = BRepGraph_Tool::Edge::Tolerance(aGraph, BRepGraph_EdgeId(anIdx));
    bool         aFound    = false;
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
    EXPECT_TRUE(aFound) << "Graph edge " << anIdx << " tolerance " << aGraphTol
                        << " has no matching TopExp edge tolerance";
  }
}

TEST(BRepGraph_BuildTest, Box_AllSurfacesArePlanes)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape  aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  ASSERT_EQ(aGraph.Topo().NbFaces(), 6);
  for (int anIdx = 0; anIdx < aGraph.Topo().NbFaces(); ++anIdx)
  {
    const occ::handle<Geom_Surface>& aSurf =
      BRepGraph_Tool::Face::Surface(aGraph, BRepGraph_FaceId(anIdx));
    ASSERT_FALSE(aSurf.IsNull());
    EXPECT_TRUE(aSurf->DynamicType() == STANDARD_TYPE(Geom_Plane))
      << "Face " << anIdx << " surface is not Geom_Plane";
  }
}

TEST(BRepGraph_BuildTest, Box_NoDegenerateEdges)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape  aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  for (int anIdx = 0; anIdx < aGraph.Topo().NbEdges(); ++anIdx)
  {
    EXPECT_FALSE(BRepGraph_Tool::Edge::Degenerated(aGraph, BRepGraph_EdgeId(anIdx)))
      << "Box edge " << anIdx << " is degenerate unexpectedly";
  }
}

TEST(BRepGraph_BuildTest, Box_EdgeVertexDefsAreValid)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape  aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  for (int anIdx = 0; anIdx < aGraph.Topo().NbEdges(); ++anIdx)
  {
    const BRepGraph_EdgeId anEdgeId(anIdx);
    const BRepGraphInc::VertexRefEntry& aStartRef =
      BRepGraph_Tool::Edge::StartVertex(aGraph, anEdgeId);
    const BRepGraphInc::VertexRefEntry& anEndRef =
      BRepGraph_Tool::Edge::EndVertex(aGraph, anEdgeId);
    EXPECT_TRUE(aStartRef.VertexEntityId.IsValid())
      << "Edge " << anIdx << " has invalid start vertex";
    EXPECT_TRUE(anEndRef.VertexEntityId.IsValid()) << "Edge " << anIdx << " has invalid end vertex";
    EXPECT_EQ(BRepGraph_NodeId(aStartRef.VertexEntityId).NodeKind, BRepGraph_NodeId::Kind::Vertex);
    EXPECT_EQ(BRepGraph_NodeId(anEndRef.VertexEntityId).NodeKind, BRepGraph_NodeId::Kind::Vertex);
  }
}

TEST(BRepGraph_BuildTest, Box_FaceSurfacesAreValid)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape  aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  for (int anIdx = 0; anIdx < aGraph.Topo().NbFaces(); ++anIdx)
  {
    EXPECT_TRUE(BRepGraph_Tool::Face::HasSurface(aGraph, BRepGraph_FaceId(anIdx)))
      << "Face " << anIdx << " has no surface rep";
  }
}

TEST(BRepGraph_BuildTest, Box_EdgeParamRange_IsNonDegenerate)
{
  BRepPrimAPI_MakeBox aMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape  aBox = aMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  for (int anIdx = 0; anIdx < aGraph.Topo().NbEdges(); ++anIdx)
  {
    const BRepGraphInc::EdgeEntity& anEdge = aGraph.Topo().Edge(BRepGraph_EdgeId(anIdx));
    EXPECT_LT(anEdge.ParamFirst, anEdge.ParamLast)
      << "Edge " << anIdx << " has invalid parameter range [" << anEdge.ParamFirst << ", "
      << anEdge.ParamLast << "]";
  }
}

TEST(BRepGraph_BuildTest, AppendShape_OnEmptyGraph_BuildsFaceLevelGraph)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Builder().AppendShape(aBox);

  EXPECT_TRUE(aGraph.IsDone());
  EXPECT_EQ(aGraph.Topo().NbSolids(), 0);
  EXPECT_EQ(aGraph.Topo().NbShells(), 0);
  EXPECT_EQ(aGraph.Topo().NbFaces(), 6);
}

TEST(BRepGraph_BuildTest, AppendShape_SameFaceTwice_DedupsDefinition)
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
  // Same TShape appended twice: definition is deduplicated.
  ASSERT_EQ(aGraph.Topo().NbFaces(), 1);
}

TEST(BRepGraph_BuildTest, AppendShape_AfterBuild_DoesNotCreateNewSolidDefs)
{
  BRepPrimAPI_MakeBox aBox1Maker(10.0, 20.0, 30.0);
  BRepPrimAPI_MakeBox aBox2Maker(15.0, 25.0, 35.0);

  BRepGraph aGraph;
  aGraph.Build(aBox1Maker.Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const int aNbSolidsBefore = aGraph.Topo().NbSolids();
  const int aNbFacesBefore  = aGraph.Topo().NbFaces();

  aGraph.Builder().AppendShape(aBox2Maker.Shape());

  EXPECT_EQ(aGraph.Topo().NbSolids(), aNbSolidsBefore);
  EXPECT_EQ(aGraph.Topo().NbFaces(), aNbFacesBefore + 6);
}

TEST(BRepGraph_BuildTest, AppendShape_AppendedFaceHasNoParentShell)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  const TopoDS_Face aFace = TopoDS::Face(anExp.Current());

  BRepGraph aGraph;
  aGraph.Builder().AppendShape(aFace);

  ASSERT_EQ(aGraph.Topo().NbFaces(), 1);
  // Appended face should not be part of any shell.
  EXPECT_EQ(aGraph.Topo().NbShells(), 0);
}

TEST(BRepGraph_BuildTest, AppendShape_PreservesExistingUIDs)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().NbEdges(), 0);

  // Record UID of the first edge before append.
  const BRepGraph_UID anOrigUID = aGraph.UIDs().Of(BRepGraph_NodeId::Edge(0));
  ASSERT_TRUE(anOrigUID.IsValid());

  // Append a sphere.
  BRepPrimAPI_MakeSphere aSphereMaker(5.0);
  const TopoDS_Shape&    aSphere = aSphereMaker.Shape();
  aGraph.Builder().AppendShape(aSphere);
  ASSERT_TRUE(aGraph.IsDone());

  // Verify original edge UID is unchanged.
  const BRepGraph_UID aPostUID = aGraph.UIDs().Of(BRepGraph_NodeId::Edge(0));
  EXPECT_EQ(anOrigUID, aPostUID);
}

TEST(BRepGraph_BuildTest, Build_WithoutPostPasses_BasicQueriesWork)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRepGraphInc_Populate::Options anOpts;
  anOpts.ExtractRegularities    = false;
  anOpts.ExtractVertexPointReps = false;

  BRepGraph aGraph;
  aGraph.Build(aBox, false, anOpts);
  ASSERT_TRUE(aGraph.IsDone());

  EXPECT_EQ(aGraph.Topo().NbFaces(), 6);
  EXPECT_EQ(aGraph.Topo().NbEdges(), 12);
  EXPECT_EQ(aGraph.Topo().NbVertices(), 8);

  // Regularities should be empty.
  for (int i = 0; i < aGraph.Topo().NbEdges(); ++i)
  {
    EXPECT_EQ(aGraph.Topo().Edge(BRepGraph_EdgeId(i)).Regularities.Length(), 0);
  }

  // Vertex point reps should be empty.
  for (int i = 0; i < aGraph.Topo().NbVertices(); ++i)
  {
    EXPECT_EQ(aGraph.Topo().Vertex(BRepGraph_VertexId(i)).PointsOnCurve.Length(), 0);
    EXPECT_EQ(aGraph.Topo().Vertex(BRepGraph_VertexId(i)).PointsOnSurface.Length(), 0);
    EXPECT_EQ(aGraph.Topo().Vertex(BRepGraph_VertexId(i)).PointsOnPCurve.Length(), 0);
  }
}
