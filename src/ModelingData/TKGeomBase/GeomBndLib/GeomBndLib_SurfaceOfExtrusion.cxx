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

#include <GeomBndLib_SurfaceOfExtrusion.hxx>

#include <GeomAdaptor_Surface.hxx>
#include <GeomBndLib_Curve.hxx>
#include <GeomBndLib_InfiniteHelpers.pxx>
#include <GeomBndLib_OtherSurface.hxx>
#include <Geom_Curve.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <Standard_Failure.hxx>

namespace
{

//! Build the extrusion box from a pre-computed basis curve box.
Bnd_Box buildExtrusionBox(const Bnd_Box& theCurveBox,
                          const gp_Dir&  theDir,
                          const gp_XYZ&  theDirXYZ,
                          double         theVMin,
                          double         theVMax,
                          double         theTol)
{
  Bnd_Box aBox;

  if (theCurveBox.IsVoid())
  {
    aBox.Enlarge(theTol);
    return aBox;
  }

  const bool isVMinInf = Precision::IsNegativeInfinite(theVMin);
  const bool isVMaxInf = Precision::IsPositiveInfinite(theVMax);

  if (theCurveBox.IsOpen())
  {
    // Propagate curve openness through the extrusion rather than returning whole space.
    if (theCurveBox.IsOpenXmin())
      aBox.OpenXmin();
    if (theCurveBox.IsOpenXmax())
      aBox.OpenXmax();
    if (theCurveBox.IsOpenYmin())
      aBox.OpenYmin();
    if (theCurveBox.IsOpenYmax())
      aBox.OpenYmax();
    if (theCurveBox.IsOpenZmin())
      aBox.OpenZmin();
    if (theCurveBox.IsOpenZmax())
      aBox.OpenZmax();
    if (isVMinInf && isVMaxInf)
      GeomBndLib_InfiniteHelpers::OpenMinMax(theDir, aBox);
    else if (isVMinInf)
      GeomBndLib_InfiniteHelpers::OpenMin(theDir, aBox);
    else if (isVMaxInf)
      GeomBndLib_InfiniteHelpers::OpenMax(theDir, aBox);
    if (theCurveBox.HasFinitePart())
    {
      const auto [aXmin, aXmax, aYmin, aYmax, aZmin, aZmax] = theCurveBox.FinitePart().Get();
      if (!isVMinInf)
      {
        const gp_XYZ aShift = theVMin * theDirXYZ;
        aBox.Add(gp_Pnt(aXmin + aShift.X(), aYmin + aShift.Y(), aZmin + aShift.Z()));
        aBox.Add(gp_Pnt(aXmax + aShift.X(), aYmax + aShift.Y(), aZmax + aShift.Z()));
      }
      if (!isVMaxInf)
      {
        const gp_XYZ aShift = theVMax * theDirXYZ;
        aBox.Add(gp_Pnt(aXmin + aShift.X(), aYmin + aShift.Y(), aZmin + aShift.Z()));
        aBox.Add(gp_Pnt(aXmax + aShift.X(), aYmax + aShift.Y(), aZmax + aShift.Z()));
      }
    }
    aBox.Enlarge(theTol);
    return aBox;
  }

  if (isVMinInf && isVMaxInf)
  {
    aBox.Add(theCurveBox);
    GeomBndLib_InfiniteHelpers::OpenMinMax(theDir, aBox);
    aBox.Enlarge(theTol);
    return aBox;
  }

  const auto [aXmin, aXmax, aYmin, aYmax, aZmin, aZmax] = theCurveBox.Get();

  if (isVMinInf)
  {
    GeomBndLib_InfiniteHelpers::OpenMin(theDir, aBox);
    const gp_XYZ aShift = theVMax * theDirXYZ;
    aBox.Add(gp_Pnt(aXmin + aShift.X(), aYmin + aShift.Y(), aZmin + aShift.Z()));
    aBox.Add(gp_Pnt(aXmax + aShift.X(), aYmax + aShift.Y(), aZmax + aShift.Z()));
  }
  else if (isVMaxInf)
  {
    GeomBndLib_InfiniteHelpers::OpenMax(theDir, aBox);
    const gp_XYZ aShift = theVMin * theDirXYZ;
    aBox.Add(gp_Pnt(aXmin + aShift.X(), aYmin + aShift.Y(), aZmin + aShift.Z()));
    aBox.Add(gp_Pnt(aXmax + aShift.X(), aYmax + aShift.Y(), aZmax + aShift.Z()));
  }
  else
  {
    const gp_XYZ aShiftMin = theVMin * theDirXYZ;
    aBox.Add(gp_Pnt(aXmin + aShiftMin.X(), aYmin + aShiftMin.Y(), aZmin + aShiftMin.Z()));
    aBox.Add(gp_Pnt(aXmax + aShiftMin.X(), aYmax + aShiftMin.Y(), aZmax + aShiftMin.Z()));

    const gp_XYZ aShiftMax = theVMax * theDirXYZ;
    aBox.Add(gp_Pnt(aXmin + aShiftMax.X(), aYmin + aShiftMax.Y(), aZmin + aShiftMax.Z()));
    aBox.Add(gp_Pnt(aXmax + aShiftMax.X(), aYmax + aShiftMax.Y(), aZmax + aShiftMax.Z()));
  }

  aBox.Enlarge(theTol);
  return aBox;
}

} // namespace

//=================================================================================================

Bnd_Box GeomBndLib_SurfaceOfExtrusion::Box(double theTol) const
{
  double aU1 = 0., aU2 = 0., aV1 = 0., aV2 = 0.;
  myGeom->Bounds(aU1, aU2, aV1, aV2);
  return Box(aU1, aU2, aV1, aV2, theTol);
}

//=================================================================================================

Bnd_Box GeomBndLib_SurfaceOfExtrusion::Box(double theUMin,
                                           double theUMax,
                                           double theVMin,
                                           double theVMax,
                                           double theTol) const
{
  try
  {
    // P(U, V) = BasisCurve(U) + V * Direction
    const occ::handle<Geom_Curve>& aBasisCurve = myGeom->BasisCurve();
    const gp_Dir&                  aDir        = myGeom->Direction();
    const gp_XYZ&                  aDirXYZ     = aDir.XYZ();

    double aCurveU1 = theUMin;
    double aCurveU2 = theUMax;
    if (!aBasisCurve->IsPeriodic())
    {
      const double aFirst = aBasisCurve->FirstParameter();
      const double aLast  = aBasisCurve->LastParameter();
      if (aCurveU1 < aFirst)
        aCurveU1 = aFirst;
      else if (aCurveU1 > aLast)
        aCurveU1 = aLast;
      if (aCurveU2 < aFirst)
        aCurveU2 = aFirst;
      else if (aCurveU2 > aLast)
        aCurveU2 = aLast;
      if (aCurveU1 > aCurveU2)
      {
        const double aTmp = aCurveU1;
        aCurveU1          = aCurveU2;
        aCurveU2          = aTmp;
      }
    }

    GeomBndLib_Curve aCurveEval(aBasisCurve);
    const Bnd_Box    aCurveBox = aCurveEval.Box(aCurveU1, aCurveU2, 0.);
    if (aCurveBox.IsVoid())
    {
      GeomAdaptor_Surface     anAdaptor(myGeom);
      GeomBndLib_OtherSurface anOther(anAdaptor);
      return anOther.Box(theUMin, theUMax, theVMin, theVMax, theTol);
    }
    return buildExtrusionBox(aCurveBox, aDir, aDirXYZ, theVMin, theVMax, theTol);
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

Bnd_Box GeomBndLib_SurfaceOfExtrusion::BoxOptimal(double theUMin,
                                                  double theUMax,
                                                  double theVMin,
                                                  double theVMax,
                                                  double theTol) const
{
  try
  {
    // Use the tight basis curve box for a more precise result.
    const occ::handle<Geom_Curve>& aBasisCurve = myGeom->BasisCurve();
    const gp_Dir&                  aDir        = myGeom->Direction();
    const gp_XYZ&                  aDirXYZ     = aDir.XYZ();

    double aCurveU1 = theUMin;
    double aCurveU2 = theUMax;
    if (!aBasisCurve->IsPeriodic())
    {
      const double aFirst = aBasisCurve->FirstParameter();
      const double aLast  = aBasisCurve->LastParameter();
      if (aCurveU1 < aFirst)
        aCurveU1 = aFirst;
      else if (aCurveU1 > aLast)
        aCurveU1 = aLast;
      if (aCurveU2 < aFirst)
        aCurveU2 = aFirst;
      else if (aCurveU2 > aLast)
        aCurveU2 = aLast;
      if (aCurveU1 > aCurveU2)
      {
        const double aTmp = aCurveU1;
        aCurveU1          = aCurveU2;
        aCurveU2          = aTmp;
      }
    }

    GeomBndLib_Curve aCurveEval(aBasisCurve);
    const Bnd_Box    aCurveBox = aCurveEval.BoxOptimal(aCurveU1, aCurveU2, 0.);
    if (aCurveBox.IsVoid())
    {
      GeomAdaptor_Surface     anAdaptor(myGeom);
      GeomBndLib_OtherSurface anOther(anAdaptor);
      return anOther.BoxOptimal(theUMin, theUMax, theVMin, theVMax, theTol);
    }
    return buildExtrusionBox(aCurveBox, aDir, aDirXYZ, theVMin, theVMax, theTol);
  }
  catch (Standard_Failure const&)
  {
    // Fall back to robust generic sampling if basis-curve segmentation fails.
    GeomAdaptor_Surface     anAdaptor(myGeom);
    GeomBndLib_OtherSurface anOther(anAdaptor);
    return anOther.BoxOptimal(theUMin, theUMax, theVMin, theVMax, theTol);
  }
}
