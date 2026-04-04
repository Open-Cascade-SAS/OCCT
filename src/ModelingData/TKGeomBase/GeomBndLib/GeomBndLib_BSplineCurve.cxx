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

#include <ElCLib.hxx>
#include <Geom_Geometry.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Precision.hxx>
#include "GeomBndLib_SplineHelpers.pxx"

//=================================================================================================

Bnd_Box GeomBndLib_BSplineCurve::Box(double theTol) const
{
  return Box(myGeom->FirstParameter(), myGeom->LastParameter(), theTol);
}

//=================================================================================================

Bnd_Box GeomBndLib_BSplineCurve::Box(double theU1, double theU2, double theTol) const
{
  constexpr double             aWeakness = 1.5;
  occ::handle<Geom_BSplineCurve> aCurve  = myGeom;
  double                       aU1       = theU1;
  double                       aU2       = theU2;

  if (std::abs(aCurve->FirstParameter() - aU1) > Precision::Parametric(theTol)
      || std::abs(aCurve->LastParameter() - aU2) > Precision::Parametric(theTol))
  {
    occ::handle<Geom_Geometry>     aGeometry = aCurve->Copy();
    occ::handle<Geom_BSplineCurve> aSegmentedCurve = occ::down_cast<Geom_BSplineCurve>(aGeometry);
    if (aSegmentedCurve->IsPeriodic())
    {
      ElCLib::AdjustPeriodic(aSegmentedCurve->FirstParameter(),
                             aSegmentedCurve->LastParameter(),
                             Precision::PConfusion(),
                             aU1,
                             aU2);
    }
    else
    {
      if (aSegmentedCurve->FirstParameter() > aU1)
      {
        aU1 = aSegmentedCurve->FirstParameter();
      }
      if (aSegmentedCurve->LastParameter() < aU2)
      {
        aU2 = aSegmentedCurve->LastParameter();
      }
    }

    double aSegmentTol = 2.0 * Precision::PConfusion();
    if (aSegmentedCurve->IsPeriodic())
    {
      const double aPeriod           = aSegmentedCurve->LastParameter() - aSegmentedCurve->FirstParameter();
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

    aSegmentedCurve->Segment(aU1, aU2, aSegmentTol);
    aCurve = aSegmentedCurve;
  }

  Bnd_Box                         aSampledBox;
  const int                       aKnotFirst = aCurve->FirstUKnotIndex();
  const int                       aKnotLast  = aCurve->LastUKnotIndex();
  const int                       aDegree    = aCurve->Degree();
  const NCollection_Array1<double>& aKnots   = aCurve->Knots();
  GeomAdaptor_Curve               aGACurve(aCurve);
  double                          aMaxDeflection = 0.0;
  double                          aFirst = aKnots(aKnotFirst);
  for (int aKnot = aKnotFirst + 1; aKnot <= aKnotLast; ++aKnot)
  {
    const double aLast = aKnots(aKnot);
    aMaxDeflection = std::max(GeomBndLib_SplineHelpers::FillBox<Bnd_Box,
                                                                GeomAdaptor_Curve,
                                                                gp_Pnt>(aSampledBox,
                                                                        aGACurve,
                                                                        aFirst,
                                                                        aLast,
                                                                        aDegree),
                              aMaxDeflection);
    aFirst = aLast;
  }

  Bnd_Box aBox;
  if (!aSampledBox.IsVoid())
  {
    aSampledBox.Enlarge(aWeakness * aMaxDeflection);
    GeomBndLib_SplineHelpers::ReduceSplineBox(myGeom->Poles(), aSampledBox, aBox);
    aBox.Enlarge(theTol);
  }
  return aBox;
}

//=================================================================================================

Bnd_Box GeomBndLib_BSplineCurve::BoxOptimal(double theU1, double theU2, double theTol) const
{
  GeomAdaptor_Curve aGACurve(myGeom);
  return GeomBndLib_SplineHelpers::CurveBoxOptimal<GeomAdaptor_Curve, Bnd_Box, gp_Pnt>(aGACurve,
                                                                                       theU1,
                                                                                       theU2,
                                                                                       theTol);
}
