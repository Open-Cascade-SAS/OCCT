// File:	GccAna_Circ2d2TanOn_3.cxx
// Created:	Thu Jan  2 15:53:42 1992
// Author:	Remi GILET
//		<reg@topsn3>

#include <GccAna_Circ2d2TanOn.jxx>

#include <ElCLib.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Ax2d.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <GccInt_IType.hxx>
#include <GccInt_Bisec.hxx>
#include <GccInt_BLine.hxx>
#include <GccInt_BCirc.hxx>
#include <IntAna2d_Conic.hxx>
#include <GccAna_CircPnt2dBisec.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <GccEnt_BadQualifier.hxx>

//=========================================================================
//  Cercles tangents un cercle C1, passant par un point Point2 et centres +
//  sur une droite OnLine.                                                +
//  Nous commencons par distinguer les differents cas limites que nous    +
//  allons traiter separement.                                            +
//  Pour le cas general:                                                  +
//  ====================                                                  +
//  Nous calculons les bissectrices a C1 et Point2 qui nous donnent       +
//  l ensemble des lieux possibles des centres de tous les cercles        +
//  tangents a C1 et passant par Point2.                                  +
//  Nous intersectons ces bissectrices avec la droite OnLine ce qui nous  +
//  donne les points parmis lesquels nous allons choisir les solutions.   +
//  Les choix s effectuent a partir des Qualifieurs qualifiant C1 et C2.  +
//=========================================================================

GccAna_Circ2d2TanOn::
   GccAna_Circ2d2TanOn (const GccEnt_QualifiedCirc& Qualified1 ,
                        const gp_Pnt2d&             Point2     ,
                        const gp_Lin2d&             OnLine     ,
                        const Standard_Real         Tolerance  ):
   cirsol(1,4),
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
  Standard_Real Tol = Abs(Tolerance);
  WellDone = Standard_False;
  NbrSol = 0;
  if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
	Qualified1.IsOutside() || Qualified1.IsUnqualified())) {
    GccEnt_BadQualifier::Raise();
    return;
  }
  TColStd_Array1OfReal Radius(1,2);
  gp_Dir2d dirx(1.,0.);
  gp_Circ2d C1 = Qualified1.Qualified();
  Standard_Real R1 = C1.Radius();
  gp_Pnt2d center1(C1.Location());
  
//=========================================================================
//   Traitement des cas limites.                                          +
//=========================================================================

  Standard_Real dp2l = OnLine.Distance(Point2);
  gp_Dir2d donline(OnLine.Direction());
  gp_Pnt2d pinterm(Point2.XY()+dp2l*gp_XY(-donline.Y(),donline.X()));
  if (OnLine.Distance(pinterm) > Tol) {
    pinterm = gp_Pnt2d(Point2.XY()-dp2l*gp_XY(-donline.Y(),donline.X()));
  }
  Standard_Real dist = pinterm.Distance(center1);
  if (Qualified1.IsEnclosed() && Abs(R1-dist-dp2l) <= Tol) {
    WellDone = Standard_True;
  }
  else if (Qualified1.IsEnclosing() && Abs(R1+dist-dp2l) <= Tol) {
    WellDone = Standard_True;
   }
  else if (Qualified1.IsOutside() && Abs(dist-dp2l) <= Tol) {
    WellDone = Standard_True;
   }
  else if (Qualified1.IsUnqualified() && 
	   (Abs(dist-dp2l) <= Tol || Abs(R1-dist-dp2l) <= Tol ||
	    Abs(R1+dist-dp2l) <= Tol)) {
    WellDone = Standard_True;
  }
  if (WellDone) {
    NbrSol++;
    cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(pinterm,dirx),dp2l);
//  ======================================================
    gp_Dir2d dc1(center1.XY()-pinterm.XY());
    Standard_Real distcc1 = pinterm.Distance(center1);
    if (!Qualified1.IsUnqualified()) { 
      qualifier1(NbrSol) = Qualified1.Qualifier();
    }
    else if (Abs(distcc1+dp2l-R1) < Tol) {
      qualifier1(NbrSol) = GccEnt_enclosed;
    }
    else if (Abs(distcc1-R1-dp2l) < Tol) {
      qualifier1(NbrSol) = GccEnt_outside;
    }
    else { qualifier1(NbrSol) = GccEnt_enclosing; }
    qualifier2(NbrSol) = GccEnt_noqualifier;
    pnttg1sol(NbrSol) = gp_Pnt2d(pinterm.XY()+dp2l*dc1.XY());
    par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),pnttg1sol(NbrSol));
    pararg1(NbrSol)=ElCLib::Parameter(C1,pnttg1sol(NbrSol));
    pnttg2sol(NbrSol) = Point2;
    par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),pnttg2sol(NbrSol));
    pntcen(NbrSol) = cirsol(NbrSol).Location();
    parcen3(NbrSol)=ElCLib::Parameter(OnLine,pntcen(NbrSol));
    return;
  }

//=========================================================================
//   cas general.                                                         +
//=========================================================================

  GccAna_CircPnt2dBisec Bis(C1,Point2);
  if (Bis.IsDone()) {
    Standard_Integer nbsolution = Bis.NbSolutions();
    for (Standard_Integer i = 1 ; i <=  nbsolution; i++) {
      Handle(GccInt_Bisec) Sol = Bis.ThisSolution(i);
      GccInt_IType type = Sol->ArcType();
      IntAna2d_AnaIntersection Intp;
      if (type == GccInt_Lin) {
	Intp.Perform(OnLine,Sol->Line());
      }
      else if (type == GccInt_Cir) {
	Intp.Perform(OnLine,Sol->Circle());
      }
      else if (type == GccInt_Ell) {
	Intp.Perform(OnLine,IntAna2d_Conic(Sol->Ellipse()));
      }
      else if (type == GccInt_Hpr) {
	Intp.Perform(OnLine,IntAna2d_Conic(Sol->Hyperbola()));
      }
      if (Intp.IsDone()) {
	if (!Intp.IsEmpty()) {
	  for (Standard_Integer j = 1 ; j <= Intp.NbPoints() ; j++) {
	    gp_Pnt2d Center(Intp.Point(j).Value());
	    Standard_Real dist1 = center1.Distance(Center);
	    Standard_Integer nbsol = 1;
	    Standard_Boolean ok = Standard_False;
	    if (Qualified1.IsEnclosed()) {
	      if (dist1-C1.Radius() <= Tolerance) {
		ok = Standard_True;
		Radius(1) = Abs(C1.Radius()-dist1);
	      }
	    }
	    else if (Qualified1.IsOutside()) {
	      if (C1.Radius()-dist1 <= Tolerance) {
		ok = Standard_True;
		Radius(1) = Abs(C1.Radius()-dist1);
	      }
	    }
	    else if (Qualified1.IsEnclosing()) {
	      ok = Standard_True;
	      Radius(1) = C1.Radius()+dist1;
	    }
/*	    else if (Qualified1.IsUnqualified() && ok) {
	      ok = Standard_True;
	      nbsol = 2;
	      Radius(1) = Abs(C1.Radius()-dist1);
	      Radius(2) = C1.Radius()+dist1;
	    }
*/
	    else if (Qualified1.IsUnqualified() ) {
	      Standard_Real popradius = Center.Distance(Point2);
	      if (Abs(popradius-dist1)) {
		ok = Standard_True;
		Radius(1) = popradius;
	      } 
	    }	
		
	    if (ok) {
	      for (Standard_Integer k = 1 ; k <= nbsol ; k++) {
		NbrSol++;
		cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius(k));
//              ==========================================================
		Standard_Real distcc1 = Center.Distance(center1);
		if (!Qualified1.IsUnqualified()) { 
		  qualifier1(NbrSol) = Qualified1.Qualifier();
		}
		else if (Abs(distcc1+Radius(k)-R1) < Tol) {
		  qualifier1(NbrSol) = GccEnt_enclosed;
		}
		else if (Abs(distcc1-R1-Radius(k)) < Tol) {
		  qualifier1(NbrSol) = GccEnt_outside;
		}
		else { qualifier1(NbrSol) = GccEnt_enclosing; }
		qualifier2(NbrSol) = GccEnt_noqualifier;
		if (Center.Distance(center1) <= Tolerance &&
		    Abs(Radius(k)-C1.Radius()) <= Tolerance) {
		  TheSame1(NbrSol) = 1;
		}
		else {
		  TheSame1(NbrSol) = 0;
		  gp_Dir2d dc1(center1.XY()-Center.XY());
		  pnttg1sol(NbrSol)=gp_Pnt2d(Center.XY()+Radius(k)*dc1.XY());
		  par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
						    pnttg1sol(NbrSol));
		  pararg1(i)=ElCLib::Parameter(C1,pnttg1sol(NbrSol));
		}
		TheSame2(NbrSol) = 0;
		pnttg2sol(NbrSol) = Point2;
		par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
						  pnttg2sol(NbrSol));
		pararg2(NbrSol)=0.;
		pntcen(NbrSol) = Center;
		parcen3(NbrSol)=ElCLib::Parameter(OnLine,pntcen(NbrSol));
	      }
	    }
	  }
	}
	WellDone = Standard_True;
      }
    }
  }
}
