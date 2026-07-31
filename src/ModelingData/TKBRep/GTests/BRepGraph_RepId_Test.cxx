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

#include <BRepGraphInc_RepId.hxx>

TEST(BRepGraph_RepIdTest, DefaultRepId_IsInvalid)
{
  const BRepGraph_RepId anId;
  EXPECT_FALSE(anId.IsValid());
}

TEST(BRepGraph_RepIdTest, TypedDefaultRepId_IsInvalid)
{
  const BRepGraph_EdgeCurve3DRepId       aCurve;
  const BRepGraph_FaceSurfaceRepId       aSurface;
  const BRepGraph_CoEdgeCurve2DRepId     aPCurve;
  const BRepGraph_FaceTriangulationRepId aTri;
  EXPECT_FALSE(aCurve.IsValid());
  EXPECT_FALSE(aSurface.IsValid());
  EXPECT_FALSE(aPCurve.IsValid());
  EXPECT_FALSE(aTri.IsValid());
}

TEST(BRepGraph_RepIdTest, TypedRepId_ConvertsToUntyped)
{
  const BRepGraph_EdgeCurve3DRepId aTyped(42);
  const BRepGraph_RepId            anUntyped = aTyped;
  EXPECT_EQ(anUntyped.RepKind, BRepGraph_RepId::Kind::EdgeCurve3D);
  EXPECT_EQ(anUntyped.Index, 42u);
}

TEST(BRepGraph_RepIdTest, KindClassification)
{
  EXPECT_TRUE(BRepGraph_RepId::IsValidKind(BRepGraph_RepId::Kind::EdgeCurve3D));
  EXPECT_TRUE(BRepGraph_RepId::IsValidKind(BRepGraph_RepId::Kind::FaceSurface));
  EXPECT_TRUE(BRepGraph_RepId::IsValidKind(BRepGraph_RepId::Kind::CoEdgeCurve2D));
  EXPECT_TRUE(BRepGraph_RepId::IsValidKind(BRepGraph_RepId::Kind::FaceTriangulation));
}

TEST(BRepGraph_RepIdTest, ValidRepId_PassesBoundsCheck)
{
  const BRepGraph_FaceSurfaceRepId anId(5);
  EXPECT_TRUE(anId.IsValid());
  EXPECT_TRUE(anId.IsValid(10));
  EXPECT_FALSE(anId.IsValid(3));
}

TEST(BRepGraph_RepIdTest, StartId_IsZero)
{
  const BRepGraph_EdgeCurve3DRepId aStart = BRepGraph_EdgeCurve3DRepId::Start();
  EXPECT_TRUE(aStart.IsValid());
  EXPECT_EQ(aStart.Index, 0u);
}

TEST(BRepGraph_RepIdTest, EqualityAndComparison)
{
  const BRepGraph_EdgeCurve3DRepId a1(1);
  const BRepGraph_EdgeCurve3DRepId a2(2);
  const BRepGraph_EdgeCurve3DRepId a1Copy(1);
  EXPECT_EQ(a1, a1Copy);
  EXPECT_NE(a1, a2);
  EXPECT_LT(a1, a2);
  EXPECT_LE(a1, a1Copy);
  EXPECT_GT(a2, a1);
  EXPECT_GE(a1Copy, a1);
}

TEST(BRepGraph_RepIdTest, DifferentKinds_AreNotEqual)
{
  const BRepGraph_RepId aCurve(BRepGraph_RepId::Kind::EdgeCurve3D, 0);
  const BRepGraph_RepId aSurface(BRepGraph_RepId::Kind::FaceSurface, 0);
  EXPECT_NE(aCurve, aSurface);
}

TEST(BRepGraph_RepIdTest, Increment)
{
  BRepGraph_EdgeCurve3DRepId anId = BRepGraph_EdgeCurve3DRepId::Start();
  EXPECT_EQ(anId.Index, 0u);
  ++anId;
  EXPECT_EQ(anId.Index, 1u);
  anId++;
  EXPECT_EQ(anId.Index, 2u);
}

TEST(BRepGraph_RepIdTest, AllTypedAliases_DefaultInvalid)
{
  EXPECT_FALSE(BRepGraph_EdgeCurve3DRepId().IsValid());
  EXPECT_FALSE(BRepGraph_EdgePolygon3DRepId().IsValid());
  EXPECT_FALSE(BRepGraph_CoEdgeCurve2DRepId().IsValid());
  EXPECT_FALSE(BRepGraph_CoEdgePolygon2DRepId().IsValid());
  EXPECT_FALSE(BRepGraph_CoEdgePolygonOnTriRepId().IsValid());
  EXPECT_FALSE(BRepGraph_FaceSurfaceRepId().IsValid());
  EXPECT_FALSE(BRepGraph_FaceTriangulationRepId().IsValid());
}
