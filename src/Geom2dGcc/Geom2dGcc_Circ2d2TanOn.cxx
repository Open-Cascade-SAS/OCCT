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


#include <GccAna_Circ2d2TanOn.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <GccEnt_QualifiedLin.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Point.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dGcc_Circ2d2TanOn.hxx>
#include <Geom2dGcc_Circ2d2TanOnGeo.hxx>
#include <Geom2dGcc_Circ2d2TanOnIter.hxx>
#include <Geom2dGcc_QCurve.hxx>
#include <Geom2dGcc_QualifiedCurve.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

Geom2dGcc_Circ2d2TanOn::
   Geom2dGcc_Circ2d2TanOn (const Geom2dGcc_QualifiedCurve&    Qualified1 , 
			   const Geom2dGcc_QualifiedCurve&    Qualified2 , 
			   const Geom2dAdaptor_Curve&         OnCurve    ,
			   const Standard_Real                Tolerance  ,
			   const Standard_Real                Param1     ,
			   const Standard_Real                Param2     ,
			   const Standard_Real                ParamOn    ):
  cirsol(1,8)   ,
  qualifier1(1,8),
  qualifier2(1,8),
  TheSame1(1,8) ,
  TheSame2(1,8) ,
  pnttg1sol(1,8),
  pnttg2sol(1,8),
  pntcen(1,8)   ,
  par1sol(1,8)  ,
  par2sol(1,8)  ,
  pararg1(1,8)  ,
  pararg2(1,8)  ,
  parcen3(1,8)  
{
  Geom2dAdaptor_Curve C1 = Qualified1.Qualified();
  Geom2dAdaptor_Curve C2 = Qualified2.Qualified();
  GeomAbs_CurveType Type1 = C1.GetType();
  GeomAbs_CurveType Type2 = C2.GetType();
  GeomAbs_CurveType Type3 = OnCurve.GetType();
  Handle(Geom2d_Curve) CC1 = C1.Curve();
  Handle(Geom2d_Curve) CC2 = C2.Curve();
  Handle(Geom2d_Curve) Con = OnCurve.Curve();

//=============================================================================
//                            Appel a GccAna.                                 +
//=============================================================================

  Invert = Standard_False;
  NbrSol = 0;
  if ((Type1 == GeomAbs_Line || Type1 == GeomAbs_Circle) &&
      (Type2 == GeomAbs_Line || Type2 == GeomAbs_Circle)) {
    if (Type3 == GeomAbs_Line || Type3 == GeomAbs_Circle) {
      if (Type1 == GeomAbs_Circle) {
	Handle(Geom2d_Circle) CCC1 = Handle(Geom2d_Circle)::DownCast(CC1);
	gp_Circ2d c1(CCC1->Circ2d());
	GccEnt_QualifiedCirc Qc1 = 
	  GccEnt_QualifiedCirc(c1,Qualified1.Qualifier());
	if (Type2 == GeomAbs_Circle) {
	  Handle(Geom2d_Circle) CCC2 = Handle(Geom2d_Circle)::DownCast(CC2);
	  gp_Circ2d c2(CCC2->Circ2d());
	  if (Type3 == GeomAbs_Circle) {
	    Handle(Geom2d_Circle) CCon = Handle(Geom2d_Circle)::DownCast(Con);
	    GccAna_Circ2d2TanOn CircAna(Qc1,
			       GccEnt_QualifiedCirc(c2,Qualified2.Qualifier()),
					CCon->Circ2d(),Tolerance);
	    WellDone = CircAna.IsDone();
            if (WellDone)
            {
              NbrSol = CircAna.NbSolutions();
              for(Standard_Integer i=1; i<=NbrSol; i++) {
                CircAna.WhichQualifier(i,qualifier1(i),qualifier2(i));
              }
              Results(CircAna);
            }
	  }
	  else {
	    Handle(Geom2d_Line) LLon = Handle(Geom2d_Line)::DownCast(Con);
	    GccAna_Circ2d2TanOn CircAna(Qc1,
			       GccEnt_QualifiedCirc(c2,Qualified2.Qualifier()),
					LLon->Lin2d(),Tolerance);
	    WellDone = CircAna.IsDone();
            if (WellDone)
            {
              NbrSol = CircAna.NbSolutions();
              for(Standard_Integer i=1; i<=NbrSol; i++) {
                CircAna.WhichQualifier(i,qualifier1(i),qualifier2(i));
              }
              Results(CircAna);
            }
	  }
	}
	else {
	  Handle(Geom2d_Line) LL2 = Handle(Geom2d_Line)::DownCast(CC2);
	  gp_Lin2d l2(LL2->Lin2d());
	  if (Type3 == GeomAbs_Circle) {
	    Handle(Geom2d_Circle) CCon = Handle(Geom2d_Circle)::DownCast(Con);
	    GccAna_Circ2d2TanOn CircAna(Qc1,
			       GccEnt_QualifiedLin(l2,Qualified2.Qualifier()),
					CCon->Circ2d(),Tolerance);
	    WellDone = CircAna.IsDone();
            if (WellDone)
            {
              NbrSol = CircAna.NbSolutions();
              for(Standard_Integer i=1; i<=NbrSol; i++) {
                CircAna.WhichQualifier(i,qualifier1(i),qualifier2(i));
              }
              Results(CircAna);
            }
	  }
	  else {
	    Handle(Geom2d_Line) LLon = Handle(Geom2d_Line)::DownCast(Con);
	    GccAna_Circ2d2TanOn CircAna(Qc1,
			       GccEnt_QualifiedLin(l2,Qualified2.Qualifier()),
					LLon->Lin2d(),Tolerance);
	    WellDone = CircAna.IsDone();
            if (WellDone)
            {
              NbrSol = CircAna.NbSolutions();
              for(Standard_Integer i=1; i<=NbrSol; i++) {
                CircAna.WhichQualifier(i,qualifier1(i),qualifier2(i));
              }
              Results(CircAna);
            }
	  }
	}
      }
      else {
	Handle(Geom2d_Line) LL1 = Handle(Geom2d_Line)::DownCast(CC1);
	gp_Lin2d l1(LL1->Lin2d());
	GccEnt_QualifiedLin Ql1 = 
	  GccEnt_QualifiedLin(l1,Qualified1.Qualifier());
	if (Type2 == GeomAbs_Circle) {
	  Handle(Geom2d_Circle) CCC2 = Handle(Geom2d_Circle)::DownCast(CC2);
	  gp_Circ2d c2(CCC2->Circ2d());
	  if (Type3 == GeomAbs_Circle) {
	    Handle(Geom2d_Circle) CCon = Handle(Geom2d_Circle)::DownCast(Con);
	    GccAna_Circ2d2TanOn CircAna(GccEnt_QualifiedCirc(c2,
						       Qualified2.Qualifier()),
					Ql1,CCon->Circ2d(),Tolerance);
	    WellDone = CircAna.IsDone();
            if (WellDone)
            {
              NbrSol = CircAna.NbSolutions();
              for(Standard_Integer i=1; i<=NbrSol; i++) {
                CircAna.WhichQualifier(i,qualifier1(i),qualifier2(i));
              }
              Results(CircAna);
              Invert = Standard_True;
            }
	  }
	  else {
	    Handle(Geom2d_Line) LLon = Handle(Geom2d_Line)::DownCast(Con);
	    GccAna_Circ2d2TanOn CircAna(GccEnt_QualifiedCirc(c2,
						       Qualified2.Qualifier()),
					Ql1,LLon->Lin2d(),Tolerance);
	    WellDone = CircAna.IsDone();
            if (WellDone)
            {
              NbrSol = CircAna.NbSolutions();
              for(Standard_Integer i=1; i<=NbrSol; i++) {
                CircAna.WhichQualifier(i,qualifier1(i),qualifier2(i));
              }
              Results(CircAna);
              Invert = Standard_True;
            }
	  }
	}
	else {
	  Handle(Geom2d_Line) LL2 = Handle(Geom2d_Line)::DownCast(CC2);
	  gp_Lin2d l2(LL2->Lin2d());
	  if (Type3 == GeomAbs_Circle) {
	    Handle(Geom2d_Circle) CCon = Handle(Geom2d_Circle)::DownCast(Con);
	    GccAna_Circ2d2TanOn CircAna(Ql1,
			       GccEnt_QualifiedLin(l2,Qualified2.Qualifier()),
					CCon->Circ2d(),Tolerance);
	    WellDone = CircAna.IsDone();
            if (WellDone)
            {
              NbrSol = CircAna.NbSolutions();
              for(Standard_Integer i=1; i<=NbrSol; i++) {
                CircAna.WhichQualifier(i,qualifier1(i),qualifier2(i));
              }
              Results(CircAna);
            }
	  }
	  else {
	    Handle(Geom2d_Line) LLon = Handle(Geom2d_Line)::DownCast(Con);
	    GccAna_Circ2d2TanOn CircAna(Ql1,
			       GccEnt_QualifiedLin(l2,Qualified2.Qualifier()),
					LLon->Lin2d(),Tolerance);
	    WellDone = CircAna.IsDone();
            if (WellDone)
            {
              NbrSol = CircAna.NbSolutions();
              for(Standard_Integer i=1; i<=NbrSol; i++) {
                CircAna.WhichQualifier(i,qualifier1(i),qualifier2(i));
              }
              Results(CircAna);
            }
	  }
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
	GccEnt_QualifiedCirc Qc1 =
	  GccEnt_QualifiedCirc(c1,Qualified1.Qualifier());
	if (Type2 == GeomAbs_Circle) {
	  Handle(Geom2d_Circle) CCC2 = Handle(Geom2d_Circle)::DownCast(CC2);
	  gp_Circ2d c2(CCC2->Circ2d());
	  GccEnt_QualifiedCirc Qc2 =
	    GccEnt_QualifiedCirc(c2,Qualified2.Qualifier());
	  Geom2dGcc_Circ2d2TanOnGeo CircGeo(Qc1,Qc2,OnCurve,Tolerance);
	  WellDone = CircGeo.IsDone();
          if (WellDone)
          {
            NbrSol = CircGeo.NbSolutions();
            for(Standard_Integer i=1; i<=NbrSol; i++) {
              CircGeo.WhichQualifier(i,qualifier1(i),qualifier2(i));
            }
            Results(CircGeo);
          }
	}
	else {
	  Handle(Geom2d_Line) LL2 = Handle(Geom2d_Line)::DownCast(CC2);
	  gp_Lin2d l2(LL2->Lin2d());
	  GccEnt_QualifiedLin Ql2 =
	    GccEnt_QualifiedLin(l2,Qualified2.Qualifier());
	  Geom2dGcc_Circ2d2TanOnGeo CircGeo(Qc1,Ql2,OnCurve,Tolerance);
	  WellDone = CircGeo.IsDone();
          if (WellDone)
          {
            NbrSol = CircGeo.NbSolutions();
            for(Standard_Integer i=1; i<=NbrSol; i++) {
              CircGeo.WhichQualifier(i,qualifier1(i),qualifier2(i));
            }
            Results(CircGeo);
          }
	}
      }
      else {
	Handle(Geom2d_Line) LL1 = Handle(Geom2d_Line)::DownCast(CC1);
	gp_Lin2d l1(LL1->Lin2d());
	GccEnt_QualifiedLin Ql1 =
	  GccEnt_QualifiedLin(l1,Qualified1.Qualifier());
	if (Type2 == GeomAbs_Circle) {
	  Handle(Geom2d_Circle) CCC2 = Handle(Geom2d_Circle)::DownCast(CC2);
	  gp_Circ2d c2(CCC2->Circ2d());
	  GccEnt_QualifiedCirc Qc2 =
	    GccEnt_QualifiedCirc(c2,Qualified2.Qualifier());
	  Geom2dGcc_Circ2d2TanOnGeo CircGeo(Qc2,Ql1,OnCurve,Tolerance);
	  WellDone = CircGeo.IsDone();
          if (WellDone)
          {
            NbrSol = CircGeo.NbSolutions();
            for(Standard_Integer i=1; i<=NbrSol; i++) {
              CircGeo.WhichQualifier(i,qualifier1(i),qualifier2(i));
            }
            Results(CircGeo);
            Invert = Standard_True;
          }
	}
	else {
	  Handle(Geom2d_Line) LL2 = Handle(Geom2d_Line)::DownCast(CC2);
	  gp_Lin2d l2(LL2->Lin2d());
	  GccEnt_QualifiedLin Ql2 =
	    GccEnt_QualifiedLin(l2,Qualified2.Qualifier());
	  Geom2dGcc_Circ2d2TanOnGeo CircGeo(Ql1,Ql2,OnCurve,Tolerance);
	  WellDone = CircGeo.IsDone();
          if (WellDone)
          {
            NbrSol = CircGeo.NbSolutions();
            for(Standard_Integer i=1; i<=NbrSol; i++) {
              CircGeo.WhichQualifier(i,qualifier1(i),qualifier2(i));
            }
            Results(CircGeo);
          }
	}
      }
    }
  }
  else {
    Geom2dGcc_QCurve Qc1(C1,Qualified1.Qualifier());
    Geom2dGcc_QCurve Qc2(C2,Qualified2.Qualifier());
    if ((Type3 == GeomAbs_Circle || Type3 == GeomAbs_Line)) {
      if (Type3 == GeomAbs_Circle) {
	Handle(Geom2d_Circle) CCon = Handle(Geom2d_Circle)::DownCast(Con);
	Geom2dGcc_Circ2d2TanOnIter Circ(Qc1,Qc2,CCon->Circ2d(),
				   Param1,Param2,ParamOn,Tolerance);
	WellDone = Circ.IsDone();
        if (WellDone)
        {
          NbrSol = 1;
          cirsol(1)   = Circ.ThisSolution();
          if (Circ.IsTheSame1()) { TheSame1(1) = 1; }
          else {TheSame1(1) = 0; }
          if (Circ.IsTheSame2()) { TheSame2(1) = 1; }
          else {TheSame2(1) = 0; }
          Circ.Tangency1(par1sol(1),pararg1(1),pnttg1sol(1));
          Circ.Tangency2(par2sol(1),pararg2(1),pnttg2sol(1));
        }
      }
      else {
	Handle(Geom2d_Line) LLon = Handle(Geom2d_Line)::DownCast(Con);
	Geom2dGcc_Circ2d2TanOnIter Circ(Qc1,Qc2,LLon->Lin2d(),
				       Param1,Param2,ParamOn,Tolerance);
	WellDone = Circ.IsDone();
        if (WellDone)
        {
          NbrSol = 1;
          cirsol(1)   = Circ.ThisSolution();
          if (Circ.IsTheSame1()) { TheSame1(1) = 1; }
          else {TheSame1(1) = 0; }
          if (Circ.IsTheSame2()) { TheSame2(1) = 1; }
          else {TheSame2(1) = 0; }
          Circ.WhichQualifier(qualifier1(1),qualifier2(1));
          Circ.Tangency1(par1sol(1),pararg1(1),pnttg1sol(1));
          Circ.Tangency2(par2sol(1),pararg2(1),pnttg2sol(1));
        }
      }
    }
    Geom2dGcc_Circ2d2TanOnIter Circ(Qc1,Qc2,OnCurve,
				   Param1,Param2,ParamOn,Tolerance);
    WellDone = Circ.IsDone();
    if (WellDone)
    {
      NbrSol = 1;
      cirsol(1)   = Circ.ThisSolution();
      if (Circ.IsTheSame1()) { TheSame1(1) = 1; }
      else {TheSame1(1) = 0; }
      if (Circ.IsTheSame2()) { TheSame2(1) = 1; }
      else {TheSame2(1) = 0; }
      Circ.WhichQualifier(qualifier1(1),qualifier2(1));
      Circ.Tangency1(par1sol(1),pararg1(1),pnttg1sol(1));
      Circ.Tangency2(par2sol(1),pararg2(1),pnttg2sol(1));
    }
  }
}

Geom2dGcc_Circ2d2TanOn::
   Geom2dGcc_Circ2d2TanOn (const Geom2dGcc_QualifiedCurve&    Qualified1 , 
			   const Handle(Geom2d_Point)&        Point      , 
			   const Geom2dAdaptor_Curve&         OnCurve    ,
			   const Standard_Real                Tolerance  ,
			   const Standard_Real                Param1     ,
			   const Standard_Real                ParamOn    ):
  cirsol(1,8)   ,
  qualifier1(1,8),
  qualifier2(1,8),
  TheSame1(1,8) ,
  TheSame2(1,8) ,
  pnttg1sol(1,8),
  pnttg2sol(1,8),
  pntcen(1,8)   ,
  par1sol(1,8)  ,
  par2sol(1,8)  ,
  pararg1(1,8)  ,
  pararg2(1,8)  ,
  parcen3(1,8)  
{
  Geom2dAdaptor_Curve C1 = Qualified1.Qualified();
  GeomAbs_CurveType Type1 = C1.GetType();
  GeomAbs_CurveType Type3 = OnCurve.GetType();
  Handle(Geom2d_Curve) CC1 = C1.Curve();
  Handle(Geom2d_Curve) Con = OnCurve.Curve();

//=============================================================================
//                            Appel a GccAna.                                 +
//=============================================================================

  Invert = Standard_False;
  NbrSol = 0;
  if (Type1 == GeomAbs_Line || Type1 == GeomAbs_Circle) {
    if (Type3 == GeomAbs_Line || Type3 == GeomAbs_Circle) {
      gp_Pnt2d pnt(Point->Pnt2d());
      if (Type1 == GeomAbs_Circle) {
	Handle(Geom2d_Circle) CCC1 = Handle(Geom2d_Circle)::DownCast(CC1);
	gp_Circ2d c1(CCC1->Circ2d());
	GccEnt_QualifiedCirc Qc1(c1,Qualified1.Qualifier());
	if (Type3 == GeomAbs_Circle) {
	  Handle(Geom2d_Circle) CCon = Handle(Geom2d_Circle)::DownCast(Con);
	  GccAna_Circ2d2TanOn CircAna(Qc1,pnt,CCon->Circ2d(),Tolerance);
	  WellDone = CircAna.IsDone();
          if (WellDone)
          {
            NbrSol = CircAna.NbSolutions();
            for(Standard_Integer i=1; i<=NbrSol; i++) {
              CircAna.WhichQualifier(i,qualifier1(i),qualifier2(i));
            }
            Results(CircAna);
          }
	}
	else if (Type3 == GeomAbs_Line) {
	  Handle(Geom2d_Line) CCon = Handle(Geom2d_Line)::DownCast(Con);
	  GccAna_Circ2d2TanOn CircAna(Qc1,pnt,CCon->Lin2d(),Tolerance);
	  WellDone = CircAna.IsDone();
          if (WellDone)
          {
            NbrSol = CircAna.NbSolutions();
            for(Standard_Integer i=1; i<=NbrSol; i++) {
              CircAna.WhichQualifier(i,qualifier1(i),qualifier2(i));
            }
            Results(CircAna);
          }
	}
      }
      else {
	Handle(Geom2d_Line) LLL1 = Handle(Geom2d_Line)::DownCast(CC1);
	gp_Lin2d l1(LLL1->Lin2d());
	GccEnt_QualifiedLin Ql1(l1,Qualified1.Qualifier());
	if (Type3 == GeomAbs_Circle) {
	  Handle(Geom2d_Circle) CCon = Handle(Geom2d_Circle)::DownCast(Con);
	  GccAna_Circ2d2TanOn CircAna(Ql1,pnt,CCon->Circ2d(),Tolerance);
	  WellDone = CircAna.IsDone();
          if (WellDone)
          {
            NbrSol = CircAna.NbSolutions();
            for(Standard_Integer i=1; i<=NbrSol; i++) {
              CircAna.WhichQualifier(i,qualifier1(i),qualifier2(i));
            }
            Results(CircAna);
          }
	}
	else if (Type3 == GeomAbs_Line) {
	  Handle(Geom2d_Line) LLon = Handle(Geom2d_Line)::DownCast(Con);
	  GccAna_Circ2d2TanOn CircAna(Ql1,pnt,LLon->Lin2d(),Tolerance);
	  WellDone = CircAna.IsDone();
          if (WellDone)
          {
            NbrSol = CircAna.NbSolutions();
            for(Standard_Integer i=1; i<=NbrSol; i++) {
              CircAna.WhichQualifier(i,qualifier1(i),qualifier2(i));
            }
            Results(CircAna);
          }
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
	GccEnt_QualifiedCirc Qc1(c1,Qualified1.Qualifier());
	Geom2dGcc_Circ2d2TanOnGeo CircGeo(Qc1,Point->Pnt2d(),OnCurve,Tolerance);
	WellDone = CircGeo.IsDone();
        if (WellDone)
        {
          NbrSol = CircGeo.NbSolutions();
          for(Standard_Integer i=1; i<=NbrSol; i++) {
            CircGeo.WhichQualifier(i,qualifier1(i),qualifier2(i));
          }
          Results(CircGeo);
        }
      }
      else {
	Handle(Geom2d_Line) LLL1 = Handle(Geom2d_Line)::DownCast(CC1);
	gp_Lin2d l1(LLL1->Lin2d());
	GccEnt_QualifiedLin Ql1(l1,Qualified1.Qualifier());
	Geom2dGcc_Circ2d2TanOnGeo CircGeo(Ql1,Point->Pnt2d(),OnCurve,Tolerance);
	WellDone = CircGeo.IsDone();
        if (WellDone)
        {
          NbrSol = CircGeo.NbSolutions();
          for(Standard_Integer i=1; i<=NbrSol; i++) {
            CircGeo.WhichQualifier(i,qualifier1(i),qualifier2(i));
          }
          Results(CircGeo);
        }
      }
    }
  }                                   
  else {
    Geom2dGcc_QCurve Qc1(C1,Qualified1.Qualifier());
    if ((Type3 == GeomAbs_Circle || Type3 == GeomAbs_Line)) {
      if (Type3 == GeomAbs_Circle) {
	Handle(Geom2d_Circle) CCon = Handle(Geom2d_Circle)::DownCast(Con);
	Geom2dGcc_Circ2d2TanOnIter Circ(Qc1,Point->Pnt2d(),CCon->Circ2d(),
				   Param1,ParamOn,Tolerance);
	WellDone = Circ.IsDone();
        if (WellDone)
        {
          NbrSol = 1;
          cirsol(1)   = Circ.ThisSolution();
          if (Circ.IsTheSame1()) { TheSame1(1) = 1; }
          else {TheSame1(1) = 0; }
          Circ.WhichQualifier(qualifier1(1),qualifier2(1));
          Circ.Tangency1(par1sol(1),pararg1(1),pnttg1sol(1));
          Circ.Tangency2(par2sol(1),pararg2(1),pnttg2sol(1));
        }
      }
      else {
	Handle(Geom2d_Line) LLon = Handle(Geom2d_Line)::DownCast(Con);
	Geom2dGcc_Circ2d2TanOnIter Circ(Qc1,Point->Pnt2d(),LLon->Lin2d(),
				       Param1,ParamOn,Tolerance);
	WellDone = Circ.IsDone();
        if (WellDone)
        {
          NbrSol = 1;
          cirsol(1)   = Circ.ThisSolution();
          if (Circ.IsTheSame1()) { TheSame1(1) = 1; }
          else {TheSame1(1) = 0; }
          Circ.WhichQualifier(qualifier1(1),qualifier2(1));
          Circ.Tangency1(par1sol(1),pararg1(1),pnttg1sol(1));
          Circ.Tangency2(par2sol(1),pararg2(1),pnttg2sol(1));
        }
      }
    }
    else {
      Geom2dGcc_Circ2d2TanOnIter Circ(Qc1,Point->Pnt2d(),OnCurve,
				 Param1,ParamOn,Tolerance);
      WellDone = Circ.IsDone();
      if (WellDone)
      {
        NbrSol = 1;
        cirsol(1)   = Circ.ThisSolution();
        if (Circ.IsTheSame1()) { TheSame1(1) = 1; }
        else {TheSame1(1) = 0; }
        if (Circ.IsTheSame2()) { TheSame2(1) = 1; }
        else {TheSame2(1) = 0; }
        Circ.WhichQualifier(qualifier1(1),qualifier2(1));
        Circ.Tangency1(par1sol(1),pararg1(1),pnttg1sol(1));
        Circ.Tangency2(par2sol(1),pararg2(1),pnttg2sol(1));
      }
    }
  }
}

Geom2dGcc_Circ2d2TanOn::
   Geom2dGcc_Circ2d2TanOn (const Handle(Geom2d_Point)&        Point1     , 
			   const Handle(Geom2d_Point)&        Point2     , 
			   const Geom2dAdaptor_Curve&         OnCurve    ,
			   const Standard_Real                Tolerance  ):
  cirsol(1,8)   ,
  qualifier1(1,8),
  qualifier2(1,8),
  TheSame1(1,8) ,
  TheSame2(1,8) ,
  pnttg1sol(1,8),
  pnttg2sol(1,8),
  pntcen(1,8)   ,
  par1sol(1,8)  ,
  par2sol(1,8)  ,
  pararg1(1,8)  ,
  pararg2(1,8)  ,
  parcen3(1,8)  

{
  GeomAbs_CurveType Type3 = OnCurve.GetType();
  Handle(Geom2d_Curve) Con = OnCurve.Curve();

//=============================================================================
//                            Appel a GccAna.                                 +
//=============================================================================

  Invert = Standard_False;
  NbrSol = 0;
  if (Type3 == GeomAbs_Line || Type3 == GeomAbs_Circle) {
    gp_Pnt2d pnt1(Point1->Pnt2d());
    gp_Pnt2d pnt2(Point2->Pnt2d());
    if (Type3 == GeomAbs_Circle) {
      Handle(Geom2d_Circle) CCon = Handle(Geom2d_Circle)::DownCast(Con);
      GccAna_Circ2d2TanOn CircAna(pnt1,pnt2,CCon->Circ2d(),Tolerance);
      WellDone = CircAna.IsDone();
      if (WellDone)
      {
        NbrSol = CircAna.NbSolutions();
        for(Standard_Integer i=1; i<=NbrSol; i++) {
          CircAna.WhichQualifier(i,qualifier1(i),qualifier2(i));
        }
        Results(CircAna);
      }
    }
    else {
      Handle(Geom2d_Line) LLon = Handle(Geom2d_Line)::DownCast(Con);
      GccAna_Circ2d2TanOn CircAna(pnt1,pnt2,LLon->Lin2d(),Tolerance);
      WellDone = CircAna.IsDone();
      if (WellDone)
      {
        NbrSol = CircAna.NbSolutions();
        for(Standard_Integer i=1; i<=NbrSol; i++) {
          CircAna.WhichQualifier(i,qualifier1(i),qualifier2(i));
        }
        Results(CircAna);
      }
    }
  }

//=============================================================================
//                            Appel a GccGeo.                                 +
//=============================================================================

  else {
    Geom2dGcc_Circ2d2TanOnGeo CircGeo(Point1->Pnt2d(),Point2->Pnt2d(),
				     OnCurve,Tolerance);
    WellDone = CircGeo.IsDone();
    if (WellDone)
    {
      NbrSol = CircGeo.NbSolutions();
      for(Standard_Integer i=1; i<=NbrSol; i++) {
        CircGeo.WhichQualifier(i,qualifier1(i),qualifier2(i));
      }
      Results(CircGeo);
    }
  }
}

void Geom2dGcc_Circ2d2TanOn::Results(const GccAna_Circ2d2TanOn& Circ)
{
  for (Standard_Integer j = 1; j <= NbrSol; j++) {
    cirsol(j)   = Circ.ThisSolution(j);
    if (Circ.IsTheSame1(j)) { TheSame1(j) = 1; }
    else {TheSame1(j) = 0; }
    if (Circ.IsTheSame2(j)) { TheSame2(j) = 1; }
    else {TheSame2(j) = 0; }
    Circ.WhichQualifier(j,qualifier1(j),qualifier2(j));
    Circ.Tangency1(j,par1sol(j),pararg1(j),pnttg1sol(j));
    Circ.Tangency2(j,par2sol(j),pararg2(j),pnttg2sol(j));
    Circ.CenterOn3(j,parcen3(j),pntcen(j));
  }
}

void Geom2dGcc_Circ2d2TanOn::Results(const Geom2dGcc_Circ2d2TanOnGeo& Circ)
{
  for (Standard_Integer j = 1; j <= NbrSol; j++) {
    cirsol(j)   = Circ.ThisSolution(j);
    if (Circ.IsTheSame1(j)) { TheSame1(j) = 1; }
    else {TheSame1(j) = 0; }
    if (Circ.IsTheSame2(j)) { TheSame2(j) = 1; }
    else {TheSame2(j) = 0; }
    Circ.WhichQualifier(j,qualifier1(j),qualifier2(j));
    Circ.Tangency1(j,par1sol(j),pararg1(j),pnttg1sol(j));
    Circ.Tangency2(j,par2sol(j),pararg2(j),pnttg2sol(j));
    Circ.CenterOn3(j,parcen3(j),pntcen(j));
  }
}

Standard_Boolean Geom2dGcc_Circ2d2TanOn::
   IsDone () const { return WellDone; }

Standard_Integer Geom2dGcc_Circ2d2TanOn::
  NbSolutions () const 
{ 
  return NbrSol;
}

gp_Circ2d Geom2dGcc_Circ2d2TanOn::
  ThisSolution (const Standard_Integer Index) const 
{
  if (!WellDone) { throw StdFail_NotDone(); }
  if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
  return cirsol(Index);
}

void Geom2dGcc_Circ2d2TanOn::
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

void Geom2dGcc_Circ2d2TanOn::
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

void Geom2dGcc_Circ2d2TanOn::
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

void Geom2dGcc_Circ2d2TanOn::
   CenterOn3 (const Standard_Integer Index,
              Standard_Real& ParArg,
              gp_Pnt2d& PntSol) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
  else if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
  else {
    ParArg = parcen3(Index);
    PntSol = pntcen(Index);
  }
}

Standard_Boolean Geom2dGcc_Circ2d2TanOn::
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

Standard_Boolean Geom2dGcc_Circ2d2TanOn::
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
