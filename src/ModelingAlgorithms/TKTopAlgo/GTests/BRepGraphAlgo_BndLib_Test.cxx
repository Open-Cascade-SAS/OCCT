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
#include <Bnd_OBB.hxx>
#include <BRep_Builder.hxx>
#include <BRepBndLib.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraphAlgo_BndLib.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <Precision.hxx>
#include <TopoDS_Compound.hxx>

#include <gtest/gtest.h>

namespace
{

//! Compare two Bnd_Box instances for near-equality.
//! Checks that the min/max corners differ by at most theTol.
void expectBoxNear(const Bnd_Box& theGraphBox,
                   const Bnd_Box& theLegacyBox,
                   double         theTol)
{
  ASSERT_FALSE(theGraphBox.IsVoid()) << "Graph box is void";
  ASSERT_FALSE(theLegacyBox.IsVoid()) << "Legacy box is void";

  double gxmin, gymin, gzmin, gxmax, gymax, gzmax;
  double lxmin, lymin, lzmin, lxmax, lymax, lzmax;
  theGraphBox.Get(gxmin, gymin, gzmin, gxmax, gymax, gzmax);
  theLegacyBox.Get(lxmin, lymin, lzmin, lxmax, lymax, lzmax);

  EXPECT_NEAR(gxmin, lxmin, theTol) << "XMin differs";
  EXPECT_NEAR(gymin, lymin, theTol) << "YMin differs";
  EXPECT_NEAR(gzmin, lzmin, theTol) << "ZMin differs";
  EXPECT_NEAR(gxmax, lxmax, theTol) << "XMax differs";
  EXPECT_NEAR(gymax, lymax, theTol) << "YMax differs";
  EXPECT_NEAR(gzmax, lzmax, theTol) << "ZMax differs";
}

//! Build graph from shape and verify it's done.
BRepGraph buildGraph(const TopoDS_Shape& theShape)
{
  BRepGraph aGraph;
  aGraph.Build(theShape);
  return aGraph;
}

} // namespace

// =====================================================
// Add() tests
// =====================================================

TEST(BRepGraphAlgo_BndLibTest, Add_Box_MatchesLegacy)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph = buildGraph(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  Bnd_Box aGraphBox;
  BRepGraphAlgo_BndLib::Add(aGraph, aGraphBox);

  Bnd_Box aLegacyBox;
  BRepBndLib::Add(aBox, aLegacyBox);

  expectBoxNear(aGraphBox, aLegacyBox, Precision::Confusion());
}

TEST(BRepGraphAlgo_BndLibTest, Add_Sphere_MatchesLegacy)
{
  const TopoDS_Shape aSphere = BRepPrimAPI_MakeSphere(10.0).Shape();

  BRepGraph aGraph = buildGraph(aSphere);
  ASSERT_TRUE(aGraph.IsDone());

  Bnd_Box aGraphBox;
  BRepGraphAlgo_BndLib::Add(aGraph, aGraphBox);

  Bnd_Box aLegacyBox;
  BRepBndLib::Add(aSphere, aLegacyBox);

  expectBoxNear(aGraphBox, aLegacyBox, 0.1);
}

TEST(BRepGraphAlgo_BndLibTest, Add_Cylinder_MatchesLegacy)
{
  const TopoDS_Shape aCyl = BRepPrimAPI_MakeCylinder(5.0, 10.0).Shape();

  BRepGraph aGraph = buildGraph(aCyl);
  ASSERT_TRUE(aGraph.IsDone());

  Bnd_Box aGraphBox;
  BRepGraphAlgo_BndLib::Add(aGraph, aGraphBox);

  Bnd_Box aLegacyBox;
  BRepBndLib::Add(aCyl, aLegacyBox);

  expectBoxNear(aGraphBox, aLegacyBox, 0.1);
}

// =====================================================
// AddOptimal() tests
// =====================================================

TEST(BRepGraphAlgo_BndLibTest, AddOptimal_Box_MatchesLegacy)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph = buildGraph(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  Bnd_Box aGraphBox;
  BRepGraphAlgo_BndLib::AddOptimal(aGraph, aGraphBox);

  Bnd_Box aLegacyBox;
  BRepBndLib::AddOptimal(aBox, aLegacyBox);

  expectBoxNear(aGraphBox, aLegacyBox, Precision::Confusion());
}

TEST(BRepGraphAlgo_BndLibTest, AddOptimal_Sphere_MatchesLegacy)
{
  const TopoDS_Shape aSphere = BRepPrimAPI_MakeSphere(10.0).Shape();

  BRepGraph aGraph = buildGraph(aSphere);
  ASSERT_TRUE(aGraph.IsDone());

  Bnd_Box aGraphBox;
  BRepGraphAlgo_BndLib::AddOptimal(aGraph, aGraphBox);

  Bnd_Box aLegacyBox;
  BRepBndLib::AddOptimal(aSphere, aLegacyBox);

  expectBoxNear(aGraphBox, aLegacyBox, 0.1);
}

TEST(BRepGraphAlgo_BndLibTest, AddOptimal_WithTolerance)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph = buildGraph(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  Bnd_Box aGraphBox;
  BRepGraphAlgo_BndLib::AddOptimal(aGraph, aGraphBox, true, true);

  Bnd_Box aLegacyBox;
  BRepBndLib::AddOptimal(aBox, aLegacyBox, true, true);

  expectBoxNear(aGraphBox, aLegacyBox, Precision::Confusion());
}

// =====================================================
// Per-node Add() tests
// =====================================================

TEST(BRepGraphAlgo_BndLibTest, Add_PerNode_Face)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph = buildGraph(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Defs().NbFaces(), 0);

  Bnd_Box aFaceBox;
  BRepGraphAlgo_BndLib::Add(aGraph, BRepGraph_NodeId::Face(0), aFaceBox);

  EXPECT_FALSE(aFaceBox.IsVoid());

  // Face box should be smaller than the whole shape box.
  Bnd_Box aWholeBox;
  BRepGraphAlgo_BndLib::Add(aGraph, aWholeBox);

  double fxmin, fymin, fzmin, fxmax, fymax, fzmax;
  double wxmin, wymin, wzmin, wxmax, wymax, wzmax;
  aFaceBox.Get(fxmin, fymin, fzmin, fxmax, fymax, fzmax);
  aWholeBox.Get(wxmin, wymin, wzmin, wxmax, wymax, wzmax);

  // Face box should be contained within the whole box (with tolerance).
  const double aTol = 1.0;
  EXPECT_GE(fxmin, wxmin - aTol);
  EXPECT_GE(fymin, wymin - aTol);
  EXPECT_GE(fzmin, wzmin - aTol);
  EXPECT_LE(fxmax, wxmax + aTol);
  EXPECT_LE(fymax, wymax + aTol);
  EXPECT_LE(fzmax, wzmax + aTol);
}

TEST(BRepGraphAlgo_BndLibTest, Add_PerNode_Edge)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph = buildGraph(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Defs().NbEdges(), 0);

  Bnd_Box anEdgeBox;
  BRepGraphAlgo_BndLib::Add(aGraph, BRepGraph_NodeId::Edge(0), anEdgeBox);

  EXPECT_FALSE(anEdgeBox.IsVoid());
}

TEST(BRepGraphAlgo_BndLibTest, Add_PerNode_Shell)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph = buildGraph(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Defs().NbShells(), 0);

  Bnd_Box aShellBox;
  BRepGraphAlgo_BndLib::Add(aGraph, BRepGraph_NodeId::Shell(0), aShellBox);

  // Shell box should match the whole box (box has one shell).
  Bnd_Box aWholeBox;
  BRepGraphAlgo_BndLib::Add(aGraph, aWholeBox);

  expectBoxNear(aShellBox, aWholeBox, 1.0);
}

TEST(BRepGraphAlgo_BndLibTest, Add_PerNode_Solid)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph = buildGraph(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Defs().NbSolids(), 0);

  Bnd_Box aSolidBox;
  BRepGraphAlgo_BndLib::Add(aGraph, BRepGraph_NodeId::Solid(0), aSolidBox);

  Bnd_Box aWholeBox;
  BRepGraphAlgo_BndLib::Add(aGraph, aWholeBox);

  expectBoxNear(aSolidBox, aWholeBox, 1.0);
}

// =====================================================
// AddOBB() tests
// =====================================================

TEST(BRepGraphAlgo_BndLibTest, AddOBB_Box_MatchesLegacy)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph = buildGraph(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  Bnd_OBB aGraphOBB;
  BRepGraphAlgo_BndLib::AddOBB(aGraph, aGraphOBB);

  Bnd_OBB aLegacyOBB;
  BRepBndLib::AddOBB(aBox, aLegacyOBB);

  EXPECT_FALSE(aGraphOBB.IsVoid());
  EXPECT_FALSE(aLegacyOBB.IsVoid());

  // Compare centers.
  EXPECT_NEAR(aGraphOBB.Center().X(), aLegacyOBB.Center().X(), 1.0);
  EXPECT_NEAR(aGraphOBB.Center().Y(), aLegacyOBB.Center().Y(), 1.0);
  EXPECT_NEAR(aGraphOBB.Center().Z(), aLegacyOBB.Center().Z(), 1.0);
}

TEST(BRepGraphAlgo_BndLibTest, AddOBB_Sphere_MatchesLegacy)
{
  const TopoDS_Shape aSphere = BRepPrimAPI_MakeSphere(10.0).Shape();

  BRepGraph aGraph = buildGraph(aSphere);
  ASSERT_TRUE(aGraph.IsDone());

  Bnd_OBB aGraphOBB;
  BRepGraphAlgo_BndLib::AddOBB(aGraph, aGraphOBB);

  Bnd_OBB aLegacyOBB;
  BRepBndLib::AddOBB(aSphere, aLegacyOBB);

  EXPECT_FALSE(aGraphOBB.IsVoid());
  EXPECT_FALSE(aLegacyOBB.IsVoid());

  // For spheres the centers should match roughly.
  EXPECT_NEAR(aGraphOBB.Center().X(), aLegacyOBB.Center().X(), 1.0);
  EXPECT_NEAR(aGraphOBB.Center().Y(), aLegacyOBB.Center().Y(), 1.0);
  EXPECT_NEAR(aGraphOBB.Center().Z(), aLegacyOBB.Center().Z(), 1.0);
}

// =====================================================
// Compound tests
// =====================================================

TEST(BRepGraphAlgo_BndLibTest, Add_Compound)
{
  const TopoDS_Shape aBox1 = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  const TopoDS_Shape aBox2 = BRepPrimAPI_MakeBox(gp_Pnt(20.0, 0.0, 0.0), 10.0, 10.0, 10.0).Shape();

  BRep_Builder       aBuilder;
  TopoDS_Compound    aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBox1);
  aBuilder.Add(aCompound, aBox2);

  BRepGraph aGraph = buildGraph(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  Bnd_Box aGraphBox;
  BRepGraphAlgo_BndLib::Add(aGraph, aGraphBox);

  Bnd_Box aLegacyBox;
  BRepBndLib::Add(aCompound, aLegacyBox);

  expectBoxNear(aGraphBox, aLegacyBox, Precision::Confusion());
}

// =====================================================
// Edge cases
// =====================================================

TEST(BRepGraphAlgo_BndLibTest, Add_EmptyGraph_VoidBox)
{
  TopoDS_Compound anEmpty;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(anEmpty);

  BRepGraph aGraph = buildGraph(anEmpty);

  Bnd_Box aBox;
  BRepGraphAlgo_BndLib::Add(aGraph, aBox);

  EXPECT_TRUE(aBox.IsVoid());
}

// =====================================================
// Cached API tests
// =====================================================

TEST(BRepGraphAlgo_BndLibTest, AddCached_Standard_ReturnsNonVoid)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph = buildGraph(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Defs().NbFaces(), 0);

  Bnd_Box aCachedBox = BRepGraphAlgo_BndLib::AddCached(
    aGraph, BRepGraph_NodeId::Face(0), BRepGraphAlgo_BndLib::Precision::Standard);

  EXPECT_FALSE(aCachedBox.IsVoid());
}

TEST(BRepGraphAlgo_BndLibTest, GetCached_AfterAddCached_ReturnsTrue)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph = buildGraph(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId aFaceId = BRepGraph_NodeId::Face(0);

  // Initially no cache.
  BRepGraphAlgo_BndLib::CachedData aData;
  EXPECT_FALSE(BRepGraphAlgo_BndLib::GetCached(aGraph, aFaceId, aData));

  // Populate cache.
  BRepGraphAlgo_BndLib::AddCached(aGraph, aFaceId, BRepGraphAlgo_BndLib::Precision::Standard);

  // Now cache should be available.
  EXPECT_TRUE(BRepGraphAlgo_BndLib::GetCached(aGraph, aFaceId, aData));
  EXPECT_FALSE(aData.Box.IsVoid());
  EXPECT_EQ(aData.BoxPrecision, BRepGraphAlgo_BndLib::Precision::Standard);
}

TEST(BRepGraphAlgo_BndLibTest, AddCached_OptimalSupersedesStandard)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph = buildGraph(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId aFaceId = BRepGraph_NodeId::Face(0);

  // Cache at Standard level.
  Bnd_Box aStdBox = BRepGraphAlgo_BndLib::AddCached(
    aGraph, aFaceId, BRepGraphAlgo_BndLib::Precision::Standard);
  ASSERT_FALSE(aStdBox.IsVoid());

  // Request Optimal -- should recompute (Standard < Optimal).
  Bnd_Box anOptBox = BRepGraphAlgo_BndLib::AddCached(
    aGraph, aFaceId, BRepGraphAlgo_BndLib::Precision::Optimal);
  ASSERT_FALSE(anOptBox.IsVoid());

  // Verify cached precision is now Optimal.
  BRepGraphAlgo_BndLib::CachedData aData;
  ASSERT_TRUE(BRepGraphAlgo_BndLib::GetCached(aGraph, aFaceId, aData));
  EXPECT_EQ(aData.BoxPrecision, BRepGraphAlgo_BndLib::Precision::Optimal);

  // Request Standard again -- should return cached Optimal (sufficient).
  Bnd_Box aStdBox2 = BRepGraphAlgo_BndLib::AddCached(
    aGraph, aFaceId, BRepGraphAlgo_BndLib::Precision::Standard);
  EXPECT_FALSE(aStdBox2.IsVoid());
}

TEST(BRepGraphAlgo_BndLibTest, SetCached_ExternalBox_RetrievedByGet)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph = buildGraph(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId aFaceId = BRepGraph_NodeId::Face(0);

  // Store an externally-computed box.
  Bnd_Box anExternalBox;
  anExternalBox.Update(0.0, 0.0, 0.0, 100.0, 200.0, 300.0);

  BRepGraphAlgo_BndLib::SetCached(
    aGraph, aFaceId, anExternalBox, BRepGraphAlgo_BndLib::Precision::Optimal, false, true);

  // Retrieve and verify.
  BRepGraphAlgo_BndLib::CachedData aData;
  ASSERT_TRUE(BRepGraphAlgo_BndLib::GetCached(aGraph, aFaceId, aData));
  EXPECT_EQ(aData.BoxPrecision, BRepGraphAlgo_BndLib::Precision::Optimal);
  EXPECT_TRUE(aData.UsedShapeTolerance);
  EXPECT_FALSE(aData.UsedTriangulation);

  double xn, yn, zn, xx, yx, zx;
  aData.Box.Get(xn, yn, zn, xx, yx, zx);
  EXPECT_NEAR(xx, 100.0, Precision::Confusion());
  EXPECT_NEAR(yx, 200.0, Precision::Confusion());
  EXPECT_NEAR(zx, 300.0, Precision::Confusion());
}

TEST(BRepGraphAlgo_BndLibTest, InvalidateCached_ForcesRecompute)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph = buildGraph(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_NodeId aFaceId = BRepGraph_NodeId::Face(0);

  // Populate cache.
  Bnd_Box aBox1 = BRepGraphAlgo_BndLib::AddCached(
    aGraph, aFaceId, BRepGraphAlgo_BndLib::Precision::Standard);
  ASSERT_FALSE(aBox1.IsVoid());

  // Invalidate.
  BRepGraphAlgo_BndLib::InvalidateCached(aGraph, aFaceId);

  // GetCached should now return false (dirty).
  BRepGraphAlgo_BndLib::CachedData aData;
  EXPECT_FALSE(BRepGraphAlgo_BndLib::GetCached(aGraph, aFaceId, aData));

  // AddCached should recompute and return a valid box.
  Bnd_Box aBox2 = BRepGraphAlgo_BndLib::AddCached(
    aGraph, aFaceId, BRepGraphAlgo_BndLib::Precision::Standard);
  ASSERT_FALSE(aBox2.IsVoid());

  // Results should match (same geometry, no mutation).
  double xn1, yn1, zn1, xx1, yx1, zx1;
  double xn2, yn2, zn2, xx2, yx2, zx2;
  aBox1.Get(xn1, yn1, zn1, xx1, yx1, zx1);
  aBox2.Get(xn2, yn2, zn2, xx2, yx2, zx2);

  EXPECT_NEAR(xn1, xn2, Precision::Confusion());
  EXPECT_NEAR(yn1, yn2, Precision::Confusion());
  EXPECT_NEAR(zn1, zn2, Precision::Confusion());
  EXPECT_NEAR(xx1, xx2, Precision::Confusion());
  EXPECT_NEAR(yx1, yx2, Precision::Confusion());
  EXPECT_NEAR(zx1, zx2, Precision::Confusion());
}
