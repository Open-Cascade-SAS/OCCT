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

#include <GeomBndLib_Sphere.hxx>

#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <GeomBndLib_Circle.hxx>
#include <gp_Circ.hxx>
#include <gp_Sphere.hxx>
#include <Precision.hxx>

//=================================================================================================

void GeomBndLib_Sphere::Add(double theTol, Bnd_Box& theBox) const
{
  const gp_Sphere aSphere = myGeom->Sphere();
  const gp_Pnt    aP      = aSphere.Location();
  const double    aR      = aSphere.Radius();
  theBox.Update(aP.X() - aR, aP.Y() - aR, aP.Z() - aR,
                aP.X() + aR, aP.Y() + aR, aP.Z() + aR);
  theBox.Enlarge(theTol);
}

//=================================================================================================

//! Compute bounding box for a sphere patch.
static void computeSphere(const gp_Sphere& theSphere,
                           const double     theUMin,
                           const double     theUMax,
                           const double     theVMin,
                           const double     theVMax,
                           Bnd_Box&         theBox)
{
  const gp_Pnt  aP = theSphere.Location();
  const double  aR = theSphere.Radius();

  const double aXmin = aP.X() - aR;
  const double aXmax = aP.X() + aR;
  const double aYmin = aP.Y() - aR;
  const double aYmax = aP.Y() + aR;
  const double aZmin = aP.Z() - aR;
  const double aZmax = aP.Z() + aR;

  constexpr double anUper = 2. * M_PI - Precision::PConfusion();
  constexpr double aVper  = M_PI - Precision::PConfusion();
  if (theUMax - theUMin >= anUper && theVMax - theVMin >= aVper)
  {
    // Whole sphere.
    theBox.Update(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
    return;
  }

  // Check 6 extremal points (axis-aligned extrema of the full sphere).
  double           anU, aV;
  const double     anUmax = theUMin + 2. * M_PI;
  const gp_Ax3&    aPos   = theSphere.Position();
  gp_Pnt           aPExt  = aP;

  // -X extremum
  aPExt.SetX(aXmin);
  ElSLib::SphereParameters(aPos, aR, aPExt, anU, aV);
  anU = ElCLib::InPeriod(anU, theUMin, anUmax);
  if (anU >= theUMin && anU <= theUMax && aV >= theVMin && aV <= theVMax)
  {
    theBox.Add(aPExt);
  }
  // +X extremum
  aPExt.SetX(aXmax);
  ElSLib::SphereParameters(aPos, aR, aPExt, anU, aV);
  anU = ElCLib::InPeriod(anU, theUMin, anUmax);
  if (anU >= theUMin && anU <= theUMax && aV >= theVMin && aV <= theVMax)
  {
    theBox.Add(aPExt);
  }
  aPExt.SetX(aP.X());

  // -Y extremum
  aPExt.SetY(aYmin);
  ElSLib::SphereParameters(aPos, aR, aPExt, anU, aV);
  anU = ElCLib::InPeriod(anU, theUMin, anUmax);
  if (anU >= theUMin && anU <= theUMax && aV >= theVMin && aV <= theVMax)
  {
    theBox.Add(aPExt);
  }
  // +Y extremum
  aPExt.SetY(aYmax);
  ElSLib::SphereParameters(aPos, aR, aPExt, anU, aV);
  anU = ElCLib::InPeriod(anU, theUMin, anUmax);
  if (anU >= theUMin && anU <= theUMax && aV >= theVMin && aV <= theVMax)
  {
    theBox.Add(aPExt);
  }
  aPExt.SetY(aP.Y());

  // -Z extremum
  aPExt.SetZ(aZmin);
  ElSLib::SphereParameters(aPos, aR, aPExt, anU, aV);
  anU = ElCLib::InPeriod(anU, theUMin, anUmax);
  if (anU >= theUMin && anU <= theUMax && aV >= theVMin && aV <= theVMax)
  {
    theBox.Add(aPExt);
  }
  // +Z extremum
  aPExt.SetZ(aZmax);
  ElSLib::SphereParameters(aPos, aR, aPExt, anU, aV);
  anU = ElCLib::InPeriod(anU, theUMin, anUmax);
  if (anU >= theUMin && anU <= theUMax && aV >= theVMin && aV <= theVMax)
  {
    theBox.Add(aPExt);
  }

  // Add boundary iso-curves of the patch.
  gp_Circ aC = ElSLib::SphereUIso(aPos, aR, theUMin);
  GeomBndLib_Circle::Add(aC,theVMin, theVMax, 0., theBox);
  aC = ElSLib::SphereUIso(aPos, aR, theUMax);
  GeomBndLib_Circle::Add(aC,theVMin, theVMax, 0., theBox);

  aC = ElSLib::SphereVIso(aPos, aR, theVMin);
  GeomBndLib_Circle::Add(aC,theUMin, theUMax, 0., theBox);
  aC = ElSLib::SphereVIso(aPos, aR, theVMax);
  GeomBndLib_Circle::Add(aC,theUMin, theUMax, 0., theBox);
}

//=================================================================================================

void GeomBndLib_Sphere::Add(double   theUMin,
                             double   theUMax,
                             double   theVMin,
                             double   theVMax,
                             double   theTol,
                             Bnd_Box& theBox) const
{
  const gp_Sphere aSphere = myGeom->Sphere();

  // Check if it is a whole sphere.
  if (std::abs(theUMin) < Precision::Angular()
      && std::abs(theUMax - 2. * M_PI) < Precision::Angular()
      && std::abs(theVMin + M_PI / 2.) < Precision::Angular()
      && std::abs(theVMax - M_PI / 2.) < Precision::Angular())
  {
    Add(theTol, theBox);
    return;
  }

  computeSphere(aSphere, theUMin, theUMax, theVMin, theVMax, theBox);
  theBox.Enlarge(theTol);
}
