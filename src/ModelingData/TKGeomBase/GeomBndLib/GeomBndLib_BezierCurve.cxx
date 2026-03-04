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

#include <GeomBndLib_BezierCurve.hxx>

#include <GeomBndLib_OptimizationHelpers.pxx>
#include <GeomBndLib_SamplingHelpers.pxx>
#include "GeomBndLib_SplineHelpers.pxx"
#include <GeomAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>

namespace
{

} // namespace

//=================================================================================================

Bnd_Box GeomBndLib_BezierCurve::Box(double theTol) const
{
  return Box(myGeom->FirstParameter(), myGeom->LastParameter(), theTol);
}

//=================================================================================================

Bnd_Box GeomBndLib_BezierCurve::Box(double theU1, double theU2, double theTol) const
{
  Bnd_Box           aBox;
  constexpr double  aWeakness = 1.5;
  const int         aDegree   = myGeom->Degree();
  GeomAdaptor_Curve aGACurve(myGeom);
  Bnd_Box           aB1;
  const double      aTol =
    GeomBndLib_SplineHelpers::FillBox<Bnd_Box, GeomAdaptor_Curve, gp_Pnt>(aB1,
                                                                          aGACurve,
                                                                          theU1,
                                                                          theU2,
                                                                          aDegree);
  aB1.Enlarge(aWeakness * aTol);
  GeomBndLib_SplineHelpers::ReduceSplineBox(myGeom->Poles(), aB1, aBox);
  aBox.Enlarge(theTol);
  return aBox;
}

//=================================================================================================

Bnd_Box GeomBndLib_BezierCurve::BoxOptimal(double theU1, double theU2, double theTol) const
{
  Bnd_Box           aBox;
  GeomAdaptor_Curve aGACurve(myGeom);
  const int aNbSamples = GeomBndLib_SamplingHelpers::ComputeNbSamples(aGACurve, theU1, theU2);

  double aCoordMin[3] = {RealLast(), RealLast(), RealLast()};
  double aCoordMax[3] = {-RealLast(), -RealLast(), -RealLast()};
  double aDeflMax[3]  = {-RealLast(), -RealLast(), -RealLast()};

  const double               aDU = (theU2 - theU1) / (aNbSamples - 1), aDU2 = aDU / 2.;
  NCollection_Array1<gp_XYZ> aPnts(1, aNbSamples);
  double                     aU;
  for (int anI = 1, aJ = 0; anI <= aNbSamples; anI++, aJ++)
  {
    aU              = theU1 + aJ * aDU;
    const gp_Pnt aP = aGACurve.EvalD0(aU);
    aPnts(anI)      = aP.XYZ();
    for (int aK = 0; aK < 3; ++aK)
    {
      if (aCoordMin[aK] > aP.Coord(aK + 1))
        aCoordMin[aK] = aP.Coord(aK + 1);
      if (aCoordMax[aK] < aP.Coord(aK + 1))
        aCoordMax[aK] = aP.Coord(aK + 1);
    }
    if (anI > 1)
    {
      const gp_XYZ aPm   = 0.5 * (aPnts(anI - 1) + aPnts(anI));
      const gp_Pnt aPMid = aGACurve.EvalD0(aU - aDU2);
      const gp_XYZ aD    = (aPMid.XYZ() - aPm);
      for (int aK = 0; aK < 3; ++aK)
      {
        if (aCoordMin[aK] > aPMid.Coord(aK + 1))
          aCoordMin[aK] = aPMid.Coord(aK + 1);
        if (aCoordMax[aK] < aPMid.Coord(aK + 1))
          aCoordMax[aK] = aPMid.Coord(aK + 1);
        const double aDiff = std::abs(aD.Coord(aK + 1));
        if (aDeflMax[aK] < aDiff)
          aDeflMax[aK] = aDiff;
      }
    }
  }

  const double anEps = std::max(theTol, Precision::Confusion());
  for (int aK = 0; aK < 3; ++aK)
  {
    const double aDiff = aDeflMax[aK];
    if (aDiff <= anEps)
      continue;
    double aCMin = aCoordMin[aK];
    double aCMax = aCoordMax[aK];
    for (int anI = 1; anI <= aNbSamples; ++anI)
    {
      if (aPnts(anI).Coord(aK + 1) - aCMin < aDiff)
      {
        const double anUMin  = theU1 + std::max(0, anI - 2) * aDU;
        const double anUMax  = theU1 + std::min(aNbSamples - 1, anI) * aDU;
        const double aLocMin = GeomBndLib_OptimizationHelpers::AdjustExtrCurve(aGACurve,
                                                                               anUMin,
                                                                               anUMax,
                                                                               aCMin,
                                                                               aK + 1,
                                                                               anEps,
                                                                               true);
        if (aLocMin < aCMin)
          aCMin = aLocMin;
      }
      else if (aCMax - aPnts(anI).Coord(aK + 1) < aDiff)
      {
        const double anUMin  = theU1 + std::max(0, anI - 2) * aDU;
        const double anUMax  = theU1 + std::min(aNbSamples - 1, anI) * aDU;
        const double aLocMax = GeomBndLib_OptimizationHelpers::AdjustExtrCurve(aGACurve,
                                                                               anUMin,
                                                                               anUMax,
                                                                               aCMax,
                                                                               aK + 1,
                                                                               anEps,
                                                                               false);
        if (aLocMax > aCMax)
          aCMax = aLocMax;
      }
    }
    aCoordMin[aK] = aCMin;
    aCoordMax[aK] = aCMax;
  }

  aBox.Add(gp_Pnt(aCoordMin[0], aCoordMin[1], aCoordMin[2]));
  aBox.Add(gp_Pnt(aCoordMax[0], aCoordMax[1], aCoordMax[2]));
  aBox.Enlarge(anEps);
  return aBox;
}
