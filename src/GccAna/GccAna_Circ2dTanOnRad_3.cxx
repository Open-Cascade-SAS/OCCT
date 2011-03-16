// file GccAna_Circ2dTanOnRad_3.cxx, REG 08/07/91

#include <GccAna_Circ2dTanOnRad.jxx>

#include <ElCLib.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <Standard_NegativeValue.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColgp_Array1OfDir2d.hxx>
#include <gp_Dir2d.hxx>
#include <GccEnt_BadQualifier.hxx>

//=========================================================================
//   Cercle tangent a un cercle   Qualified1 (C1).                        +
//          centre sur un cercle  OnCirc.                                 +
//          de rayon              Radius.                                 +
//                                                                        +
//  On initialise le tableau de solutions cirsol ainsi que tous les       +
//  champs.                                                               +
//  On elimine les cas ne presentant pas de solution.                     +
//  On cree la (les) parallele(s) a C1 dans le (les) sens voulu(s).       +
//  On intersecte cette (ces) parallele(s) avec OnCirc et on obtient le   +
//  (les) point(s) de centre de la (des) solution(s) recherchee(s).       +
//  On cree cette (ces) solution(s) cirsol.                               +
//=========================================================================

GccAna_Circ2dTanOnRad::
   GccAna_Circ2dTanOnRad (const GccEnt_QualifiedCirc& Qualified1,
                          const gp_Circ2d&            OnCirc    ,
                          const Standard_Real         Radius    ,
                          const Standard_Real         Tolerance ):
   cirsol(1,4)   ,
   qualifier1(1,4) ,
   TheSame1(1,4) ,
   pnttg1sol(1,4),
   pntcen3(1,4)  ,
   par1sol(1,4)  ,
   pararg1(1,4)  ,
   parcen3(1,4)  
{
     
   TheSame1.Init(0);
   gp_Dir2d dirx(1.0,0.0);
   Standard_Real Tol = Abs(Tolerance);
   Standard_Integer signe[5];
   signe[0] = 0;
   signe[1] = 0;
   signe[2] = 0;
   signe[3] = 0;
   signe[4] = 0;
   Standard_Real disparal[2];
   Standard_Integer nparal = 0;
   Standard_Integer sign = 0;
   WellDone = Standard_False;
   NbrSol = 0;
   if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
	 Qualified1.IsOutside() || Qualified1.IsUnqualified())) {
     GccEnt_BadQualifier::Raise();
     return;
   }
   gp_Circ2d C1 = Qualified1.Qualified();
   TColgp_Array1OfPnt2d Center(1,4);
   TColgp_Array1OfDir2d dir1on(1,4);

   if (Radius < 0.0) { Standard_NegativeValue::Raise(); }
   else {
     Standard_Real R1 = C1.Radius();
     Standard_Real R2 = OnCirc.Radius();
     Standard_Real c1x = C1.Location().X();
     Standard_Real c1y = C1.Location().Y();
     gp_Pnt2d center1(c1x,c1y);
     Standard_Real dist = OnCirc.Location().Distance(center1);
     Standard_Real onx = OnCirc.Location().X();
     Standard_Real ony = OnCirc.Location().Y();
     if (Qualified1.IsEnclosed()) {
//   ============================
       if ((Radius-R1 > Tol) || (R1-dist-R2-Radius > Tol)) {
	 WellDone=Standard_True;
       }
       else {
         if (Abs(Radius-R1) < Tol) {
	   if (OnCirc.Distance(center1) < Tol) {
	     cirsol(NbrSol) = C1;
//           ==============
	     qualifier1(NbrSol) = Qualified1.Qualifier();
	     NbrSol = 1;
	     TheSame1(NbrSol) = 1;
	     pntcen3(NbrSol) = center1;
	     parcen3(NbrSol)=ElCLib::Parameter(OnCirc,pntcen3(NbrSol));
	     WellDone = Standard_True;
	   }
	   else { WellDone = Standard_False; }
         }
         else if (Abs(R1-dist-R2-Radius) <= Tol) {
	   dir1on(1) = gp_Dir2d(c1x-onx,c1y-ony);
	   sign = -1;
         }
	 else {
	   nparal = 1;
	   disparal[0] = Abs(R1-Radius);
	 }
       }
     }
     else if (Qualified1.IsEnclosing()) {
//   ==================================
       if ((Tol<R1-Radius)||(Tol<R1+dist-R2-Radius)||(Radius-R1-dist-R2>Tol)) {
	 WellDone = Standard_True;
       }
       else {
         if (Abs(Radius-R1) < Tol) {
	   if (OnCirc.Distance(center1) < Tol) {
	     cirsol(NbrSol) = C1;
//           ==============
	     qualifier1(NbrSol) = Qualified1.Qualifier();
	     NbrSol = 1;
	     TheSame1(NbrSol) = 1;
	     pntcen3(NbrSol) = center1;
	     parcen3(NbrSol)=ElCLib::Parameter(OnCirc,pntcen3(NbrSol));
	     WellDone = Standard_True;
	   }
	   else { WellDone = Standard_True; }
	 }
	 else if ((Abs(R1+dist-R2-Radius)<Tol)||(Abs(Radius-R1-dist-R2)<Tol)) {
	   dir1on(1) = gp_Dir2d(c1x-onx,c1y-ony);
	   if (Abs(R1+dist-R2-Radius) < Tol) { sign = 1; }
	   else { sign = -1; }
	 }
	 else {
	   nparal = 1;
	   disparal[0] = Abs(R1-Radius);
	 }
       }
     }
     else if (Qualified1.IsOutside()) {
//   ================================
       if ((dist-R2-R1-Radius>Tol)||(Radius-dist-R2+R1>Tol)) { 
	 WellDone=Standard_True;
       }
       else {
	 dir1on(1) = gp_Dir2d(c1x-onx,c1y-ony);
	 if (Abs(R1-dist-R2+Radius) < Tol) {
	   sign = -1;
	 }
	 else if (Abs(dist-R1-R2-Radius) < Tol) { sign = 1; }
	 else {
	   nparal = 1;
	   disparal[0] = Abs(R1+Radius);
	 }
       }
     }
     else {
       if ((dist-R2-R1-Radius>Tol)||(Radius-dist-R1-R2>Tol)) { 
	 WellDone=Standard_True;
       }
       else if ((R1-dist-R2>Tol)&&(Tol<R1-R2-dist-Radius)) {
	 WellDone = Standard_True;
       }
       else {
	 dir1on(1) = gp_Dir2d(c1x-onx,c1y-ony);
	 if (Abs(dist-R1-R2-Radius) < Tol) {
	   sign = 1;
	 }
	 else if (dist+R1+R2-Radius < 0.0) { sign = -1; }
	 else if ((R1-dist+R2>0.0) && (R1-R2-dist-Radius>0.0)) { sign = -1; }
	 else {
	   nparal = 2;
	   disparal[0] = Abs(R1+Radius);
	   disparal[1] = Abs(R1-Radius);
	 }
       }
     }
     for (Standard_Integer jj = 0 ; jj < nparal ; jj++) {
       IntAna2d_AnaIntersection Intp(OnCirc,
		     gp_Circ2d(gp_Ax2d(center1,dirx),disparal[jj]));
       if (Intp.IsDone()) {
	 if (!Intp.IsEmpty()) {
	   for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
	     NbrSol++;
	     Center(NbrSol) = gp_Pnt2d(Intp.Point(i).Value());
	     cirsol(NbrSol)=gp_Circ2d(gp_Ax2d(Center(NbrSol),dirx),Radius);
//           =============================================================
	     Standard_Real distcc1 = Center(NbrSol).Distance(center1);
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
	     dir1on(NbrSol) = gp_Dir2d(c1x-Center(NbrSol).X(),
				       c1y-Center(NbrSol).Y());
	     if ((R1>Radius) && (Center(NbrSol).Distance (center1)<=R1)){
	       signe[NbrSol-1] = -1;
	     }
	     else {
	       signe[NbrSol-1] = 1;
	     }
	   }
	 }
	 WellDone = Standard_True;
       }
     }
     if (sign != 0) {
       Center(1) = gp_Pnt2d(OnCirc.Location().XY()+sign*R2*dir1on(1).XY());
       cirsol(1) = gp_Circ2d(gp_Ax2d(Center(1),dirx),Radius); 
//     =====================================================
       Standard_Real distcc1 = Center(1).Distance(center1);
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
       NbrSol = 1;
       signe[NbrSol-1] = 1;
       WellDone = Standard_True;
     }
     Standard_Integer ii = 0;
     while (signe[ii] != 0) {
       pnttg1sol(ii+1) = gp_Pnt2d(Center(ii+1).XY()+
				  signe[ii]*Radius*dir1on(ii+1).XY());
       pntcen3(ii+1) = cirsol(ii+1).Location();
       pararg1(ii+1)=ElCLib::Parameter(C1,pnttg1sol(ii+1));
       par1sol(ii+1)=ElCLib::Parameter(cirsol(ii+1),pnttg1sol(ii+1));
       parcen3(ii+1)=ElCLib::Parameter(OnCirc,pntcen3(ii+1));
       ii++;
     }
   }
 }
