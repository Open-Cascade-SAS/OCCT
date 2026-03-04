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
#include <Geom_CylindricalSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Precision.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

namespace
{

//! Helper: verify new box is no larger than old box (tighter or equal).
void ExpectNoLarger(const Bnd_Box& theNew, const Bnd_Box& theOld, const double theTol)
{
  const auto [aXminN, aXmaxN, aYminN, aYmaxN, aZminN, aZmaxN] = theNew.Get();
  const auto [aXminO, aXmaxO, aYminO, aYmaxO, aZminO, aZmaxO] = theOld.Get();
  EXPECT_GE(aXminN, aXminO - theTol) << "New Xmin is smaller than old";
  EXPECT_LE(aXmaxN, aXmaxO + theTol) << "New Xmax is larger than old";
  EXPECT_GE(aYminN, aYminO - theTol) << "New Ymin is smaller than old";
  EXPECT_LE(aYmaxN, aYmaxO + theTol) << "New Ymax is larger than old";
  EXPECT_GE(aZminN, aZminO - theTol) << "New Zmin is smaller than old";
  EXPECT_LE(aZmaxN, aZmaxO + theTol) << "New Zmax is larger than old";
}

//! Helper: verify box contains all sampled points of the offset surface.
void ExpectContainsOffsetSurface(const Bnd_Box&                    theBox,
                                 const Handle(Geom_OffsetSurface)& theSurf,
                                 const double                      theUMin,
                                 const double                      theUMax,
                                 const double                      theVMin,
                                 const double                      theVMax,
                                 const int                         theNbU,
                                 const int                         theNbV)
{
  double aXmin = 0., aYmin = 0., aZmin = 0., aXmax = 0., aYmax = 0., aZmax = 0.;
  theBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
  const double aUStep = (theUMax - theUMin) / theNbU;
  const double aVStep = (theVMax - theVMin) / theNbV;
  for (int i = 0; i <= theNbU; ++i)
  {
    const double aU = theUMin + i * aUStep;
    for (int j = 0; j <= theNbV; ++j)
    {
      const double aV   = theVMin + j * aVStep;
      const gp_Pnt aPnt = theSurf->Value(aU, aV);
      EXPECT_GE(aPnt.X(), aXmin - Precision::Confusion())
        << "Point outside box at U=" << aU << " V=" << aV;
      EXPECT_LE(aPnt.X(), aXmax + Precision::Confusion())
        << "Point outside box at U=" << aU << " V=" << aV;
      EXPECT_GE(aPnt.Y(), aYmin - Precision::Confusion())
        << "Point outside box at U=" << aU << " V=" << aV;
      EXPECT_LE(aPnt.Y(), aYmax + Precision::Confusion())
        << "Point outside box at U=" << aU << " V=" << aV;
      EXPECT_GE(aPnt.Z(), aZmin - Precision::Confusion())
        << "Point outside box at U=" << aU << " V=" << aV;
      EXPECT_LE(aPnt.Z(), aZmax + Precision::Confusion())
        << "Point outside box at U=" << aU << " V=" << aV;
    }
  }
}

} // namespace

// =========================================================================
// Offset sphere (positive)
// =========================================================================

TEST(GeomBndLib_OffsetSurfaceTest, Sphere_PositiveOffset_Full_CompareWithBndLib)
{
  Handle(Geom_SphericalSurface) aSphere = new Geom_SphericalSurface(gp::XOY(), 5.0);
  Handle(Geom_OffsetSurface)    anOff   = new Geom_OffsetSurface(aSphere, 2.0);
  GeomAdaptor_Surface           anAdaptor(anOff);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(anOff).Add(Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsOffsetSurface(aNewBox, anOff, 0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0, 36, 18);
}

TEST(GeomBndLib_OffsetSurfaceTest, Sphere_PositiveOffset_Patch_CompareWithBndLib)
{
  Handle(Geom_SphericalSurface) aSphere = new Geom_SphericalSurface(gp::XOY(), 5.0);
  Handle(Geom_OffsetSurface)    anOff   = new Geom_OffsetSurface(aSphere, 2.0);
  GeomAdaptor_Surface           anAdaptor(anOff);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(anOff)
    .Add(0.0, M_PI, -M_PI / 4.0, M_PI / 4.0, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor,
                         0.0,
                         M_PI,
                         -M_PI / 4.0,
                         M_PI / 4.0,
                         Precision::Confusion(),
                         anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsOffsetSurface(aNewBox, anOff, 0.0, M_PI, -M_PI / 4.0, M_PI / 4.0, 18, 9);
}

// =========================================================================
// Offset sphere (negative)
// =========================================================================

TEST(GeomBndLib_OffsetSurfaceTest, Sphere_NegativeOffset_Full_CompareWithBndLib)
{
  Handle(Geom_SphericalSurface) aSphere = new Geom_SphericalSurface(gp::XOY(), 10.0);
  Handle(Geom_OffsetSurface)    anOff   = new Geom_OffsetSurface(aSphere, -3.0);
  GeomAdaptor_Surface           anAdaptor(anOff);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(anOff).Add(Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsOffsetSurface(aNewBox, anOff, 0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0, 36, 18);
}

// =========================================================================
// Offset cylinder
// =========================================================================

TEST(GeomBndLib_OffsetSurfaceTest, Cylinder_Full_CompareWithBndLib)
{
  Handle(Geom_CylindricalSurface) aCyl  = new Geom_CylindricalSurface(gp::XOY(), 4.0);
  Handle(Geom_OffsetSurface)      anOff = new Geom_OffsetSurface(aCyl, 1.5);
  GeomAdaptor_Surface             anAdaptor(anOff);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(anOff).Add(0.0, 2.0 * M_PI, -5.0, 5.0, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, 0.0, 2.0 * M_PI, -5.0, 5.0, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsOffsetSurface(aNewBox, anOff, 0.0, 2.0 * M_PI, -5.0, 5.0, 36, 10);
}

TEST(GeomBndLib_OffsetSurfaceTest, Cylinder_Patch_CompareWithBndLib)
{
  Handle(Geom_CylindricalSurface) aCyl  = new Geom_CylindricalSurface(gp::XOY(), 4.0);
  Handle(Geom_OffsetSurface)      anOff = new Geom_OffsetSurface(aCyl, 1.5);
  GeomAdaptor_Surface             anAdaptor(anOff);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(anOff).Add(0.0, M_PI, 0.0, 5.0, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, 0.0, M_PI, 0.0, 5.0, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsOffsetSurface(aNewBox, anOff, 0.0, M_PI, 0.0, 5.0, 18, 5);
}

TEST(GeomBndLib_OffsetSurfaceTest, Cylinder_NegativeOffset_CompareWithBndLib)
{
  Handle(Geom_CylindricalSurface) aCyl  = new Geom_CylindricalSurface(gp::XOY(), 8.0);
  Handle(Geom_OffsetSurface)      anOff = new Geom_OffsetSurface(aCyl, -2.0);
  GeomAdaptor_Surface             anAdaptor(anOff);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(anOff).Add(0.0, 2.0 * M_PI, 0.0, 10.0, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, 0.0, 2.0 * M_PI, 0.0, 10.0, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsOffsetSurface(aNewBox, anOff, 0.0, 2.0 * M_PI, 0.0, 10.0, 36, 10);
}

// =========================================================================
// Offset plane
// =========================================================================

TEST(GeomBndLib_OffsetSurfaceTest, Plane_PositiveOffset_CompareWithBndLib)
{
  Handle(Geom_Plane)         aPlane = new Geom_Plane(gp::XOY());
  Handle(Geom_OffsetSurface) anOff  = new Geom_OffsetSurface(aPlane, 3.0);
  GeomAdaptor_Surface        anAdaptor(anOff);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(anOff).Add(-5.0, 5.0, -5.0, 5.0, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, -5.0, 5.0, -5.0, 5.0, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsOffsetSurface(aNewBox, anOff, -5.0, 5.0, -5.0, 5.0, 10, 10);
}

TEST(GeomBndLib_OffsetSurfaceTest, Plane_PositiveOffset_Patch_IsNotArtificiallyThickenedInZ)
{
  Handle(Geom_Plane)         aPlane = new Geom_Plane(gp::XOY());
  Handle(Geom_OffsetSurface) anOff  = new Geom_OffsetSurface(aPlane, 3.0);

  Bnd_Box aBox;
  GeomBndLib_Surface(anOff).Add(-4.0, 6.0, -2.0, 7.0, Precision::Confusion(), aBox);

  const auto [aXmin, aXmax, aYmin, aYmax, aZmin, aZmax] = aBox.Get();
  (void)aXmin;
  (void)aXmax;
  (void)aYmin;
  (void)aYmax;

  const double aTol = Precision::Confusion();
  EXPECT_NEAR(aZmin, 3.0 - aTol, 1e-12);
  EXPECT_NEAR(aZmax, 3.0 + aTol, 1e-12);
}

TEST(GeomBndLib_OffsetSurfaceTest, Plane_NegativeOffset_CompareWithBndLib)
{
  Handle(Geom_Plane)         aPlane = new Geom_Plane(gp::XOY());
  Handle(Geom_OffsetSurface) anOff  = new Geom_OffsetSurface(aPlane, -5.0);
  GeomAdaptor_Surface        anAdaptor(anOff);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(anOff).Add(-10.0, 10.0, -10.0, 10.0, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, -10.0, 10.0, -10.0, 10.0, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsOffsetSurface(aNewBox, anOff, -10.0, 10.0, -10.0, 10.0, 10, 10);
}

// =========================================================================
// Offset torus
// =========================================================================

TEST(GeomBndLib_OffsetSurfaceTest, Torus_PositiveOffset_CompareWithBndLib)
{
  Handle(Geom_ToroidalSurface) aTorus = new Geom_ToroidalSurface(gp::XOY(), 10.0, 3.0);
  Handle(Geom_OffsetSurface)   anOff  = new Geom_OffsetSurface(aTorus, 1.0);
  GeomAdaptor_Surface          anAdaptor(anOff);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(anOff).Add(0.0, 2.0 * M_PI, 0.0, 2.0 * M_PI, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor,
                         0.0,
                         2.0 * M_PI,
                         0.0,
                         2.0 * M_PI,
                         Precision::Confusion(),
                         anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsOffsetSurface(aNewBox, anOff, 0.0, 2.0 * M_PI, 0.0, 2.0 * M_PI, 36, 36);
}

TEST(GeomBndLib_OffsetSurfaceTest, Torus_Patch_CompareWithBndLib)
{
  Handle(Geom_ToroidalSurface) aTorus = new Geom_ToroidalSurface(gp::XOY(), 10.0, 3.0);
  Handle(Geom_OffsetSurface)   anOff  = new Geom_OffsetSurface(aTorus, 1.0);
  GeomAdaptor_Surface          anAdaptor(anOff);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(anOff).Add(0.0, M_PI, 0.0, M_PI, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, 0.0, M_PI, 0.0, M_PI, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsOffsetSurface(aNewBox, anOff, 0.0, M_PI, 0.0, M_PI, 18, 18);
}

// =========================================================================
// Large offset
// =========================================================================

TEST(GeomBndLib_OffsetSurfaceTest, Sphere_LargeOffset_CompareWithBndLib)
{
  Handle(Geom_SphericalSurface) aSphere = new Geom_SphericalSurface(gp::XOY(), 2.0);
  Handle(Geom_OffsetSurface)    anOff   = new Geom_OffsetSurface(aSphere, 20.0);
  GeomAdaptor_Surface           anAdaptor(anOff);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(anOff).Add(Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsOffsetSurface(aNewBox, anOff, 0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0, 36, 18);
}
