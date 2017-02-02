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
#include <GccAna_CircLin2dBisec.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <GccEnt_QualifiedLin.hxx>
#include <GccInt_BCirc.hxx>
#include <GccInt_IType.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_Conic.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <TColStd_Array1OfReal.hxx>

GccAna_Circ2d2TanOn::
   GccAna_Circ2d2TanOn (const GccEnt_QualifiedCirc& Qualified1 ,
                        const GccEnt_QualifiedLin&  Qualified2 ,
                        const gp_Circ2d&            OnCirc     ,
                        const Standard_Real         Tolerance  ):
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
  if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
	Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
      !(Qualified2.IsEnclosed() ||
	Qualified2.IsOutside() || Qualified2.IsUnqualified())) {
    throw GccEnt_BadQualifier();
    return;
  }
  Standard_Real Radius=0;
  gp_Dir2d dirx(1.,0.);
  gp_Circ2d C1 = Qualified1.Qualified();
  gp_Lin2d L2 = Qualified2.Qualified();
  Standard_Real R1 = C1.Radius();
  gp_Pnt2d center1(C1.Location());
  gp_Pnt2d origin2(L2.Location());
  gp_Dir2d dirL2(L2.Direction());
  gp_Dir2d normL2(-dirL2.Y(),dirL2.X());

//=========================================================================
//   Processing of boundary cases.                                          +
//=========================================================================

  Standard_Real Tol = Abs(Tolerance);
  TColStd_Array1OfReal Rradius(1,2);
  Standard_Integer nbsol1 = 1;
//  Standard_Integer nbsol2 = 0;
  Standard_Real Ron = OnCirc.Radius();
  Standard_Real distcco = OnCirc.Location().Distance(center1);
  gp_Dir2d dircc(OnCirc.Location().XY()-center1.XY());
  gp_Pnt2d pinterm(center1.XY()+(distcco-Ron)*dircc.XY());
  Standard_Real distpl2 =L2.Distance(pinterm);
  Standard_Real distcc1 =pinterm.Distance(center1);
  Standard_Real d1 = Abs(distpl2-Abs(distcc1-R1));
  Standard_Real d2 = Abs(distpl2-(distcc1+R1));
  if ( d1 > Tol || d2 > Tol ) {
    pinterm = gp_Pnt2d(center1.XY()+(distcco-Ron)*dircc.XY());
    if ( d1 > Tol || d2 > Tol ) {
      nbsol1 = 0;
    }
  }
  if (nbsol1 > 0) {
    if (Qualified1.IsEnclosed() || Qualified1.IsOutside()) {
      nbsol1 = 1;
      Rradius(1) = Abs(distcc1-R1);
    }
    else if (Qualified1.IsEnclosing()) {
      nbsol1 = 1;
      Rradius(1) = R1+distcc1;
    }
    else if (Qualified1.IsUnqualified()) {
      nbsol1 = 2;
      Rradius(1) = Abs(distcc1-R1);
      Rradius(2) = R1+distcc1;
    }
    gp_Dir2d dirbid(origin2.XY()-pinterm.XY());
    gp_Dir2d normal(-dirL2.Y(),dirL2.X());
    if (Qualified1.IsEnclosed() && dirbid.Dot(normal) < 0.) {
      nbsol1 = 0;
    }
    else if (Qualified1.IsOutside() && dirbid.Dot(normal) < 0.) {
      nbsol1 = 0;
    }
    for (Standard_Integer i = 1 ; i <= nbsol1 ; i++) {
      if (Abs(Rradius(i)-distpl2) <= Tol) {
	WellDone = Standard_True;
	NbrSol++;
	cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(pinterm,dirx),Rradius(i));
//      ===========================================================
	gp_Dir2d dc1(center1.XY()-pinterm.XY());
	gp_Dir2d dc2(origin2.XY()-pinterm.XY());
	distcc1 = pinterm.Distance(center1);
	if (!Qualified1.IsUnqualified()) { 
	  qualifier1(NbrSol) = Qualified1.Qualifier();
	}
	else if (Abs(distcc1+Rradius(i)-R1) < Tol) {
	  qualifier1(NbrSol) = GccEnt_enclosed;
	}
	else if (Abs(distcc1-R1-Rradius(i)) < Tol) {
	  qualifier1(NbrSol) = GccEnt_outside;
	}
	else { qualifier1(NbrSol) = GccEnt_enclosing; }
	if (!Qualified2.IsUnqualified()) { 
	  qualifier2(NbrSol) = Qualified2.Qualifier();
	}
	else if (dc2.Dot(normL2) > 0.0) {
	  qualifier2(NbrSol) = GccEnt_outside;
	}
	else { qualifier2(NbrSol) = GccEnt_enclosed; }

	Standard_Real sign = dc2.Dot(gp_Dir2d(-dirL2.Y(),dirL2.X()));
	dc2 = gp_Dir2d(sign*gp_XY(-dirL2.Y(),dirL2.X()));
	pnttg1sol(NbrSol) = gp_Pnt2d(pinterm.XY()+Rradius(i)*dc1.XY());
	pnttg2sol(NbrSol) = gp_Pnt2d(pinterm.XY()+Rradius(i)*dc2.XY());
	par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),pnttg1sol(NbrSol));
	pararg1(NbrSol)=ElCLib::Parameter(C1,pnttg1sol(NbrSol));
	par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),pnttg2sol(NbrSol));
	pararg2(NbrSol)=ElCLib::Parameter(L2,pnttg2sol(NbrSol));
	parcen3(NbrSol)=ElCLib::Parameter(OnCirc,pntcen(NbrSol));
      }
    }
    if (WellDone) { return; }
  }

//=========================================================================
//   General case.                                                         +
//=========================================================================

  GccAna_CircLin2dBisec Bis(C1,L2);
  if (Bis.IsDone()) {
    Standard_Integer nbsolution = Bis.NbSolutions();
    for (Standard_Integer i = 1 ; i <=  nbsolution; i++) {
      Handle(GccInt_Bisec) Sol = Bis.ThisSolution(i);
      GccInt_IType type = Sol->ArcType();
      IntAna2d_AnaIntersection Intp;
      if (type == GccInt_Lin) {
	Intp.Perform(Sol->Line(),OnCirc);
      }
      else if (type == GccInt_Par) {
	Intp.Perform(OnCirc,IntAna2d_Conic(Sol->Parabola()));
      }
      if (Intp.IsDone()) {
	if ((!Intp.IsEmpty())&&(!Intp.ParallelElements())&&
	    (!Intp.IdenticalElements())) {
	  for (Standard_Integer j = 1 ; j <= Intp.NbPoints() ; j++) {
	    gp_Pnt2d Center(Intp.Point(j).Value());
	    Standard_Real dist1 = Center.Distance(center1);
	    Standard_Real dist2 = L2.Distance(Center);
//	    Standard_Integer nbsol = 1;
	    Standard_Boolean ok = Standard_False;
	    if (Qualified1.IsEnclosed()) {
	      if (dist1-R1 < Tolerance) {
		if (Abs(Abs(R1-dist1)-dist2)<Tolerance) { ok=Standard_True; }
	      }
	    }
	    else if (Qualified1.IsOutside()) {
	      if (R1-dist1 < Tolerance) {
		if (Abs(Abs(R1-dist1)-dist2)<Tolerance) { ok=Standard_True; }
	      }
	    }
	    else if (Qualified1.IsEnclosing() || Qualified1.IsUnqualified()) {
	      ok = Standard_True;
	    }
	    if (Qualified2.IsEnclosed() && ok) {
	      if ((((origin2.X()-Center.X())*(-dirL2.Y()))+
		   ((origin2.Y()-Center.Y())*(dirL2.X())))<=0){
		ok = Standard_True;
		Radius = dist2;
	      }
	    }
	    else if (Qualified2.IsOutside() && ok) {
	      if ((((origin2.X()-Center.X())*(-dirL2.Y()))+
		   ((origin2.Y()-Center.Y())*(dirL2.X())))>=0){
		ok = Standard_True;
		Radius = dist2;
	      }
	    }
	    else if (Qualified2.IsUnqualified() && ok) {
	      ok = Standard_True;
	      Radius = dist2;
	    }
	    if (ok) {
	      NbrSol++;
	      cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius);
//            =======================================================
	      gp_Dir2d dc2(origin2.XY()-Center.XY());
	      distcc1 = Center.Distance(center1);
	      if (!Qualified1.IsUnqualified()) { 
		qualifier1(NbrSol) = Qualified1.Qualifier();
	      }
	      else if (Abs(distcc1+Radius-R1) < Tol) {
		qualifier1(NbrSol) = GccEnt_enclosed;
	      }
	      else if (Abs(distcc1-R1-Radius) < Tol) {
		qualifier1(NbrSol) = GccEnt_outside;
	      }
	      else { qualifier1(NbrSol) = GccEnt_enclosing; }
	      if (!Qualified2.IsUnqualified()) { 
		qualifier2(NbrSol) = Qualified2.Qualifier();
	       }
	      else if (dc2.Dot(normL2) > 0.0) {
		qualifier2(NbrSol) = GccEnt_outside;
	      }
	      else { qualifier2(NbrSol) = GccEnt_enclosed; }
	      if (Center.Distance(center1) <= Tolerance &&
		  Abs(Radius-C1.Radius()) <= Tolerance) {
		TheSame1(NbrSol) = 1;
	      }
	      else {
		TheSame1(NbrSol) = 0;
		gp_Dir2d dc1(center1.XY()-Center.XY());
		pnttg1sol(NbrSol) = gp_Pnt2d(Center.XY()+Radius*dc1.XY());
		par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
						  pnttg1sol(NbrSol));
		pararg1(NbrSol)=ElCLib::Parameter(C1,pnttg1sol(NbrSol));
	      }
	      TheSame2(NbrSol) = 0;
	      Standard_Real sign = dc2.Dot(gp_Dir2d(normL2.XY()));
	      dc2 = gp_Dir2d(sign*gp_XY(normL2.XY()));
	      pnttg2sol(NbrSol) = gp_Pnt2d(Center.XY()+Radius*dc2.XY());
	      par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
						pnttg2sol(NbrSol));
	      pararg2(NbrSol)=ElCLib::Parameter(L2,pnttg2sol(NbrSol));
	      pntcen(NbrSol) = Center;
	      parcen3(NbrSol)=ElCLib::Parameter(OnCirc,pntcen(NbrSol));
	    }
	  }
	}
	WellDone = Standard_True;
      }
    }
  }
}

