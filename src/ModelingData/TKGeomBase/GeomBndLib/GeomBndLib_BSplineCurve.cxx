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

#include <GeomBndLib_BSplineCurve.hxx>

#include <GeomBndLib_OptimizationHelpers.pxx>
#include <GeomBndLib_SamplingHelpers.pxx>
#include "GeomBndLib_SplineHelpers.pxx"
#include <BSplCLib.hxx>
#include <ElCLib.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>

namespace
{

} // namespace

//=================================================================================================

Bnd_Box GeomBndLib_BSplineCurve::Box(double theTol) const
{
  return Box(myGeom->FirstParameter(), myGeom->LastParameter(), theTol);
}

//=================================================================================================

Bnd_Box GeomBndLib_BSplineCurve::Box(double theU1, double theU2, double theTol) const
{
  Bnd_Box          aBox;
  constexpr double aWeakness = 1.5;

  occ::handle<Geom_BSplineCurve> aBs = myGeom;
  if (std::abs(aBs->FirstParameter() - theU1) > Precision::Parametric(theTol)
      || std::abs(aBs->LastParameter() - theU2) > Precision::Parametric(theTol))
  {
    occ::handle<Geom_Geometry>     aG = aBs->Copy();
    occ::handle<Geom_BSplineCurve> aBsAux(occ::down_cast<Geom_BSplineCurve>(aG));
    double                         aU1 = theU1, aU2 = theU2;
    if (aBsAux->IsPeriodic())
      ElCLib::AdjustPeriodic(aBsAux->FirstParameter(),
                             aBsAux->LastParameter(),
                             Precision::PConfusion(),
                             aU1,
                             aU2);
    else
    {
      if (aBsAux->FirstParameter() > theU1)
        aU1 = aBsAux->FirstParameter();
      if (aBsAux->LastParameter() < theU2)
        aU2 = aBsAux->LastParameter();
    }
    double aSegmentTol = 2. * Precision::PConfusion();

    if (aBsAux->IsPeriodic())
    {
      const double aPeriod           = aBsAux->LastParameter() - aBsAux->FirstParameter();
      const double aDirectDiff       = std::abs(aU2 - aU1);
      const double aCrossPeriodDiff1 = std::abs(aU2 - aPeriod - aU1);
      const double aCrossPeriodDiff2 = std::abs(aU1 - aPeriod - aU2);
      const double aMinDiff = std::min(aDirectDiff, std::min(aCrossPeriodDiff1, aCrossPeriodDiff2));
      if (aMinDiff < aSegmentTol)
      {
        aSegmentTol = aMinDiff * 0.01;
      }
    }
    else if (std::abs(aU2 - aU1) < aSegmentTol)
    {
      aSegmentTol = std::abs(aU2 - aU1) * 0.01;
    }
    aBsAux->Segment(aU1, aU2, aSegmentTol);
    aBs = aBsAux;
  }

  Bnd_Box                           aB1;
  int                               aK1 = aBs->FirstUKnotIndex(), aK2 = aBs->LastUKnotIndex();
  int                               aDegree = aBs->Degree();
  const NCollection_Array1<double>& aKnots  = aBs->Knots();
  GeomAdaptor_Curve                 aGACurve(aBs);
  double                            aFirst = aKnots(aK1), aLast;
  double                            aTol   = 0.0;
  for (int aK = aK1 + 1; aK <= aK2; aK++)
  {
    aLast  = aKnots(aK);
    aTol   = std::max(
      GeomBndLib_SplineHelpers::FillBox<Bnd_Box, GeomAdaptor_Curve, gp_Pnt>(
        aB1, aGACurve, aFirst, aLast, aDegree),
      aTol);
    aFirst = aLast;
  }
  if (!aB1.IsVoid())
  {
    aB1.Enlarge(aWeakness * aTol);
    GeomBndLib_SplineHelpers::ReduceSplineBox(myGeom->Poles(), aB1, aBox);
    aBox.Enlarge(theTol);
  }
  return aBox;
}

//=================================================================================================

Bnd_Box GeomBndLib_BSplineCurve::BoxOptimal(double theU1, double theU2, double theTol) const
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
