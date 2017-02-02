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

//=========================================================================
//   Creation of a circle tangent to Circle C1 and a straight line L2.    +
//                        centered on a straight line.                    +
//  We start by making difference between cases that we are going to      +
//  proceess separately.                                            +
//  In general case:                                                  +
//  ====================                                                  +
//  We calculate bissectrices to C1 and L2 that give us            +
//  all possibles locations of centers of all circles tangent to C1 and L2+                                                  +
//  We intersect these bissectrices with straight line OnLine which gives   +
//  us points among which we'll choose the solutions.   +
//  The choices are made basing on Qualifiers of C1 and L2.  +
//=========================================================================
GccAna_Circ2d2TanOn::
   GccAna_Circ2d2TanOn (const GccEnt_QualifiedCirc& Qualified1  , 
                        const GccEnt_QualifiedLin&  Qualified2  , 
                        const gp_Lin2d&             OnLine      ,
                        const Standard_Real         Tolerance   ):
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
   Standard_Real Tol = Abs(Tolerance);
   Standard_Real Radius=0;
   Standard_Boolean ok = Standard_False;
   gp_Dir2d dirx(1.,0.);
   gp_Circ2d C1 = Qualified1.Qualified();
   gp_Lin2d  L2 = Qualified2.Qualified();
   Standard_Real R1 = C1.Radius();
   gp_Pnt2d center1(C1.Location());
   gp_Pnt2d origin2(L2.Location());
   gp_Dir2d dirL2(L2.Direction());
   gp_Dir2d normL2(-dirL2.Y(),dirL2.X());

//=========================================================================
//   Processing of limit cases.                                          +
//=========================================================================

   Standard_Real distcl = OnLine.Distance(center1);
   gp_Pnt2d pinterm(center1.XY()+distcl*
		    gp_XY(-OnLine.Direction().Y(),OnLine.Direction().X()));
   if (OnLine.Distance(pinterm) > Tolerance) {
     pinterm = gp_Pnt2d(center1.XY()+distcl*
			gp_XY(-OnLine.Direction().Y(),OnLine.Direction().X()));
   }
   Standard_Real dist2 = L2.Distance(pinterm);
   if (Qualified1.IsEnclosed() || Qualified1.IsOutside()) {
     if (Abs(distcl-R1-dist2) <= Tol) { ok = Standard_True; }
   }
   else if (Qualified1.IsEnclosing()) {
     if (Abs(dist2-distcl-R1) <= Tol) { ok = Standard_True; }
   }
   else if (Qualified1.IsUnqualified()) { ok = Standard_True; }
   else { 
     throw GccEnt_BadQualifier();
     return;
   }
   if (ok) {
     if (Qualified2.IsOutside()) {
       gp_Pnt2d pbid(pinterm.XY()+dist2*gp_XY(-dirL2.Y(),dirL2.X()));
       if (L2.Distance(pbid) <= Tol) { WellDone = Standard_True; }
     }
     else if (Qualified2.IsEnclosed()) {
       gp_Pnt2d pbid(pinterm.XY()-dist2*gp_XY(-dirL2.Y(),dirL2.X()));
       if (L2.Distance(pbid) <= Tol) { WellDone = Standard_True; }
     }
     else if (Qualified2.IsUnqualified()) { WellDone = Standard_False; }
     else { 
       throw GccEnt_BadQualifier();
       return;
     }
   }
   if (WellDone) {
     NbrSol++;
     cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(pinterm,dirx),dist2);
//   =======================================================
     gp_Dir2d dc1(center1.XY()-pinterm.XY());
     gp_Dir2d dc2(origin2.XY()-pinterm.XY());
     Standard_Real distcc1 = pinterm.Distance(center1);
     if (!Qualified1.IsUnqualified()) { 
       qualifier1(NbrSol) = Qualified1.Qualifier();
     }
     else if (Abs(distcc1+dist2-R1) < Tol) {
       qualifier1(NbrSol) = GccEnt_enclosed;
     }
     else if (Abs(distcc1-R1-dist2) < Tol) {
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
     pnttg1sol(NbrSol) = gp_Pnt2d(pinterm.XY()+dist2*dc1.XY());
     pnttg2sol(NbrSol) = gp_Pnt2d(pinterm.XY()+dist2*dc2.XY());
     par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),pnttg1sol(NbrSol));
     pararg1(NbrSol)=ElCLib::Parameter(C1,pnttg1sol(NbrSol));
     par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),pnttg2sol(NbrSol));
     pararg2(NbrSol)=ElCLib::Parameter(L2,pnttg2sol(NbrSol));
     pntcen(NbrSol) = cirsol(NbrSol).Location();
     parcen3(NbrSol)=ElCLib::Parameter(OnLine,pntcen(NbrSol));
     return;
   }

//=========================================================================
//   General case.                                                        +
//=========================================================================

   GccAna_CircLin2dBisec Bis(C1,L2);
   if (Bis.IsDone()) {
     Standard_Integer nbsolution = Bis.NbSolutions();
     for (Standard_Integer i = 1 ; i <=  nbsolution; i++) {
       Handle(GccInt_Bisec) Sol = Bis.ThisSolution(i);
       GccInt_IType type = Sol->ArcType();
       IntAna2d_AnaIntersection Intp;
       if (type == GccInt_Lin) {
	 Intp.Perform(OnLine,Sol->Line());
       }
       else if (type == GccInt_Par) {
	 Intp.Perform(OnLine,IntAna2d_Conic(Sol->Parabola()));
       }
       if (Intp.IsDone()) {
	 if (!Intp.IsEmpty()) {
	   for (Standard_Integer j = 1 ; j <= Intp.NbPoints() ; j++) {
	     gp_Pnt2d Center(Intp.Point(j).Value());
	     Standard_Real dist1 = Center.Distance(center1);
	     dist2 = L2.Distance(Center);
//	     Standard_Integer nbsol = 1;
	     ok = Standard_False;
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
//             =======================================================
	       gp_Dir2d dc1(center1.XY()-Center.XY());
	       gp_Dir2d dc2(origin2.XY()-Center.XY());
	       Standard_Real distcc1 = Center.Distance(center1);
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
		 pnttg1sol(NbrSol) = gp_Pnt2d(Center.XY()+Radius*dc1.XY());
		 par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
							pnttg1sol(NbrSol));
		 pararg1(NbrSol)=ElCLib::Parameter(C1,pnttg1sol(NbrSol));
	       }
	       TheSame2(NbrSol) = 0;
	       Standard_Real sign = dc2.Dot(gp_Dir2d(-dirL2.Y(),dirL2.X()));
	       dc2 = gp_Dir2d(sign*gp_XY(-dirL2.Y(),dirL2.X()));
	       pnttg2sol(NbrSol) = gp_Pnt2d(Center.XY()+Radius*dc2.XY());
	       par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
						      pnttg2sol(NbrSol));
	       pararg2(NbrSol)=ElCLib::Parameter(L2,pnttg2sol(NbrSol));
	       pntcen(NbrSol) = Center;
	       parcen3(NbrSol)=ElCLib::Parameter(OnLine,pntcen(NbrSol));
	     }
	   }
	 }
	 WellDone = Standard_True;
       }
     }
   }
 }






