// Created on: 1992-10-21
// Created by: Remi GILET
// Copyright (c) 1992-1999 Matra Datavision
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


#include <GccAna_Circ2dTanOnRad.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <GccEnt_QualifiedLin.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Point.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dGcc_Circ2dTanOnRad.hxx>
#include <Geom2dGcc_Circ2dTanOnRadGeo.hxx>
#include <Geom2dGcc_QCurve.hxx>
#include <Geom2dGcc_QualifiedCurve.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_NegativeValue.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

Geom2dGcc_Circ2dTanOnRad::
   Geom2dGcc_Circ2dTanOnRad (const Geom2dGcc_QualifiedCurve& Qualified1 ,
			     const Geom2dAdaptor_Curve&      OnCurve    ,
			     const Standard_Real             Radius     ,
			     const Standard_Real             Tolerance  ):
  cirsol(1,8)   ,
  qualifier1(1,8),
  TheSame1(1,8) ,
  pnttg1sol(1,8),
  par1sol(1,8)  ,
  pararg1(1,8)  ,
  pntcen3(1,8)   ,
  parcen3(1,8)  
{
  if (Radius < 0.) {
    throw Standard_NegativeValue();
  }
  else {
    Geom2dAdaptor_Curve C1 = Qualified1.Qualified();
    GeomAbs_CurveType Type1 = C1.GetType();
    GeomAbs_CurveType Type2 = OnCurve.GetType();
    Handle(Geom2d_Curve) CC1 = C1.Curve();
    Handle(Geom2d_Curve) Con = OnCurve.Curve();

//=============================================================================
//                            Appel a GccAna.                                 +
//=============================================================================

    NbrSol = 0;
    if ((Type1 == GeomAbs_Line || Type1 == GeomAbs_Circle) &&
	(Type2 == GeomAbs_Line || Type2 == GeomAbs_Circle)) {
      if (Type1 == GeomAbs_Circle) {
	Handle(Geom2d_Circle) CCC1 = Handle(Geom2d_Circle)::DownCast(CC1);
	gp_Circ2d c1(CCC1->Circ2d());
	GccEnt_QualifiedCirc Qc1=GccEnt_QualifiedCirc(c1,
						      Qualified1.Qualifier());
	if (Type2 == GeomAbs_Circle) {
	  Handle(Geom2d_Circle) CCon = Handle(Geom2d_Circle)::DownCast(Con);
	  gp_Circ2d con(CCon->Circ2d());
	  GccAna_Circ2dTanOnRad Circ(Qc1,con,Radius,Tolerance);
	  WellDone = Circ.IsDone();
	  NbrSol = Circ.NbSolutions();
	  Results(Circ);
	}
	else {
	  Handle(Geom2d_Line) LLon = Handle(Geom2d_Line)::DownCast(Con);
	  gp_Lin2d lon(LLon->Lin2d());
	  GccAna_Circ2dTanOnRad Circ(Qc1,lon,Radius,Tolerance);
	  WellDone = Circ.IsDone();
	  NbrSol = Circ.NbSolutions();
	  Results(Circ);
	}
      }
      else {
	Handle(Geom2d_Line) LL1 = Handle(Geom2d_Line)::DownCast(CC1);
	gp_Lin2d l1(LL1->Lin2d());
	GccEnt_QualifiedLin Ql1=GccEnt_QualifiedLin(l1,Qualified1.Qualifier());
	if (Type2 == GeomAbs_Circle) {
	  Handle(Geom2d_Circle) CCon = Handle(Geom2d_Circle)::DownCast(Con);
	  gp_Circ2d con(CCon->Circ2d());
	  GccAna_Circ2dTanOnRad Circ(Ql1,con,Radius,Tolerance);
	  WellDone = Circ.IsDone();
	  NbrSol = Circ.NbSolutions();
	  Results(Circ);
	}
	else {
	  Handle(Geom2d_Line) LLon = Handle(Geom2d_Line)::DownCast(Con);
	  gp_Lin2d lon(LLon->Lin2d());
	  GccAna_Circ2dTanOnRad Circ(Ql1,lon,Radius,Tolerance);
	  WellDone = Circ.IsDone();
	  NbrSol = Circ.NbSolutions();
	  Results(Circ);
	}
      }
    }

//=============================================================================
//                            Appel a GccGeo.                                 +
//=============================================================================

    else {
      if (Type1 == GeomAbs_Circle) {
	Handle(Geom2d_Circle) CCC1 = Handle(Geom2d_Circle)::DownCast(CC1);
	gp_Circ2d c1(CCC1->Circ2d());
	GccEnt_QualifiedCirc Qc1=GccEnt_QualifiedCirc(c1,
						      Qualified1.Qualifier());
	Geom2dGcc_Circ2dTanOnRadGeo CircGeo(Qc1,OnCurve,Radius,Tolerance);
	WellDone = CircGeo.IsDone();
	NbrSol = CircGeo.NbSolutions();
	Results(CircGeo);
      }
      else if (Type1 == GeomAbs_Line) {
	Handle(Geom2d_Line) LL1 = Handle(Geom2d_Line)::DownCast(CC1);
	gp_Lin2d l1(LL1->Lin2d());
	GccEnt_QualifiedLin Ql1=GccEnt_QualifiedLin(l1,Qualified1.Qualifier());
	Geom2dGcc_Circ2dTanOnRadGeo CircGeo(Ql1,OnCurve,Radius,Tolerance);
	WellDone = CircGeo.IsDone();
	NbrSol = CircGeo.NbSolutions();
	Results(CircGeo);
      }
      else {
	Geom2dGcc_QCurve Qc1(C1,Qualified1.Qualifier());
	Geom2dGcc_Circ2dTanOnRadGeo CircGeo(Qc1,OnCurve,Radius,Tolerance);
	WellDone = CircGeo.IsDone();
	NbrSol = CircGeo.NbSolutions();
	Results(CircGeo);
      }
    }
  }
}

Geom2dGcc_Circ2dTanOnRad::
   Geom2dGcc_Circ2dTanOnRad (const Handle(Geom2d_Point)&     Point1     ,
			     const Geom2dAdaptor_Curve&     OnCurve    ,
			     const Standard_Real             Radius     ,
			     const Standard_Real             Tolerance  ):
  cirsol(1,8)   ,
  qualifier1(1,8),
  TheSame1(1,8) ,
  pnttg1sol(1,8),
  par1sol(1,8)  ,
  pararg1(1,8)  ,
  pntcen3(1,8)   ,
  parcen3(1,8)  
{
  if (Radius < 0.) {
    throw Standard_NegativeValue();
  }
  else {
    gp_Pnt2d point1(Point1->Pnt2d());
    GeomAbs_CurveType Type2 = OnCurve.GetType();
    Handle(Geom2d_Curve) Con = OnCurve.Curve();

//=============================================================================
//                            Appel a GccAna.                                 +
//=============================================================================

    NbrSol = 0;
    if (Type2 == GeomAbs_Line || Type2 == GeomAbs_Circle) {
      if (Type2 == GeomAbs_Circle) {
	Handle(Geom2d_Circle) CCon = Handle(Geom2d_Circle)::DownCast(Con);
	gp_Circ2d con(CCon->Circ2d());
	GccAna_Circ2dTanOnRad Circ(point1,con,Radius,Tolerance);
	WellDone = Circ.IsDone();
	NbrSol = Circ.NbSolutions();
	Results(Circ);
      }
      else {
	Handle(Geom2d_Line) LLon = Handle(Geom2d_Line)::DownCast(Con);
	gp_Lin2d lon(LLon->Lin2d());
	GccAna_Circ2dTanOnRad Circ(point1,lon,Radius,Tolerance);
	WellDone = Circ.IsDone();
	NbrSol = Circ.NbSolutions();
	Results(Circ);
      }
    }

//=============================================================================
//                            Appel a GccGeo.                                 +
//=============================================================================

    else {
      Geom2dGcc_Circ2dTanOnRadGeo CircGeo(point1,OnCurve,Radius,Tolerance);
      WellDone = CircGeo.IsDone();
      NbrSol = CircGeo.NbSolutions();
      Results(CircGeo);
    }
  }
}

void Geom2dGcc_Circ2dTanOnRad::Results(const GccAna_Circ2dTanOnRad& Circ)
{
  for (Standard_Integer j = 1; j <= NbrSol; j++) {
    cirsol(j)   = Circ.ThisSolution(j);
    if (Circ.IsTheSame1(j)) { TheSame1(j) = 1; }
    else {TheSame1(j) = 0; }
    Circ.Tangency1(j,par1sol(j),pararg1(j),pnttg1sol(j));
    Circ.CenterOn3(j,parcen3(j),pntcen3(j));
    Circ.WhichQualifier(j,qualifier1(j));
  }
}

void Geom2dGcc_Circ2dTanOnRad::Results(const Geom2dGcc_Circ2dTanOnRadGeo& Circ)
{
  for (Standard_Integer j = 1; j <= NbrSol; j++) {
    cirsol(j)   = Circ.ThisSolution(j);
    if (Circ.IsTheSame1(j)) { TheSame1(j) = 1; }
    else {TheSame1(j) = 0; }
    Circ.Tangency1(j,par1sol(j),pararg1(j),pnttg1sol(j));
    Circ.CenterOn3(j,parcen3(j),pntcen3(j));
    Circ.WhichQualifier(j,qualifier1(j));
  }
}

Standard_Boolean Geom2dGcc_Circ2dTanOnRad::
   IsDone () const { return WellDone; }

Standard_Integer Geom2dGcc_Circ2dTanOnRad::
  NbSolutions () const 
{ 
  return NbrSol;
}

gp_Circ2d Geom2dGcc_Circ2dTanOnRad::
  ThisSolution (const Standard_Integer Index) const 
{
  if (!WellDone) { throw StdFail_NotDone(); }
  if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
  return cirsol(Index);
}

void Geom2dGcc_Circ2dTanOnRad::
  WhichQualifier (const Standard_Integer Index,
		        GccEnt_Position& Qualif1) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
  else if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
  else { Qualif1 = qualifier1(Index); }
}

void Geom2dGcc_Circ2dTanOnRad::
  Tangency1 (const Standard_Integer Index,
	           Standard_Real&   ParSol,
	           Standard_Real&   ParArg,
	           gp_Pnt2d&        PntSol) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
  else if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
  else {
    if (TheSame1(Index) == 0) {
      ParSol = par1sol(Index);
      ParArg = pararg1(Index);
      PntSol = pnttg1sol(Index);
    }
    else { throw StdFail_NotDone(); }
  }
}

void Geom2dGcc_Circ2dTanOnRad::
   CenterOn3 (const Standard_Integer Index,
              Standard_Real& ParArg,
              gp_Pnt2d& PntSol) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
  else if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
  else {
    ParArg = parcen3(Index);
    PntSol = pntcen3(Index);
  }
}

Standard_Boolean Geom2dGcc_Circ2dTanOnRad::
   IsTheSame1 (const Standard_Integer Index) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
  if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
  if (TheSame1(Index) == 0) { return Standard_False; }
  return Standard_True; 
}


