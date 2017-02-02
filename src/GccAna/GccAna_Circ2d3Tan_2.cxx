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
#include <GccAna_CircLin2dBisec.hxx>
#include <GccAna_Lin2dBisec.hxx>
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

//=========================================================================
//   Creation of a circle tangent to a circle and two straight lines.          +
//=========================================================================
GccAna_Circ2d3Tan::GccAna_Circ2d3Tan (const GccEnt_QualifiedCirc& Qualified1 ,
				      const GccEnt_QualifiedLin&  Qualified2 ,
				      const GccEnt_QualifiedLin&  Qualified3 ,
				      const Standard_Real         Tolerance  )
		   
//=========================================================================
//   Initialisation of fields.                                           +
//=========================================================================

:cirsol(1,8)    ,
qualifier1(1,8) ,
qualifier2(1,8) ,
qualifier3(1,8) ,
TheSame1(1,8)   ,
TheSame2(1,8)   ,
TheSame3(1,8)   ,
pnttg1sol(1,8)  ,
pnttg2sol(1,8)  , 
pnttg3sol(1,8)  ,
par1sol(1,8)    ,
par2sol(1,8)    ,
par3sol(1,8)    ,
pararg1(1,8)    ,
pararg2(1,8)    ,
pararg3(1,8)    
{
  
  TheSame1.Init(0);
  
  gp_Dir2d dirx(1.0,0.0);
   Standard_Real Tol = Abs(Tolerance);
   WellDone = Standard_False;
   NbrSol = 0;
   if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
	 Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
       !(Qualified2.IsEnclosed() || 
	 Qualified2.IsOutside() || Qualified2.IsUnqualified()) ||
       !(Qualified3.IsEnclosed() ||
	 Qualified3.IsOutside() || Qualified3.IsUnqualified())) {
     throw GccEnt_BadQualifier();
     return;
   }

//=========================================================================
//   Processing.                                                          +
//=========================================================================

   gp_Circ2d C1 = Qualified1.Qualified();
   gp_Lin2d L2  = Qualified2.Qualified();
   gp_Lin2d L3  = Qualified3.Qualified();
   Standard_Real R1      = C1.Radius();
   gp_Pnt2d center1(C1.Location());
   gp_Pnt2d origin2(L2.Location());
   gp_Dir2d dir2(L2.Direction());
   gp_Dir2d normL2(-dir2.Y(),dir2.X());
   gp_Pnt2d origin3(L3.Location());
   gp_Dir2d dir3(L3.Direction());
   gp_Dir2d normL3(-dir3.Y(),dir3.X());

   TColStd_Array1OfReal Radius(1,2);
   GccAna_CircLin2dBisec Bis1(C1,L2);
   GccAna_Lin2dBisec Bis2(L2,L3);
   if (Bis1.IsDone() && Bis2.IsDone()) {
     Standard_Integer nbsolution1 = Bis1.NbSolutions();
     Standard_Integer nbsolution2 = Bis2.NbSolutions();
     for (Standard_Integer i = 1 ; i <=  nbsolution1; i++) {
       Handle(GccInt_Bisec) Sol1 = Bis1.ThisSolution(i);
       GccInt_IType typ1 = Sol1->ArcType();
       IntAna2d_AnaIntersection Intp;
       for (Standard_Integer k = 1 ; k <=  nbsolution2; k++) {
	 if (typ1 == GccInt_Lin) {
	   Intp.Perform(Sol1->Line(),Bis2.ThisSolution(k));
	 }
	 else if (typ1 == GccInt_Par) {
	   Intp.Perform(Bis2.ThisSolution(k),IntAna2d_Conic(Sol1->Parabola()));
	 }
	 if (Intp.IsDone()) {
	   if ((!Intp.IsEmpty())&&(!Intp.ParallelElements())&&
	       (!Intp.IdenticalElements())) {
	     for (Standard_Integer j = 1 ; j <= Intp.NbPoints() ; j++) {
	       gp_Pnt2d Center(Intp.Point(j).Value());
	       Standard_Real dist1 = Center.Distance(center1);
	       Standard_Real dist2 = L2.Distance(Center);
	       Standard_Real dist3 = L3.Distance(Center);
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
		 if ((((origin2.X()-Center.X())*(-dir2.Y()))+
		    ((origin2.Y()-Center.Y())*(dir2.X())))<=0){
		   for (Standard_Integer ii = 1 ; ii <= nbsol1 ; ii++) {
		     if (Abs(dist2-Radius(ii)) < Tol) { 
		       ok = Standard_True;
		       Radius(1) = Radius(ii);
		     }
		   }
		 }
	       }
	       else if (Qualified2.IsOutside() && ok) {
		 if ((((origin2.X()-Center.X())*(-dir2.Y()))+
		    ((origin2.Y()-Center.Y())*(dir2.X())))>=0){
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
	       if (Qualified3.IsEnclosed() && ok) {
		 if ((((origin3.X()-Center.X())*(-dir3.Y()))+
		    ((origin3.Y()-Center.Y())*(dir3.X())))<=0){
		   if (Abs(dist3-Radius(1)) < Tol) { 
		     ok = Standard_True;
		     nbsol3 = 1;
		   }
		 }
	       }
	       else if (Qualified3.IsOutside() && ok) {
		 if ((((origin3.X()-Center.X())*(-dir3.Y()))+
		    ((origin3.Y()-Center.Y())*(dir3.X())))>=0){
		   if (Abs(dist3-Radius(1)) < Tol) { 
		     ok = Standard_True;
		     nbsol3 = 1;
		   }
		 }
	       }
	       else if (Qualified3.IsUnqualified() && ok) {
		 if (Abs(dist3-Radius(1)) < Tol) { 
		   ok = Standard_True;
		   nbsol3 = 1;
		 }
	       }
	       if (ok) {
		 for (Standard_Integer m = 1 ; m <= nbsol3 ; m++) {
		   NbrSol++;
		   cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius(m));
//                 ==========================================================
		   Standard_Real distcc1 = Center.Distance(center1);
		   if (!Qualified1.IsUnqualified()) { 
		     qualifier1(NbrSol) = Qualified1.Qualifier();
		   }
		   else if (Abs(distcc1+Radius(m)-R1) < Tol) {
		     qualifier1(NbrSol) = GccEnt_enclosed;
		   }
		   else if (Abs(distcc1-R1-Radius(m)) < Tol) {
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
		   gp_Dir2d dc3(origin3.XY()-Center.XY());
		   if (!Qualified3.IsUnqualified()) { 
		     qualifier3(NbrSol) = Qualified3.Qualifier();
		   }
		   else if (dc3.Dot(normL3) > 0.0) {
		     qualifier3(NbrSol) = GccEnt_outside;
		   }
		   else { qualifier3(NbrSol) = GccEnt_enclosed; }
		   if (Center.Distance(center1) <= Tolerance &&
		       Abs(Radius(m)-R1) <= Tolerance) {
		     TheSame1(NbrSol) = 1;
		   }
		   else {
		     TheSame1(NbrSol) = 0;
		     gp_Dir2d dc(center1.XY()-Center.XY());
		     pnttg1sol(NbrSol)=gp_Pnt2d(Center.XY()+Radius(m)*dc.XY());
		     par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
						      pnttg1sol(NbrSol));
		     pararg1(NbrSol)=ElCLib::Parameter(C1,pnttg1sol(NbrSol));
		   }
		   TheSame2(NbrSol) = 0;
		   TheSame3(NbrSol) = 0;
		   gp_Dir2d dc(origin2.XY()-Center.XY());
		   Standard_Real sign = dc.Dot(gp_Dir2d(-dir2.Y(),dir2.X()));
		   dc = gp_Dir2d(sign*gp_XY(-dir2.Y(),dir2.X()));
		   pnttg2sol(NbrSol) = gp_Pnt2d(Center.XY()+Radius(m)*dc.XY());
		   par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
						    pnttg2sol(NbrSol));
		   pararg2(NbrSol)=ElCLib::Parameter(L2,pnttg2sol(NbrSol));
		   dc = gp_Dir2d(origin3.XY()-Center.XY());
		   sign = dc.Dot(gp_Dir2d(-dir3.Y(),dir3.X()));
		   dc = gp_Dir2d(sign*gp_XY(-dir3.Y(),dir3.X()));
		   pnttg3sol(NbrSol) = gp_Pnt2d(Center.XY()+Radius(m)*dc.XY());
		   par3sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
						    pnttg3sol(NbrSol));
		   pararg3(NbrSol)=ElCLib::Parameter(L3,pnttg3sol(NbrSol));
		 }
	       }
	     }
	   }
	   WellDone = Standard_True;
	 }
       }
     }
   }
 }


