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
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <IntPolyh_Intersection.hxx>
#include <Precision.hxx>
#include <gp_Ax3.hxx>

#include <gtest/gtest.h>

// Helper to validate UV coordinates: they must be finite and within surface bounds.
static void validateUV(const double theU,
                       const double theV,
                       const char*  theSurfName,
                       const int    theLineIdx,
                       const int    thePntIdx)
{
  EXPECT_TRUE(std::isfinite(theU))
    << theSurfName << " U is not finite at line=" << theLineIdx << " pt=" << thePntIdx;
  EXPECT_TRUE(std::isfinite(theV))
    << theSurfName << " V is not finite at line=" << theLineIdx << " pt=" << thePntIdx;
}

// Regression test for bugs #13/#14: UV coordinate assignment in degenerate
// alpha branches of IntPolyh_MaillageAffinage must correctly pair SetUV1/SetUV2.
TEST(IntPolyh_Intersection, SpherePlane_ValidUVCoordinates)
{
  gp_Ax3 anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));

  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(anAxis, 1.0);
  occ::handle<GeomAdaptor_Surface>   aSurfS  = new GeomAdaptor_Surface(aSphere);

  occ::handle<Geom_Plane>          aPlane = new Geom_Plane(anAxis);
  occ::handle<GeomAdaptor_Surface> aSurfP = new GeomAdaptor_Surface(aPlane);

  IntPolyh_Intersection anInter(aSurfS, aSurfP);

  EXPECT_TRUE(anInter.IsDone());

  const int aNbLines = anInter.NbSectionLines();
  EXPECT_GT(aNbLines, 0);

  // Verify all section-line points have valid UV coordinates on both surfaces.
  for (int iL = 1; iL <= aNbLines; ++iL)
  {
    const int aNbPts = anInter.NbPointsInLine(iL);
    for (int iP = 1; iP <= aNbPts; ++iP)
    {
      double x = 0., y = 0., z = 0., u1 = 0., v1 = 0., u2 = 0., v2 = 0., incidence = 0.;
      anInter.GetLinePoint(iL, iP, x, y, z, u1, v1, u2, v2, incidence);

      validateUV(u1, v1, "S1", iL, iP);
      validateUV(u2, v2, "S2", iL, iP);
    }
  }
}

// Sphere-cylinder intersection produces section lines with correct UVs.
TEST(IntPolyh_Intersection, SphereCylinder_ValidUVCoordinates)
{
  gp_Ax3 anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));

  occ::handle<Geom_SphericalSurface>    aSphere = new Geom_SphericalSurface(anAxis, 2.0);
  occ::handle<GeomAdaptor_Surface>      aSurfS  = new GeomAdaptor_Surface(aSphere);

  gp_Ax3                                aCylAxis(gp_Pnt(1.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  occ::handle<Geom_CylindricalSurface>  aCyl = new Geom_CylindricalSurface(aCylAxis, 1.0);
  occ::handle<GeomAdaptor_Surface>      aSurfC = new GeomAdaptor_Surface(aCyl);

  IntPolyh_Intersection anInter(aSurfS, aSurfC);
  EXPECT_TRUE(anInter.IsDone());

  const int aNbLines = anInter.NbSectionLines();
  EXPECT_GT(aNbLines, 0);

  for (int iL = 1; iL <= aNbLines; ++iL)
  {
    const int aNbPts = anInter.NbPointsInLine(iL);
    for (int iP = 1; iP <= aNbPts; ++iP)
    {
      double x = 0., y = 0., z = 0., u1 = 0., v1 = 0., u2 = 0., v2 = 0., incidence = 0.;
      anInter.GetLinePoint(iL, iP, x, y, z, u1, v1, u2, v2, incidence);

      validateUV(u1, v1, "S1", iL, iP);
      validateUV(u2, v2, "S2", iL, iP);

      // UV1 must not equal UV2 (different surfaces have different parametrization).
      const bool isSameUV = (std::abs(u1 - u2) < Precision::PConfusion()
                             && std::abs(v1 - v2) < Precision::PConfusion());
      EXPECT_FALSE(isSameUV)
        << "UV1 == UV2 at line=" << iL << " pt=" << iP << " (possible SetUV copy/paste bug)";
    }
  }
}

// Basic intersection: two tilted planes should intersect along a line.
TEST(IntPolyh_Intersection, TwoPlanes_ProducesSectionLine)
{
  gp_Ax3 anAxis1(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  gp_Ax3 anAxis2(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 1));

  occ::handle<Geom_Plane>          aPlane1 = new Geom_Plane(anAxis1);
  occ::handle<GeomAdaptor_Surface> aSurf1  = new GeomAdaptor_Surface(aPlane1, -5., 5., -5., 5.);

  occ::handle<Geom_Plane>          aPlane2 = new Geom_Plane(anAxis2);
  occ::handle<GeomAdaptor_Surface> aSurf2  = new GeomAdaptor_Surface(aPlane2, -5., 5., -5., 5.);

  IntPolyh_Intersection anInter(aSurf1, aSurf2);
  EXPECT_TRUE(anInter.IsDone());
  EXPECT_GT(anInter.NbSectionLines(), 0);
}
