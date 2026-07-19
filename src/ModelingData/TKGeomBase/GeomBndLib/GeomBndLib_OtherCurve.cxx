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

#include <GeomBndLib_OtherCurve.hxx>

#include <GeomBndLib_OptimizationHelpers.pxx>
#include <GeomBndLib_SamplingHelpers.pxx>
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

} // namespace

//=================================================================================================

Bnd_Box GeomBndLib_OtherCurve::Box(double theTol) const
{
  return Box(myCurve.get().FirstParameter(), myCurve.get().LastParameter(), theTol);
}

//=================================================================================================

Bnd_Box GeomBndLib_OtherCurve::Box(double theU1, double theU2, double theTol) const
{
  Bnd_Box          aBox;
  constexpr double weakness = 1.5;
  constexpr int    N        = 33;
  Bnd_Box          aB1;
  double           tol = FillBox(aB1, myCurve.get(), theU1, theU2, N);
  aB1.Enlarge(weakness * tol);
  double x, y, z, X, Y, Z;
  aB1.Get(x, y, z, X, Y, Z);
  aBox.Update(x, y, z, X, Y, Z);
  aBox.Enlarge(theTol);
  return aBox;
}

//=================================================================================================

Bnd_Box GeomBndLib_OtherCurve::BoxOptimal(double theU1, double theU2, double theTol) const
{
  Bnd_Box                aBox;
  const Adaptor3d_Curve& C  = myCurve.get();
  int                    Nu = GeomBndLib_SamplingHelpers::ComputeNbSamples(C, theU1, theU2);

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
    C.D0(u, P);
    aPnts(i) = P.XYZ();
    for (int k = 0; k < 3; ++k)
    {
      if (CoordMin[k] > P.Coord(k + 1))
      {
        CoordMin[k] = P.Coord(k + 1);
      }
      if (CoordMax[k] < P.Coord(k + 1))
      {
        CoordMax[k] = P.Coord(k + 1);
      }
    }
    if (i > 1)
    {
      gp_XYZ aPm = 0.5 * (aPnts(i - 1) + aPnts(i));
      C.D0(u - du2, P);
      gp_XYZ aD = (P.XYZ() - aPm);
      for (int k = 0; k < 3; ++k)
      {
        if (CoordMin[k] > P.Coord(k + 1))
        {
          CoordMin[k] = P.Coord(k + 1);
        }
        if (CoordMax[k] < P.Coord(k + 1))
        {
          CoordMax[k] = P.Coord(k + 1);
        }
        double d = std::abs(aD.Coord(k + 1));
        if (DeflMax[k] < d)
        {
          DeflMax[k] = d;
        }
      }
    }
  }

  double eps = std::max(theTol, Precision::Confusion());
  for (int k = 0; k < 3; ++k)
  {
    double d = DeflMax[k];
    if (d <= eps)
    {
      continue;
    }
    double CMin = CoordMin[k];
    double CMax = CoordMax[k];
    for (int i = 1; i <= Nu; ++i)
    {
      if (aPnts(i).Coord(k + 1) - CMin < d)
      {
        double umin = theU1 + std::max(0, i - 2) * du;
        double umax = theU1 + std::min(Nu - 1, i) * du;
        double cmin =
          GeomBndLib_OptimizationHelpers::AdjustExtrCurve(C, umin, umax, CMin, k + 1, eps, true);
        if (cmin < CMin)
        {
          CMin = cmin;
        }
      }
      else if (CMax - aPnts(i).Coord(k + 1) < d)
      {
        double umin = theU1 + std::max(0, i - 2) * du;
        double umax = theU1 + std::min(Nu - 1, i) * du;
        double cmax =
          GeomBndLib_OptimizationHelpers::AdjustExtrCurve(C, umin, umax, CMax, k + 1, eps, false);
        if (cmax > CMax)
        {
          CMax = cmax;
        }
      }
    }
    CoordMin[k] = CMin;
    CoordMax[k] = CMax;
  }

  aBox.Add(gp_Pnt(CoordMin[0], CoordMin[1], CoordMin[2]));
  aBox.Add(gp_Pnt(CoordMax[0], CoordMax[1], CoordMax[2]));
  aBox.Enlarge(eps);
  return aBox;
}
