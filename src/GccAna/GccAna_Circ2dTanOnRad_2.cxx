// file GccAna_Circ2dTanOnRad_2.cxx, REG 08/07/91

#include <GccAna_Circ2dTanOnRad.jxx>

#include <ElCLib.hxx>
#include <math_DirectPolynomialRoots.hxx>
#include <Standard_NegativeValue.hxx>
#include <Standard_OutOfRange.hxx>
#include <gp_Dir2d.hxx>

//=========================================================================
//    typedef des objets manipules :                                      +
//=========================================================================

typedef math_DirectPolynomialRoots Roots;

//=========================================================================
//   Cercle tangent a un point    Point1.                                 +
//          centre sur une droite OnLine.                                 +
//          de rayon              Radius.                                 +
//                                                                        +
//  On initialise le tableau de solutions cirsol ainsi que tous les       +
//  champs.                                                               +
//  On elimine les cas ne presentant pas de solution.                     +
//  On resoud l equation du second degre indiquant que le point de centre +
//  recherche (xc,yc) est a une distance Radius du point Point1 et        +
//                        sur la droite OnLine.                           +
//  Les solutions sont representees par les cercles :                     +
//                   - de centre Pntcen(xc,yc)                            +
//                   - de rayon Radius.                                   +
//=========================================================================

GccAna_Circ2dTanOnRad::
   GccAna_Circ2dTanOnRad (const gp_Pnt2d&     Point1    ,
                          const gp_Lin2d&     OnLine    ,
                          const Standard_Real Radius    ,
                          const Standard_Real Tolerance ):
   cirsol(1,2)   ,
   qualifier1(1,2) ,
   TheSame1(1,2) ,
   pnttg1sol(1,2),
   pntcen3(1,2)  ,
   par1sol(1,2)  ,
   pararg1(1,2)  ,
   parcen3(1,2)  
{

   gp_Dir2d dirx(1.0,0.0);
   Standard_Real Tol = Abs(Tolerance);
   WellDone = Standard_False;
   NbrSol = 0;
   Standard_Real dp1lin = OnLine.Distance(Point1);

   if (Radius < 0.0) { Standard_NegativeValue::Raise(); }
   else {
     if (dp1lin > Radius+Tol) { WellDone = Standard_True; }
     Standard_Real xc;
     Standard_Real yc;
     Standard_Real x1 = Point1.X();
     Standard_Real y1 = Point1.Y();
     Standard_Real xbid = 0;
     Standard_Real xdir = (OnLine.Direction()).X();
     Standard_Real ydir = (OnLine.Direction()).Y();
     Standard_Real lxloc = (OnLine.Location()).X();
     Standard_Real lyloc = (OnLine.Location()).Y();
     if (Abs(dp1lin-Radius) < Tol) {
       WellDone = Standard_True;
       NbrSol = 1;
       if (-ydir*(x1-lxloc)+xdir*(y1-lyloc)<0.0) {
	 gp_Ax2d axe(gp_Pnt2d(x1-ydir*dp1lin,y1+xdir*dp1lin),dirx);
         cirsol(NbrSol) = gp_Circ2d(axe,Radius);
//       ======================================
	 qualifier1(NbrSol) = GccEnt_noqualifier;
       }
       else {
	 gp_Ax2d axe(gp_Pnt2d(x1+ydir*dp1lin,y1-xdir*dp1lin),dirx);
         cirsol(NbrSol) = gp_Circ2d(axe,Radius);
//       ======================================
	 qualifier1(NbrSol) = GccEnt_noqualifier;
       }
       TheSame1(NbrSol) = 0;
       pnttg1sol(NbrSol) = Point1;
       pntcen3(NbrSol) = cirsol(NbrSol).Location();
       pararg1(NbrSol) = 0.0;
       par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),pnttg1sol(NbrSol));
       parcen3(NbrSol)=ElCLib::Parameter(OnLine,pntcen3(NbrSol));
     }
     else if (dp1lin < Tol) {
       pntcen3(1) = gp_Pnt2d(Point1.X()+Radius*xdir,Point1.Y()+Radius*ydir);
       pntcen3(2) = gp_Pnt2d(Point1.X()-Radius*xdir,Point1.Y()-Radius*ydir);
       pntcen3(1) = ElCLib::Value(ElCLib::Parameter(OnLine,pntcen3(1)),OnLine);
       pntcen3(2) = ElCLib::Value(ElCLib::Parameter(OnLine,pntcen3(2)),OnLine);
       gp_Ax2d axe(pntcen3(1),OnLine.Direction());
       cirsol(1) = gp_Circ2d(axe,Radius);
       axe = gp_Ax2d(pntcen3(2),OnLine.Direction());
       cirsol(2) = gp_Circ2d(axe,Radius);
       TheSame1(1) = 0;
       pnttg1sol(1) = Point1;
       pararg1(1) = 0.0;
       par1sol(1)=ElCLib::Parameter(cirsol(1),pnttg1sol(1));
       parcen3(1)=ElCLib::Parameter(OnLine,pntcen3(1));
       TheSame1(2) = 0;
       pnttg1sol(2) = Point1;
       pararg1(2) = 0.0;
       par1sol(2)=ElCLib::Parameter(cirsol(2),pnttg1sol(2));
       parcen3(2)=ElCLib::Parameter(OnLine,pntcen3(2));
       NbrSol = 2;
     }
     else {
       Standard_Real A,B,C;
       OnLine.Coefficients(A,B,C);
       Standard_Real D = A;
       if (A == 0.0) {
	 A = B;
	 B = D;
	 xbid = x1;
	 x1 = y1;
	 y1 = xbid;
       }
       if (A != 0.0) {
	 Roots Sol((B*B+A*A)/(A*A),
		   2.0*(B*C/(A*A)+(B/A)*x1-y1),
		   x1*x1+y1*y1+C*C/(A*A)-Radius*Radius+2.0*C*x1/A);
         if (Sol.IsDone()) {
           for (Standard_Integer i = 1 ; i <= Sol.NbSolutions() ; i++) {
	     if (D != 0.0) {
	       yc = Sol.Value(i);
	       xc = -(B/A)*yc-C/A;
	     }
	     else {
	       xc = Sol.Value(i);
	       yc = -(B/A)*xc-C/A;
	     }
             NbrSol++;
	     gp_Pnt2d Center(xc,yc);
             cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius);
//           =======================================================
	     qualifier1(NbrSol) = GccEnt_noqualifier;
	     TheSame1(NbrSol) = 0;
	     pnttg1sol(NbrSol) = Point1;
	     pntcen3(NbrSol) = cirsol(NbrSol).Location();
	     pararg1(NbrSol) = 0.0;
	     par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
					      pnttg1sol(NbrSol));
	     parcen3(NbrSol)=ElCLib::Parameter(OnLine,pntcen3(NbrSol));
           }
           WellDone = Standard_True;
         }
       }
     }
   }
 }
