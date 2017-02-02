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


#include <GccAna_Circ2d3Tan.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <GccEnt_QualifiedLin.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Point.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dGcc_Circ2d3Tan.hxx>
#include <Geom2dGcc_Circ2d3TanIter.hxx>
#include <Geom2dGcc_QCurve.hxx>
#include <Geom2dGcc_QualifiedCurve.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

Geom2dGcc_Circ2d3Tan::
   Geom2dGcc_Circ2d3Tan (const Geom2dGcc_QualifiedCurve& Qualified1 ,
			 const Geom2dGcc_QualifiedCurve& Qualified2 ,
			 const Geom2dGcc_QualifiedCurve& Qualified3 ,
			 const Standard_Real             Tolerance  ,
			 const Standard_Real             Param1     ,
			 const Standard_Real             Param2     ,
			 const Standard_Real             Param3     ):
  cirsol(1,16)   ,
  qualifier1(1,16),
  qualifier2(1,16),
  qualifier3(1,16),
  TheSame1(1,16) ,
  TheSame2(1,16) ,
  TheSame3(1,16) ,
  pnttg1sol(1,16),
  pnttg2sol(1,16),
  pnttg3sol(1,16),
  par1sol(1,16)  ,
  par2sol(1,16)  ,
  par3sol(1,16)  ,
  pararg1(1,16)  ,
  pararg2(1,16)  ,
  pararg3(1,16)  
{
  Geom2dAdaptor_Curve C1 = Qualified1.Qualified();
  Geom2dAdaptor_Curve C2 = Qualified2.Qualified();
  Geom2dAdaptor_Curve C3 = Qualified3.Qualified();
  Handle(Geom2d_Curve) CC1 = C1.Curve();
  Handle(Geom2d_Curve) CC2 = C2.Curve();
  Handle(Geom2d_Curve) CC3 = C3.Curve();
  GeomAbs_CurveType Type1 = C1.GetType();
  GeomAbs_CurveType Type2 = C2.GetType();
  GeomAbs_CurveType Type3 = C3.GetType();

//=============================================================================
//                            Appel a GccAna.                                 +
//=============================================================================

  NbrSol = 0;
  if ((Type1 == GeomAbs_Line || Type1 == GeomAbs_Circle) &&
      (Type2 == GeomAbs_Line || Type2 == GeomAbs_Circle) &&
      (Type3 == GeomAbs_Line || Type3 == GeomAbs_Circle)) {
    if (Type1 == GeomAbs_Circle) {
      Handle(Geom2d_Circle) CCC1 = Handle(Geom2d_Circle)::DownCast(CC1);
      gp_Circ2d c1(CCC1->Circ2d());
      GccEnt_QualifiedCirc Qc1=GccEnt_QualifiedCirc(c1,Qualified1.Qualifier());
      if (Type2 == GeomAbs_Circle) {
	Handle(Geom2d_Circle) CCC2 = Handle(Geom2d_Circle)::DownCast(CC2);
	gp_Circ2d c2(CCC2->Circ2d());
	GccEnt_QualifiedCirc Qc2=GccEnt_QualifiedCirc(c2,
						      Qualified2.Qualifier());
	if (Type3 == GeomAbs_Circle) {
	  Handle(Geom2d_Circle) CCC3 = Handle(Geom2d_Circle)::DownCast(CC3);
	  gp_Circ2d c3(CCC3->Circ2d());
	  GccEnt_QualifiedCirc Qc3=GccEnt_QualifiedCirc(c3,
						       Qualified3.Qualifier());
	  GccAna_Circ2d3Tan Circ(Qc1,Qc2,Qc3,Tolerance);
	  WellDone = Circ.IsDone();
	  NbrSol = Circ.NbSolutions();
	  for(Standard_Integer i=1; i<=NbrSol; i++) {
	    Circ.WhichQualifier(i,qualifier1(i),
				   qualifier2(i),qualifier3(i));
	  }
	  Results(Circ,1,2,3);
	}
	else {
	  Handle(Geom2d_Line) LL3 = Handle(Geom2d_Line)::DownCast(CC3);
	  gp_Lin2d l3(LL3->Lin2d());
	  GccEnt_QualifiedLin Ql3=GccEnt_QualifiedLin(l3,
						      Qualified3.Qualifier());
	  GccAna_Circ2d3Tan Circ(Qc1,Qc2,Ql3,Tolerance);
	  WellDone = Circ.IsDone();
	  NbrSol = Circ.NbSolutions();
	  for(Standard_Integer i=1; i<=NbrSol; i++) {
	    Circ.WhichQualifier(i,qualifier1(i),
				   qualifier2(i),qualifier3(i));
	  }
	  Results(Circ,1,2,3);
	}
      }
      else {
	Handle(Geom2d_Line) LL2 = Handle(Geom2d_Line)::DownCast(CC2);
	gp_Lin2d l2(LL2->Lin2d());
	GccEnt_QualifiedLin Ql2(l2,Qualified2.Qualifier());
	if (Type3 == GeomAbs_Circle) {
	  Handle(Geom2d_Circle) CCC3 = Handle(Geom2d_Circle)::DownCast(CC3);
	  gp_Circ2d c3(CCC3->Circ2d());
	  GccEnt_QualifiedCirc Qc3(c3,Qualified3.Qualifier());
	  GccAna_Circ2d3Tan Circ(Qc1,Qc3,Ql2,Tolerance);
	  WellDone = Circ.IsDone();
	  NbrSol = Circ.NbSolutions();
	  for(Standard_Integer i=1; i<=NbrSol; i++) {
	    Circ.WhichQualifier(i,qualifier1(i),
				   qualifier3(i),qualifier2(i));
	  }
	  Results(Circ,1,3,2);
	}
	else {
	  Handle(Geom2d_Line) LL3 = Handle(Geom2d_Line)::DownCast(CC3);
	  gp_Lin2d l3(LL3->Lin2d());
	  GccEnt_QualifiedLin Ql3=GccEnt_QualifiedLin(l3,
						      Qualified3.Qualifier());
	  GccAna_Circ2d3Tan Circ(Qc1,Ql2,Ql3,Tolerance);
	  WellDone = Circ.IsDone();
	  NbrSol = Circ.NbSolutions();
	  for(Standard_Integer i=1; i<=NbrSol; i++) {
	    Circ.WhichQualifier(i,qualifier1(i),
				   qualifier2(i),qualifier3(i));
	  }
	  Results(Circ,1,2,3);
	}
      }
    }
    else {
      Handle(Geom2d_Line) LL1 = Handle(Geom2d_Line)::DownCast(CC1);
      gp_Lin2d l1(LL1->Lin2d());
      GccEnt_QualifiedLin Ql1=GccEnt_QualifiedLin(l1,Qualified1.Qualifier());
      if (Type2 == GeomAbs_Circle) {
	Handle(Geom2d_Circle) CCC2 = Handle(Geom2d_Circle)::DownCast(CC2);
	gp_Circ2d c2(CCC2->Circ2d());
	GccEnt_QualifiedCirc Qc2=GccEnt_QualifiedCirc(c2,
						      Qualified2.Qualifier());
	if (Type3 == GeomAbs_Circle) {
	  Handle(Geom2d_Circle) CCC3 = Handle(Geom2d_Circle)::DownCast(CC3);
	  gp_Circ2d c3(CCC3->Circ2d());
	  GccEnt_QualifiedCirc Qc3=GccEnt_QualifiedCirc(c3,
						       Qualified3.Qualifier());
	  GccAna_Circ2d3Tan Circ(Qc2,Qc3,Ql1,Tolerance);
	  WellDone = Circ.IsDone();
	  NbrSol = Circ.NbSolutions();
	  for(Standard_Integer i=1; i<=NbrSol; i++) {
	    Circ.WhichQualifier(i,qualifier3(i),
				   qualifier1(i),qualifier2(i));
	  }
	  Results(Circ,3,1,2);
	}
	else {
	  Handle(Geom2d_Line) LL3 = Handle(Geom2d_Line)::DownCast(CC3);
	  gp_Lin2d l3(LL3->Lin2d());
	  GccEnt_QualifiedLin Ql3=GccEnt_QualifiedLin(l3,
						      Qualified3.Qualifier());
	  GccAna_Circ2d3Tan Circ(Qc2,Ql1,Ql3,Tolerance);
	  WellDone = Circ.IsDone();
	  NbrSol = Circ.NbSolutions();
	  for(Standard_Integer i=1; i<=NbrSol; i++) {
	    Circ.WhichQualifier(i,qualifier2(i),
				   qualifier1(i),qualifier3(i));
	  }
	  Results(Circ,2,1,3);
	}
      }
      else {
	Handle(Geom2d_Line) LL2 = Handle(Geom2d_Line)::DownCast(CC2);
	gp_Lin2d l2(LL2->Lin2d());
	GccEnt_QualifiedLin Ql2=GccEnt_QualifiedLin(l2,Qualified2.Qualifier());
	if (Type3 == GeomAbs_Circle) {
	  Handle(Geom2d_Circle) CCC3 = Handle(Geom2d_Circle)::DownCast(CC3);
	  gp_Circ2d c3(CCC3->Circ2d());
	  GccEnt_QualifiedCirc Qc3=GccEnt_QualifiedCirc(c3,
						       Qualified3.Qualifier());
	  GccAna_Circ2d3Tan Circ(Qc3,Ql2,Ql1,Tolerance);
	  WellDone = Circ.IsDone();
	  NbrSol = Circ.NbSolutions();
	  for(Standard_Integer i=1; i<=NbrSol; i++) {
	    Circ.WhichQualifier(i,qualifier3(i),
				   qualifier2(i),qualifier1(i));
	  }
	  Results(Circ,3,2,1);
	}
	else {
	  Handle(Geom2d_Line) LL3 = Handle(Geom2d_Line)::DownCast(CC3);
	  gp_Lin2d l3(LL3->Lin2d());
	  GccEnt_QualifiedLin Ql3=GccEnt_QualifiedLin(l3,
						      Qualified3.Qualifier());
	  GccAna_Circ2d3Tan Circ(Ql1,Ql2,Ql3,Tolerance);
	  WellDone = Circ.IsDone();
	  NbrSol = Circ.NbSolutions();
	  for(Standard_Integer i=1; i<=NbrSol; i++) {
	    Circ.WhichQualifier(i,qualifier1(i),
				   qualifier2(i),qualifier3(i));
	  }
	  Results(Circ,1,2,3);
	}
      }
    }
  }
  else {
    Geom2dGcc_QCurve Qc1(C1,Qualified1.Qualifier());
    Geom2dGcc_QCurve Qc2(C2,Qualified2.Qualifier());
    Geom2dGcc_QCurve Qc3(C3,Qualified3.Qualifier());
    Geom2dGcc_Circ2d3TanIter Circ(Qc1,Qc2,Qc3,Param1,Param2,Param3,Tolerance);
    WellDone = Circ.IsDone();
    NbrSol = 1;
    if (WellDone) {
      cirsol(1)   = Circ.ThisSolution();
      if (Circ.IsTheSame1()) { TheSame1(1) = 1; }
      else {TheSame1(1) = 0; }
      if (Circ.IsTheSame2()) { TheSame2(1) = 1; }
      else {TheSame2(1) = 0; }
      if (Circ.IsTheSame3()) { TheSame3(1) = 1; }
      else {TheSame3(1) = 0; }
      Circ.Tangency1(par1sol(1),pararg1(1),pnttg1sol(1));
      Circ.Tangency2(par2sol(1),pararg2(1),pnttg2sol(1));
      Circ.Tangency3(par3sol(1),pararg3(1),pnttg3sol(1));
      Circ.WhichQualifier(qualifier1(1),qualifier2(1),qualifier3(1));
    }
  }
}

Geom2dGcc_Circ2d3Tan::
   Geom2dGcc_Circ2d3Tan (const Geom2dGcc_QualifiedCurve& Qualified1 ,
			 const Geom2dGcc_QualifiedCurve& Qualified2 ,
			 const Handle(Geom2d_Point)&     Point      ,
			 const Standard_Real             Tolerance  ,
			 const Standard_Real             Param1     ,
			 const Standard_Real             Param2     ):
  cirsol(1,16)   ,
  qualifier1(1,16),
  qualifier2(1,16),
  qualifier3(1,16),
  TheSame1(1,16) ,
  TheSame2(1,16) ,
  TheSame3(1,16) ,
  pnttg1sol(1,16),
  pnttg2sol(1,16),
  pnttg3sol(1,16),
  par1sol(1,16)  ,
  par2sol(1,16)  ,
  par3sol(1,16)  ,
  pararg1(1,16)  ,
  pararg2(1,16)  ,
  pararg3(1,16)  
{
  Geom2dAdaptor_Curve C1 = Qualified1.Qualified();
  Geom2dAdaptor_Curve C2 = Qualified2.Qualified();
  Handle(Geom2d_Curve) CC1 = C1.Curve();
  Handle(Geom2d_Curve) CC2 = C2.Curve();
  GeomAbs_CurveType Type1 = C1.GetType();
  GeomAbs_CurveType Type2 = C2.GetType();

//=============================================================================
//                            Appel a GccAna.                                 +
//=============================================================================

  NbrSol = 0;
  if ((Type1 == GeomAbs_Line || Type1 == GeomAbs_Circle) &&
      (Type2 == GeomAbs_Line || Type2 == GeomAbs_Circle)) {
    if (Type1 == GeomAbs_Circle) {
      Handle(Geom2d_Circle) CCC1 = Handle(Geom2d_Circle)::DownCast(CC1);
      gp_Circ2d c1(CCC1->Circ2d());
      GccEnt_QualifiedCirc Qc1(c1,Qualified1.Qualifier());
      if (Type2 == GeomAbs_Circle) {
	Handle(Geom2d_Circle) CCC2 = Handle(Geom2d_Circle)::DownCast(CC2);
	gp_Circ2d c2(CCC2->Circ2d());
	GccEnt_QualifiedCirc Qc2(c2,Qualified2.Qualifier());
	GccAna_Circ2d3Tan Circ(Qc1,Qc2,Point->Pnt2d(),Tolerance);
	WellDone = Circ.IsDone();
	NbrSol = Circ.NbSolutions();
	for(Standard_Integer i=1; i<=NbrSol; i++) {
	  Circ.WhichQualifier(i,qualifier1(i),qualifier2(i),qualifier3(i));
	}
	Results(Circ,1,2,3);
      }
      else {
	Handle(Geom2d_Line) LL2 = Handle(Geom2d_Line)::DownCast(CC2);
	gp_Lin2d l2(LL2->Lin2d());
	GccEnt_QualifiedLin Ql2(l2,Qualified2.Qualifier());
	GccAna_Circ2d3Tan Circ(Qc1,Ql2,Point->Pnt2d(),Tolerance);
	WellDone = Circ.IsDone();
	NbrSol = Circ.NbSolutions();
	for(Standard_Integer i=1; i<=NbrSol; i++) {
	  Circ.WhichQualifier(i,qualifier1(i),qualifier2(i),qualifier3(i));
	}
	Results(Circ,1,2,3);
      }
    }
    else {
      Handle(Geom2d_Line) LL1 = Handle(Geom2d_Line)::DownCast(CC1);
      gp_Lin2d l1(LL1->Lin2d());
      GccEnt_QualifiedLin Ql1(l1,Qualified1.Qualifier());
      if (Type2 == GeomAbs_Circle) {
	Handle(Geom2d_Circle) CCC2 = Handle(Geom2d_Circle)::DownCast(CC2);
	gp_Circ2d c2(CCC2->Circ2d());
	GccEnt_QualifiedCirc Qc2(c2,Qualified2.Qualifier());
	GccAna_Circ2d3Tan Circ(Qc2,Ql1,Point->Pnt2d(),Tolerance);
	WellDone = Circ.IsDone();
	NbrSol = Circ.NbSolutions();
	for(Standard_Integer i=1; i<=NbrSol; i++) {
	  Circ.WhichQualifier(i,qualifier2(i),qualifier1(i),qualifier3(i));
	}
	Results(Circ,2,1,3);
      }
      else {
	Handle(Geom2d_Line) LL2 = Handle(Geom2d_Line)::DownCast(CC2);
	gp_Lin2d l2(LL2->Lin2d());
	GccEnt_QualifiedLin Ql2(l2,Qualified2.Qualifier());
	GccAna_Circ2d3Tan Circ(Ql1,Ql2,Point->Pnt2d(),Tolerance);
	WellDone = Circ.IsDone();
	NbrSol = Circ.NbSolutions();
	for(Standard_Integer i=1; i<=NbrSol; i++) {
	  Circ.WhichQualifier(i,qualifier1(i),qualifier2(i),qualifier3(i));
	}
	Results(Circ,1,2,3);
      }
    }
  }
  else {
    Geom2dGcc_QCurve Qc1(C1,Qualified1.Qualifier());
    Geom2dGcc_QCurve Qc2(C2,Qualified2.Qualifier());
    Geom2dGcc_Circ2d3TanIter Circ(Qc1,Qc2,Point->Pnt2d(),Param1,Param2,Tolerance);
    WellDone = Circ.IsDone();
    NbrSol = 1;
    if (WellDone) {
      cirsol(1)   = Circ.ThisSolution();
      if (Circ.IsTheSame1()) { TheSame1(1) = 1; }
      else {TheSame1(1) = 0; }
      if (Circ.IsTheSame2()) { TheSame2(1) = 1; }
      else {TheSame2(1) = 0; }
      if (Circ.IsTheSame3()) { TheSame3(1) = 1; }
      else {TheSame3(1) = 0; }
      Circ.Tangency1(par1sol(1),pararg1(1),pnttg1sol(1));
      Circ.Tangency2(par2sol(1),pararg2(1),pnttg2sol(1));
      Circ.Tangency3(par3sol(1),pararg3(1),pnttg3sol(1));
      Circ.WhichQualifier(qualifier1(1),qualifier2(1),qualifier3(1));
    }
  }
}

Geom2dGcc_Circ2d3Tan::
   Geom2dGcc_Circ2d3Tan (const Geom2dGcc_QualifiedCurve& Qualified1 ,
			 const Handle(Geom2d_Point)&     Point1     ,
			 const Handle(Geom2d_Point)&     Point2     ,
			 const Standard_Real             Tolerance  ,
			 const Standard_Real             Param1     ):
  cirsol(1,16)   ,
  qualifier1(1,16),
  qualifier2(1,16),
  qualifier3(1,16),
  TheSame1(1,16) ,
  TheSame2(1,16) ,
  TheSame3(1,16) ,
  pnttg1sol(1,16),
  pnttg2sol(1,16),
  pnttg3sol(1,16),
  par1sol(1,16)  ,
  par2sol(1,16)  ,
  par3sol(1,16)  ,
  pararg1(1,16)  ,
  pararg2(1,16)  ,
  pararg3(1,16)  
{
  Geom2dAdaptor_Curve C1 = Qualified1.Qualified();
  Handle(Geom2d_Curve) CC1 = C1.Curve();
  GeomAbs_CurveType Type1 = C1.GetType();

//=============================================================================
//                            Appel a GccAna.                                 +
//=============================================================================

  NbrSol = 0;
  if ((Type1 == GeomAbs_Line || Type1 == GeomAbs_Circle)) {
    if (Type1 == GeomAbs_Circle) {
      Handle(Geom2d_Circle) CCC1 = Handle(Geom2d_Circle)::DownCast(CC1);
      gp_Circ2d c1(CCC1->Circ2d());
      GccEnt_QualifiedCirc Qc1(c1,Qualified1.Qualifier());
      GccAna_Circ2d3Tan Circ(Qc1,Point1->Pnt2d(),Point2->Pnt2d(),Tolerance);
      WellDone = Circ.IsDone();
      NbrSol = Circ.NbSolutions();
      for(Standard_Integer i=1; i<=NbrSol; i++) {
	Circ.WhichQualifier(i,qualifier1(i),qualifier2(i),qualifier3(i));
      }
      Results(Circ,1,2,3);
    }
    else {
      Handle(Geom2d_Line) LL1 = Handle(Geom2d_Line)::DownCast(CC1);
      gp_Lin2d l1(LL1->Lin2d());
      GccEnt_QualifiedLin Ql1(l1,Qualified1.Qualifier());
      GccAna_Circ2d3Tan Circ(Ql1,Point1->Pnt2d(),Point2->Pnt2d(),Tolerance);
      WellDone = Circ.IsDone();
      NbrSol = Circ.NbSolutions();
      for(Standard_Integer i=1; i<=NbrSol; i++) {
	Circ.WhichQualifier(i,qualifier1(i),qualifier2(i),qualifier3(i));
      }
      Results(Circ,1,2,3);
    }
  }
  else {
    Geom2dGcc_QCurve Qc1(C1,Qualified1.Qualifier());
    Geom2dGcc_Circ2d3TanIter Circ(Qc1,Point1->Pnt2d(),Point2->Pnt2d(),
			     Param1,Tolerance);
    WellDone = Circ.IsDone();
    NbrSol = 1;
    if (WellDone) {
      cirsol(1)   = Circ.ThisSolution();
      if (Circ.IsTheSame1()) { TheSame1(1) = 1; }
      else {TheSame1(1) = 0; }
      if (Circ.IsTheSame2()) { TheSame2(1) = 1; }
      else {TheSame2(1) = 0; }
      if (Circ.IsTheSame3()) { TheSame3(1) = 1; }
      else {TheSame3(1) = 0; }
      Circ.Tangency1(par1sol(1),pararg1(1),pnttg1sol(1));
      Circ.Tangency2(par2sol(1),pararg2(1),pnttg2sol(1));
      Circ.Tangency3(par3sol(1),pararg3(1),pnttg3sol(1));
      Circ.WhichQualifier(qualifier1(1),qualifier2(1),qualifier3(1));
    }
  }
}

Geom2dGcc_Circ2d3Tan::
   Geom2dGcc_Circ2d3Tan (const Handle(Geom2d_Point)&     Point1     ,
			 const Handle(Geom2d_Point)&     Point2     ,
			 const Handle(Geom2d_Point)&     Point3     ,
			 const Standard_Real             Tolerance  ):
  cirsol(1,2)   ,
  qualifier1(1,2),
  qualifier2(1,2),
  qualifier3(1,2),
  TheSame1(1,2) ,
  TheSame2(1,2) ,
  TheSame3(1,2) ,
  pnttg1sol(1,2),
  pnttg2sol(1,2),
  pnttg3sol(1,2),
  par1sol(1,2)  ,
  par2sol(1,2)  ,
  par3sol(1,2)  ,
  pararg1(1,2)  ,
  pararg2(1,2)  ,
  pararg3(1,2)  
{

//=============================================================================
//                            Appel a GccAna.                                 +
//=============================================================================

  NbrSol = 0;
  GccAna_Circ2d3Tan Circ(Point1->Pnt2d(),Point2->Pnt2d(),Point3->Pnt2d(),
			 Tolerance);
  WellDone = Circ.IsDone();
  NbrSol = Circ.NbSolutions();
  for(Standard_Integer i=1; i<=NbrSol; i++) {
    Circ.WhichQualifier(i,qualifier1(i),qualifier2(i),qualifier3(i));
  }
  Results(Circ,1,2,3);
}


void Geom2dGcc_Circ2d3Tan::Results(const GccAna_Circ2d3Tan& Circ ,
				   const Standard_Integer   Rank1,
				   const Standard_Integer   Rank2,
				   const Standard_Integer   Rank3)
{
  for (Standard_Integer j = 1; j <= NbrSol; j++) {
    cirsol(j)   = Circ.ThisSolution(j);
    Standard_Integer i1 = 0, i2 = 0, i3 = 0;
    if (Circ.IsTheSame1(j)) { i1 = 1; }
    if (Circ.IsTheSame2(j)) { i2 = 1; }
    if (Circ.IsTheSame3(j)) { i3 = 1; }    
    if (Rank1 == 1) { 
      TheSame1(j) = i1; 
      if ( i1 == 0)
	Circ.Tangency1(j,par1sol(j),pararg1(j),pnttg1sol(j));
    }
    else if (Rank1 == 2) { 
      TheSame1(j) = i2; 
      if ( i2 == 0)
	Circ.Tangency2(j,par1sol(j),pararg1(j),pnttg1sol(j));
    }
    else if (Rank1 == 3) { 
      TheSame1(j) = i3; 
      if ( i3 == 0)
	Circ.Tangency3(j,par1sol(j),pararg1(j),pnttg1sol(j));
    }
    if (Rank2 == 1) { 
      TheSame2(j) = i1; 
      if ( i1 == 0)
	Circ.Tangency1(j,par2sol(j),pararg2(j),pnttg2sol(j));
    }
    else if (Rank2 == 2) { 
      TheSame2(j) = i2; 
      if ( i2 == 0)
	Circ.Tangency2(j,par2sol(j),pararg2(j),pnttg2sol(j));
    }
    else if (Rank2 == 3) { 
      TheSame2(j) = i3; 
      if ( i3 == 0)
	Circ.Tangency3(j,par2sol(j),pararg2(j),pnttg2sol(j));
    }
    if (Rank3 == 1) { 
      TheSame3(j) = i1; 
      if ( i1 == 0)
	Circ.Tangency1(j,par3sol(j),pararg3(j),pnttg3sol(j));
    }
    else if (Rank3 == 2) { 
      TheSame3(j) = i2; 
      if ( i2 == 0)
	Circ.Tangency2(j,par3sol(j),pararg3(j),pnttg3sol(j));
    }
    else if (Rank3 == 3) { 
      TheSame3(j) = i3; 
      if ( i3 == 0)
	Circ.Tangency3(j,par3sol(j),pararg3(j),pnttg3sol(j));
    }
  }
}

Standard_Boolean Geom2dGcc_Circ2d3Tan::
   IsDone () const { return WellDone; }

Standard_Integer Geom2dGcc_Circ2d3Tan::
  NbSolutions () const 
{ 
  return (Standard_Integer ) NbrSol;
}

gp_Circ2d Geom2dGcc_Circ2d3Tan::
  ThisSolution (const Standard_Integer Index) const 
{
  if (!WellDone) { throw StdFail_NotDone(); }
  if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
  return cirsol(Index); 
}

void Geom2dGcc_Circ2d3Tan::
  WhichQualifier (const Standard_Integer Index   ,
	                GccEnt_Position& Qualif1 ,
	                GccEnt_Position& Qualif2 ,
	                GccEnt_Position& Qualif3) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
  else if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
  else {
    Qualif1 = qualifier1(Index);
    Qualif2 = qualifier2(Index);
    Qualif3 = qualifier3(Index);
  }
}

void Geom2dGcc_Circ2d3Tan::
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

void Geom2dGcc_Circ2d3Tan::
   Tangency2 (const Standard_Integer Index,
              Standard_Real& ParSol,
              Standard_Real& ParArg,
              gp_Pnt2d& PntSol) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
  else if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
  else {
    if (TheSame2(Index) == 0) {
      ParSol = par2sol(Index);
      ParArg = pararg2(Index);
      PntSol = pnttg2sol(Index);
    }
    else { throw StdFail_NotDone(); }
  }
}

void Geom2dGcc_Circ2d3Tan::
   Tangency3 (const Standard_Integer Index,
              Standard_Real& ParSol,
              Standard_Real& ParArg,
              gp_Pnt2d& PntSol) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
  else if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
  else {
    if (TheSame3(Index) == 0) {
      ParSol = par3sol(Index);
      ParArg = pararg3(Index);
      PntSol = pnttg3sol(Index);
    }
    else { throw StdFail_NotDone(); }
  }
}

Standard_Boolean Geom2dGcc_Circ2d3Tan::IsTheSame1 (const Standard_Integer Index) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
  if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
  if (TheSame1(Index) == 0) { return Standard_False; }
  return Standard_True; 
}

Standard_Boolean Geom2dGcc_Circ2d3Tan::
   IsTheSame2 (const Standard_Integer Index) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
  if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
  if (TheSame2(Index) == 0) { return Standard_False; }
  return Standard_True;
}

Standard_Boolean Geom2dGcc_Circ2d3Tan::
   IsTheSame3 (const Standard_Integer Index) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
  if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
  if (TheSame3(Index) == 0) { return Standard_False; }
  return Standard_True; 
}



