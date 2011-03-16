// File:	GccAna_Circ2d2TanOn_5.cxx
// Created:	Thu Jan  2 15:55:26 1992
// Author:	Remi GILET
//		<reg@topsn3>

#include <GccAna_Circ2d2TanOn.jxx>

#include <ElCLib.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Ax2d.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>

//=========================================================================
//   Creation d un cercle Passant par : 2 points Point1 et Point2.        +
//                        Centre sur  : 1 droite OnLine.                  +
//   avec une Tolerance de precision  : Tolerance.                        +
//                                                                        +
//   On cree L1 la droite des points equidistant de Point1 et Point2.     +
//   On cree alors la solutions cirsol telle que :                        +
//   cirsol est l ensemble des cercle ayant pour centre l intersections   +
//   de L1 avec OnLine et de rayon la distance entre Point1 et le point   +
//   ci dessus calcule.                                                   +
//=========================================================================

GccAna_Circ2d2TanOn::
   GccAna_Circ2d2TanOn (const gp_Pnt2d&      Point1     , 
                        const gp_Pnt2d&      Point2     ,
                        const gp_Lin2d&      OnLine     ,
                        const Standard_Real  Tolerance  ):
   cirsol(1,2)      ,
   qualifier1(1,2)  ,
   qualifier2(1,2),
   TheSame1(1,2)  ,
   TheSame2(1,2)  ,
   pnttg1sol(1,2)   ,
   pnttg2sol(1,2)   ,
   pntcen(1,2)      ,
   par1sol(1,2)     ,
   par2sol(1,2)     ,
   pararg1(1,2)     ,
   pararg2(1,2)     ,
   parcen3(1,2)     
{
  TheSame1.Init(0);
  TheSame2.Init(0);
  WellDone = Standard_False;
  NbrSol = 0;
  
  gp_Dir2d dirx(1.,0.);
  Standard_Real dist   = Point1.Distance(Point2);
  if (dist < Abs(Tolerance)) { WellDone = Standard_True; }
  else {
    gp_Lin2d L1(gp_Pnt2d((Point1.XY()+Point2.XY())/2.0),
		gp_Dir2d(Point1.Y()-Point2.Y(),Point2.X()-Point1.X()));
    IntAna2d_AnaIntersection Intp(L1,OnLine);
    if (Intp.IsDone()) {
      if (!Intp.IsEmpty()) {
	for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
	  NbrSol++;
	  gp_Ax2d axe(Intp.Point(i).Value(),dirx);
	  cirsol(NbrSol)=gp_Circ2d(axe,Point1.Distance(Intp.Point(i).Value()));
//        ====================================================================
	  qualifier1(NbrSol) = GccEnt_noqualifier;
	  qualifier2(NbrSol) = GccEnt_noqualifier;
	  pnttg1sol(NbrSol) = Point1;
	  pnttg2sol(NbrSol) = Point2;
	  pntcen(NbrSol) = cirsol(NbrSol).Location();
	  pararg1(NbrSol) = 0.;
	  pararg2(NbrSol) = 0.;
	  par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),pnttg1sol(NbrSol));
	  par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),pnttg2sol(NbrSol));
	  parcen3(NbrSol)=ElCLib::Parameter(OnLine,pntcen(NbrSol));
	}
      }
      WellDone = Standard_True;
    }
  }
}






