// file GccAna_Circ2dTanOnRad_4.cxx, REG 08/07/91

#include <GccAna_Circ2dTanOnRad.jxx>

#include <ElCLib.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <Standard_NegativeValue.hxx>
#include <Standard_OutOfRange.hxx>
#include <gp_Dir2d.hxx>
#include <GccEnt_BadQualifier.hxx>

//=========================================================================
//   Cercle tangent a une droite Qualified1 (L1).                         +
//          centre sur un cercle OnCirc.                                  +
//          de rayon             Radius.                                  +
//                                                                        +
//  On initialise le tableau de solutions cirsol ainsi que tous les       +
//  champs.                                                               +
//  On elimine les cas ne presentant pas de solution.                     +
//  On cree la (les) parallele(s) a L1 dans le (les) sens voulu(s).       +
//  On intersecte cette (ces) parallele(s) avec OnCirc et on obtient les  +
//  points de centre des solutions recherchees.                           +
//  On cree ces solutions cirsol.                                         +
//=========================================================================

GccAna_Circ2dTanOnRad::
   GccAna_Circ2dTanOnRad (const GccEnt_QualifiedLin& Qualified1,
                          const gp_Circ2d&           OnCirc    ,
                          const Standard_Real        Radius    ,
                          const Standard_Real        Tolerance ):

//=========================================================================
//  Initialisation des champs.                                            +
//=========================================================================

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
   WellDone = Standard_False;
   NbrSol = 0;
   if (!(Qualified1.IsEnclosed() ||
	 Qualified1.IsOutside() || Qualified1.IsUnqualified())) {
     GccEnt_BadQualifier::Raise();
     return;
   }

//=========================================================================
//  Initialisation de diverses variables.                                 +
//=========================================================================

   Standard_Integer nbsol = 0;
   Standard_Integer sign = 0;
   gp_Lin2d L1 = Qualified1.Qualified();
   gp_Pnt2d origin1(L1.Location());
   gp_Dir2d dir1(L1.Direction());
   gp_Dir2d normL1(-dir1.Y(),dir1.X());
   Standard_Real dist1 = L1.Distance(OnCirc.Location())-OnCirc.Radius();
   Standard_Real dist2 = L1.Distance(OnCirc.Location())+OnCirc.Radius();

//=========================================================================
//  Traitement.                                                           +
//=========================================================================

   if (Radius < 0.0) { Standard_NegativeValue::Raise(); }
   else {
     L1 = Qualified1.Qualified();
     if ((dist1-Radius>Tol) || (Tol<Radius-dist2)) { WellDone=Standard_True; }
     else {

// a modifier ulterieurement.

       if (dist1-Radius > 0.0) { dist1 = Radius; }
       else if (dist2-Radius < 0.0) { dist2 = Radius; }

       if (Qualified1.IsEnclosed()) {
//     ============================
	 nbsol = 1;
	 sign = -1;
       }
       else if (Qualified1.IsOutside()) {
//     ================================
	 nbsol = 1;
	 sign = 1;
       }
       else {
//     ====
	 nbsol = 2;
	 sign = 1;
       }
       for (Standard_Integer j = 1 ; j <= nbsol ;j++) {
	 sign = -sign;
         gp_Lin2d L(gp_Pnt2d(origin1.X()-sign*Radius*dir1.Y(),
			     origin1.Y()+sign*Radius*dir1.X()),dir1);
         IntAna2d_AnaIntersection Intp(L,OnCirc);
         if (Intp.IsDone()) {
           if (!Intp.IsEmpty()) {
             for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
               NbrSol++;
	       gp_Pnt2d Center(Intp.Point(i).Value());
	       gp_Ax2d axe(Center,dirx);
               cirsol(NbrSol) = gp_Circ2d(axe,Radius);
//             ======================================
	       gp_Dir2d dc1(origin1.XY()-Center.XY());
	       sign = (Standard_Integer) dc1.Dot(normL1);
	       if (!Qualified1.IsUnqualified()) { 
		 qualifier1(NbrSol) = Qualified1.Qualifier();
	       }
	       else if (dc1.Dot(normL1) > 0.0) {	
		 qualifier1(NbrSol) = GccEnt_outside; 
	       }
	       else { qualifier1(NbrSol) = GccEnt_enclosed; }
	       pntcen3(NbrSol) = cirsol(NbrSol).Location();
	       pnttg1sol(NbrSol) = gp_Pnt2d(pntcen3(NbrSol).XY()+
				    gp_XY(sign*Radius*dir1.Y(),
					  -sign*Radius*dir1.X()));
	       pararg1(NbrSol)=ElCLib::Parameter(L1,pnttg1sol(NbrSol));
	       par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
						pnttg1sol(NbrSol));
	       parcen3(NbrSol)=ElCLib::Parameter(OnCirc,pntcen3(NbrSol));
             }
           }
	   WellDone = Standard_True;
         }
       }
     }
   }
 }



