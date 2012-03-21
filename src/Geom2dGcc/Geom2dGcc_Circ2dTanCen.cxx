// Created on: 1992-10-26
// Created by: Remi GILET
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <Geom2dGcc_Circ2dTanCen.ixx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GccAna_Circ2dTanCen.hxx>
#include <Geom2dGcc_MyCirc2dTanCen.hxx>
#include <Geom2dGcc_MyQCurve.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Line.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <GccEnt_QualifiedLin.hxx>
#include <StdFail_NotDone.hxx>

Geom2dGcc_Circ2dTanCen::
   Geom2dGcc_Circ2dTanCen (const Geom2dGcc_QualifiedCurve& Qualified1 ,
			   const Handle(Geom2d_Point&)     PCenter    ,
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
    Geom2dGcc_MyQCurve Qc1(C1,Qualified1.Qualifier());
    Geom2dGcc_MyCirc2dTanCen Circ(Qc1,pcenter,Tolerance);
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
  if (!WellDone) { StdFail_NotDone::Raise(); }
  if (Index <= 0 ||Index > NbrSol) { Standard_OutOfRange::Raise(); }
  return cirsol(Index); 
}

void Geom2dGcc_Circ2dTanCen::
  WhichQualifier(const Standard_Integer Index,
		       GccEnt_Position& Qualif1) const
{
  if (!WellDone) { StdFail_NotDone::Raise(); }
  else if (Index <= 0 ||Index > NbrSol) { Standard_OutOfRange::Raise(); }
  else { Qualif1 = qualifier1(Index); }
}

void Geom2dGcc_Circ2dTanCen::
  Tangency1 (const Standard_Integer Index,
	           Standard_Real&   ParSol,
	           Standard_Real&   ParArg,
	           gp_Pnt2d&        PntSol) const
{
  if (!WellDone) { StdFail_NotDone::Raise(); }
  else if (Index <= 0 ||Index > NbrSol) { Standard_OutOfRange::Raise(); }
  else {
    if (TheSame1(Index) == 0) {
      ParSol = par1sol(Index);
      ParArg = pararg1(Index);
      PntSol = pnttg1sol(Index);
    }
    else { StdFail_NotDone::Raise(); }
  }
}

Standard_Boolean Geom2dGcc_Circ2dTanCen::
   IsTheSame1 (const Standard_Integer Index) const
{
  if (!WellDone) { StdFail_NotDone::Raise(); }
  if (Index <= 0 ||Index > NbrSol) { Standard_OutOfRange::Raise(); }
  if (TheSame1(Index) == 0) { return Standard_False; }
  return Standard_True; 
}

