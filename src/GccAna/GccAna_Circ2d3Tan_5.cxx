// Copyright (c) 1995-1999 Matra Datavision
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

// init. de MinRad et MaxRad (PRO15604), JCT 09/10/98

#include <ElCLib.hxx>
#include <GccAna_Circ2d3Tan.hxx>
#include <GccAna_CircLin2dBisec.hxx>
#include <GccAna_LinPnt2dBisec.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <GccEnt_QualifiedLin.hxx>
#include <GccInt_BLine.hxx>
#include <GccInt_BParab.hxx>
#include <GccInt_IType.hxx>
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

//===========================================================================
//   Creation of a circle tangent to a circle, a straight line and a point. +
//===========================================================================
GccAna_Circ2d3Tan::
   GccAna_Circ2d3Tan (const GccEnt_QualifiedCirc& Qualified1 ,
                      const GccEnt_QualifiedLin&  Qualified2 ,
                      const gp_Pnt2d&             Point3     ,
		      const Standard_Real         Tolerance  ):

//=========================================================================
//   Initialization of fields.                                           +
//=========================================================================

   cirsol(1,4)     ,
   qualifier1(1,4) ,
   qualifier2(1,4) ,
   qualifier3(1,4) ,
   TheSame1(1,4)   ,  
   TheSame2(1,4)   ,
   TheSame3(1,4)   ,
   pnttg1sol(1,4)  ,
   pnttg2sol(1,4)  ,
   pnttg3sol(1,4)  ,
   par1sol(1,4)    ,
   par2sol(1,4)    ,
   par3sol(1,4)    ,
   pararg1(1,4)    ,
   pararg2(1,4)    ,
   pararg3(1,4)    
{

   gp_Dir2d dirx(1.0,0.0);
   Standard_Real Tol = Abs(Tolerance);
   Standard_Real MaxRad = 1e10, MinRad = 1e-6;
   WellDone = Standard_False;
   NbrSol = 0;
   if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
	 Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
       !(Qualified2.IsEnclosed() ||
	 Qualified2.IsOutside() || Qualified2.IsUnqualified())) {
     throw GccEnt_BadQualifier();
     return;
   }

//=========================================================================
//   Processing.                                                          +
//=========================================================================

   gp_Circ2d C1(Qualified1.Qualified());
   gp_Lin2d L2(Qualified2.Qualified());
   Standard_Real R1      = C1.Radius();
   gp_Pnt2d center1(C1.Location());
   gp_Pnt2d origin2(L2.Location());
   gp_Dir2d dir2(L2.Direction());
   gp_Dir2d normL2(-dir2.Y(),dir2.X());

   TColStd_Array1OfReal Radius(1,2);
   GccAna_CircLin2dBisec Bis1(C1,L2);
   GccAna_LinPnt2dBisec Bis2(L2,Point3);
   if (Bis1.IsDone() && Bis2.IsDone()) {
     Standard_Integer nbsolution1 = Bis1.NbSolutions();
     for (Standard_Integer i = 1 ; i <=  nbsolution1; i++) {
       Handle(GccInt_Bisec) Sol1 = Bis1.ThisSolution(i);
       Handle(GccInt_Bisec) Sol2 = Bis2.ThisSolution();
       GccInt_IType typ1 = Sol1->ArcType();
       GccInt_IType typ2 = Sol2->ArcType();
       IntAna2d_AnaIntersection Intp;
       if (typ1 == GccInt_Lin) {
	 if (typ2 == GccInt_Lin) {
	   Intp.Perform(Sol1->Line(),Sol2->Line());
	 }
	 else if (typ2 == GccInt_Par) {
	   Intp.Perform(Sol1->Line(),IntAna2d_Conic(Sol2->Parabola()));
	 }
       }
       else if (typ1 == GccInt_Par) {
	 if (typ2 == GccInt_Lin) {
	   Intp.Perform(Sol2->Line(),IntAna2d_Conic(Sol1->Parabola()));
	 }
	 else if (typ2 == GccInt_Par) {
	   Intp.Perform(Sol1->Parabola(),IntAna2d_Conic(Sol2->Parabola()));
	 }
       }
       if (Intp.IsDone()) {
	 if (!Intp.IsEmpty()) {
	   for (Standard_Integer j = 1 ; j <= Intp.NbPoints() ; j++) {
	     gp_Pnt2d Center(Intp.Point(j).Value());
	     Standard_Real dist1 = Center.Distance(C1.Location());
	     Standard_Real dist2 = L2.Distance(Center);
	     Standard_Real dist3 = Center.Distance(Point3);
	     Standard_Integer nbsol1 = 0;
	     Standard_Integer nbsol3 = 0;
	     Standard_Boolean ok = Standard_False;
	     if (Qualified1.IsEnclosed()) {
	       if (dist1-R1 < Tolerance) {
		 Radius(1) = Abs(R1-dist1);
		 nbsol1 = 1;
		 ok = Standard_True;
	       }
	     }
	     else if (Qualified1.IsOutside()) {
	       if (R1-dist1 < Tolerance) {
		 Radius(1) = Abs(R1-dist1);
		 nbsol1 = 1;
		 ok = Standard_True;
	       }
	     }
	     else if (Qualified1.IsEnclosing()) {
	       ok = Standard_True;
	       nbsol1 = 1;
	       Radius(1) = Abs(R1-dist1);
	     }
	     else if (Qualified1.IsUnqualified()) {
	       ok = Standard_True;
	       nbsol1 = 2;
	       Radius(1) = Abs(R1-dist1);
	       Radius(2) = R1+dist1;
	     }
	     if (Qualified2.IsEnclosed() && ok) {
	       if ((((L2.Location().X()-Center.X())*(-L2.Direction().Y()))+
		    ((L2.Location().Y()-Center.Y())*(L2.Direction().X())))<=0){
		 for (Standard_Integer ii = 1 ; ii <= nbsol1 ; ii++) {
		   if (Abs(dist2-Radius(ii)) < Tol) { 
		     ok = Standard_True;
		     Radius(1) = Radius(ii);
		   }
		 }
	       }
	     }
	     else if (Qualified2.IsOutside() && ok) {
	       if ((((L2.Location().X()-Center.X())*(-L2.Direction().Y()))+
		    ((L2.Location().Y()-Center.Y())*(L2.Direction().X())))>=0){
		 for (Standard_Integer ii = 1 ; ii <= nbsol1 ; ii++) {
		   if (Abs(dist2-Radius(ii)) < Tol) { 
		     ok = Standard_True;
		     Radius(1) = Radius(ii);
		   }
		 }
	       }
	     }
	     else if (Qualified2.IsUnqualified() && ok) {
	       for (Standard_Integer ii = 1 ; ii <= nbsol1 ; ii++) {
		 if (Abs(dist2-Radius(ii)) < Tol) { 
		   ok = Standard_True;
		   Radius(1) = Radius(ii);
		 }
	       }
	     }
	     if (Abs(dist3-Radius(1)) <= Tol && ok) {
	       ok = Standard_True;
	       nbsol3 = 1;
	     }
	     if (ok) {
	       for (Standard_Integer k = 1 ; k <= nbsol3 ; k++) {
		 if (NbrSol==4)
		   break;
// pop : if the radius is too great - no creation		 
		 if (Radius(k) > MaxRad) break;
		 if (Abs(Radius(k)) < MinRad) break;

		 NbrSol++;
		 cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius(k));
//               ==========================================================
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
		 gp_Dir2d dc2(origin2.XY()-Center.XY());
		 if (!Qualified2.IsUnqualified()) { 
		   qualifier2(NbrSol) = Qualified2.Qualifier();
		 }
		 else if (dc2.Dot(normL2) > 0.0) {
		   qualifier2(NbrSol) = GccEnt_outside;
		 }
		 else { qualifier2(NbrSol) = GccEnt_enclosed; }
		 qualifier3(NbrSol) = GccEnt_noqualifier;
		 if (Center.Distance(C1.Location()) <= Tolerance &&
		     Abs(Radius(k)-R1) <= Tolerance) {
		   TheSame1(NbrSol) = 1;
		 }
		 else {
		   TheSame1(NbrSol) = 0;
//  modified by NIZHNY-EAP Mon Nov  1 13:48:21 1999 ___BEGIN___
//		   gp_Dir2d dc(C1.Location().XY()-Center.XY());
		   gp_Dir2d dc(Center.XY()-C1.Location().XY());
//  modified by NIZHNY-EAP Mon Nov  1 13:48:55 1999 ___END___
		   pnttg1sol(NbrSol)=gp_Pnt2d(Center.XY()+Radius(k)*dc.XY());
		   par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
							  pnttg1sol(NbrSol));
		   pararg1(NbrSol)=ElCLib::Parameter(C1,pnttg1sol(NbrSol));
		 }
		 TheSame2(NbrSol) = 0;
		 TheSame3(NbrSol) = 0;
		 gp_Dir2d dc(L2.Location().XY()-Center.XY());
		 Standard_Real sign = dc.Dot(gp_Dir2d(-L2.Direction().Y(),
					     L2.Direction().X()));
		 dc = gp_Dir2d(sign*gp_XY(-L2.Direction().Y(),
					  L2.Direction().X()));
		 pnttg2sol(NbrSol) = gp_Pnt2d(Center.XY()+Radius(k)*dc.XY());
		 par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
						  pnttg2sol(NbrSol));
		 pararg2(NbrSol)=ElCLib::Parameter(L2,pnttg2sol(NbrSol));
		 pnttg3sol(NbrSol) = Point3;
		 par3sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
						  pnttg3sol(NbrSol));
		 pararg3(NbrSol) = 0.;
	       }
	     }
	   }
	 }
	 WellDone = Standard_True;
       }
       if (NbrSol==4)
	 break;
     }
   }
 }
