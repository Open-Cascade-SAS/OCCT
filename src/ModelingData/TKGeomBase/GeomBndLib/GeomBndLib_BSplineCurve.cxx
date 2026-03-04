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

  const int  aDegree = myGeom->Degree();
  const bool isPerio = myGeom->IsPeriodic();

  if (isPerio)
  {
    // Periodic: use Segment to correctly unwrap the parameter range.
    occ::handle<Geom_Geometry>     aG = myGeom->Copy();
    occ::handle<Geom_BSplineCurve> aBs(occ::down_cast<Geom_BSplineCurve>(aG));
    double                         aU1 = theU1, aU2 = theU2;
    ElCLib::AdjustPeriodic(aBs->FirstParameter(),
                           aBs->LastParameter(),
                           Precision::PConfusion(),
                           aU1,
                           aU2);

    const double aPeriod           = aBs->LastParameter() - aBs->FirstParameter();
    const double aDirectDiff       = std::abs(aU2 - aU1);
    const double aCrossPeriodDiff1 = std::abs(aU2 - aPeriod - aU1);
    const double aCrossPeriodDiff2 = std::abs(aU1 - aPeriod - aU2);
    double       aSegmentTol       = 2. * Precision::PConfusion();
    const double aMinDiff = std::min(aDirectDiff, std::min(aCrossPeriodDiff1, aCrossPeriodDiff2));
    if (aMinDiff < aSegmentTol)
      aSegmentTol = aMinDiff * 0.01;
    aBs->Segment(aU1, aU2, aSegmentTol);

    Bnd_Box                           aB1;
    const int                         aK1    = aBs->FirstUKnotIndex();
    const int                         aK2    = aBs->LastUKnotIndex();
    const NCollection_Array1<double>& aKnots = aBs->Knots();
    GeomAdaptor_Curve                 aGACurve(aBs);
    double                            aFirst = aKnots(aK1), aLast;
    double                            aTol   = 0.0;
    for (int aK = aK1 + 1; aK <= aK2; aK++)
    {
      aLast = aKnots(aK);
      aTol =
        std::max(GeomBndLib_SplineHelpers::FillBox<Bnd_Box, GeomAdaptor_Curve, gp_Pnt>(aB1,
                                                                                       aGACurve,
                                                                                       aFirst,
                                                                                       aLast,
                                                                                       aDegree),
                 aTol);
      aFirst = aLast;
    }
    if (!aB1.IsVoid())
    {
      aB1.Enlarge(aWeakness * aTol);
      GeomBndLib_SplineHelpers::ReduceSplineBox(aBs->Poles(), aB1, aBox);
      aBox.Enlarge(theTol);
    }
    return aBox;
  }

  // Non-periodic: traverse knot spans directly without Copy/Segment.
  const double aU1 = std::max(theU1, myGeom->FirstParameter());
  const double aU2 = std::min(theU2, myGeom->LastParameter());

  const NCollection_Array1<double>& aKnots   = myGeom->Knots();
  const NCollection_Array1<int>&    aMults   = myGeom->Multiplicities();
  const int                         aNbPoles = myGeom->NbPoles();

  // Find the knot-index span range covering [aU1, aU2].
  int aK1 = aKnots.Lower();
  BSplCLib::Hunt(aKnots, aU1, aK1);
  aK1 = std::clamp(aK1, aKnots.Lower(), aKnots.Upper() - 1);

  int aK2 = aKnots.Lower();
  BSplCLib::Hunt(aKnots, aU2, aK2);
  aK2 = std::clamp(aK2 + 1, aKnots.Lower(), aKnots.Upper());

  // FillBox over each knot span, clamping the first and last spans to [aU1, aU2].
  GeomAdaptor_Curve aGACurve(myGeom);
  Bnd_Box           aB1;
  double            aTol   = 0.0;
  double            aFirst = aU1;
  for (int aK = aK1 + 1; aK <= aK2; aK++)
  {
    const double aLast = (aK < aK2) ? std::min(aU2, aKnots(aK)) : aU2;
    if (aLast > aFirst + Precision::PConfusion())
    {
      aTol =
        std::max(GeomBndLib_SplineHelpers::FillBox<Bnd_Box, GeomAdaptor_Curve, gp_Pnt>(aB1,
                                                                                       aGACurve,
                                                                                       aFirst,
                                                                                       aLast,
                                                                                       aDegree),
                 aTol);
    }
    aFirst = aLast;
    if (aFirst >= aU2 - Precision::PConfusion())
      break;
  }
  if (!aB1.IsVoid())
  {
    aB1.Enlarge(aWeakness * aTol);
    int aPoleMin = 1, aPoleMax = aNbPoles;
    GeomBndLib_SplineHelpers::ComputePoleIndexRange(aKnots,
                                                    aMults,
                                                    aDegree,
                                                    aU1,
                                                    aU2,
                                                    aNbPoles,
                                                    false,
                                                    aPoleMin,
                                                    aPoleMax);
    GeomBndLib_SplineHelpers::ReduceSplineBox(myGeom->Poles(), aPoleMin, aPoleMax, aB1, aBox);
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
