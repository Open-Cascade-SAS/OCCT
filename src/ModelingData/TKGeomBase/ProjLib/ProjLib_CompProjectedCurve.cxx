// Created on: 1997-09-23
// Created by: Roman BORISOV
// Copyright (c) 1997-1999 Matra Datavision
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

#include <algorithm>

#include <Approx_CurveOnSurface.hxx>
#include <Extrema_ExtCS.hxx>
#include <Extrema_ExtPS.hxx>
#include <Extrema_POnCurv.hxx>
#include <Extrema_POnSurf.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomLib.hxx>
#include <gp_Mat2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_XY.hxx>
#include <Precision.hxx>
#include <ProjLib_CompProjectedCurve.hxx>
#include <ProjLib_HCompProjectedCurve.hxx>
#include <ProjLib_PrjResolve.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_TypeMismatch.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Extrema_ExtCC.hxx>
#include <NCollection_Vector.hxx>

#define FuncTol 1.e-10

IMPLEMENT_STANDARD_RTTIEXT(ProjLib_CompProjectedCurve, Adaptor2d_Curve2d)

#ifdef OCCT_DEBUG_CHRONO
  #include <OSD_Timer.hxx>

static OSD_Chronometer chr_init_point, chr_dicho_bound;

Standard_EXPORT double t_init_point, t_dicho_bound;
Standard_EXPORT int    init_point_count, dicho_bound_count;

static void InitChron(OSD_Chronometer& ch)
{
  ch.Reset();
  ch.Start();
}

static void ResultChron(OSD_Chronometer& ch, double& time)
{
  double tch;
  ch.Stop();
  ch.Show(tch);
  time = time + tch;
}
#endif

// Structure to perform splits computation.
// This structure is not thread-safe since operations under mySplits should be performed in a
// critical section. myPeriodicDir - 0 for U periodicity and 1 for V periodicity.
struct SplitDS
{
  SplitDS(const occ::handle<Adaptor3d_Curve>&   theCurve,
          const occ::handle<Adaptor3d_Surface>& theSurface,
          NCollection_Vector<double>&           theSplits)
      : myCurve(theCurve),
        mySurface(theSurface),
        mySplits(theSplits),
        myPerMinParam(0.0),
        myPerMaxParam(0.0),
        myPeriodicDir(0),
        myExtCCCurve1(nullptr),
        myExtCCLast2DParam(0.0),
        myExtPS(nullptr)
  {
  }

  const occ::handle<Adaptor3d_Curve>   myCurve;
  const occ::handle<Adaptor3d_Surface> mySurface;
  NCollection_Vector<double>&          mySplits;

  double myPerMinParam;
  double myPerMaxParam;
  int    myPeriodicDir;

  Adaptor3d_CurveOnSurface* myExtCCCurve1;
  double                    myExtCCLast2DParam;

  Extrema_ExtPS* myExtPS;

private:
  // Assignment operator is forbidden.
  void operator=(const SplitDS& theSplitDS) = delete;
};

//! Compute split points in the parameter space of the curve.
static void BuildCurveSplits(const occ::handle<Adaptor3d_Curve>&   theCurve,
                             const occ::handle<Adaptor3d_Surface>& theSurface,
                             const double                          theTolU,
                             const double                          theTolV,
                             NCollection_Vector<double>&           theSplits);

//! Perform splitting on a specified direction. Sub-method in BuildCurveSplits.
static void SplitOnDirection(SplitDS& theSplitDS);

//! Perform recursive search of the split points.
static void FindSplitPoint(SplitDS& theSplitDS, const double theMinParam, const double theMaxParam);

//=======================================================================
// function : Comparator
// purpose  : used in sort algorithm
//=======================================================================
inline bool Comparator(const double theA, const double theB)
{
  return theA < theB;
}

//=======================================================================
// function : d1
// purpose  : computes first derivative of the projected curve
//=======================================================================

static void d1(const double                          t,
               const double                          u,
               const double                          v,
               gp_Vec2d&                             V,
               const occ::handle<Adaptor3d_Curve>&   Curve,
               const occ::handle<Adaptor3d_Surface>& Surface)
{
  gp_Pnt S, C;
  gp_Vec DS1_u, DS1_v, DS2_u, DS2_uv, DS2_v, DC1_t;
  Surface->D2(u, v, S, DS1_u, DS1_v, DS2_u, DS2_v, DS2_uv);
  Curve->D1(t, C, DC1_t);
  gp_Vec Ort(C, S); // Ort = S - C

  gp_Vec2d dE_dt(-DC1_t * DS1_u, -DC1_t * DS1_v);
  gp_XY    dE_du(DS1_u * DS1_u + Ort * DS2_u, DS1_u * DS1_v + Ort * DS2_uv);
  gp_XY    dE_dv(DS1_v * DS1_u + Ort * DS2_uv, DS1_v * DS1_v + Ort * DS2_v);

  double det = dE_du.X() * dE_dv.Y() - dE_du.Y() * dE_dv.X();
  if (fabs(det) < gp::Resolution())
    throw Standard_ConstructionError();

  gp_Mat2d M(gp_XY(dE_dv.Y() / det, -dE_du.Y() / det), gp_XY(-dE_dv.X() / det, dE_du.X() / det));

  V = -gp_Vec2d(gp_Vec2d(M.Row(1)) * dE_dt, gp_Vec2d(M.Row(2)) * dE_dt);
}

//=======================================================================
// function : d2
// purpose  : computes second derivative of the projected curve
//=======================================================================

static void d2(const double                          t,
               const double                          u,
               const double                          v,
               gp_Vec2d&                             V1,
               gp_Vec2d&                             V2,
               const occ::handle<Adaptor3d_Curve>&   Curve,
               const occ::handle<Adaptor3d_Surface>& Surface)
{
  gp_Pnt S, C;
  gp_Vec DS1_u, DS1_v, DS2_u, DS2_uv, DS2_v, DS3_u, DS3_v, DS3_uuv, DS3_uvv, DC1_t, DC2_t;
  Surface->D3(u, v, S, DS1_u, DS1_v, DS2_u, DS2_v, DS2_uv, DS3_u, DS3_v, DS3_uuv, DS3_uvv);
  Curve->D2(t, C, DC1_t, DC2_t);
  gp_Vec Ort(C, S);

  gp_Vec2d dE_dt(-DC1_t * DS1_u, -DC1_t * DS1_v);
  gp_XY    dE_du(DS1_u * DS1_u + Ort * DS2_u, DS1_u * DS1_v + Ort * DS2_uv);
  gp_XY    dE_dv(DS1_v * DS1_u + Ort * DS2_uv, DS1_v * DS1_v + Ort * DS2_v);

  double det = dE_du.X() * dE_dv.Y() - dE_du.Y() * dE_dv.X();
  if (fabs(det) < gp::Resolution())
    throw Standard_ConstructionError();

  gp_Mat2d M(gp_XY(dE_dv.Y() / det, -dE_du.Y() / det), gp_XY(-dE_dv.X() / det, dE_du.X() / det));

  // First derivative
  V1 = -gp_Vec2d(gp_Vec2d(M.Row(1)) * dE_dt, gp_Vec2d(M.Row(2)) * dE_dt);

  /* Second derivative */

  // Computation of d2E_dt2 = S1
  gp_Vec2d d2E_dt(-DC2_t * DS1_u, -DC2_t * DS1_v);

  // Computation of 2*(d2E/dtdX)(dX/dt) = S2
  gp_Vec2d d2E1_dtdX(-DC1_t * DS2_u, -DC1_t * DS2_uv);
  gp_Vec2d d2E2_dtdX(-DC1_t * DS2_uv, -DC1_t * DS2_v);
  gp_Vec2d S2 = 2 * gp_Vec2d(d2E1_dtdX * V1, d2E2_dtdX * V1);

  // Computation of (d2E/dX2)*(dX/dt)2 = S3

  // Row11 = (d2E1/du2, d2E1/dudv)
  double   tmp;
  gp_Vec2d Row11(3 * DS1_u * DS2_u + Ort * DS3_u,
                 tmp = 2 * DS1_u * DS2_uv + DS1_v * DS2_u + Ort * DS3_uuv);

  // Row12 = (d2E1/dudv, d2E1/dv2)
  gp_Vec2d Row12(tmp, DS2_v * DS1_u + 2 * DS1_v * DS2_uv + Ort * DS3_uvv);

  // Row21 = (d2E2/du2, d2E2/dudv)
  gp_Vec2d Row21(DS2_u * DS1_v + 2 * DS1_u * DS2_uv + Ort * DS3_uuv,
                 tmp = 2 * DS2_uv * DS1_v + DS1_u * DS2_v + Ort * DS3_uvv);

  // Row22 = (d2E2/duv, d2E2/dvdv)
  gp_Vec2d Row22(tmp, 3 * DS1_v * DS2_v + Ort * DS3_v);

  gp_Vec2d S3(V1 * gp_Vec2d(Row11 * V1, Row12 * V1), V1 * gp_Vec2d(Row21 * V1, Row22 * V1));

  gp_Vec2d Sum = d2E_dt + S2 + S3;

  V2 = -gp_Vec2d(gp_Vec2d(M.Row(1)) * Sum, gp_Vec2d(M.Row(2)) * Sum);
}

//=======================================================================
// function : d1CurveOnSurf
// purpose  : computes first derivative of the 3d projected curve
//=======================================================================

//=======================================================================
// function : d2CurveOnSurf
// purpose  : computes second derivative of the 3D projected curve
//=======================================================================

static void d2CurvOnSurf(const double                          t,
                         const double                          u,
                         const double                          v,
                         gp_Vec&                               V1,
                         gp_Vec&                               V2,
                         const occ::handle<Adaptor3d_Curve>&   Curve,
                         const occ::handle<Adaptor3d_Surface>& Surface)
{
  gp_Pnt   S, C;
  gp_Vec2d V12d, V22d;
  gp_Vec   DS1_u, DS1_v, DS2_u, DS2_uv, DS2_v, DS3_u, DS3_v, DS3_uuv, DS3_uvv, DC1_t, DC2_t;
  Surface->D3(u, v, S, DS1_u, DS1_v, DS2_u, DS2_v, DS2_uv, DS3_u, DS3_v, DS3_uuv, DS3_uvv);
  Curve->D2(t, C, DC1_t, DC2_t);
  gp_Vec Ort(C, S);

  gp_Vec2d dE_dt(-DC1_t * DS1_u, -DC1_t * DS1_v);
  gp_XY    dE_du(DS1_u * DS1_u + Ort * DS2_u, DS1_u * DS1_v + Ort * DS2_uv);
  gp_XY    dE_dv(DS1_v * DS1_u + Ort * DS2_uv, DS1_v * DS1_v + Ort * DS2_v);

  double det = dE_du.X() * dE_dv.Y() - dE_du.Y() * dE_dv.X();
  if (fabs(det) < gp::Resolution())
    throw Standard_ConstructionError();

  gp_Mat2d M(gp_XY(dE_dv.Y() / det, -dE_du.Y() / det), gp_XY(-dE_dv.X() / det, dE_du.X() / det));

  // First derivative
  V12d = -gp_Vec2d(gp_Vec2d(M.Row(1)) * dE_dt, gp_Vec2d(M.Row(2)) * dE_dt);

  /* Second derivative */

  // Computation of d2E_dt2 = S1
  gp_Vec2d d2E_dt(-DC2_t * DS1_u, -DC2_t * DS1_v);

  // Computation of 2*(d2E/dtdX)(dX/dt) = S2
  gp_Vec2d d2E1_dtdX(-DC1_t * DS2_u, -DC1_t * DS2_uv);
  gp_Vec2d d2E2_dtdX(-DC1_t * DS2_uv, -DC1_t * DS2_v);
  gp_Vec2d S2 = 2 * gp_Vec2d(d2E1_dtdX * V12d, d2E2_dtdX * V12d);

  // Computation of (d2E/dX2)*(dX/dt)2 = S3

  // Row11 = (d2E1/du2, d2E1/dudv)
  double   tmp;
  gp_Vec2d Row11(3 * DS1_u * DS2_u + Ort * DS3_u,
                 tmp = 2 * DS1_u * DS2_uv + DS1_v * DS2_u + Ort * DS3_uuv);

  // Row12 = (d2E1/dudv, d2E1/dv2)
  gp_Vec2d Row12(tmp, DS2_v * DS1_u + 2 * DS1_v * DS2_uv + Ort * DS3_uvv);

  // Row21 = (d2E2/du2, d2E2/dudv)
  gp_Vec2d Row21(DS2_u * DS1_v + 2 * DS1_u * DS2_uv + Ort * DS3_uuv,
                 tmp = 2 * DS2_uv * DS1_v + DS1_u * DS2_v + Ort * DS3_uvv);

  // Row22 = (d2E2/duv, d2E2/dvdv)
  gp_Vec2d Row22(tmp, 3 * DS1_v * DS2_v + Ort * DS3_v);

  gp_Vec2d S3(V12d * gp_Vec2d(Row11 * V12d, Row12 * V12d),
              V12d * gp_Vec2d(Row21 * V12d, Row22 * V12d));

  gp_Vec2d Sum = d2E_dt + S2 + S3;

  V22d = -gp_Vec2d(gp_Vec2d(M.Row(1)) * Sum, gp_Vec2d(M.Row(2)) * Sum);

  V1 = DS1_u * V12d.X() + DS1_v * V12d.Y();
  V2 = DS2_u * V12d.X() * V12d.X() + DS1_u * V22d.X() + 2 * DS2_uv * V12d.X() * V12d.Y()
       + DS2_v * V12d.Y() * V12d.Y() + DS1_v * V22d.Y();
}

//=======================================================================
// function : ExactBound
// purpose  : computes exact boundary point
//=======================================================================

static bool ExactBound(gp_Pnt&                               Sol,
                       const double                          NotSol,
                       const double                          Tol,
                       const double                          TolU,
                       const double                          TolV,
                       const occ::handle<Adaptor3d_Curve>&   Curve,
                       const occ::handle<Adaptor3d_Surface>& Surface)
{
  double   U0, V0, t, t1, t2, FirstU, LastU, FirstV, LastV;
  gp_Pnt2d POnS;
  U0     = Sol.Y();
  V0     = Sol.Z();
  FirstU = Surface->FirstUParameter();
  LastU  = Surface->LastUParameter();
  FirstV = Surface->FirstVParameter();
  LastV  = Surface->LastVParameter();
  // Here we have to compute the boundary that projection is going to intersect
  gp_Vec2d D2d;
  // these variables are to estimate which boundary has more opportunity
  // to be intersected
  double RU1, RU2, RV1, RV2;
  d1(Sol.X(), U0, V0, D2d, Curve, Surface);
  // Here we assume that D2d != (0, 0)
  if (std::abs(D2d.X()) < gp::Resolution())
  {
    RU1 = Precision::Infinite();
    RU2 = Precision::Infinite();
    RV1 = V0 - FirstV;
    RV2 = LastV - V0;
  }
  else if (std::abs(D2d.Y()) < gp::Resolution())
  {
    RU1 = U0 - FirstU;
    RU2 = LastU - U0;
    RV1 = Precision::Infinite();
    RV2 = Precision::Infinite();
  }
  else
  {
    RU1 = gp_Pnt2d(U0, V0).Distance(gp_Pnt2d(FirstU, V0 + (FirstU - U0) * D2d.Y() / D2d.X()));
    RU2 = gp_Pnt2d(U0, V0).Distance(gp_Pnt2d(LastU, V0 + (LastU - U0) * D2d.Y() / D2d.X()));
    RV1 = gp_Pnt2d(U0, V0).Distance(gp_Pnt2d(U0 + (FirstV - V0) * D2d.X() / D2d.Y(), FirstV));
    RV2 = gp_Pnt2d(U0, V0).Distance(gp_Pnt2d(U0 + (LastV - V0) * D2d.X() / D2d.Y(), LastV));
  }
  NCollection_Sequence<gp_Pnt> Seq;
  Seq.Append(gp_Pnt(FirstU, RU1, 2));
  Seq.Append(gp_Pnt(LastU, RU2, 2));
  Seq.Append(gp_Pnt(FirstV, RV1, 3));
  Seq.Append(gp_Pnt(LastV, RV2, 3));
  int i, j;
  for (i = 1; i <= 3; i++)
  {
    for (j = 1; j <= 4 - i; j++)
    {
      if (Seq(j).Y() < Seq(j + 1).Y())
      {
        gp_Pnt swp;
        swp                    = Seq.Value(j + 1);
        Seq.ChangeValue(j + 1) = Seq.Value(j);
        Seq.ChangeValue(j)     = swp;
      }
    }
  }

  t  = Sol.X();
  t1 = std::min(Sol.X(), NotSol);
  t2 = std::max(Sol.X(), NotSol);

  bool isDone = false;
  while (!Seq.IsEmpty())
  {
    gp_Pnt P;
    P = Seq.Last();
    Seq.Remove(Seq.Length());
    ProjLib_PrjResolve aPrjPS(*Curve, *Surface, int(P.Z()));
    if (int(P.Z()) == 2)
    {
      aPrjPS.Perform(t,
                     P.X(),
                     V0,
                     gp_Pnt2d(Tol, TolV),
                     gp_Pnt2d(t1, Surface->FirstVParameter()),
                     gp_Pnt2d(t2, Surface->LastVParameter()),
                     FuncTol);
      if (!aPrjPS.IsDone())
        continue;
      POnS   = aPrjPS.Solution();
      Sol    = gp_Pnt(POnS.X(), P.X(), POnS.Y());
      isDone = true;
      break;
    }
    else
    {
      aPrjPS.Perform(t,
                     U0,
                     P.X(),
                     gp_Pnt2d(Tol, TolU),
                     gp_Pnt2d(t1, Surface->FirstUParameter()),
                     gp_Pnt2d(t2, Surface->LastUParameter()),
                     FuncTol);
      if (!aPrjPS.IsDone())
        continue;
      POnS   = aPrjPS.Solution();
      Sol    = gp_Pnt(POnS.X(), POnS.Y(), P.X());
      isDone = true;
      break;
    }
  }

  return isDone;
}

//=======================================================================
// function : DichExactBound
// purpose  : computes exact boundary point
//=======================================================================

static void DichExactBound(gp_Pnt&                               Sol,
                           const double                          NotSol,
                           const double                          Tol,
                           const double                          TolU,
                           const double                          TolV,
                           const occ::handle<Adaptor3d_Curve>&   Curve,
                           const occ::handle<Adaptor3d_Surface>& Surface)
{
#ifdef OCCT_DEBUG_CHRONO
  InitChron(chr_dicho_bound);
#endif

  double   U0, V0, t;
  gp_Pnt2d POnS;
  U0 = Sol.Y();
  V0 = Sol.Z();
  ProjLib_PrjResolve aPrjPS(*Curve, *Surface, 1);

  double aNotSol = NotSol;
  while (fabs(Sol.X() - aNotSol) > Tol)
  {
    t = (Sol.X() + aNotSol) / 2;
    aPrjPS.Perform(t,
                   U0,
                   V0,
                   gp_Pnt2d(TolU, TolV),
                   gp_Pnt2d(Surface->FirstUParameter(), Surface->FirstVParameter()),
                   gp_Pnt2d(Surface->LastUParameter(), Surface->LastVParameter()),
                   FuncTol,
                   true);

    if (aPrjPS.IsDone())
    {
      POnS = aPrjPS.Solution();
      Sol  = gp_Pnt(t, POnS.X(), POnS.Y());
      U0   = Sol.Y();
      V0   = Sol.Z();
    }
    else
      aNotSol = t;
  }
#ifdef OCCT_DEBUG_CHRONO
  ResultChron(chr_dicho_bound, t_dicho_bound);
  dicho_bound_count++;
#endif
}

//=================================================================================================

static bool InitialPoint(const gp_Pnt&                         Point,
                         const double                          t,
                         const occ::handle<Adaptor3d_Curve>&   C,
                         const occ::handle<Adaptor3d_Surface>& S,
                         const double                          TolU,
                         const double                          TolV,
                         double&                               U,
                         double&                               V,
                         double                                theMaxDist)
{

  ProjLib_PrjResolve aPrjPS(*C, *S, 1);
  double             ParU, ParV;
  Extrema_ExtPS      aExtPS;
  aExtPS.Initialize(*S,
                    S->FirstUParameter(),
                    S->LastUParameter(),
                    S->FirstVParameter(),
                    S->LastVParameter(),
                    TolU,
                    TolV);

  aExtPS.Perform(Point);
  int    argmin   = 0;
  double aMaxDist = theMaxDist;
  if (aMaxDist > 0.)
  {
    aMaxDist *= aMaxDist;
  }
  if (aExtPS.IsDone() && aExtPS.NbExt())
  {
    int i, Nend;
    // Search for the nearest solution which is also a normal projection
    Nend = aExtPS.NbExt();
    for (i = 1; i <= Nend; i++)
    {
      if (aMaxDist > 0. && aMaxDist < aExtPS.SquareDistance(i))
      {
        continue;
      }
      Extrema_POnSurf POnS = aExtPS.Point(i);
      POnS.Parameter(ParU, ParV);
      aPrjPS.Perform(t,
                     ParU,
                     ParV,
                     gp_Pnt2d(TolU, TolV),
                     gp_Pnt2d(S->FirstUParameter(), S->FirstVParameter()),
                     gp_Pnt2d(S->LastUParameter(), S->LastVParameter()),
                     FuncTol,
                     true);
      if (aPrjPS.IsDone())
        if (argmin == 0 || aExtPS.SquareDistance(i) < aExtPS.SquareDistance(argmin))
          argmin = i;
    }
  }
  if (argmin == 0)
    return false;
  else
  {
    Extrema_POnSurf POnS = aExtPS.Point(argmin);
    POnS.Parameter(U, V);
    return true;
  }
}

//=================================================================================================

ProjLib_CompProjectedCurve::ProjLib_CompProjectedCurve()
    : myNbCurves(0),
      myMaxDist(0.0),
      myTolU(0.0),
      myTolV(0.0)
{
}

//=================================================================================================

ProjLib_CompProjectedCurve::ProjLib_CompProjectedCurve(
  const occ::handle<Adaptor3d_Surface>& theSurface,
  const occ::handle<Adaptor3d_Curve>&   theCurve,
  const double                          theTolU,
  const double                          theTolV)
    : mySurface(theSurface),
      myCurve(theCurve),
      myNbCurves(0),
      mySequence(new NCollection_HSequence<occ::handle<NCollection_HSequence<gp_Pnt>>>()),
      myTol3d(1.e-6),
      myContinuity(GeomAbs_C2),
      myMaxDegree(14),
      myMaxSeg(16),
      myProj2d(true),
      myProj3d(false),
      myMaxDist(-1.0),
      myTolU(theTolU),
      myTolV(theTolV)
{
  Init();
}

//=================================================================================================

ProjLib_CompProjectedCurve::ProjLib_CompProjectedCurve(
  const occ::handle<Adaptor3d_Surface>& theSurface,
  const occ::handle<Adaptor3d_Curve>&   theCurve,
  const double                          theTolU,
  const double                          theTolV,
  const double                          theMaxDist)
    : mySurface(theSurface),
      myCurve(theCurve),
      myNbCurves(0),
      mySequence(new NCollection_HSequence<occ::handle<NCollection_HSequence<gp_Pnt>>>()),
      myTol3d(1.e-6),
      myContinuity(GeomAbs_C2),
      myMaxDegree(14),
      myMaxSeg(16),
      myProj2d(true),
      myProj3d(false),
      myMaxDist(theMaxDist),
      myTolU(theTolU),
      myTolV(theTolV)
{
  Init();
}

//=================================================================================================

ProjLib_CompProjectedCurve::ProjLib_CompProjectedCurve(
  const double                          theTol3d,
  const occ::handle<Adaptor3d_Surface>& theSurface,
  const occ::handle<Adaptor3d_Curve>&   theCurve,
  const double                          theMaxDist)
    : mySurface(theSurface),
      myCurve(theCurve),
      myNbCurves(0),
      mySequence(new NCollection_HSequence<occ::handle<NCollection_HSequence<gp_Pnt>>>()),
      myTol3d(theTol3d),
      myContinuity(GeomAbs_C2),
      myMaxDegree(14),
      myMaxSeg(16),
      myProj2d(true),
      myProj3d(false),
      myMaxDist(theMaxDist)
{
  myTolU = std::max(Precision::PConfusion(), mySurface->UResolution(theTol3d));
  myTolV = std::max(Precision::PConfusion(), mySurface->VResolution(theTol3d));

  Init();
}

//=================================================================================================

occ::handle<Adaptor2d_Curve2d> ProjLib_CompProjectedCurve::ShallowCopy() const
{
  occ::handle<ProjLib_CompProjectedCurve> aCopy = new ProjLib_CompProjectedCurve();

  if (!mySurface.IsNull())
  {
    aCopy->mySurface = mySurface->ShallowCopy();
  }
  if (!myCurve.IsNull())
  {
    aCopy->myCurve = myCurve->ShallowCopy();
  }
  aCopy->myNbCurves    = myNbCurves;
  aCopy->mySequence    = mySequence;
  aCopy->myTolU        = myTolU;
  aCopy->myTolV        = myTolV;
  aCopy->myMaxDist     = myMaxDist;
  aCopy->myUIso        = myUIso;
  aCopy->myVIso        = myVIso;
  aCopy->mySnglPnts    = mySnglPnts;
  aCopy->myMaxDistance = myMaxDistance;

  return aCopy;
}

//=================================================================================================

void ProjLib_CompProjectedCurve::Init()
{
  myTabInt.Nullify();
  NCollection_Vector<double> aSplits;
  aSplits.Clear();

  double Tol; // Tolerance for ExactBound
  int    i, Nend = 0, aSplitIdx = 0;
  bool   FromLastU = false, isSplitsComputed = false;

  constexpr double aTolExt = Precision::PConfusion();
  Extrema_ExtCS    CExt(*myCurve, *mySurface, aTolExt, aTolExt);
  if (CExt.IsDone() && CExt.NbExt())
  {
    // Search for the minimum solution.
    // Avoid usage of extrema result that can be wrong for extrusion.
    if (myMaxDist > 0 &&

        mySurface->GetType() != GeomAbs_SurfaceOfExtrusion)
    {
      double min_val2;
      min_val2 = CExt.SquareDistance(1);

      Nend = CExt.NbExt();
      for (i = 2; i <= Nend; i++)
      {
        if (CExt.SquareDistance(i) < min_val2)
          min_val2 = CExt.SquareDistance(i);
      }
      if (min_val2 > myMaxDist * myMaxDist)
        return; // No near solution -> exit.
    }
  }

  double FirstU, LastU, Step, SearchStep, WalkStep, t;

  FirstU                     = myCurve->FirstParameter();
  LastU                      = myCurve->LastParameter();
  const double GlobalMinStep = 1.e-4;
  //<GlobalMinStep> is sufficiently small to provide solving from initial point
  // and, on the other hand, it is sufficiently large to avoid too close solutions.
  const double MinStep = 0.01 * (LastU - FirstU), MaxStep = 0.1 * (LastU - FirstU);
  SearchStep = 10 * MinStep;
  Step       = SearchStep;

  gp_Pnt2d           aLowBorder(mySurface->FirstUParameter(), mySurface->FirstVParameter());
  gp_Pnt2d           aUppBorder(mySurface->LastUParameter(), mySurface->LastVParameter());
  gp_Pnt2d           aTol(myTolU, myTolV);
  ProjLib_PrjResolve aPrjPS(*myCurve, *mySurface, 1);

  t = FirstU;
  bool   new_part;
  double prevDeb = 0.;
  bool   SameDeb = false;

  gp_Pnt Triple, prevTriple;

  // Basic loop
  while (t <= LastU)
  {
    // Search for the beginning of a new continuous part
    // to avoid infinite computation in some difficult cases.
    new_part = false;
    if (t > FirstU && std::abs(t - prevDeb) <= Precision::PConfusion())
      SameDeb = true;
    while (t <= LastU && !new_part && !FromLastU && !SameDeb)
    {
      prevDeb = t;
      if (t == LastU)
        FromLastU = true;
      bool   initpoint = false;
      double U = 0., V = 0.;
      gp_Pnt CPoint;
      double ParT, ParU, ParV;

      // Search an initial point in the list of Extrema Curve-Surface
      if (Nend != 0 && !CExt.IsParallel())
      {
        for (i = 1; i <= Nend; i++)
        {
          Extrema_POnCurv P1;
          Extrema_POnSurf P2;
          CExt.Points(i, P1, P2);
          ParT = P1.Parameter();
          P2.Parameter(ParU, ParV);

          aPrjPS.Perform(ParT, ParU, ParV, aTol, aLowBorder, aUppBorder, FuncTol, true);

          if (aPrjPS.IsDone()
              && P1.Parameter() > std::max(FirstU, t - Step + Precision::PConfusion())
              && P1.Parameter() <= t)
          {
            t         = ParT;
            U         = ParU;
            V         = ParV;
            CPoint    = P1.Value();
            initpoint = true;
            break;
          }
        }
      }
      if (!initpoint)
      {
        myCurve->D0(t, CPoint);
#ifdef OCCT_DEBUG_CHRONO
        InitChron(chr_init_point);
#endif
        // PConfusion - use geometric tolerances in extrema / optimization.
        initpoint = InitialPoint(CPoint, t, myCurve, mySurface, myTolU, myTolV, U, V, myMaxDist);
#ifdef OCCT_DEBUG_CHRONO
        ResultChron(chr_init_point, t_init_point);
        init_point_count++;
#endif
      }
      if (initpoint)
      {
        // When U or V lie on surface joint in some cases we cannot use them
        // as initial point for aPrjPS, so we switch them
        gp_Vec2d D;

        if ((mySurface->IsUPeriodic()
             && std::abs(aUppBorder.X() - aLowBorder.X() - mySurface->UPeriod())
                  < Precision::Confusion())
            || (mySurface->IsVPeriodic()
                && std::abs(aUppBorder.Y() - aLowBorder.Y() - mySurface->VPeriod())
                     < Precision::Confusion()))
        {
          if ((std::abs(U - aLowBorder.X()) < mySurface->UResolution(Precision::PConfusion()))
              && mySurface->IsUPeriodic())
          {
            d1(t, U, V, D, myCurve, mySurface);
            if (D.X() < 0)
              U = aUppBorder.X();
          }
          else if ((std::abs(U - aUppBorder.X()) < mySurface->UResolution(Precision::PConfusion()))
                   && mySurface->IsUPeriodic())
          {
            d1(t, U, V, D, myCurve, mySurface);
            if (D.X() > 0)
              U = aLowBorder.X();
          }

          if ((std::abs(V - aLowBorder.Y()) < mySurface->VResolution(Precision::PConfusion()))
              && mySurface->IsVPeriodic())
          {
            d1(t, U, V, D, myCurve, mySurface);
            if (D.Y() < 0)
              V = aUppBorder.Y();
          }
          else if ((std::abs(V - aUppBorder.Y()) <= mySurface->VResolution(Precision::PConfusion()))
                   && mySurface->IsVPeriodic())
          {
            d1(t, U, V, D, myCurve, mySurface);
            if (D.Y() > 0)
              V = aLowBorder.Y();
          }
        }

        if (myMaxDist > 0)
        {
          // Here we are going to stop if the distance between projection and
          // corresponding curve point is greater than myMaxDist
          gp_Pnt POnS;
          double d;
          mySurface->D0(U, V, POnS);
          d = CPoint.Distance(POnS);
          if (d > myMaxDist)
          {
            mySequence->Clear();
            myNbCurves = 0;
            return;
          }
        }
        Triple = gp_Pnt(t, U, V);
        if (t != FirstU)
        {
          // Search for exact boundary point
          Tol = std::min(myTolU, myTolV);
          gp_Vec2d aD;
          d1(Triple.X(), Triple.Y(), Triple.Z(), aD, myCurve, mySurface);
          Tol /= std::max(std::abs(aD.X()), std::abs(aD.Y()));

          if (!ExactBound(Triple, t - Step, Tol, myTolU, myTolV, myCurve, mySurface))
          {
#ifdef OCCT_DEBUG
            std::cout << "There is a problem with ExactBound computation" << std::endl;
#endif
            DichExactBound(Triple, t - Step, Tol, myTolU, myTolV, myCurve, mySurface);
          }
        }
        new_part = true;
      }
      else
      {
        if (t == LastU)
          break;
        t += Step;
        if (t > LastU)
        {
          Step = Step + LastU - t;
          t    = LastU;
        }
      }
    }
    if (!new_part)
      break;

    // We have found a new continuous part
    occ::handle<NCollection_HSequence<gp_Pnt>> hSeq = new NCollection_HSequence<gp_Pnt>();
    mySequence->Append(hSeq);
    myNbCurves++;
    mySequence->Value(myNbCurves)->Append(Triple);
    prevTriple = Triple;

    if (Triple.X() == LastU)
      break; // return;

    // Computation of WalkStep
    gp_Vec D1, D2;
    double MagnD1, MagnD2;
    d2CurvOnSurf(Triple.X(), Triple.Y(), Triple.Z(), D1, D2, myCurve, mySurface);
    MagnD1 = D1.Magnitude();
    MagnD2 = D2.Magnitude();
    if (MagnD2 < Precision::Confusion())
      WalkStep = MaxStep;
    else
      WalkStep = std::min(MaxStep, std::max(MinStep, 0.1 * MagnD1 / MagnD2));

    Step = WalkStep;

    t = Triple.X() + Step;
    if (t > LastU)
      t = LastU;
    double prevStep = Step;
    double U0, V0;

    // Here we are trying to prolong continuous part
    while (t <= LastU && new_part)
    {

      U0 = Triple.Y() + (Step / prevStep) * (Triple.Y() - prevTriple.Y());
      V0 = Triple.Z() + (Step / prevStep) * (Triple.Z() - prevTriple.Z());
      // adjust U0 to be in [mySurface->FirstUParameter(),mySurface->LastUParameter()]
      U0 = std::min(std::max(U0, aLowBorder.X()), aUppBorder.X());
      // adjust V0 to be in [mySurface->FirstVParameter(),mySurface->LastVParameter()]
      V0 = std::min(std::max(V0, aLowBorder.Y()), aUppBorder.Y());

      aPrjPS.Perform(t, U0, V0, aTol, aLowBorder, aUppBorder, FuncTol, true);
      if (!aPrjPS.IsDone())
      {
        if (Step <= GlobalMinStep)
        {
          // Search for exact boundary point
          Tol = std::min(myTolU, myTolV);
          gp_Vec2d D;
          d1(Triple.X(), Triple.Y(), Triple.Z(), D, myCurve, mySurface);
          Tol /= std::max(std::abs(D.X()), std::abs(D.Y()));

          if (!ExactBound(Triple, t, Tol, myTolU, myTolV, myCurve, mySurface))
          {
#ifdef OCCT_DEBUG
            std::cout << "There is a problem with ExactBound computation" << std::endl;
#endif
            DichExactBound(Triple, t, Tol, myTolU, myTolV, myCurve, mySurface);
          }

          if ((Triple.X()
               - mySequence->Value(myNbCurves)->Value(mySequence->Value(myNbCurves)->Length()).X())
              > 1.e-10)
            mySequence->Value(myNbCurves)->Append(Triple);
          if ((LastU - Triple.X()) < Tol)
          {
            t = LastU + 1;
            break;
          } // return;

          Step = SearchStep;
          t    = Triple.X() + Step;
          if (t > (LastU - MinStep / 2))
          {
            Step = Step + LastU - t;
            t    = LastU;
          }
          new_part = false;
        }
        else
        {
          // decrease step
          double SaveStep = Step;
          Step /= 2.;
          t = Triple.X() + Step;
          if (t > (LastU - MinStep / 4))
          {
            Step = Step + LastU - t;
            if (std::abs(Step - SaveStep) <= Precision::PConfusion())
              Step = GlobalMinStep; // to avoid looping
            t = LastU;
          }
        }
      }
      // Go further
      else
      {
        prevTriple = Triple;
        prevStep   = Step;
        Triple     = gp_Pnt(t, aPrjPS.Solution().X(), aPrjPS.Solution().Y());

        // Check for possible local traps.
        UpdateTripleByTrapCriteria(Triple);

        // Protection from case when the whole curve lies on a seam.
        if (!isSplitsComputed)
        {
          bool isUPossible = false;
          if (mySurface->IsUPeriodic()
              && (std::abs(Triple.Y() - mySurface->FirstUParameter()) > Precision::PConfusion()
                  && std::abs(Triple.Y() - mySurface->LastUParameter()) > Precision::PConfusion()))
          {
            isUPossible = true;
          }

          bool isVPossible = false;
          if (mySurface->IsVPeriodic()
              && (std::abs(Triple.Z() - mySurface->FirstVParameter()) > Precision::PConfusion()
                  && std::abs(Triple.Z() - mySurface->LastVParameter()) > Precision::PConfusion()))
          {
            isVPossible = true;
          }

          if (isUPossible || isVPossible)
          {
            // When point is good conditioned.
            BuildCurveSplits(myCurve, mySurface, myTolU, myTolV, aSplits);
            isSplitsComputed = true;
          }
        }

        if ((Triple.X()
             - mySequence->Value(myNbCurves)->Value(mySequence->Value(myNbCurves)->Length()).X())
            > 1.e-10)
          mySequence->Value(myNbCurves)->Append(Triple);
        if (t == LastU)
        {
          t = LastU + 1;
          break;
        } // return;
        // Computation of WalkStep
        d2CurvOnSurf(Triple.X(), Triple.Y(), Triple.Z(), D1, D2, myCurve, mySurface);
        MagnD1 = D1.Magnitude();
        MagnD2 = D2.Magnitude();
        if (MagnD2 < Precision::Confusion())
          WalkStep = MaxStep;
        else
          WalkStep = std::min(MaxStep, std::max(MinStep, 0.1 * MagnD1 / MagnD2));

        Step = WalkStep;
        t += Step;
        if (t > (LastU - MinStep / 2))
        {
          Step = Step + LastU - t;
          t    = LastU;
        }

        // We assume at least one point of cache inside of a split.
        const int aSize = aSplits.Size();
        for (int anIdx = aSplitIdx; anIdx < aSize; ++anIdx)
        {
          const double aParam = aSplits(anIdx);
          if (std::abs(aParam - Triple.X()) < Precision::PConfusion())
          {
            // The current point is equal to a split point.
            new_part = false;

            // Move split index to avoid check of the whole list.
            ++aSplitIdx;
            break;
          }
          else if (aParam < t + Precision::PConfusion())
          {
            // The next point crosses the split point.
            t    = aParam;
            Step = t - prevTriple.X();
          }
        } // for(int anIdx = aSplitIdx; anIdx < aSize; ++anIdx)
      }
    }
  }

  // Sequence post-proceeding.
  int j;

  // 1. Removing poor parts
  int NbPart = myNbCurves;
  int ipart  = 1;
  for (i = 1; i <= NbPart; i++)
  {
    //    int NbPoints = mySequence->Value(i)->Length();
    if (mySequence->Value(ipart)->Length() < 2)
    {
      mySequence->Remove(ipart);
      myNbCurves--;
    }
    else
      ipart++;
  }

  if (myNbCurves == 0)
    return;

  // 2. Removing common parts of bounds
  for (i = 1; i < myNbCurves; i++)
  {
    if (mySequence->Value(i)->Value(mySequence->Value(i)->Length()).X()
        >= mySequence->Value(i + 1)->Value(1).X())
    {
      mySequence->ChangeValue(i + 1)->ChangeValue(1).SetX(
        mySequence->Value(i)->Value(mySequence->Value(i)->Length()).X() + 1.e-12);
    }
  }

  // 3. Computation of the maximum distance from each part of curve to surface

  myMaxDistance = new NCollection_HArray1<double>(1, myNbCurves);
  myMaxDistance->Init(0);
  for (i = 1; i <= myNbCurves; i++)
  {
    for (j = 1; j <= mySequence->Value(i)->Length(); j++)
    {
      gp_Pnt POnC, POnS, aTriple;
      double Distance;
      aTriple = mySequence->Value(i)->Value(j);
      myCurve->D0(aTriple.X(), POnC);
      mySurface->D0(aTriple.Y(), aTriple.Z(), POnS);
      Distance = POnC.Distance(POnS);
      if (myMaxDistance->Value(i) < Distance)
      {
        myMaxDistance->ChangeValue(i) = Distance;
      }
    }
  }

  // 4. Check the projection to be a single point

  gp_Pnt2d Pmoy, Pcurr, P;
  double   AveU, AveV;
  mySnglPnts = new NCollection_HArray1<bool>(1, myNbCurves);
  mySnglPnts->Init(true);

  for (i = 1; i <= myNbCurves; i++)
  {
    // compute an average U and V

    for (j = 1, AveU = 0., AveV = 0.; j <= mySequence->Value(i)->Length(); j++)
    {
      AveU += mySequence->Value(i)->Value(j).Y();
      AveV += mySequence->Value(i)->Value(j).Z();
    }
    AveU /= mySequence->Value(i)->Length();
    AveV /= mySequence->Value(i)->Length();

    Pmoy.SetCoord(AveU, AveV);
    for (j = 1; j <= mySequence->Value(i)->Length(); j++)
    {
      Pcurr = gp_Pnt2d(mySequence->Value(i)->Value(j).Y(), mySequence->Value(i)->Value(j).Z());
      if (Pcurr.Distance(Pmoy) > ((myTolU < myTolV) ? myTolV : myTolU))
      {
        mySnglPnts->SetValue(i, false);
        break;
      }
    }
  }

  // 5. Check the projection to be an isoparametric curve of the surface

  myUIso = new NCollection_HArray1<bool>(1, myNbCurves);
  myUIso->Init(true);

  myVIso = new NCollection_HArray1<bool>(1, myNbCurves);
  myVIso->Init(true);

  for (i = 1; i <= myNbCurves; i++)
  {
    if (IsSinglePnt(i, P) || mySequence->Value(i)->Length() <= 2)
    {
      myUIso->SetValue(i, false);
      myVIso->SetValue(i, false);
      continue;
    }

    // new test for isoparametrics

    if (mySequence->Value(i)->Length() > 2)
    {
      // compute an average U and V

      for (j = 1, AveU = 0., AveV = 0.; j <= mySequence->Value(i)->Length(); j++)
      {
        AveU += mySequence->Value(i)->Value(j).Y();
        AveV += mySequence->Value(i)->Value(j).Z();
      }
      AveU /= mySequence->Value(i)->Length();
      AveV /= mySequence->Value(i)->Length();

      // is i-part U-isoparametric ?
      for (j = 1; j <= mySequence->Value(i)->Length(); j++)
      {
        if (std::abs(mySequence->Value(i)->Value(j).Y() - AveU) > myTolU)
        {
          myUIso->SetValue(i, false);
          break;
        }
      }

      // is i-part V-isoparametric ?
      for (j = 1; j <= mySequence->Value(i)->Length(); j++)
      {
        if (std::abs(mySequence->Value(i)->Value(j).Z() - AveV) > myTolV)
        {
          myVIso->SetValue(i, false);
          break;
        }
      }
      //
    }
  }
}

//=================================================================================================

void ProjLib_CompProjectedCurve::Perform()
{
  if (myNbCurves == 0)
    return;

  bool                           approx2d = myProj2d;
  bool                           approx3d = myProj3d;
  double                         Udeb, Ufin, UIso, VIso;
  gp_Pnt2d                       P2d, Pdeb, Pfin;
  gp_Pnt                         P;
  occ::handle<Adaptor2d_Curve2d> HPCur;
  occ::handle<Adaptor3d_Surface> HS = mySurface->ShallowCopy(); // For expand bounds of surface
  occ::handle<Geom2d_Curve>      PCur2d;                        // Only for isoparametric projection
  occ::handle<Geom_Curve>        PCur3d;

  if (myProj2d == true)
  {
    myResult2dPoint = new NCollection_HArray1<gp_Pnt2d>(1, myNbCurves);
    myResult2dCurve = new NCollection_HArray1<occ::handle<Geom2d_Curve>>(1, myNbCurves);
  }

  if (myProj3d == true)
  {
    myResult3dPoint = new NCollection_HArray1<gp_Pnt>(1, myNbCurves);
    myResult3dCurve = new NCollection_HArray1<occ::handle<Geom_Curve>>(1, myNbCurves);
  }

  myResultIsPoint = new NCollection_HArray1<bool>(1, myNbCurves);
  myResultIsPoint->Init(false);

  myResult3dApproxError = new NCollection_HArray1<double>(1, myNbCurves);
  myResult3dApproxError->Init(0.0);

  myResult2dUApproxError = new NCollection_HArray1<double>(1, myNbCurves);
  myResult2dUApproxError->Init(0.0);

  myResult2dVApproxError = new NCollection_HArray1<double>(1, myNbCurves);
  myResult2dVApproxError->Init(0.0);

  for (int k = 1; k <= myNbCurves; k++)
  {
    if (IsSinglePnt(k, P2d)) // Part k of the projection is punctual
    {
      GetSurface()->D0(P2d.X(), P2d.Y(), P);
      if (myProj2d == true)
      {
        myResult2dPoint->SetValue(k, P2d);
      }
      if (myProj3d == true)
      {
        myResult3dPoint->SetValue(k, P);
      }
      myResultIsPoint->SetValue(k, true);
    }
    else
    {
      Bounds(k, Udeb, Ufin);
      gp_Dir2d Dir; // Only for isoparametric projection

      if (IsUIso(k, UIso)) // Part k of the projection is U-isoparametric curve
      {
        approx2d = false;

        D0(Udeb, Pdeb);
        D0(Ufin, Pfin);
        Udeb = Pdeb.Y();
        Ufin = Pfin.Y();
        if (Udeb > Ufin)
        {
          Dir  = gp_Dir2d(gp_Dir2d::D::NY);
          Udeb = -Udeb;
          Ufin = -Ufin;
        }
        else
          Dir = gp_Dir2d(gp_Dir2d::D::Y);
        PCur2d = new Geom2d_TrimmedCurve(new Geom2d_Line(gp_Pnt2d(UIso, 0), Dir), Udeb, Ufin);
        HPCur  = new Geom2dAdaptor_Curve(PCur2d);
      }
      else if (IsVIso(k, VIso)) // Part k of the projection is V-isoparametric curve
      {
        approx2d = false;

        D0(Udeb, Pdeb);
        D0(Ufin, Pfin);
        Udeb = Pdeb.X();
        Ufin = Pfin.X();
        if (Udeb > Ufin)
        {
          Dir  = gp_Dir2d(gp_Dir2d::D::NX);
          Udeb = -Udeb;
          Ufin = -Ufin;
        }
        else
          Dir = gp_Dir2d(gp_Dir2d::D::X);
        PCur2d = new Geom2d_TrimmedCurve(new Geom2d_Line(gp_Pnt2d(0, VIso), Dir), Udeb, Ufin);
        HPCur  = new Geom2dAdaptor_Curve(PCur2d);
      }
      else
      {
        if (!mySurface->IsUPeriodic())
        {
          double U1, U2;
          double dU = 10. * myTolU;

          U1 = mySurface->FirstUParameter();
          U2 = mySurface->LastUParameter();
          U1 -= dU;
          U2 += dU;

          HS = HS->UTrim(U1, U2, 0.0);
        }

        if (!mySurface->IsVPeriodic())
        {
          double V1, V2;
          double dV = 10. * myTolV;

          V1 = mySurface->FirstVParameter();
          V2 = mySurface->LastVParameter();
          V1 -= dV;
          V2 += dV;

          HS = HS->VTrim(V1, V2, 0.0);
        }

        occ::handle<ProjLib_CompProjectedCurve> HP =
          occ::down_cast<ProjLib_CompProjectedCurve>(this->ShallowCopy());
        HP->Load(HS);
        HPCur = HP;
      }

      if (approx2d || approx3d)
      {
        bool only2d, only3d;
        if (approx2d && approx3d)
        {
          only2d = !approx2d;
          only3d = !approx3d;
        }
        else
        {
          only2d = approx2d;
          only3d = approx3d;
        }

        Approx_CurveOnSurface appr(HPCur, HS, Udeb, Ufin, myTol3d);
        appr.Perform(myMaxSeg, myMaxDegree, myContinuity, only3d, only2d);

        if (approx2d)
        {
          PCur2d = appr.Curve2d();
          myResult2dUApproxError->SetValue(k, appr.MaxError2dU());
          myResult2dVApproxError->SetValue(k, appr.MaxError2dV());
        }

        if (approx3d)
        {
          PCur3d = appr.Curve3d();
          myResult3dApproxError->SetValue(k, appr.MaxError3d());
        }
      }

      if (myProj2d == true)
      {
        myResult2dCurve->SetValue(k, PCur2d);
      }

      if (myProj3d == true)
      {
        myResult3dCurve->SetValue(k, PCur3d);
      }
    }
  }
}

//=================================================================================================

void ProjLib_CompProjectedCurve::SetTol3d(const double theTol3d)
{
  myTol3d = theTol3d;
}

//=================================================================================================

void ProjLib_CompProjectedCurve::SetContinuity(const GeomAbs_Shape theContinuity)
{
  myContinuity = theContinuity;
}

//=================================================================================================

void ProjLib_CompProjectedCurve::SetMaxDegree(const int theMaxDegree)
{
  if (theMaxDegree < 1)
    return;
  myMaxDegree = theMaxDegree;
}

//=================================================================================================

void ProjLib_CompProjectedCurve::SetMaxSeg(const int theMaxSeg)
{
  if (theMaxSeg < 1)
    return;
  myMaxSeg = theMaxSeg;
}

//=================================================================================================

void ProjLib_CompProjectedCurve::SetProj3d(const bool theProj3d)
{
  myProj3d = theProj3d;
}

//=================================================================================================

void ProjLib_CompProjectedCurve::SetProj2d(const bool theProj2d)
{
  myProj2d = theProj2d;
}

//=================================================================================================

void ProjLib_CompProjectedCurve::Load(const occ::handle<Adaptor3d_Surface>& S)
{
  mySurface = S;
}

//=================================================================================================

void ProjLib_CompProjectedCurve::Load(const occ::handle<Adaptor3d_Curve>& C)
{
  myCurve = C;
}

//=================================================================================================

const occ::handle<Adaptor3d_Surface>& ProjLib_CompProjectedCurve::GetSurface() const
{
  return mySurface;
}

//=================================================================================================

const occ::handle<Adaptor3d_Curve>& ProjLib_CompProjectedCurve::GetCurve() const
{
  return myCurve;
}

//=================================================================================================

void ProjLib_CompProjectedCurve::GetTolerance(double& TolU, double& TolV) const
{
  TolU = myTolU;
  TolV = myTolV;
}

//=================================================================================================

int ProjLib_CompProjectedCurve::NbCurves() const
{
  return myNbCurves;
}

//=================================================================================================

void ProjLib_CompProjectedCurve::Bounds(const int Index, double& Udeb, double& Ufin) const
{
  if (Index < 1 || Index > myNbCurves)
    throw Standard_NoSuchObject();
  Udeb = mySequence->Value(Index)->Value(1).X();
  Ufin = mySequence->Value(Index)->Value(mySequence->Value(Index)->Length()).X();
}

//=================================================================================================

bool ProjLib_CompProjectedCurve::IsSinglePnt(const int Index, gp_Pnt2d& P) const
{
  if (Index < 1 || Index > myNbCurves)
    throw Standard_NoSuchObject();
  P = gp_Pnt2d(mySequence->Value(Index)->Value(1).Y(), mySequence->Value(Index)->Value(1).Z());
  return mySnglPnts->Value(Index);
}

//=================================================================================================

bool ProjLib_CompProjectedCurve::IsUIso(const int Index, double& U) const
{
  if (Index < 1 || Index > myNbCurves)
    throw Standard_NoSuchObject();
  U = mySequence->Value(Index)->Value(1).Y();
  return myUIso->Value(Index);
}

//=================================================================================================

bool ProjLib_CompProjectedCurve::IsVIso(const int Index, double& V) const
{
  if (Index < 1 || Index > myNbCurves)
    throw Standard_NoSuchObject();
  V = mySequence->Value(Index)->Value(1).Z();
  return myVIso->Value(Index);
}

//=================================================================================================

gp_Pnt2d ProjLib_CompProjectedCurve::Value(const double t) const
{
  gp_Pnt2d P;
  D0(t, P);
  return P;
}

//=================================================================================================

void ProjLib_CompProjectedCurve::D0(const double U, gp_Pnt2d& P) const
{
  int    i, j;
  double Udeb, Ufin;
  bool   found = false;

  for (i = 1; i <= myNbCurves; i++)
  {
    Bounds(i, Udeb, Ufin);
    if (U >= Udeb && U <= Ufin)
    {
      found = true;
      break;
    }
  }
  if (!found)
  {
    throw Standard_DomainError("ProjLib_CompProjectedCurve::D0");
  }

  double U0, V0;

  int End = mySequence->Value(i)->Length();
  for (j = 1; j < End; j++)
    if ((U >= mySequence->Value(i)->Value(j).X()) && (U <= mySequence->Value(i)->Value(j + 1).X()))
      break;

  //  U0 = mySequence->Value(i)->Value(j).Y();
  //  V0 = mySequence->Value(i)->Value(j).Z();

  //  Cubic Interpolation
  if (mySequence->Value(i)->Length() < 4
      || (std::abs(U - mySequence->Value(i)->Value(j).X()) <= Precision::PConfusion()))
  {
    U0 = mySequence->Value(i)->Value(j).Y();
    V0 = mySequence->Value(i)->Value(j).Z();
  }
  else if (std::abs(U - mySequence->Value(i)->Value(j + 1).X()) <= Precision::PConfusion())
  {
    U0 = mySequence->Value(i)->Value(j + 1).Y();
    V0 = mySequence->Value(i)->Value(j + 1).Z();
  }
  else
  {
    if (j == 1)
      j = 2;
    if (j > mySequence->Value(i)->Length() - 2)
      j = mySequence->Value(i)->Length() - 2;

    gp_Vec2d I1, I2, I3, I21, I22, I31, Y1, Y2, Y3, Y4, Res;
    double   X1, X2, X3, X4;

    X1 = mySequence->Value(i)->Value(j - 1).X();
    X2 = mySequence->Value(i)->Value(j).X();
    X3 = mySequence->Value(i)->Value(j + 1).X();
    X4 = mySequence->Value(i)->Value(j + 2).X();

    Y1 = gp_Vec2d(mySequence->Value(i)->Value(j - 1).Y(), mySequence->Value(i)->Value(j - 1).Z());
    Y2 = gp_Vec2d(mySequence->Value(i)->Value(j).Y(), mySequence->Value(i)->Value(j).Z());
    Y3 = gp_Vec2d(mySequence->Value(i)->Value(j + 1).Y(), mySequence->Value(i)->Value(j + 1).Z());
    Y4 = gp_Vec2d(mySequence->Value(i)->Value(j + 2).Y(), mySequence->Value(i)->Value(j + 2).Z());

    I1 = (Y1 - Y2) / (X1 - X2);
    I2 = (Y2 - Y3) / (X2 - X3);
    I3 = (Y3 - Y4) / (X3 - X4);

    I21 = (I1 - I2) / (X1 - X3);
    I22 = (I2 - I3) / (X2 - X4);

    I31 = (I21 - I22) / (X1 - X4);

    Res = Y1 + (U - X1) * (I1 + (U - X2) * (I21 + (U - X3) * I31));

    U0 = Res.X();
    V0 = Res.Y();

    if (U0 < mySurface->FirstUParameter())
      U0 = mySurface->FirstUParameter();
    else if (U0 > mySurface->LastUParameter())
      U0 = mySurface->LastUParameter();

    if (V0 < mySurface->FirstVParameter())
      V0 = mySurface->FirstVParameter();
    else if (V0 > mySurface->LastVParameter())
      V0 = mySurface->LastVParameter();
  }
  // End of cubic interpolation

  ProjLib_PrjResolve aPrjPS(*myCurve, *mySurface, 1);
  aPrjPS.Perform(U,
                 U0,
                 V0,
                 gp_Pnt2d(myTolU, myTolV),
                 gp_Pnt2d(mySurface->FirstUParameter(), mySurface->FirstVParameter()),
                 gp_Pnt2d(mySurface->LastUParameter(), mySurface->LastVParameter()),
                 FuncTol);
  if (aPrjPS.IsDone())
    P = aPrjPS.Solution();
  else
  {
    gp_Pnt        thePoint = myCurve->Value(U);
    Extrema_ExtPS aExtPS(thePoint, *mySurface, myTolU, myTolV);
    if (aExtPS.IsDone() && aExtPS.NbExt())
    {
      int k, Nend, imin = 1;
      // Search for the nearest solution which is also a normal projection
      Nend = aExtPS.NbExt();
      for (k = 2; k <= Nend; k++)
        if (aExtPS.SquareDistance(k) < aExtPS.SquareDistance(imin))
          imin = k;
      const Extrema_POnSurf& POnS = aExtPS.Point(imin);
      double                 ParU, ParV;
      POnS.Parameter(ParU, ParV);
      P.SetCoord(ParU, ParV);
    }
    else
      P.SetCoord(U0, V0);
  }
}

//=================================================================================================

void ProjLib_CompProjectedCurve::D1(const double t, gp_Pnt2d& P, gp_Vec2d& V) const
{
  double u, v;
  D0(t, P);
  u = P.X();
  v = P.Y();
  d1(t, u, v, V, myCurve, mySurface);
}

//=================================================================================================

void ProjLib_CompProjectedCurve::D2(const double t, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2) const
{
  double u, v;
  D0(t, P);
  u = P.X();
  v = P.Y();
  d2(t, u, v, V1, V2, myCurve, mySurface);
}

//=================================================================================================

gp_Vec2d ProjLib_CompProjectedCurve::DN(const double t, const int N) const
{
  if (N < 1)
    throw Standard_OutOfRange("ProjLib_CompProjectedCurve : N must be greater than 0");
  else if (N == 1)
  {
    gp_Pnt2d P;
    gp_Vec2d V;
    D1(t, P, V);
    return V;
  }
  else if (N == 2)
  {
    gp_Pnt2d P;
    gp_Vec2d V1, V2;
    D2(t, P, V1, V2);
    return V2;
  }
  else if (N > 2)
    throw Standard_NotImplemented("ProjLib_CompProjectedCurve::DN");
  return gp_Vec2d();
}

//=================================================================================================

const occ::handle<NCollection_HSequence<occ::handle<NCollection_HSequence<gp_Pnt>>>>&
  ProjLib_CompProjectedCurve::GetSequence() const
{
  return mySequence;
}

//=================================================================================================

double ProjLib_CompProjectedCurve::FirstParameter() const
{
  return myCurve->FirstParameter();
}

//=================================================================================================

double ProjLib_CompProjectedCurve::LastParameter() const
{
  return myCurve->LastParameter();
}

//=================================================================================================

GeomAbs_Shape ProjLib_CompProjectedCurve::Continuity() const
{
  GeomAbs_Shape ContC  = myCurve->Continuity();
  GeomAbs_Shape ContSu = mySurface->UContinuity();
  if (ContSu < ContC)
    ContC = ContSu;
  GeomAbs_Shape ContSv = mySurface->VContinuity();
  if (ContSv < ContC)
    ContC = ContSv;

  return ContC;
}

//=================================================================================================

double ProjLib_CompProjectedCurve::MaxDistance(const int Index) const
{
  if (Index < 1 || Index > myNbCurves)
    throw Standard_NoSuchObject();
  return myMaxDistance->Value(Index);
}

//=================================================================================================

int ProjLib_CompProjectedCurve::NbIntervals(const GeomAbs_Shape S) const
{
  const_cast<ProjLib_CompProjectedCurve*>(this)->myTabInt.Nullify();
  BuildIntervals(S);
  return myTabInt->Length() - 1;
}

//=================================================================================================

void ProjLib_CompProjectedCurve::Intervals(NCollection_Array1<double>& T,
                                           const GeomAbs_Shape         S) const
{
  if (myTabInt.IsNull())
    BuildIntervals(S);
  T = myTabInt->Array1();
}

//=================================================================================================

void ProjLib_CompProjectedCurve::BuildIntervals(const GeomAbs_Shape S) const
{
  GeomAbs_Shape SforS = GeomAbs_CN;
  switch (S)
  {
    case GeomAbs_C0:
      SforS = GeomAbs_C1;
      break;
    case GeomAbs_C1:
      SforS = GeomAbs_C2;
      break;
    case GeomAbs_C2:
      SforS = GeomAbs_C3;
      break;
    case GeomAbs_C3:
      SforS = GeomAbs_CN;
      break;
    case GeomAbs_CN:
      SforS = GeomAbs_CN;
      break;
    default:
      throw Standard_OutOfRange();
  }
  int i, j, k;
  int NbIntCur  = myCurve->NbIntervals(S);
  int NbIntSurU = mySurface->NbUIntervals(SforS);
  int NbIntSurV = mySurface->NbVIntervals(SforS);

  NCollection_Array1<double> CutPntsT(1, NbIntCur + 1);
  NCollection_Array1<double> CutPntsU(1, NbIntSurU + 1);
  NCollection_Array1<double> CutPntsV(1, NbIntSurV + 1);

  myCurve->Intervals(CutPntsT, S);
  mySurface->UIntervals(CutPntsU, SforS);
  mySurface->VIntervals(CutPntsV, SforS);

  double Tl, Tr, Ul, Ur, Vl, Vr, Tol;

  occ::handle<NCollection_HArray1<double>> BArr = nullptr, CArr = nullptr, UArr = nullptr,
                                           VArr = nullptr;

  // processing projection bounds
  BArr = new NCollection_HArray1<double>(1, 2 * myNbCurves);
  for (i = 1; i <= myNbCurves; i++)
  {
    Bounds(i, BArr->ChangeValue(2 * i - 1), BArr->ChangeValue(2 * i));
  }

  // processing curve discontinuities
  if (NbIntCur > 1)
  {
    CArr = new NCollection_HArray1<double>(1, NbIntCur - 1);
    for (i = 1; i <= CArr->Length(); i++)
    {
      CArr->ChangeValue(i) = CutPntsT(i + 1);
    }
  }

  // processing U-surface discontinuities
  NCollection_Sequence<double> TUdisc;

  for (k = 2; k <= NbIntSurU; k++)
  {
    //    std::cout<<"CutPntsU("<<k<<") = "<<CutPntsU(k)<<std::endl;
    for (i = 1; i <= myNbCurves; i++)
    {
      for (j = 1; j < mySequence->Value(i)->Length(); j++)
      {
        Ul = mySequence->Value(i)->Value(j).Y();
        Ur = mySequence->Value(i)->Value(j + 1).Y();

        if (std::abs(Ul - CutPntsU(k)) <= myTolU)
          TUdisc.Append(mySequence->Value(i)->Value(j).X());
        else if (std::abs(Ur - CutPntsU(k)) <= myTolU)
          TUdisc.Append(mySequence->Value(i)->Value(j + 1).X());
        else if ((Ul < CutPntsU(k) && CutPntsU(k) < Ur) || (Ur < CutPntsU(k) && CutPntsU(k) < Ul))
        {
          double V;
          V = (mySequence->Value(i)->Value(j).Z() + mySequence->Value(i)->Value(j + 1).Z()) / 2;
          ProjLib_PrjResolve Solver(*myCurve, *mySurface, 2);

          gp_Vec2d D;
          gp_Pnt   Triple;
          Triple = mySequence->Value(i)->Value(j);
          d1(Triple.X(), Triple.Y(), Triple.Z(), D, myCurve, mySurface);
          if (std::abs(D.X()) < Precision::Confusion())
            Tol = myTolU;
          else
            Tol = std::min(myTolU, myTolU / std::abs(D.X()));

          Tl = mySequence->Value(i)->Value(j).X();
          Tr = mySequence->Value(i)->Value(j + 1).X();

          Solver.Perform((Tl + Tr) / 2,
                         CutPntsU(k),
                         V,
                         gp_Pnt2d(Tol, myTolV),
                         gp_Pnt2d(Tl, mySurface->FirstVParameter()),
                         gp_Pnt2d(Tr, mySurface->LastVParameter()),
                         FuncTol);
          //
          if (Solver.IsDone())
          {
            TUdisc.Append(Solver.Solution().X());
          }
        }
      }
    }
  }
  for (i = 2; i <= TUdisc.Length(); i++)
  {
    if (TUdisc(i) - TUdisc(i - 1) < Precision::PConfusion())
    {
      TUdisc.Remove(i--);
    }
  }

  if (TUdisc.Length())
  {
    UArr = new NCollection_HArray1<double>(1, TUdisc.Length());
    for (i = 1; i <= UArr->Length(); i++)
    {
      UArr->ChangeValue(i) = TUdisc(i);
    }
  }
  // processing V-surface discontinuities
  NCollection_Sequence<double> TVdisc;

  for (k = 2; k <= NbIntSurV; k++)
  {
    for (i = 1; i <= myNbCurves; i++)
    {
      //      std::cout<<"CutPntsV("<<k<<") = "<<CutPntsV(k)<<std::endl;
      for (j = 1; j < mySequence->Value(i)->Length(); j++)
      {

        Vl = mySequence->Value(i)->Value(j).Z();
        Vr = mySequence->Value(i)->Value(j + 1).Z();

        if (std::abs(Vl - CutPntsV(k)) <= myTolV)
          TVdisc.Append(mySequence->Value(i)->Value(j).X());
        else if (std::abs(Vr - CutPntsV(k)) <= myTolV)
          TVdisc.Append(mySequence->Value(i)->Value(j + 1).X());
        else if ((Vl < CutPntsV(k) && CutPntsV(k) < Vr) || (Vr < CutPntsV(k) && CutPntsV(k) < Vl))
        {
          double U;
          U = (mySequence->Value(i)->Value(j).Y() + mySequence->Value(i)->Value(j + 1).Y()) / 2;
          ProjLib_PrjResolve Solver(*myCurve, *mySurface, 3);

          gp_Vec2d D;
          gp_Pnt   Triple;
          Triple = mySequence->Value(i)->Value(j);
          d1(Triple.X(), Triple.Y(), Triple.Z(), D, myCurve, mySurface);
          if (std::abs(D.Y()) < Precision::Confusion())
            Tol = myTolV;
          else
            Tol = std::min(myTolV, myTolV / std::abs(D.Y()));

          Tl = mySequence->Value(i)->Value(j).X();
          Tr = mySequence->Value(i)->Value(j + 1).X();

          Solver.Perform((Tl + Tr) / 2,
                         U,
                         CutPntsV(k),
                         gp_Pnt2d(Tol, myTolV),
                         gp_Pnt2d(Tl, mySurface->FirstUParameter()),
                         gp_Pnt2d(Tr, mySurface->LastUParameter()),
                         FuncTol);
          //
          if (Solver.IsDone())
          {
            TVdisc.Append(Solver.Solution().X());
          }
        }
      }
    }
  }

  for (i = 2; i <= TVdisc.Length(); i++)
  {
    if (TVdisc(i) - TVdisc(i - 1) < Precision::PConfusion())
    {
      TVdisc.Remove(i--);
    }
  }

  if (TVdisc.Length())
  {
    VArr = new NCollection_HArray1<double>(1, TVdisc.Length());
    for (i = 1; i <= VArr->Length(); i++)
    {
      VArr->ChangeValue(i) = TVdisc(i);
    }
  }

  // fusion
  NCollection_Sequence<double> Fusion;
  if (!CArr.IsNull())
  {
    GeomLib::FuseIntervals(BArr->ChangeArray1(),
                           CArr->ChangeArray1(),
                           Fusion,
                           Precision::PConfusion());
    BArr = new NCollection_HArray1<double>(1, Fusion.Length());
    for (i = 1; i <= BArr->Length(); i++)
    {
      BArr->ChangeValue(i) = Fusion(i);
    }
    Fusion.Clear();
  }

  if (!UArr.IsNull())
  {
    GeomLib::FuseIntervals(BArr->ChangeArray1(),
                           UArr->ChangeArray1(),
                           Fusion,
                           Precision::PConfusion());
    BArr = new NCollection_HArray1<double>(1, Fusion.Length());
    for (i = 1; i <= BArr->Length(); i++)
    {
      BArr->ChangeValue(i) = Fusion(i);
    }
    Fusion.Clear();
  }

  if (!VArr.IsNull())
  {
    GeomLib::FuseIntervals(BArr->ChangeArray1(),
                           VArr->ChangeArray1(),
                           Fusion,
                           Precision::PConfusion());
    BArr = new NCollection_HArray1<double>(1, Fusion.Length());
    for (i = 1; i <= BArr->Length(); i++)
    {
      BArr->ChangeValue(i) = Fusion(i);
    }
  }

  const_cast<ProjLib_CompProjectedCurve*>(this)->myTabInt =
    new NCollection_HArray1<double>(1, BArr->Length());
  for (i = 1; i <= BArr->Length(); i++)
  {
    myTabInt->ChangeValue(i) = BArr->Value(i);
  }
}

//=================================================================================================

occ::handle<Adaptor2d_Curve2d> ProjLib_CompProjectedCurve::Trim(const double First,
                                                                const double Last,
                                                                const double Tol) const
{
  occ::handle<ProjLib_HCompProjectedCurve> HCS = new ProjLib_HCompProjectedCurve(*this);
  HCS->Load(mySurface);
  HCS->Load(myCurve->Trim(First, Last, Tol));
  return HCS;
}

//=================================================================================================

GeomAbs_CurveType ProjLib_CompProjectedCurve::GetType() const
{
  return GeomAbs_OtherCurve;
}

//=================================================================================================

bool ProjLib_CompProjectedCurve::ResultIsPoint(const int theIndex) const
{
  return myResultIsPoint->Value(theIndex);
}

//=================================================================================================

double ProjLib_CompProjectedCurve::GetResult2dUApproxError(const int theIndex) const
{
  return myResult2dUApproxError->Value(theIndex);
}

//=================================================================================================

double ProjLib_CompProjectedCurve::GetResult2dVApproxError(const int theIndex) const
{
  return myResult2dVApproxError->Value(theIndex);
}

//=================================================================================================

double ProjLib_CompProjectedCurve::GetResult3dApproxError(const int theIndex) const
{
  return myResult3dApproxError->Value(theIndex);
}

//=================================================================================================

occ::handle<Geom2d_Curve> ProjLib_CompProjectedCurve::GetResult2dC(const int theIndex) const
{
  return myResult2dCurve->Value(theIndex);
}

//=================================================================================================

occ::handle<Geom_Curve> ProjLib_CompProjectedCurve::GetResult3dC(const int theIndex) const
{
  return myResult3dCurve->Value(theIndex);
}

//=================================================================================================

gp_Pnt2d ProjLib_CompProjectedCurve::GetResult2dP(const int theIndex) const
{
  Standard_TypeMismatch_Raise_if(!myResultIsPoint->Value(theIndex),
                                 "ProjLib_CompProjectedCurve : result is not a point 2d");
  return myResult2dPoint->Value(theIndex);
}

//=================================================================================================

gp_Pnt ProjLib_CompProjectedCurve::GetResult3dP(const int theIndex) const
{
  Standard_TypeMismatch_Raise_if(!myResultIsPoint->Value(theIndex),
                                 "ProjLib_CompProjectedCurve : result is not a point 3d");
  return myResult3dPoint->Value(theIndex);
}

//=================================================================================================

void ProjLib_CompProjectedCurve::UpdateTripleByTrapCriteria(gp_Pnt& thePoint) const
{
  bool isProblemsPossible = false;
  // Check possible traps cases:

  // 25892 bug.
  if (mySurface->GetType() == GeomAbs_SurfaceOfRevolution)
  {
    // Compute maximal deviation from 3D and choose the biggest one.
    double aVRes   = mySurface->VResolution(Precision::Confusion());
    double aMaxTol = std::max(Precision::PConfusion(), aVRes);

    if (std::abs(thePoint.Z() - mySurface->FirstVParameter()) < aMaxTol
        || std::abs(thePoint.Z() - mySurface->LastVParameter()) < aMaxTol)
    {
      isProblemsPossible = true;
    }
  }

  // 27135 bug. Trap on degenerated edge.
  if (mySurface->GetType() == GeomAbs_Sphere
      && (std::abs(thePoint.Z() - mySurface->FirstVParameter()) < Precision::PConfusion()
          || std::abs(thePoint.Z() - mySurface->LastVParameter()) < Precision::PConfusion()
          || std::abs(thePoint.Y() - mySurface->FirstUParameter()) < Precision::PConfusion()
          || std::abs(thePoint.Y() - mySurface->LastUParameter()) < Precision::PConfusion()))
  {
    isProblemsPossible = true;
  }

  if (!isProblemsPossible)
    return;

  double U, V;
  bool   isDone = InitialPoint(myCurve->Value(thePoint.X()),
                             thePoint.X(),
                             myCurve,
                             mySurface,
                             Precision::PConfusion(),
                             Precision::PConfusion(),
                             U,
                             V,
                             myMaxDist);

  if (!isDone)
    return;

  // Restore original position in case of period jump.
  if (mySurface->IsUPeriodic()
      && std::abs(std::abs(U - thePoint.Y()) - mySurface->UPeriod()) < Precision::PConfusion())
  {
    U = thePoint.Y();
  }
  if (mySurface->IsVPeriodic()
      && std::abs(std::abs(V - thePoint.Z()) - mySurface->VPeriod()) < Precision::PConfusion())
  {
    V = thePoint.Z();
  }
  thePoint.SetY(U);
  thePoint.SetZ(V);
}

//=================================================================================================

void BuildCurveSplits(const occ::handle<Adaptor3d_Curve>&   theCurve,
                      const occ::handle<Adaptor3d_Surface>& theSurface,
                      const double                          theTolU,
                      const double                          theTolV,
                      NCollection_Vector<double>&           theSplits)
{
  SplitDS aDS(theCurve, theSurface, theSplits);

  Extrema_ExtPS anExtPS;
  anExtPS.Initialize(*theSurface,
                     theSurface->FirstUParameter(),
                     theSurface->LastUParameter(),
                     theSurface->FirstVParameter(),
                     theSurface->LastVParameter(),
                     theTolU,
                     theTolV);
  aDS.myExtPS = &anExtPS;

  if (theSurface->IsUPeriodic())
  {
    aDS.myPeriodicDir = 0;
    SplitOnDirection(aDS);
  }
  if (theSurface->IsVPeriodic())
  {
    aDS.myPeriodicDir = 1;
    SplitOnDirection(aDS);
  }

  std::sort(aDS.mySplits.begin(), aDS.mySplits.end(), Comparator);
}

//=======================================================================
// function : SplitOnDirection
// purpose  : This method compute points in the parameter space of the curve
//           on which curve should be split since period jump is happen.
//=======================================================================
void SplitOnDirection(SplitDS& theSplitDS)
{
  // Algorithm:
  // Create 3D curve which is correspond to the periodic bound in 2d space.
  // Run curve / curve extrema and run extrema point / surface to check that
  // the point will be projected to the periodic bound.
  // In this method assumed that the points cannot be closer to each other that 1% of the parameter
  // space.

  gp_Pnt2d aStartPnt(theSplitDS.mySurface->FirstUParameter(),
                     theSplitDS.mySurface->FirstVParameter());
  gp_Dir2d aDir(theSplitDS.myPeriodicDir, (int)!theSplitDS.myPeriodicDir);

  theSplitDS.myPerMinParam = !theSplitDS.myPeriodicDir ? theSplitDS.mySurface->FirstUParameter()
                                                       : theSplitDS.mySurface->FirstVParameter();
  theSplitDS.myPerMaxParam = !theSplitDS.myPeriodicDir ? theSplitDS.mySurface->LastUParameter()
                                                       : theSplitDS.mySurface->LastVParameter();
  double aLast2DParam =
    theSplitDS.myPeriodicDir
      ? theSplitDS.mySurface->LastUParameter() - theSplitDS.mySurface->FirstUParameter()
      : theSplitDS.mySurface->LastVParameter() - theSplitDS.mySurface->FirstVParameter();

  // Create line which is represent periodic border.
  occ::handle<Geom2d_Curve>        aC2GC = new Geom2d_Line(aStartPnt, aDir);
  occ::handle<Geom2dAdaptor_Curve> aC    = new Geom2dAdaptor_Curve(aC2GC, 0, aLast2DParam);
  Adaptor3d_CurveOnSurface         aCOnS(aC, theSplitDS.mySurface);
  theSplitDS.myExtCCCurve1      = &aCOnS;
  theSplitDS.myExtCCLast2DParam = aLast2DParam;

  FindSplitPoint(theSplitDS,
                 theSplitDS.myCurve->FirstParameter(), // Initial curve range.
                 theSplitDS.myCurve->LastParameter());
}

//=================================================================================================

void FindSplitPoint(SplitDS& theSplitDS, const double theMinParam, const double theMaxParam)
{
  // Make extrema copy to avoid dependencies between different levels of the recursion.
  Extrema_ExtCC anExtCC;
  anExtCC.SetCurve(1, *theSplitDS.myExtCCCurve1);
  anExtCC.SetCurve(2, *theSplitDS.myCurve);
  // clang-format off
  anExtCC.SetSingleSolutionFlag (true); // Search only one solution since multiple invocations are needed.
  // clang-format on
  anExtCC.SetRange(1, 0, theSplitDS.myExtCCLast2DParam);
  anExtCC.SetRange(2, theMinParam, theMaxParam);
  anExtCC.Perform();

  if (anExtCC.IsDone() && !anExtCC.IsParallel())
  {
    const int aNbExt = anExtCC.NbExt();
    for (int anIdx = 1; anIdx <= aNbExt; ++anIdx)
    {
      Extrema_POnCurv aPOnC1, aPOnC2;
      anExtCC.Points(anIdx, aPOnC1, aPOnC2);

      theSplitDS.myExtPS->Perform(aPOnC2.Value());
      if (!theSplitDS.myExtPS->IsDone())
        return;

      // Find point with the minimal Euclidean distance to avoid
      // false positive points detection.
      int       aMinIdx    = -1;
      double    aMinSqDist = RealLast();
      const int aNbPext    = theSplitDS.myExtPS->NbExt();
      for (int aPIdx = 1; aPIdx <= aNbPext; ++aPIdx)
      {
        const double aCurrSqDist = theSplitDS.myExtPS->SquareDistance(aPIdx);

        if (aCurrSqDist < aMinSqDist)
        {
          aMinSqDist = aCurrSqDist;
          aMinIdx    = aPIdx;
        }
      }

      // Check that is point will be projected to the periodic border.
      const Extrema_POnSurf& aPOnS = theSplitDS.myExtPS->Point(aMinIdx);
      double                 U, V, aProjParam;
      aPOnS.Parameter(U, V);
      aProjParam = theSplitDS.myPeriodicDir ? V : U;

      if (std::abs(aProjParam - theSplitDS.myPerMinParam) < Precision::PConfusion()
          || std::abs(aProjParam - theSplitDS.myPerMaxParam) < Precision::PConfusion())
      {
        const double aParam   = aPOnC2.Parameter();
        const double aCFParam = theSplitDS.myCurve->FirstParameter();
        const double aCLParam = theSplitDS.myCurve->LastParameter();

        if (aParam > aCFParam + Precision::PConfusion()
            && aParam < aCLParam - Precision::PConfusion())
        {
          // Add only inner points.
          theSplitDS.mySplits.Append(aParam);
        }

        const double aDeltaCoeff = 0.01;
        const double aDelta      = (theMaxParam - theMinParam + aCLParam - aCFParam) * aDeltaCoeff;

        if (aParam - aDelta > theMinParam + Precision::PConfusion())
        {
          FindSplitPoint(theSplitDS, theMinParam, aParam - aDelta); // Curve parameters.
        }

        if (aParam + aDelta < theMaxParam - Precision::PConfusion())
        {
          FindSplitPoint(theSplitDS, aParam + aDelta, theMaxParam); // Curve parameters.
        }
      }
    } // for (int anIdx = 1; anIdx <= aNbExt; ++anIdx)
  }
}
