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
#include <GeomBndLib_SamplingHelpers.pxx>
#include <gp_Pnt2d.hxx>
#include <math_BrentMinimum.hxx>
#include <math_Function.hxx>
#include <math_MultipleVarFunction.hxx>
#include <math_PSO.hxx>
#include <math_Vector.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>

namespace
{

//! Fill 2D box with sampled points and return maximum deflection.
double FillBox2d(Bnd_Box2d&               theBox,
                 const Adaptor2d_Curve2d&  theCurve,
                 const double              theFirst,
                 const double              theLast,
                 const int                 theN)
{
  gp_Pnt2d aP1, aP2, aP3;
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
      gp_Pnt2d aPc((aP1.XY() + aP3.XY()) / 2.0);
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
void ReduceSplineBox2d(const occ::handle<Geom2d_BSplineCurve>& theCurve,
                       const Bnd_Box2d&                        theOrigBox,
                       Bnd_Box2d&                              theReducedBox)
{
  Bnd_Box2d                           aPolesBox;
  const NCollection_Array1<gp_Pnt2d>& aPoles = theCurve->Poles();
  for (int anIdx = aPoles.Lower(); anIdx <= aPoles.Upper(); ++anIdx)
  {
    aPolesBox.Add(aPoles.Value(anIdx));
  }

  const auto [aXMin, aXMax, aYMin, aYMax] = theOrigBox.Get();
  if (!aPolesBox.IsVoid())
  {
    const auto [aPXMin, aPXMax, aPYMin, aPYMax] = aPolesBox.Get();
    theReducedBox.Update(std::max(aXMin, aPXMin),
                         std::max(aYMin, aPYMin),
                         std::min(aXMax, aPXMax),
                         std::min(aYMax, aPYMax));
  }
  else
  {
    theReducedBox.Update(aXMin, aYMin, aXMax, aYMax);
  }
}

//! 1D function object for coordinate minimization/maximization along a 2D curve.
class Curv2dCoordFunc : public math_Function
{
public:
  Curv2dCoordFunc(const Adaptor2d_Curve2d& theCurve,
                  const double             theUMin,
                  const double             theUMax,
                  const int                theCoordIdx,
                  const double             theSign)
      : myCurve(theCurve),
        myUMin(theUMin),
        myUMax(theUMax),
        myCoordIdx(theCoordIdx),
        mySign(theSign)
  {
  }

  bool Value(const double theX, double& theF) override
  {
    if (theX < myUMin || theX > myUMax)
    {
      return false;
    }
    gp_Pnt2d aP = myCurve.Value(theX);
    theF         = mySign * aP.Coord(myCoordIdx);
    return true;
  }

private:
  Curv2dCoordFunc& operator=(const Curv2dCoordFunc&) = delete;

  const Adaptor2d_Curve2d& myCurve;
  double                   myUMin;
  double                   myUMax;
  int                      myCoordIdx;
  double                   mySign;
};

//! Multi-variable function for PSO on a 2D curve.
class Curv2dCoordMVarFunc : public math_MultipleVarFunction
{
public:
  Curv2dCoordMVarFunc(const Adaptor2d_Curve2d& theCurve,
                      const double             theUMin,
                      const double             theUMax,
                      const int                theCoordIdx,
                      const double             theSign)
      : myCurve(theCurve),
        myUMin(theUMin),
        myUMax(theUMax),
        myCoordIdx(theCoordIdx),
        mySign(theSign)
  {
  }

  bool Value(const math_Vector& theX, double& theF) override
  {
    if (theX(1) < myUMin || theX(1) > myUMax)
    {
      return false;
    }
    gp_Pnt2d aP = myCurve.Value(theX(1));
    theF         = mySign * aP.Coord(myCoordIdx);
    return true;
  }

  int NbVariables() const override { return 1; }

private:
  Curv2dCoordMVarFunc& operator=(const Curv2dCoordMVarFunc&) = delete;

  const Adaptor2d_Curve2d& myCurve;
  double                   myUMin;
  double                   myUMax;
  int                      myCoordIdx;
  double                   mySign;
};

//! Adjust coordinate extremum using PSO + Brent for 2D curve.
double AdjustExtr2dCurve(const Adaptor2d_Curve2d& theCurve,
                         const double             theUMin,
                         const double             theUMax,
                         const double             theExtr0,
                         const int                theCoordIdx,
                         const double             theTol,
                         const bool               theIsMin)
{
  double aSign = theIsMin ? 1. : -1.;
  double extr  = aSign * theExtr0;

  double uTol = std::max(theCurve.Resolution(theTol), Precision::PConfusion());
  double Du   = (theCurve.LastParameter() - theCurve.FirstParameter());

  double reltol = uTol / std::max(std::abs(theUMin), std::abs(theUMax));
  if (theUMax - theUMin < 0.01 * Du)
  {
    math_BrentMinimum anOptLoc(reltol, 100, uTol);
    Curv2dCoordFunc   aFunc(theCurve, theUMin, theUMax, theCoordIdx, aSign);
    anOptLoc.Perform(aFunc, theUMin, (theUMin + theUMax) / 2., theUMax);
    if (anOptLoc.IsDone())
    {
      extr = anOptLoc.Minimum();
      return aSign * extr;
    }
  }

  int         aNbParticles = std::max(8, RealToInt(32 * (theUMax - theUMin) / Du));
  double      aMaxStep     = (theUMax - theUMin) / (aNbParticles + 1);
  math_Vector aT(1, 1);
  math_Vector aLowBorder(1, 1);
  math_Vector aUppBorder(1, 1);
  math_Vector aSteps(1, 1);
  aLowBorder(1) = theUMin;
  aUppBorder(1) = theUMax;
  aSteps(1)     = std::min(0.1 * Du, aMaxStep);

  Curv2dCoordMVarFunc aFunc(theCurve, theUMin, theUMax, theCoordIdx, aSign);
  math_PSO            aFinder(&aFunc, aLowBorder, aUppBorder, aSteps, aNbParticles);
  aFinder.Perform(aSteps, extr, aT);

  math_BrentMinimum anOptLoc(reltol, 100, uTol);
  Curv2dCoordFunc   aFunc1(theCurve, theUMin, theUMax, theCoordIdx, aSign);
  anOptLoc.Perform(aFunc1,
                   std::max(aT(1) - aSteps(1), theUMin),
                   aT(1),
                   std::min(aT(1) + aSteps(1), theUMax));

  if (anOptLoc.IsDone())
  {
    extr = anOptLoc.Minimum();
    return aSign * extr;
  }

  return aSign * extr;
}

} // namespace

//=================================================================================================

Bnd_Box2d GeomBndLib_BSplineCurve2d::Box(double theTol) const
{
  return Box(myGeom->FirstParameter(), myGeom->LastParameter(), theTol);
}

//=================================================================================================

Bnd_Box2d GeomBndLib_BSplineCurve2d::Box(double theU1, double theU2, double theTol) const
{
  Bnd_Box2d aBox;
  constexpr double weakness = 1.5;

  occ::handle<Geom2d_BSplineCurve> aBs = myGeom;
  if (std::abs(aBs->FirstParameter() - theU1) > Precision::Parametric(theTol)
      || std::abs(aBs->LastParameter() - theU2) > Precision::Parametric(theTol))
  {
    occ::handle<Geom2d_Geometry>      G = aBs->Copy();
    occ::handle<Geom2d_BSplineCurve> Bsaux(occ::down_cast<Geom2d_BSplineCurve>(G));
    double                            u1 = theU1, u2 = theU2;
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

  Bnd_Box2d                        aB1;
  int                              k1 = aBs->FirstUKnotIndex(), k2 = aBs->LastUKnotIndex();
  int                              N  = aBs->Degree();
  const NCollection_Array1<double>& Knots = aBs->Knots();
  Geom2dAdaptor_Curve              aGACurve(aBs);
  double                           first = Knots(k1), last;
  double                           tol = 0.0;
  for (int k = k1 + 1; k <= k2; k++)
  {
    last  = Knots(k);
    tol   = std::max(FillBox2d(aB1, aGACurve, first, last, N), tol);
    first = last;
  }
  if (!aB1.IsVoid())
  {
    aB1.Enlarge(weakness * tol);
    ReduceSplineBox2d(myGeom, aB1, aBox);
    aBox.Enlarge(theTol);
  }
  return aBox;
}

//=================================================================================================

Bnd_Box2d GeomBndLib_BSplineCurve2d::BoxOptimal(double theU1,
                                                double theU2,
                                                double theTol) const
{
  Bnd_Box2d aBox;
  Geom2dAdaptor_Curve aGACurve(myGeom);
  int                 Nu = GeomBndLib_SamplingHelpers::ComputeNbSamples2d(aGACurve, theU1, theU2);

  double CoordMin[2] = {RealLast(), RealLast()};
  double CoordMax[2] = {-RealLast(), -RealLast()};
  double DeflMax[2]  = {-RealLast(), -RealLast()};

  gp_Pnt2d                   P;
  double                     du = (theU2 - theU1) / (Nu - 1), du2 = du / 2.;
  NCollection_Array1<gp_XY>  aPnts(1, Nu);
  double                     u;
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
        double cmin = AdjustExtr2dCurve(aGACurve, umin, umax, CMin, k + 1, eps, true);
        if (cmin < CMin)
          CMin = cmin;
      }
      else if (CMax - aPnts(i).Coord(k + 1) < d)
      {
        double umin = theU1 + std::max(0, i - 2) * du;
        double umax = theU1 + std::min(Nu - 1, i) * du;
        double cmax = AdjustExtr2dCurve(aGACurve, umin, umax, CMax, k + 1, eps, false);
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
