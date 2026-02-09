// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <Bnd_Box.hxx>
#include <BndLib.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <ElCLib.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <NCollection_Array1.hxx>
#include <math_Function.hxx>
#include <math_PSO.hxx>
#include <math_BrentMinimum.hxx>
//
static int NbSamples(const Adaptor3d_Curve& C, const double Umin, const double Umax);

static double AdjustExtr(const Adaptor3d_Curve& C,
                         const double           UMin,
                         const double           UMax,
                         const double           Extr0,
                         const int              CoordIndx,
                         const double           Tol,
                         const bool             IsMin);

//=======================================================================
// function : reduceSplineBox
// purpose  : This method intended to reduce box in case of
//           bezier and bspline curve.
//=======================================================================
static void reduceSplineBox(const Adaptor3d_Curve& theCurve,
                            const Bnd_Box&         theOrigBox,
                            Bnd_Box&               theReducedBox)
{
  // Guaranteed bounding box based on poles of bspline/bezier.
  Bnd_Box aPolesBox;

  if (theCurve.GetType() == GeomAbs_BSplineCurve)
  {
    occ::handle<Geom_BSplineCurve>    aC     = theCurve.BSpline();
    const NCollection_Array1<gp_Pnt>& aPoles = aC->Poles();
    for (int anIdx = aPoles.Lower(); anIdx <= aPoles.Upper(); ++anIdx)
    {
      aPolesBox.Add(aPoles.Value(anIdx));
    }
  }
  else if (theCurve.GetType() == GeomAbs_BezierCurve)
  {
    occ::handle<Geom_BezierCurve>     aC     = theCurve.Bezier();
    const NCollection_Array1<gp_Pnt>& aPoles = aC->Poles();
    for (int anIdx = aPoles.Lower(); anIdx <= aPoles.Upper(); ++anIdx)
    {
      aPolesBox.Add(aPoles.Value(anIdx));
    }
  }

  // Get original box limits using C++17 structured bindings.
  const auto [aXMin, aXMax, aYMin, aYMax, aZMin, aZMax] = theOrigBox.Get();

  // If poles box is valid, intersect with original box.
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

//=================================================================================================

void BndLib_Add3dCurve::Add(const Adaptor3d_Curve& C, const double Tol, Bnd_Box& B)
{
  BndLib_Add3dCurve::Add(C, C.FirstParameter(), C.LastParameter(), Tol, B);
}

// OCC566(apo)->
static double FillBox(Bnd_Box&               B,
                      const Adaptor3d_Curve& C,
                      const double           first,
                      const double           last,
                      const int              N)
{
  gp_Pnt P1, P2, P3;
  C.D0(first, P1);
  B.Add(P1);
  double p = first, dp = last - first, tol = 0.;
  if (std::abs(dp) > Precision::PConfusion())
  {
    int i;
    dp /= 2 * N;
    for (i = 1; i <= N; i++)
    {
      p += dp;
      C.D0(p, P2);
      B.Add(P2);
      p += dp;
      C.D0(p, P3);
      B.Add(P3);
      gp_Pnt Pc((P1.XYZ() + P3.XYZ()) / 2.0);
      tol = std::max(tol, Pc.Distance(P2));
      P1  = P3;
    }
  }
  else
  {
    C.D0(first, P1);
    B.Add(P1);
    C.D0(last, P3);
    B.Add(P3);
    tol = 0.;
  }
  return tol;
}

//<-OCC566(apo)
//=================================================================================================

void BndLib_Add3dCurve::Add(const Adaptor3d_Curve& C,
                            const double           U1,
                            const double           U2,
                            const double           Tol,
                            Bnd_Box&               B)
{
  constexpr double weakness = 1.5; // OCC566(apo)
  double           tol      = 0.0;
  switch (C.GetType())
  {

    case GeomAbs_Line: {
      BndLib::Add(C.Line(), U1, U2, Tol, B);
      break;
    }
    case GeomAbs_Circle: {
      BndLib::Add(C.Circle(), U1, U2, Tol, B);
      break;
    }
    case GeomAbs_Ellipse: {
      BndLib::Add(C.Ellipse(), U1, U2, Tol, B);
      break;
    }
    case GeomAbs_Hyperbola: {
      BndLib::Add(C.Hyperbola(), U1, U2, Tol, B);
      break;
    }
    case GeomAbs_Parabola: {
      BndLib::Add(C.Parabola(), U1, U2, Tol, B);
      break;
    }
    case GeomAbs_BezierCurve: {
      occ::handle<Geom_BezierCurve> Bz = C.Bezier();
      int                           N  = Bz->Degree();
      GeomAdaptor_Curve             GACurve(Bz);
      Bnd_Box                       B1;
      tol = FillBox(B1, GACurve, U1, U2, N);
      B1.Enlarge(weakness * tol);
      reduceSplineBox(C, B1, B);
      B.Enlarge(Tol);
      break;
    }
    case GeomAbs_BSplineCurve: {
      occ::handle<Geom_BSplineCurve> Bs = C.BSpline();
      if (std::abs(Bs->FirstParameter() - U1) > Precision::Parametric(Tol)
          || std::abs(Bs->LastParameter() - U2) > Precision::Parametric(Tol))
      {

        occ::handle<Geom_Geometry>     G = Bs->Copy();
        occ::handle<Geom_BSplineCurve> Bsaux(occ::down_cast<Geom_BSplineCurve>(G));
        double                         u1 = U1, u2 = U2;
        //// modified by jgv, 24.10.01 for BUC61031 ////
        if (Bsaux->IsPeriodic())
          ElCLib::AdjustPeriodic(Bsaux->FirstParameter(),
                                 Bsaux->LastParameter(),
                                 Precision::PConfusion(),
                                 u1,
                                 u2);
        else
        {
          ////////////////////////////////////////////////
          //  modified by NIZHNY-EAP Fri Dec  3 14:29:14 1999 ___BEGIN___
          // To avoid exception in Segment
          if (Bsaux->FirstParameter() > U1)
            u1 = Bsaux->FirstParameter();
          if (Bsaux->LastParameter() < U2)
            u2 = Bsaux->LastParameter();
          //  modified by NIZHNY-EAP Fri Dec  3 14:29:18 1999 ___END___
        }
        double aSegmentTol = 2. * Precision::PConfusion();

        // For periodic curves, check if parameters are close in either direction
        if (Bsaux->IsPeriodic())
        {
          const double aPeriod = Bsaux->LastParameter() - Bsaux->FirstParameter();

          // Check direct distance between parameters
          const double aDirectDiff = std::abs(u2 - u1);

          // Check distances across period boundary (in both directions)
          const double aCrossPeriodDiff1 = std::abs(u2 - aPeriod - u1);
          const double aCrossPeriodDiff2 = std::abs(u1 - aPeriod - u2);

          // Find the minimum difference (closest approach)
          const double aMinDiff =
            std::min(aDirectDiff, std::min(aCrossPeriodDiff1, aCrossPeriodDiff2));

          if (aMinDiff < aSegmentTol)
          {
            aSegmentTol = aMinDiff * 0.01;
          }
        }
        // For non-periodic curves, just check direct parameter difference
        else if (std::abs(u2 - u1) < aSegmentTol)
        {
          aSegmentTol = std::abs(u2 - u1) * 0.01;
        }
        Bsaux->Segment(u1, u2, aSegmentTol);
        Bs = Bsaux;
      }
      // OCC566(apo)->
      Bnd_Box B1;
      int                               k, k1 = Bs->FirstUKnotIndex(), k2 = Bs->LastUKnotIndex(),
                                        N = Bs->Degree();
      const NCollection_Array1<double>& Knots = Bs->Knots();
      GeomAdaptor_Curve GACurve(Bs);
      double            first = Knots(k1), last;
      for (k = k1 + 1; k <= k2; k++)
      {
        last  = Knots(k);
        tol   = std::max(FillBox(B1, GACurve, first, last, N), tol);
        first = last;
      }
      if (!B1.IsVoid())
      {
        B1.Enlarge(weakness * tol);
        reduceSplineBox(C, B1, B);
        B.Enlarge(Tol);
      }
      //<-OCC566(apo)
      break;
    }
    default: {
      Bnd_Box       B1;
      constexpr int N = 33;
      tol             = FillBox(B1, C, U1, U2, N);
      B1.Enlarge(weakness * tol);
      double x, y, z, X, Y, Z;
      B1.Get(x, y, z, X, Y, Z);
      B.Update(x, y, z, X, Y, Z);
      B.Enlarge(Tol);
    }
  }
}

//=================================================================================================

void BndLib_Add3dCurve::AddOptimal(const Adaptor3d_Curve& C, const double Tol, Bnd_Box& B)
{
  BndLib_Add3dCurve::AddOptimal(C, C.FirstParameter(), C.LastParameter(), Tol, B);
}

//=================================================================================================

void BndLib_Add3dCurve::AddOptimal(const Adaptor3d_Curve& C,
                                   const double           U1,
                                   const double           U2,
                                   const double           Tol,
                                   Bnd_Box&               B)
{
  switch (C.GetType())
  {

    case GeomAbs_Line: {
      BndLib::Add(C.Line(), U1, U2, Tol, B);
      break;
    }
    case GeomAbs_Circle: {
      BndLib::Add(C.Circle(), U1, U2, Tol, B);
      break;
    }
    case GeomAbs_Ellipse: {
      BndLib::Add(C.Ellipse(), U1, U2, Tol, B);
      break;
    }
    case GeomAbs_Hyperbola: {
      BndLib::Add(C.Hyperbola(), U1, U2, Tol, B);
      break;
    }
    case GeomAbs_Parabola: {
      BndLib::Add(C.Parabola(), U1, U2, Tol, B);
      break;
    }
    default: {
      AddGenCurv(C, U1, U2, Tol, B);
    }
  }
}

//=================================================================================================

void BndLib_Add3dCurve::AddGenCurv(const Adaptor3d_Curve& C,
                                   const double           UMin,
                                   const double           UMax,
                                   const double           Tol,
                                   Bnd_Box&               B)
{
  int Nu = NbSamples(C, UMin, UMax);
  //
  double CoordMin[3] = {RealLast(), RealLast(), RealLast()};
  double CoordMax[3] = {-RealLast(), -RealLast(), -RealLast()};
  double DeflMax[3]  = {-RealLast(), -RealLast(), -RealLast()};
  //
  gp_Pnt                     P;
  int                        i, k;
  double                     du = (UMax - UMin) / (Nu - 1), du2 = du / 2.;
  NCollection_Array1<gp_XYZ> aPnts(1, Nu);
  double                     u;
  for (i = 1, u = UMin; i <= Nu; i++, u += du)
  {
    C.D0(u, P);
    aPnts(i) = P.XYZ();
    //
    for (k = 0; k < 3; ++k)
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
    //
    if (i > 1)
    {
      gp_XYZ aPm = 0.5 * (aPnts(i - 1) + aPnts(i));
      C.D0(u - du2, P);
      gp_XYZ aD = (P.XYZ() - aPm);
      for (k = 0; k < 3; ++k)
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
  //
  // Adjusting minmax
  double eps = std::max(Tol, Precision::Confusion());
  for (k = 0; k < 3; ++k)
  {
    double d = DeflMax[k];
    if (d <= eps)
    {
      continue;
    }
    double CMin = CoordMin[k];
    double CMax = CoordMax[k];
    for (i = 1; i <= Nu; ++i)
    {
      if (aPnts(i).Coord(k + 1) - CMin < d)
      {
        double umin, umax;
        umin        = UMin + std::max(0, i - 2) * du;
        umax        = UMin + std::min(Nu - 1, i) * du;
        double cmin = AdjustExtr(C, umin, umax, CMin, k + 1, eps, true);
        if (cmin < CMin)
        {
          CMin = cmin;
        }
      }
      else if (CMax - aPnts(i).Coord(k + 1) < d)
      {
        double umin, umax;
        umin        = UMin + std::max(0, i - 2) * du;
        umax        = UMin + std::min(Nu - 1, i) * du;
        double cmax = AdjustExtr(C, umin, umax, CMax, k + 1, eps, false);
        if (cmax > CMax)
        {
          CMax = cmax;
        }
      }
    }
    CoordMin[k] = CMin;
    CoordMax[k] = CMax;
  }

  B.Add(gp_Pnt(CoordMin[0], CoordMin[1], CoordMin[2]));
  B.Add(gp_Pnt(CoordMax[0], CoordMax[1], CoordMax[2]));
  B.Enlarge(eps);
}

//
class CurvMaxMinCoordMVar : public math_MultipleVarFunction
{
public:
  CurvMaxMinCoordMVar(const Adaptor3d_Curve& theCurve,
                      const double           UMin,
                      const double           UMax,
                      const int              CoordIndx,
                      const double           Sign)
      : myCurve(theCurve),
        myUMin(UMin),
        myUMax(UMax),
        myCoordIndx(CoordIndx),
        mySign(Sign)
  {
  }

  bool Value(const math_Vector& X, double& F) override
  {
    if (!CheckInputData(X(1)))
    {
      return false;
    }
    gp_Pnt aP = myCurve.Value(X(1));

    F = mySign * aP.Coord(myCoordIndx);

    return true;
  }

  int NbVariables() const override { return 1; }

private:
  CurvMaxMinCoordMVar& operator=(const CurvMaxMinCoordMVar&) = delete;

  bool CheckInputData(double theParam) { return theParam >= myUMin && theParam <= myUMax; }

  const Adaptor3d_Curve& myCurve;
  double                 myUMin;
  double                 myUMax;
  int                    myCoordIndx;
  double                 mySign;
};

//
class CurvMaxMinCoord : public math_Function
{
public:
  CurvMaxMinCoord(const Adaptor3d_Curve& theCurve,
                  const double           UMin,
                  const double           UMax,
                  const int              CoordIndx,
                  const double           Sign)
      : myCurve(theCurve),
        myUMin(UMin),
        myUMax(UMax),
        myCoordIndx(CoordIndx),
        mySign(Sign)
  {
  }

  bool Value(const double X, double& F) override
  {
    if (!CheckInputData(X))
    {
      return false;
    }
    gp_Pnt aP = myCurve.Value(X);

    F = mySign * aP.Coord(myCoordIndx);

    return true;
  }

private:
  CurvMaxMinCoord& operator=(const CurvMaxMinCoord&) = delete;

  bool CheckInputData(double theParam) { return theParam >= myUMin && theParam <= myUMax; }

  const Adaptor3d_Curve& myCurve;
  double                 myUMin;
  double                 myUMax;
  int                    myCoordIndx;
  double                 mySign;
};

//=================================================================================================

double AdjustExtr(const Adaptor3d_Curve& C,
                  const double           UMin,
                  const double           UMax,
                  const double           Extr0,
                  const int              CoordIndx,
                  const double           Tol,
                  const bool             IsMin)
{
  double aSign = IsMin ? 1. : -1.;
  double extr  = aSign * Extr0;
  //
  double uTol = std::max(C.Resolution(Tol), Precision::PConfusion());
  double Du   = (C.LastParameter() - C.FirstParameter());
  //
  double reltol = uTol / std::max(std::abs(UMin), std::abs(UMax));
  if (UMax - UMin < 0.01 * Du)
  {

    math_BrentMinimum anOptLoc(reltol, 100, uTol);
    CurvMaxMinCoord   aFunc(C, UMin, UMax, CoordIndx, aSign);
    anOptLoc.Perform(aFunc, UMin, (UMin + UMax) / 2., UMax);
    if (anOptLoc.IsDone())
    {
      extr = anOptLoc.Minimum();
      return aSign * extr;
    }
  }
  //
  int         aNbParticles = std::max(8, RealToInt(32 * (UMax - UMin) / Du));
  double      maxstep      = (UMax - UMin) / (aNbParticles + 1);
  math_Vector aT(1, 1);
  math_Vector aLowBorder(1, 1);
  math_Vector aUppBorder(1, 1);
  math_Vector aSteps(1, 1);
  aLowBorder(1) = UMin;
  aUppBorder(1) = UMax;
  aSteps(1)     = std::min(0.1 * Du, maxstep);

  CurvMaxMinCoordMVar aFunc(C, UMin, UMax, CoordIndx, aSign);
  math_PSO            aFinder(&aFunc, aLowBorder, aUppBorder, aSteps, aNbParticles);
  aFinder.Perform(aSteps, extr, aT);
  //
  math_BrentMinimum anOptLoc(reltol, 100, uTol);
  CurvMaxMinCoord   aFunc1(C, UMin, UMax, CoordIndx, aSign);
  anOptLoc.Perform(aFunc1,
                   std::max(aT(1) - aSteps(1), UMin),
                   aT(1),
                   std::min(aT(1) + aSteps(1), UMax));

  if (anOptLoc.IsDone())
  {
    extr = anOptLoc.Minimum();
    return aSign * extr;
  }

  return aSign * extr;
}

//=================================================================================================

int NbSamples(const Adaptor3d_Curve& C, const double Umin, const double Umax)
{
  int               N;
  GeomAbs_CurveType Type = C.GetType();
  switch (Type)
  {
    case GeomAbs_BezierCurve: {
      N = 2 * C.NbPoles();
      // By default parametric range of Bezier curv is [0, 1]
      double du = Umax - Umin;
      if (du < .9)
      {
        N = RealToInt(du * N) + 1;
        N = std::max(N, 5);
      }
      break;
    }
    case GeomAbs_BSplineCurve: {
      const occ::handle<Geom_BSplineCurve>& BC = C.BSpline();
      N                                        = 2 * (BC->Degree() + 1) * (BC->NbKnots() - 1);
      double umin = BC->FirstParameter(), umax = BC->LastParameter();
      double du = (Umax - Umin) / (umax - umin);
      if (du < .9)
      {
        N = RealToInt(du * N) + 1;
        N = std::max(N, 5);
      }
      break;
    }
    default:
      N = 33;
  }
  return std::min(500, N);
}
