// File:	GccAna_Circ2d2TanOn_10.cxx
// Created:	Thu Jan  2 16:00:00 1992
// Author:	Remi GILET
//		<reg@topsn3>

#include <GccAna_Circ2d2TanOn.jxx>

#include <ElCLib.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Ax2d.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <GccAna_LinPnt2dBisec.hxx>
#include <GccInt_IType.hxx>
#include <GccInt_Bisec.hxx>
#include <GccInt_BLine.hxx>
#include <IntAna2d_Conic.hxx>
#include <GccEnt_BadQualifier.hxx>

//=========================================================================
//   Creation d un cercle tangent a une Droite L1 et a un point Point2.   +
//                        centre sur un cercle.                           +
//  Nous commencons par distinguer les differents cas limites que nous    +
//  allons traiter separement.                                            +
//  Pour le cas general:                                                  +
//  ====================                                                  +
//  Nous calculons les bissectrices a L1 et Point2 qui nous donnent       +
//  l ensemble des lieux possibles des centres de tous les cercles        +
//  tangents a L1 et Point2.                                              +
//  Nous intersectons ces bissectrices avec le cerclee OnCirc ce qui nous +
//  donne les points parmis lesquels nous allons choisir les solutions.   +
//  Les choix s effectuent a partir des Qualifieurs qualifiant L1.        +
//=========================================================================

  GccAna_Circ2d2TanOn::
   GccAna_Circ2d2TanOn (const GccEnt_QualifiedLin&  Qualified1 ,
                        const gp_Pnt2d&             Point2     ,
                        const gp_Circ2d&            OnCirc     ,
                        const Standard_Real         Tolerance  ):
   cirsol(1,4)     ,
   qualifier1(1,4) ,
   qualifier2(1,4) ,
   TheSame1(1,4) ,
   TheSame2(1,4) ,
   pnttg1sol(1,4)  ,
   pnttg2sol(1,4)  ,
   pntcen(1,4)     ,
   par1sol(1,4)    ,
   par2sol(1,4)    ,
   pararg1(1,4)    ,
   pararg2(1,4)    ,
   parcen3(1,4)
{

  TheSame1.Init(0);
  TheSame2.Init(0);
  WellDone = Standard_False;
  NbrSol = 0;
  if (!(Qualified1.IsEnclosed() ||
	Qualified1.IsOutside() || Qualified1.IsUnqualified())) {
  GccEnt_BadQualifier::Raise();
    return;
  }
  Standard_Real Tol = Abs(Tolerance);
  gp_Dir2d dirx(1.,0.);
  gp_Lin2d L1 = Qualified1.Qualified();
  gp_Pnt2d originL1(L1.Location());
  gp_Dir2d dirL1(L1.Direction());
  gp_Dir2d normL1(-dirL1.Y(),dirL1.X());

//=========================================================================
//   Traitement des cas limites.                                          +
//=========================================================================

  Standard_Real Ron = OnCirc.Radius();
  Standard_Real distpc = OnCirc.Location().Distance(Point2);
  gp_Dir2d dir(OnCirc.Location().XY()-Point2.XY());
  gp_Pnt2d pinterm(Point2.XY()+(distpc+Ron)*dir.XY());
  Standard_Real dist1 = L1.Distance(pinterm);
  if (Abs(dist1-distpc-Ron) > Tol) {
#ifdef DEB
    gp_Pnt2d pinterm(Point2.XY()+(distpc-Ron)*dir.XY()); // Unused ! BUG ???
    Standard_Real dist1 = L1.Distance(pinterm); // Unused ! BUG ???
#endif
  }
  if (Abs(dist1-distpc+Ron) <= Tol) {
    dir = gp_Dir2d(-dirL1.Y(),dirL1.X());
    gp_Dir2d direc(originL1.XY()-pinterm.XY());
    if (Qualified1.IsOutside()) {
      if (direc.Dot(dir) >= 0.0) { WellDone = Standard_True; }
    }
    else if (Qualified1.IsEnclosed()) {
      if (direc.Dot(dir) <= 0.0) { WellDone = Standard_True; }
    }
    else { WellDone = Standard_True; }
    if (WellDone) {
      NbrSol++;
      cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(pinterm,dirx),dist1);
//    =======================================================
      gp_Dir2d dc1(originL1.XY()-pinterm.XY());
      Standard_Real sign = dc1.Dot(normL1);
      if (!Qualified1.IsUnqualified()) { 
	qualifier1(NbrSol) = Qualified1.Qualifier();
      }
      else if (dc1.Dot(normL1) > 0.0) {	qualifier1(NbrSol) = GccEnt_outside; }
      else { qualifier1(NbrSol) = GccEnt_enclosed; }
      qualifier2(NbrSol) = GccEnt_noqualifier;
      dc1 = gp_Dir2d(sign*gp_XY(-dirL1.Y(),dirL1.X()));
      pnttg1sol(NbrSol) = gp_Pnt2d(pinterm.XY()+dist1*dc1.XY());
      par1sol(NbrSol) = ElCLib::Parameter(cirsol(NbrSol),pnttg1sol(NbrSol));
      pararg1(NbrSol)=ElCLib::Parameter(L1,pnttg1sol(NbrSol));
      pntcen(NbrSol) = pinterm;
      parcen3(NbrSol)=ElCLib::Parameter(OnCirc,pntcen(NbrSol));
      parcen3(NbrSol) = 0.;
      pnttg2sol(NbrSol) = Point2;
      pararg2(NbrSol) = 0.;
      par2sol(NbrSol) = ElCLib::Parameter(cirsol(NbrSol),pnttg2sol(NbrSol));
      return;
    }
  }

//=========================================================================
//   Cas general.                                                         +
//=========================================================================

  GccAna_LinPnt2dBisec Bis(L1,Point2);
  if (Bis.IsDone()) {
    Handle(GccInt_Bisec) Sol = Bis.ThisSolution();
    GccInt_IType type = Sol->ArcType();
    IntAna2d_AnaIntersection Intp;
    if (type == GccInt_Lin) {
      Intp.Perform(Sol->Line(),OnCirc);
    }
    if (type == GccInt_Par) {
      Intp.Perform(OnCirc,IntAna2d_Conic(Sol->Parabola()));
    }
    if (Intp.IsDone()) {
      if (!Intp.IsEmpty()) {
	for (Standard_Integer j = 1 ; j <= Intp.NbPoints() ; j++) {
	  gp_Pnt2d Center(Intp.Point(j).Value());
	  Standard_Real Radius = L1.Distance(Center);
//	  Standard_Integer nbsol = 1;
	  Standard_Boolean ok = Standard_False;
	  if (Qualified1.IsEnclosed()) {
	    if ((((originL1.X()-Center.X())*(-dirL1.Y()))+
		 ((originL1.Y()-Center.Y())*(dirL1.X())))<=0){
	      ok = Standard_True;
	    }
	  }
	  else if (Qualified1.IsOutside()) {
	    if ((((originL1.X()-Center.X())*(-dirL1.Y()))+
		 ((originL1.Y()-Center.Y())*(dirL1.X())))>=0){
	      ok = Standard_True;
	    }
	  }
	  else if (Qualified1.IsUnqualified()) {
	    ok = Standard_True;
	  }
	  if (ok) {
	    NbrSol++;
	    cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius);
//          =======================================================
	    TheSame1(NbrSol) = 0;
	    TheSame2(NbrSol) = 0;
	    gp_Dir2d dc1(originL1.XY()-Center.XY());
	    Standard_Real sign = dc1.Dot(normL1);
	    if (!Qualified1.IsUnqualified()) { 
	      qualifier1(NbrSol) = Qualified1.Qualifier();
	    }
	    else if (dc1.Dot(normL1) > 0.0) {	
	      qualifier1(NbrSol) = GccEnt_outside; 
	    }
	    else { qualifier1(NbrSol) = GccEnt_enclosed; }
	    qualifier2(NbrSol) = GccEnt_noqualifier;
	    dc1=gp_Dir2d(sign*gp_XY(-dirL1.Y(),dirL1.X()));
	    pnttg1sol(NbrSol) = gp_Pnt2d(Center.XY()+dist1*dc1.XY());
	    par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
					      pnttg1sol(NbrSol));
	    pararg1(NbrSol)=ElCLib::Parameter(L1,pnttg1sol(NbrSol));
	    pntcen(NbrSol) = Center;
	    parcen3(NbrSol) = ElCLib::Parameter(OnCirc,pntcen(NbrSol));
	    pnttg2sol(NbrSol) = Point2;
	    pararg2(NbrSol) = 0.;
	    par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
					      pnttg2sol(NbrSol));
	    qualifier2(NbrSol) = GccEnt_noqualifier;
	   }
	}
      }
      WellDone = Standard_True;
    }
  }
}

