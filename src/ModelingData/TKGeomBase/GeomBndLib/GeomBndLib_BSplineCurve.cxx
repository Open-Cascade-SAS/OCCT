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
#include <BSplCLib.hxx>
#include <ElCLib.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>

namespace
{

//! Fill box with sampled points and return maximum deflection.
double FillBox(Bnd_Box&               theBox,
               const Adaptor3d_Curve& theCurve,
               const double           theFirst,
               const double           theLast,
               const int              theN)
{
  gp_Pnt aP1, aP2, aP3;
  theCurve.D0(theFirst, aP1);
  theBox.Add(aP1);
  double p = theFirst, dp = theLast - theFirst, tol = 0.;
  if (std::abs(dp) > Precision::PConfusion())
  {
    dp /= 2 * theN;
    for (int i = 1; i <= theN; i++)
    {
      p += dp;
      theCurve.D0(p, aP2);
      theBox.Add(aP2);
      p += dp;
      theCurve.D0(p, aP3);
      theBox.Add(aP3);
      gp_Pnt aPc((aP1.XYZ() + aP3.XYZ()) / 2.0);
      tol = std::max(tol, aPc.Distance(aP2));
      aP1 = aP3;
    }
  }
  else
  {
    theCurve.D0(theFirst, aP1);
    theBox.Add(aP1);
    theCurve.D0(theLast, aP3);
    theBox.Add(aP3);
  }
  return tol;
}

//! Reduce box using poles convex hull.
void ReduceSplineBox(const occ::handle<Geom_BSplineCurve>& theCurve,
                     const Bnd_Box&                        theOrigBox,
                     Bnd_Box&                              theReducedBox)
{
  Bnd_Box                         aPolesBox;
  const NCollection_Array1<gp_Pnt>& aPoles = theCurve->Poles();
  for (int anIdx = aPoles.Lower(); anIdx <= aPoles.Upper(); ++anIdx)
  {
    aPolesBox.Add(aPoles.Value(anIdx));
  }

  const auto [aXMin, aXMax, aYMin, aYMax, aZMin, aZMax] = theOrigBox.Get();
  if (!aPolesBox.IsVoid())
  {
    const auto [aPXMin, aPXMax, aPYMin, aPYMax, aPZMin, aPZMax] = aPolesBox.Get();
    theReducedBox.Update(std::max(aXMin, aPXMin),
                         std::max(aYMin, aPYMin),
                         std::max(aZMin, aPZMin),
                         std::min(aXMax, aPXMax),
                         std::min(aYMax, aPYMax),
                         std::min(aZMax, aPZMax));
  }
  else
  {
    theReducedBox.Update(aXMin, aYMin, aZMin, aXMax, aYMax, aZMax);
  }
}

} // namespace

//=================================================================================================

void GeomBndLib_BSplineCurve::Add(double theTol, Bnd_Box& theBox) const
{
  Add(myGeom->FirstParameter(), myGeom->LastParameter(), theTol, theBox);
}

//=================================================================================================

void GeomBndLib_BSplineCurve::Add(double theU1, double theU2, double theTol, Bnd_Box& theBox) const
{
  constexpr double weakness = 1.5;

  occ::handle<Geom_BSplineCurve> aBs = myGeom;
  if (std::abs(aBs->FirstParameter() - theU1) > Precision::Parametric(theTol)
      || std::abs(aBs->LastParameter() - theU2) > Precision::Parametric(theTol))
  {
    occ::handle<Geom_Geometry>     G = aBs->Copy();
    occ::handle<Geom_BSplineCurve> Bsaux(occ::down_cast<Geom_BSplineCurve>(G));
    double                         u1 = theU1, u2 = theU2;
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
      const double aPeriod = Bsaux->LastParameter() - Bsaux->FirstParameter();
      const double aDirectDiff      = std::abs(u2 - u1);
      const double aCrossPeriodDiff1 = std::abs(u2 - aPeriod - u1);
      const double aCrossPeriodDiff2 = std::abs(u1 - aPeriod - u2);
      const double aMinDiff =
        std::min(aDirectDiff, std::min(aCrossPeriodDiff1, aCrossPeriodDiff2));
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

  Bnd_Box                          aB1;
  int                              k1 = aBs->FirstUKnotIndex(), k2 = aBs->LastUKnotIndex();
  int                              N = aBs->Degree();
  const NCollection_Array1<double>& Knots = aBs->Knots();
  GeomAdaptor_Curve                 aGACurve(aBs);
  double                            first = Knots(k1), last;
  double                            tol = 0.0;
  for (int k = k1 + 1; k <= k2; k++)
  {
    last  = Knots(k);
    tol   = std::max(FillBox(aB1, aGACurve, first, last, N), tol);
    first = last;
  }
  if (!aB1.IsVoid())
  {
    aB1.Enlarge(weakness * tol);
    ReduceSplineBox(myGeom, aB1, theBox);
    theBox.Enlarge(theTol);
  }
}

//=================================================================================================

void GeomBndLib_BSplineCurve::AddOptimal(double theU1, double theU2, double theTol, Bnd_Box& theBox) const
{
  GeomAdaptor_Curve aGACurve(myGeom);
  int               Nu = GeomBndLib_SamplingHelpers::ComputeNbSamples(aGACurve, theU1, theU2);

  double CoordMin[3] = {RealLast(), RealLast(), RealLast()};
  double CoordMax[3] = {-RealLast(), -RealLast(), -RealLast()};
  double DeflMax[3]  = {-RealLast(), -RealLast(), -RealLast()};

  gp_Pnt                     P;
  double                     du = (theU2 - theU1) / (Nu - 1), du2 = du / 2.;
  NCollection_Array1<gp_XYZ> aPnts(1, Nu);
  double                     u;
  for (int i = 1, j = 0; i <= Nu; i++, j++)
  {
    u = theU1 + j * du;
    aGACurve.D0(u, P);
    aPnts(i) = P.XYZ();
    for (int k = 0; k < 3; ++k)
    {
      if (CoordMin[k] > P.Coord(k + 1))
        CoordMin[k] = P.Coord(k + 1);
      if (CoordMax[k] < P.Coord(k + 1))
        CoordMax[k] = P.Coord(k + 1);
    }
    if (i > 1)
    {
      gp_XYZ aPm = 0.5 * (aPnts(i - 1) + aPnts(i));
      aGACurve.D0(u - du2, P);
      gp_XYZ aD = (P.XYZ() - aPm);
      for (int k = 0; k < 3; ++k)
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
  for (int k = 0; k < 3; ++k)
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
        double cmin =
          GeomBndLib_OptimizationHelpers::AdjustExtrCurve(aGACurve, umin, umax, CMin, k + 1, eps, true);
        if (cmin < CMin)
          CMin = cmin;
      }
      else if (CMax - aPnts(i).Coord(k + 1) < d)
      {
        double umin = theU1 + std::max(0, i - 2) * du;
        double umax = theU1 + std::min(Nu - 1, i) * du;
        double cmax =
          GeomBndLib_OptimizationHelpers::AdjustExtrCurve(aGACurve, umin, umax, CMax, k + 1, eps, false);
        if (cmax > CMax)
          CMax = cmax;
      }
    }
    CoordMin[k] = CMin;
    CoordMax[k] = CMax;
  }

  theBox.Add(gp_Pnt(CoordMin[0], CoordMin[1], CoordMin[2]));
  theBox.Add(gp_Pnt(CoordMax[0], CoordMax[1], CoordMax[2]));
  theBox.Enlarge(eps);
}
