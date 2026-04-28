// Copyright (c) 2024 OPEN CASCADE SAS
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

#include <Adaptor3d_CurveOnSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <gp_Ax3.hxx>
#include <gp_Sphere.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pln.hxx>
#include <IntPatch_BVHTraversal.hxx>
#include <IntPatch_InterferencePolyhedron.hxx>
#include <IntPatch_Polyhedron.hxx>
#include <IntPatch_PolyhedronBVH.hxx>
#include <IntPatch_PolyhedronTool.hxx>

// Test fixture for IntPatch_PolyhedronBVH tests
class IntPatch_PolyhedronBVHTest : public testing::Test
{
protected:
  void SetUp() override
  {
    // Create a sphere surface
    const gp_Sphere                          aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 1.0);
    const occ::handle<Geom_SphericalSurface> aSphereSurf = new Geom_SphericalSurface(aSphere);
    mySphereAdaptor                                      = new GeomAdaptor_Surface(aSphereSurf);

    // Create a cylinder surface
    const gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0.5, 0, 0), gp_Dir(0, 0, 1)), 0.8);
    const occ::handle<Geom_CylindricalSurface> aCylSurf = new Geom_CylindricalSurface(aCyl);
    myCylAdaptor = new GeomAdaptor_Surface(aCylSurf, 0, 2 * M_PI, -1, 1);
  }

  occ::handle<GeomAdaptor_Surface> mySphereAdaptor;
  occ::handle<GeomAdaptor_Surface> myCylAdaptor;
};

// Test that PolyhedronBVH can be constructed and has correct size
TEST_F(IntPatch_PolyhedronBVHTest, Construction)
{
  constexpr int aNbU = 10;
  constexpr int aNbV = 10;

  const IntPatch_Polyhedron    aPoly(mySphereAdaptor, aNbU, aNbV);
  const IntPatch_PolyhedronBVH aBVH(aPoly);

  EXPECT_TRUE(aBVH.IsInitialized());
  EXPECT_GT(aBVH.Size(), 0);
  EXPECT_LE(aBVH.Size(), IntPatch_PolyhedronTool::NbTriangles(aPoly));
}

// Test that Box() returns valid bounding boxes
TEST_F(IntPatch_PolyhedronBVHTest, Box)
{
  constexpr int aNbU = 5;
  constexpr int aNbV = 5;

  const IntPatch_Polyhedron    aPoly(mySphereAdaptor, aNbU, aNbV);
  const IntPatch_PolyhedronBVH aBVH(aPoly);

  // Check that all boxes are valid
  for (int i = 0; i < aBVH.Size(); ++i)
  {
    const BVH_Box<double, 3> aBox = aBVH.Box(i);
    EXPECT_TRUE(aBox.IsValid()) << "Box " << i << " is not valid";
  }
}

// Test that Center() returns valid centroid coordinates
TEST_F(IntPatch_PolyhedronBVHTest, Center)
{
  constexpr int aNbU = 5;
  const int     aNbV = 5;

  const IntPatch_Polyhedron    aPoly(mySphereAdaptor, aNbU, aNbV);
  const IntPatch_PolyhedronBVH aBVH(aPoly);

  // Check that centroids are within the overall bounding box
  const Bnd_Box& aBounding = IntPatch_PolyhedronTool::Bounding(aPoly);
  double         aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBounding.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  for (int i = 0; i < aBVH.Size(); ++i)
  {
    const double aCx = aBVH.Center(i, 0);
    const double aCy = aBVH.Center(i, 1);
    const double aCz = aBVH.Center(i, 2);

    EXPECT_GE(aCx, aXmin - 1e-10) << "Center X of triangle " << i << " is below min";
    EXPECT_LE(aCx, aXmax + 1e-10) << "Center X of triangle " << i << " is above max";
    EXPECT_GE(aCy, aYmin - 1e-10) << "Center Y of triangle " << i << " is below min";
    EXPECT_LE(aCy, aYmax + 1e-10) << "Center Y of triangle " << i << " is above max";
    EXPECT_GE(aCz, aZmin - 1e-10) << "Center Z of triangle " << i << " is below min";
    EXPECT_LE(aCz, aZmax + 1e-10) << "Center Z of triangle " << i << " is above max";
  }
}

// Test that OriginalIndex() returns valid 1-based indices
TEST_F(IntPatch_PolyhedronBVHTest, OriginalIndex)
{
  constexpr int aNbU = 5;
  constexpr int aNbV = 5;

  const IntPatch_Polyhedron aPoly(mySphereAdaptor, aNbU, aNbV);
  IntPatch_PolyhedronBVH    aBVH(aPoly);

  const int aNbTri     = aBVH.Size();
  const int aNbPolyTri = IntPatch_PolyhedronTool::NbTriangles(aPoly);

  // Before BVH build, indices should be sequential
  for (int i = 0; i < aNbTri; ++i)
  {
    const int anOrigIdx = aBVH.OriginalIndex(i);
    EXPECT_GE(anOrigIdx, 1) << "Original index should be >= 1";
    EXPECT_LE(anOrigIdx, aNbPolyTri) << "Original index should be <= NbTriangles";
  }

  // Force BVH build
  aBVH.BVH();

  // After BVH build, each original index should appear exactly once
  std::vector<bool> aUsed(aNbPolyTri + 1, false);
  for (int i = 0; i < aNbTri; ++i)
  {
    const int anOrigIdx = aBVH.OriginalIndex(i);
    EXPECT_FALSE(aUsed[anOrigIdx]) << "Original index " << anOrigIdx << " used more than once";
    aUsed[anOrigIdx] = true;
  }
}

// Test BVH traversal finds overlapping triangles
TEST_F(IntPatch_PolyhedronBVHTest, Traversal)
{
  constexpr int aNbU = 10;
  constexpr int aNbV = 10;

  const IntPatch_Polyhedron aPoly1(mySphereAdaptor, aNbU, aNbV);
  const IntPatch_Polyhedron aPoly2(myCylAdaptor, aNbU, aNbV);

  IntPatch_PolyhedronBVH aSet1(aPoly1);
  IntPatch_PolyhedronBVH aSet2(aPoly2);

  IntPatch_BVHTraversal aTraversal;
  const int             aNbPairs = aTraversal.Perform(aSet1, aSet2, false);

  // The sphere and cylinder should have some overlapping triangles
  EXPECT_GT(aNbPairs, 0) << "Expected some overlapping triangle pairs";
  EXPECT_EQ(aNbPairs, static_cast<int>(aTraversal.Pairs().Size()));

  // Verify all pairs have valid indices
  for (const auto& aPair : aTraversal.Pairs())
  {
    EXPECT_GE(aPair.First, 1);
    EXPECT_LE(aPair.First, IntPatch_PolyhedronTool::NbTriangles(aPoly1));
    EXPECT_GE(aPair.Second, 1);
    EXPECT_LE(aPair.Second, IntPatch_PolyhedronTool::NbTriangles(aPoly2));
  }
}

// Test self-interference mode
TEST_F(IntPatch_PolyhedronBVHTest, SelfInterference)
{
  constexpr int aNbU = 5;
  constexpr int aNbV = 5;

  const IntPatch_Polyhedron aPoly(mySphereAdaptor, aNbU, aNbV);
  IntPatch_PolyhedronBVH    aSet(aPoly);

  IntPatch_BVHTraversal aTraversal;
  aTraversal.Perform(aSet, aSet, true); // self-interference mode

  // In self-interference mode, First < Second for all pairs
  for (const auto& aPair : aTraversal.Pairs())
  {
    EXPECT_LT(aPair.First, aPair.Second) << "Self-interference should have First < Second";
  }
}

// Test that IntPatch_InterferencePolyhedron works with BVH
TEST_F(IntPatch_PolyhedronBVHTest, InterferencePolyhedron)
{
  constexpr int aNbU = 10;
  constexpr int aNbV = 10;

  const IntPatch_Polyhedron aPoly1(mySphereAdaptor, aNbU, aNbV);
  const IntPatch_Polyhedron aPoly2(myCylAdaptor, aNbU, aNbV);

  // Create interference and check it completes without error
  const IntPatch_InterferencePolyhedron anInterf(aPoly1, aPoly2);

  // The result should have some section points or lines
  // (exact number depends on geometry, just verify it runs)
  const int aNbPoints = anInterf.NbSectionPoints();
  const int aNbLines  = anInterf.NbSectionLines();
  const int aNbZones  = anInterf.NbTangentZones();

  // At least some intersection should be found
  EXPECT_TRUE(aNbPoints > 0 || aNbLines > 0 || aNbZones > 0)
    << "Expected some intersection results";
}

// Test with non-overlapping surfaces
TEST_F(IntPatch_PolyhedronBVHTest, NoOverlap)
{
  // Create a plane far from the sphere
  const gp_Pln                           aPlane(gp_Pnt(10, 10, 10), gp_Dir(1, 0, 0));
  const occ::handle<Geom_Plane>          aPlaneSurf = new Geom_Plane(aPlane);
  const occ::handle<GeomAdaptor_Surface> aPlaneAdaptor =
    new GeomAdaptor_Surface(aPlaneSurf, -1, 1, -1, 1);

  const int aNbU = 5;
  const int aNbV = 5;

  const IntPatch_Polyhedron aPoly1(mySphereAdaptor, aNbU, aNbV);
  const IntPatch_Polyhedron aPoly2(aPlaneAdaptor, aNbU, aNbV);

  IntPatch_PolyhedronBVH aSet1(aPoly1);
  IntPatch_PolyhedronBVH aSet2(aPoly2);

  IntPatch_BVHTraversal aTraversal;
  const int             aNbPairs = aTraversal.Perform(aSet1, aSet2, false);

  // Far-away surfaces should have no overlapping boxes
  EXPECT_EQ(aNbPairs, 0) << "Expected no overlapping triangle pairs for distant surfaces";
}
