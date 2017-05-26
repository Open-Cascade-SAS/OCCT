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

// cas de 2 cercles concentriques JCT 28/11/97

#include <ElCLib.hxx>
#include <GccAna_Circ2d3Tan.hxx>
#include <GccAna_Circ2dBisec.hxx>
#include <GccAna_CircPnt2dBisec.hxx>
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
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <TColStd_Array1OfReal.hxx>

static Standard_Integer MaxSol = 20;
//=========================================================================
//   Creation of a circle tangent to two circles and a point.           +
//=========================================================================

GccAna_Circ2d3Tan::
   GccAna_Circ2d3Tan (const GccEnt_QualifiedCirc& Qualified1 ,
                      const GccEnt_QualifiedCirc& Qualified2 ,
                      const gp_Pnt2d&             Point3     ,
		      const Standard_Real         Tolerance  ):

//=========================================================================
//   Initialization of fields.                                           +
//=========================================================================

   cirsol(1,MaxSol)     ,
   qualifier1(1,MaxSol) ,
   qualifier2(1,MaxSol) ,
   qualifier3(1,MaxSol) ,
   TheSame1(1,MaxSol)   ,
   TheSame2(1,MaxSol)   ,
   TheSame3(1,MaxSol)   ,
   pnttg1sol(1,MaxSol)  ,
   pnttg2sol(1,MaxSol)  ,
   pnttg3sol(1,MaxSol)  ,
   par1sol(1,MaxSol)    ,
   par2sol(1,MaxSol)    ,
   par3sol(1,MaxSol)    ,
   pararg1(1,MaxSol)    ,
   pararg2(1,MaxSol)    ,
   pararg3(1,MaxSol)    
{

   gp_Dir2d dirx(1.0,0.0);
   Standard_Real Tol = Abs(Tolerance);
   WellDone = Standard_False;
   NbrSol = 0;
   if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
	 Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
       !(Qualified2.IsEnclosed() || Qualified2.IsEnclosing() || 
	 Qualified2.IsOutside() || Qualified2.IsUnqualified())) {
     throw GccEnt_BadQualifier();
     return;
   }

//=========================================================================
//   Processing.                                                          +
//=========================================================================

   gp_Circ2d C1(Qualified1.Qualified());
   gp_Circ2d C2(Qualified2.Qualified());
   Standard_Real R1 = C1.Radius();
   Standard_Real R2 = C2.Radius();
   gp_Pnt2d center1(C1.Location());
   gp_Pnt2d center2(C2.Location());

   TColStd_Array1OfReal Radius(1,2);
   GccAna_Circ2dBisec Bis1(C1,C2);
   GccAna_CircPnt2dBisec Bis2(C1,Point3);
   if (Bis1.IsDone() && Bis2.IsDone()) {
     Standard_Integer nbsolution1 = Bis1.NbSolutions();
     Standard_Integer nbsolution2 = Bis2.NbSolutions();
     for (Standard_Integer i = 1 ; i <=  nbsolution1; i++) {
       Handle(GccInt_Bisec) Sol1 = Bis1.ThisSolution(i);
       GccInt_IType typ1 = Sol1->ArcType();
       IntAna2d_AnaIntersection Intp;
       for (Standard_Integer k = 1 ; k <=  nbsolution2; k++) {
	 Handle(GccInt_Bisec) Sol2 = Bis2.ThisSolution(k);
	 GccInt_IType typ2 = Sol2->ArcType();
	 if (typ1 == GccInt_Cir) {
	   if (typ2 == GccInt_Cir) {
	     Intp.Perform(Sol1->Circle(),Sol2->Circle());
	   }
	   else if (typ2 == GccInt_Lin) {
	     Intp.Perform(Sol2->Line(),Sol1->Circle());
	   }
	   else if (typ2 == GccInt_Hpr) {
	     Intp.Perform(Sol1->Circle(),IntAna2d_Conic(Sol2->Hyperbola()));
	   }
	   else if (typ2 == GccInt_Ell) {
	     Intp.Perform(Sol1->Circle(),IntAna2d_Conic(Sol2->Ellipse()));
	   }
	 }
	 else if (typ1 == GccInt_Ell) {
	   if (typ2 == GccInt_Cir) {
	     Intp.Perform(Sol2->Circle(),IntAna2d_Conic(Sol1->Ellipse()));
	   }
	   else if (typ2 == GccInt_Lin) {
	     Intp.Perform(Sol2->Line(),IntAna2d_Conic(Sol1->Ellipse()));
	   }
	   else if (typ2 == GccInt_Hpr) {
	     Intp.Perform(Sol1->Ellipse(),IntAna2d_Conic(Sol2->Hyperbola()));
	   }
	   else if (typ2 == GccInt_Ell) {
	     Intp.Perform(Sol1->Ellipse(),IntAna2d_Conic(Sol2->Ellipse()));
	   }
	 }
	 else if (typ1 == GccInt_Lin) {
	   if (typ2 == GccInt_Cir) {
	     Intp.Perform(Sol1->Line(),Sol2->Circle());
	   }
	   else if (typ2 == GccInt_Lin) {
	     Intp.Perform(Sol1->Line(),Sol2->Line());
	   }
	   else if (typ2 == GccInt_Hpr) {
	     Intp.Perform(Sol1->Line(),IntAna2d_Conic(Sol2->Hyperbola()));
	   }
	   else if (typ2 == GccInt_Ell) {
	     Intp.Perform(Sol1->Line(),IntAna2d_Conic(Sol2->Ellipse()));
	   }
	 }
	 else if (typ1 == GccInt_Hpr) {
	   if (typ2 == GccInt_Cir) {
	     Intp.Perform(Sol2->Circle(),IntAna2d_Conic(Sol1->Hyperbola()));
	   }
	   else if (typ2 == GccInt_Lin) {
	     Intp.Perform(Sol2->Line(),IntAna2d_Conic(Sol1->Hyperbola()));
	   }
	   else if (typ2 == GccInt_Hpr) {
	     Intp.Perform(Sol2->Hyperbola(),IntAna2d_Conic(Sol1->Hyperbola()));
	   }
	   else if (typ2 == GccInt_Ell) {
	     Intp.Perform(Sol2->Ellipse(),IntAna2d_Conic(Sol1->Hyperbola()));
	   }
	 }
	 if (Intp.IsDone()) {
	   if (!Intp.IsEmpty()) {
	     for (Standard_Integer j = 1 ; j <= Intp.NbPoints() ; j++) {
	       Standard_Real Rradius=0;
	       gp_Pnt2d Center(Intp.Point(j).Value());
	       Standard_Real dist1 = Center.Distance(center1);
	       Standard_Real dist2 = Center.Distance(center2);
	       Standard_Real dist3 = Center.Distance(Point3);
	       Standard_Integer nbsol1 = 0;
	       Standard_Integer nbsol2 = 0;
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
	       if (Qualified2.IsEnclosed() && ok) {
		 if (dist2-R2 < Tolerance) {
		   for (Standard_Integer ii = 1 ; ii <= nbsol1 ; ii++) {
		     if (Abs(Radius(ii)-Abs(R2-dist2)) < Tol) {
		       Radius(1) = Abs(R2-dist2);
		       ok = Standard_True;
		       nbsol2 = 1;
		     }
		   }
		 }
	       }
	       else if (Qualified2.IsOutside() && ok) {
		 if (R2-dist2 < Tolerance) {
		   for (Standard_Integer ii = 1 ; ii <= nbsol1 ; ii++) {
		     if (Abs(Radius(ii)-Abs(R2-dist2)) < Tol) {
		       Radius(1) = Abs(R2-dist2);
		       ok = Standard_True;
		       nbsol2 = 1;
		     }
		   }
		 }
	       }
	       else if (Qualified2.IsEnclosing() && ok) {
		 for (Standard_Integer ii = 1 ; ii <= nbsol1 ; ii++) {
		   if (Abs(Radius(ii)-R2-dist2) < Tol) {
		     Radius(1) = R2+dist2;
		     ok = Standard_True;
		     nbsol2 = 1;
		   }
		 }
	       }
	       else if (Qualified2.IsUnqualified() && ok) {
		 for (Standard_Integer ii = 1 ; ii <= nbsol1 ; ii++) {
		   if (Abs(Radius(ii)-Abs(R2-dist2)) < Tol) {
		     Rradius = Abs(R2-dist2);
		     ok = Standard_True;
		     nbsol2++;
		   }
		   else if (Abs(Radius(ii)-R2-dist2) < Tol) {
		     Rradius = R2+dist2;
		     ok = Standard_True;
		     nbsol2++;
		   }
		 }
		 if (nbsol2 == 1) {
		   Radius(1) = Rradius;
		 }
		 else if (nbsol2 == 2) {
		   Radius(1) = Abs(R2-dist2);
		   Radius(2) = R2+dist2;
		 }
	       }
	       for (Standard_Integer ii = 1 ; ii <= nbsol2 ; ii++) {
		 if (Abs(dist3-Radius(ii)) <= Tol) {
		   nbsol3++;
		   ok = Standard_True;
		 }
	       }
	       if (ok) {
		 for (Standard_Integer k1 = 1 ; k1 <= nbsol3 ; k1++) {
		   NbrSol++;
		   cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius(k1));
//                 ==========================================================
		   Standard_Real distcc1 = Center.Distance(center1);
		   if (!Qualified1.IsUnqualified()) { 
		     qualifier1(NbrSol) = Qualified1.Qualifier();
		   }
		   else if (Abs(distcc1+Radius(k1)-R1) < Tol) {
		     qualifier1(NbrSol) = GccEnt_enclosed;
		   }
		   else if (Abs(distcc1-R1-Radius(k1)) < Tol) {
		     qualifier1(NbrSol) = GccEnt_outside;
		   }
		   else { qualifier1(NbrSol) = GccEnt_enclosing; }

//		   Standard_Real distcc2 = Center.Distance(center1);
		   Standard_Real distcc2 = Center.Distance(center2);
		   if (!Qualified2.IsUnqualified()) { 
		     qualifier2(NbrSol) = Qualified2.Qualifier();
		   }
		   else if (Abs(distcc2+Radius(k1)-R2) < Tol) {
		     qualifier2(NbrSol) = GccEnt_enclosed;
		   }
		   else if (Abs(distcc2-R2-Radius(k1)) < Tol) {
		     qualifier2(NbrSol) = GccEnt_outside;
		   }
		   else { qualifier2(NbrSol) = GccEnt_enclosing; }
		   qualifier3(NbrSol) = GccEnt_noqualifier;
		   if (Center.Distance(center1) <= Tolerance &&
		       Abs(Radius(k1)-R1) <= Tolerance) {
		     TheSame1(NbrSol) = 1;
		   }
		   else {
		     TheSame1(NbrSol) = 0;
		     gp_Dir2d dc(center1.XY()-Center.XY());
		     if (qualifier1(NbrSol) == GccEnt_enclosed)
		       dc.Reverse(); // if tangent circle is inside the source circle, moving to edge of source circle
		     pnttg1sol(NbrSol)=gp_Pnt2d(Center.XY()+Radius(k1)*dc.XY());
		     par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
						      pnttg1sol(NbrSol));
		     pararg1(NbrSol)=ElCLib::Parameter(C1,
						      pnttg1sol(NbrSol));
		   }
		   if (Center.Distance(center2) <= Tolerance &&
		       Abs(Radius(k1)-R2) <= Tolerance) {
		     TheSame2(NbrSol) = 1;
		   }
		   else {
		     TheSame2(NbrSol) = 0;
		     gp_Dir2d dc(center2.XY()-Center.XY());
		     // case of concentric circles : 
		     // 2nd tangency point is at the other side of the circle solution
		     Standard_Real alpha = 1.;
		     if (center1.Distance(center2)<=Tolerance) alpha = -1;
		     pnttg2sol(NbrSol)=gp_Pnt2d(Center.XY()+alpha*Radius(k1)*dc.XY());
		     par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
						      pnttg2sol(NbrSol));
		     pararg2(NbrSol)=ElCLib::Parameter(C2,pnttg2sol(NbrSol));
		   }
		   TheSame3(NbrSol) = 0;
		   pnttg3sol(NbrSol) = Point3;
		   par3sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
						    pnttg3sol(NbrSol));
		   pararg3(NbrSol) = 0.;
		   WellDone = Standard_True;
		   if (NbrSol==MaxSol) break;
		 }
	       }
	     }
	   }
	   WellDone = Standard_True;
	   if (NbrSol==MaxSol) break;
	 }
	 if (NbrSol==MaxSol) break;
       }
       if (NbrSol==MaxSol) break;
     }
   }

   // Debug to create the point on the solution circles.

   Standard_Integer kk ;
   for ( kk = 1; kk <= NbrSol; kk++) {
     gp_Circ2d CC = cirsol(kk);
     Standard_Real NR = CC.Location().Distance(Point3);
     if (Abs(NR - CC.Radius()) > Tol) {
       cirsol(kk).SetRadius(NR);
     }
   }

   // Debug to eliminate multiple solution.
   // this happens in case of intersection line hyperbola.
   Standard_Real Tol2 = Tol*Tol;
   for (kk = 1; kk <NbrSol; kk++) {
     gp_Pnt2d PK = cirsol(kk).Location();
     for (Standard_Integer ll = kk+1 ; ll <= NbrSol; ll++) {
       gp_Pnt2d PL = cirsol(ll).Location();
       if (PK.SquareDistance(PL) < Tol2) {
	 for (Standard_Integer mm = ll+1 ; mm <= NbrSol; mm++) {
	   cirsol(mm - 1)   = cirsol (mm);   
	   pnttg1sol(mm-1)  = pnttg1sol(mm);
	   pnttg2sol(mm-1)  = pnttg2sol(mm);
	   pnttg3sol(mm-1)  = pnttg3sol(mm);
	   par1sol(mm-1)    = par1sol(mm);
	   par2sol(mm-1)    = par2sol(mm);
	   par3sol(mm-1)    = par3sol(mm);
	   pararg1(mm-1)    = pararg1(mm);
	   pararg2(mm-1)    = pararg2(mm);
	   pararg3(mm-1)    = pararg3(mm);
	   qualifier1(mm-1) = qualifier1(mm);
	   qualifier2(mm-1) = qualifier2(mm);
	   qualifier3(mm-1) = qualifier3(mm);
	 }
	 NbrSol--;
       }
     }
   }
 }

