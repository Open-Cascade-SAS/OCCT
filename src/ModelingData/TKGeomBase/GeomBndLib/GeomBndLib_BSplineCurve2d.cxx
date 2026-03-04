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

#include <GeomBndLib_BSplineCurve2d.hxx>

#include <ElCLib.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomBndLib_OptimizationHelpers.pxx>
#include <GeomBndLib_SamplingHelpers.pxx>
#include "GeomBndLib_SplineHelpers.pxx"
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <cmath>

//=================================================================================================

Bnd_Box2d GeomBndLib_BSplineCurve2d::Box(double theTol) const
{
  return Box(myGeom->FirstParameter(), myGeom->LastParameter(), theTol);
}

//=================================================================================================

Bnd_Box2d GeomBndLib_BSplineCurve2d::Box(double theU1, double theU2, double theTol) const
{
  Bnd_Box2d        aBox;
  constexpr double aWeakness = 1.5;

  const int  aDegree = myGeom->Degree();
  const bool isPerio = myGeom->IsPeriodic();

  if (isPerio)
  {
    // Periodic: use Segment to correctly unwrap the parameter range.
    occ::handle<Geom2d_Geometry>     aG = myGeom->Copy();
    occ::handle<Geom2d_BSplineCurve> aBs(occ::down_cast<Geom2d_BSplineCurve>(aG));
    double                           aU1 = theU1, aU2 = theU2;
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

    Bnd_Box2d                         aB1;
    const int                         aK1    = aBs->FirstUKnotIndex();
    const int                         aK2    = aBs->LastUKnotIndex();
    const NCollection_Array1<double>& aKnots = aBs->Knots();
    Geom2dAdaptor_Curve               aGACurve(aBs);
    double                            aFirst = aKnots(aK1), aLast;
    double                            aTol   = 0.0;
    for (int aK = aK1 + 1; aK <= aK2; aK++)
    {
      aLast = aKnots(aK);
      aTol  = std::max(
        GeomBndLib_SplineHelpers::FillBox<Bnd_Box2d, Geom2dAdaptor_Curve, gp_Pnt2d>(aB1,
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
  Geom2dAdaptor_Curve aGACurve(myGeom);
  Bnd_Box2d           aB1;
  double              aTol   = 0.0;
  double              aFirst = aU1;
  for (int aK = aK1 + 1; aK <= aK2; aK++)
  {
    const double aLast = (aK < aK2) ? std::min(aU2, aKnots(aK)) : aU2;
    if (aLast > aFirst + Precision::PConfusion())
    {
      aTol = std::max(
        GeomBndLib_SplineHelpers::FillBox<Bnd_Box2d, Geom2dAdaptor_Curve, gp_Pnt2d>(aB1,
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

Bnd_Box2d GeomBndLib_BSplineCurve2d::BoxOptimal(double theU1, double theU2, double theTol) const
{
  Bnd_Box2d           aBox;
  Geom2dAdaptor_Curve aGACurve(myGeom);
  int                 Nu = GeomBndLib_SamplingHelpers::ComputeNbSamples2d(aGACurve, theU1, theU2);

  double CoordMin[2] = {RealLast(), RealLast()};
  double CoordMax[2] = {-RealLast(), -RealLast()};
  double DeflMax[2]  = {-RealLast(), -RealLast()};

  gp_Pnt2d                  P;
  double                    du = (theU2 - theU1) / (Nu - 1), du2 = du / 2.;
  NCollection_Array1<gp_XY> aPnts(1, Nu);
  double                    u;
  for (int i = 1, j = 0; i <= Nu; i++, j++)
  {
    u = theU1 + j * du;
    aGACurve.D0(u, P);
    aPnts(i) = P.XY();
    for (int k = 0; k < 2; ++k)
    {
      if (CoordMin[k] > P.Coord(k + 1))
        CoordMin[k] = P.Coord(k + 1);
      if (CoordMax[k] < P.Coord(k + 1))
        CoordMax[k] = P.Coord(k + 1);
    }
    if (i > 1)
    {
      gp_XY aPm = 0.5 * (aPnts(i - 1) + aPnts(i));
      aGACurve.D0(u - du2, P);
      gp_XY aD = (P.XY() - aPm);
      for (int k = 0; k < 2; ++k)
      {
        if (CoordMin[k] > P.Coord(k + 1))
          CoordMin[k] = P.Coord(k + 1);
        if (CoordMax[k] < P.Coord(k + 1))
          CoordMax[k] = P.Coord(k + 1);
        double d = std::abs(aD.Coord(k + 1));
        if (DeflMax[k] < d)
          DeflMax[k] = d;
      }
    }
  }

  double eps = std::max(theTol, Precision::Confusion());
  for (int k = 0; k < 2; ++k)
  {
    double d = DeflMax[k];
    if (d <= eps)
      continue;
    double CMin = CoordMin[k];
    double CMax = CoordMax[k];
    for (int i = 1; i <= Nu; ++i)
    {
      if (aPnts(i).Coord(k + 1) - CMin < d)
      {
        double umin = theU1 + std::max(0, i - 2) * du;
        double umax = theU1 + std::min(Nu - 1, i) * du;
        double cmin = GeomBndLib_OptimizationHelpers::AdjustExtrCurve2d(aGACurve,
                                                                        umin,
                                                                        umax,
                                                                        CMin,
                                                                        k + 1,
                                                                        eps,
                                                                        true);
        if (cmin < CMin)
          CMin = cmin;
      }
      else if (CMax - aPnts(i).Coord(k + 1) < d)
      {
        double umin = theU1 + std::max(0, i - 2) * du;
        double umax = theU1 + std::min(Nu - 1, i) * du;
        double cmax = GeomBndLib_OptimizationHelpers::AdjustExtrCurve2d(aGACurve,
                                                                        umin,
                                                                        umax,
                                                                        CMax,
                                                                        k + 1,
                                                                        eps,
                                                                        false);
        if (cmax > CMax)
          CMax = cmax;
      }
    }
    CoordMin[k] = CMin;
    CoordMax[k] = CMax;
  }

  aBox.Add(gp_Pnt2d(CoordMin[0], CoordMin[1]));
  aBox.Add(gp_Pnt2d(CoordMax[0], CoordMax[1]));
  aBox.Enlarge(eps);
  return aBox;
}
