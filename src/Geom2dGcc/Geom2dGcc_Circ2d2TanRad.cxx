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


#include <GccAna_Circ2d2TanRad.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <GccEnt_QualifiedLin.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Point.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dGcc_Circ2d2TanRad.hxx>
#include <Geom2dGcc_Circ2d2TanRadGeo.hxx>
#include <Geom2dGcc_QCurve.hxx>
#include <Geom2dGcc_QualifiedCurve.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_NegativeValue.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

static const Standard_Integer aNbSolMAX = 16;

// circulaire tangent a deux cercles et de rayon donne
//====================================================
//========================================================================
// On initialise WellDone a false.                                       +
// On recupere le cercle C1 et le cercle C2.                             +
// On sort en erreur dans les cas ou la construction est impossible.     +
// On distingue les cas limites pour les triater separement.             +
// On fait la parallele a C1 dans le bon sens.                           +
// On fait la parallele a C2 dans le bon sens.                           +
// On intersecte les paralleles ==> point de centre de la solution.      +
// On cree la solution qu on ajoute aux solutions deja trouvees.         +
// On remplit les champs.                                                +
//========================================================================

Geom2dGcc_Circ2d2TanRad::
   Geom2dGcc_Circ2d2TanRad (const Geom2dGcc_QualifiedCurve& Qualified1 ,
			    const Geom2dGcc_QualifiedCurve& Qualified2 ,
			    const Standard_Real             Radius     ,
			    const Standard_Real             Tolerance  ):
  cirsol(1,aNbSolMAX)   ,
  qualifier1(1,aNbSolMAX),
  qualifier2(1,aNbSolMAX),
  TheSame1(1,aNbSolMAX) ,
  TheSame2(1,aNbSolMAX) ,
  pnttg1sol(1,aNbSolMAX),
  pnttg2sol(1,aNbSolMAX),
  par1sol(1,aNbSolMAX)  ,
  par2sol(1,aNbSolMAX)  ,
  pararg1(1,aNbSolMAX)  ,
  pararg2(1,aNbSolMAX)  
{
  if (Radius < 0.) { throw Standard_NegativeValue(); }
  else {
    Geom2dAdaptor_Curve C1 = Qualified1.Qualified();
    Geom2dAdaptor_Curve C2 = Qualified2.Qualified();
    Handle(Geom2d_Curve) CC1 = C1.Curve();
    Handle(Geom2d_Curve) CC2 = C2.Curve();
    GeomAbs_CurveType Type1 = C1.GetType();
    GeomAbs_CurveType Type2 = C2.GetType();

//=============================================================================
//                            Appel a GccAna.                                 +
//=============================================================================

    Invert = Standard_False;
    NbrSol = 0;
    if ((Type1 == GeomAbs_Line || Type1 == GeomAbs_Circle) &&
	(Type2 == GeomAbs_Line || Type2 == GeomAbs_Circle)) {
      if (Type1 == GeomAbs_Circle) {
	Handle(Geom2d_Circle) CCC1 = Handle(Geom2d_Circle)::DownCast(CC1);
	gp_Circ2d c1(CCC1->Circ2d());
	GccEnt_QualifiedCirc Qc1 = GccEnt_QualifiedCirc(c1,
						       Qualified1.Qualifier());
	if (Type2 == GeomAbs_Circle) {
	  Handle(Geom2d_Circle) CCC2 = Handle(Geom2d_Circle)::DownCast(CC2);
	  gp_Circ2d c2(CCC2->Circ2d());
	  GccAna_Circ2d2TanRad CircAna(Qc1,
			       GccEnt_QualifiedCirc(c2,Qualified2.Qualifier()),
				       Radius,Tolerance);
	  WellDone = CircAna.IsDone();
	  NbrSol = CircAna.NbSolutions();
	  for(Standard_Integer i=1; i<=NbrSol; i++) {
	    CircAna.WhichQualifier(i,qualifier1(i),qualifier2(i));
	  }
	  Results(CircAna);
	}
	else {
	  Handle(Geom2d_Line) LL2 = Handle(Geom2d_Line)::DownCast(CC2);
	  gp_Lin2d l2(LL2->Lin2d());
	  if (!Qualified2.IsEnclosing()) {
	    GccAna_Circ2d2TanRad CircAna(Qc1,
				GccEnt_QualifiedLin(l2,Qualified2.Qualifier()),
					 Radius,Tolerance);
	    WellDone = CircAna.IsDone();
	    NbrSol = CircAna.NbSolutions();
	    for(Standard_Integer i=1; i<=NbrSol; i++) {
	      CircAna.WhichQualifier(i,qualifier1(i),qualifier2(i));
	    }
	    Results(CircAna);
	  }
	  else { 
	    WellDone = Standard_False;
	    throw GccEnt_BadQualifier();
	  }
	}
      }
      else {
	Handle(Geom2d_Line) LL1 = Handle(Geom2d_Line)::DownCast(CC1);
	gp_Lin2d l1(LL1->Lin2d());
	if (Qualified1.IsEnclosing()) {
	  WellDone = Standard_False;
	  throw GccEnt_BadQualifier();
	}
	else {
	  GccEnt_QualifiedLin Ql1 = GccEnt_QualifiedLin(l1,
						       Qualified1.Qualifier());
	  if (Type2 == GeomAbs_Circle) {
	    Handle(Geom2d_Circle) CCC2 = Handle(Geom2d_Circle)::DownCast(CC2);
	    gp_Circ2d c2(CCC2->Circ2d());
	    Invert = Standard_True;
	    GccAna_Circ2d2TanRad CircAna(GccEnt_QualifiedCirc(c2,
						       Qualified2.Qualifier()),
					 Ql1,Radius,Tolerance);
	    WellDone = CircAna.IsDone();
	    NbrSol = CircAna.NbSolutions();
	    for(Standard_Integer i=1; i<=NbrSol; i++) {
	      CircAna.WhichQualifier(i,qualifier1(i),qualifier2(i));
	    }
	    Results(CircAna);
	  }
	  else {
	    Handle(Geom2d_Line) LL2 = Handle(Geom2d_Line)::DownCast(CC2);
	    gp_Lin2d l2(LL2->Lin2d());
	    if (!Qualified2.IsEnclosing()) {
	      GccAna_Circ2d2TanRad CircAna(Ql1,
				GccEnt_QualifiedLin(l2,Qualified2.Qualifier()),
					   Radius,Tolerance);
	      WellDone = CircAna.IsDone();
	      NbrSol = CircAna.NbSolutions();
	      for(Standard_Integer i=1; i<=NbrSol; i++) {
		CircAna.WhichQualifier(i,qualifier1(i),qualifier2(i));
	      }
	      Results(CircAna);
	    }
	    else { 
	      WellDone = Standard_False;
	      throw GccEnt_BadQualifier();
	    }
	  }
	}
      }
    }
//=============================================================================
//                            Appel a GccGeo.                                 +
//=============================================================================
    else {
      if (Type1 == GeomAbs_Line) {
	Handle(Geom2d_Line) LL1 = Handle(Geom2d_Line)::DownCast(CC1);
	gp_Lin2d l1(LL1->Lin2d());
	if (Qualified1.IsEnclosing()) {
	  WellDone = Standard_False;
	  throw GccEnt_BadQualifier();
	}
	else {
	  GccEnt_QualifiedLin Ql1 = GccEnt_QualifiedLin(l1,
						       Qualified1.Qualifier());
	  Geom2dGcc_QCurve Qc2(C2,Qualified2.Qualifier());
	  Geom2dGcc_Circ2d2TanRadGeo CircGeo(Ql1,Qc2,Radius,Tolerance);
	  WellDone = CircGeo.IsDone();
	  NbrSol = CircGeo.NbSolutions();
	  for(Standard_Integer i=1; i<=NbrSol; i++) {
	    CircGeo.WhichQualifier(i,qualifier1(i),qualifier2(i));
	  }
	  Results(CircGeo);
	}
      }
      else if (Type1 == GeomAbs_Circle) {
	Handle(Geom2d_Circle) CCC1 = Handle(Geom2d_Circle)::DownCast(CC1);
	gp_Circ2d c1(CCC1->Circ2d());
	GccEnt_QualifiedCirc Qc1 = GccEnt_QualifiedCirc(c1,
						       Qualified1.Qualifier());
	Geom2dGcc_QCurve Qc2(C2,Qualified2.Qualifier());
	Geom2dGcc_Circ2d2TanRadGeo CircGeo(Qc1,Qc2,Radius,Tolerance);
	WellDone = CircGeo.IsDone();
	NbrSol = CircGeo.NbSolutions();
	for(Standard_Integer i=1; i<=NbrSol; i++) {
	  CircGeo.WhichQualifier(i,qualifier1(i),qualifier2(i));
	}
	Results(CircGeo);
      }
      else if (Type2 == GeomAbs_Line) {
	Invert = Standard_True;
	Handle(Geom2d_Line) LL2 = Handle(Geom2d_Line)::DownCast(CC2);
	gp_Lin2d l2(LL2->Lin2d());
	if (Qualified2.IsEnclosing()) {
	  WellDone = Standard_False;
	  throw GccEnt_BadQualifier();
	}
	else {
	  GccEnt_QualifiedLin Ql2 = GccEnt_QualifiedLin(l2,
						       Qualified2.Qualifier());
	  Geom2dGcc_QCurve Qc1(C1,Qualified1.Qualifier());
	  Geom2dGcc_Circ2d2TanRadGeo CircGeo(Ql2,Qc1,Radius,Tolerance);
	  WellDone = CircGeo.IsDone();
	  NbrSol = CircGeo.NbSolutions();
	  for(Standard_Integer i=1; i<=NbrSol; i++) {
	    CircGeo.WhichQualifier(i,qualifier1(i),qualifier2(i));
	  }
	  Results(CircGeo);
	}
      }
      else if (Type2 == GeomAbs_Circle) {
	Invert = Standard_True;
	Handle(Geom2d_Circle) CCC2 = Handle(Geom2d_Circle)::DownCast(CC2);
	gp_Circ2d c2(CCC2->Circ2d());
	GccEnt_QualifiedCirc Qc2 = GccEnt_QualifiedCirc(c2,
						       Qualified2.Qualifier());
	Geom2dGcc_QCurve Qc1(C1,Qualified1.Qualifier());
	Geom2dGcc_Circ2d2TanRadGeo CircGeo(Qc2,Qc1,Radius,Tolerance);
	WellDone = CircGeo.IsDone();
	NbrSol = CircGeo.NbSolutions();
	for(Standard_Integer i=1; i<=NbrSol; i++) {
	  CircGeo.WhichQualifier(i,qualifier1(i),qualifier2(i));
	}
	Results(CircGeo);
      }
      else {
	Geom2dGcc_QCurve Qc1(C1,Qualified1.Qualifier());
	Geom2dGcc_QCurve Qc2(C2,Qualified2.Qualifier());
	Geom2dGcc_Circ2d2TanRadGeo CircGeo(Qc1,Qc2,Radius,Tolerance);
	WellDone = CircGeo.IsDone();
	NbrSol = CircGeo.NbSolutions();
	for(Standard_Integer i=1; i<=NbrSol; i++) {
	  CircGeo.WhichQualifier(i,qualifier1(i),qualifier2(i));
	}
	Results(CircGeo);
      }
    }
  }
}

Geom2dGcc_Circ2d2TanRad::
   Geom2dGcc_Circ2d2TanRad (const Geom2dGcc_QualifiedCurve& Qualified1 ,
			    const Handle(Geom2d_Point)&     Point      ,
			    const Standard_Real             Radius     ,
			    const Standard_Real             Tolerance  ):
  cirsol(1,8)   ,
  qualifier1(1,8),
  qualifier2(1,8),
  TheSame1(1,8) ,
  TheSame2(1,8) ,
  pnttg1sol(1,8),
  pnttg2sol(1,8),
  par1sol(1,8)  ,
  par2sol(1,8)  ,
  pararg1(1,8)  ,
  pararg2(1,8)  
{
  if (Radius < 0.) { throw Standard_NegativeValue(); }
  else {
    Geom2dAdaptor_Curve C1 = Qualified1.Qualified();
    Handle(Geom2d_Curve) CC1 = C1.Curve();
    GeomAbs_CurveType Type1 = C1.GetType();

//=============================================================================
//                            Appel a GccAna.                                 +
//=============================================================================

    Invert = Standard_False;
    NbrSol = 0;
    if (Type1 == GeomAbs_Line || Type1 == GeomAbs_Circle) {
      if (Type1 == GeomAbs_Circle) {
	Handle(Geom2d_Circle) CCC1 = Handle(Geom2d_Circle)::DownCast(CC1);
	gp_Circ2d c1(CCC1->Circ2d());
	GccEnt_QualifiedCirc Qc1(c1,Qualified1.Qualifier());
	GccAna_Circ2d2TanRad CircAna(Qc1,Point->Pnt2d(),Radius,Tolerance);
	WellDone = CircAna.IsDone();
	NbrSol = CircAna.NbSolutions();
	for(Standard_Integer i=1; i<=NbrSol; i++) {
	  CircAna.WhichQualifier(i,qualifier1(i),qualifier2(i));
	}
	Results(CircAna);
      }
      else {
	Handle(Geom2d_Line) LLL1 = Handle(Geom2d_Line)::DownCast(CC1);
	gp_Lin2d l1(LLL1->Lin2d());
	GccEnt_QualifiedLin Ql1(l1,Qualified1.Qualifier());
	GccAna_Circ2d2TanRad CircAna(Ql1,Point->Pnt2d(),Radius,Tolerance);
	WellDone = CircAna.IsDone();
	NbrSol = CircAna.NbSolutions();
	for(Standard_Integer i=1; i<=NbrSol; i++) {
	  CircAna.WhichQualifier(i,qualifier1(i),qualifier2(i));
	}
	Results(CircAna);
      }
    }
//=============================================================================
//                            Appel a GccGeo.                                 +
//=============================================================================
    else {
      Geom2dGcc_QCurve Qc1(C1,Qualified1.Qualifier());
      Geom2dGcc_Circ2d2TanRadGeo CircGeo(Qc1,Point->Pnt2d(),Radius,Tolerance);
      WellDone = CircGeo.IsDone();
      NbrSol = CircGeo.NbSolutions();
      for(Standard_Integer i=1; i<=NbrSol; i++) {
	CircGeo.WhichQualifier(i,qualifier1(i),qualifier2(i));
      }
      Results(CircGeo);
    }
  }
}
  
Geom2dGcc_Circ2d2TanRad::
   Geom2dGcc_Circ2d2TanRad (const Handle(Geom2d_Point)& Point1     ,
			    const Handle(Geom2d_Point)& Point2     ,
			    const Standard_Real         Radius     ,
			    const Standard_Real         Tolerance  ):
  cirsol(1,2)   ,
  qualifier1(1,2),
  qualifier2(1,2),
  TheSame1(1,2) ,
  TheSame2(1,2) ,
  pnttg1sol(1,2),
  pnttg2sol(1,2),
  par1sol(1,2)  ,
  par2sol(1,2)  ,
  pararg1(1,2)  ,
  pararg2(1,2)  
{
  if (Radius < 0.) { throw Standard_NegativeValue(); }
  else {

//=============================================================================
//                            Appel a GccAna.                                 +
//=============================================================================

    Invert = Standard_False;
    NbrSol = 0;
    GccAna_Circ2d2TanRad CircAna(Point1->Pnt2d(),Point2->Pnt2d(),
				 Radius,Tolerance);
    WellDone = CircAna.IsDone();
    NbrSol = CircAna.NbSolutions();
    for(Standard_Integer i=1; i<=NbrSol; i++) {
      CircAna.WhichQualifier(i,qualifier1(i),qualifier2(i));
    }
    Results(CircAna);
  }
}

void Geom2dGcc_Circ2d2TanRad::Results(const GccAna_Circ2d2TanRad& Circ)
{
  for (Standard_Integer j = 1; j <= NbrSol; j++) {
    cirsol(j)   = Circ.ThisSolution(j);
    if (Circ.IsTheSame1(j)) { TheSame1(j) = 1; }
    else {TheSame1(j) = 0; }
    if (Circ.IsTheSame2(j)) { TheSame2(j) = 1; }
    else {TheSame2(j) = 0; }
    Circ.Tangency1(j,par1sol(j),pararg1(j),pnttg1sol(j));
    Circ.Tangency2(j,par2sol(j),pararg2(j),pnttg2sol(j));
  }
}

void Geom2dGcc_Circ2d2TanRad::Results(const Geom2dGcc_Circ2d2TanRadGeo& Circ)
{
  for (Standard_Integer j = 1; j <= NbrSol; j++) {
    cirsol(j)   = Circ.ThisSolution(j);
    if (Circ.IsTheSame1(j)) { TheSame1(j) = 1; }
    else {TheSame1(j) = 0; }
    if (Circ.IsTheSame2(j)) { TheSame2(j) = 1; }
    else {TheSame2(j) = 0; }
    Circ.Tangency1(j,par1sol(j),pararg1(j),pnttg1sol(j));
    Circ.Tangency2(j,par2sol(j),pararg2(j),pnttg2sol(j));
  }
}

Standard_Boolean Geom2dGcc_Circ2d2TanRad::
   IsDone () const { return WellDone; }

Standard_Integer Geom2dGcc_Circ2d2TanRad::
  NbSolutions () const 
{ 
  return NbrSol;
}

gp_Circ2d Geom2dGcc_Circ2d2TanRad::
  ThisSolution (const Standard_Integer Index) const 
{
  if (!WellDone) { throw StdFail_NotDone(); }
  if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
  return cirsol(Index);
}

void Geom2dGcc_Circ2d2TanRad::
  WhichQualifier (const Standard_Integer Index   ,
	                GccEnt_Position& Qualif1 ,
	                GccEnt_Position& Qualif2) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
  else if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
  else {
    if (Invert) {
      Qualif1 = qualifier2(Index);
      Qualif2 = qualifier1(Index);
    }
    else {
      Qualif1 = qualifier1(Index);
      Qualif2 = qualifier2(Index);
    }
  }
}

void Geom2dGcc_Circ2d2TanRad::
  Tangency1 (const Standard_Integer Index,
	           Standard_Real&   ParSol,
	           Standard_Real&   ParArg,
	           gp_Pnt2d&        PntSol) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
  else if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
  else {
    if (Invert) {
      if (TheSame2(Index) == 0) {
	ParSol = par2sol(Index);
	ParArg = pararg2(Index);
        PntSol = pnttg2sol(Index);
      }
      else { throw StdFail_NotDone(); }
    }
    else {
      if (TheSame1(Index) == 0) {
	ParSol = par1sol(Index);
	ParArg = pararg1(Index);
        PntSol = pnttg1sol(Index);
      }
      else { throw StdFail_NotDone(); }
    }
  }
}

void Geom2dGcc_Circ2d2TanRad::
   Tangency2 (const Standard_Integer Index,
              Standard_Real& ParSol,
              Standard_Real& ParArg,
              gp_Pnt2d& PntSol) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
  else if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
  else {
    if (!Invert) {
      if (TheSame2(Index) == 0) {
	ParSol = par2sol(Index);
	ParArg = pararg2(Index);
        PntSol = pnttg2sol(Index);
      }
      else { throw StdFail_NotDone(); }
    }
    else {
      if (TheSame1(Index) == 0) {
	ParSol = par1sol(Index);
	ParArg = pararg1(Index);
        PntSol = pnttg1sol(Index);
      }
      else { throw StdFail_NotDone(); }
    }
  }
}

Standard_Boolean Geom2dGcc_Circ2d2TanRad::
   IsTheSame1 (const Standard_Integer Index) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
  if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
  if (Invert) {
    if (TheSame2(Index) == 0) { return Standard_False; }
    else { return Standard_True; }
  }
  else {
    if (TheSame1(Index) == 0) { return Standard_False; }
    else { return Standard_True; }
  }
}

Standard_Boolean Geom2dGcc_Circ2d2TanRad::
   IsTheSame2 (const Standard_Integer Index) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
  if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
  if (!Invert) {
    if (TheSame2(Index) == 0) { return Standard_False; }
    else { return Standard_True; }
  }
  else {
    if (TheSame1(Index) == 0) { return Standard_False; }
    else { return Standard_True; }
  }
//  return Standard_True;
}
