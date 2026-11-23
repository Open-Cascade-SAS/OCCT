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

#include <GeomInt_TheImpPrmSvSurfacesOfWLApprox.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_Plane.hxx>
#include <IntSurf_Quadric.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>

#include <gtest/gtest.h>

TEST(ApproxInt_ImpPrmSvSurfacesTest, CacheRestoresRefinedParametersAndSolverMode)
{
  const IntSurf_Quadric         aQuadric{gp_Pln()};
  const occ::handle<Geom_Plane> aPlane =
    new Geom_Plane(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, -1, 0), gp_Dir(1, 0, 0)));
  const occ::handle<GeomAdaptor_Surface> aSurface =
    new GeomAdaptor_Surface(aPlane, -10.0, 10.0, -10.0, 10.0);
  GeomInt_TheImpPrmSvSurfacesOfWLApprox anApproximation(aQuadric, aSurface);
  gp_Pnt                                aPoint;
  gp_Vec                                aTangent;
  gp_Vec2d                              aTangent1, aTangent2;
  for (const double aU : {0.0, 1.0, 0.0, 1.0})
  {
    double aU1 = aU, aV1 = 0.0, aU2 = aU, aV2 = 1.0e-4;
    ASSERT_TRUE(
      anApproximation.Compute(aU1, aV1, aU2, aV2, aPoint, aTangent, aTangent1, aTangent2));
    EXPECT_NEAR(aV2, 0.0, 1.0e-12);
    EXPECT_NEAR(aPoint.Z(), 0.0, 1.0e-12);
  }
  anApproximation.SetUseSolver(false);
  double aU1 = 1.0, aV1 = 0.0, aU2 = 1.0, aV2 = 1.0e-4;
  ASSERT_TRUE(anApproximation.Compute(aU1, aV1, aU2, aV2, aPoint, aTangent, aTangent1, aTangent2));
  EXPECT_EQ(aV2, 1.0e-4);
  EXPECT_NEAR(aPoint.Z(), 5.0e-5, 1.0e-12);
}

//==================================================================================================

TEST(ApproxInt_ImpPrmSvSurfacesTest, ReversedSurfaceOrderRestoresRefinedParameters)
{
  const IntSurf_Quadric         aQuadric{gp_Pln()};
  const occ::handle<Geom_Plane> aPlane =
    new Geom_Plane(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, -1, 0), gp_Dir(1, 0, 0)));
  const occ::handle<GeomAdaptor_Surface> aSurface =
    new GeomAdaptor_Surface(aPlane, -10.0, 10.0, -10.0, 10.0);
  GeomInt_TheImpPrmSvSurfacesOfWLApprox anApproximation(aSurface, aQuadric);
  for (int i = 0; i < 2; ++i)
  {
    double   aU1 = 0.0, aV1 = 1.0e-4, aU2 = 0.0, aV2 = 0.0;
    gp_Pnt   aPoint;
    gp_Vec   aTangent;
    gp_Vec2d aTangent1, aTangent2;
    ASSERT_TRUE(
      anApproximation.Compute(aU1, aV1, aU2, aV2, aPoint, aTangent, aTangent1, aTangent2));
    EXPECT_NEAR(aV1, 0.0, 1.0e-12);
  }
}
