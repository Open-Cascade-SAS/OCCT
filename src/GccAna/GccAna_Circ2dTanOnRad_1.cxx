// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <GccAna_Circ2dTanOnRad.jxx>

#include <ElCLib.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <Standard_NegativeValue.hxx>
#include <gp_Dir2d.hxx>
#include <Standard_OutOfRange.hxx>
#include <GccEnt_BadQualifier.hxx>

//=========================================================================
//   Circle tangent to straight line  Qualified1 (L1)                         +
//          center on straight line OnLine                                  +
//          of radius Radius.                                              +
//                                                                        +
//  Initialize the table of solutions cirsol and all fields.              + 
//  Elimine depending on the qualifier the cases not being solutions.     +
//  Create L1para : parallel to L1 in the direction required by the       +
//                  qualifier at distance Radius.                         +
//  Point P of intersection between L1para and OnLine will give the center point +
//  of the solution.                                                +
//  Create solutions cirsol with center P and radius Radius.          +
//  Fill the fields.                                                +
//=========================================================================

GccAna_Circ2dTanOnRad::
   GccAna_Circ2dTanOnRad (const GccEnt_QualifiedLin& Qualified1,
                          const gp_Lin2d&            OnLine    ,
                          const Standard_Real        Radius    ,
                          const Standard_Real        Tolerance ):
   cirsol(1,2)     ,
   qualifier1(1,2) ,
   TheSame1(1,2)   ,
   pnttg1sol(1,2)  ,
   pntcen3(1,2)    ,
   par1sol(1,2)    ,
   pararg1(1,2)    ,
   parcen3(1,2)    
{

   Standard_Real Tol =Abs(Tolerance);
   gp_Dir2d dirx(1.0,0.0);
   WellDone = Standard_False;
   NbrSol = 0;
   if (!(Qualified1.IsEnclosed() ||
	 Qualified1.IsOutside() || Qualified1.IsUnqualified())) {
     GccEnt_BadQualifier::Raise();
     return;
   }
   Standard_Integer nbsol = 0;
   TColStd_Array1OfInteger eps(1,2);
   gp_Lin2d L1 = Qualified1.Qualified();
   gp_Pnt2d origin1(L1.Location());
   gp_Dir2d dir1(L1.Direction());
   gp_Dir2d normL1(-dir1.Y(),dir1.X());

   if (Radius < 0.0) { Standard_NegativeValue::Raise(); }
   else if ((OnLine.Direction()).IsParallel(dir1,Tol)) {
     WellDone = Standard_True;
   }
   else {
     if (Qualified1.IsEnclosed()) {
//   ============================
       eps(1) = -1;
       nbsol = 1;
     }
     else if (Qualified1.IsOutside()) {
//   ================================
       eps(1) = 1;
       nbsol = 1;
     }
     else {
//   ====
       eps(1) = 1;
       eps(2) = -1;
       nbsol = 2;
     }
     Standard_Real dx1 = dir1.X();
     Standard_Real dy1 = dir1.Y();
     Standard_Real lx1 = origin1.X();
     Standard_Real ly1 = origin1.Y();
     for (Standard_Integer j = 1 ; j <= nbsol ; j++) {
       gp_Lin2d L1para(gp_Pnt2d(lx1+eps(j)*Radius*dy1,ly1-eps(j)*Radius*dx1),
		       dir1);
       IntAna2d_AnaIntersection Intp(OnLine,L1para);
       if (Intp.IsDone()) {
         if (!Intp.IsEmpty()) {
           for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
             NbrSol++;
	     gp_Pnt2d Center(Intp.Point(i).Value());
             cirsol(NbrSol)=gp_Circ2d(gp_Ax2d(Center,dirx),Radius);
//           =====================================================
	     gp_Dir2d dc1(origin1.XY()-Center.XY());
	     if (!Qualified1.IsUnqualified()) { 
	       qualifier1(NbrSol) = Qualified1.Qualifier();
	     }
	     else if (dc1.Dot(normL1) > 0.0) {	
	       qualifier1(NbrSol) = GccEnt_outside; 
	     }
	     else { qualifier1(NbrSol) = GccEnt_enclosed; }
	     TheSame1(NbrSol) = 0;
	     if (gp_Vec2d(Center,origin1).Dot(gp_Dir2d(-dy1,dx1))>0.0) {
	       pnttg1sol(NbrSol)=gp_Pnt2d(Center.XY()+Radius*gp_XY(-dy1,dx1));
	       pntcen3(NbrSol) = cirsol(1).Location();
	     }
	     else {
	       pnttg1sol(NbrSol)=gp_Pnt2d(Center.XY()-Radius*gp_XY(-dy1,dx1));
	       pntcen3(NbrSol) = cirsol(1).Location();
	     }
	     par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
					      pnttg1sol(NbrSol));
	     pararg1(NbrSol)=ElCLib::Parameter(L1,pnttg1sol(NbrSol));
	     parcen3(NbrSol)=ElCLib::Parameter(OnLine,pntcen3(NbrSol));
           }
         }
	 WellDone = Standard_True;
       }
     }
   }
 }
