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
#include <GccAna_LinPnt2dBisec.hxx>
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
#include <Precision.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

//=========================================================================
//   Creation of a circle Tangent to : 1 straight line L1.                +
//                        Passing by : 1 point Point2.                    +
//                        Centered on  : 1 straight line OnLine.                  +
//   with a Tolerance of precision  : Tolerance.                        +
//                                                                        +
//  We start by making difference with various boundary cases that will be +
//  processed separately.                                            +
//  For the general case:                                                  +
//  ====================                                                  +
//  We calculate bissectrices to L1 and Point2 that give us       +
//  all possible locations of centers of all circles        +
//  tangent to L1 and passing through Point2.                                  +
//  We intersect these bissectrices with straight line OnLine which gives us +
//  the points among which we'll choose the solutions.   +
//  The choices are made basing on Qualifieurs of L1.        +
//=========================================================================
GccAna_Circ2d2TanOn::
   GccAna_Circ2d2TanOn (const GccEnt_QualifiedLin&  Qualified1 ,
                        const gp_Pnt2d&             Point2     ,
                        const gp_Lin2d&             OnLine     ,
                        const Standard_Real         Tolerance  ):
   cirsol(1,4)     ,
   qualifier1(1,4) ,
   qualifier2(1,4),
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
  WellDone = Standard_False;
  NbrSol = 0;
  if (!(Qualified1.IsEnclosed() ||
	Qualified1.IsOutside() || Qualified1.IsUnqualified())) {
    throw GccEnt_BadQualifier();
    return;
  }
  Standard_Real Tol = Abs(Tolerance);
  gp_Dir2d dirx(1.,0.);
  gp_Lin2d L1 = Qualified1.Qualified();
  gp_Pnt2d originL1(L1.Location());
  gp_Dir2d dirL1(L1.Direction());
  gp_Dir2d normal(-dirL1.Y(),dirL1.X());

//=========================================================================
//   Processing of boundary cases.                                          +
//=========================================================================

  if (dirL1.IsEqual(OnLine.Direction(),Precision::Confusion()) &&
      OnLine.Distance(originL1)<Precision::Confusion()) {
    // POP : l2s 2 straight line are identic : no Sol
    NbrSol = 0;
    return ;
  }


  Standard_Real dp2l = OnLine.Distance(Point2);
  gp_Dir2d donline(OnLine.Direction());
  gp_Pnt2d pinterm(Point2.XY()+dp2l*gp_XY(-donline.Y(),donline.X()));
  if (OnLine.Distance(pinterm) > Tol) {
    pinterm = gp_Pnt2d(Point2.XY()-dp2l*gp_XY(-donline.Y(),donline.X()));
  }
  Standard_Real dist = L1.Distance(pinterm);
  if (Abs(dist-dp2l) <= Tol) {
    gp_Dir2d dirbid(originL1.XY()-pinterm.XY());
    if (Qualified1.IsEnclosed() && dirbid.Dot(normal)<0.) {
      WellDone = Standard_True;
    }
    else if (Qualified1.IsOutside() && dirbid.Dot(normal) > 0.) {
      WellDone = Standard_True;
    }
    else if (Qualified1.IsUnqualified()) { WellDone = Standard_True; }
    if (WellDone) {
      NbrSol++;
      cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(pinterm,dirx),dp2l);
//    ======================================================
      qualifier2(NbrSol) = GccEnt_noqualifier;
      gp_Dir2d dc2(originL1.XY()-pinterm.XY());
      if (!Qualified1.IsUnqualified()) { 
	qualifier1(NbrSol) = Qualified1.Qualifier();
      }
      else if (dc2.Dot(normal) > 0.0) {
	qualifier1(NbrSol) = GccEnt_outside;
      }
      else { qualifier1(NbrSol) = GccEnt_enclosed; }
      Standard_Real sign = dc2.Dot(gp_Dir2d(-dirL1.Y(),
					    dirL1.X()));
      dc2 = gp_Dir2d(sign*gp_XY(-dirL1.Y(),dirL1.X()));
      pnttg1sol(NbrSol) = gp_Pnt2d(pinterm.XY()+dp2l*dc2.XY());
      pnttg2sol(NbrSol) = Point2;
      pntcen(NbrSol) = pinterm;
      par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),pnttg1sol(NbrSol));
      pararg1(NbrSol)=ElCLib::Parameter(L1,pnttg1sol(NbrSol));
      par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),pnttg2sol(NbrSol));
      pararg2(NbrSol) = 0.;
      parcen3(NbrSol)=ElCLib::Parameter(OnLine,pntcen(NbrSol));
      return;
    }
  }

//=========================================================================
//   General case.                                                        +
//=========================================================================

  GccAna_LinPnt2dBisec Bis(L1,Point2);
  if (Bis.IsDone()) {
    Handle(GccInt_Bisec) Sol = Bis.ThisSolution();
    GccInt_IType type = Sol->ArcType();
    IntAna2d_AnaIntersection Intp;
    if (type == GccInt_Lin) {
      Intp.Perform(OnLine,Sol->Line());
    }
    if (type == GccInt_Par) {
      Intp.Perform(OnLine,IntAna2d_Conic(Sol->Parabola()));
    }
    if (Intp.IsDone()) {
      if (!Intp.IsEmpty()) {
	for (Standard_Integer j = 1 ; j <= Intp.NbPoints() ; j++) {
	  gp_Pnt2d Center(Intp.Point(j).Value());
	  Standard_Real Radius = L1.Distance(Center);
//	  Standard_Integer nbsol = 1;
	  Standard_Boolean ok = Standard_False;
	  if (Qualified1.IsEnclosed()) {
	    if ((((originL1.X()-Center.X())*(-dirL1.Y()))+
		 ((originL1.Y()-Center.Y())*(dirL1.X())))<=0){
	      ok = Standard_True;
	    }
	  }
	  else if (Qualified1.IsOutside()) {
	    if ((((originL1.X()-Center.X())*(-dirL1.Y()))+
		 ((originL1.Y()-Center.Y())*(dirL1.X())))>=0){
	      ok = Standard_True;
	    }
	  }
	  else if (Qualified1.IsUnqualified()) {
	    ok = Standard_True;
	  }
	  if (ok) {
	    NbrSol++;
	    cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius);
//          =======================================================
	    qualifier2(NbrSol) = GccEnt_noqualifier;
	    gp_Dir2d dc2(originL1.XY()-Center.XY());
	    if (!Qualified1.IsUnqualified()) { 
	      qualifier1(NbrSol) = Qualified1.Qualifier();
	    }
	    else if (dc2.Dot(normal) > 0.0) {
	      qualifier1(NbrSol) = GccEnt_outside;
	    }
	    else { qualifier1(NbrSol) = GccEnt_enclosed; }
	    TheSame1(NbrSol) = 0;
	    TheSame2(NbrSol) = 0;
	    gp_Dir2d dc1(originL1.XY()-Center.XY());
	    Standard_Real sign = dc1.Dot(gp_Dir2d(normal));
	    dc1=gp_Dir2d(sign*(normal.XY()));
	    pnttg1sol(NbrSol) = gp_Pnt2d(Center.XY()+Radius*dc1.XY());
	    pnttg2sol(NbrSol) = Point2;
	    pntcen(NbrSol) = Center;
	    par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
					      pnttg1sol(NbrSol));
	    pararg1(NbrSol)=ElCLib::Parameter(L1,pnttg1sol(NbrSol));
	    par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
					      pnttg2sol(NbrSol));
	    pararg2(NbrSol) = 0.;
	    parcen3(NbrSol)=ElCLib::Parameter(OnLine,pntcen(NbrSol));
	  }
	}
      }
      WellDone = Standard_True;
    }
  }
}

