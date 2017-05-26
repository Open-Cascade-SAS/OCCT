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


#include <ElCLib.hxx>
#include <GccAna_Circ2d3Tan.hxx>
#include <GccAna_CircPnt2dBisec.hxx>
#include <GccAna_Pnt2dBisec.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <GccEnt_QualifiedLin.hxx>
#include <GccInt_BCirc.hxx>
#include <GccInt_BElips.hxx>
#include <GccInt_BHyper.hxx>
#include <GccInt_BLine.hxx>
#include <GccInt_IType.hxx>
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
#include <TColStd_Array1OfReal.hxx>

//=======================================================================
//   Creation of a circle tangent to a circle and two points.           +
//=======================================================================
GccAna_Circ2d3Tan::
   GccAna_Circ2d3Tan (const GccEnt_QualifiedCirc& Qualified1 ,
                      const gp_Pnt2d&             Point2     ,
                      const gp_Pnt2d&             Point3     ,
		      const Standard_Real         Tolerance  ):

   cirsol(1,2)     ,
   qualifier1(1,2) ,
   qualifier2(1,2) ,
   qualifier3(1,2) ,
   TheSame1(1,2)   ,
   TheSame2(1,2)   ,
   TheSame3(1,2)   ,
   pnttg1sol(1,2)  ,
   pnttg2sol(1,2)  ,
   pnttg3sol(1,2)  ,
   par1sol(1,2)    ,
   par2sol(1,2)    ,
   par3sol(1,2)    ,
   pararg1(1,2)    ,
   pararg2(1,2)    ,
   pararg3(1,2)    
{

   gp_Dir2d dirx(1.0,0.0);
   Standard_Real Tol = Abs(Tolerance);
   WellDone = Standard_False;
   NbrSol = 0;
   if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
	 Qualified1.IsOutside() || Qualified1.IsUnqualified())) {
     throw GccEnt_BadQualifier();
     return;
   }

//=========================================================================
//   Processing.                                                          +
//=========================================================================

   gp_Circ2d C1 = Qualified1.Qualified();
   Standard_Real R1 = C1.Radius();
   gp_Pnt2d center1(C1.Location());
   TColStd_Array1OfReal Radius(1,2);

   if (Point2.IsEqual(Point3,Precision::Confusion())) {
     WellDone = Standard_False;
     return ;
   }

   GccAna_Pnt2dBisec Bis1(Point2,Point3);
   GccAna_CircPnt2dBisec Bis2(C1,Point2);

   if (Bis1.IsDone() && Bis2.IsDone()) {
     Standard_Integer nbsolution2 = Bis2.NbSolutions();
     for (Standard_Integer i = 1 ; i <=  nbsolution2; i++) {
       Handle(GccInt_Bisec) Sol2 = Bis2.ThisSolution(i);
       GccInt_IType typ2 = Sol2->ArcType();
       gp_Lin2d Sol1(Bis1.ThisSolution());
       IntAna2d_AnaIntersection Intp;
       if (typ2 == GccInt_Cir) {
	 Intp.Perform(Sol1,Sol2->Circle());
       }
       else if (typ2 == GccInt_Lin) {
	 Intp.Perform(Sol1,Sol2->Line());
       }
       else if (typ2 == GccInt_Hpr) {
	 Intp.Perform(Sol1,IntAna2d_Conic(Sol2->Hyperbola()));
       }
       else if (typ2 == GccInt_Ell) {
	 Intp.Perform(Sol1,IntAna2d_Conic(Sol2->Ellipse()));
       }

       if (Intp.IsDone()) {
	 if (!Intp.IsEmpty()) {
	   for (Standard_Integer j = 1 ; j <= Intp.NbPoints() ; j++) {
	     gp_Pnt2d Center(Intp.Point(j).Value());
	     Standard_Real dist1 = Center.Distance(center1);
	     Standard_Real dist2 = Center.Distance(Point2);
	     Standard_Real dist3 = Center.Distance(Point3);
	     Standard_Integer nbsol1 = 0;
//	     Standard_Integer nbsol2 = 0;
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
	       Radius(1) = R1+dist1;
	     }
	     else if (Qualified1.IsUnqualified()) {
	       ok = Standard_True;
	       nbsol1 = 2;
	       Radius(1) = Abs(R1-dist1);
	       Radius(2) = R1+dist1;
	     }
	     if (ok) {
	       ok = Standard_False;
	       for (Standard_Integer ii = 1 ; ii <= nbsol1 ; ii++) {
//pop		 if (Abs(dist2-Radius(ii))<=Tol && Abs(dist2-Radius(ii))<=Tol){
		 if (Abs(dist2-Radius(ii))<=Tol && Abs(dist3-Radius(ii))<=Tol){
		   nbsol3 = ii;
		   ok = Standard_True;
		 }
	       }
	     }

	     if (ok) {
//	       for (Standard_Integer k = 1 ; k <= nbsol3 ; k++) {
		 if (NbrSol>=2) break;
		 NbrSol++;
//		 cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius(k));
		 cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius(nbsol3));
//               ==========================================================
		 Standard_Real distcc1 = Center.Distance(center1);
		 if (!Qualified1.IsUnqualified()) { 
		   qualifier1(NbrSol) = Qualified1.Qualifier();
		 }
		 else if (Abs(distcc1+Radius(nbsol3)-R1) < Tol) {
		   qualifier1(NbrSol) = GccEnt_enclosed;
		 }
		 else if (Abs(distcc1-R1-Radius(nbsol3)) < Tol) {
		   qualifier1(NbrSol) = GccEnt_outside;
		 }
		 else { qualifier1(NbrSol) = GccEnt_enclosing; }
		 qualifier2(NbrSol) = GccEnt_noqualifier;
		 qualifier3(NbrSol) = GccEnt_noqualifier;
		 if (Center.Distance(center1) <= Tolerance &&
		     Abs(Radius(nbsol3)-R1) <= Tolerance) {
		   TheSame1(NbrSol) = 1;
		 }
		 else {
		   TheSame1(NbrSol) = 0;
		   gp_Dir2d dc(center1.XY()-Center.XY());
		   if (qualifier1(NbrSol) == GccEnt_enclosed)
		     dc.Reverse(); // if tangent circle is inside the source circle, moving to edge of source circle
		   pnttg1sol(NbrSol)=gp_Pnt2d(Center.XY()+Radius(nbsol3)*dc.XY());
		   par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol), pnttg1sol(NbrSol));
		   pararg1(NbrSol)=ElCLib::Parameter(C1,pnttg1sol(NbrSol));
		 }

		 TheSame2(NbrSol) = 0;
		 pnttg2sol(NbrSol) = Point2;
		 par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
						  pnttg2sol(NbrSol));
		 pararg2(NbrSol)=0.;
		 TheSame3(NbrSol) = 0;
		 pnttg3sol(NbrSol) = Point3;
		 par3sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
						  pnttg3sol(NbrSol));
		 pararg3(NbrSol) = 0.;
	       //}
	     }
	   }
	 }
	 WellDone = Standard_True;
       }
     }
   }
 }


