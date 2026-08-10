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

#include <gtest/gtest.h>

#include <GeomHash_Polygon2DHasher.hxx>
#include <GeomHash_Polygon3DHasher.hxx>
#include <GeomHash_PolygonOnTriHasher.hxx>
#include <GeomHash_TriangulationHasher.hxx>
#include <NCollection_Array1.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_Triangle.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

namespace
{
occ::handle<Poly_Polygon2D> makePolygon2D(const double theDeflection)
{
  occ::handle<Poly_Polygon2D> aPoly = new Poly_Polygon2D(2);
  aPoly->ChangeNodes().SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoly->ChangeNodes().SetValue(2, gp_Pnt2d(1.0, 0.0));
  aPoly->Deflection(theDeflection);
  return aPoly;
}

occ::handle<Poly_Polygon3D> makePolygon3D(const double theDeflection)
{
  occ::handle<Poly_Polygon3D> aPoly = new Poly_Polygon3D(2, false);
  aPoly->ChangeNodes().SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aPoly->ChangeNodes().SetValue(2, gp_Pnt(1.0, 0.0, 0.0));
  aPoly->Deflection(theDeflection);
  return aPoly;
}

occ::handle<Poly_PolygonOnTriangulation> makePolygonOnTriangulation(const double theDeflection)
{
  occ::handle<Poly_PolygonOnTriangulation> aPoly = new Poly_PolygonOnTriangulation(2, false);
  aPoly->SetNode(1, 1);
  aPoly->SetNode(2, 2);
  aPoly->Deflection(theDeflection);
  return aPoly;
}

occ::handle<Poly_Triangulation> makeTriangulation(const double theDeflection)
{
  occ::handle<Poly_Triangulation> aTri = new Poly_Triangulation(3, 1, false);
  aTri->SetNode(1, gp_Pnt(0.0, 0.0, 0.0));
  aTri->SetNode(2, gp_Pnt(1.0, 0.0, 0.0));
  aTri->SetNode(3, gp_Pnt(0.0, 1.0, 0.0));
  aTri->SetTriangle(1, Poly_Triangle(1, 2, 3));
  aTri->Deflection(theDeflection);
  return aTri;
}
} // namespace

TEST(GeomHash_MeshHasherTest, Polygon2D_CloseDeflectionKeepsEqualHash)
{
  const GeomHash_Polygon2DHasher    aHasher(0.1, 0.01);
  const occ::handle<Poly_Polygon2D> aPoly1 = makePolygon2D(0.004);
  const occ::handle<Poly_Polygon2D> aPoly2 = makePolygon2D(0.0044);

  ASSERT_TRUE(aHasher(aPoly1, aPoly2));
  EXPECT_EQ(aHasher(aPoly1), aHasher(aPoly2));
}

TEST(GeomHash_MeshHasherTest, Polygon3D_CloseDeflectionKeepsEqualHash)
{
  const GeomHash_Polygon3DHasher    aHasher(0.1, 0.01);
  const occ::handle<Poly_Polygon3D> aPoly1 = makePolygon3D(0.004);
  const occ::handle<Poly_Polygon3D> aPoly2 = makePolygon3D(0.0044);

  ASSERT_TRUE(aHasher(aPoly1, aPoly2));
  EXPECT_EQ(aHasher(aPoly1), aHasher(aPoly2));
}

TEST(GeomHash_MeshHasherTest, PolygonOnTriangulation_CloseDeflectionKeepsEqualHash)
{
  const GeomHash_PolygonOnTriHasher aHasher(0.1, 0.01);
  const PolygonOnTriHashKey         aKey1{makePolygonOnTriangulation(0.004), 7};
  const PolygonOnTriHashKey         aKey2{makePolygonOnTriangulation(0.0044), 7};

  ASSERT_TRUE(aHasher(aKey1, aKey2));
  EXPECT_EQ(aHasher(aKey1), aHasher(aKey2));
}

TEST(GeomHash_MeshHasherTest, Triangulation_CloseDeflectionKeepsEqualHash)
{
  const GeomHash_TriangulationHasher    aHasher(0.1, 0.01);
  const occ::handle<Poly_Triangulation> aTri1 = makeTriangulation(0.004);
  const occ::handle<Poly_Triangulation> aTri2 = makeTriangulation(0.0044);

  ASSERT_TRUE(aHasher(aTri1, aTri2));
  EXPECT_EQ(aHasher(aTri1), aHasher(aTri2));
}

TEST(GeomHash_MeshHasherTest, Polygon2D_DifferentSameCountGeometryChangesHash)
{
  const GeomHash_Polygon2DHasher    aHasher(0.1, 0.01);
  const occ::handle<Poly_Polygon2D> aPoly1 = makePolygon2D(0.004);
  const occ::handle<Poly_Polygon2D> aPoly2 = makePolygon2D(0.004);
  aPoly2->ChangeNodes().SetValue(2, gp_Pnt2d(2.0, 0.0));

  EXPECT_FALSE(aHasher(aPoly1, aPoly2));
  EXPECT_NE(aHasher(aPoly1), aHasher(aPoly2));
}

TEST(GeomHash_MeshHasherTest, Polygon3D_DifferentSameCountGeometryChangesHash)
{
  const GeomHash_Polygon3DHasher    aHasher(0.1, 0.01);
  const occ::handle<Poly_Polygon3D> aPoly1 = makePolygon3D(0.004);
  const occ::handle<Poly_Polygon3D> aPoly2 = makePolygon3D(0.004);
  aPoly2->ChangeNodes().SetValue(2, gp_Pnt(2.0, 0.0, 0.0));

  EXPECT_FALSE(aHasher(aPoly1, aPoly2));
  EXPECT_NE(aHasher(aPoly1), aHasher(aPoly2));
}

TEST(GeomHash_MeshHasherTest, PolygonOnTriangulation_DifferentSameCountIndicesChangesHash)
{
  const GeomHash_PolygonOnTriHasher aHasher(0.1, 0.01);
  PolygonOnTriHashKey               aKey1{makePolygonOnTriangulation(0.004), 7};
  PolygonOnTriHashKey               aKey2{makePolygonOnTriangulation(0.004), 7};
  aKey2.Poly->SetNode(2, 3);

  EXPECT_FALSE(aHasher(aKey1, aKey2));
  EXPECT_NE(aHasher(aKey1), aHasher(aKey2));
}

TEST(GeomHash_MeshHasherTest, Triangulation_DifferentSameCountGeometryChangesHash)
{
  const GeomHash_TriangulationHasher    aHasher(0.1, 0.01);
  const occ::handle<Poly_Triangulation> aTri1 = makeTriangulation(0.004);
  const occ::handle<Poly_Triangulation> aTri2 = makeTriangulation(0.004);
  aTri2->SetNode(3, gp_Pnt(0.0, 2.0, 0.0));

  EXPECT_FALSE(aHasher(aTri1, aTri2));
  EXPECT_NE(aHasher(aTri1), aHasher(aTri2));
}

TEST(GeomHash_MeshHasherTest, NullHandlesDoNotCrash)
{
  const GeomHash_Polygon2DHasher           aPoly2dHasher;
  const GeomHash_Polygon3DHasher           aPoly3dHasher;
  const GeomHash_PolygonOnTriHasher        aPolyOnTriHasher;
  const GeomHash_TriangulationHasher       aTriHasher;
  occ::handle<Poly_Polygon2D>              aNullPoly2d;
  occ::handle<Poly_Polygon3D>              aNullPoly3d;
  occ::handle<Poly_PolygonOnTriangulation> aNullPolyOnTri;
  occ::handle<Poly_Triangulation>          aNullTri;

  EXPECT_EQ(0u, aPoly2dHasher(aNullPoly2d));
  EXPECT_TRUE(aPoly2dHasher(aNullPoly2d, aNullPoly2d));
  EXPECT_FALSE(aPoly2dHasher(aNullPoly2d, makePolygon2D(0.004)));

  EXPECT_EQ(0u, aPoly3dHasher(aNullPoly3d));
  EXPECT_TRUE(aPoly3dHasher(aNullPoly3d, aNullPoly3d));
  EXPECT_FALSE(aPoly3dHasher(aNullPoly3d, makePolygon3D(0.004)));

  EXPECT_EQ(0u, aTriHasher(aNullTri));
  EXPECT_TRUE(aTriHasher(aNullTri, aNullTri));
  EXPECT_FALSE(aTriHasher(aNullTri, makeTriangulation(0.004)));

  const PolygonOnTriHashKey aNullKey1{aNullPolyOnTri, 7};
  const PolygonOnTriHashKey aNullKey2{aNullPolyOnTri, 7};
  const PolygonOnTriHashKey aNullKey3{aNullPolyOnTri, 8};
  EXPECT_TRUE(aPolyOnTriHasher(aNullKey1, aNullKey2));
  EXPECT_FALSE(aPolyOnTriHasher(aNullKey1, aNullKey3));
  EXPECT_FALSE(
    aPolyOnTriHasher(aNullKey1, PolygonOnTriHashKey{makePolygonOnTriangulation(0.004), 7}));
}

TEST(GeomHash_MeshHasherTest, HashToleranceAffectsNumericFields)
{
  const occ::handle<Poly_Polygon2D> aPoly = makePolygon2D(0.06);

  const GeomHash_Polygon2DHasher aFineHasher(0.1, 0.01);
  const GeomHash_Polygon2DHasher aCoarseHasher(0.1, 0.1);

  EXPECT_NE(aFineHasher(aPoly), aCoarseHasher(aPoly));
}
