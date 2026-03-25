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

#include <BRepGraph.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraphAlgo_UVBounds.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepTools.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>

#include <gtest/gtest.h>

namespace
{

//! Build graph from shape and verify it's done.
BRepGraph buildGraph(const TopoDS_Shape& theShape)
{
  BRepGraph aGraph;
  aGraph.Build(theShape);
  return aGraph;
}

} // namespace

// =====================================================
// Compute() tests
// =====================================================

TEST(BRepGraphAlgo_UVBoundsTest, Compute_Box_ValidBounds)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph = buildGraph(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  // Box faces are planar; verify valid UV bounds that match legacy.
  int aFaceIdx = 0;
  for (TopExp_Explorer anExp(aBox, TopAbs_FACE); anExp.More(); anExp.Next(), ++aFaceIdx)
  {
    if (aFaceIdx >= aGraph.Defs().NbFaces())
    {
      break;
    }

    BRepGraphAlgo_UVBounds::CachedData aData;
    BRepGraphAlgo_UVBounds::Compute(aGraph, BRepGraph_FaceId(aFaceIdx), aData);
    EXPECT_TRUE(aData.IsValid) << "Face " << aFaceIdx << " should have valid UV bounds";
    EXPECT_LE(aData.UMin, aData.UMax) << "U range for face " << aFaceIdx;
    EXPECT_LE(aData.VMin, aData.VMax) << "V range for face " << aFaceIdx;

    const TopoDS_Face& aFace = TopoDS::Face(anExp.Current());
    double             aLegacyUmin, aLegacyUmax, aLegacyVmin, aLegacyVmax;
    BRepTools::UVBounds(aFace, aLegacyUmin, aLegacyUmax, aLegacyVmin, aLegacyVmax);

    const double aTol = 1.0e-6;
    EXPECT_NEAR(aData.UMin, aLegacyUmin, aTol) << "UMin differs for box face " << aFaceIdx;
    EXPECT_NEAR(aData.UMax, aLegacyUmax, aTol) << "UMax differs for box face " << aFaceIdx;
    EXPECT_NEAR(aData.VMin, aLegacyVmin, aTol) << "VMin differs for box face " << aFaceIdx;
    EXPECT_NEAR(aData.VMax, aLegacyVmax, aTol) << "VMax differs for box face " << aFaceIdx;
  }
}

TEST(BRepGraphAlgo_UVBoundsTest, Compute_Cylinder_MatchesLegacy)
{
  const TopoDS_Shape aCyl = BRepPrimAPI_MakeCylinder(5.0, 20.0).Shape();

  BRepGraph aGraph = buildGraph(aCyl);
  ASSERT_TRUE(aGraph.IsDone());

  // Compare graph-based UV bounds against legacy BRepTools::UVBounds.
  int aFaceIdx = 0;
  for (TopExp_Explorer anExp(aCyl, TopAbs_FACE); anExp.More(); anExp.Next(), ++aFaceIdx)
  {
    if (aFaceIdx >= aGraph.Defs().NbFaces())
    {
      break;
    }

    const TopoDS_Face& aFace = TopoDS::Face(anExp.Current());
    double             aLegacyUmin, aLegacyUmax, aLegacyVmin, aLegacyVmax;
    BRepTools::UVBounds(aFace, aLegacyUmin, aLegacyUmax, aLegacyVmin, aLegacyVmax);

    BRepGraphAlgo_UVBounds::CachedData aData;
    BRepGraphAlgo_UVBounds::Compute(aGraph, BRepGraph_FaceId(aFaceIdx), aData);
    ASSERT_TRUE(aData.IsValid) << "Face " << aFaceIdx;

    const double aTol = 1.0e-6;
    EXPECT_NEAR(aData.UMin, aLegacyUmin, aTol) << "UMin differs for face " << aFaceIdx;
    EXPECT_NEAR(aData.UMax, aLegacyUmax, aTol) << "UMax differs for face " << aFaceIdx;
    EXPECT_NEAR(aData.VMin, aLegacyVmin, aTol) << "VMin differs for face " << aFaceIdx;
    EXPECT_NEAR(aData.VMax, aLegacyVmax, aTol) << "VMax differs for face " << aFaceIdx;
  }
}

TEST(BRepGraphAlgo_UVBoundsTest, Compute_Sphere_PeriodicClamping)
{
  const TopoDS_Shape aSphere = BRepPrimAPI_MakeSphere(10.0).Shape();

  BRepGraph aGraph = buildGraph(aSphere);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GE(aGraph.Defs().NbFaces(), 1);

  // Sphere surface is U-periodic. Check UV bounds are within valid range.
  for (int i = 0; i < aGraph.Defs().NbFaces(); ++i)
  {
    BRepGraphAlgo_UVBounds::CachedData aData;
    BRepGraphAlgo_UVBounds::Compute(aGraph, BRepGraph_FaceId(i), aData);
    ASSERT_TRUE(aData.IsValid) << "Face " << i;

    // UV range should be finite and non-degenerate.
    EXPECT_LT(aData.UMin, aData.UMax) << "U range degenerate for face " << i;
    EXPECT_LE(aData.VMin, aData.VMax) << "V range degenerate for face " << i;
  }

  // Compare against legacy for the sphere.
  int aFaceIdx = 0;
  for (TopExp_Explorer anExp(aSphere, TopAbs_FACE); anExp.More(); anExp.Next(), ++aFaceIdx)
  {
    if (aFaceIdx >= aGraph.Defs().NbFaces())
    {
      break;
    }

    const TopoDS_Face& aFace = TopoDS::Face(anExp.Current());
    double             aLegacyUmin, aLegacyUmax, aLegacyVmin, aLegacyVmax;
    BRepTools::UVBounds(aFace, aLegacyUmin, aLegacyUmax, aLegacyVmin, aLegacyVmax);

    BRepGraphAlgo_UVBounds::CachedData aData;
    BRepGraphAlgo_UVBounds::Compute(aGraph, BRepGraph_FaceId(aFaceIdx), aData);
    ASSERT_TRUE(aData.IsValid);

    const double aTol = 1.0e-6;
    EXPECT_NEAR(aData.UMin, aLegacyUmin, aTol) << "UMin differs for sphere face " << aFaceIdx;
    EXPECT_NEAR(aData.UMax, aLegacyUmax, aTol) << "UMax differs for sphere face " << aFaceIdx;
    EXPECT_NEAR(aData.VMin, aLegacyVmin, aTol) << "VMin differs for sphere face " << aFaceIdx;
    EXPECT_NEAR(aData.VMax, aLegacyVmax, aTol) << "VMax differs for sphere face " << aFaceIdx;
  }
}

// =====================================================
// Cached API tests
// =====================================================

TEST(BRepGraphAlgo_UVBoundsTest, CachedAPI_AddGet_Consistent)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph = buildGraph(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GE(aGraph.Defs().NbFaces(), 1);

  const BRepGraph_NodeId aFaceNode = aGraph.Defs().Face(BRepGraph_FaceId(0)).Id;

  // AddCached computes and stores.
  BRepGraphAlgo_UVBounds::CachedData aAdded = BRepGraphAlgo_UVBounds::AddCached(aGraph, aFaceNode);
  ASSERT_TRUE(aAdded.IsValid);

  // GetCached retrieves the same data.
  BRepGraphAlgo_UVBounds::CachedData aGot;
  ASSERT_TRUE(BRepGraphAlgo_UVBounds::GetCached(aGraph, aFaceNode, aGot));
  EXPECT_TRUE(aGot.IsValid);
  EXPECT_NEAR(aGot.UMin, aAdded.UMin, Precision::Confusion());
  EXPECT_NEAR(aGot.UMax, aAdded.UMax, Precision::Confusion());
  EXPECT_NEAR(aGot.VMin, aAdded.VMin, Precision::Confusion());
  EXPECT_NEAR(aGot.VMax, aAdded.VMax, Precision::Confusion());
  EXPECT_EQ(aGot.IsNaturalRestriction, aAdded.IsNaturalRestriction);
}

TEST(BRepGraphAlgo_UVBoundsTest, CachedAPI_Invalidate_ClearsCache)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph = buildGraph(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GE(aGraph.Defs().NbFaces(), 1);

  const BRepGraph_NodeId aFaceNode = aGraph.Defs().Face(BRepGraph_FaceId(0)).Id;

  // Populate cache.
  BRepGraphAlgo_UVBounds::AddCached(aGraph, aFaceNode);
  BRepGraphAlgo_UVBounds::CachedData aData;
  ASSERT_TRUE(BRepGraphAlgo_UVBounds::GetCached(aGraph, aFaceNode, aData));

  // Invalidate and verify cache is dirty.
  BRepGraphAlgo_UVBounds::InvalidateCached(aGraph, aFaceNode);
  EXPECT_FALSE(BRepGraphAlgo_UVBounds::GetCached(aGraph, aFaceNode, aData));
}
