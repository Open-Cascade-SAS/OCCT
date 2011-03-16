
// File GccAna_Circ2d3Tan.cxx, REG 08/07/91

#include <GccAna_Circ2d3Tan.jxx>

#include <ElCLib.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <GccAna_LinPnt2dBisec.hxx>
#include <GccAna_Lin2dBisec.hxx>
#include <GccAna_Pnt2dBisec.hxx>
#include <GccInt_Bisec.hxx>
#include <GccInt_IType.hxx>
#include <IntAna2d_Conic.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <Precision.hxx>
//=========================================================================
//   Creation d un cercle tangent a une droite et a deux points.          +
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
     GccEnt_BadQualifier::Raise();
     return;
   }

//=========================================================================
//   Traitement.                                                          +
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
     gp_Lin2d linint1(Bis1.ThisSolution());
     Handle(GccInt_Bisec) Sol2 = Bis2.ThisSolution();
     GccInt_IType typ2 = Sol2->ArcType();
#ifdef DEB
     gp_Lin2d linintb(Bis1.ThisSolution());
#else
     Bis1.ThisSolution() ;
#endif
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
#ifdef DEB
	   Standard_Real dist3 = Center.Distance(Point3);
#else
           Center.Distance(Point3);
#endif
	   Standard_Real Radius=0;
	   Standard_Integer nbsol1 = 0;
//	   Standard_Integer nbsol2 = 0;
	   Standard_Integer nbsol3 = 0;
	   Standard_Boolean ok = Standard_False;
	   if (Qualified1.IsEnclosed()) {
	     if ((((origin1.X()-Center.X())*(-dir1.Y()))+
		  ((origin1.Y()-Center.Y())*(dir1.X())))<=0){
	       ok = Standard_True;
	       nbsol1 = 1;
	       Radius = dist1;
	     }
	   }
	   else if (Qualified1.IsOutside()) {
	     if ((((origin1.X()-Center.X())*(-dir1.Y()))+
		  ((origin1.Y()-Center.Y())*(dir1.X())))>=0){
	       ok = Standard_True;
	       nbsol1 = 1;
	       Radius = dist1;
	     }
	   }
	   else if (Qualified1.IsUnqualified()) {
	     ok = Standard_True;
	     nbsol1 = 1;
	     Radius = dist1;
	   }
	   if (ok) {
	     if (Abs(dist2-Radius)<=Tol && Abs(dist2-Radius)<=Tol) { 
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

