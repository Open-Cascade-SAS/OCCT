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
#include <BRepTools_TrsfModification.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_Plane.hxx>
#include <NCollection_Array1.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangle.hxx>
#include <Poly_Triangulation.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <gtest/gtest.h>

namespace
{
occ::handle<Poly_Triangulation> makeTriangulation(const bool theHasNormals = false)
{
  occ::handle<Poly_Triangulation> aTriangulation =
    new Poly_Triangulation(3, 1, false, theHasNormals);
  aTriangulation->SetNode(1, gp_Pnt(0.0, 0.0, 0.0));
  aTriangulation->SetNode(2, gp_Pnt(1.0, 0.0, 0.0));
  aTriangulation->SetNode(3, gp_Pnt(0.0, 1.0, 1.0));
  aTriangulation->SetTriangle(1, Poly_Triangle(1, 2, 3));
  aTriangulation->Deflection(0.25);
  if (theHasNormals)
  {
    const gp_Dir aNormal(0.0, -1.0, 1.0);
    for (int anIndex = 1; anIndex <= 3; ++anIndex)
    {
      aTriangulation->SetNormal(anIndex, aNormal);
    }
  }
  return aTriangulation;
}

gp_Trsf makeScale(const double theScale)
{
  gp_Trsf aTrsf;
  aTrsf.SetScale(gp_Pnt(0.0, 0.0, 0.0), theScale);
  return aTrsf;
}

void expectPointNear(const gp_Pnt& theActual, const gp_Pnt& theExpected)
{
  EXPECT_LT(theActual.Distance(theExpected), 1.0e-12);
}
} // namespace

TEST(BRepTools_TrsfModificationTest, CopyMeshFlag_SkipsCachedMeshOnGeometricFace)
{
  BRep_Builder                          aBuilder;
  TopoDS_Face                           aFace;
  const occ::handle<Geom_Plane>         aPlane         = new Geom_Plane(gp_Pln());
  const occ::handle<Poly_Triangulation> aTriangulation = makeTriangulation();
  aBuilder.MakeFace(aFace, aPlane, 0.1);
  aBuilder.UpdateFace(aFace, aTriangulation);

  BRepTools_TrsfModification            aModification(makeScale(2.0));
  occ::handle<Poly_Triangulation>       aResult   = makeTriangulation();
  const occ::handle<Poly_Triangulation> aSentinel = aResult;
  EXPECT_FALSE(aModification.NewTriangulation(aFace, aResult));
  EXPECT_EQ(aResult, aSentinel);

  aModification.IsCopyMesh() = true;
  ASSERT_TRUE(aModification.NewTriangulation(aFace, aResult));
  expectPointNear(aResult->Node(2), gp_Pnt(2.0, 0.0, 0.0));
  EXPECT_DOUBLE_EQ(aResult->Deflection(), 0.5);
  expectPointNear(aTriangulation->Node(2), gp_Pnt(1.0, 0.0, 0.0));
}

TEST(BRepTools_TrsfModificationTest, Polygon3D_DistinctRepresentationLocation)
{
  NCollection_Array1<gp_Pnt> aNodes(1, 2);
  aNodes(1)                                  = gp_Pnt(1.0, 2.0, 3.0);
  aNodes(2)                                  = gp_Pnt(4.0, 5.0, 6.0);
  const occ::handle<Poly_Polygon3D> aPolygon = new Poly_Polygon3D(aNodes);

  gp_Trsf aRepresentationTrsf;
  aRepresentationTrsf.SetTranslation(gp_Vec(10.0, 0.0, 0.0));
  gp_Trsf anEdgeTrsf;
  anEdgeTrsf.SetTranslation(gp_Vec(0.0, 20.0, 0.0));
  BRep_Builder aBuilder;
  TopoDS_Edge  anEdge;
  aBuilder.MakeEdge(anEdge);
  aBuilder.UpdateEdge(anEdge, aPolygon, TopLoc_Location(aRepresentationTrsf));
  anEdge.Location(TopLoc_Location(anEdgeTrsf));

  BRepTools_TrsfModification aModification(makeScale(2.0));
  aModification.IsCopyMesh() = true;
  occ::handle<Poly_Polygon3D> aResult;
  ASSERT_TRUE(aModification.NewPolygon(anEdge, aResult));

  TopLoc_Location aSourceLocation;
  ASSERT_FALSE(BRep_Tool::Polygon3D(anEdge, aSourceLocation).IsNull());
  for (int anIndex = 1; anIndex <= aResult->NbNodes(); ++anIndex)
  {
    gp_Pnt anExpected = aPolygon->Nodes()(anIndex).Transformed(aSourceLocation.Transformation());
    anExpected.Transform(makeScale(2.0));
    const gp_Pnt anActual =
      aResult->Nodes()(anIndex).Transformed(anEdge.Location().Transformation());
    expectPointNear(anActual, anExpected);
  }
}

TEST(BRepTools_TrsfModificationTest, ParameterlessPolygonOnTriangulationDoesNotThrow)
{
  BRep_Builder                          aBuilder;
  TopoDS_Face                           aFace;
  TopoDS_Edge                           anEdge;
  const occ::handle<Geom_Plane>         aPlane         = new Geom_Plane(gp_Pln());
  const occ::handle<Poly_Triangulation> aTriangulation = makeTriangulation();
  aBuilder.MakeFace(aFace, aPlane, 0.1);
  aBuilder.UpdateFace(aFace, aTriangulation);

  NCollection_Array1<int> aNodes(1, 2);
  aNodes(1)                                               = 1;
  aNodes(2)                                               = 2;
  const occ::handle<Poly_PolygonOnTriangulation> aPolygon = new Poly_PolygonOnTriangulation(aNodes);
  aPolygon->Deflection(0.25);
  aBuilder.MakeEdge(anEdge, aPolygon, aTriangulation);
  const occ::handle<Geom2d_Line> aPcurve =
    new Geom2d_Line(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)));
  aBuilder.UpdateEdge(anEdge, aPcurve, aFace, 0.1);
  aBuilder.Range(anEdge, aFace, 0.0, 1.0);

  BRepTools_TrsfModification aModification(makeScale(2.0));
  aModification.IsCopyMesh() = true;
  occ::handle<Poly_PolygonOnTriangulation> aResult;
  EXPECT_NO_THROW(EXPECT_TRUE(aModification.NewPolygonOnTriangulation(anEdge, aFace, aResult)));
  ASSERT_FALSE(aResult.IsNull());
  EXPECT_FALSE(aResult->HasParameters());
  EXPECT_DOUBLE_EQ(aResult->Deflection(), 0.5);
}

TEST(BRepTools_TrsfModificationTest, NegativeScaleUpdatesWindingNormalsAndDeflection)
{
  BRep_Builder                          aBuilder;
  TopoDS_Face                           aFace;
  const occ::handle<Poly_Triangulation> aTriangulation = makeTriangulation(true);
  aBuilder.MakeFace(aFace, aTriangulation);

  BRepTools_TrsfModification aModification(makeScale(-2.0));
  aModification.IsCopyMesh() = true;
  occ::handle<Poly_Triangulation> aResult;
  ASSERT_TRUE(aModification.NewTriangulation(aFace, aResult));
  int aNode1 = 0, aNode2 = 0, aNode3 = 0;
  aResult->Triangle(1).Get(aNode1, aNode2, aNode3);
  EXPECT_EQ(aNode1, 1);
  EXPECT_EQ(aNode2, 3);
  EXPECT_EQ(aNode3, 2);
  EXPECT_LT(aResult->Normal(1).Angle(gp_Dir(0.0, 1.0, -1.0)), 1.0e-6);
  EXPECT_DOUBLE_EQ(aResult->Deflection(), 0.5);
}
