// Created on: 1992-01-02
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


#include <ElCLib.hxx>
#include <GccAna_Circ2d2TanOn.hxx>
#include <GccAna_Lin2dBisec.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <GccEnt_QualifiedLin.hxx>
#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

GccAna_Circ2d2TanOn::
   GccAna_Circ2d2TanOn (const GccEnt_QualifiedLin&  Qualified1 ,
                        const GccEnt_QualifiedLin&  Qualified2 ,
                        const gp_Circ2d&            OnCirc     ,
                        const Standard_Real                     ):
   cirsol(1,4)     ,
   qualifier1(1,4) ,
   qualifier2(1,4),
   TheSame1(1,4)   ,
   TheSame2(1,4)   ,
   pnttg1sol(1,4)  ,
   pnttg2sol(1,4)  ,
   pntcen(1,4)     ,
   par1sol(1,4)    ,
   par2sol(1,4)    ,
   pararg1(1,4)    ,
   pararg2(1,4)    ,
   parcen3(1,4)    
{
  TheSame1.Init(0);
  TheSame2.Init(0);
  WellDone = Standard_False;
  NbrSol = 0;
  
  gp_Dir2d dirx(1.,0.);
  if (!(Qualified1.IsEnclosed() ||
	Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
      !(Qualified2.IsEnclosed() ||
	Qualified2.IsOutside() || Qualified2.IsUnqualified())) {
    throw GccEnt_BadQualifier();
    return;
  }
  gp_Lin2d L1(Qualified1.Qualified());
  gp_Lin2d L2(Qualified2.Qualified());
  GccAna_Lin2dBisec Bis(L1,L2);
  Standard_Integer i=0,j=0;
  Standard_Integer nbsol = 0;
  Standard_Real sgn = 1.;
  Standard_Real s = 1.;
  Standard_Boolean ok = Standard_False;
  gp_Dir2d D1(L1.Direction());
  gp_Dir2d D2(L2.Direction());
  gp_Dir2d Dnor1(-D1.Y(),D1.X());
  gp_Dir2d Dnor2(-D2.Y(),D2.X());
  gp_XY XYnor1(-D1.Y(),D1.X());
  gp_XY XYnor2(-D2.Y(),D2.X());
  gp_Pnt2d originL1(L1.Location());
  gp_Pnt2d originL2(L2.Location());
  gp_XY Dloc(originL1.XY()-originL2.XY());
  if (D1.Angle(D2) <= gp::Resolution()) {
    if (Qualified1.IsEnclosed()) { 
      if (Dloc.Dot(XYnor1) <= 0.) { ok = Standard_True; }
      else { ok = Standard_False; }
    }
    else if (Qualified1.IsOutside()) { 
      if (Dloc.Dot(XYnor1) >= 0.) { ok = Standard_True; }
      else { ok = Standard_False; }
    }
    else {ok = Standard_True; }
    if (Qualified2.IsEnclosed()) { 
      if (Dloc.Dot(XYnor2) >= 0.) { ok = Standard_True; }
      else { ok = Standard_False; }
    }
    else if (Qualified2.IsOutside()) {
      if (Dloc.Dot(XYnor2) <= 0.) { ok = Standard_True; }
      else { ok = Standard_False; }
    }
    else {ok = Standard_True; }
    if ( ok ) {
      IntAna2d_AnaIntersection Intp(Bis.ThisSolution(1),OnCirc);
      if (Intp.IsDone()) {
	WellDone = Standard_True;
	if (!Intp.IsEmpty()) {
	  for (Standard_Integer l = 1 ; l <= Intp.NbPoints() ; l++) {	 
	    NbrSol++;
	    gp_Pnt2d pt(Intp.Point(l).Value());
	    gp_Ax2d axe(pt,dirx);
	    cirsol(NbrSol) = gp_Circ2d(axe,L1.Distance(pt));
//          ===============================================
	    gp_Dir2d dc1(originL1.XY()-pt.XY());
	    gp_Dir2d dc2(originL2.XY()-pt.XY());
	    if (!Qualified1.IsUnqualified()) { 
	      qualifier1(NbrSol) = Qualified1.Qualifier();
	    }
	    else if (dc1.Dot(Dnor1) > 0.0) {
	      qualifier1(NbrSol) = GccEnt_outside;
	    }
	    else { qualifier1(NbrSol) = GccEnt_enclosed; }
	    if (!Qualified2.IsUnqualified()) { 
	      qualifier2(NbrSol) = Qualified2.Qualifier();
	    }
	    else if (dc2.Dot(Dnor2) > 0.0) {
	      qualifier2(NbrSol) = GccEnt_outside;
	    }
	    else { qualifier2(NbrSol) = GccEnt_enclosed; }
	  }
	}
      }
    }
  }
  else if (Qualified1.IsEnclosed() && Qualified2.IsEnclosed()) {
//============================================================
    if (Bis.IsDone()) {
      if (Bis.NbSolutions() == 2) {
	nbsol = 1;
	i = 2;
	j = 1;
	sgn = -1.;
      }
    }
  }
  else if (Qualified1.IsEnclosed() && Qualified2.IsOutside()) {
//===========================================================
    if (Bis.IsDone()) {
      if (Bis.NbSolutions() >= 1) {
	nbsol = 1;
	i = 1;
	j = 1;
	if (D1.Angle(D2) >= 0.0) { sgn = -1.; }
      }
    }
  }
  else if (Qualified1.IsOutside() && Qualified2.IsEnclosed()) {
//===========================================================
    if (Bis.IsDone()) {
      if (Bis.NbSolutions() >= 1) {
	nbsol = 1;
	i = 1;
	j = 1;
	if (D1.Angle(D2) <= 0.0) { sgn = -1.; }
      }
    }
  }
  else if (Qualified1.IsOutside() && Qualified2.IsOutside()) {
//==========================================================
    if (Bis.IsDone()) {
      if (Bis.NbSolutions() >= 1) {
	nbsol = 1;
	i = 2;
	j = 1;
      }
    }
  }
  else if (Qualified1.IsUnqualified() && Qualified2.IsEnclosed()) {
//=============================================================
    if (Bis.IsDone()) {
      nbsol = 2;
      if (Bis.NbSolutions() >= 1) {
	i = 1;
	j = 2;
      }
      if (D1.Angle(D2) >= 0.0) { s = -1.; }
      else { sgn = -1.; }
    }
  }
  else if (Qualified1.IsUnqualified() && Qualified2.IsOutside()) {
//==============================================================
    if (Bis.IsDone()) {
      nbsol = 2;
      if (Bis.NbSolutions() >= 1) {
	i = 1;
	j = 2;
      }
      if (D1.Angle(D2) >= 0.0) {
	s = -1.;
	sgn = -1.;
      }
    }
  }
  else if (Qualified1.IsEnclosed() && Qualified2.IsUnqualified()) {
//===============================================================
    if (Bis.IsDone()) {
      nbsol = 2;
      if (Bis.NbSolutions() >= 1) {
	i = 1;
	j = 2;
      }
      if (D1.Angle(D2) >= 0.0) { sgn = -1.; }
      else { s = -1.; }
    }
  }
  else if (Qualified1.IsOutside() && Qualified2.IsUnqualified()) {
//==============================================================
    if (Bis.IsDone()) {
      nbsol = 2;
      if (Bis.NbSolutions() >= 1) {
	i = 1;
	j = 2;
      }
      if (D1.Angle(D2) <= 0.0) {
	s = -1.;
	sgn = -1.;
      }
    }
  }
  else if (Qualified1.IsUnqualified() && Qualified2.IsUnqualified()) {
//==================================================================
    nbsol = 4;
    i = 1;
    j = 2;
  }
  if (nbsol >= 1) {
    if (Bis.IsDone()) {
      Standard_Integer kk = 0;
      for (Standard_Integer k = i ; k <= i+j-1 ; k++) {
	kk++;
	IntAna2d_AnaIntersection Intp(Bis.ThisSolution(k),OnCirc);
	if (Intp.IsDone()) {
	  if (!Intp.IsEmpty()) {
	    for (Standard_Integer l = 1 ; l <= Intp.NbPoints() ; l++) {
	      gp_Vec2d V(Intp.Point(l).Value(),
			 Bis.ThisSolution(k).Location());
	      if ((kk==1 && sgn*V.Dot(Bis.ThisSolution(k).Direction())>=0.0)||
		  (kk==2 && sgn*s*V.Dot(Bis.ThisSolution(k).Direction())>=0.0)
		  || nbsol == 4) {
		NbrSol++;
		gp_Pnt2d pt(Intp.Point(i).Value());
		gp_Ax2d axe(pt,dirx);
		cirsol(NbrSol) = gp_Circ2d(axe,
//              ===============================
					   L1.Distance(Intp.Point(l).Value()));
//                                         ===================================
		gp_Dir2d dc1(originL1.XY()-pt.XY());
		gp_Dir2d dc2(originL2.XY()-pt.XY());
		if (!Qualified1.IsUnqualified()) { 
		  qualifier1(NbrSol) = Qualified1.Qualifier();
		}
		else if (dc1.Dot(Dnor1) > 0.0) {
		  qualifier1(NbrSol) = GccEnt_outside;
		}
		else { qualifier1(NbrSol) = GccEnt_enclosed; }
		if (!Qualified2.IsUnqualified()) { 
		  qualifier2(NbrSol) = Qualified2.Qualifier();
		}
		else if (dc2.Dot(Dnor2) > 0.0) {
		  qualifier2(NbrSol) = GccEnt_outside;
		}
		else { qualifier2(NbrSol) = GccEnt_enclosed; }
	      }
	    }
	  }
	  WellDone = Standard_True;
	}
      }
    }
  }
  if (NbrSol > 0) {
    for (i =1 ; i <= NbrSol ; i++) {
      gp_Pnt2d pbid(cirsol(i).Location());
      Standard_Real Radius = cirsol(i).Radius();
      gp_Dir2d dc2(originL1.XY()-pbid.XY());
      Standard_Real sign = dc2.Dot(gp_Dir2d(-L1.Direction().Y(),
					    L1.Direction().X()));
      dc2 = gp_Dir2d(sign*gp_XY(-L1.Direction().Y(),L1.Direction().X()));
      pnttg1sol(i) = gp_Pnt2d(pbid.XY()+Radius*dc2.XY());
      dc2 = gp_Dir2d(originL2.XY()-pbid.XY());
      sign = dc2.Dot(gp_Dir2d(-L2.Direction().Y(),L2.Direction().X()));
      dc2 = gp_Dir2d(sign*gp_XY(-L2.Direction().Y(),L2.Direction().X()));
      pnttg2sol(i) = gp_Pnt2d(pbid.XY()+Radius*dc2.XY());
      pntcen(i) = pbid;
      par1sol(i)=ElCLib::Parameter(cirsol(i),pnttg1sol(i));
      pararg1(i)=ElCLib::Parameter(L1,pnttg1sol(i));
      par2sol(i)=ElCLib::Parameter(cirsol(i),pnttg2sol(i));
      pararg2(i)=ElCLib::Parameter(L2,pnttg2sol(i));
      parcen3(i)=ElCLib::Parameter(OnCirc,pntcen(i));
    }
  }
}

