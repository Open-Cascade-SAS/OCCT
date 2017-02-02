// Created on: 1992-10-26
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


#include <GccAna_Circ2dTanCen.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <GccEnt_QualifiedLin.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Point.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dGcc_Circ2dTanCen.hxx>
#include <Geom2dGcc_Circ2dTanCenGeo.hxx>
#include <Geom2dGcc_QCurve.hxx>
#include <Geom2dGcc_QualifiedCurve.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

Geom2dGcc_Circ2dTanCen::
   Geom2dGcc_Circ2dTanCen (const Geom2dGcc_QualifiedCurve& Qualified1 ,
			   const Handle(Geom2d_Point)&     PCenter    ,
			   const Standard_Real             Tolerance  ):
  cirsol(1,2)   ,
  qualifier1(1,2),
  TheSame1(1,2) ,
  pnttg1sol(1,2),
  par1sol(1,2)  ,
  pararg1(1,2)  
{
  Geom2dAdaptor_Curve C1 = Qualified1.Qualified();
  Handle(Geom2d_Curve) CC1 = C1.Curve();
  GeomAbs_CurveType Type1 = C1.GetType();

//=============================================================================
//                            Appel a GccAna.                                 +
//=============================================================================

  gp_Pnt2d pcenter(PCenter->Pnt2d());
  NbrSol = 0;
  if ((Type1 == GeomAbs_Line || Type1 == GeomAbs_Circle)) {
    if (Type1 == GeomAbs_Circle) {
      Handle(Geom2d_Circle) CCC1 = Handle(Geom2d_Circle)::DownCast(CC1);
      gp_Circ2d c1(CCC1->Circ2d());
      GccEnt_QualifiedCirc Qc1(c1,Qualified1.Qualifier());
      GccAna_Circ2dTanCen Circ(Qc1,pcenter,Tolerance);
      WellDone = Circ.IsDone();
      NbrSol = Circ.NbSolutions();
      for (Standard_Integer j = 1; j <= NbrSol; j++) {
	cirsol(j)   = Circ.ThisSolution(j);
	Circ.WhichQualifier(j,qualifier1(j));
	if (Circ.IsTheSame1(j)) { TheSame1(j) = 1; }
	else {TheSame1(j) = 0; }
	Circ.Tangency1(j,par1sol(j),pararg1(j),pnttg1sol(j));
      }
    }
    else {
      Handle(Geom2d_Line) LL1 = Handle(Geom2d_Line)::DownCast(CC1);
      gp_Lin2d l1(LL1->Lin2d());
      GccAna_Circ2dTanCen Circ(l1,pcenter);
      WellDone = Circ.IsDone();
      NbrSol = Circ.NbSolutions();
      for (Standard_Integer j = 1; j <= NbrSol; j++) {
	cirsol(j)   = Circ.ThisSolution(j);
	Circ.WhichQualifier(j,qualifier1(j));
	if (Circ.IsTheSame1(j)) { TheSame1(j) = 1; }
	else {TheSame1(j) = 0; }
	Circ.Tangency1(j,par1sol(j),pararg1(j),pnttg1sol(j));
      }
    }
  }

//=============================================================================
//                            Appel a GccGeo.                                 +
//=============================================================================

  else {
    Geom2dGcc_QCurve Qc1(C1,Qualified1.Qualifier());
    Geom2dGcc_Circ2dTanCenGeo Circ(Qc1,pcenter,Tolerance);
    WellDone = Circ.IsDone();
    NbrSol = Circ.NbSolutions();
    for (Standard_Integer j = 1; j <= NbrSol; j++) {
      cirsol(j)   = Circ.ThisSolution(j);
      TheSame1(j) = 0;
      Circ.Tangency1(j,par1sol(j),pararg1(j),pnttg1sol(j));
      Circ.WhichQualifier(j,qualifier1(j));
    }
  }
}

Standard_Boolean Geom2dGcc_Circ2dTanCen::
   IsDone () const { return WellDone; }

Standard_Integer Geom2dGcc_Circ2dTanCen::
  NbSolutions () const 
{ 
  return NbrSol;
}

gp_Circ2d Geom2dGcc_Circ2dTanCen::
  ThisSolution (const Standard_Integer Index) const 
{
  if (!WellDone) { throw StdFail_NotDone(); }
  if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
  return cirsol(Index); 
}

void Geom2dGcc_Circ2dTanCen::
  WhichQualifier(const Standard_Integer Index,
		       GccEnt_Position& Qualif1) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
  else if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
  else { Qualif1 = qualifier1(Index); }
}

void Geom2dGcc_Circ2dTanCen::
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

Standard_Boolean Geom2dGcc_Circ2dTanCen::
   IsTheSame1 (const Standard_Integer Index) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
  if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
  if (TheSame1(Index) == 0) { return Standard_False; }
  return Standard_True; 
}

