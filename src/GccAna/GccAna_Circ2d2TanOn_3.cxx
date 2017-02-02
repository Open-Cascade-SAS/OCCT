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
#include <GccAna_CircPnt2dBisec.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <GccEnt_QualifiedLin.hxx>
#include <GccInt_BCirc.hxx>
#include <GccInt_Bisec.hxx>
#include <GccInt_BLine.hxx>
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

//=========================================================================
//  Circles tangent to circle C1, passing by point Point2 and centers     +
//  on a straight line OnLine.                                            +
//  We start by making difference with boundary cases that will be        +
//  processed separately.                                                 +
//  For the general case:                                                 +
//  ====================                                                  +
//  We calculate bissectrices to C1 and Point2 that give us all           +
//  possible locations of centers of all circles                          +
//  tangent to C1 and passing by Point2.                                  +
//  We intersect these bissectrices with the straight line OnLine which   +
//  gives us the points among which we'll choose the solutions.           +
//  The choices are made using Qualifiers of C1 and C2.                   +
//=========================================================================
GccAna_Circ2d2TanOn::
   GccAna_Circ2d2TanOn (const GccEnt_QualifiedCirc& Qualified1 ,
                        const gp_Pnt2d&             Point2     ,
                        const gp_Lin2d&             OnLine     ,
                        const Standard_Real         Tolerance  ):
   cirsol(1,4),
   qualifier1(1,4) ,
   qualifier2(1,4) ,
   TheSame1(1,4) ,
   TheSame2(1,4) ,
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
  Standard_Real Tol = Abs(Tolerance);
  WellDone = Standard_False;
  NbrSol = 0;
  if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
	Qualified1.IsOutside() || Qualified1.IsUnqualified())) {
    throw GccEnt_BadQualifier();
    return;
  }
  TColStd_Array1OfReal Radius(1,2);
  gp_Dir2d dirx(1.,0.);
  gp_Circ2d C1 = Qualified1.Qualified();
  Standard_Real R1 = C1.Radius();
  gp_Pnt2d center1(C1.Location());
  
//=========================================================================
//   Processing of boundary cases.                                        +
//=========================================================================

  Standard_Real dp2l = OnLine.Distance(Point2);
  gp_Dir2d donline(OnLine.Direction());
  gp_Pnt2d pinterm(Point2.XY()+dp2l*gp_XY(-donline.Y(),donline.X()));
  if (OnLine.Distance(pinterm) > Tol) {
    pinterm = gp_Pnt2d(Point2.XY()-dp2l*gp_XY(-donline.Y(),donline.X()));
  }
  Standard_Real dist = pinterm.Distance(center1);
  if (Qualified1.IsEnclosed() && Abs(R1-dist-dp2l) <= Tol) {
    WellDone = Standard_True;
  }
  else if (Qualified1.IsEnclosing() && Abs(R1+dist-dp2l) <= Tol) {
    WellDone = Standard_True;
   }
  else if (Qualified1.IsOutside() && Abs(dist-dp2l) <= Tol) {
    WellDone = Standard_True;
   }
  else if (Qualified1.IsUnqualified() && 
	   (Abs(dist-dp2l) <= Tol || Abs(R1-dist-dp2l) <= Tol ||
	    Abs(R1+dist-dp2l) <= Tol)) {
    WellDone = Standard_True;
  }
  if (WellDone) {
    NbrSol++;
    cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(pinterm,dirx),dp2l);
//  ======================================================
    gp_Dir2d dc1(center1.XY()-pinterm.XY());
    Standard_Real distcc1 = pinterm.Distance(center1);
    if (!Qualified1.IsUnqualified()) { 
      qualifier1(NbrSol) = Qualified1.Qualifier();
    }
    else if (Abs(distcc1+dp2l-R1) < Tol) {
      qualifier1(NbrSol) = GccEnt_enclosed;
    }
    else if (Abs(distcc1-R1-dp2l) < Tol) {
      qualifier1(NbrSol) = GccEnt_outside;
    }
    else { qualifier1(NbrSol) = GccEnt_enclosing; }
    qualifier2(NbrSol) = GccEnt_noqualifier;
    pnttg1sol(NbrSol) = gp_Pnt2d(pinterm.XY()+dp2l*dc1.XY());
    par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),pnttg1sol(NbrSol));
    pararg1(NbrSol)=ElCLib::Parameter(C1,pnttg1sol(NbrSol));
    pnttg2sol(NbrSol) = Point2;
    par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),pnttg2sol(NbrSol));
    pntcen(NbrSol) = cirsol(NbrSol).Location();
    parcen3(NbrSol)=ElCLib::Parameter(OnLine,pntcen(NbrSol));
    return;
  }

//=========================================================================
//   General case.                                                       +
//=========================================================================

  GccAna_CircPnt2dBisec Bis(C1,Point2);
  if (Bis.IsDone()) {
    Standard_Integer nbsolution = Bis.NbSolutions();
    for (Standard_Integer i = 1 ; i <=  nbsolution; i++) {
      Handle(GccInt_Bisec) Sol = Bis.ThisSolution(i);
      GccInt_IType type = Sol->ArcType();
      IntAna2d_AnaIntersection Intp;
      if (type == GccInt_Lin) {
	Intp.Perform(OnLine,Sol->Line());
      }
      else if (type == GccInt_Cir) {
	Intp.Perform(OnLine,Sol->Circle());
      }
      else if (type == GccInt_Ell) {
	Intp.Perform(OnLine,IntAna2d_Conic(Sol->Ellipse()));
      }
      else if (type == GccInt_Hpr) {
	Intp.Perform(OnLine,IntAna2d_Conic(Sol->Hyperbola()));
      }
      if (Intp.IsDone()) {
	if (!Intp.IsEmpty()) {
	  for (Standard_Integer j = 1 ; j <= Intp.NbPoints() ; j++) {
	    gp_Pnt2d Center(Intp.Point(j).Value());
	    Standard_Real dist1 = center1.Distance(Center);
	    Standard_Integer nbsol = 1;
	    Standard_Boolean ok = Standard_False;
	    if (Qualified1.IsEnclosed()) {
	      if (dist1-C1.Radius() <= Tolerance) {
		ok = Standard_True;
		Radius(1) = Abs(C1.Radius()-dist1);
	      }
	    }
	    else if (Qualified1.IsOutside()) {
	      if (C1.Radius()-dist1 <= Tolerance) {
		ok = Standard_True;
		Radius(1) = Abs(C1.Radius()-dist1);
	      }
	    }
	    else if (Qualified1.IsEnclosing()) {
	      ok = Standard_True;
	      Radius(1) = C1.Radius()+dist1;
	    }
/*	    else if (Qualified1.IsUnqualified() && ok) {
	      ok = Standard_True;
	      nbsol = 2;
	      Radius(1) = Abs(C1.Radius()-dist1);
	      Radius(2) = C1.Radius()+dist1;
	    }
*/
	    else if (Qualified1.IsUnqualified() ) {
	      Standard_Real popradius = Center.Distance(Point2);
	      if (Abs(popradius-dist1)) {
		ok = Standard_True;
		Radius(1) = popradius;
	      } 
	    }	
		
	    if (ok) {
	      for (Standard_Integer k = 1 ; k <= nbsol ; k++) {
		NbrSol++;
		cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius(k));
//              ==========================================================
		Standard_Real distcc1 = Center.Distance(center1);
		if (!Qualified1.IsUnqualified()) { 
		  qualifier1(NbrSol) = Qualified1.Qualifier();
		}
		else if (Abs(distcc1+Radius(k)-R1) < Tol) {
		  qualifier1(NbrSol) = GccEnt_enclosed;
		}
		else if (Abs(distcc1-R1-Radius(k)) < Tol) {
		  qualifier1(NbrSol) = GccEnt_outside;
		}
		else { qualifier1(NbrSol) = GccEnt_enclosing; }
		qualifier2(NbrSol) = GccEnt_noqualifier;
		if (Center.Distance(center1) <= Tolerance &&
		    Abs(Radius(k)-C1.Radius()) <= Tolerance) {
		  TheSame1(NbrSol) = 1;
		}
		else {
		  TheSame1(NbrSol) = 0;
		  gp_Dir2d dc1(center1.XY()-Center.XY());
		  pnttg1sol(NbrSol)=gp_Pnt2d(Center.XY()+Radius(k)*dc1.XY());
		  par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
						    pnttg1sol(NbrSol));
		  pararg1(i)=ElCLib::Parameter(C1,pnttg1sol(NbrSol));
		}
		TheSame2(NbrSol) = 0;
		pnttg2sol(NbrSol) = Point2;
		par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
						  pnttg2sol(NbrSol));
		pararg2(NbrSol)=0.;
		pntcen(NbrSol) = Center;
		parcen3(NbrSol)=ElCLib::Parameter(OnLine,pntcen(NbrSol));
	      }
	    }
	  }
	}
	WellDone = Standard_True;
      }
    }
  }
}
