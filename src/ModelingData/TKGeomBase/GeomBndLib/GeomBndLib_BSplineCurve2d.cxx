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

//=================================================================================================

Bnd_Box2d GeomBndLib_BSplineCurve2d::Box(double theTol) const
{
  return Box(myGeom->FirstParameter(), myGeom->LastParameter(), theTol);
}

//=================================================================================================

Bnd_Box2d GeomBndLib_BSplineCurve2d::Box(double theU1, double theU2, double theTol) const
{
  Bnd_Box2d        aBox;
  constexpr double weakness = 1.5;

  occ::handle<Geom2d_BSplineCurve> aBs = myGeom;
  if (std::abs(aBs->FirstParameter() - theU1) > Precision::Parametric(theTol)
      || std::abs(aBs->LastParameter() - theU2) > Precision::Parametric(theTol))
  {
    occ::handle<Geom2d_Geometry>     G = aBs->Copy();
    occ::handle<Geom2d_BSplineCurve> Bsaux(occ::down_cast<Geom2d_BSplineCurve>(G));
    double                           u1 = theU1, u2 = theU2;
    if (Bsaux->IsPeriodic())
      ElCLib::AdjustPeriodic(Bsaux->FirstParameter(),
                             Bsaux->LastParameter(),
                             Precision::PConfusion(),
                             u1,
                             u2);
    else
    {
      if (Bsaux->FirstParameter() > theU1)
        u1 = Bsaux->FirstParameter();
      if (Bsaux->LastParameter() < theU2)
        u2 = Bsaux->LastParameter();
    }
    double aSegmentTol = 2. * Precision::PConfusion();

    if (Bsaux->IsPeriodic())
    {
      const double aPeriod           = Bsaux->LastParameter() - Bsaux->FirstParameter();
      const double aDirectDiff       = std::abs(u2 - u1);
      const double aCrossPeriodDiff1 = std::abs(u2 - aPeriod - u1);
      const double aCrossPeriodDiff2 = std::abs(u1 - aPeriod - u2);
      const double aMinDiff = std::min(aDirectDiff, std::min(aCrossPeriodDiff1, aCrossPeriodDiff2));
      if (aMinDiff < aSegmentTol)
      {
        aSegmentTol = aMinDiff * 0.01;
      }
    }
    else if (std::abs(u2 - u1) < aSegmentTol)
    {
      aSegmentTol = std::abs(u2 - u1) * 0.01;
    }
    Bsaux->Segment(u1, u2, aSegmentTol);
    aBs = Bsaux;
  }

  Bnd_Box2d                         aB1;
  int                               k1 = aBs->FirstUKnotIndex(), k2 = aBs->LastUKnotIndex();
  int                               N     = aBs->Degree();
  const NCollection_Array1<double>& Knots = aBs->Knots();
  Geom2dAdaptor_Curve               aGACurve(aBs);
  double                            first = Knots(k1), last;
  double                            tol   = 0.0;
  for (int k = k1 + 1; k <= k2; k++)
  {
    last  = Knots(k);
    tol   = std::max(
      GeomBndLib_SplineHelpers::FillBox<Bnd_Box2d, Geom2dAdaptor_Curve, gp_Pnt2d>(
        aB1, aGACurve, first, last, N),
      tol);
    first = last;
  }
  if (!aB1.IsVoid())
  {
    aB1.Enlarge(weakness * tol);
    GeomBndLib_SplineHelpers::ReduceSplineBox(myGeom->Poles(), aB1, aBox);
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
