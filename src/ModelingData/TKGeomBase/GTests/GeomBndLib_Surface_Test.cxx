// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <GeomBndLib_Surface.hxx>

#include <BndLib_AddSurface.hxx>
#include <Bnd_Box.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Precision.hxx>
#include <gp_Ax3.hxx>
#include <gp_Cone.hxx>
#include <gp_Pnt.hxx>

#include <cmath>

#include <gtest/gtest.h>

namespace
{

//! Helper: compare two Bnd_Box results within tolerance.
void CompareBoxes(const Bnd_Box& theNew, const Bnd_Box& theOld, const double theTol)
{
  const auto [aXminN, aXmaxN, aYminN, aYmaxN, aZminN, aZmaxN] = theNew.Get();
  const auto [aXminO, aXmaxO, aYminO, aYmaxO, aZminO, aZmaxO] = theOld.Get();
  EXPECT_NEAR(aXminN, aXminO, theTol);
  EXPECT_NEAR(aXmaxN, aXmaxO, theTol);
  EXPECT_NEAR(aYminN, aYminO, theTol);
  EXPECT_NEAR(aYmaxN, aYmaxO, theTol);
  EXPECT_NEAR(aZminN, aZminO, theTol);
  EXPECT_NEAR(aZmaxN, aZmaxO, theTol);
}

} // namespace

// =========================================================================
// Plane
// =========================================================================

TEST(GeomBndLib_SurfaceTest, Plane_FinitePatch)
{
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp::XOY());
  GeomBndLib_Surface aSurf(aPlane);
  EXPECT_EQ(aSurf.GetType(), GeomAbs_Plane);

  Bnd_Box aBox;
  aSurf.Add(-5.0, 5.0, -3.0, 3.0, 0.0, aBox);

  const auto [aXmin, aXmax, aYmin, aYmax, aZmin, aZmax] = aBox.Get();

  EXPECT_NEAR(aXmin, -5.0, Precision::Confusion());
  EXPECT_NEAR(aYmin, -3.0, Precision::Confusion());
  EXPECT_NEAR(aZmin, 0.0, Precision::Confusion());
  EXPECT_NEAR(aXmax, 5.0, Precision::Confusion());
  EXPECT_NEAR(aYmax, 3.0, Precision::Confusion());
  EXPECT_NEAR(aZmax, 0.0, Precision::Confusion());
}

TEST(GeomBndLib_SurfaceTest, Plane_CompareWithBndLib)
{
  occ::handle<Geom_Plane>  aPlane = new Geom_Plane(gp::XOY());
  GeomAdaptor_Surface anAdaptor(aPlane);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(aPlane).Add(-5.0, 5.0, -3.0, 3.0, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, -5.0, 5.0, -3.0, 3.0, Precision::Confusion(), anOldBox);

  CompareBoxes(aNewBox, anOldBox, Precision::Confusion());
}

// =========================================================================
// Cylinder
// =========================================================================

TEST(GeomBndLib_SurfaceTest, Cylinder_Patch)
{
  const double                    aRadius = 3.0;
  occ::handle<Geom_CylindricalSurface> aCyl    = new Geom_CylindricalSurface(gp::XOY(), aRadius);
  GeomBndLib_Surface              aSurf(aCyl);
  EXPECT_EQ(aSurf.GetType(), GeomAbs_Cylinder);

  Bnd_Box aBox;
  aSurf.Add(0.0, 2.0 * M_PI, 0.0, 10.0, Precision::Confusion(), aBox);

  const auto [aXmin, aXmax, aYmin, aYmax, aZmin, aZmax] = aBox.Get();

  EXPECT_NEAR(aXmin, -aRadius, Precision::Confusion());
  EXPECT_NEAR(aYmin, -aRadius, Precision::Confusion());
  EXPECT_NEAR(aZmin, 0.0, Precision::Confusion());
  EXPECT_NEAR(aXmax, aRadius, Precision::Confusion());
  EXPECT_NEAR(aYmax, aRadius, Precision::Confusion());
  EXPECT_NEAR(aZmax, 10.0, Precision::Confusion());
}

TEST(GeomBndLib_SurfaceTest, Cylinder_CompareWithBndLib)
{
  occ::handle<Geom_CylindricalSurface> aCyl = new Geom_CylindricalSurface(gp::XOY(), 4.0);
  GeomAdaptor_Surface             anAdaptor(aCyl);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(aCyl).Add(0.0, 2.0 * M_PI, -5.0, 5.0, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, 0.0, 2.0 * M_PI, -5.0, 5.0, Precision::Confusion(), anOldBox);

  CompareBoxes(aNewBox, anOldBox, Precision::Confusion());
}

// =========================================================================
// Cone
// =========================================================================

TEST(GeomBndLib_SurfaceTest, Cone_Patch)
{
  const double                aRefRadius = 2.0;
  const double                aSemiAngle = M_PI / 4.0;
  gp_Cone                     aCone(gp::XOY(), aSemiAngle, aRefRadius);
  occ::handle<Geom_ConicalSurface> aConeSurf = new Geom_ConicalSurface(aCone);
  GeomBndLib_Surface          aSurf(aConeSurf);
  EXPECT_EQ(aSurf.GetType(), GeomAbs_Cone);

  Bnd_Box aBox;
  aSurf.Add(0.0, 2.0 * M_PI, 0.0, 5.0, Precision::Confusion(), aBox);

  const auto [aXmin, aXmax, aYmin, aYmax, aZmin, aZmax] = aBox.Get();

  // At V=5, radius = aRefRadius + 5*sin(PI/4)
  const double aMaxR = aRefRadius + 5.0 * std::sin(aSemiAngle);

  EXPECT_GE(aXmax, aMaxR - Precision::Confusion());
  EXPECT_GE(aYmax, aMaxR - Precision::Confusion());
  EXPECT_LE(aXmin, -aMaxR + Precision::Confusion());
  EXPECT_LE(aYmin, -aMaxR + Precision::Confusion());
}

TEST(GeomBndLib_SurfaceTest, Cone_CompareWithBndLib)
{
  gp_Cone                     aCone(gp::XOY(), M_PI / 6.0, 3.0);
  occ::handle<Geom_ConicalSurface> aConeSurf = new Geom_ConicalSurface(aCone);
  GeomAdaptor_Surface         anAdaptor(aConeSurf);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(aConeSurf).Add(0.0, 2.0 * M_PI, 0.0, 5.0, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, 0.0, 2.0 * M_PI, 0.0, 5.0, Precision::Confusion(), anOldBox);

  CompareBoxes(aNewBox, anOldBox, Precision::Confusion());
}

// =========================================================================
// Sphere
// =========================================================================

TEST(GeomBndLib_SurfaceTest, Sphere_Full)
{
  const double                  aRadius = 5.0;
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp::XOY(), aRadius);
  GeomBndLib_Surface            aSurf(aSphere);
  EXPECT_EQ(aSurf.GetType(), GeomAbs_Sphere);

  Bnd_Box aBox;
  aSurf.Add(0.0, aBox);

  const auto [aXmin, aXmax, aYmin, aYmax, aZmin, aZmax] = aBox.Get();

  EXPECT_NEAR(aXmin, -aRadius, Precision::Confusion());
  EXPECT_NEAR(aYmin, -aRadius, Precision::Confusion());
  EXPECT_NEAR(aZmin, -aRadius, Precision::Confusion());
  EXPECT_NEAR(aXmax, aRadius, Precision::Confusion());
  EXPECT_NEAR(aYmax, aRadius, Precision::Confusion());
  EXPECT_NEAR(aZmax, aRadius, Precision::Confusion());
}

TEST(GeomBndLib_SurfaceTest, Sphere_Patch)
{
  const double                  aRadius = 5.0;
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp::XOY(), aRadius);
  GeomBndLib_Surface            aSurf(aSphere);

  Bnd_Box aBox;
  // U in [0, PI/2], V in [0, PI/2] (first octant patch)
  aSurf.Add(0.0, M_PI / 2.0, 0.0, M_PI / 2.0, Precision::Confusion(), aBox);

  const auto [aXmin, aXmax, aYmin, aYmax, aZmin, aZmax] = aBox.Get();

  // Patch should be in the positive octant
  EXPECT_GE(aXmin, -Precision::Confusion());
  EXPECT_GE(aYmin, -Precision::Confusion());
  EXPECT_GE(aZmin, -Precision::Confusion());
  EXPECT_LE(aXmax, aRadius + Precision::Confusion());
  EXPECT_LE(aYmax, aRadius + Precision::Confusion());
  EXPECT_LE(aZmax, aRadius + Precision::Confusion());
}

TEST(GeomBndLib_SurfaceTest, Sphere_CompareWithBndLib)
{
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp::XOY(), 7.0);
  GeomAdaptor_Surface           anAdaptor(aSphere);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(aSphere).Add(Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, Precision::Confusion(), anOldBox);

  CompareBoxes(aNewBox, anOldBox, Precision::Confusion());
}

TEST(GeomBndLib_SurfaceTest, Sphere_Patch_CompareWithBndLib)
{
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp::XOY(), 7.0);
  GeomAdaptor_Surface           anAdaptor(aSphere);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(aSphere)
    .Add(0.0, M_PI, -M_PI / 4.0, M_PI / 4.0, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor,
                         0.0,
                         M_PI,
                         -M_PI / 4.0,
                         M_PI / 4.0,
                         Precision::Confusion(),
                         anOldBox);

  CompareBoxes(aNewBox, anOldBox, Precision::Confusion());
}

// =========================================================================
// Torus
// =========================================================================

TEST(GeomBndLib_SurfaceTest, Torus_Full)
{
  const double                 aMajR  = 10.0;
  const double                 aMinR  = 3.0;
  occ::handle<Geom_ToroidalSurface> aTorus = new Geom_ToroidalSurface(gp::XOY(), aMajR, aMinR);
  GeomBndLib_Surface           aSurf(aTorus);
  EXPECT_EQ(aSurf.GetType(), GeomAbs_Torus);

  Bnd_Box aBox;
  aSurf.Add(0.0, aBox);

  const auto [aXmin, aXmax, aYmin, aYmax, aZmin, aZmax] = aBox.Get();

  const double aOuterR = aMajR + aMinR;
  EXPECT_NEAR(aXmin, -aOuterR, Precision::Confusion());
  EXPECT_NEAR(aYmin, -aOuterR, Precision::Confusion());
  EXPECT_NEAR(aZmin, -aMinR, Precision::Confusion());
  EXPECT_NEAR(aXmax, aOuterR, Precision::Confusion());
  EXPECT_NEAR(aYmax, aOuterR, Precision::Confusion());
  EXPECT_NEAR(aZmax, aMinR, Precision::Confusion());
}

TEST(GeomBndLib_SurfaceTest, Torus_CompareWithBndLib)
{
  occ::handle<Geom_ToroidalSurface> aTorus = new Geom_ToroidalSurface(gp::XOY(), 10.0, 3.0);
  GeomAdaptor_Surface          anAdaptor(aTorus);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(aTorus).Add(Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, Precision::Confusion(), anOldBox);

  // New implementation produces a tighter (better) box than old BndLib for full torus.
  // Verify new box is no larger than old box.
  const auto [aXminN, aXmaxN, aYminN, aYmaxN, aZminN, aZmaxN] = aNewBox.Get();
  const auto [aXminO, aXmaxO, aYminO, aYmaxO, aZminO, aZmaxO] = anOldBox.Get();
  EXPECT_GE(aXminN, aXminO - Precision::Confusion());
  EXPECT_LE(aXmaxN, aXmaxO + Precision::Confusion());
  EXPECT_GE(aYminN, aYminO - Precision::Confusion());
  EXPECT_LE(aYmaxN, aYmaxO + Precision::Confusion());
  EXPECT_GE(aZminN, aZminO - Precision::Confusion());
  EXPECT_LE(aZmaxN, aZmaxO + Precision::Confusion());
}

TEST(GeomBndLib_SurfaceTest, Torus_Patch_CompareWithBndLib)
{
  occ::handle<Geom_ToroidalSurface> aTorus = new Geom_ToroidalSurface(gp::XOY(), 10.0, 3.0);
  GeomAdaptor_Surface          anAdaptor(aTorus);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(aTorus).Add(0.0, M_PI, 0.0, M_PI, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, 0.0, M_PI, 0.0, M_PI, Precision::Confusion(), anOldBox);

  // New implementation produces a tighter (better) box than old BndLib for torus patches.
  // Verify new box is no larger than old box.
  const auto [aXminN, aXmaxN, aYminN, aYmaxN, aZminN, aZmaxN] = aNewBox.Get();
  const auto [aXminO, aXmaxO, aYminO, aYmaxO, aZminO, aZmaxO] = anOldBox.Get();
  EXPECT_GE(aXminN, aXminO - Precision::Confusion());
  EXPECT_LE(aXmaxN, aXmaxO + Precision::Confusion());
  EXPECT_GE(aYminN, aYminO - Precision::Confusion());
  EXPECT_LE(aYmaxN, aYmaxO + Precision::Confusion());
  EXPECT_GE(aZminN, aZminO - Precision::Confusion());
  EXPECT_LE(aZmaxN, aZmaxO + Precision::Confusion());
}

// =========================================================================
// BezierSurface
// =========================================================================

TEST(GeomBndLib_SurfaceTest, BezierSurface_Simple)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 2);
  aPoles.SetValue(1, 1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles.SetValue(1, 2, gp_Pnt(10.0, 0.0, 0.0));
  aPoles.SetValue(2, 1, gp_Pnt(0.0, 10.0, 0.0));
  aPoles.SetValue(2, 2, gp_Pnt(10.0, 10.0, 5.0));

  occ::handle<Geom_BezierSurface> aBezSurf = new Geom_BezierSurface(aPoles);
  GeomBndLib_Surface         aSurf(aBezSurf);
  EXPECT_EQ(aSurf.GetType(), GeomAbs_BezierSurface);

  Bnd_Box aBox;
  aSurf.Add(Precision::Confusion(), aBox);

  const auto [aXmin, aXmax, aYmin, aYmax, aZmin, aZmax] = aBox.Get();

  EXPECT_LE(aXmin, Precision::Confusion());
  EXPECT_GE(aXmax, 10.0 - Precision::Confusion());
  EXPECT_LE(aYmin, Precision::Confusion());
  EXPECT_GE(aYmax, 10.0 - Precision::Confusion());
  EXPECT_LE(aZmin, Precision::Confusion());
  EXPECT_GE(aZmax, 5.0 - Precision::Confusion());
}

TEST(GeomBndLib_SurfaceTest, BezierSurface_CompareWithBndLib)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 2);
  aPoles.SetValue(1, 1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles.SetValue(1, 2, gp_Pnt(10.0, 0.0, 0.0));
  aPoles.SetValue(2, 1, gp_Pnt(0.0, 10.0, 0.0));
  aPoles.SetValue(2, 2, gp_Pnt(10.0, 10.0, 5.0));

  occ::handle<Geom_BezierSurface> aBezSurf = new Geom_BezierSurface(aPoles);
  GeomAdaptor_Surface        anAdaptor(aBezSurf);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(aBezSurf).Add(Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, Precision::Confusion(), anOldBox);

  CompareBoxes(aNewBox, anOldBox, Precision::Confusion());
}

// =========================================================================
// BSplineSurface
// =========================================================================

TEST(GeomBndLib_SurfaceTest, BSplineSurface_CompareWithBndLib)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  aPoles.SetValue(1, 1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles.SetValue(1, 2, gp_Pnt(5.0, 0.0, 2.0));
  aPoles.SetValue(1, 3, gp_Pnt(10.0, 0.0, 0.0));
  aPoles.SetValue(2, 1, gp_Pnt(0.0, 5.0, 1.0));
  aPoles.SetValue(2, 2, gp_Pnt(5.0, 5.0, 5.0));
  aPoles.SetValue(2, 3, gp_Pnt(10.0, 5.0, 1.0));
  aPoles.SetValue(3, 1, gp_Pnt(0.0, 10.0, 0.0));
  aPoles.SetValue(3, 2, gp_Pnt(5.0, 10.0, 2.0));
  aPoles.SetValue(3, 3, gp_Pnt(10.0, 10.0, 0.0));

  NCollection_Array1<double> aUKnots(1, 2);
  aUKnots.SetValue(1, 0.0);
  aUKnots.SetValue(2, 1.0);
  NCollection_Array1<double> aVKnots(1, 2);
  aVKnots.SetValue(1, 0.0);
  aVKnots.SetValue(2, 1.0);
  NCollection_Array1<int> aUMults(1, 2);
  aUMults.SetValue(1, 3);
  aUMults.SetValue(2, 3);
  NCollection_Array1<int> aVMults(1, 2);
  aVMults.SetValue(1, 3);
  aVMults.SetValue(2, 3);

  occ::handle<Geom_BSplineSurface> aBSplSurf =
    new Geom_BSplineSurface(aPoles, aUKnots, aVKnots, aUMults, aVMults, 2, 2);
  GeomAdaptor_Surface anAdaptor(aBSplSurf);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(aBSplSurf).Add(Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, Precision::Confusion(), anOldBox);

  CompareBoxes(aNewBox, anOldBox, Precision::Confusion());
}

// =========================================================================
// Adaptor constructor
// =========================================================================

TEST(GeomBndLib_SurfaceTest, AdaptorConstructor_Sphere)
{
  const double                  aRadius = 5.0;
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp::XOY(), aRadius);

  // Construct from handle
  GeomBndLib_Surface aSurfH(aSphere);
  Bnd_Box            aBoxH;
  aSurfH.Add(Precision::Confusion(), aBoxH);

  // Construct from adaptor
  GeomAdaptor_Surface anAdaptor(aSphere);
  GeomBndLib_Surface  aSurfA(anAdaptor);
  EXPECT_EQ(aSurfA.GetType(), GeomAbs_Sphere);

  Bnd_Box aBoxA;
  aSurfA.Add(Precision::Confusion(), aBoxA);

  CompareBoxes(aBoxH, aBoxA, Precision::Confusion());
}

// =========================================================================
// AddOptimal
// =========================================================================

TEST(GeomBndLib_SurfaceTest, AddOptimal_Sphere)
{
  const double                  aRadius = 5.0;
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp::XOY(), aRadius);
  GeomBndLib_Surface            aSurf(aSphere);

  Bnd_Box aBox;
  aSurf.Add(Precision::Confusion(), aBox);

  Bnd_Box aBoxOpt;
  aSurf.AddOptimal(Precision::Confusion(), aBoxOpt);

  const auto [aXmin, aXmax, aYmin, aYmax, aZmin, aZmax]       = aBox.Get();
  const auto [aXminO, aXmaxO, aYminO, aYmaxO, aZminO, aZmaxO] = aBoxOpt.Get();

  // AddOptimal should produce a box no larger than Add
  EXPECT_GE(aXminO, aXmin - Precision::Confusion());
  EXPECT_GE(aYminO, aYmin - Precision::Confusion());
  EXPECT_GE(aZminO, aZmin - Precision::Confusion());
  EXPECT_LE(aXmaxO, aXmax + Precision::Confusion());
  EXPECT_LE(aYmaxO, aYmax + Precision::Confusion());
  EXPECT_LE(aZmaxO, aZmax + Precision::Confusion());
}

TEST(GeomBndLib_SurfaceTest, AddOptimal_CompareWithBndLib)
{
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp::XOY(), 7.0);
  GeomAdaptor_Surface           anAdaptor(aSphere);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(aSphere).AddOptimal(Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::AddOptimal(anAdaptor, Precision::Confusion(), anOldBox);

  CompareBoxes(aNewBox, anOldBox, Precision::Confusion());
}

TEST(GeomBndLib_SurfaceTest, AddOptimal_Torus_TiltedBetterThanFast)
{
  // Tilted torus: axis is (1,0,1)/sqrt(2), Ra=8, Rmi=3.
  // For a tilted torus the PI/4 circle-sampling used by Box() misses the true
  // extremum, so BoxOptimal (PSO) must give a tighter result.
  // Analytic: for axis n=(1/sqrt(2), 0, 1/sqrt(2)),
  //   Xmax = Zmax = Ra/sqrt(2) + Rmi = 4*sqrt(2)+3 ~= 8.657
  //   Ymax = Ra + Rmi = 11  (ny=0, so full equatorial radius applies)
  gp_Ax3                       aTiltedAx(gp::Origin(), gp_Dir(1.0, 0.0, 1.0));
  occ::handle<Geom_ToroidalSurface> aTorus = new Geom_ToroidalSurface(aTiltedAx, 8.0, 3.0);

  Bnd_Box aFastBox;
  GeomBndLib_Surface(aTorus).Add(Precision::Confusion(), aFastBox);

  Bnd_Box aOptBox;
  GeomBndLib_Surface(aTorus).AddOptimal(Precision::Confusion(), aOptBox);

  const auto [aXminF, aXmaxF, aYminF, aYmaxF, aZminF, aZmaxF] = aFastBox.Get();
  const auto [aXminO, aXmaxO, aYminO, aYmaxO, aZminO, aZmaxO] = aOptBox.Get();

  // Optimal must be no larger than the fast (conservative) box.
  EXPECT_GE(aXminO, aXminF - Precision::Confusion()) << "OptXmin outside FastBox";
  EXPECT_LE(aXmaxO, aXmaxF + Precision::Confusion()) << "OptXmax outside FastBox";
  EXPECT_GE(aYminO, aYminF - Precision::Confusion()) << "OptYmin outside FastBox";
  EXPECT_LE(aYmaxO, aYmaxF + Precision::Confusion()) << "OptYmax outside FastBox";
  EXPECT_GE(aZminO, aZminF - Precision::Confusion()) << "OptZmin outside FastBox";
  EXPECT_LE(aZmaxO, aZmaxF + Precision::Confusion()) << "OptZmax outside FastBox";

  // PSO must converge to the analytic tight bound.
  const double aTrueXZ = 8.0 / std::sqrt(2.0) + 3.0;    // ~= 8.6569
  const double aTrueY  = 8.0 + 3.0;                     // = 11.0
  const double aTol    = 10.0 * Precision::Confusion(); // PSO converges to ~Confusion()
  EXPECT_NEAR(aXmaxO, aTrueXZ, aTol) << "Xmax";
  EXPECT_NEAR(-aXminO, aTrueXZ, aTol) << "Xmin";
  EXPECT_NEAR(aYmaxO, aTrueY, aTol) << "Ymax";
  EXPECT_NEAR(-aYminO, aTrueY, aTol) << "Ymin";
  EXPECT_NEAR(aZmaxO, aTrueXZ, aTol) << "Zmax";
  EXPECT_NEAR(-aZminO, aTrueXZ, aTol) << "Zmin";
}

TEST(GeomBndLib_SurfaceTest, AdaptorConstructor_TrimmedRangeUsedForFullAdd)
{
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp::XOY(), 7.0);
  GeomAdaptor_Surface           anAdaptor(aSphere, 0.0, M_PI, -M_PI / 4.0, M_PI / 4.0);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(anAdaptor).Add(Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, Precision::Confusion(), anOldBox);

  CompareBoxes(aNewBox, anOldBox, Precision::Confusion());
}

TEST(GeomBndLib_SurfaceTest, TrimmedSurfaceHandle_Fallback_CompareWithBndLib)
{
  occ::handle<Geom_SphericalSurface>          aSphere = new Geom_SphericalSurface(gp::XOY(), 7.0);
  occ::handle<Geom_RectangularTrimmedSurface> aTrim =
    new Geom_RectangularTrimmedSurface(aSphere, 0.0, M_PI, -M_PI / 4.0, M_PI / 4.0);
  GeomAdaptor_Surface anAdaptor(aTrim);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(aTrim).Add(Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, Precision::Confusion(), anOldBox);

  EXPECT_FALSE(aNewBox.IsVoid());
  CompareBoxes(aNewBox, anOldBox, Precision::Confusion());
}

TEST(GeomBndLib_SurfaceTest, RectangularTrimmedSurface_UsesBasisSpecialization)
{
  occ::handle<Geom_SphericalSurface>          aSphere = new Geom_SphericalSurface(gp::XOY(), 7.0);
  occ::handle<Geom_RectangularTrimmedSurface> aTrim =
    new Geom_RectangularTrimmedSurface(aSphere, 0.0, M_PI, -M_PI / 4.0, M_PI / 4.0);

  GeomBndLib_Surface aSurf(aTrim);
  EXPECT_EQ(aSurf.GetType(), GeomAbs_Sphere);

  GeomAdaptor_Surface anAdaptor(aTrim);
  Bnd_Box             aNewBox;
  aSurf.Add(Precision::Confusion(), aNewBox);
  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, Precision::Confusion(), anOldBox);
  CompareBoxes(aNewBox, anOldBox, Precision::Confusion());
}

TEST(GeomBndLib_SurfaceTest, Torus_NegativeVMin_NoUnderBounding)
{
  occ::handle<Geom_ToroidalSurface> aTorus = new Geom_ToroidalSurface(gp::XOY(), 10.0, 3.0);
  const double                 aU1    = 0.0;
  const double                 aU2    = 2.0 * M_PI;
  const double                 aV1    = -M_PI / 8.0;
  const double                 aV2    = M_PI / 8.0;

  Bnd_Box aNewBox;
  GeomBndLib_Surface(aTorus).Add(aU1, aU2, aV1, aV2, Precision::Confusion(), aNewBox);

  const auto [aXmin, aXmax, aYmin, aYmax, aZmin, aZmax] = aNewBox.Get();
  for (int i = 0; i <= 72; ++i)
  {
    const double aU = aU1 + (aU2 - aU1) * static_cast<double>(i) / 72.0;
    for (int j = 0; j <= 16; ++j)
    {
      const double aV = aV1 + (aV2 - aV1) * static_cast<double>(j) / 16.0;
      const gp_Pnt aP = aTorus->Value(aU, aV);
      EXPECT_GE(aP.X(), aXmin - Precision::Confusion());
      EXPECT_LE(aP.X(), aXmax + Precision::Confusion());
      EXPECT_GE(aP.Y(), aYmin - Precision::Confusion());
      EXPECT_LE(aP.Y(), aYmax + Precision::Confusion());
      EXPECT_GE(aP.Z(), aZmin - Precision::Confusion());
      EXPECT_LE(aP.Z(), aZmax + Precision::Confusion());
    }
  }
}
