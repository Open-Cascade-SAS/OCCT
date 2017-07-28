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
#include <GccAna_Lin2dBisec.hxx>
#include <GccAna_LinPnt2dBisec.hxx>
#include <GccAna_Pnt2dBisec.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <GccEnt_QualifiedLin.hxx>
#include <GccInt_Bisec.hxx>
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

//=========================================================================
//   Creation of a circle tangent to a straight line and two points.      +
//=========================================================================
GccAna_Circ2d3Tan::
   GccAna_Circ2d3Tan (const GccEnt_QualifiedLin& Qualified1,
                      const gp_Pnt2d&            Point2    ,
                      const gp_Pnt2d&            Point3    ,
		      const Standard_Real        Tolerance ):

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

   WellDone = Standard_False;
   Standard_Real Tol = Abs(Tolerance);
   gp_Dir2d dirx(1.0,0.0);
   NbrSol = 0;
   if (!(Qualified1.IsEnclosed() ||
	 Qualified1.IsOutside() || Qualified1.IsUnqualified())) {
     throw GccEnt_BadQualifier();
     return;
   }

//=========================================================================
//   Processing.                                                          +
//=========================================================================

   gp_Lin2d L1  = Qualified1.Qualified();
   gp_Pnt2d origin1(L1.Location());
   gp_Dir2d dir1(L1.Direction());
   gp_Dir2d normL1(-dir1.Y(),dir1.X());

   if (Point2.IsEqual(Point3,Precision::Confusion())) {
     WellDone = Standard_False;
     return ;
   }

   GccAna_Pnt2dBisec Bis1(Point2,Point3);
   GccAna_LinPnt2dBisec Bis2(L1,Point2);
   if (Bis1.IsDone() && Bis2.IsDone()) {
     const gp_Lin2d linint1(Bis1.ThisSolution());
     Handle(GccInt_Bisec) Sol2 = Bis2.ThisSolution();
     GccInt_IType typ2 = Sol2->ArcType();
     IntAna2d_AnaIntersection Intp;
     if (typ2 == GccInt_Lin) {
       gp_Lin2d linint2(Sol2->Line());
       Intp.Perform (linint1,linint2);
     }
     else if (typ2 == GccInt_Par) {
       Intp.Perform (linint1,IntAna2d_Conic(Sol2->Parabola()));
     }
     if (Intp.IsDone()) {
       if ((!Intp.IsEmpty())&&(!Intp.ParallelElements())&&
	   (!Intp.IdenticalElements())) {
	 for (Standard_Integer j = 1 ; j <= Intp.NbPoints() ; j++) {
	   gp_Pnt2d Center(Intp.Point(j).Value());
	   Standard_Real dist1 = L1.Distance(Center);
	   Standard_Real dist2 = Center.Distance(Point2);

	   Standard_Real Radius=0;
	   Standard_Integer nbsol3 = 0;
	   Standard_Boolean ok = Standard_False;
     Standard_Real  difference = (((origin1.X()-Center.X())*(-dir1.Y())) + ((origin1.Y()-Center.Y())*(dir1.X())));
     if ((Qualified1.IsEnclosed() && difference <= 0) ||
         (Qualified1.IsOutside() && difference >= 0) ||
         (Qualified1.IsUnqualified()))
     {
       ok = Standard_True;
       Radius = dist1;
     }
	   if (ok) {
	     if (Abs(dist2-Radius)<=Tol) { 
	       nbsol3 = 1;
	     }
	     else { ok = Standard_False; }
	   }
	   if (ok) {
	     for (Standard_Integer k = 1 ; k <= nbsol3 ; k++) {
	       NbrSol++;
	       cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius);
//             =======================================================
	       gp_Dir2d dc1(origin1.XY()-Center.XY());
	       if (!Qualified1.IsUnqualified()) { 
		 qualifier1(NbrSol) = Qualified1.Qualifier();
	       }
	       else if (dc1.Dot(normL1) > 0.0) {
		 qualifier1(NbrSol) = GccEnt_outside;
	       }
	       else { qualifier1(NbrSol) = GccEnt_enclosed; }
	       qualifier2(NbrSol) = GccEnt_noqualifier;
	       qualifier3(NbrSol) = GccEnt_noqualifier;
	       TheSame1(NbrSol) = 0;
	       gp_Dir2d dc(origin1.XY()-Center.XY());
	       Standard_Real sign = dc.Dot(gp_Dir2d(-dir1.Y(),dir1.X()));
	       dc = gp_Dir2d(sign*gp_XY(-dir1.Y(),dir1.X()));
	       pnttg1sol(NbrSol) = gp_Pnt2d(Center.XY()+Radius*dc.XY());
	       par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
						pnttg1sol(NbrSol));
	       pararg1(NbrSol)=ElCLib::Parameter(L1,pnttg1sol(NbrSol));
	       TheSame2(NbrSol) = 0;
	       pnttg2sol(NbrSol) = Point2;
	       par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
						pnttg2sol(NbrSol));
	       pararg2(NbrSol) = 0.;
	       TheSame3(NbrSol) = 0;
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
   }
 }

