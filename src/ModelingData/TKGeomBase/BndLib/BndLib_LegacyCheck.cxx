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

// Temporary debugging facility. Contains verbatim copies of the pre-migration
// BndLib bounding-box algorithms so they can be compared against the new
// GeomBndLib-delegating versions at runtime.

#include "BndLib_LegacyCheck.hxx"

#include <typeinfo>

// --- 3D curve old implementation headers ---
#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_Surface.hxx>
#include <Bnd_Box.hxx>
#include <Bnd_Box2d.hxx>
#include <BndLib.hxx>
#include <BSplCLib.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Conic.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Geometry.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomGridEval_Surface.hxx>
#include <gp.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <math_BrentMinimum.hxx>
#include <math_Function.hxx>
#include <math_Powell.hxx>
#include <math_PSO.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>

#include <cmath>
#include <iostream>

namespace
{

//=============================================================================
// Type name helpers
//=============================================================================

static const char* curveTypeName(GeomAbs_CurveType theType)
{
  switch (theType)
  {
    case GeomAbs_Line:
      return "Line";
    case GeomAbs_Circle:
      return "Circle";
    case GeomAbs_Ellipse:
      return "Ellipse";
    case GeomAbs_Hyperbola:
      return "Hyperbola";
    case GeomAbs_Parabola:
      return "Parabola";
    case GeomAbs_BezierCurve:
      return "BezierCurve";
    case GeomAbs_BSplineCurve:
      return "BSplineCurve";
    case GeomAbs_OffsetCurve:
      return "OffsetCurve";
    case GeomAbs_OtherCurve:
      return "OtherCurve";
    default:
      return "Unknown";
  }
}

static const char* surfaceTypeName(GeomAbs_SurfaceType theType)
{
  switch (theType)
  {
    case GeomAbs_Plane:
      return "Plane";
    case GeomAbs_Cylinder:
      return "Cylinder";
    case GeomAbs_Cone:
      return "Cone";
    case GeomAbs_Sphere:
      return "Sphere";
    case GeomAbs_Torus:
      return "Torus";
    case GeomAbs_BezierSurface:
      return "BezierSurface";
    case GeomAbs_BSplineSurface:
      return "BSplineSurface";
    case GeomAbs_SurfaceOfRevolution:
      return "SurfaceOfRevolution";
    case GeomAbs_SurfaceOfExtrusion:
      return "SurfaceOfExtrusion";
    case GeomAbs_OffsetSurface:
      return "OffsetSurface";
    case GeomAbs_OtherSurface:
      return "OtherSurface";
    default:
      return "Unknown";
  }
}

//=============================================================================
// Helpers shared across curve/surface comparisons
//=============================================================================

static bool boxesEqual(const Bnd_Box& theA, const Bnd_Box& theB, double theTol = 1.e-7)
{
  if (theA.IsVoid() != theB.IsVoid())
    return false;
  if (theA.IsWhole() != theB.IsWhole())
    return false;
  if (theA.IsVoid())
    return true;
  if (theA.IsWhole())
    return true;
  double xMin1, yMin1, zMin1, xMax1, yMax1, zMax1;
  double xMin2, yMin2, zMin2, xMax2, yMax2, zMax2;
  theA.Get(xMin1, yMin1, zMin1, xMax1, yMax1, zMax1);
  theB.Get(xMin2, yMin2, zMin2, xMax2, yMax2, zMax2);
  return std::abs(xMin1 - xMin2) <= theTol && std::abs(yMin1 - yMin2) <= theTol
         && std::abs(zMin1 - zMin2) <= theTol && std::abs(xMax1 - xMax2) <= theTol
         && std::abs(yMax1 - yMax2) <= theTol && std::abs(zMax1 - zMax2) <= theTol;
}

static bool boxesEqual2d(const Bnd_Box2d& theA, const Bnd_Box2d& theB, double theTol = 1.e-7)
{
  if (theA.IsVoid() != theB.IsVoid())
    return false;
  if (theA.IsVoid())
    return true;
  double xMin1, yMin1, xMax1, yMax1;
  double xMin2, yMin2, xMax2, yMax2;
  theA.Get(xMin1, yMin1, xMax1, yMax1);
  theB.Get(xMin2, yMin2, xMax2, yMax2);
  return std::abs(xMin1 - xMin2) <= theTol && std::abs(yMin1 - yMin2) <= theTol
         && std::abs(xMax1 - xMax2) <= theTol && std::abs(yMax1 - yMax2) <= theTol;
}

static void printBox(const char* thePrefix, const Bnd_Box& theBox)
{
  if (theBox.IsVoid())
  {
    std::cerr << thePrefix << " VOID\n";
    return;
  }
  if (theBox.IsWhole())
  {
    std::cerr << thePrefix << " WHOLE\n";
    return;
  }
  double xMin, yMin, zMin, xMax, yMax, zMax;
  theBox.Get(xMin, yMin, zMin, xMax, yMax, zMax);
  std::cerr << thePrefix << " [" << xMin << ", " << yMin << ", " << zMin << "] - [" << xMax << ", "
            << yMax << ", " << zMax << "]\n";
}

static void printBox2d(const char* thePrefix, const Bnd_Box2d& theBox)
{
  if (theBox.IsVoid())
  {
    std::cerr << thePrefix << " VOID\n";
    return;
  }
  double xMin, yMin, xMax, yMax;
  theBox.Get(xMin, yMin, xMax, yMax);
  std::cerr << thePrefix << " [" << xMin << ", " << yMin << "] - [" << xMax << ", " << yMax
            << "]\n";
}

//=============================================================================
// OLD BndLib_Add3dCurve implementation
//=============================================================================

static double LegacyFillBox3d(Bnd_Box&               B,
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
    dp /= 2 * N;
    for (int i = 1; i <= N; i++)
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

static void LegacyReduceSplineBox3d(const Adaptor3d_Curve& theCurve,
                                    const Bnd_Box&         theOrigBox,
                                    Bnd_Box&               theReducedBox)
{
  Bnd_Box aPolesBox;
  if (theCurve.GetType() == GeomAbs_BSplineCurve)
  {
    occ::handle<Geom_BSplineCurve>    aC     = theCurve.BSpline();
    const NCollection_Array1<gp_Pnt>& aPoles = aC->Poles();
    for (int anIdx = aPoles.Lower(); anIdx <= aPoles.Upper(); ++anIdx)
      aPolesBox.Add(aPoles.Value(anIdx));
  }
  else if (theCurve.GetType() == GeomAbs_BezierCurve)
  {
    occ::handle<Geom_BezierCurve>     aC     = theCurve.Bezier();
    const NCollection_Array1<gp_Pnt>& aPoles = aC->Poles();
    for (int anIdx = aPoles.Lower(); anIdx <= aPoles.Upper(); ++anIdx)
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

// forward declaration
static double LegacyAdjustExtr3d(const Adaptor3d_Curve& C,
                                 double                 UMin,
                                 double                 UMax,
                                 double                 Extr0,
                                 int                    CoordIndx,
                                 double                 Tol,
                                 bool                   IsMin);

static int LegacyNbSamples3d(const Adaptor3d_Curve& C, const double Umin, const double Umax)
{
  int               N;
  GeomAbs_CurveType Type = C.GetType();
  switch (Type)
  {
    case GeomAbs_BezierCurve: {
      N         = 2 * C.NbPoles();
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

class Legacy3dCurvMaxMinCoordMVar : public math_MultipleVarFunction
{
public:
  Legacy3dCurvMaxMinCoordMVar(const Adaptor3d_Curve& theCurve,
                              double                 UMin,
                              double                 UMax,
                              int                    CoordIndx,
                              double                 Sign)
      : myCurve(theCurve),
        myUMin(UMin),
        myUMax(UMax),
        myCoordIndx(CoordIndx),
        mySign(Sign)
  {
  }

  bool Value(const math_Vector& X, double& F) override
  {
    if (X(1) < myUMin || X(1) > myUMax)
      return false;
    gp_Pnt aP = myCurve.Value(X(1));
    F         = mySign * aP.Coord(myCoordIndx);
    return true;
  }

  int NbVariables() const override { return 1; }

private:
  Legacy3dCurvMaxMinCoordMVar& operator=(const Legacy3dCurvMaxMinCoordMVar&) = delete;
  const Adaptor3d_Curve&       myCurve;
  double                       myUMin, myUMax;
  int                          myCoordIndx;
  double                       mySign;
};

class Legacy3dCurvMaxMinCoord : public math_Function
{
public:
  Legacy3dCurvMaxMinCoord(const Adaptor3d_Curve& theCurve,
                          double                 UMin,
                          double                 UMax,
                          int                    CoordIndx,
                          double                 Sign)
      : myCurve(theCurve),
        myUMin(UMin),
        myUMax(UMax),
        myCoordIndx(CoordIndx),
        mySign(Sign)
  {
  }

  bool Value(const double X, double& F) override
  {
    if (X < myUMin || X > myUMax)
      return false;
    gp_Pnt aP = myCurve.Value(X);
    F         = mySign * aP.Coord(myCoordIndx);
    return true;
  }

private:
  Legacy3dCurvMaxMinCoord& operator=(const Legacy3dCurvMaxMinCoord&) = delete;
  const Adaptor3d_Curve&   myCurve;
  double                   myUMin, myUMax;
  int                      myCoordIndx;
  double                   mySign;
};

static double LegacyAdjustExtr3d(const Adaptor3d_Curve& C,
                                 double                 UMin,
                                 double                 UMax,
                                 double                 Extr0,
                                 int                    CoordIndx,
                                 double                 Tol,
                                 bool                   IsMin)
{
  double aSign  = IsMin ? 1. : -1.;
  double extr   = aSign * Extr0;
  double uTol   = std::max(C.Resolution(Tol), Precision::PConfusion());
  double Du     = (C.LastParameter() - C.FirstParameter());
  double reltol = uTol / std::max(std::abs(UMin), std::abs(UMax));
  if (UMax - UMin < 0.01 * Du)
  {
    math_BrentMinimum       anOptLoc(reltol, 100, uTol);
    Legacy3dCurvMaxMinCoord aFunc(C, UMin, UMax, CoordIndx, aSign);
    anOptLoc.Perform(aFunc, UMin, (UMin + UMax) / 2., UMax);
    if (anOptLoc.IsDone())
      return aSign * anOptLoc.Minimum();
  }
  int         aNbParticles = std::max(8, RealToInt(32 * (UMax - UMin) / Du));
  double      maxstep      = (UMax - UMin) / (aNbParticles + 1);
  math_Vector aT(1, 1), aLowBorder(1, 1), aUppBorder(1, 1), aSteps(1, 1);
  aLowBorder(1) = UMin;
  aUppBorder(1) = UMax;
  aSteps(1)     = std::min(0.1 * Du, maxstep);
  Legacy3dCurvMaxMinCoordMVar aFunc(C, UMin, UMax, CoordIndx, aSign);
  math_PSO                    aFinder(&aFunc, aLowBorder, aUppBorder, aSteps, aNbParticles);
  aFinder.Perform(aSteps, extr, aT);
  math_BrentMinimum       anOptLoc(reltol, 100, uTol);
  Legacy3dCurvMaxMinCoord aFunc1(C, UMin, UMax, CoordIndx, aSign);
  anOptLoc.Perform(aFunc1,
                   std::max(aT(1) - aSteps(1), UMin),
                   aT(1),
                   std::min(aT(1) + aSteps(1), UMax));
  if (anOptLoc.IsDone())
    extr = anOptLoc.Minimum();
  return aSign * extr;
}

static void LegacyAddGenCurv3d(const Adaptor3d_Curve& C,
                               double                 UMin,
                               double                 UMax,
                               double                 Tol,
                               Bnd_Box&               B)
{
  int                        Nu          = LegacyNbSamples3d(C, UMin, UMax);
  double                     CoordMin[3] = {RealLast(), RealLast(), RealLast()};
  double                     CoordMax[3] = {-RealLast(), -RealLast(), -RealLast()};
  double                     DeflMax[3]  = {-RealLast(), -RealLast(), -RealLast()};
  gp_Pnt                     P;
  double                     du = (UMax - UMin) / (Nu - 1), du2 = du / 2.;
  NCollection_Array1<gp_XYZ> aPnts(1, Nu);
  double                     u;
  for (int i = 1; i <= Nu; i++, u += du)
  {
    if (i == 1)
      u = UMin;
    C.D0(u, P);
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
      C.D0(u - du2, P);
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
  double eps = std::max(Tol, Precision::Confusion());
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
        double umin = UMin + std::max(0, i - 2) * du;
        double umax = UMin + std::min(Nu - 1, i) * du;
        double cmin = LegacyAdjustExtr3d(C, umin, umax, CMin, k + 1, eps, true);
        if (cmin < CMin)
          CMin = cmin;
      }
      else if (CMax - aPnts(i).Coord(k + 1) < d)
      {
        double umin = UMin + std::max(0, i - 2) * du;
        double umax = UMin + std::min(Nu - 1, i) * du;
        double cmax = LegacyAdjustExtr3d(C, umin, umax, CMax, k + 1, eps, false);
        if (cmax > CMax)
          CMax = cmax;
      }
    }
    CoordMin[k] = CMin;
    CoordMax[k] = CMax;
  }
  B.Add(gp_Pnt(CoordMin[0], CoordMin[1], CoordMin[2]));
  B.Add(gp_Pnt(CoordMax[0], CoordMax[1], CoordMax[2]));
  B.Enlarge(eps);
}

static Bnd_Box LegacyAdd3dCurveImpl(const Adaptor3d_Curve& C, double U1, double U2, double Tol)
{
  constexpr double weakness = 1.5;
  double           tol      = 0.0;
  Bnd_Box          B;
  switch (C.GetType())
  {
    case GeomAbs_Line:
      BndLib::Add(C.Line(), U1, U2, Tol, B);
      break;
    case GeomAbs_Circle:
      BndLib::Add(C.Circle(), U1, U2, Tol, B);
      break;
    case GeomAbs_Ellipse:
      BndLib::Add(C.Ellipse(), U1, U2, Tol, B);
      break;
    case GeomAbs_Hyperbola:
      BndLib::Add(C.Hyperbola(), U1, U2, Tol, B);
      break;
    case GeomAbs_Parabola:
      BndLib::Add(C.Parabola(), U1, U2, Tol, B);
      break;
    case GeomAbs_BezierCurve: {
      occ::handle<Geom_BezierCurve> Bz = C.Bezier();
      int                           N  = Bz->Degree();
      GeomAdaptor_Curve             GACurve(Bz);
      Bnd_Box                       B1;
      tol = LegacyFillBox3d(B1, GACurve, U1, U2, N);
      B1.Enlarge(weakness * tol);
      LegacyReduceSplineBox3d(C, B1, B);
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
        if (Bsaux->IsPeriodic())
          ElCLib::AdjustPeriodic(Bsaux->FirstParameter(),
                                 Bsaux->LastParameter(),
                                 Precision::PConfusion(),
                                 u1,
                                 u2);
        else
        {
          if (Bsaux->FirstParameter() > U1)
            u1 = Bsaux->FirstParameter();
          if (Bsaux->LastParameter() < U2)
            u2 = Bsaux->LastParameter();
        }
        double aSegmentTol = 2. * Precision::PConfusion();
        if (Bsaux->IsPeriodic())
        {
          const double aPeriod           = Bsaux->LastParameter() - Bsaux->FirstParameter();
          const double aDirectDiff       = std::abs(u2 - u1);
          const double aCrossPeriodDiff1 = std::abs(u2 - aPeriod - u1);
          const double aCrossPeriodDiff2 = std::abs(u1 - aPeriod - u2);
          const double aMinDiff =
            std::min(aDirectDiff, std::min(aCrossPeriodDiff1, aCrossPeriodDiff2));
          if (aMinDiff < aSegmentTol)
            aSegmentTol = aMinDiff * 0.01;
        }
        else if (std::abs(u2 - u1) < aSegmentTol)
          aSegmentTol = std::abs(u2 - u1) * 0.01;
        Bsaux->Segment(u1, u2, aSegmentTol);
        Bs = Bsaux;
      }
      Bnd_Box B1;
      int     k, k1 = Bs->FirstUKnotIndex(), k2 = Bs->LastUKnotIndex(), N = Bs->Degree();
      const NCollection_Array1<double>& Knots = Bs->Knots();
      GeomAdaptor_Curve                 GACurve(Bs);
      double                            first = Knots(k1), last;
      for (k = k1 + 1; k <= k2; k++)
      {
        last  = Knots(k);
        tol   = std::max(LegacyFillBox3d(B1, GACurve, first, last, N), tol);
        first = last;
      }
      if (!B1.IsVoid())
      {
        B1.Enlarge(weakness * tol);
        LegacyReduceSplineBox3d(C, B1, B);
        B.Enlarge(Tol);
      }
      break;
    }
    default: {
      Bnd_Box       B1;
      constexpr int N = 33;
      tol             = LegacyFillBox3d(B1, C, U1, U2, N);
      B1.Enlarge(weakness * tol);
      double x, y, z, X, Y, Z;
      B1.Get(x, y, z, X, Y, Z);
      B.Update(x, y, z, X, Y, Z);
      B.Enlarge(Tol);
    }
  }
  return B;
}

static Bnd_Box LegacyAddOptimal3dCurveImpl(const Adaptor3d_Curve& C,
                                           double                 U1,
                                           double                 U2,
                                           double                 Tol)
{
  Bnd_Box B;
  switch (C.GetType())
  {
    case GeomAbs_Line:
      BndLib::Add(C.Line(), U1, U2, Tol, B);
      break;
    case GeomAbs_Circle:
      BndLib::Add(C.Circle(), U1, U2, Tol, B);
      break;
    case GeomAbs_Ellipse:
      BndLib::Add(C.Ellipse(), U1, U2, Tol, B);
      break;
    case GeomAbs_Hyperbola:
      BndLib::Add(C.Hyperbola(), U1, U2, Tol, B);
      break;
    case GeomAbs_Parabola:
      BndLib::Add(C.Parabola(), U1, U2, Tol, B);
      break;
    default:
      LegacyAddGenCurv3d(C, U1, U2, Tol, B);
  }
  return B;
}

//=============================================================================
// OLD BndLib_AddSurface implementation
//=============================================================================

static int LegacyNbUSamples(const Adaptor3d_Surface& S)
{
  int N;
  switch (S.GetType())
  {
    case GeomAbs_BezierSurface:
      N = 2 * S.NbUPoles();
      break;
    case GeomAbs_BSplineSurface:
      N = 2 * (S.BSpline()->UDegree() + 1) * (S.BSpline()->NbUKnots() - 1);
      break;
    default:
      N = 33;
  }
  return std::min(50, N);
}

static int LegacyNbVSamples(const Adaptor3d_Surface& S)
{
  int N;
  switch (S.GetType())
  {
    case GeomAbs_BezierSurface:
      N = 2 * S.NbVPoles();
      break;
    case GeomAbs_BSplineSurface:
      N = 2 * (S.BSpline()->VDegree() + 1) * (S.BSpline()->NbVKnots() - 1);
      break;
    default:
      N = 33;
  }
  return std::min(50, N);
}

static int LegacyNbUSampleRange(const Adaptor3d_Surface& S, const double Umin, const double Umax)
{
  int N;
  switch (S.GetType())
  {
    case GeomAbs_BezierSurface: {
      N         = 2 * S.NbUPoles();
      double du = Umax - Umin;
      if (du < .9)
      {
        N = RealToInt(du * N) + 1;
        N = std::max(N, 5);
      }
      break;
    }
    case GeomAbs_BSplineSurface: {
      const occ::handle<Geom_BSplineSurface>& BS = S.BSpline();
      N                                          = 2 * (BS->UDegree() + 1) * (BS->NbUKnots() - 1);
      double umin, umax, vmin, vmax;
      BS->Bounds(umin, umax, vmin, vmax);
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
  return std::min(50, N);
}

static int LegacyNbVSampleRange(const Adaptor3d_Surface& S, const double Vmin, const double Vmax)
{
  int N;
  switch (S.GetType())
  {
    case GeomAbs_BezierSurface: {
      N         = 2 * S.NbVPoles();
      double dv = Vmax - Vmin;
      if (dv < .9)
      {
        N = RealToInt(dv * N) + 1;
        N = std::max(N, 5);
      }
      break;
    }
    case GeomAbs_BSplineSurface: {
      const occ::handle<Geom_BSplineSurface>& BS = S.BSpline();
      N                                          = 2 * (BS->VDegree() + 1) * (BS->NbVKnots() - 1);
      double umin, umax, vmin, vmax;
      BS->Bounds(umin, umax, vmin, vmax);
      double dv = (Vmax - Vmin) / (vmax - vmin);
      if (dv < .9)
      {
        N = RealToInt(dv * N) + 1;
        N = std::max(N, 5);
      }
      break;
    }
    default:
      N = 33;
  }
  return std::min(50, N);
}

static void LegacyComputePolesIndexes(const NCollection_Array1<double>& theKnots,
                                      const NCollection_Array1<int>&    theMults,
                                      const int                         theDegree,
                                      const double                      theMin,
                                      const double                      theMax,
                                      const int                         theMaxPoleIdx,
                                      const bool                        theIsPeriodic,
                                      int&                              theOutMinIdx,
                                      int&                              theOutMaxIdx)
{
  BSplCLib::Hunt(theKnots, theMin, theOutMinIdx);
  theOutMinIdx = std::clamp(theOutMinIdx, theKnots.Lower(), theKnots.Upper());
  BSplCLib::Hunt(theKnots, theMax, theOutMaxIdx);
  theOutMaxIdx++;
  theOutMaxIdx          = std::clamp(theOutMaxIdx, theKnots.Lower(), theKnots.Upper());
  const int aMultiplier = theMults(theOutMaxIdx);
  theOutMinIdx          = BSplCLib::PoleIndex(theDegree, theOutMinIdx, theIsPeriodic, theMults) + 1;
  theOutMinIdx          = std::max(theOutMinIdx, 1);
  theOutMaxIdx          = BSplCLib::PoleIndex(theDegree, theOutMaxIdx, theIsPeriodic, theMults) + 1;
  theOutMaxIdx += theDegree - aMultiplier;
  if (!theIsPeriodic)
    theOutMaxIdx = std::min(theOutMaxIdx, theMaxPoleIdx);
}

static gp_Pnt LegacyBaryCenter(const gp_Pln& aPlane,
                               const double  aUMin,
                               const double  aUMax,
                               const double  aVMin,
                               const double  aVMax)
{
  double aU, aV;
  bool   isU1Inf = Precision::IsInfinite(aUMin);
  bool   isU2Inf = Precision::IsInfinite(aUMax);
  bool   isV1Inf = Precision::IsInfinite(aVMin);
  bool   isV2Inf = Precision::IsInfinite(aVMax);
  if (isU1Inf && isU2Inf)
    aU = 0;
  else if (isU1Inf)
    aU = aUMax - 10.;
  else if (isU2Inf)
    aU = aUMin + 10.;
  else
    aU = (aUMin + aUMax) / 2.;
  if (isV1Inf && isV2Inf)
    aV = 0;
  else if (isV1Inf)
    aV = aVMax - 10.;
  else if (isV2Inf)
    aV = aVMin + 10.;
  else
    aV = (aVMin + aVMax) / 2.;
  return ElSLib::Value(aU, aV, aPlane);
}

static void LegacyTreatInfinitePlane(const gp_Pln& aPlane,
                                     const double  aUMin,
                                     const double  aUMax,
                                     const double  aVMin,
                                     const double  aVMax,
                                     const double  aTol,
                                     Bnd_Box&      aB)
{
  const gp_Dir&    aNorm        = aPlane.Axis().Direction();
  constexpr double anAngularTol = RealEpsilon();
  gp_Pnt           aLocation    = LegacyBaryCenter(aPlane, aUMin, aUMax, aVMin, aVMax);
  if (aNorm.IsParallel(gp::DX(), anAngularTol))
  {
    aB.Add(aLocation);
    aB.OpenYmin();
    aB.OpenYmax();
    aB.OpenZmin();
    aB.OpenZmax();
  }
  else if (aNorm.IsParallel(gp::DY(), anAngularTol))
  {
    aB.Add(aLocation);
    aB.OpenXmin();
    aB.OpenXmax();
    aB.OpenZmin();
    aB.OpenZmax();
  }
  else if (aNorm.IsParallel(gp::DZ(), anAngularTol))
  {
    aB.Add(aLocation);
    aB.OpenXmin();
    aB.OpenXmax();
    aB.OpenYmin();
    aB.OpenYmax();
  }
  else
  {
    aB.SetWhole();
    return;
  }
  aB.Enlarge(aTol);
}

// forward declaration
static double LegacyAdjustExtrSurf(const Adaptor3d_Surface& S,
                                   double                   UMin,
                                   double                   UMax,
                                   double                   VMin,
                                   double                   VMax,
                                   double                   Extr0,
                                   int                      CoordIndx,
                                   double                   Tol,
                                   bool                     IsMin);

class LegacySurfMaxMinCoord : public math_MultipleVarFunction
{
public:
  LegacySurfMaxMinCoord(const Adaptor3d_Surface& theSurf,
                        double                   UMin,
                        double                   UMax,
                        double                   VMin,
                        double                   VMax,
                        int                      CoordIndx,
                        double                   Sign)
      : mySurf(theSurf),
        myUMin(UMin),
        myUMax(UMax),
        myVMin(VMin),
        myVMax(VMax),
        myCoordIndx(CoordIndx),
        mySign(Sign),
        myPenalty(0.)
  {
    math_Vector X(1, 2);
    X(1) = UMin;
    X(2) = (VMin + VMax) / 2.;
    double F1, F2;
    Value(X, F1);
    X(1) = UMax;
    Value(X, F2);
    double DU = std::abs((F2 - F1) / (UMax - UMin));
    X(1)      = (UMin + UMax) / 2.;
    X(2)      = VMin;
    Value(X, F1);
    X(2) = VMax;
    Value(X, F2);
    double DV = std::abs((F2 - F1) / (VMax - VMin));
    myPenalty = 10. * std::max(DU, DV);
    myPenalty = std::max(myPenalty, 1.);
  }

  bool Value(const math_Vector& X, double& F) override
  {
    if (X(1) >= myUMin && X(1) <= myUMax && X(2) >= myVMin && X(2) <= myVMax)
    {
      gp_Pnt aP = mySurf.Value(X(1), X(2));
      F         = mySign * aP.Coord(myCoordIndx);
    }
    else
    {
      double UPen = 0., VPen = 0., u0, v0;
      if (X(1) < myUMin)
      {
        UPen = myPenalty * (myUMin - X(1));
        u0   = myUMin;
      }
      else if (X(1) > myUMax)
      {
        UPen = myPenalty * (X(1) - myUMax);
        u0   = myUMax;
      }
      else
        u0 = X(1);
      if (X(2) < myVMin)
      {
        VPen = myPenalty * (myVMin - X(2));
        v0   = myVMin;
      }
      else if (X(2) > myVMax)
      {
        VPen = myPenalty * (X(2) - myVMax);
        v0   = myVMax;
      }
      else
        v0 = X(2);
      gp_Pnt aP = mySurf.Value(u0, v0);
      F         = mySign * aP.Coord(myCoordIndx) + UPen + VPen;
    }
    return true;
  }

  int NbVariables() const override { return 2; }

private:
  LegacySurfMaxMinCoord&   operator=(const LegacySurfMaxMinCoord&) = delete;
  const Adaptor3d_Surface& mySurf;
  double                   myUMin, myUMax, myVMin, myVMax;
  int                      myCoordIndx;
  double                   mySign, myPenalty;
};

static double LegacyAdjustExtrSurf(const Adaptor3d_Surface& S,
                                   double                   UMin,
                                   double                   UMax,
                                   double                   VMin,
                                   double                   VMax,
                                   double                   Extr0,
                                   int                      CoordIndx,
                                   double                   Tol,
                                   bool                     IsMin)
{
  double aSign  = IsMin ? 1. : -1.;
  double extr   = aSign * Extr0;
  double relTol = 2. * Tol;
  if (std::abs(extr) > Tol)
    relTol /= std::abs(extr);
  double      Du = (S.LastUParameter() - S.FirstUParameter());
  double      Dv = (S.LastVParameter() - S.FirstVParameter());
  math_Vector aT(1, 2), aLowBorder(1, 2), aUppBorder(1, 2), aSteps(1, 2);
  aLowBorder(1)    = UMin;
  aUppBorder(1)    = UMax;
  aLowBorder(2)    = VMin;
  aUppBorder(2)    = VMax;
  int aNbU         = std::max(8, RealToInt(32 * (UMax - UMin) / Du));
  int aNbV         = std::max(8, RealToInt(32 * (VMax - VMin) / Dv));
  int aNbParticles = aNbU * aNbV;
  aSteps(1)        = std::min(0.1 * Du, (UMax - UMin) / (aNbU + 1));
  aSteps(2)        = std::min(0.1 * Dv, (VMax - VMin) / (aNbV + 1));
  LegacySurfMaxMinCoord aFunc(S, UMin, UMax, VMin, VMax, CoordIndx, aSign);
  math_PSO              aFinder(&aFunc, aLowBorder, aUppBorder, aSteps, aNbParticles);
  aFinder.Perform(aSteps, extr, aT);
  math_Matrix aDir(1, 2, 1, 2, 0.0);
  aDir(1, 1) = 1.;
  aDir(2, 2) = 1.;
  math_Powell powell(aFunc, relTol, 200, Tol);
  powell.Perform(aFunc, aT, aDir);
  if (powell.IsDone())
  {
    powell.Location(aT);
    extr = powell.Minimum();
  }
  return aSign * extr;
}

static void LegacyAddGenSurf(const Adaptor3d_Surface& S,
                             double                   UMin,
                             double                   UMax,
                             double                   VMin,
                             double                   VMax,
                             double                   Tol,
                             Bnd_Box&                 B)
{
  const int                  Nu          = LegacyNbUSampleRange(S, UMin, UMax);
  const int                  Nv          = LegacyNbVSampleRange(S, VMin, VMax);
  double                     CoordMin[3] = {RealLast(), RealLast(), RealLast()};
  double                     CoordMax[3] = {-RealLast(), -RealLast(), -RealLast()};
  double                     DeflMax[3]  = {-RealLast(), -RealLast(), -RealLast()};
  const double               du = (UMax - UMin) / (Nu - 1), du2 = du / 2.;
  const double               dv = (VMax - VMin) / (Nv - 1), dv2 = dv / 2.;
  const int                  NuFine = 2 * Nu - 1, NvFine = 2 * Nv - 1;
  NCollection_Array1<double> aUParams(1, NuFine), aVParams(1, NvFine);
  for (int i = 1; i <= NuFine; i++)
    aUParams.SetValue(i, UMin + (i - 1) * du2);
  for (int j = 1; j <= NvFine; j++)
    aVParams.SetValue(j, VMin + (j - 1) * dv2);
  GeomGridEval_Surface             anEvaluator(S);
  const NCollection_Array2<gp_Pnt> aFineGrid = anEvaluator.EvaluateGrid(aUParams, aVParams);
  NCollection_Array2<gp_XYZ>       aPnts(1, Nu, 1, Nv);
  for (int i = 1; i <= Nu; i++)
  {
    const int iFine = 2 * i - 1;
    for (int j = 1; j <= Nv; j++)
    {
      const int     jFine = 2 * j - 1;
      const gp_Pnt& P     = aFineGrid.Value(iFine, jFine);
      aPnts(i, j)         = P.XYZ();
      for (int k = 0; k < 3; ++k)
      {
        if (CoordMin[k] > P.Coord(k + 1))
          CoordMin[k] = P.Coord(k + 1);
        if (CoordMax[k] < P.Coord(k + 1))
          CoordMax[k] = P.Coord(k + 1);
      }
      if (i > 1)
      {
        const gp_XYZ  aPm = 0.5 * (aPnts(i - 1, j) + aPnts(i, j));
        const gp_Pnt& PM  = aFineGrid.Value(iFine - 1, jFine);
        const gp_XYZ  aD  = (PM.XYZ() - aPm);
        for (int k = 0; k < 3; ++k)
        {
          if (CoordMin[k] > PM.Coord(k + 1))
            CoordMin[k] = PM.Coord(k + 1);
          if (CoordMax[k] < PM.Coord(k + 1))
            CoordMax[k] = PM.Coord(k + 1);
          const double d = std::abs(aD.Coord(k + 1));
          if (DeflMax[k] < d)
            DeflMax[k] = d;
        }
      }
      if (j > 1)
      {
        const gp_XYZ  aPm = 0.5 * (aPnts(i, j - 1) + aPnts(i, j));
        const gp_Pnt& PM  = aFineGrid.Value(iFine, jFine - 1);
        const gp_XYZ  aD  = (PM.XYZ() - aPm);
        for (int k = 0; k < 3; ++k)
        {
          if (CoordMin[k] > PM.Coord(k + 1))
            CoordMin[k] = PM.Coord(k + 1);
          if (CoordMax[k] < PM.Coord(k + 1))
            CoordMax[k] = PM.Coord(k + 1);
          const double d = std::abs(aD.Coord(k + 1));
          if (DeflMax[k] < d)
            DeflMax[k] = d;
        }
      }
    }
  }
  double eps = std::max(Tol, Precision::Confusion());
  for (int k = 0; k < 3; ++k)
  {
    double d = DeflMax[k];
    if (d <= eps)
      continue;
    double CMin = CoordMin[k];
    double CMax = CoordMax[k];
    for (int i = 1; i <= Nu; ++i)
    {
      for (int j = 1; j <= Nv; ++j)
      {
        if (aPnts(i, j).Coord(k + 1) - CMin < d)
        {
          double umin = UMin + std::max(0, i - 2) * du, umax = UMin + std::min(Nu - 1, i) * du;
          double vmin = VMin + std::max(0, j - 2) * dv, vmax = VMin + std::min(Nv - 1, j) * dv;
          double cmin = LegacyAdjustExtrSurf(S, umin, umax, vmin, vmax, CMin, k + 1, eps, true);
          if (cmin < CMin)
            CMin = cmin;
        }
        else if (CMax - aPnts(i, j).Coord(k + 1) < d)
        {
          double umin = UMin + std::max(0, i - 2) * du, umax = UMin + std::min(Nu - 1, i) * du;
          double vmin = VMin + std::max(0, j - 2) * dv, vmax = VMin + std::min(Nv - 1, j) * dv;
          double cmax = LegacyAdjustExtrSurf(S, umin, umax, vmin, vmax, CMax, k + 1, eps, false);
          if (cmax > CMax)
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

// forward declaration for recursive OffsetSurface case
static Bnd_Box LegacyAddSurfaceImpl(const Adaptor3d_Surface& S,
                                    double                   UMin,
                                    double                   UMax,
                                    double                   VMin,
                                    double                   VMax,
                                    double                   Tol);

static Bnd_Box LegacyAddSurfaceImpl(const Adaptor3d_Surface& S,
                                    double                   UMin,
                                    double                   UMax,
                                    double                   VMin,
                                    double                   VMax,
                                    double                   Tol)
{
  Bnd_Box             B;
  GeomAbs_SurfaceType Type = S.GetType();
  if (Precision::IsInfinite(VMin) || Precision::IsInfinite(VMax) || Precision::IsInfinite(UMin)
      || Precision::IsInfinite(UMax))
  {
    if (Type == GeomAbs_Plane)
      LegacyTreatInfinitePlane(S.Plane(), UMin, UMax, VMin, VMax, Tol, B);
    else
      B.SetWhole();
    return B;
  }
  switch (Type)
  {
    case GeomAbs_Plane: {
      gp_Pln Plan = S.Plane();
      B.Add(ElSLib::Value(UMin, VMin, Plan));
      B.Add(ElSLib::Value(UMin, VMax, Plan));
      B.Add(ElSLib::Value(UMax, VMin, Plan));
      B.Add(ElSLib::Value(UMax, VMax, Plan));
      B.Enlarge(Tol);
      break;
    }
    case GeomAbs_Cylinder:
      BndLib::Add(S.Cylinder(), UMin, UMax, VMin, VMax, Tol, B);
      break;
    case GeomAbs_Cone:
      BndLib::Add(S.Cone(), UMin, UMax, VMin, VMax, Tol, B);
      break;
    case GeomAbs_Torus:
      BndLib::Add(S.Torus(), UMin, UMax, VMin, VMax, Tol, B);
      break;
    case GeomAbs_Sphere: {
      if (std::abs(UMin) < Precision::Angular() && std::abs(UMax - 2. * M_PI) < Precision::Angular()
          && std::abs(VMin + M_PI / 2.) < Precision::Angular()
          && std::abs(VMax - M_PI / 2.) < Precision::Angular())
        BndLib::Add(S.Sphere(), Tol, B);
      else
        BndLib::Add(S.Sphere(), UMin, UMax, VMin, VMax, Tol, B);
      break;
    }
    case GeomAbs_OffsetSurface: {
      occ::handle<Adaptor3d_Surface> HS = S.BasisSurface();
      B                                 = LegacyAddSurfaceImpl(*HS, UMin, UMax, VMin, VMax, Tol);
      B.Enlarge(S.OffsetValue());
      B.Enlarge(Tol);
      break;
    }
    case GeomAbs_BezierSurface:
    case GeomAbs_BSplineSurface: {
      bool   isUseConvexHull = true;
      double PTol            = Precision::Parametric(Precision::Confusion());
      double anUMinParam = UMin, anUMaxParam = UMax, aVMinParam = VMin, aVMaxParam = VMax;
      occ::handle<Geom_BSplineSurface> aBS;
      if (Type == GeomAbs_BezierSurface)
      {
        if (std::abs(UMin - S.FirstUParameter()) > PTol
            || std::abs(VMin - S.FirstVParameter()) > PTol
            || std::abs(UMax - S.LastUParameter()) > PTol
            || std::abs(VMax - S.LastVParameter()) > PTol)
          isUseConvexHull = false;
      }
      else
      {
        aBS = S.BSpline();
        aBS->Bounds(anUMinParam, anUMaxParam, aVMinParam, aVMaxParam);
        if ((UMin - anUMinParam) < -PTol || (VMin - aVMinParam) < -PTol
            || (UMax - anUMaxParam) > PTol || (VMax - aVMaxParam) > PTol)
          isUseConvexHull = false;
      }
      if (isUseConvexHull)
      {
        int                        aNbUPoles = S.NbUPoles(), aNbVPoles = S.NbVPoles();
        NCollection_Array2<gp_Pnt> Tp(1, aNbUPoles, 1, aNbVPoles);
        int                        UMinIdx = 0, UMaxIdx = 0, VMinIdx = 0, VMaxIdx = 0;
        bool                       isUPeriodic = S.IsUPeriodic(), isVPeriodic = S.IsVPeriodic();
        if (Type == GeomAbs_BezierSurface)
        {
          Tp      = S.Bezier()->Poles();
          UMinIdx = 1;
          UMaxIdx = aNbUPoles;
          VMinIdx = 1;
          VMaxIdx = aNbVPoles;
        }
        else
        {
          Tp      = aBS->Poles();
          UMinIdx = 1;
          UMaxIdx = aNbUPoles;
          VMinIdx = 1;
          VMaxIdx = aNbVPoles;
          if (UMin > anUMinParam || UMax < anUMaxParam)
          {
            const NCollection_Array1<int>&    aMults = aBS->UMultiplicities();
            const NCollection_Array1<double>& aKnots = aBS->UKnots();
            LegacyComputePolesIndexes(aKnots,
                                      aMults,
                                      aBS->UDegree(),
                                      UMin,
                                      UMax,
                                      aNbUPoles,
                                      isUPeriodic,
                                      UMinIdx,
                                      UMaxIdx);
          }
          if (VMin > aVMinParam || VMax < aVMaxParam)
          {
            const NCollection_Array1<int>&    aMults = aBS->VMultiplicities();
            const NCollection_Array1<double>& aKnots = aBS->VKnots();
            LegacyComputePolesIndexes(aKnots,
                                      aMults,
                                      aBS->VDegree(),
                                      VMin,
                                      VMax,
                                      aNbVPoles,
                                      isVPeriodic,
                                      VMinIdx,
                                      VMaxIdx);
          }
        }
        for (int i = UMinIdx; i <= UMaxIdx; i++)
        {
          int ip = i;
          if (isUPeriodic && ip > aNbUPoles)
            ip -= aNbUPoles;
          for (int j = VMinIdx; j <= VMaxIdx; j++)
          {
            int jp = j;
            if (isVPeriodic && jp > aNbVPoles)
              jp -= aNbVPoles;
            B.Add(Tp(ip, jp));
          }
        }
        B.Enlarge(Tol);
        break;
      }
    }
      [[fallthrough]];
    default: {
      const int                  Nu = LegacyNbUSamples(S);
      const int                  Nv = LegacyNbVSamples(S);
      NCollection_Array1<double> aUParams(1, Nu), aVParams(1, Nv);
      for (int i = 1; i <= Nu; i++)
        aUParams.SetValue(i, UMin + ((UMax - UMin) * (i - 1) / (Nu - 1)));
      for (int j = 1; j <= Nv; j++)
        aVParams.SetValue(j, VMin + ((VMax - VMin) * (j - 1) / (Nv - 1)));
      GeomGridEval_Surface             anEvaluator(S);
      const NCollection_Array2<gp_Pnt> aGrid = anEvaluator.EvaluateGrid(aUParams, aVParams);
      for (int i = aGrid.LowerRow(); i <= aGrid.UpperRow(); i++)
        for (int j = aGrid.LowerCol(); j <= aGrid.UpperCol(); j++)
          B.Add(aGrid.Value(i, j));
      B.Enlarge(Tol);
    }
  }
  return B;
}

static Bnd_Box LegacyAddOptimalSurfaceImpl(const Adaptor3d_Surface& S,
                                           double                   UMin,
                                           double                   UMax,
                                           double                   VMin,
                                           double                   VMax,
                                           double                   Tol)
{
  Bnd_Box             B;
  GeomAbs_SurfaceType Type = S.GetType();
  if (Precision::IsInfinite(VMin) || Precision::IsInfinite(VMax) || Precision::IsInfinite(UMin)
      || Precision::IsInfinite(UMax))
  {
    if (Type == GeomAbs_Plane)
      LegacyTreatInfinitePlane(S.Plane(), UMin, UMax, VMin, VMax, Tol, B);
    else
      B.SetWhole();
    return B;
  }
  switch (Type)
  {
    case GeomAbs_Plane: {
      gp_Pln Plan = S.Plane();
      B.Add(ElSLib::Value(UMin, VMin, Plan));
      B.Add(ElSLib::Value(UMin, VMax, Plan));
      B.Add(ElSLib::Value(UMax, VMin, Plan));
      B.Add(ElSLib::Value(UMax, VMax, Plan));
      B.Enlarge(Tol);
      break;
    }
    case GeomAbs_Cylinder:
      BndLib::Add(S.Cylinder(), UMin, UMax, VMin, VMax, Tol, B);
      break;
    case GeomAbs_Cone:
      BndLib::Add(S.Cone(), UMin, UMax, VMin, VMax, Tol, B);
      break;
    case GeomAbs_Sphere:
      BndLib::Add(S.Sphere(), UMin, UMax, VMin, VMax, Tol, B);
      break;
    default:
      LegacyAddGenSurf(S, UMin, UMax, VMin, VMax, Tol, B);
  }
  return B;
}

//=============================================================================
// OLD BndLib_Add2dCurve / BndLib_Box2dCurve implementation
//=============================================================================

class LegacyCurv2dMaxMinCoordMVar : public math_MultipleVarFunction
{
public:
  LegacyCurv2dMaxMinCoordMVar(const occ::handle<Geom2d_Curve>& theCurve,
                              double                           UMin,
                              double                           UMax,
                              int                              CoordIndx,
                              double                           Sign)
      : myCurve(theCurve),
        myUMin(UMin),
        myUMax(UMax),
        myCoordIndx(CoordIndx),
        mySign(Sign)
  {
  }

  bool Value(const math_Vector& X, double& F) override
  {
    if (X(1) < myUMin || X(1) > myUMax)
      return false;
    gp_Pnt2d aP = myCurve->Value(X(1));
    F           = mySign * aP.Coord(myCoordIndx);
    return true;
  }

  int NbVariables() const override { return 1; }

private:
  LegacyCurv2dMaxMinCoordMVar&     operator=(const LegacyCurv2dMaxMinCoordMVar&) = delete;
  const occ::handle<Geom2d_Curve>& myCurve;
  double                           myUMin, myUMax;
  int                              myCoordIndx;
  double                           mySign;
};

class LegacyCurv2dMaxMinCoord : public math_Function
{
public:
  LegacyCurv2dMaxMinCoord(const occ::handle<Geom2d_Curve>& theCurve,
                          double                           UMin,
                          double                           UMax,
                          int                              CoordIndx,
                          double                           Sign)
      : myCurve(theCurve),
        myUMin(UMin),
        myUMax(UMax),
        myCoordIndx(CoordIndx),
        mySign(Sign)
  {
  }

  bool Value(const double X, double& F) override
  {
    if (X < myUMin || X > myUMax)
      return false;
    gp_Pnt2d aP = myCurve->Value(X);
    F           = mySign * aP.Coord(myCoordIndx);
    return true;
  }

private:
  LegacyCurv2dMaxMinCoord&         operator=(const LegacyCurv2dMaxMinCoord&) = delete;
  const occ::handle<Geom2d_Curve>& myCurve;
  double                           myUMin, myUMax;
  int                              myCoordIndx;
  double                           mySign;
};

class LegacyBox2dCurve
{
public:
  LegacyBox2dCurve();

  void SetCurve(const occ::handle<Geom2d_Curve>& aC) { myCurve = aC; }

  void SetRange(const double aT1, const double aT2)
  {
    myT1 = aT1;
    myT2 = aT2;
  }

  const Bnd_Box2d& Box() const { return myBox; }

  int ErrorStatus() const { return myErrorStatus; }

  void Perform();
  void PerformOptimal(const double Tol);

private:
  void CheckData();
  void GetInfoBase();
  void PerformOnePoint();
  void PerformLineConic();
  void PerformBezier();
  void PerformBSpline();
  void PerformOther();
  void PerformGenCurv(const double Tol);
  void D0(const double, gp_Pnt2d&);
  void Compute(const occ::handle<Geom2d_Conic>&, GeomAbs_CurveType, double, double, Bnd_Box2d&);
  static int    Compute(const occ::handle<Geom2d_Conic>&, GeomAbs_CurveType, double*);
  static bool   IsTypeBase(const occ::handle<Geom2d_Curve>&, GeomAbs_CurveType&);
  static double AdjustToPeriod(double, double);
  int           NbSamples();
  double AdjustExtr(double UMin, double UMax, double Extr0, int CoordIndx, double Tol, bool IsMin);

  occ::handle<Geom2d_Curve> myCurve;
  Bnd_Box2d                 myBox;
  int                       myErrorStatus;
  occ::handle<Geom2d_Curve> myCurveBase;
  double                    myOffsetBase;
  bool                      myOffsetFlag;
  double                    myT1;
  double                    myT2;
  GeomAbs_CurveType         myTypeBase;
};

LegacyBox2dCurve::LegacyBox2dCurve()
    : myErrorStatus(-1),
      myOffsetBase(0.),
      myOffsetFlag(false),
      myT1(0.),
      myT2(0.),
      myTypeBase(GeomAbs_OtherCurve)
{
  myBox.SetVoid();
}

void LegacyBox2dCurve::CheckData()
{
  myErrorStatus = 0;
  if (myCurve.IsNull())
  {
    myErrorStatus = 10;
    return;
  }
  if (myT1 > myT2)
  {
    myErrorStatus = 12;
    return;
  }
}

void LegacyBox2dCurve::Perform()
{
  myBox.SetVoid();
  myErrorStatus = 0;
  myTypeBase    = GeomAbs_OtherCurve;
  myOffsetBase  = 0.;
  myOffsetFlag  = false;
  CheckData();
  if (myErrorStatus)
    return;
  if (myT1 == myT2)
  {
    PerformOnePoint();
    return;
  }
  GetInfoBase();
  if (myErrorStatus)
    return;
  if (myTypeBase == GeomAbs_Line || myTypeBase == GeomAbs_Circle || myTypeBase == GeomAbs_Ellipse
      || myTypeBase == GeomAbs_Parabola || myTypeBase == GeomAbs_Hyperbola)
    PerformLineConic();
  else if (myTypeBase == GeomAbs_BezierCurve)
    PerformBezier();
  else if (myTypeBase == GeomAbs_BSplineCurve)
    PerformBSpline();
  else
    myErrorStatus = 11;
}

void LegacyBox2dCurve::PerformOptimal(const double Tol)
{
  myBox.SetVoid();
  myErrorStatus = 0;
  myTypeBase    = GeomAbs_OtherCurve;
  myOffsetBase  = 0.;
  myOffsetFlag  = false;
  CheckData();
  if (myErrorStatus)
    return;
  if (myT1 == myT2)
  {
    PerformOnePoint();
    return;
  }
  GetInfoBase();
  if (myErrorStatus)
    return;
  if (myTypeBase == GeomAbs_Line || myTypeBase == GeomAbs_Circle || myTypeBase == GeomAbs_Ellipse
      || myTypeBase == GeomAbs_Parabola || myTypeBase == GeomAbs_Hyperbola)
    PerformLineConic();
  else
    PerformGenCurv(Tol);
}

void LegacyBox2dCurve::PerformOnePoint()
{
  gp_Pnt2d aP2D;
  myCurve->D0(myT1, aP2D);
  myBox.Add(aP2D);
}

void LegacyBox2dCurve::PerformBezier()
{
  if (myOffsetFlag)
  {
    PerformOther();
    return;
  }
  occ::handle<Geom2d_BezierCurve> aCBz = occ::down_cast<Geom2d_BezierCurve>(myCurveBase);
  double                          aT1 = aCBz->FirstParameter(), aT2 = aCBz->LastParameter();
  double                          aTb[2] = {std::max(myT1, aT1), std::min(myT2, aT2)};
  constexpr double                anEps  = Precision::PConfusion();
  if (std::abs(aT1 - aTb[0]) > anEps || std::abs(aT2 - aTb[1]) > anEps)
  {
    occ::handle<Geom2d_Geometry>    aG      = aCBz->Copy();
    occ::handle<Geom2d_BezierCurve> aCBzSeg = occ::down_cast<Geom2d_BezierCurve>(aG);
    aCBzSeg->Segment(aTb[0], aTb[1]);
    aCBz = aCBzSeg;
  }
  int aNbPoles = aCBz->NbPoles();
  for (int i = 1; i <= aNbPoles; ++i)
    myBox.Add(aCBz->Pole(i));
}

void LegacyBox2dCurve::PerformBSpline()
{
  if (myOffsetFlag)
  {
    PerformOther();
    return;
  }
  occ::handle<Geom2d_BSplineCurve> aCBS = occ::down_cast<Geom2d_BSplineCurve>(myCurveBase);
  double                           aT1 = aCBS->FirstParameter(), aT2 = aCBS->LastParameter();
  double                           aTb[2] = {std::max(myT1, aT1), std::min(myT2, aT2)};
  if (aTb[1] < aTb[0])
  {
    aTb[0] = aT1;
    aTb[1] = aT2;
  }
  constexpr double eps = Precision::PConfusion();
  if (std::abs(aT1 - aTb[0]) > eps || std::abs(aT2 - aTb[1]) > eps)
  {
    occ::handle<Geom2d_Geometry>     aG    = aCBS->Copy();
    occ::handle<Geom2d_BSplineCurve> aCBSs = occ::down_cast<Geom2d_BSplineCurve>(aG);
    aCBSs->Segment(aTb[0], aTb[1]);
    aCBS = aCBSs;
  }
  int aNbPoles = aCBS->NbPoles();
  for (int i = 1; i <= aNbPoles; ++i)
    myBox.Add(aCBS->Pole(i));
}

void LegacyBox2dCurve::PerformOther()
{
  constexpr int aNb = 33;
  double        dT  = (myT2 - myT1) / (aNb - 1);
  gp_Pnt2d      aP2D;
  for (int j = 0; j < aNb; ++j)
  {
    myCurve->D0(myT1 + j * dT, aP2D);
    myBox.Add(aP2D);
  }
  myCurve->D0(myT2, aP2D);
  myBox.Add(aP2D);
}

int LegacyBox2dCurve::NbSamples()
{
  int N;
  switch (myTypeBase)
  {
    case GeomAbs_BezierCurve: {
      occ::handle<Geom2d_BezierCurve> aCBz = occ::down_cast<Geom2d_BezierCurve>(myCurveBase);
      N                                    = aCBz->NbPoles();
      double du                            = myT2 - myT1;
      if (du < .9)
      {
        N = RealToInt(du * N) + 1;
        N = std::max(N, 5);
      }
      break;
    }
    case GeomAbs_BSplineCurve: {
      occ::handle<Geom2d_BSplineCurve> aCBS = occ::down_cast<Geom2d_BSplineCurve>(myCurveBase);
      N                                     = (aCBS->Degree() + 1) * (aCBS->NbKnots() - 1);
      double umin = aCBS->FirstParameter(), umax = aCBS->LastParameter();
      double du = (myT2 - myT1) / (umax - umin);
      if (du < .9)
      {
        N = RealToInt(du * N) + 1;
        N = std::max(N, 5);
      }
      break;
    }
    default:
      N = 17;
  }
  return std::min(23, N);
}

double LegacyBox2dCurve::AdjustExtr(double UMin,
                                    double UMax,
                                    double Extr0,
                                    int    CoordIndx,
                                    double Tol,
                                    bool   IsMin)
{
  double              aSign = IsMin ? 1. : -1.;
  double              extr  = aSign * Extr0;
  double              Du    = (myCurve->LastParameter() - myCurve->FirstParameter());
  Geom2dAdaptor_Curve aGAC(myCurve);
  double              UTol   = std::max(aGAC.Resolution(Tol), Precision::PConfusion());
  double              reltol = UTol / std::max(std::abs(UMin), std::abs(UMax));
  if (UMax - UMin < 0.01 * Du)
  {
    math_BrentMinimum       anOptLoc(reltol, 100, UTol);
    LegacyCurv2dMaxMinCoord aFunc(myCurve, UMin, UMax, CoordIndx, aSign);
    anOptLoc.Perform(aFunc, UMin, (UMin + UMax) / 2., UMax);
    if (anOptLoc.IsDone())
      return aSign * anOptLoc.Minimum();
  }
  int         aNbParticles = std::max(8, RealToInt(32 * (UMax - UMin) / Du));
  double      maxstep      = (UMax - UMin) / (aNbParticles + 1);
  math_Vector aT(1, 1), aLowBorder(1, 1), aUppBorder(1, 1), aSteps(1, 1);
  aLowBorder(1) = UMin;
  aUppBorder(1) = UMax;
  aSteps(1)     = std::min(0.1 * Du, maxstep);
  LegacyCurv2dMaxMinCoordMVar aFunc(myCurve, UMin, UMax, CoordIndx, aSign);
  math_PSO                    aFinder(&aFunc, aLowBorder, aUppBorder, aSteps, aNbParticles);
  aFinder.Perform(aSteps, extr, aT);
  math_BrentMinimum       anOptLoc(reltol, 100, UTol);
  LegacyCurv2dMaxMinCoord aFunc1(myCurve, UMin, UMax, CoordIndx, aSign);
  anOptLoc.Perform(aFunc1,
                   std::max(aT(1) - aSteps(1), UMin),
                   aT(1),
                   std::min(aT(1) + aSteps(1), UMax));
  if (anOptLoc.IsDone())
    extr = anOptLoc.Minimum();
  return aSign * extr;
}

void LegacyBox2dCurve::PerformGenCurv(const double Tol)
{
  int                       Nu          = NbSamples();
  double                    CoordMin[2] = {RealLast(), RealLast()};
  double                    CoordMax[2] = {-RealLast(), -RealLast()};
  double                    DeflMax[2]  = {-RealLast(), -RealLast()};
  gp_Pnt2d                  P;
  double                    du = (myT2 - myT1) / (Nu - 1), du2 = du / 2.;
  NCollection_Array1<gp_XY> aPnts(1, Nu);
  double                    u = myT1;
  for (int i = 1; i <= Nu; i++, u += du)
  {
    D0(u, P);
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
      D0(u - du2, P);
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
  for (int k = 0; k < 2; ++k)
  {
    double d = DeflMax[k];
    if (d <= Tol)
      continue;
    double CMin = CoordMin[k], CMax = CoordMax[k];
    for (int i = 1; i <= Nu; ++i)
    {
      if (aPnts(i).Coord(k + 1) - CMin < d)
      {
        double tmin = myT1 + std::max(0, i - 2) * du;
        double tmax = myT1 + std::min(Nu - 1, i) * du;
        double cmin = AdjustExtr(tmin, tmax, CMin, k + 1, Tol, true);
        if (cmin < CMin)
          CMin = cmin;
      }
      else if (CMax - aPnts(i).Coord(k + 1) < d)
      {
        double tmin = myT1 + std::max(0, i - 2) * du;
        double tmax = myT1 + std::min(Nu - 1, i) * du;
        double cmax = AdjustExtr(tmin, tmax, CMax, k + 1, Tol, false);
        if (cmax > CMax)
        {
          CMax = cmax;
        }
      }
    }
    CoordMin[k] = CMin;
    CoordMax[k] = CMax;
  }
  myBox.Add(gp_Pnt2d(CoordMin[0], CoordMin[1]));
  myBox.Add(gp_Pnt2d(CoordMax[0], CoordMax[1]));
  myBox.Enlarge(Tol);
}

void LegacyBox2dCurve::D0(const double aU, gp_Pnt2d& aP2D)
{
  gp_Vec2d aV1;
  myCurveBase->D1(aU, aP2D, aV1);
  if (myOffsetFlag)
  {
    int    aIndex = 2, aMaxDegree = 9;
    double aRes = gp::Resolution();
    while (aV1.Magnitude() <= aRes && aIndex <= aMaxDegree)
    {
      aV1 = myCurveBase->DN(aU, aIndex);
      ++aIndex;
    }
    double aA = aV1.Y(), aB = -aV1.X();
    double aR = std::sqrt(aA * aA + aB * aB);
    if (aR <= aRes)
    {
      myErrorStatus = 13;
      return;
    }
    aR = myOffsetBase / aR;
    aP2D.SetCoord(aP2D.X() + aA * aR, aP2D.Y() + aB * aR);
  }
}

void LegacyBox2dCurve::GetInfoBase()
{
  myErrorStatus                   = 0;
  myTypeBase                      = GeomAbs_OtherCurve;
  myOffsetBase                    = 0;
  occ::handle<Geom2d_Curve> aC2DB = myCurve;
  GeomAbs_CurveType         aTypeB;
  bool                      bIsTypeBase = IsTypeBase(aC2DB, aTypeB);
  if (bIsTypeBase)
  {
    myTypeBase  = aTypeB;
    myCurveBase = myCurve;
    return;
  }
  while (!bIsTypeBase)
  {
    int                              iTrimmed = 0, iOffset = 0;
    occ::handle<Geom2d_TrimmedCurve> aCT2D = occ::down_cast<Geom2d_TrimmedCurve>(aC2DB);
    if (!aCT2D.IsNull())
    {
      aC2DB = aCT2D->BasisCurve();
      ++iTrimmed;
    }
    occ::handle<Geom2d_OffsetCurve> aCF2D = occ::down_cast<Geom2d_OffsetCurve>(aC2DB);
    if (!aCF2D.IsNull())
    {
      myOffsetBase += aCF2D->Offset();
      myOffsetFlag = true;
      aC2DB        = aCF2D->BasisCurve();
      ++iOffset;
    }
    if (!(iTrimmed || iOffset))
      break;
    bIsTypeBase = IsTypeBase(aC2DB, aTypeB);
    if (bIsTypeBase)
    {
      myTypeBase  = aTypeB;
      myCurveBase = aC2DB;
      return;
    }
  }
  myErrorStatus = 11;
}

bool LegacyBox2dCurve::IsTypeBase(const occ::handle<Geom2d_Curve>& aC2D, GeomAbs_CurveType& aTypeB)
{
  occ::handle<Standard_Type> aType = aC2D->DynamicType();
  if (aType == STANDARD_TYPE(Geom2d_Line))
  {
    aTypeB = GeomAbs_Line;
    return true;
  }
  if (aType == STANDARD_TYPE(Geom2d_Circle))
  {
    aTypeB = GeomAbs_Circle;
    return true;
  }
  if (aType == STANDARD_TYPE(Geom2d_Ellipse))
  {
    aTypeB = GeomAbs_Ellipse;
    return true;
  }
  if (aType == STANDARD_TYPE(Geom2d_Parabola))
  {
    aTypeB = GeomAbs_Parabola;
    return true;
  }
  if (aType == STANDARD_TYPE(Geom2d_Hyperbola))
  {
    aTypeB = GeomAbs_Hyperbola;
    return true;
  }
  if (aType == STANDARD_TYPE(Geom2d_BezierCurve))
  {
    aTypeB = GeomAbs_BezierCurve;
    return true;
  }
  if (aType == STANDARD_TYPE(Geom2d_BSplineCurve))
  {
    aTypeB = GeomAbs_BSplineCurve;
    return true;
  }
  aTypeB = GeomAbs_OtherCurve;
  return false;
}

void LegacyBox2dCurve::PerformLineConic()
{
  int      iInf[2] = {0, 0};
  double   aTb[2]  = {myT1, myT2};
  gp_Pnt2d aP2D;
  for (int i = 0; i < 2; ++i)
  {
    D0(aTb[i], aP2D);
    myBox.Add(aP2D);
    if (Precision::IsNegativeInfinite(aTb[i]))
      ++iInf[0];
    else if (Precision::IsPositiveInfinite(aTb[i]))
      ++iInf[1];
  }
  if (myTypeBase == GeomAbs_Line)
    return;
  if (iInf[0] && iInf[1])
    return;
  occ::handle<Geom2d_Conic> aConic2D = occ::down_cast<Geom2d_Conic>(myCurveBase);
  Compute(aConic2D, myTypeBase, aTb[0], aTb[1], myBox);
}

void LegacyBox2dCurve::Compute(const occ::handle<Geom2d_Conic>& aConic2D,
                               const GeomAbs_CurveType          aType,
                               const double                     aT1,
                               const double                     aT2,
                               Bnd_Box2d&                       aBox2D)
{
  double pT[10];
  int    aNbT = Compute(aConic2D, aType, pT);
  if (aType == GeomAbs_Parabola || aType == GeomAbs_Hyperbola)
  {
    gp_Pnt2d aP2D;
    for (int i = 0; i < aNbT; ++i)
    {
      double aT = pT[i];
      if (aT > aT1 && aT < aT2)
      {
        D0(aT, aP2D);
        aBox2D.Add(aP2D);
      }
    }
    return;
  }
  double aEps = Precision::Angular(), aTwoPI = 2. * M_PI, dT = aT2 - aT1;
  double aT1z = AdjustToPeriod(aT1, aTwoPI);
  if (std::abs(aT1z) < aEps)
    aT1z = 0.;
  double aT2z = aT1z + dT;
  if (std::abs(aT2z - aTwoPI) < aEps)
    aT2z = aTwoPI;
  gp_Pnt2d aP2D;
  for (int i = 0; i < aNbT; ++i)
  {
    double aT = pT[i];
    aT        = (aT < aT1z ? aT + aTwoPI : aT);
    if (aT <= aT2z)
    {
      D0(aT, aP2D);
      aBox2D.Add(aP2D);
    }
  }
}

int LegacyBox2dCurve::Compute(const occ::handle<Geom2d_Conic>& aConic2D,
                              const GeomAbs_CurveType          aType,
                              double*                          pT)
{
  int             iRet   = 0;
  const gp_Ax22d& aPos   = aConic2D->Position();
  const gp_XY&    aXDir  = aPos.XDirection().XY();
  const gp_XY&    aYDir  = aPos.YDirection().XY();
  double          aCosBt = aXDir.X(), aSinBt = aXDir.Y();
  double          aCosGm = aYDir.X(), aSinGm = aYDir.Y();
  if (aType == GeomAbs_Circle || aType == GeomAbs_Ellipse)
  {
    double aR1 = 0., aR2 = 0., aTwoPI = M_PI + M_PI;
    if (aType == GeomAbs_Ellipse)
    {
      occ::handle<Geom2d_Ellipse> aEL2D = occ::down_cast<Geom2d_Ellipse>(aConic2D);
      aR1                               = aEL2D->MajorRadius();
      aR2                               = aEL2D->MinorRadius();
    }
    else
    {
      occ::handle<Geom2d_Circle> aCR2D = occ::down_cast<Geom2d_Circle>(aConic2D);
      aR1                              = aCR2D->Radius();
      aR2                              = aR1;
    }
    double aA11 = -aR1 * aCosBt, aA12 = aR2 * aCosGm;
    double aA21 = -aR1 * aSinBt, aA22 = aR2 * aSinGm;
    for (int i = 0; i < 2; ++i)
    {
      double aLx = (!i) ? 0. : 1., aLy = (!i) ? 1. : 0.;
      double aBx = aLx * aA21 - aLy * aA11;
      double aBy = aLx * aA22 - aLy * aA12;
      double aB  = std::sqrt(aBx * aBx + aBy * aBy);
      double aFi = std::acos(aBx / aB);
      if (aBy / aB < 0.)
        aFi = aTwoPI - aFi;
      int j     = 2 * i;
      pT[j]     = AdjustToPeriod(aTwoPI - aFi, aTwoPI);
      pT[j + 1] = AdjustToPeriod(M_PI - aFi, aTwoPI);
    }
    iRet = 4;
  }
  else if (aType == GeomAbs_Parabola)
  {
    occ::handle<Geom2d_Parabola> aPR2D = occ::down_cast<Geom2d_Parabola>(aConic2D);
    double                       aFc = aPR2D->Focal(), aEps = Precision::Angular();
    int                          j = 0;
    for (int i = 0; i < 2; i++)
    {
      double aLx = (!i) ? 0. : 1., aLy = (!i) ? 1. : 0.;
      double aA2 = aLx * aSinBt - aLy * aCosBt;
      if (std::abs(aA2) < aEps)
        continue;
      double aA1 = aLy * aCosGm - aLx * aSinGm;
      pT[j++]    = 2. * aFc * aA1 / aA2;
    }
    iRet = j;
  }
  else if (aType == GeomAbs_Hyperbola)
  {
    occ::handle<Geom2d_Hyperbola> aHP2D = occ::down_cast<Geom2d_Hyperbola>(aConic2D);
    double                        aR1 = aHP2D->MajorRadius(), aR2 = aHP2D->MinorRadius();
    double                        aEps = Precision::Angular();
    int                           j    = 0;
    for (int i = 0; i < 2; i++)
    {
      double aLx = (!i) ? 0. : 1., aLy = (!i) ? 1. : 0.;
      double aB1 = aR1 * (aLx * aSinBt - aLy * aCosBt);
      double aB2 = aR2 * (aLx * aSinGm - aLy * aCosGm);
      if (std::abs(aB1) < aEps)
        continue;
      if (std::abs(aB2) < aEps)
      {
        pT[j++] = 0.;
      }
      else
      {
        double aB12 = aB1 * aB1, aB22 = aB2 * aB2;
        if (!(aB12 > aB22))
          continue;
        double aD = std::sqrt(aB12 - aB22);
        for (int k = -1; k < 2; k += 2)
        {
          double aZ = (aB1 + k * aD) / aB2;
          if (std::abs(aZ) < 1.)
            pT[j++] = -std::log((1. + aZ) / (1. - aZ));
        }
      }
    }
    iRet = j;
  }
  return iRet;
}

double LegacyBox2dCurve::AdjustToPeriod(const double aT, const double aPeriod)
{
  double aTRet = aT;
  if (aT < 0.)
  {
    aTRet = aT + (1 + static_cast<int>(-aT / aPeriod)) * aPeriod;
  }
  else if (aT > aPeriod)
  {
    aTRet = aT - static_cast<int>(aT / aPeriod) * aPeriod;
  }
  if (aTRet == aPeriod)
    aTRet = 0.;
  return aTRet;
}

static Bnd_Box2d LegacyAdd2dCurveImpl(const occ::handle<Geom2d_Curve>& aC2D,
                                      double                           aT1,
                                      double                           aT2,
                                      double                           aTol)
{
  LegacyBox2dCurve aBC;
  aBC.SetCurve(aC2D);
  aBC.SetRange(aT1, aT2);
  aBC.Perform();
  Bnd_Box2d aBox = aBC.Box();
  aBox.Enlarge(aTol);
  return aBox;
}

static Bnd_Box2d LegacyAddOptimal2dCurveImpl(const occ::handle<Geom2d_Curve>& aC2D,
                                             double                           aT1,
                                             double                           aT2,
                                             double                           aTol)
{
  LegacyBox2dCurve aBC;
  aBC.SetCurve(aC2D);
  aBC.SetRange(aT1, aT2);
  aBC.PerformOptimal(aTol);
  Bnd_Box2d aBox = aBC.Box();
  aBox.Enlarge(aTol);
  return aBox;
}

} // anonymous namespace

//=============================================================================
// Public comparison functions
//=============================================================================

namespace BndLib_LegacyCheck
{

//=================================================================================================

void Compare3dCurveAdd(const Adaptor3d_Curve& theC,
                       double                 theU1,
                       double                 theU2,
                       double                 theTol,
                       const Bnd_Box&         theNewBox)
{
  const Bnd_Box aOldBox = LegacyAdd3dCurveImpl(theC, theU1, theU2, theTol);
  if (!boxesEqual(aOldBox, theNewBox))
  {
    std::cerr << "[BndLib_LegacyCheck] Add3dCurve"
              << " geom=" << curveTypeName(theC.GetType())
              << " adaptor=" << theC.DynamicType()->Name() << ":\n";
    printBox("  Old:", aOldBox);
    printBox("  New:", theNewBox);
  }
}

//=================================================================================================

void Compare3dCurveAddOptimal(const Adaptor3d_Curve& theC,
                              double                 theU1,
                              double                 theU2,
                              double                 theTol,
                              const Bnd_Box&         theNewBox)
{
  const Bnd_Box aOldBox = LegacyAddOptimal3dCurveImpl(theC, theU1, theU2, theTol);
  if (!boxesEqual(aOldBox, theNewBox))
  {
    std::cerr << "[BndLib_LegacyCheck] AddOptimal3dCurve"
              << " geom=" << curveTypeName(theC.GetType())
              << " adaptor=" << theC.DynamicType()->Name() << ":\n";
    printBox("  Old:", aOldBox);
    printBox("  New:", theNewBox);
  }
}

//=================================================================================================

void CompareSurfaceAdd(const Adaptor3d_Surface& theS,
                       double                   theUMin,
                       double                   theUMax,
                       double                   theVMin,
                       double                   theVMax,
                       double                   theTol,
                       const Bnd_Box&           theNewBox)
{
  const Bnd_Box aOldBox = LegacyAddSurfaceImpl(theS, theUMin, theUMax, theVMin, theVMax, theTol);
  if (!boxesEqual(aOldBox, theNewBox))
  {
    std::cerr << "[BndLib_LegacyCheck] AddSurface"
              << " geom=" << surfaceTypeName(theS.GetType())
              << " adaptor=" << theS.DynamicType()->Name() << ":\n";
    printBox("  Old:", aOldBox);
    printBox("  New:", theNewBox);
  }
}

//=================================================================================================

void CompareSurfaceAddOptimal(const Adaptor3d_Surface& theS,
                              double                   theUMin,
                              double                   theUMax,
                              double                   theVMin,
                              double                   theVMax,
                              double                   theTol,
                              const Bnd_Box&           theNewBox)
{
  const Bnd_Box aOldBox =
    LegacyAddOptimalSurfaceImpl(theS, theUMin, theUMax, theVMin, theVMax, theTol);
  if (!boxesEqual(aOldBox, theNewBox))
  {
    std::cerr << "[BndLib_LegacyCheck] AddOptimalSurface"
              << " geom=" << surfaceTypeName(theS.GetType())
              << " adaptor=" << theS.DynamicType()->Name() << ":\n";
    printBox("  Old:", aOldBox);
    printBox("  New:", theNewBox);
  }
}

//=================================================================================================

void Compare2dCurveAdd(const Handle(Geom2d_Curve)& theC,
                       double                      theT1,
                       double                      theT2,
                       double                      theTol,
                       const Bnd_Box2d&            theNewBox)
{
  const Bnd_Box2d aOldBox = LegacyAdd2dCurveImpl(theC, theT1, theT2, theTol);
  if (!boxesEqual2d(aOldBox, theNewBox))
  {
    std::cerr << "[BndLib_LegacyCheck] Add2dCurve"
              << " geom=" << theC->DynamicType()->Name() << ":\n";
    printBox2d("  Old:", aOldBox);
    printBox2d("  New:", theNewBox);
  }
}

//=================================================================================================

void Compare2dCurveAddOptimal(const Handle(Geom2d_Curve)& theC,
                              double                      theT1,
                              double                      theT2,
                              double                      theTol,
                              const Bnd_Box2d&            theNewBox)
{
  const Bnd_Box2d aOldBox = LegacyAddOptimal2dCurveImpl(theC, theT1, theT2, theTol);
  if (!boxesEqual2d(aOldBox, theNewBox))
  {
    std::cerr << "[BndLib_LegacyCheck] AddOptimal2dCurve"
              << " geom=" << theC->DynamicType()->Name() << ":\n";
    printBox2d("  Old:", aOldBox);
    printBox2d("  New:", theNewBox);
  }
}

} // namespace BndLib_LegacyCheck
