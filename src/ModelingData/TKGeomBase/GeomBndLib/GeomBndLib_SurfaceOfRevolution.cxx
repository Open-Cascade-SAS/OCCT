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

#include <GeomBndLib_SurfaceOfRevolution.hxx>

#include <GeomAdaptor_Surface.hxx>
#include <GeomBndLib_Circle.hxx>
#include <GeomBndLib_Curve.hxx>
#include <GeomBndLib_InfiniteHelpers.pxx>
#include <GeomBndLib_OtherSurface.hxx>
#include <Geom_Curve.hxx>
#include <gp_Ax1.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <Standard_Failure.hxx>

namespace
{

//! Add the bounding box contribution of a single revolution circle
//! for a point revolved around the axis.
void addRevolutionCircle(const gp_Pnt& theOrigin,
                         const gp_Dir& theAxisDir,
                         const gp_Pnt& theBasisPt,
                         const double  theUMin,
                         const double  theUMax,
                         Bnd_Box&      theBox)
{
  // Project the basis curve point onto the revolution axis.
  const gp_XYZ aDelta  = theBasisPt.XYZ() - theOrigin.XYZ();
  const double aH      = aDelta.Dot(theAxisDir.XYZ());
  const gp_XYZ aCenter = theOrigin.XYZ() + aH * theAxisDir.XYZ();

  // Radial vector from axis to basis point.
  const gp_XYZ aRadial = theBasisPt.XYZ() - aCenter;
  const double aRadius = aRadial.Modulus();

  if (aRadius < Precision::Confusion())
  {
    // Point is on the axis - revolution is a single point.
    theBox.Add(theBasisPt);
    return;
  }

  // Construct the revolution circle.
  const gp_Dir  aXDir(aRadial);
  const gp_Ax2  anAx2(gp_Pnt(aCenter), theAxisDir, aXDir);
  const gp_Circ aCirc(anAx2, aRadius);

  theBox.Add(GeomBndLib_Circle::Box(aCirc, theUMin, theUMax, 0.));
}

//! Build the revolution box from a pre-computed basis curve box.
static Bnd_Box buildRevolutionBox(const Bnd_Box& theCurveBox,
                                  const gp_Pnt&  theOrigin,
                                  const gp_Dir&  theAxisDir,
                                  double         theUMin,
                                  double         theUMax,
                                  double         theTol)
{
  Bnd_Box aBox;

  if (theCurveBox.IsVoid())
  {
    aBox.Enlarge(theTol);
    return aBox;
  }

  if (theCurveBox.IsOpen())
  {
    // Revolve the finite part corners for the bounded portion.
    if (theCurveBox.HasFinitePart())
    {
      const auto [aXmin, aXmax, aYmin, aYmax, aZmin, aZmax] = theCurveBox.FinitePart().Get();
      const double aXVals[2]                                = {aXmin, aXmax};
      const double aYVals[2]                                = {aYmin, aYmax};
      const double aZVals[2]                                = {aZmin, aZmax};
      for (int ix = 0; ix < 2; ++ix)
      {
        for (int iy = 0; iy < 2; ++iy)
        {
          for (int iz = 0; iz < 2; ++iz)
          {
            addRevolutionCircle(theOrigin,
                                theAxisDir,
                                gp_Pnt(aXVals[ix], aYVals[iy], aZVals[iz]),
                                theUMin,
                                theUMax,
                                aBox);
          }
        }
      }
    }
    // Propagate curve openness: open directions remain open after revolution.
    if (theCurveBox.IsOpenXmin())
    {
      aBox.OpenXmin();
    }
    if (theCurveBox.IsOpenXmax())
    {
      aBox.OpenXmax();
    }
    if (theCurveBox.IsOpenYmin())
    {
      aBox.OpenYmin();
    }
    if (theCurveBox.IsOpenYmax())
    {
      aBox.OpenYmax();
    }
    if (theCurveBox.IsOpenZmin())
    {
      aBox.OpenZmin();
    }
    if (theCurveBox.IsOpenZmax())
    {
      aBox.OpenZmax();
    }
    aBox.Enlarge(theTol);
    return aBox;
  }

  // Get() returns {Xmin, Xmax, Ymin, Ymax, Zmin, Zmax}.
  const auto [aXmin, aXmax, aYmin, aYmax, aZmin, aZmax] = theCurveBox.Get();

  // Revolve all 8 corners of the basis curve bounding box.
  const double aXVals[2] = {aXmin, aXmax};
  const double aYVals[2] = {aYmin, aYmax};
  const double aZVals[2] = {aZmin, aZmax};

  for (int ix = 0; ix < 2; ++ix)
  {
    for (int iy = 0; iy < 2; ++iy)
    {
      for (int iz = 0; iz < 2; ++iz)
      {
        addRevolutionCircle(theOrigin,
                            theAxisDir,
                            gp_Pnt(aXVals[ix], aYVals[iy], aZVals[iz]),
                            theUMin,
                            theUMax,
                            aBox);
      }
    }
  }

  aBox.Enlarge(theTol);
  return aBox;
}

} // namespace

//=================================================================================================

Bnd_Box GeomBndLib_SurfaceOfRevolution::Box(double theTol) const
{
  double aU1 = 0., aU2 = 0., aV1 = 0., aV2 = 0.;
  myGeom->Bounds(aU1, aU2, aV1, aV2);
  return Box(aU1, aU2, aV1, aV2, theTol);
}

//=================================================================================================

Bnd_Box GeomBndLib_SurfaceOfRevolution::Box(double theUMin,
                                            double theUMax,
                                            double theVMin,
                                            double theVMax,
                                            double theTol) const
{
  try
  {
    const occ::handle<Geom_Curve>& aBasisCurve = myGeom->BasisCurve();
    const gp_Ax1                   anAxis      = myGeom->Axis();
    const gp_Pnt&                  anOrigin    = anAxis.Location();
    const gp_Dir&                  anAxisDir   = anAxis.Direction();

    // Clamp V range to basis curve bounds.
    const double aVFirst = aBasisCurve->FirstParameter();
    const double aVLast  = aBasisCurve->LastParameter();
    double       aVMin   = Precision::IsNegativeInfinite(theVMin) ? aVFirst : theVMin;
    double       aVMax   = Precision::IsPositiveInfinite(theVMax) ? aVLast : theVMax;
    if (!aBasisCurve->IsPeriodic())
    {
      if (aVMin < aVFirst)
      {
        aVMin = aVFirst;
      }
      else if (aVMin > aVLast)
      {
        aVMin = aVLast;
      }
      if (aVMax < aVFirst)
      {
        aVMax = aVFirst;
      }
      else if (aVMax > aVLast)
      {
        aVMax = aVLast;
      }
      if (aVMin > aVMax)
      {
        const double aTmp = aVMin;
        aVMin             = aVMax;
        aVMax             = aTmp;
      }
    }

    // Compute the basis curve bounding box, then revolve all 8 corners around the axis.
    GeomBndLib_Curve aCurveEval(aBasisCurve);
    const Bnd_Box    aCurveBox = aCurveEval.Box(aVMin, aVMax, 0.);
    if (aCurveBox.IsVoid())
    {
      GeomAdaptor_Surface     anAdaptor(myGeom);
      GeomBndLib_OtherSurface anOther(anAdaptor);
      return anOther.Box(theUMin, theUMax, theVMin, theVMax, theTol);
    }
    return buildRevolutionBox(aCurveBox, anOrigin, anAxisDir, theUMin, theUMax, theTol);
  }
  catch (Standard_Failure const&)
  {
    // Fall back to robust generic sampling if basis-curve segmentation fails.
    GeomAdaptor_Surface     anAdaptor(myGeom);
    GeomBndLib_OtherSurface anOther(anAdaptor);
    return anOther.Box(theUMin, theUMax, theVMin, theVMax, theTol);
  }
}

//=================================================================================================

Bnd_Box GeomBndLib_SurfaceOfRevolution::BoxOptimal(double theUMin,
                                                   double theUMax,
                                                   double theVMin,
                                                   double theVMax,
                                                   double theTol) const
{
  try
  {
    const occ::handle<Geom_Curve>& aBasisCurve = myGeom->BasisCurve();
    const gp_Ax1                   anAxis      = myGeom->Axis();
    const gp_Pnt&                  anOrigin    = anAxis.Location();
    const gp_Dir&                  anAxisDir   = anAxis.Direction();

    // Clamp V range to basis curve bounds.
    const double aVFirst = aBasisCurve->FirstParameter();
    const double aVLast  = aBasisCurve->LastParameter();
    double       aVMin   = Precision::IsNegativeInfinite(theVMin) ? aVFirst : theVMin;
    double       aVMax   = Precision::IsPositiveInfinite(theVMax) ? aVLast : theVMax;
    if (!aBasisCurve->IsPeriodic())
    {
      if (aVMin < aVFirst)
      {
        aVMin = aVFirst;
      }
      else if (aVMin > aVLast)
      {
        aVMin = aVLast;
      }
      if (aVMax < aVFirst)
      {
        aVMax = aVFirst;
      }
      else if (aVMax > aVLast)
      {
        aVMax = aVLast;
      }
      if (aVMin > aVMax)
      {
        const double aTmp = aVMin;
        aVMin             = aVMax;
        aVMax             = aTmp;
      }
    }

    // Use the tight basis curve box for a more precise result.
    GeomBndLib_Curve aCurveEval(aBasisCurve);
    const Bnd_Box    aCurveBox = aCurveEval.BoxOptimal(aVMin, aVMax, 0.);
    if (aCurveBox.IsVoid())
    {
      GeomAdaptor_Surface     anAdaptor(myGeom);
      GeomBndLib_OtherSurface anOther(anAdaptor);
      return anOther.BoxOptimal(theUMin, theUMax, theVMin, theVMax, theTol);
    }
    return buildRevolutionBox(aCurveBox, anOrigin, anAxisDir, theUMin, theUMax, theTol);
  }
  catch (Standard_Failure const&)
  {
    // Fall back to robust generic sampling if basis-curve segmentation fails.
    GeomAdaptor_Surface     anAdaptor(myGeom);
    GeomBndLib_OtherSurface anOther(anAdaptor);
    return anOther.BoxOptimal(theUMin, theUMax, theVMin, theVMax, theTol);
  }
}
