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

#include <GeomBndLib_Torus.hxx>

#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomBndLib_Circle.hxx>
#include <GeomBndLib_OtherSurface.hxx>
#include <gp_Circ.hxx>
#include <gp_Torus.hxx>
#include <Precision.hxx>
#include <cmath>

//=================================================================================================

//! Compute bounding box for a degenerate torus (Ra < Ri) patch
//! using the extremal-point algorithm.
static void computeDegeneratedTorus(const gp_Torus& theTorus,
                                     const double    theUMin,
                                     const double    theUMax,
                                     const double    theVMin,
                                     const double    theVMax,
                                     Bnd_Box&        theBox)
{
  const gp_Pnt  aP   = theTorus.Location();
  const double  aRa  = theTorus.MajorRadius();
  const double  aRi  = theTorus.MinorRadius();
  const double  aXmin = aP.X() - aRa - aRi;
  const double  aXmax = aP.X() + aRa + aRi;
  const double  aYmin = aP.Y() - aRa - aRi;
  const double  aYmax = aP.Y() + aRa + aRi;
  const double  aZmin = aP.Z() - aRi;
  const double  aZmax = aP.Z() + aRi;

  const double aPhi = std::acos(-aRa / aRi);

  constexpr double anUper = 2. * M_PI - Precision::PConfusion();
  const double     aVper  = 2. * aPhi - Precision::PConfusion();
  if (theUMax - theUMin >= anUper && theVMax - theVMin >= aVper)
  {
    // Whole degenerate torus.
    theBox.Update(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
    return;
  }

  // Check 6 extremal points.
  double        anU, aV;
  const double  anUmax = theUMin + 2. * M_PI;
  const gp_Ax3& aPos   = theTorus.Position();
  gp_Pnt        aPExt  = aP;

  aPExt.SetX(aXmin);
  ElSLib::TorusParameters(aPos, aRa, aRi, aPExt, anU, aV);
  anU = ElCLib::InPeriod(anU, theUMin, anUmax);
  if (anU >= theUMin && anU <= theUMax && aV >= theVMin && aV <= theVMax)
  {
    theBox.Add(aPExt);
  }
  aPExt.SetX(aXmax);
  ElSLib::TorusParameters(aPos, aRa, aRi, aPExt, anU, aV);
  anU = ElCLib::InPeriod(anU, theUMin, anUmax);
  if (anU >= theUMin && anU <= theUMax && aV >= theVMin && aV <= theVMax)
  {
    theBox.Add(aPExt);
  }
  aPExt.SetX(aP.X());

  aPExt.SetY(aYmin);
  ElSLib::TorusParameters(aPos, aRa, aRi, aPExt, anU, aV);
  anU = ElCLib::InPeriod(anU, theUMin, anUmax);
  if (anU >= theUMin && anU <= theUMax && aV >= theVMin && aV <= theVMax)
  {
    theBox.Add(aPExt);
  }
  aPExt.SetY(aYmax);
  ElSLib::TorusParameters(aPos, aRa, aRi, aPExt, anU, aV);
  anU = ElCLib::InPeriod(anU, theUMin, anUmax);
  if (anU >= theUMin && anU <= theUMax && aV >= theVMin && aV <= theVMax)
  {
    theBox.Add(aPExt);
  }
  aPExt.SetY(aP.Y());

  aPExt.SetZ(aZmin);
  ElSLib::TorusParameters(aPos, aRa, aRi, aPExt, anU, aV);
  anU = ElCLib::InPeriod(anU, theUMin, anUmax);
  if (anU >= theUMin && anU <= theUMax && aV >= theVMin && aV <= theVMax)
  {
    theBox.Add(aPExt);
  }
  aPExt.SetZ(aZmax);
  ElSLib::TorusParameters(aPos, aRa, aRi, aPExt, anU, aV);
  anU = ElCLib::InPeriod(anU, theUMin, anUmax);
  if (anU >= theUMin && anU <= theUMax && aV >= theVMin && aV <= theVMax)
  {
    theBox.Add(aPExt);
  }

  // Add boundary iso-curves of the patch.
  gp_Circ aC = ElSLib::TorusUIso(aPos, aRa, aRi, theUMin);
  theBox.Add(GeomBndLib_Circle::Box(aC, theVMin, theVMax, 0.));
  aC = ElSLib::TorusUIso(aPos, aRa, aRi, theUMax);
  theBox.Add(GeomBndLib_Circle::Box(aC, theVMin, theVMax, 0.));

  aC = ElSLib::TorusVIso(aPos, aRa, aRi, theVMin);
  theBox.Add(GeomBndLib_Circle::Box(aC, theUMin, theUMax, 0.));
  aC = ElSLib::TorusVIso(aPos, aRa, aRi, theVMax);
  theBox.Add(GeomBndLib_Circle::Box(aC, theUMin, theUMax, 0.));
}

//=================================================================================================

Bnd_Box GeomBndLib_Torus::Box(double theTol) const
{
  Bnd_Box aBox;
  const gp_Torus aTorus = myGeom->Torus();
  const double   aRMa   = aTorus.MajorRadius();
  const double   aRmi   = aTorus.MinorRadius();
  const double   aR     = aRMa + aRmi;
  const gp_XYZ   aO     = aTorus.Location().XYZ();
  const gp_XYZ   aXd    = aTorus.XAxis().Direction().XYZ();
  const gp_XYZ   aYd    = aTorus.YAxis().Direction().XYZ();
  const gp_XYZ   aZd    = aTorus.Axis().Direction().XYZ();
  // Precompute scaled direction vectors.
  const gp_XYZ aRXd  = aR * aXd;
  const gp_XYZ aRYd  = aR * aYd;
  const gp_XYZ aRiZd = aRmi * aZd;
  // Add 8 corner points of torus bounding box.
  aBox.Add(gp_Pnt(aO - aRXd - aRYd + aRiZd));
  aBox.Add(gp_Pnt(aO - aRXd - aRYd - aRiZd));
  aBox.Add(gp_Pnt(aO + aRXd - aRYd + aRiZd));
  aBox.Add(gp_Pnt(aO + aRXd - aRYd - aRiZd));
  aBox.Add(gp_Pnt(aO - aRXd + aRYd + aRiZd));
  aBox.Add(gp_Pnt(aO - aRXd + aRYd - aRiZd));
  aBox.Add(gp_Pnt(aO + aRXd + aRYd + aRiZd));
  aBox.Add(gp_Pnt(aO + aRXd + aRYd - aRiZd));
  aBox.Enlarge(theTol);
  return aBox;
}

//=================================================================================================

Bnd_Box GeomBndLib_Torus::Box(double   theUMin,
                              double   theUMax,
                              double   theVMin,
                              double   theVMax,
                              double   theTol) const
{
  Bnd_Box aBox;
  const gp_Torus aTorus = myGeom->Torus();
  const double   aRa    = aTorus.MajorRadius();
  const double   aRi    = aTorus.MinorRadius();

  if (aRa < aRi)
  {
    computeDegeneratedTorus(aTorus, theUMin, theUMax, theVMin, theVMax, aBox);
    aBox.Enlarge(theTol);
    return aBox;
  }

  // Compute cross-section V parameter range as integer multiples of PI/4.
  int aFi1, aFi2;
  if (theVMax < theVMin)
  {
    aFi1 = static_cast<int>(std::floor(theVMax / (M_PI / 4.)));
    aFi2 = static_cast<int>(std::floor(theVMin / (M_PI / 4.)));
  }
  else
  {
    aFi1 = static_cast<int>(std::floor(theVMin / (M_PI / 4.)));
    aFi2 = static_cast<int>(std::floor(theVMax / (M_PI / 4.)));
  }
  aFi2++;

  if (aFi2 < aFi1)
  {
    return aBox;
  }

  constexpr double THE_COS_PI4 = 0.70710678118654746;

  // Cache direction vectors.
  const gp_XYZ aZDir   = aTorus.Axis().Direction().XYZ();
  const gp_XYZ aLocXYZ = aTorus.Location().XYZ();
  const gp_Dir aNorm    = aTorus.Axis().Direction();
  const gp_Dir aXDir    = aTorus.XAxis().Direction();

  // Multipliers for torus cross-section points at 45-degree intervals.
  constexpr double aRadiusMult[8] =
    {1., THE_COS_PI4, 0., -THE_COS_PI4, -1., -THE_COS_PI4, 0., THE_COS_PI4};
  constexpr double aZMult[8] =
    {0., THE_COS_PI4, 1., THE_COS_PI4, 0., -THE_COS_PI4, -1., -THE_COS_PI4};

  // For each cross-section sample, construct the circle in the U-direction
  // and compute its analytical arc bounding box.
  const auto addTorusPoint = [&](int theIdx) {
    const double aRadius = aRa + aRi * aRadiusMult[theIdx];
    const gp_Pnt aCenter(aLocXYZ + (aRi * aZMult[theIdx]) * aZDir);
    if (aRadius < Precision::Confusion())
    {
      aBox.Add(aCenter);
      return;
    }
    gp_Circ aC(gp_Ax2(aCenter, aNorm, aXDir), aRadius);
    aBox.Add(GeomBndLib_Circle::Box(aC, theUMin, theUMax, 0.));
  };

  for (int i = aFi1; i <= aFi2; ++i)
  {
    addTorusPoint(((i % 8) + 8) % 8);
  }

  aBox.Enlarge(theTol);
  return aBox;
}

//=================================================================================================

Bnd_Box GeomBndLib_Torus::BoxOptimal(double theUMin,
                                     double theUMax,
                                     double theVMin,
                                     double theVMax,
                                     double theTol) const
{
  GeomAdaptor_Surface     anAdaptor(myGeom);
  GeomBndLib_OtherSurface anOther(anAdaptor);
  return anOther.BoxOptimal(theUMin, theUMax, theVMin, theVMax, theTol);
}

//=================================================================================================

Bnd_Box GeomBndLib_Torus::BoxOptimal(double theTol) const
{
  GeomAdaptor_Surface     anAdaptor(myGeom);
  GeomBndLib_OtherSurface anOther(anAdaptor);
  return anOther.BoxOptimal(theTol);
}
