// file GccAna_Circ2dTanOnRad_1.cxx, REG 08/07/91

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
//   Cercle tangent a une droite  Qualified1 (L1)                         +
//          centre sur une droite OnLine                                  +
//          de rayon Radius.                                              +
//                                                                        +
//  On initialise le tableau de solutions cirsol ainsi que tous les       +
//  champs.                                                               +
//  On elimine en fonction du qualifieur les cas ne presentant pas de     +
//  solutions.                                                            +
//  On cree L1para : la parallele a L1 dans le sens voulu par le          +
//                   qualifieur a une distance Radius.                    +
//  Le point P d intersection entre L1para et OnLine donnera le point de  +
//  centre de la solution.                                                +
//  On cree les solutions cirsol de centre P et de rayon Radius.          +
//  On remplit les champs.                                                +
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
#ifdef DEB
	     Standard_Real sign = dc1.Dot(normL1);
#else
             dc1.Dot(normL1);
#endif
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
