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

#include <GeomAdaptor_Surface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <IntPatch_Polyhedron.hxx>
#include <gp_Ax3.hxx>

#include <gtest/gtest.h>

static occ::handle<GeomAdaptor_Surface> makePlane()
{
  gp_Ax3                  anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(anAxis);
  return new GeomAdaptor_Surface(aPlane, -1., 1., -1., 1.);
}

static occ::handle<GeomAdaptor_Surface> makeSphere()
{
  gp_Ax3                             anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(anAxis, 1.0);
  return new GeomAdaptor_Surface(aSphere);
}

// Basic construction with auto-computed subdivision.
TEST(IntPatch_Polyhedron, DefaultConstructor_ProducesValidMesh)
{
  occ::handle<GeomAdaptor_Surface> aSurf = makeSphere();
  IntPatch_Polyhedron              aPoly(aSurf);

  int nbU = 0, nbV = 0;
  aPoly.Size(nbU, nbV);
  EXPECT_GT(nbU, 0);
  EXPECT_GT(nbV, 0);
  EXPECT_GT(aPoly.NbTriangles(), 0);
  EXPECT_GT(aPoly.NbPoints(), 0);
}

// Regression test for bug #51: zero nbu/nbv must not cause division-by-zero.
// Constructor clamps to minimum 1.
TEST(IntPatch_Polyhedron, ZeroSubdivision_ClampedToMinimum)
{
  occ::handle<GeomAdaptor_Surface> aSurf = makePlane();

  IntPatch_Polyhedron aPoly(aSurf, 0, 0);
  int                 nbU = 0, nbV = 0;
  aPoly.Size(nbU, nbV);
  EXPECT_GE(nbU, 1);
  EXPECT_GE(nbV, 1);
  EXPECT_GT(aPoly.NbTriangles(), 0);
}

// Small subdivision values should work normally.
TEST(IntPatch_Polyhedron, SmallSubdivision_ProducesValidMesh)
{
  occ::handle<GeomAdaptor_Surface> aSurf = makePlane();
  IntPatch_Polyhedron              aPoly(aSurf, 2, 2);

  int nbU = 0, nbV = 0;
  aPoly.Size(nbU, nbV);
  EXPECT_EQ(nbU, 2);
  EXPECT_EQ(nbV, 2);
  EXPECT_EQ(aPoly.NbTriangles(), 2 * 2 * 2);
}

// Regression test for bug #53: TriConnex must not crash when Pedge == 0.
TEST(IntPatch_Polyhedron, TriConnex_PedgeZero_NoCrash)
{
  occ::handle<GeomAdaptor_Surface> aSurf = makeSphere();
  IntPatch_Polyhedron              aPoly(aSurf, 4, 4);

  // Get a valid triangle and pivot point.
  int aP1 = 0, aP2 = 0, aP3 = 0;
  aPoly.Triangle(1, aP1, aP2, aP3);

  // Call TriConnex with Pedge = 0 (unknown edge mode).
  int aTriCon = 0, anOtherP = 0;
  int aResult = aPoly.TriConnex(1, aP1, 0, aTriCon, anOtherP);
  // Must not crash; result is a valid triangle index or 0.
  EXPECT_GE(aResult, 0);
}

// TriConnex with various Pedge values must not crash.
TEST(IntPatch_Polyhedron, TriConnex_AllVertices_NoCrash)
{
  occ::handle<GeomAdaptor_Surface> aSurf = makeSphere();
  IntPatch_Polyhedron              aPoly(aSurf, 3, 3);

  int aP1 = 0, aP2 = 0, aP3 = 0;
  aPoly.Triangle(1, aP1, aP2, aP3);

  // Call TriConnex with each vertex as Pedge and with Pedge=0.
  int aTriCon = 0, anOtherP = 0;
  aPoly.TriConnex(1, aP1, 0, aTriCon, anOtherP);
  aPoly.TriConnex(1, aP1, aP2, aTriCon, anOtherP);
  aPoly.TriConnex(1, aP1, aP3, aTriCon, anOtherP);
  aPoly.TriConnex(1, aP2, aP3, aTriCon, anOtherP);
  // All calls must complete without crash.
  SUCCEED();
}
