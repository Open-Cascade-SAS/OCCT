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
#include <IntCurveSurface_ThePolyhedronOfHInter.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Ax3.hxx>

#include <gtest/gtest.h>

static occ::handle<GeomAdaptor_Surface> makeTestPlane()
{
  gp_Ax3                  anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(anAxis);
  return new GeomAdaptor_Surface(aPlane, -1., 1., -1., 1.);
}

// Regression test for bug #55: singularity flags must be initialized to false.
TEST(IntCurveSurface_ThePolyhedronOfHInter, SingularityFlags_InitializedFalse)
{
  occ::handle<GeomAdaptor_Surface>        aSurf = makeTestPlane();
  IntCurveSurface_ThePolyhedronOfHInter aPoly(aSurf, 3, 3, -1., -1., 1., 1.);

  EXPECT_FALSE(aPoly.HasUMinSingularity());
  EXPECT_FALSE(aPoly.HasUMaxSingularity());
  EXPECT_FALSE(aPoly.HasVMinSingularity());
  EXPECT_FALSE(aPoly.HasVMaxSingularity());
}

// Singularity setters should update the flags.
TEST(IntCurveSurface_ThePolyhedronOfHInter, SingularitySetters_UpdateFlags)
{
  occ::handle<GeomAdaptor_Surface>        aSurf = makeTestPlane();
  IntCurveSurface_ThePolyhedronOfHInter aPoly(aSurf, 3, 3, -1., -1., 1., 1.);

  aPoly.UMinSingularity(true);
  EXPECT_TRUE(aPoly.HasUMinSingularity());
  EXPECT_FALSE(aPoly.HasUMaxSingularity());

  aPoly.VMaxSingularity(true);
  EXPECT_TRUE(aPoly.HasVMaxSingularity());
  EXPECT_FALSE(aPoly.HasVMinSingularity());
}

// Basic construction produces valid polyhedron.
TEST(IntCurveSurface_ThePolyhedronOfHInter, BasicConstruction_ValidMesh)
{
  occ::handle<GeomAdaptor_Surface>        aSurf = makeTestPlane();
  IntCurveSurface_ThePolyhedronOfHInter aPoly(aSurf, 4, 4, -1., -1., 1., 1.);

  int nbU = 0, nbV = 0;
  aPoly.Size(nbU, nbV);
  EXPECT_EQ(nbU, 4);
  EXPECT_EQ(nbV, 4);
  EXPECT_EQ(aPoly.NbTriangles(), 4 * 4 * 2);
  EXPECT_EQ(aPoly.NbPoints(), (4 + 1) * (4 + 1));
}

// Regression test for bug #58: parameter-array constructor with minimum-size arrays
// must not cause division by zero (arrays of length 2 give nbdelta = 1).
TEST(IntCurveSurface_ThePolyhedronOfHInter, ParamArrayConstructor_MinimumSize)
{
  occ::handle<GeomAdaptor_Surface> aSurf = makeTestPlane();

  NCollection_Array1<double> aUpars(1, 2);
  aUpars(1) = -1.0;
  aUpars(2) = 1.0;

  NCollection_Array1<double> aVpars(1, 2);
  aVpars(1) = -1.0;
  aVpars(2) = 1.0;

  IntCurveSurface_ThePolyhedronOfHInter aPoly(aSurf, aUpars, aVpars);

  int nbU = 0, nbV = 0;
  aPoly.Size(nbU, nbV);
  EXPECT_GE(nbU, 1);
  EXPECT_GE(nbV, 1);
  EXPECT_GT(aPoly.NbTriangles(), 0);
}

// PlaneEquation for a valid triangle must produce finite results.
TEST(IntCurveSurface_ThePolyhedronOfHInter, PlaneEquation_FiniteResults)
{
  occ::handle<GeomAdaptor_Surface>        aSurf = makeTestPlane();
  IntCurveSurface_ThePolyhedronOfHInter aPoly(aSurf, 3, 3, -1., -1., 1., 1.);

  gp_XYZ  aNormal;
  double  aPolarDist = 0.0;
  aPoly.PlaneEquation(1, aNormal, aPolarDist);

  EXPECT_TRUE(std::isfinite(aNormal.X()));
  EXPECT_TRUE(std::isfinite(aNormal.Y()));
  EXPECT_TRUE(std::isfinite(aNormal.Z()));
  EXPECT_TRUE(std::isfinite(aPolarDist));
}
