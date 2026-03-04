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

#include <GeomBndLib_Curve.hxx>
#include <GeomBndLib_InfiniteHelpers.pxx>
#include <Geom_Curve.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>

//=================================================================================================

void GeomBndLib_SurfaceOfExtrusion::Add(double theTol, Bnd_Box& theBox) const
{
  double aU1 = 0., aU2 = 0., aV1 = 0., aV2 = 0.;
  myGeom->Bounds(aU1, aU2, aV1, aV2);
  Add(aU1, aU2, aV1, aV2, theTol, theBox);
}

//=================================================================================================

void GeomBndLib_SurfaceOfExtrusion::Add(double   theUMin,
                                        double   theUMax,
                                        double   theVMin,
                                        double   theVMax,
                                        double   theTol,
                                        Bnd_Box& theBox) const
{
  // P(U, V) = BasisCurve(U) + V * Direction
  // The bounding box = basis curve box shifted by V * Direction for V in [VMin, VMax].
  const occ::handle<Geom_Curve>& aBasisCurve = myGeom->BasisCurve();
  const gp_Dir&                  aDir        = myGeom->Direction();
  const gp_XYZ&                  aDirXYZ     = aDir.XYZ();

  // Compute basis curve bounding box for [UMin, UMax].
  GeomBndLib_Curve aCurveEval(aBasisCurve);
  Bnd_Box          aCurveBox;
  aCurveEval.Add(theUMin, theUMax, 0., aCurveBox);

  if (aCurveBox.IsVoid())
  {
    theBox.Enlarge(theTol);
    return;
  }
  if (aCurveBox.IsOpen())
  {
    theBox.SetWhole();
    theBox.Enlarge(theTol);
    return;
  }

  const bool isVMinInf = Precision::IsNegativeInfinite(theVMin);
  const bool isVMaxInf = Precision::IsPositiveInfinite(theVMax);

  if (isVMinInf && isVMaxInf)
  {
    // Fully infinite in V: add basis curve box and open along direction.
    theBox.Add(aCurveBox);
    GeomBndLib_InfiniteHelpers::OpenMinMax(aDir, theBox);
    theBox.Enlarge(theTol);
    return;
  }

  // Get basis curve box limits.
  auto [aXmin, aXmax, aYmin, aYmax, aZmin, aZmax] = aCurveBox.Get();

  if (isVMinInf)
  {
    // V goes to -infinity: open in the negative direction sense.
    GeomBndLib_InfiniteHelpers::OpenMin(aDir, theBox);
    // Add the finite endpoint (VMax).
    const gp_XYZ aShift = theVMax * aDirXYZ;
    theBox.Add(gp_Pnt(aXmin + aShift.X(), aYmin + aShift.Y(), aZmin + aShift.Z()));
    theBox.Add(gp_Pnt(aXmax + aShift.X(), aYmax + aShift.Y(), aZmax + aShift.Z()));
  }
  else if (isVMaxInf)
  {
    // V goes to +infinity: open in the positive direction sense.
    GeomBndLib_InfiniteHelpers::OpenMax(aDir, theBox);
    // Add the finite endpoint (VMin).
    const gp_XYZ aShift = theVMin * aDirXYZ;
    theBox.Add(gp_Pnt(aXmin + aShift.X(), aYmin + aShift.Y(), aZmin + aShift.Z()));
    theBox.Add(gp_Pnt(aXmax + aShift.X(), aYmax + aShift.Y(), aZmax + aShift.Z()));
  }
  else
  {
    // Both V values are finite.
    // Add basis curve box shifted by VMin * Direction.
    const gp_XYZ aShiftMin = theVMin * aDirXYZ;
    theBox.Add(gp_Pnt(aXmin + aShiftMin.X(), aYmin + aShiftMin.Y(), aZmin + aShiftMin.Z()));
    theBox.Add(gp_Pnt(aXmax + aShiftMin.X(), aYmax + aShiftMin.Y(), aZmax + aShiftMin.Z()));

    // Add basis curve box shifted by VMax * Direction.
    const gp_XYZ aShiftMax = theVMax * aDirXYZ;
    theBox.Add(gp_Pnt(aXmin + aShiftMax.X(), aYmin + aShiftMax.Y(), aZmin + aShiftMax.Z()));
    theBox.Add(gp_Pnt(aXmax + aShiftMax.X(), aYmax + aShiftMax.Y(), aZmax + aShiftMax.Z()));
  }

  theBox.Enlarge(theTol);
}
