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

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepTools.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <GeomFill_Gordon.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Precision.hxx>
#include <StdFail_NotDone.hxx>
#include <TopoDS_Compound.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

#include <cmath>

namespace
{

// ============================================================================
// Interpolation helpers
// ============================================================================

//! Interpolate a BSpline curve through given points, uniformly parametrized on [0,1].
occ::handle<Geom_BSplineCurve> interpPoints(const NCollection_Array1<gp_Pnt>& thePts)
{
  const int                                  aNbPts = thePts.Length();
  StdFail_NotDone_Raise_if(aNbPts < 2,
                           "GeomFill_GordonDemo_Test::interpPoints() requires at least two points");
  occ::handle<NCollection_HArray1<gp_Pnt>>  aPnts  = new NCollection_HArray1<gp_Pnt>(1, aNbPts);
  occ::handle<NCollection_HArray1<double>>   aParams = new NCollection_HArray1<double>(1, aNbPts);
  for (int i = 0; i < aNbPts; ++i)
  {
    aPnts->SetValue(i + 1, thePts(thePts.Lower() + i));
    aParams->SetValue(i + 1, static_cast<double>(i) / (aNbPts - 1));
  }
  GeomAPI_Interpolate anInterp(aPnts, aParams, false, Precision::Confusion());
  anInterp.Perform();
  StdFail_NotDone_Raise_if(!anInterp.IsDone(),
                           "GeomFill_GordonDemo_Test::interpPoints() interpolation failed");
  return anInterp.Curve();
}

//! Build a profile curve {x, theY, Z(x, theY)} for x in [0, 1] from a Z-function.
template <typename ZFunc>
occ::handle<Geom_BSplineCurve> buildProfile(const ZFunc& theZFunc,
                                             const double theY,
                                             const int    theNbPts = 21)
{
  StdFail_NotDone_Raise_if(theNbPts < 2,
                           "GeomFill_GordonDemo_Test::buildProfile() requires at least two points");
  NCollection_Array1<gp_Pnt> aPts(1, theNbPts);
  for (int k = 1; k <= theNbPts; ++k)
  {
    const double aX = static_cast<double>(k - 1) / (theNbPts - 1);
    aPts(k) = gp_Pnt(aX, theY, theZFunc(aX, theY));
  }
  return interpPoints(aPts);
}

//! Build a guide curve {theX, y, Z(theX, y)} for y in [0, 1] from a Z-function.
template <typename ZFunc>
occ::handle<Geom_BSplineCurve> buildGuide(const ZFunc& theZFunc,
                                           const double theX,
                                           const int    theNbPts = 21)
{
  StdFail_NotDone_Raise_if(theNbPts < 2,
                           "GeomFill_GordonDemo_Test::buildGuide() requires at least two points");
  NCollection_Array1<gp_Pnt> aPts(1, theNbPts);
  for (int k = 1; k <= theNbPts; ++k)
  {
    const double aY = static_cast<double>(k - 1) / (theNbPts - 1);
    aPts(k) = gp_Pnt(theX, aY, theZFunc(theX, aY));
  }
  return interpPoints(aPts);
}

// ============================================================================
// BRep output helpers
// ============================================================================

//! Save profile and guide curves as a compound of edges to a BRep file.
void saveBasisToBRep(const NCollection_Array1<occ::handle<Geom_Curve>>& theProfiles,
                     const NCollection_Array1<occ::handle<Geom_Curve>>& theGuides,
                     const char*                                        theFileName)
{
  BRep_Builder    aBld;
  TopoDS_Compound aCompound;
  aBld.MakeCompound(aCompound);
  for (int i = theProfiles.Lower(); i <= theProfiles.Upper(); ++i)
  {
    BRepBuilderAPI_MakeEdge aME(theProfiles(i));
    if (aME.IsDone())
    {
      aBld.Add(aCompound, aME.Shape());
    }
  }
  for (int i = theGuides.Lower(); i <= theGuides.Upper(); ++i)
  {
    BRepBuilderAPI_MakeEdge aME(theGuides(i));
    if (aME.IsDone())
    {
      aBld.Add(aCompound, aME.Shape());
    }
  }
  BRepTools::Write(aCompound, theFileName);
}

//! Save a BSpline surface as a face to a BRep file.
void saveSurfaceToBRep(const occ::handle<Geom_BSplineSurface>& theSurf,
                       const char*                              theFileName)
{
  BRepBuilderAPI_MakeFace aMF(theSurf, Precision::Confusion());
  if (aMF.IsDone())
  {
    BRepTools::Write(aMF.Face(), theFileName);
  }
}

} // namespace

// ============================================================================
// Demo 1: Barrel Vault - arch-shaped profiles with height varying along span.
// Z(x,y) = (0.4 + 0.2*sin(pi*y)) * sin(pi*x)
// Arch profiles create a tunnel; mid-span arch is tallest.
// ============================================================================

TEST(GeomFill_GordonDemo, BarrelVault)
{
  auto aZFunc = [](const double theX, const double theY) -> double {
    return (0.4 + 0.2 * std::sin(M_PI * theY)) * std::sin(M_PI * theX);
  };

  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 5);
  aProfiles(1) = buildProfile(aZFunc, 0.0);
  aProfiles(2) = buildProfile(aZFunc, 0.25);
  aProfiles(3) = buildProfile(aZFunc, 0.5);
  aProfiles(4) = buildProfile(aZFunc, 0.75);
  aProfiles(5) = buildProfile(aZFunc, 1.0);

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 5);
  aGuides(1) = buildGuide(aZFunc, 0.0);
  aGuides(2) = buildGuide(aZFunc, 0.25);
  aGuides(3) = buildGuide(aZFunc, 0.5);
  aGuides(4) = buildGuide(aZFunc, 0.75);
  aGuides(5) = buildGuide(aZFunc, 1.0);

  saveBasisToBRep(aProfiles, aGuides, "GordonDemo_BarrelVault_basis.brep");

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, 0.01);
  aGordon.Perform();
  ASSERT_TRUE(aGordon.IsDone());

  const occ::handle<Geom_BSplineSurface>& aSurf = aGordon.Surface();
  ASSERT_FALSE(aSurf.IsNull());
  saveSurfaceToBRep(aSurf, "GordonDemo_BarrelVault_surface.brep");

  const double aUMid = 0.5 * (aSurf->UKnot(1) + aSurf->UKnot(aSurf->NbUKnots()));
  const double aVMid = 0.5 * (aSurf->VKnot(1) + aSurf->VKnot(aSurf->NbVKnots()));
  const gp_Pnt aMidPt = aSurf->Value(aUMid, aVMid);
  EXPECT_GT(aMidPt.Z(), 0.3) << "Barrel vault should arch upward at center, Z=" << aMidPt.Z();
}

// ============================================================================
// Demo 2: Saddle - profiles arch up, guides arch down. Classic saddle point.
// Z(x,y) = 0.5*sin(pi*x) - 0.5*sin(pi*y)
// Creates a hyperbolic-paraboloid-like surface.
// ============================================================================

TEST(GeomFill_GordonDemo, Saddle)
{
  auto aZFunc = [](const double theX, const double theY) -> double {
    return 0.5 * std::sin(M_PI * theX) - 0.5 * std::sin(M_PI * theY);
  };

  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 3);
  aProfiles(1) = buildProfile(aZFunc, 0.0);
  aProfiles(2) = buildProfile(aZFunc, 0.5);
  aProfiles(3) = buildProfile(aZFunc, 1.0);

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 3);
  aGuides(1) = buildGuide(aZFunc, 0.0);
  aGuides(2) = buildGuide(aZFunc, 0.5);
  aGuides(3) = buildGuide(aZFunc, 1.0);

  saveBasisToBRep(aProfiles, aGuides, "GordonDemo_Saddle_basis.brep");

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, 0.01);
  aGordon.Perform();
  ASSERT_TRUE(aGordon.IsDone());

  const occ::handle<Geom_BSplineSurface>& aSurf = aGordon.Surface();
  ASSERT_FALSE(aSurf.IsNull());
  saveSurfaceToBRep(aSurf, "GordonDemo_Saddle_surface.brep");

  // Saddle center Z = sin(pi*0.5) - sin(pi*0.5) = 0.
  const double aUMid = 0.5 * (aSurf->UKnot(1) + aSurf->UKnot(aSurf->NbUKnots()));
  const double aVMid = 0.5 * (aSurf->VKnot(1) + aSurf->VKnot(aSurf->NbVKnots()));
  const gp_Pnt aMidPt = aSurf->Value(aUMid, aVMid);
  EXPECT_NEAR(aMidPt.Z(), 0.0, 0.15) << "Saddle center should be near Z=0";
}

// ============================================================================
// Demo 3: Gaussian Peak - bell-shaped dome with smooth falloff in all directions.
// Z(x,y) = 0.8 * exp(-8 * ((x-0.5)^2 + (y-0.5)^2))
// Peak at center, profiles and guides are all bell curves.
// ============================================================================

TEST(GeomFill_GordonDemo, GaussianPeak)
{
  auto aZFunc = [](const double theX, const double theY) -> double {
    const double aDx = theX - 0.5;
    const double aDy = theY - 0.5;
    return 0.8 * std::exp(-8.0 * (aDx * aDx + aDy * aDy));
  };

  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 5);
  aProfiles(1) = buildProfile(aZFunc, 0.0);
  aProfiles(2) = buildProfile(aZFunc, 0.25);
  aProfiles(3) = buildProfile(aZFunc, 0.5);
  aProfiles(4) = buildProfile(aZFunc, 0.75);
  aProfiles(5) = buildProfile(aZFunc, 1.0);

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 5);
  aGuides(1) = buildGuide(aZFunc, 0.0);
  aGuides(2) = buildGuide(aZFunc, 0.25);
  aGuides(3) = buildGuide(aZFunc, 0.5);
  aGuides(4) = buildGuide(aZFunc, 0.75);
  aGuides(5) = buildGuide(aZFunc, 1.0);

  saveBasisToBRep(aProfiles, aGuides, "GordonDemo_GaussianPeak_basis.brep");

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, 0.01);
  aGordon.Perform();
  ASSERT_TRUE(aGordon.IsDone());

  const occ::handle<Geom_BSplineSurface>& aSurf = aGordon.Surface();
  ASSERT_FALSE(aSurf.IsNull());
  saveSurfaceToBRep(aSurf, "GordonDemo_GaussianPeak_surface.brep");

  // The peak at the center should be close to 0.8.
  const double aUMid = 0.5 * (aSurf->UKnot(1) + aSurf->UKnot(aSurf->NbUKnots()));
  const double aVMid = 0.5 * (aSurf->VKnot(1) + aSurf->VKnot(aSurf->NbVKnots()));
  const gp_Pnt aMidPt = aSurf->Value(aUMid, aVMid);
  EXPECT_GT(aMidPt.Z(), 0.4) << "Gaussian peak should be significant at center, Z=" << aMidPt.Z();
}

// ============================================================================
// Demo 4: Traveling Wave - a cosine wave propagating diagonally across the surface.
// Z(x,y) = 0.25 * cos(2*pi*x - pi*y)
// Profiles are phase-shifted cosines; guides are smooth arches.
// ============================================================================

TEST(GeomFill_GordonDemo, TravelingWave)
{
  auto aZFunc = [](const double theX, const double theY) -> double {
    return 0.25 * std::cos(2.0 * M_PI * theX - M_PI * theY);
  };

  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 5);
  aProfiles(1) = buildProfile(aZFunc, 0.0);
  aProfiles(2) = buildProfile(aZFunc, 0.25);
  aProfiles(3) = buildProfile(aZFunc, 0.5);
  aProfiles(4) = buildProfile(aZFunc, 0.75);
  aProfiles(5) = buildProfile(aZFunc, 1.0);

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 5);
  aGuides(1) = buildGuide(aZFunc, 0.0);
  aGuides(2) = buildGuide(aZFunc, 0.25);
  aGuides(3) = buildGuide(aZFunc, 0.5);
  aGuides(4) = buildGuide(aZFunc, 0.75);
  aGuides(5) = buildGuide(aZFunc, 1.0);

  saveBasisToBRep(aProfiles, aGuides, "GordonDemo_TravelingWave_basis.brep");

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, 0.01);
  aGordon.Perform();
  ASSERT_TRUE(aGordon.IsDone());

  const occ::handle<Geom_BSplineSurface>& aSurf = aGordon.Surface();
  ASSERT_FALSE(aSurf.IsNull());
  saveSurfaceToBRep(aSurf, "GordonDemo_TravelingWave_surface.brep");

  // The surface should have both positive and negative Z regions.
  const double aUMin = aSurf->UKnot(1);
  const double aUMax = aSurf->UKnot(aSurf->NbUKnots());
  const double aVMin = aSurf->VKnot(1);
  const double aVMax = aSurf->VKnot(aSurf->NbVKnots());
  bool         aHasPositive = false;
  bool         aHasNegative = false;
  for (int i = 0; i <= 10; ++i)
  {
    for (int j = 0; j <= 10; ++j)
    {
      const double aU  = aUMin + i * (aUMax - aUMin) / 10.0;
      const double aV  = aVMin + j * (aVMax - aVMin) / 10.0;
      const gp_Pnt aPt = aSurf->Value(aU, aV);
      if (aPt.Z() > 0.05)
      {
        aHasPositive = true;
      }
      if (aPt.Z() < -0.05)
      {
        aHasNegative = true;
      }
    }
  }
  EXPECT_TRUE(aHasPositive) << "Traveling wave should have positive Z regions";
  EXPECT_TRUE(aHasNegative) << "Traveling wave should have negative Z regions";
}

// ============================================================================
// Demo 5: Helicoid - a ruled helical surface with true 3D space curves.
// Profiles are tilted radial lines; guides are helical arcs.
// S(u,v) = {v*cos(u), v*sin(u), 0.3*u} for u in [0, pi], v in [0.3, 1.0]
// ============================================================================

TEST(GeomFill_GordonDemo, Helicoid)
{
  constexpr double THE_PITCH = 0.3;
  constexpr double THE_V_MIN = 0.3;
  constexpr double THE_V_MAX = 1.0;
  constexpr double THE_U_MAX = M_PI;

  // 4 profiles at equally spaced u positions (radial lines at different angles).
  const double aUPos[] = {0.0, M_PI / 3.0, 2.0 * M_PI / 3.0, M_PI};

  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 4);
  for (int i = 0; i < 4; ++i)
  {
    const double aU = aUPos[i];
    NCollection_Array1<gp_Pnt> aPts(1, 11);
    for (int k = 1; k <= 11; ++k)
    {
      const double aT = static_cast<double>(k - 1) / 10.0;
      const double aV = THE_V_MIN + aT * (THE_V_MAX - THE_V_MIN);
      aPts(k) = gp_Pnt(aV * std::cos(aU), aV * std::sin(aU), THE_PITCH * aU);
    }
    aProfiles(i + 1) = interpPoints(aPts);
  }

  // 4 guides at equally spaced v positions (helical arcs).
  const double aVPos[] = {0.3, 0.533, 0.767, 1.0};

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 4);
  for (int j = 0; j < 4; ++j)
  {
    const double aV = aVPos[j];
    NCollection_Array1<gp_Pnt> aPts(1, 21);
    for (int k = 1; k <= 21; ++k)
    {
      const double aS = static_cast<double>(k - 1) / 20.0;
      const double aU = aS * THE_U_MAX;
      aPts(k) = gp_Pnt(aV * std::cos(aU), aV * std::sin(aU), THE_PITCH * aU);
    }
    aGuides(j + 1) = interpPoints(aPts);
  }

  saveBasisToBRep(aProfiles, aGuides, "GordonDemo_Helicoid_basis.brep");

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, 0.01);
  aGordon.Perform();
  ASSERT_TRUE(aGordon.IsDone());

  const occ::handle<Geom_BSplineSurface>& aSurf = aGordon.Surface();
  ASSERT_FALSE(aSurf.IsNull());
  saveSurfaceToBRep(aSurf, "GordonDemo_Helicoid_surface.brep");

  // The helicoid Z range should span from 0 to THE_PITCH * pi ≈ 0.94.
  const double aUMid = 0.5 * (aSurf->UKnot(1) + aSurf->UKnot(aSurf->NbUKnots()));
  const double aVMid = 0.5 * (aSurf->VKnot(1) + aSurf->VKnot(aSurf->NbVKnots()));
  const gp_Pnt aMidPt = aSurf->Value(aUMid, aVMid);
  EXPECT_GT(aMidPt.Z(), 0.0) << "Helicoid should have positive Z at center";
}

// ============================================================================
// Demo 6: Monkey Saddle - a surface with 3-fold symmetry and three alternating
// hills/valleys around the center.
// Z(x,y) = ((x-0.5)^3 - 3*(x-0.5)*(y-0.5)^2) * 4
// Each profile and guide has a distinct S-shaped or M-shaped curve.
// ============================================================================

TEST(GeomFill_GordonDemo, MonkeySaddle)
{
  auto aZFunc = [](const double theX, const double theY) -> double {
    const double aDx = theX - 0.5;
    const double aDy = theY - 0.5;
    return 4.0 * (aDx * aDx * aDx - 3.0 * aDx * aDy * aDy);
  };

  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 5);
  aProfiles(1) = buildProfile(aZFunc, 0.0);
  aProfiles(2) = buildProfile(aZFunc, 0.25);
  aProfiles(3) = buildProfile(aZFunc, 0.5);
  aProfiles(4) = buildProfile(aZFunc, 0.75);
  aProfiles(5) = buildProfile(aZFunc, 1.0);

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 5);
  aGuides(1) = buildGuide(aZFunc, 0.0);
  aGuides(2) = buildGuide(aZFunc, 0.25);
  aGuides(3) = buildGuide(aZFunc, 0.5);
  aGuides(4) = buildGuide(aZFunc, 0.75);
  aGuides(5) = buildGuide(aZFunc, 1.0);

  saveBasisToBRep(aProfiles, aGuides, "GordonDemo_MonkeySaddle_basis.brep");

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, 0.01);
  aGordon.Perform();
  ASSERT_TRUE(aGordon.IsDone());

  const occ::handle<Geom_BSplineSurface>& aSurf = aGordon.Surface();
  ASSERT_FALSE(aSurf.IsNull());
  saveSurfaceToBRep(aSurf, "GordonDemo_MonkeySaddle_surface.brep");

  // Monkey saddle center: Z(0.5, 0.5) = 0.
  const double aUMid = 0.5 * (aSurf->UKnot(1) + aSurf->UKnot(aSurf->NbUKnots()));
  const double aVMid = 0.5 * (aSurf->VKnot(1) + aSurf->VKnot(aSurf->NbVKnots()));
  const gp_Pnt aMidPt = aSurf->Value(aUMid, aVMid);
  EXPECT_NEAR(aMidPt.Z(), 0.0, 0.1) << "Monkey saddle center should be near Z=0";
}
