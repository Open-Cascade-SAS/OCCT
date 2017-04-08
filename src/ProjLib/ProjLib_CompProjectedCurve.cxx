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

#include <Adaptor2d_HCurve2d.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Extrema_ExtCS.hxx>
#include <Extrema_ExtPS.hxx>
#include <Extrema_GenLocateExtPS.hxx>
#include <Extrema_POnCurv.hxx>
#include <Extrema_POnSurf.hxx>
#include <GeomAbs_CurveType.hxx>
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
#include <TColgp_HSequenceOfPnt.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Extrema_ExtCC.hxx>
#include <NCollection_Vector.hxx>

#define FuncTol 1.e-10

#ifdef OCCT_DEBUG_CHRONO
#include <OSD_Timer.hxx>

static OSD_Chronometer chr_init_point, chr_dicho_bound;

Standard_EXPORT Standard_Real t_init_point, t_dicho_bound;
Standard_EXPORT Standard_Integer init_point_count, dicho_bound_count;

static void InitChron(OSD_Chronometer& ch)
{ 
  ch.Reset();
  ch.Start();
}

static void ResultChron( OSD_Chronometer & ch, Standard_Real & time) 
{
  Standard_Real tch ;
  ch.Stop();
  ch.Show(tch);
  time=time +tch;
}
#endif

// Structure to perform splits computation.
// This structure is not thread-safe since operations under mySplits should be performed in a critical section.
// myPeriodicDir - 0 for U periodicity and 1 for V periodicity.
struct SplitDS
{
  SplitDS(const Handle(Adaptor3d_HCurve)   &theCurve,
          const Handle(Adaptor3d_HSurface) &theSurface,
          NCollection_Vector<Standard_Real> &theSplits)
  : myCurve(theCurve),
    mySurface(theSurface),
    mySplits(theSplits)
  { }

  // Assignment operator is forbidden.
  void operator=(const SplitDS &theSplitDS);

  const Handle(Adaptor3d_HCurve) myCurve;
  const Handle(Adaptor3d_HSurface) mySurface;
  NCollection_Vector<Standard_Real> &mySplits;

  Standard_Real myPerMinParam;
  Standard_Real myPerMaxParam;
  Standard_Integer myPeriodicDir;

  Extrema_ExtCC *myExtCC;
  Extrema_ExtPS *myExtPS;
};

  //! Compute split points in the parameter space of the curve.
  static void BuildCurveSplits(const Handle(Adaptor3d_HCurve)   &theCurve,
                               const Handle(Adaptor3d_HSurface) &theSurface,
                               const Standard_Real theTolU,
                               const Standard_Real theTolV,
                               NCollection_Vector<Standard_Real> &theSplits);

  //! Perform splitting on a specified direction. Sub-method in BuildCurveSplits.
  static void SplitOnDirection(SplitDS & theSplitDS);

  //! Perform recursive search of the split points.
  static void FindSplitPoint(SplitDS & theSplitDS,
                             const Standard_Real theMinParam,
                             const Standard_Real theMaxParam);


//=======================================================================
//function : Comparator
//purpose  : used in sort algorithm
//=======================================================================
inline Standard_Boolean Comparator(const Standard_Real theA,
                                   const Standard_Real theB)
{
  return theA < theB;
}

//=======================================================================
//function : d1
//purpose  : computes first derivative of the projected curve
//=======================================================================

static void d1(const Standard_Real t,
  const Standard_Real u,
  const Standard_Real v,
  gp_Vec2d& V, 
  const Handle(Adaptor3d_HCurve)& Curve, 
  const Handle(Adaptor3d_HSurface)& Surface)
{
  gp_Pnt S, C;
  gp_Vec DS1_u, DS1_v, DS2_u, DS2_uv, DS2_v, DC1_t;
  Surface->D2(u, v, S, DS1_u, DS1_v, DS2_u, DS2_v, DS2_uv);
  Curve->D1(t, C, DC1_t);
  gp_Vec Ort(C, S);// Ort = S - C

  gp_Vec2d dE_dt(-DC1_t*DS1_u, -DC1_t*DS1_v);
  gp_XY dE_du(DS1_u*DS1_u + Ort*DS2_u, 
    DS1_u*DS1_v + Ort*DS2_uv);
  gp_XY dE_dv(DS1_v*DS1_u + Ort*DS2_uv, 
    DS1_v*DS1_v + Ort*DS2_v);

  Standard_Real det = dE_du.X()*dE_dv.Y() - dE_du.Y()*dE_dv.X();
  if (fabs(det) < gp::Resolution()) throw Standard_ConstructionError();

  gp_Mat2d M(gp_XY(dE_dv.Y()/det, -dE_du.Y()/det), 
    gp_XY(-dE_dv.X()/det, dE_du.X()/det));

  V = - gp_Vec2d(gp_Vec2d(M.Row(1))*dE_dt, gp_Vec2d(M.Row(2))*dE_dt);
}

//=======================================================================
//function : d2
//purpose  : computes second derivative of the projected curve
//=======================================================================

static void d2(const Standard_Real t,
  const Standard_Real u,
  const Standard_Real v,
  gp_Vec2d& V1, gp_Vec2d& V2,
  const Handle(Adaptor3d_HCurve)& Curve, 
  const Handle(Adaptor3d_HSurface)& Surface)
{
  gp_Pnt S, C;
  gp_Vec DS1_u, DS1_v, DS2_u, DS2_uv, DS2_v, 
    DS3_u, DS3_v, DS3_uuv, DS3_uvv, 
    DC1_t, DC2_t;
  Surface->D3(u, v, S, DS1_u, DS1_v, DS2_u, DS2_v, DS2_uv, 
    DS3_u, DS3_v, DS3_uuv, DS3_uvv);
  Curve->D2(t, C, DC1_t, DC2_t);
  gp_Vec Ort(C, S);

  gp_Vec2d dE_dt(-DC1_t*DS1_u, -DC1_t*DS1_v);
  gp_XY dE_du(DS1_u*DS1_u + Ort*DS2_u, 
    DS1_u*DS1_v + Ort*DS2_uv);
  gp_XY dE_dv(DS1_v*DS1_u + Ort*DS2_uv, 
    DS1_v*DS1_v + Ort*DS2_v);

  Standard_Real det = dE_du.X()*dE_dv.Y() - dE_du.Y()*dE_dv.X();
  if (fabs(det) < gp::Resolution()) throw Standard_ConstructionError();

  gp_Mat2d M(gp_XY(dE_dv.Y()/det, -dE_du.Y()/det), 
    gp_XY(-dE_dv.X()/det, dE_du.X()/det));

  // First derivative
  V1 = - gp_Vec2d(gp_Vec2d(M.Row(1))*dE_dt, gp_Vec2d(M.Row(2))*dE_dt);

  /* Second derivative */

  // Computation of d2E_dt2 = S1
  gp_Vec2d d2E_dt(-DC2_t*DS1_u, -DC2_t*DS1_v);

  // Computation of 2*(d2E/dtdX)(dX/dt) = S2
  gp_Vec2d d2E1_dtdX(-DC1_t*DS2_u,
    -DC1_t*DS2_uv);
  gp_Vec2d d2E2_dtdX(-DC1_t*DS2_uv,
    -DC1_t*DS2_v);
  gp_Vec2d S2 = 2*gp_Vec2d(d2E1_dtdX*V1, d2E2_dtdX*V1);

  // Computation of (d2E/dX2)*(dX/dt)2 = S3

  // Row11 = (d2E1/du2, d2E1/dudv)
  Standard_Real tmp;
  gp_Vec2d Row11(3*DS1_u*DS2_u + Ort*DS3_u,
    tmp = 2*DS1_u*DS2_uv + 
    DS1_v*DS2_u + Ort*DS3_uuv);  

  // Row12 = (d2E1/dudv, d2E1/dv2)
  gp_Vec2d Row12(tmp, DS2_v*DS1_u + 2*DS1_v*DS2_uv + 
    Ort*DS3_uvv);

  // Row21 = (d2E2/du2, d2E2/dudv)
  gp_Vec2d Row21(DS2_u*DS1_v + 2*DS1_u*DS2_uv + Ort*DS3_uuv, 
    tmp = 2*DS2_uv*DS1_v + DS1_u*DS2_v + Ort*DS3_uvv);

  // Row22 = (d2E2/duv, d2E2/dvdv)
  gp_Vec2d Row22(tmp, 3*DS1_v*DS2_v + Ort*DS3_v);

  gp_Vec2d S3(V1*gp_Vec2d(Row11*V1, Row12*V1),
    V1*gp_Vec2d(Row21*V1, Row22*V1));

  gp_Vec2d Sum = d2E_dt + S2 + S3;

  V2 = - gp_Vec2d(gp_Vec2d(M.Row(1))*Sum, gp_Vec2d(M.Row(2))*Sum);
}
//=======================================================================
//function : d1CurveOnSurf
//purpose  : computes first derivative of the 3d projected curve
//=======================================================================

#if 0
static void d1CurvOnSurf(const Standard_Real t,
  const Standard_Real u,
  const Standard_Real v,
  gp_Vec& V, 
  const Handle(Adaptor3d_HCurve)& Curve, 
  const Handle(Adaptor3d_HSurface)& Surface)
{
  gp_Pnt S, C;
  gp_Vec2d V2d;
  gp_Vec DS1_u, DS1_v, DS2_u, DS2_uv, DS2_v, DC1_t;
  Surface->D2(u, v, S, DS1_u, DS1_v, DS2_u, DS2_v, DS2_uv);
  Curve->D1(t, C, DC1_t);
  gp_Vec Ort(C, S);// Ort = S - C

  gp_Vec2d dE_dt(-DC1_t*DS1_u, -DC1_t*DS1_v);
  gp_XY dE_du(DS1_u*DS1_u + Ort*DS2_u, 
    DS1_u*DS1_v + Ort*DS2_uv);
  gp_XY dE_dv(DS1_v*DS1_u + Ort*DS2_uv, 
    DS1_v*DS1_v + Ort*DS2_v);

  Standard_Real det = dE_du.X()*dE_dv.Y() - dE_du.Y()*dE_dv.X();
  if (fabs(det) < gp::Resolution()) throw Standard_ConstructionError();

  gp_Mat2d M(gp_XY(dE_dv.Y()/det, -dE_du.Y()/det), 
    gp_XY(-dE_dv.X()/det, dE_du.X()/det));

  V2d = - gp_Vec2d(gp_Vec2d(M.Row(1))*dE_dt, gp_Vec2d(M.Row(2))*dE_dt);

  V = DS1_u * V2d.X() + DS1_v * V2d.Y();

}
#endif

//=======================================================================
//function : d2CurveOnSurf
//purpose  : computes second derivative of the 3D projected curve
//=======================================================================

static void d2CurvOnSurf(const Standard_Real t,
  const Standard_Real u,
  const Standard_Real v,
  gp_Vec& V1 , gp_Vec& V2 ,
  const Handle(Adaptor3d_HCurve)& Curve, 
  const Handle(Adaptor3d_HSurface)& Surface)
{
  gp_Pnt S, C;
  gp_Vec2d V12d,V22d;
  gp_Vec DS1_u, DS1_v, DS2_u, DS2_uv, DS2_v, 
    DS3_u, DS3_v, DS3_uuv, DS3_uvv, 
    DC1_t, DC2_t;
  Surface->D3(u, v, S, DS1_u, DS1_v, DS2_u, DS2_v, DS2_uv, 
    DS3_u, DS3_v, DS3_uuv, DS3_uvv);
  Curve->D2(t, C, DC1_t, DC2_t);
  gp_Vec Ort(C, S);

  gp_Vec2d dE_dt(-DC1_t*DS1_u, -DC1_t*DS1_v);
  gp_XY dE_du(DS1_u*DS1_u + Ort*DS2_u, 
    DS1_u*DS1_v + Ort*DS2_uv);
  gp_XY dE_dv(DS1_v*DS1_u + Ort*DS2_uv, 
    DS1_v*DS1_v + Ort*DS2_v);

  Standard_Real det = dE_du.X()*dE_dv.Y() - dE_du.Y()*dE_dv.X();
  if (fabs(det) < gp::Resolution()) throw Standard_ConstructionError();

  gp_Mat2d M(gp_XY(dE_dv.Y()/det, -dE_du.Y()/det), 
    gp_XY(-dE_dv.X()/det, dE_du.X()/det));

  // First derivative
  V12d = - gp_Vec2d(gp_Vec2d(M.Row(1))*dE_dt, gp_Vec2d(M.Row(2))*dE_dt);

  /* Second derivative */

  // Computation of d2E_dt2 = S1
  gp_Vec2d d2E_dt(-DC2_t*DS1_u, -DC2_t*DS1_v);

  // Computation of 2*(d2E/dtdX)(dX/dt) = S2
  gp_Vec2d d2E1_dtdX(-DC1_t*DS2_u,
    -DC1_t*DS2_uv);
  gp_Vec2d d2E2_dtdX(-DC1_t*DS2_uv,
    -DC1_t*DS2_v);
  gp_Vec2d S2 = 2*gp_Vec2d(d2E1_dtdX*V12d, d2E2_dtdX*V12d);

  // Computation of (d2E/dX2)*(dX/dt)2 = S3

  // Row11 = (d2E1/du2, d2E1/dudv)
  Standard_Real tmp;
  gp_Vec2d Row11(3*DS1_u*DS2_u + Ort*DS3_u,
    tmp = 2*DS1_u*DS2_uv + 
    DS1_v*DS2_u + Ort*DS3_uuv);  

  // Row12 = (d2E1/dudv, d2E1/dv2)
  gp_Vec2d Row12(tmp, DS2_v*DS1_u + 2*DS1_v*DS2_uv + 
    Ort*DS3_uvv);

  // Row21 = (d2E2/du2, d2E2/dudv)
  gp_Vec2d Row21(DS2_u*DS1_v + 2*DS1_u*DS2_uv + Ort*DS3_uuv, 
    tmp = 2*DS2_uv*DS1_v + DS1_u*DS2_v + Ort*DS3_uvv);

  // Row22 = (d2E2/duv, d2E2/dvdv)
  gp_Vec2d Row22(tmp, 3*DS1_v*DS2_v + Ort*DS3_v);

  gp_Vec2d S3(V12d*gp_Vec2d(Row11*V12d, Row12*V12d),
    V12d*gp_Vec2d(Row21*V12d, Row22*V12d));

  gp_Vec2d Sum = d2E_dt + S2 + S3;

  V22d = - gp_Vec2d(gp_Vec2d(M.Row(1))*Sum, gp_Vec2d(M.Row(2))*Sum);

  V1 = DS1_u * V12d.X() + DS1_v * V12d.Y();
  V2 =     DS2_u * V12d.X() *V12d.X()  
    +     DS1_u * V22d.X() 
    + 2 * DS2_uv * V12d.X() *V12d.Y()
    +     DS2_v * V12d.Y() * V12d.Y()
    +     DS1_v * V22d.Y();
}

//=======================================================================
//function : ExactBound
//purpose  : computes exact boundary point
//=======================================================================

static Standard_Boolean ExactBound(gp_Pnt& Sol, 
  const Standard_Real NotSol, 
  const Standard_Real Tol, 
  const Standard_Real TolU, 
  const Standard_Real TolV,  
  const Handle(Adaptor3d_HCurve)& Curve, 
  const Handle(Adaptor3d_HSurface)& Surface)
{
  Standard_Real U0, V0, t, t1, t2, FirstU, LastU, FirstV, LastV;
  gp_Pnt2d POnS;
  U0 = Sol.Y();
  V0 = Sol.Z();
  FirstU = Surface->FirstUParameter();
  LastU = Surface->LastUParameter();
  FirstV = Surface->FirstVParameter();
  LastV = Surface->LastVParameter();
  // Here we have to compute the boundary that projection is going to intersect
  gp_Vec2d D2d;
  //these variables are to estimate which boundary has more apportunity 
  //to be intersected
  Standard_Real RU1, RU2, RV1, RV2; 
  d1(Sol.X(), U0, V0, D2d, Curve, Surface);
  // Here we assume that D2d != (0, 0)
  if(Abs(D2d.X()) < gp::Resolution()) 
  {
    RU1 = Precision::Infinite();
    RU2 = Precision::Infinite();
    RV1 = V0 - FirstV;
    RV2 = LastV - V0;
  }
  else if(Abs(D2d.Y()) < gp::Resolution()) 
  {
    RU1 = U0 - FirstU;
    RU2 = LastU - U0;
    RV1 = Precision::Infinite();
    RV2 = Precision::Infinite();    
  }
  else 
  {
    RU1 = gp_Pnt2d(U0, V0).
      Distance(gp_Pnt2d(FirstU, V0 + (FirstU - U0)*D2d.Y()/D2d.X()));
    RU2 = gp_Pnt2d(U0, V0).
      Distance(gp_Pnt2d(LastU, V0 + (LastU - U0)*D2d.Y()/D2d.X()));
    RV1 = gp_Pnt2d(U0, V0).
      Distance(gp_Pnt2d(U0 + (FirstV - V0)*D2d.X()/D2d.Y(), FirstV));
    RV2 = gp_Pnt2d(U0, V0).
      Distance(gp_Pnt2d(U0 + (LastV - V0)*D2d.X()/D2d.Y(), LastV));
  }
  TColgp_SequenceOfPnt Seq;
  Seq.Append(gp_Pnt(FirstU, RU1, 2));
  Seq.Append(gp_Pnt(LastU, RU2, 2));
  Seq.Append(gp_Pnt(FirstV, RV1, 3));
  Seq.Append(gp_Pnt(LastV, RV2, 3));
  Standard_Integer i, j;
  for(i = 1; i <= 3; i++)
  {
    for(j = 1; j <= 4-i; j++)
    {
      if(Seq(j).Y() < Seq(j+1).Y())
      {
        gp_Pnt swp;
        swp = Seq.Value(j+1);
        Seq.ChangeValue(j+1) = Seq.Value(j);
        Seq.ChangeValue(j) = swp;
      }
    }
  }

  t = Sol.X ();
  t1 = Min (Sol.X (), NotSol);
  t2 = Max (Sol.X (), NotSol);

  Standard_Boolean isDone = Standard_False;
  while (!Seq.IsEmpty ())
  {
    gp_Pnt P;
    P = Seq.Last ();
    Seq.Remove (Seq.Length ());
    ProjLib_PrjResolve aPrjPS (Curve->Curve (),
      Surface->Surface (),
      Standard_Integer (P.Z ()));
    if (Standard_Integer (P.Z ()) == 2)
    {
      aPrjPS.Perform (t, P.X (), V0, gp_Pnt2d (Tol, TolV),
        gp_Pnt2d (t1, Surface->FirstVParameter ()),
        gp_Pnt2d (t2, Surface->LastVParameter ()), FuncTol);
      if (!aPrjPS.IsDone ()) continue;
      POnS = aPrjPS.Solution ();
      Sol = gp_Pnt (POnS.X (), P.X (), POnS.Y ());
      isDone = Standard_True;
      break;
    }
    else
    {
      aPrjPS.Perform (t, U0, P.X (), gp_Pnt2d (Tol, TolU),
        gp_Pnt2d (t1, Surface->FirstUParameter ()),
        gp_Pnt2d (t2, Surface->LastUParameter ()), FuncTol);
      if (!aPrjPS.IsDone ()) continue;
      POnS = aPrjPS.Solution ();
      Sol = gp_Pnt (POnS.X (), POnS.Y (), P.X ());
      isDone = Standard_True;
      break;
    }
  }

  return isDone;
}

//=======================================================================
//function : DichExactBound
//purpose  : computes exact boundary point
//=======================================================================

static void DichExactBound(gp_Pnt& Sol, 
  const Standard_Real NotSol, 
  const Standard_Real Tol, 
  const Standard_Real TolU, 
  const Standard_Real TolV,  
  const Handle(Adaptor3d_HCurve)& Curve, 
  const Handle(Adaptor3d_HSurface)& Surface)
{
#ifdef OCCT_DEBUG_CHRONO
  InitChron(chr_dicho_bound);
#endif

  Standard_Real U0, V0, t;
  gp_Pnt2d POnS;
  U0 = Sol.Y();
  V0 = Sol.Z();
  ProjLib_PrjResolve aPrjPS(Curve->Curve(), Surface->Surface(), 1);

  Standard_Real aNotSol = NotSol;
  while (fabs(Sol.X() - aNotSol) > Tol) 
  {
    t = (Sol.X() + aNotSol)/2;
    aPrjPS.Perform(t, U0, V0, gp_Pnt2d(TolU, TolV), 
      gp_Pnt2d(Surface->FirstUParameter(),Surface->FirstVParameter()), 
      gp_Pnt2d(Surface->LastUParameter(),Surface->LastVParameter()), 
      FuncTol, Standard_True);

    if (aPrjPS.IsDone()) 
    {
      POnS = aPrjPS.Solution();
      Sol = gp_Pnt(t, POnS.X(), POnS.Y());
      U0=Sol.Y();
      V0=Sol.Z();
    }
    else aNotSol = t; 
  }
#ifdef OCCT_DEBUG_CHRONO
  ResultChron(chr_dicho_bound,t_dicho_bound);
  dicho_bound_count++;
#endif
}

//=======================================================================
//function : InitialPoint
//purpose  : 
//=======================================================================

static Standard_Boolean InitialPoint(const gp_Pnt& Point, 
  const Standard_Real t,
  const Handle(Adaptor3d_HCurve)& C,
  const Handle(Adaptor3d_HSurface)& S, 
  const Standard_Real TolU, 
  const Standard_Real TolV, 
  Standard_Real& U, 
  Standard_Real& V)
{

  ProjLib_PrjResolve aPrjPS(C->Curve(), S->Surface(), 1);
  Standard_Real ParU,ParV;
  Extrema_ExtPS aExtPS;
  aExtPS.Initialize(S->Surface(), S->FirstUParameter(), 
    S->LastUParameter(), S->FirstVParameter(), 
    S->LastVParameter(), TolU, TolV);

  aExtPS.Perform(Point);
  Standard_Integer argmin = 0;
  if (aExtPS.IsDone() && aExtPS.NbExt()) 
  {
    Standard_Integer i, Nend;
    // Search for the nearest solution which is also a normal projection
    Nend = aExtPS.NbExt();
    for(i = 1; i <= Nend; i++)
    {
      Extrema_POnSurf POnS = aExtPS.Point(i);
      POnS.Parameter(ParU, ParV);
      aPrjPS.Perform(t, ParU, ParV, gp_Pnt2d(TolU, TolV), 
        gp_Pnt2d(S->FirstUParameter(), S->FirstVParameter()), 
        gp_Pnt2d(S->LastUParameter(), S->LastVParameter()), 
        FuncTol, Standard_True);
      if(aPrjPS.IsDone() )
        if  (argmin == 0 || aExtPS.SquareDistance(i) < aExtPS.SquareDistance(argmin)) argmin = i;  
    }
  }
  if( argmin == 0 ) return Standard_False;
  else
  {  
    Extrema_POnSurf POnS = aExtPS.Point(argmin);
    POnS.Parameter(U, V);
    return Standard_True;
  }
}

//=======================================================================
//function : ProjLib_CompProjectedCurve
//purpose  : 
//=======================================================================

ProjLib_CompProjectedCurve::ProjLib_CompProjectedCurve()
: myNbCurves(0),
  myTolU    (0.0),
  myTolV    (0.0),
  myMaxDist (0.0)
{
}

//=======================================================================
//function : ProjLib_CompProjectedCurve
//purpose  : 
//=======================================================================

ProjLib_CompProjectedCurve::ProjLib_CompProjectedCurve
                           (const Handle(Adaptor3d_HSurface)& theSurface,
                            const Handle(Adaptor3d_HCurve)&   theCurve,
                            const Standard_Real               theTolU,
                            const Standard_Real               theTolV)
: mySurface (theSurface),
  myCurve   (theCurve),
  myNbCurves(0),
  mySequence(new ProjLib_HSequenceOfHSequenceOfPnt()),
  myTolU    (theTolU),
  myTolV    (theTolV),
  myMaxDist (-1.0)
{
  Init();
}

//=======================================================================
//function : ProjLib_CompProjectedCurve
//purpose  : 
//=======================================================================

ProjLib_CompProjectedCurve::ProjLib_CompProjectedCurve
                           (const Handle(Adaptor3d_HSurface)& theSurface,
                            const Handle(Adaptor3d_HCurve)&   theCurve,
                            const Standard_Real               theTolU,
                            const Standard_Real               theTolV,
                            const Standard_Real               theMaxDist)
: mySurface (theSurface),
  myCurve   (theCurve),
  myNbCurves(0),
  mySequence(new ProjLib_HSequenceOfHSequenceOfPnt()),
  myTolU    (theTolU),
  myTolV    (theTolV),
  myMaxDist (theMaxDist)
{
  Init();
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void ProjLib_CompProjectedCurve::Init() 
{
  myTabInt.Nullify();
  NCollection_Vector<Standard_Real> aSplits;
  aSplits.Clear();

  Standard_Real Tol;// Tolerance for ExactBound
  Standard_Integer i, Nend = 0, aSplitIdx = 0;
  Standard_Boolean FromLastU = Standard_False,
                   isSplitsComputed = Standard_False;

  const Standard_Real aTol3D = Precision::Confusion();
  Extrema_ExtCS CExt(myCurve->Curve(), mySurface->Surface(), aTol3D, aTol3D);
  if (CExt.IsDone() && CExt.NbExt())
  {
    // Search for the minimum solution.
    // Avoid usage of extrema result that can be wrong for extrusion.
    if(myMaxDist > 0 &&

       mySurface->GetType() != GeomAbs_SurfaceOfExtrusion)
    {
      Standard_Real min_val2;
      min_val2 = CExt.SquareDistance(1);

      Nend = CExt.NbExt();
      for(i = 2; i <= Nend; i++)
      {
        if (CExt.SquareDistance(i) < min_val2) 
          min_val2 = CExt.SquareDistance(i);
      }
      if (min_val2 > myMaxDist * myMaxDist)
        return; // No near solution -> exit.
    }
  }

  Standard_Real FirstU, LastU, Step, SearchStep, WalkStep, t;

  FirstU = myCurve->FirstParameter();
  LastU  = myCurve->LastParameter();
  const Standard_Real GlobalMinStep = 1.e-4;
  //<GlobalMinStep> is sufficiently small to provide solving from initial point
  //and, on the other hand, it is sufficiently large to avoid too close solutions.
  const Standard_Real MinStep = 0.01*(LastU - FirstU), 
    MaxStep = 0.1*(LastU - FirstU);
  SearchStep = 10*MinStep;
  Step = SearchStep;

  gp_Pnt2d aLowBorder(mySurface->FirstUParameter(),mySurface->FirstVParameter());
  gp_Pnt2d aUppBorder(mySurface->LastUParameter(), mySurface->LastVParameter());
  gp_Pnt2d aTol(myTolU, myTolV);
  ProjLib_PrjResolve aPrjPS(myCurve->Curve(), mySurface->Surface(), 1);

  t = FirstU;
  Standard_Boolean new_part; 
  Standard_Real prevDeb=0.;
  Standard_Boolean SameDeb=Standard_False;


  gp_Pnt Triple, prevTriple;

  //Basic loop
  while(t <= LastU) 
  {
    // Search for the beginning of a new continuous part
    // to avoid infinite computation in some difficult cases.
    new_part = Standard_False;
    if(t > FirstU && Abs(t-prevDeb) <= Precision::PConfusion()) SameDeb=Standard_True;
    while(t <= LastU && !new_part && !FromLastU && !SameDeb)
    {
      prevDeb=t;
      if (t == LastU) FromLastU=Standard_True;
      Standard_Boolean initpoint=Standard_False;
      Standard_Real U = 0., V = 0.;
      gp_Pnt CPoint;
      Standard_Real ParT,ParU,ParV; 

      // Search an initial point in the list of Extrema Curve-Surface
      if(Nend != 0 && !CExt.IsParallel()) 
      {
        for (i=1;i<=Nend;i++)
        {
          Extrema_POnCurv P1;
          Extrema_POnSurf P2;
          CExt.Points(i,P1,P2);
          ParT=P1.Parameter();
          P2.Parameter(ParU, ParV);

          aPrjPS.Perform(ParT, ParU, ParV, aTol, aLowBorder, aUppBorder, FuncTol, Standard_True);

          if ( aPrjPS.IsDone() && P1.Parameter() > Max(FirstU,t-Step+Precision::PConfusion()) 
            && P1.Parameter() <= t) 
          {
            t=ParT;
            U=ParU;
            V=ParV;
            CPoint=P1.Value();
            initpoint = Standard_True;
            break;
          }
        }
      }
      if (!initpoint) 
      {
        myCurve->D0(t,CPoint);
#ifdef OCCT_DEBUG_CHRONO
        InitChron(chr_init_point);
#endif
        // PConfusion - use geometric tolerances in extrema / optimization.
        initpoint=InitialPoint(CPoint, t,myCurve,mySurface, Precision::PConfusion(), Precision::PConfusion(), U, V);
#ifdef OCCT_DEBUG_CHRONO
        ResultChron(chr_init_point,t_init_point);
        init_point_count++;
#endif
      }
      if(initpoint) 
      {
        // When U or V lie on surface joint in some cases we cannot use them 
        // as initial point for aPrjPS, so we switch them
        gp_Vec2d D;

        if ((mySurface->IsUPeriodic() &&
            Abs(aUppBorder.X() - aLowBorder.X() - mySurface->UPeriod()) < Precision::Confusion()) ||
            (mySurface->IsVPeriodic() && 
            Abs(aUppBorder.Y() - aLowBorder.Y() - mySurface->VPeriod()) < Precision::Confusion()))
        {
          if((Abs(U - aLowBorder.X()) < mySurface->UResolution(Precision::PConfusion())) &&
            mySurface->IsUPeriodic())
          { 
            d1(t, U, V, D, myCurve, mySurface);
            if (D.X() < 0 ) U = aUppBorder.X();
          }
          else if((Abs(U - aUppBorder.X()) < mySurface->UResolution(Precision::PConfusion())) &&
            mySurface->IsUPeriodic())
          {
            d1(t, U, V, D, myCurve, mySurface);
            if (D.X() > 0) U = aLowBorder.X();
          }

          if((Abs(V - aLowBorder.Y()) < mySurface->VResolution(Precision::PConfusion())) && 
            mySurface->IsVPeriodic()) 
          {
            d1(t, U, V, D, myCurve, mySurface);
            if (D.Y() < 0) V = aUppBorder.Y();
          }
          else if((Abs(V - aUppBorder.Y()) <= mySurface->VResolution(Precision::PConfusion())) &&
            mySurface->IsVPeriodic())
          {
            d1(t, U, V, D, myCurve, mySurface);
            if (D.Y() > 0) V = aLowBorder.Y();
          }
        }

        if (myMaxDist > 0) 
        {
          // Here we are going to stop if the distance between projection and 
          // corresponding curve point is greater than myMaxDist
          gp_Pnt POnS;
          Standard_Real d;
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
          //Search for exact boundary point
          Tol = Min(myTolU, myTolV);
          gp_Vec2d aD;
          d1(Triple.X(), Triple.Y(), Triple.Z(), aD, myCurve, mySurface);
          Tol /= Max(Abs(aD.X()), Abs(aD.Y()));

          if(!ExactBound(Triple, t - Step, Tol, 
            myTolU, myTolV, myCurve, mySurface)) 
          {
#ifdef OCCT_DEBUG
            cout<<"There is a problem with ExactBound computation"<<endl;
#endif
            DichExactBound(Triple, t - Step, Tol, myTolU, myTolV, 
              myCurve, mySurface);
          }
        }
        new_part = Standard_True;
      }
      else 
      {
        if(t == LastU) break;
        t += Step;
        if(t>LastU) 
        { 
          Step =Step+LastU-t;
          t=LastU;
        }  
      }
    }
    if (!new_part) break;

    //We have found a new continuous part
    Handle(TColgp_HSequenceOfPnt) hSeq = new TColgp_HSequenceOfPnt();    
    mySequence->Append(hSeq);
    myNbCurves++;
    mySequence->Value(myNbCurves)->Append(Triple);
    prevTriple = Triple;

    if (Triple.X() == LastU) break;//return;

    //Computation of WalkStep
    gp_Vec D1, D2;
    Standard_Real MagnD1, MagnD2;
    d2CurvOnSurf(Triple.X(), Triple.Y(), Triple.Z(), D1, D2, myCurve, mySurface);
    MagnD1 = D1.Magnitude();
    MagnD2 = D2.Magnitude();
    if(MagnD2 < Precision::Confusion()) WalkStep = MaxStep;
    else WalkStep = Min(MaxStep, Max(MinStep, 0.1*MagnD1/MagnD2));

    Step = WalkStep;

    t = Triple.X() + Step;
    if (t > LastU) t = LastU;
    Standard_Real prevStep = Step;
    Standard_Real U0, V0;

    //Here we are trying to prolong continuous part
    while (t <= LastU && new_part) 
    {

      U0 = Triple.Y() + (Step / prevStep) * (Triple.Y() - prevTriple.Y());
      V0 = Triple.Z() + (Step / prevStep) * (Triple.Z() - prevTriple.Z());
      // adjust U0 to be in [mySurface->FirstUParameter(),mySurface->LastUParameter()]
      U0 = Min(Max(U0, aLowBorder.X()), aUppBorder.X()); 
      // adjust V0 to be in [mySurface->FirstVParameter(),mySurface->LastVParameter()]
      V0 = Min(Max(V0, aLowBorder.Y()), aUppBorder.Y()); 


      aPrjPS.Perform(t, U0, V0, aTol,
                     aLowBorder, aUppBorder, FuncTol, Standard_True);
      if(!aPrjPS.IsDone()) 
      {
        if (Step <= GlobalMinStep)
        {
          //Search for exact boundary point
          Tol = Min(myTolU, myTolV);
          gp_Vec2d D;
          d1(Triple.X(), Triple.Y(), Triple.Z(), D, myCurve, mySurface);
          Tol /= Max(Abs(D.X()), Abs(D.Y()));

          if(!ExactBound(Triple, t, Tol, myTolU, myTolV, 
            myCurve, mySurface)) 
          {
#ifdef OCCT_DEBUG
            cout<<"There is a problem with ExactBound computation"<<endl;
#endif
            DichExactBound(Triple, t, Tol, myTolU, myTolV, 
              myCurve, mySurface);
          }

          if((Triple.X() - mySequence->Value(myNbCurves)->Value(mySequence->Value(myNbCurves)->Length()).X()) > 1.e-10)
            mySequence->Value(myNbCurves)->Append(Triple);
          if((LastU - Triple.X()) < Tol) {t = LastU + 1; break;}//return;

          Step = SearchStep;
          t = Triple.X() + Step;
          if (t > (LastU-MinStep/2) ) 
          { 
            Step =Step+LastU-t;
            t = LastU;
          }
          new_part = Standard_False;
        }
        else 
        {
          // decrease step
          Standard_Real SaveStep = Step;
          Step /= 2.;
          t = Triple .X() + Step;
          if (t > (LastU-MinStep/4) ) 
          { 
            Step =Step+LastU-t;
            if (Abs(Step - SaveStep) <= Precision::PConfusion())
              Step = GlobalMinStep; //to avoid looping
            t = LastU;
          }
        }
      }
      // Go further
      else 
      {
        prevTriple = Triple;
        prevStep = Step;
        Triple = gp_Pnt(t, aPrjPS.Solution().X(), aPrjPS.Solution().Y());

        // Check for possible local traps.
        UpdateTripleByTrapCriteria(Triple);

        // Protection from case when the whole curve lies on a seam.
        if (!isSplitsComputed)
        {
          Standard_Boolean isUPossible = Standard_False;
          if (mySurface->IsUPeriodic() &&
             (Abs(Triple.Y() - mySurface->FirstUParameter() ) > Precision::PConfusion() &&
              Abs(Triple.Y() - mySurface->LastUParameter()  ) > Precision::PConfusion()))
          {
            isUPossible = Standard_True;
          }

          Standard_Boolean isVPossible = Standard_False;
          if (mySurface->IsVPeriodic() &&
             (Abs(Triple.Z() - mySurface->FirstVParameter() ) > Precision::PConfusion() &&
              Abs(Triple.Z() - mySurface->LastVParameter()  ) > Precision::PConfusion()))
          {
            isVPossible = Standard_True;
          }

          if (isUPossible || isVPossible)
          {
            // When point is good conditioned.
            BuildCurveSplits(myCurve, mySurface, myTolU, myTolV, aSplits);
            isSplitsComputed = Standard_True;
          }
        }

        if((Triple.X() - mySequence->Value(myNbCurves)->Value(mySequence->Value(myNbCurves)->Length()).X()) > 1.e-10)
          mySequence->Value(myNbCurves)->Append(Triple);
        if (t == LastU) {t = LastU + 1; break;}//return;
        //Computation of WalkStep
        d2CurvOnSurf(Triple.X(), Triple.Y(), Triple.Z(), D1, D2, myCurve, mySurface);
        MagnD1 = D1.Magnitude();
        MagnD2 = D2.Magnitude();
        if(MagnD2 < Precision::Confusion() ) WalkStep = MaxStep;
        else WalkStep = Min(MaxStep, Max(MinStep, 0.1*MagnD1/MagnD2));

        Step = WalkStep;
        t += Step;
        if (t > (LastU-MinStep/2))
        {
          Step = Step + LastU - t;
          t = LastU;
        }

        // We assume at least one point of cache inside of a split.
        const Standard_Integer aSize = aSplits.Size();
        for(Standard_Integer anIdx = aSplitIdx; anIdx < aSize; ++anIdx)
        {
          const Standard_Real aParam = aSplits(anIdx);
          if (Abs(aParam - Triple.X() ) < Precision::PConfusion())
          {
            // The current point is equal to a split point.
            new_part = Standard_False;

            // Move split index to avoid check of the whole list.
            ++aSplitIdx;
            break;
          }
          else if (aParam < t + Precision::PConfusion() )
          {
            // The next point crosses the split point.
            t = aParam;
            Step = t - prevTriple.X();
          }
        } // for(Standard_Integer anIdx = aSplitIdx; anIdx < aSize; ++anIdx)
      }
    }
  }

  // Sequence post-proceeding.
  Standard_Integer j;

  // 1. Removing poor parts
  Standard_Integer NbPart=myNbCurves;
  Standard_Integer ipart=1;
  for(i = 1; i <= NbPart; i++) {
    //    Standard_Integer NbPoints = mySequence->Value(i)->Length();
    if(mySequence->Value(ipart)->Length() < 2) {
      mySequence->Remove(ipart);
      myNbCurves--;
    }
    else ipart++;
  }

  if(myNbCurves == 0) return;

  // 2. Removing common parts of bounds
  for(i = 1; i < myNbCurves; i++) 
  {
    if(mySequence->Value(i)->Value(mySequence->Value(i)->Length()).X() >=
      mySequence->Value(i+1)->Value(1).X())
    {
      mySequence->ChangeValue(i+1)->ChangeValue(1).SetX(mySequence->Value(i)->Value(mySequence->Value(i)->Length()).X() + 1.e-12);
    }
  }

  // 3. Computation of the maximum distance from each part of curve to surface

  myMaxDistance = new TColStd_HArray1OfReal(1, myNbCurves);
  myMaxDistance->Init(0);
  for(i = 1; i <= myNbCurves; i++)
  {
    for(j = 1; j <= mySequence->Value(i)->Length(); j++)
    {
      gp_Pnt POnC, POnS, aTriple;
      Standard_Real Distance;
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
  Standard_Real AveU, AveV;
  mySnglPnts = new TColStd_HArray1OfBoolean(1, myNbCurves);
  mySnglPnts->Init (Standard_True);

  for(i = 1; i <= myNbCurves; i++)
  {
    //compute an average U and V

    for(j = 1, AveU = 0., AveV = 0.; j <= mySequence->Value(i)->Length(); j++)
    {
      AveU += mySequence->Value(i)->Value(j).Y();
      AveV += mySequence->Value(i)->Value(j).Z();
    }
    AveU /= mySequence->Value(i)->Length();
    AveV /= mySequence->Value(i)->Length();

    Pmoy.SetCoord(AveU,AveV);
    for(j = 1; j <= mySequence->Value(i)->Length(); j++)
    {
      Pcurr =
        gp_Pnt2d(mySequence->Value(i)->Value(j).Y(), mySequence->Value(i)->Value(j).Z());
      if (Pcurr.Distance(Pmoy) > ((myTolU < myTolV) ? myTolV : myTolU))
      {
        mySnglPnts->SetValue(i, Standard_False);
        break;
      }
    }
  }

  // 5. Check the projection to be an isoparametric curve of the surface

  myUIso = new TColStd_HArray1OfBoolean(1, myNbCurves);
  myUIso->Init (Standard_True);

  myVIso = new TColStd_HArray1OfBoolean(1, myNbCurves);
  myVIso->Init (Standard_True);

  for(i = 1; i <= myNbCurves; i++) {
    if (IsSinglePnt(i, P)|| mySequence->Value(i)->Length() <=2) {
      myUIso->SetValue(i, Standard_False);
      myVIso->SetValue(i, Standard_False);
      continue;
    }

    // new test for isoparametrics

    if ( mySequence->Value(i)->Length() > 2) {
      //compute an average U and V

      for(j = 1, AveU = 0., AveV = 0.; j <= mySequence->Value(i)->Length(); j++) {
        AveU += mySequence->Value(i)->Value(j).Y();
        AveV += mySequence->Value(i)->Value(j).Z();
      }
      AveU /= mySequence->Value(i)->Length();
      AveV /= mySequence->Value(i)->Length();

      // is i-part U-isoparametric ?
      for(j = 1; j <= mySequence->Value(i)->Length(); j++)
      {
        if(Abs(mySequence->Value(i)->Value(j).Y() - AveU) > myTolU)
        {
          myUIso->SetValue(i, Standard_False);
          break;
        }
      }

      // is i-part V-isoparametric ?
      for(j = 1; j <= mySequence->Value(i)->Length(); j++)
      {
        if(Abs(mySequence->Value(i)->Value(j).Z() - AveV) > myTolV)
        {
          myVIso->SetValue(i, Standard_False);
          break;
        }
      }
      //
    }
  }
}
//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void ProjLib_CompProjectedCurve::Load(const Handle(Adaptor3d_HSurface)& S) 
{
  mySurface = S;
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void ProjLib_CompProjectedCurve::Load(const Handle(Adaptor3d_HCurve)& C) 
{
  myCurve = C;
}

//=======================================================================
//function : GetSurface
//purpose  : 
//=======================================================================

const Handle(Adaptor3d_HSurface)& ProjLib_CompProjectedCurve::GetSurface() const
{
  return mySurface;
}


//=======================================================================
//function : GetCurve
//purpose  : 
//=======================================================================

const Handle(Adaptor3d_HCurve)& ProjLib_CompProjectedCurve::GetCurve() const
{
  return myCurve;
}

//=======================================================================
//function : GetTolerance
//purpose  : 
//=======================================================================

void ProjLib_CompProjectedCurve::GetTolerance(Standard_Real& TolU,
  Standard_Real& TolV) const
{
  TolU = myTolU;
  TolV = myTolV;
}

//=======================================================================
//function : NbCurves
//purpose  : 
//=======================================================================

Standard_Integer ProjLib_CompProjectedCurve::NbCurves() const
{
  return myNbCurves;
}
//=======================================================================
//function : Bounds
//purpose  : 
//=======================================================================

void ProjLib_CompProjectedCurve::Bounds(const Standard_Integer Index,
  Standard_Real& Udeb,
  Standard_Real& Ufin) const
{
  if(Index < 1 || Index > myNbCurves) throw Standard_NoSuchObject();
  Udeb = mySequence->Value(Index)->Value(1).X();
  Ufin = mySequence->Value(Index)->Value(mySequence->Value(Index)->Length()).X();
}
//=======================================================================
//function : IsSinglePnt
//purpose  : 
//=======================================================================

Standard_Boolean ProjLib_CompProjectedCurve::IsSinglePnt(const Standard_Integer Index, gp_Pnt2d& P) const
{
  if(Index < 1 || Index > myNbCurves) throw Standard_NoSuchObject();
  P = gp_Pnt2d(mySequence->Value(Index)->Value(1).Y(), mySequence->Value(Index)->Value(1).Z());
  return mySnglPnts->Value(Index);
}

//=======================================================================
//function : IsUIso
//purpose  : 
//=======================================================================

Standard_Boolean ProjLib_CompProjectedCurve::IsUIso(const Standard_Integer Index, Standard_Real& U) const
{
  if(Index < 1 || Index > myNbCurves) throw Standard_NoSuchObject();
  U = mySequence->Value(Index)->Value(1).Y();
  return myUIso->Value(Index);
}
//=======================================================================
//function : IsVIso
//purpose  : 
//=======================================================================

Standard_Boolean ProjLib_CompProjectedCurve::IsVIso(const Standard_Integer Index, Standard_Real& V) const
{
  if(Index < 1 || Index > myNbCurves) throw Standard_NoSuchObject();
  V = mySequence->Value(Index)->Value(1).Z();
  return myVIso->Value(Index);
}
//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

gp_Pnt2d ProjLib_CompProjectedCurve::Value(const Standard_Real t) const
{
  gp_Pnt2d P;
  D0(t, P);
  return P;
}
//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void ProjLib_CompProjectedCurve::D0(const Standard_Real U,gp_Pnt2d& P) const
{
  Standard_Integer i, j;
  Standard_Real Udeb, Ufin;
  Standard_Boolean found = Standard_False;

  for(i = 1; i <= myNbCurves; i++) 
  {
    Bounds(i, Udeb, Ufin);
    if (U >= Udeb && U <= Ufin) 
    {
      found = Standard_True;
      break;
    }
  }
  if (!found) throw Standard_DomainError("ProjLib_CompProjectedCurve::D0");

  Standard_Real U0, V0;

  Standard_Integer End = mySequence->Value(i)->Length();
  for(j = 1; j < End; j++)
    if ((U >= mySequence->Value(i)->Value(j).X()) && (U <= mySequence->Value(i)->Value(j + 1).X())) break;

  //  U0 = mySequence->Value(i)->Value(j).Y();
  //  V0 = mySequence->Value(i)->Value(j).Z();

  //  Cubic Interpolation
  if(mySequence->Value(i)->Length() < 4 || 
    (Abs(U-mySequence->Value(i)->Value(j).X()) <= Precision::PConfusion()) ) 
  {
    U0 = mySequence->Value(i)->Value(j).Y();
    V0 = mySequence->Value(i)->Value(j).Z();
  }
  else if (Abs(U-mySequence->Value(i)->Value(j+1).X()) 
    <= Precision::PConfusion())
  {
    U0 = mySequence->Value(i)->Value(j+1).Y();
    V0 = mySequence->Value(i)->Value(j+1).Z();
  }
  else 
  {
    if (j == 1) j = 2;
    if (j > mySequence->Value(i)->Length() - 2) 
      j = mySequence->Value(i)->Length() - 2;

    gp_Vec2d I1, I2, I3, I21, I22, I31, Y1, Y2, Y3, Y4, Res;
    Standard_Real X1, X2, X3, X4;

    X1 = mySequence->Value(i)->Value(j - 1).X();
    X2 = mySequence->Value(i)->Value(j).X();
    X3 = mySequence->Value(i)->Value(j + 1).X();
    X4 = mySequence->Value(i)->Value(j + 2).X();

    Y1 = gp_Vec2d(mySequence->Value(i)->Value(j - 1).Y(), 
      mySequence->Value(i)->Value(j - 1).Z());
    Y2 = gp_Vec2d(mySequence->Value(i)->Value(j).Y(), 
      mySequence->Value(i)->Value(j).Z());
    Y3 = gp_Vec2d(mySequence->Value(i)->Value(j + 1).Y(), 
      mySequence->Value(i)->Value(j + 1).Z());
    Y4 = gp_Vec2d(mySequence->Value(i)->Value(j + 2).Y(), 
      mySequence->Value(i)->Value(j + 2).Z());

    I1 = (Y1 - Y2)/(X1 - X2);
    I2 = (Y2 - Y3)/(X2 - X3);
    I3 = (Y3 - Y4)/(X3 - X4);

    I21 = (I1 - I2)/(X1 - X3);
    I22 = (I2 - I3)/(X2 - X4);

    I31 = (I21 - I22)/(X1 - X4);

    Res = Y1 + (U - X1)*(I1 + (U - X2)*(I21 + (U - X3)*I31));

    U0 = Res.X();
    V0 = Res.Y();

    if(U0 < mySurface->FirstUParameter()) U0 = mySurface->FirstUParameter();
    else if(U0 > mySurface->LastUParameter()) U0 = mySurface->LastUParameter();

    if(V0 < mySurface->FirstVParameter()) V0 = mySurface->FirstVParameter();
    else if(V0 > mySurface->LastVParameter()) V0 = mySurface->LastVParameter();
  }
  //End of cubic interpolation

  ProjLib_PrjResolve aPrjPS(myCurve->Curve(), mySurface->Surface(), 1);
  aPrjPS.Perform(U, U0, V0, gp_Pnt2d(myTolU, myTolV), 
    gp_Pnt2d(mySurface->FirstUParameter(), mySurface->FirstVParameter()), 
    gp_Pnt2d(mySurface->LastUParameter(), mySurface->LastVParameter()));
  if (aPrjPS.IsDone())
    P = aPrjPS.Solution();
  else
  {
    gp_Pnt thePoint = myCurve->Value(U);
    Extrema_ExtPS aExtPS(thePoint, mySurface->Surface(), myTolU, myTolV);
    if (aExtPS.IsDone() && aExtPS.NbExt()) 
    {
      Standard_Integer k, Nend, imin = 1;
      // Search for the nearest solution which is also a normal projection
      Nend = aExtPS.NbExt();
      for(k = 2; k <= Nend; k++)
        if (aExtPS.SquareDistance(k) < aExtPS.SquareDistance(imin))
          imin = k;
      const Extrema_POnSurf& POnS = aExtPS.Point(imin);
      Standard_Real ParU,ParV;
      POnS.Parameter(ParU, ParV);
      P.SetCoord(ParU, ParV);
    }
    else
      P.SetCoord(U0,V0);
  }
}
//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void ProjLib_CompProjectedCurve::D1(const Standard_Real t,
  gp_Pnt2d& P,
  gp_Vec2d& V) const
{
  Standard_Real u, v;
  D0(t, P);
  u = P.X();
  v = P.Y();
  d1(t, u, v, V, myCurve, mySurface);
}
//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void ProjLib_CompProjectedCurve::D2(const Standard_Real t,
  gp_Pnt2d& P,
  gp_Vec2d& V1,
  gp_Vec2d& V2) const
{
  Standard_Real u, v;
  D0(t, P);
  u = P.X();
  v = P.Y();
  d2(t, u, v, V1, V2, myCurve, mySurface);
}
//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

gp_Vec2d ProjLib_CompProjectedCurve::DN(const Standard_Real t, 
  const Standard_Integer N) const 
{
  if (N < 1 ) throw Standard_OutOfRange("ProjLib_CompProjectedCurve : N must be greater than 0");
  else if (N ==1) 
  {
    gp_Pnt2d P;
    gp_Vec2d V;
    D1(t,P,V);
    return V;
  }
  else if ( N==2)
  {
    gp_Pnt2d P;
    gp_Vec2d V1,V2;
    D2(t,P,V1,V2);
    return V2;
  }
  else if (N > 2 ) 
    throw Standard_NotImplemented("ProjLib_CompProjectedCurve::DN");
  return gp_Vec2d();
}

//=======================================================================
//function : GetSequence
//purpose  : 
//=======================================================================

const Handle(ProjLib_HSequenceOfHSequenceOfPnt)& ProjLib_CompProjectedCurve::GetSequence() const
{
  return mySequence;
}
//=======================================================================
//function : FirstParameter
//purpose  : 
//=======================================================================

Standard_Real ProjLib_CompProjectedCurve::FirstParameter() const
{
  return myCurve->FirstParameter();
}

//=======================================================================
//function : LastParameter
//purpose  : 
//=======================================================================

Standard_Real ProjLib_CompProjectedCurve::LastParameter() const
{
  return myCurve->LastParameter();
}

//=======================================================================
//function : MaxDistance
//purpose  : 
//=======================================================================

Standard_Real ProjLib_CompProjectedCurve::MaxDistance(const Standard_Integer Index) const
{
  if(Index < 1 || Index > myNbCurves) throw Standard_NoSuchObject();
  return myMaxDistance->Value(Index);
}

//=======================================================================
//function : NbIntervals
//purpose  : 
//=======================================================================

Standard_Integer ProjLib_CompProjectedCurve::NbIntervals(const GeomAbs_Shape S) const
{
  const_cast<ProjLib_CompProjectedCurve*>(this)->myTabInt.Nullify();
  BuildIntervals(S);
  return myTabInt->Length() - 1;
}

//=======================================================================
//function : Intervals
//purpose  : 
//=======================================================================

void ProjLib_CompProjectedCurve::Intervals(TColStd_Array1OfReal& T,const GeomAbs_Shape S) const
{
  if (myTabInt.IsNull()) BuildIntervals (S);
  T = myTabInt->Array1();
}

//=======================================================================
//function : BuildIntervals
//purpose  : 
//=======================================================================

void ProjLib_CompProjectedCurve::BuildIntervals(const GeomAbs_Shape S) const
{
  GeomAbs_Shape SforS = GeomAbs_CN;
  switch(S) {
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
  Standard_Integer i, j, k;
  Standard_Integer NbIntCur = myCurve->NbIntervals(S);
  Standard_Integer NbIntSurU = mySurface->NbUIntervals(SforS);
  Standard_Integer NbIntSurV = mySurface->NbVIntervals(SforS);

  TColStd_Array1OfReal CutPntsT(1, NbIntCur+1);
  TColStd_Array1OfReal CutPntsU(1, NbIntSurU+1);
  TColStd_Array1OfReal CutPntsV(1, NbIntSurV+1);

  myCurve->Intervals(CutPntsT, S);
  mySurface->UIntervals(CutPntsU, SforS);
  mySurface->VIntervals(CutPntsV, SforS);

  Standard_Real Tl, Tr, Ul, Ur, Vl, Vr, Tol;

  Handle(TColStd_HArray1OfReal) BArr = NULL, 
    CArr = NULL, 
    UArr = NULL, 
    VArr = NULL;

  // proccessing projection bounds
  BArr = new TColStd_HArray1OfReal(1, 2*myNbCurves);
  for(i = 1; i <= myNbCurves; i++)
  {
    Bounds(i, BArr->ChangeValue(2*i - 1), BArr->ChangeValue(2*i));
  }

  // proccessing curve discontinuities
  if(NbIntCur > 1) {
    CArr = new TColStd_HArray1OfReal(1, NbIntCur - 1);
    for(i = 1; i <= CArr->Length(); i++)
    {
      CArr->ChangeValue(i) = CutPntsT(i + 1);
    }
  }

  // proccessing U-surface discontinuities  
  TColStd_SequenceOfReal TUdisc;

  for(k = 2; k <= NbIntSurU; k++) {
    //    cout<<"CutPntsU("<<k<<") = "<<CutPntsU(k)<<endl;
    for(i = 1; i <= myNbCurves; i++)
    {
      for(j = 1; j < mySequence->Value(i)->Length(); j++)
      {
        Ul = mySequence->Value(i)->Value(j).Y();
        Ur = mySequence->Value(i)->Value(j + 1).Y();

        if(Abs(Ul - CutPntsU(k)) <= myTolU) 
          TUdisc.Append(mySequence->Value(i)->Value(j).X());
        else if(Abs(Ur - CutPntsU(k)) <= myTolU) 
          TUdisc.Append(mySequence->Value(i)->Value(j + 1).X());
        else if((Ul < CutPntsU(k) && CutPntsU(k) < Ur) ||
          (Ur < CutPntsU(k) && CutPntsU(k) < Ul)) 
        {
          Standard_Real V;
          V = (mySequence->Value(i)->Value(j).Z() 
            + mySequence->Value(i)->Value(j +1).Z())/2;
          ProjLib_PrjResolve Solver(myCurve->Curve(), mySurface->Surface(), 2);

          gp_Vec2d D;
          gp_Pnt Triple;
          Triple = mySequence->Value(i)->Value(j);
          d1(Triple.X(), Triple.Y(), Triple.Z(), D, myCurve, mySurface);
          if (Abs(D.X()) < Precision::Confusion()) 
            Tol = myTolU;
          else 
            Tol = Min(myTolU, myTolU / Abs(D.X()));

          Tl = mySequence->Value(i)->Value(j).X();
          Tr = mySequence->Value(i)->Value(j + 1).X();

          Solver.Perform((Tl + Tr)/2, CutPntsU(k), V, 
            gp_Pnt2d(Tol, myTolV), 
            gp_Pnt2d(Tl, mySurface->FirstVParameter()), 
            gp_Pnt2d(Tr, mySurface->LastVParameter()));
          //
          if(Solver.IsDone()) 
          {
            TUdisc.Append(Solver.Solution().X());
          }
        }
      }
    }
  }
  for(i = 2; i <= TUdisc.Length(); i++)
  {
    if(TUdisc(i) - TUdisc(i-1) < Precision::PConfusion())
    {
      TUdisc.Remove(i--);
    }
  }

  if(TUdisc.Length())
  {
    UArr = new TColStd_HArray1OfReal(1, TUdisc.Length());
    for(i = 1; i <= UArr->Length(); i++)
    {
      UArr->ChangeValue(i) = TUdisc(i);
    }
  }
  // proccessing V-surface discontinuities
  TColStd_SequenceOfReal TVdisc;

  for(k = 2; k <= NbIntSurV; k++)
  {
    for(i = 1; i <= myNbCurves; i++)
    {
      //      cout<<"CutPntsV("<<k<<") = "<<CutPntsV(k)<<endl;
      for(j = 1; j < mySequence->Value(i)->Length(); j++) {

        Vl = mySequence->Value(i)->Value(j).Z();
        Vr = mySequence->Value(i)->Value(j + 1).Z();

        if(Abs(Vl - CutPntsV(k)) <= myTolV) 
          TVdisc.Append(mySequence->Value(i)->Value(j).X());
        else if (Abs(Vr - CutPntsV(k)) <= myTolV) 
          TVdisc.Append(mySequence->Value(i)->Value(j + 1).X());
        else if((Vl < CutPntsV(k) && CutPntsV(k) < Vr) ||
          (Vr < CutPntsV(k) && CutPntsV(k) < Vl)) 
        {
          Standard_Real U;
          U = (mySequence->Value(i)->Value(j).Y() 
            + mySequence->Value(i)->Value(j +1).Y())/2;
          ProjLib_PrjResolve Solver(myCurve->Curve(), mySurface->Surface(), 3);

          gp_Vec2d D;
          gp_Pnt Triple;
          Triple = mySequence->Value(i)->Value(j);
          d1(Triple.X(), Triple.Y(), Triple.Z(), D, myCurve, mySurface);
          if (Abs(D.Y()) < Precision::Confusion()) 
            Tol = myTolV;
          else 
            Tol = Min(myTolV, myTolV / Abs(D.Y()));

          Tl = mySequence->Value(i)->Value(j).X();
          Tr = mySequence->Value(i)->Value(j + 1).X();

          Solver.Perform((Tl + Tr)/2, U, CutPntsV(k), 
            gp_Pnt2d(Tol, myTolV), 
            gp_Pnt2d(Tl, mySurface->FirstUParameter()), 
            gp_Pnt2d(Tr, mySurface->LastUParameter()));
          //
          if(Solver.IsDone()) 
          {
            TVdisc.Append(Solver.Solution().X());
          }
        }
      }
    }
  }

  for(i = 2; i <= TVdisc.Length(); i++)
  {
    if(TVdisc(i) - TVdisc(i-1) < Precision::PConfusion())
    {
      TVdisc.Remove(i--);
    }
  }

  if(TVdisc.Length())
  {
    VArr = new TColStd_HArray1OfReal(1, TVdisc.Length());
    for(i = 1; i <= VArr->Length(); i++)
    {
      VArr->ChangeValue(i) = TVdisc(i);
    }
  }

  // fusion
  TColStd_SequenceOfReal Fusion;
  if(!CArr.IsNull())
  {
    GeomLib::FuseIntervals(BArr->ChangeArray1(),
      CArr->ChangeArray1(),
      Fusion, Precision::PConfusion());
    BArr = new TColStd_HArray1OfReal(1, Fusion.Length());
    for(i = 1; i <= BArr->Length(); i++)
    {
      BArr->ChangeValue(i) = Fusion(i);
    }
    Fusion.Clear();
  }

  if(!UArr.IsNull())
  {
    GeomLib::FuseIntervals(BArr->ChangeArray1(),
      UArr->ChangeArray1(),
      Fusion, Precision::PConfusion());
    BArr = new TColStd_HArray1OfReal(1, Fusion.Length());
    for(i = 1; i <= BArr->Length(); i++)
    {
      BArr->ChangeValue(i) = Fusion(i);
    }
    Fusion.Clear();
  }

  if(!VArr.IsNull())
  {
    GeomLib::FuseIntervals(BArr->ChangeArray1(),
      VArr->ChangeArray1(),
      Fusion, Precision::PConfusion());
    BArr = new TColStd_HArray1OfReal(1, Fusion.Length());
    for(i = 1; i <= BArr->Length(); i++)
    {
      BArr->ChangeValue(i) = Fusion(i);
    }
  }

  const_cast<ProjLib_CompProjectedCurve*>(this)->myTabInt = new TColStd_HArray1OfReal(1, BArr->Length());
  for(i = 1; i <= BArr->Length(); i++)
  {
    myTabInt->ChangeValue(i) = BArr->Value(i);
  }
}

//=======================================================================
//function : Trim
//purpose  : 
//=======================================================================

Handle(Adaptor2d_HCurve2d) ProjLib_CompProjectedCurve::Trim
  (const Standard_Real First,
  const Standard_Real Last,
  const Standard_Real Tol) const 
{
  Handle(ProjLib_HCompProjectedCurve) HCS = 
    new ProjLib_HCompProjectedCurve(*this);
  HCS->ChangeCurve2d().Load(mySurface);
  HCS->ChangeCurve2d().Load(myCurve->Trim(First,Last,Tol));
  return HCS;
}

//=======================================================================
//function : GetType
//purpose  : 
//=======================================================================

GeomAbs_CurveType ProjLib_CompProjectedCurve::GetType() const 
{
  return GeomAbs_OtherCurve;
}

//=======================================================================
//function : UpdateTripleByTrapCriteria
//purpose  :
//=======================================================================
void ProjLib_CompProjectedCurve::UpdateTripleByTrapCriteria(gp_Pnt &thePoint) const
{
  Standard_Boolean isProblemsPossible = Standard_False;
  // Check possible traps cases:

  // 25892 bug.
  if (mySurface->GetType() == GeomAbs_SurfaceOfRevolution)
  {
    // Compute maximal deviation from 3D and choose the biggest one.
    Standard_Real aVRes = mySurface->VResolution(Precision::Confusion());
    Standard_Real aMaxTol = Max(Precision::PConfusion(), aVRes);

    if (Abs (thePoint.Z() - mySurface->FirstVParameter()) < aMaxTol ||
        Abs (thePoint.Z() - mySurface->LastVParameter() ) < aMaxTol )
    {
      isProblemsPossible = Standard_True;
    }
  }

  // 27135 bug. Trap on degenerated edge.
  if (mySurface->GetType() == GeomAbs_Sphere &&
     (Abs (thePoint.Z() - mySurface->FirstVParameter()) < Precision::PConfusion() ||
      Abs (thePoint.Z() - mySurface->LastVParameter() ) < Precision::PConfusion() ||
      Abs (thePoint.Y() - mySurface->FirstUParameter()) < Precision::PConfusion() ||
      Abs (thePoint.Y() - mySurface->LastUParameter() ) < Precision::PConfusion() ))
  {
    isProblemsPossible = Standard_True;
  }

  if (!isProblemsPossible)
    return;

  Standard_Real U,V;
  Standard_Boolean isDone = 
    InitialPoint(myCurve->Value(thePoint.X()), thePoint.X(), myCurve, mySurface, 
                 Precision::PConfusion(), Precision::PConfusion(), U, V);

  if (!isDone)
    return;

  // Restore original position in case of period jump.
  if (mySurface->IsUPeriodic() &&
      Abs (Abs(U - thePoint.Y()) - mySurface->UPeriod()) < Precision::PConfusion())
  {
    U = thePoint.Y();
  }
  if (mySurface->IsVPeriodic() &&
      Abs (Abs(V - thePoint.Z()) - mySurface->VPeriod()) < Precision::PConfusion())
  {
    V = thePoint.Z();
  }
  thePoint.SetY(U);
  thePoint.SetZ(V);
}

//=======================================================================
//function : BuildCurveSplits
//purpose  : 
//=======================================================================
void BuildCurveSplits(const Handle(Adaptor3d_HCurve)   &theCurve,
                      const Handle(Adaptor3d_HSurface) &theSurface,
                      const Standard_Real theTolU,
                      const Standard_Real theTolV,
                      NCollection_Vector<Standard_Real> &theSplits)
{
  SplitDS aDS(theCurve, theSurface, theSplits);

  Extrema_ExtPS anExtPS;
  anExtPS.Initialize(theSurface->Surface(),
                     theSurface->FirstUParameter(), theSurface->LastUParameter(),
                     theSurface->FirstVParameter(), theSurface->LastVParameter(),
                     theTolU, theTolV);
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
//function : SplitOnDirection
//purpose  : This method compute points in the parameter space of the curve
//           on which curve should be split since period jump is happen.
//=======================================================================
void SplitOnDirection(SplitDS & theSplitDS)
{
  // Algorithm:
  // Create 3D curve which is correspond to the periodic bound in 2d space.
  // Run curve / curve extrema and run extrema point / surface to check that
  // the point will be projected to the periodic bound.
  // In this method assumed that the points cannot be closer to each other that 1% of the parameter space.

  gp_Pnt2d aStartPnt(theSplitDS.mySurface->FirstUParameter(), theSplitDS.mySurface->FirstVParameter());
  gp_Dir2d aDir(theSplitDS.myPeriodicDir, (Standard_Integer)!theSplitDS.myPeriodicDir);

  theSplitDS.myPerMinParam = !theSplitDS.myPeriodicDir ? theSplitDS.mySurface->FirstUParameter():
                                                         theSplitDS.mySurface->FirstVParameter();
  theSplitDS.myPerMaxParam = !theSplitDS.myPeriodicDir ? theSplitDS.mySurface->LastUParameter():
                                                         theSplitDS.mySurface->LastVParameter();
  Standard_Real aLast2DParam = theSplitDS.myPeriodicDir ? 
                               theSplitDS.mySurface->LastUParameter() - theSplitDS.mySurface->FirstUParameter():
                               theSplitDS.mySurface->LastVParameter() - theSplitDS.mySurface->FirstVParameter();

  // Create line which is represent periodic border.
  Handle(Geom2d_Curve) aC2GC = new Geom2d_Line(aStartPnt, aDir);
  Handle(Geom2dAdaptor_HCurve) aC = new Geom2dAdaptor_HCurve(aC2GC, 0, aLast2DParam);
  Adaptor3d_CurveOnSurface  aCOnS(aC, theSplitDS.mySurface);

  Extrema_ExtCC anExtCC;
  anExtCC.SetCurve(1, aCOnS);
  anExtCC.SetCurve(2, theSplitDS.myCurve->Curve());
  anExtCC.SetSingleSolutionFlag(Standard_True); // Search only one solution since multiple invocations are needed.
  anExtCC.SetRange(1, 0, aLast2DParam);
  theSplitDS.myExtCC = &anExtCC;

  FindSplitPoint(theSplitDS,
                 theSplitDS.myCurve->FirstParameter(), // Initial curve range.
                 theSplitDS.myCurve->LastParameter());
}


//=======================================================================
//function : FindSplitPoint
//purpose  : 
//=======================================================================
void FindSplitPoint(SplitDS &theSplitDS,
                    const Standard_Real theMinParam,
                    const Standard_Real theMaxParam)
{
  // Make extrema copy to avoid dependencies between different levels of the recursion.
  Extrema_ExtCC anExtCC(*theSplitDS.myExtCC);
  anExtCC.SetRange(2, theMinParam, theMaxParam);
  anExtCC.Perform();

  if (anExtCC.IsDone())
  {
    const Standard_Integer aNbExt = anExtCC.NbExt();
    for (Standard_Integer anIdx = 1; anIdx <= aNbExt; ++anIdx)
    {
      Extrema_POnCurv aPOnC1, aPOnC2;
      anExtCC.Points(anIdx, aPOnC1, aPOnC2);

      theSplitDS.myExtPS->Perform(aPOnC2.Value());
      if (!theSplitDS.myExtPS->IsDone())
        return;

      // Find point with the minimal Euclidean distance to avoid
      // false positive points detection.
      Standard_Integer aMinIdx = -1;
      Standard_Real aMinSqDist = RealLast();
      const Standard_Integer aNbPext = theSplitDS.myExtPS->NbExt();
      for(Standard_Integer aPIdx = 1; aPIdx <= aNbPext; ++aPIdx)
      {
        const Standard_Real aCurrSqDist = theSplitDS.myExtPS->SquareDistance(aPIdx);

        if (aCurrSqDist < aMinSqDist)
        {
          aMinSqDist = aCurrSqDist;
          aMinIdx = aPIdx;
        }
      }

      // Check that is point will be projected to the periodic border.
      const Extrema_POnSurf &aPOnS = theSplitDS.myExtPS->Point(aMinIdx);
      Standard_Real U, V, aProjParam;
      aPOnS.Parameter(U, V);
      aProjParam = theSplitDS.myPeriodicDir ? V : U;


      if (Abs(aProjParam - theSplitDS.myPerMinParam) < Precision::PConfusion() ||
          Abs(aProjParam - theSplitDS.myPerMaxParam) < Precision::PConfusion() )
      {
        const Standard_Real aParam = aPOnC2.Parameter();
        const Standard_Real aCFParam = theSplitDS.myCurve->FirstParameter();
        const Standard_Real aCLParam = theSplitDS.myCurve->LastParameter();

        if (aParam > aCFParam + Precision::PConfusion() &&
            aParam < aCLParam  - Precision::PConfusion() )
        {
          // Add only inner points.
          theSplitDS.mySplits.Append(aParam);
        }

        const Standard_Real aDeltaCoeff = 0.01;
        const Standard_Real aDelta = (theMaxParam - theMinParam + 
                                      aCLParam - aCFParam) * aDeltaCoeff;

        if (aParam - aDelta > theMinParam + Precision::PConfusion())
        {
          FindSplitPoint(theSplitDS,
                         theMinParam, aParam - aDelta); // Curve parameters.
        }

        if (aParam + aDelta < theMaxParam - Precision::PConfusion())
        {
          FindSplitPoint(theSplitDS,
                         aParam + aDelta, theMaxParam); // Curve parameters.
        }
      }
    } // for (Standard_Integer anIdx = 1; anIdx <= aNbExt; ++anIdx)
  }
}
