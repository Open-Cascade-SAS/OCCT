// File:	GccAna_Circ2d2TanOn_6.cxx
// Created:	Thu Jan  2 15:56:04 1992
// Author:	Remi GILET
//		<reg@topsn3>

#include <GccAna_Circ2d2TanOn.jxx>

#include <ElCLib.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Ax2d.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <GccAna_Circ2dBisec.hxx>
#include <GccInt_IType.hxx>
#include <GccInt_BCirc.hxx>
#include <GccInt_BLine.hxx>
#include <IntAna2d_Conic.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <GccEnt_BadQualifier.hxx>

//=========================================================================
//   Creation d un cercle tangent a deux cercle C1 et C2.                 +
//                        centre sur un cercle.                           +
//  Nous commencons par distinguer les differents cas limites que nous    +
//  allons traiter separement.                                            +
//  Pour le cas general:                                                  +
//  ====================                                                  +
//  Nous calculons les bissectrices a C1 et C2 qui nous donnent           +
//  l ensemble des lieux possibles des centres de tous les cercles        +
//  tangents aC1 et C2.                                                   +
//  Nous intersectons ces bissectrices avec le cercle OnCirc ce qui nous  +
//  donne les points parmis lesquels nous allons choisir les solutions.   +
//  Les choix s effectuent a partir des Qualifieurs qualifiant C1 et C2.  +
//=========================================================================

GccAna_Circ2d2TanOn::
   GccAna_Circ2d2TanOn (const GccEnt_QualifiedCirc& Qualified1 ,
                        const GccEnt_QualifiedCirc& Qualified2 ,
                        const gp_Circ2d&            OnCirc     ,
                        const Standard_Real         Tolerance  ):
   cirsol(1,8)     ,
   qualifier1(1,8) ,
   qualifier2(1,8) ,
   TheSame1(1,8) ,
   TheSame2(1,8) ,
   pnttg1sol(1,8)  ,
   pnttg2sol(1,8)  ,
   pntcen(1,8)     ,
   par1sol(1,8)    ,
   par2sol(1,8)    ,
   pararg1(1,8)    ,
   pararg2(1,8)    ,
   parcen3(1,8)    
{
  TheSame1.Init(0);
  TheSame2.Init(0);
  WellDone = Standard_False;
  NbrSol = 0;
  if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
	Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
      !(Qualified2.IsEnclosed() || Qualified2.IsEnclosing() || 
	Qualified2.IsOutside() || Qualified2.IsUnqualified())) {
    GccEnt_BadQualifier::Raise();
    return;
  }
  Standard_Real Tol= Abs(Tolerance);
  gp_Circ2d C1 = Qualified1.Qualified();
  gp_Circ2d C2 = Qualified2.Qualified();
  gp_Dir2d dirx(1.,0.);
  TColStd_Array1OfReal Radius(1,2);
  TColStd_Array1OfReal Rradius(1,2);
  gp_Pnt2d center1(C1.Location());
  gp_Pnt2d center2(C2.Location());
#ifdef DEB
  Standard_Real distance = center1.Distance(center2);
#else
  center1.Distance(center2);
#endif
  Standard_Real R1 = C1.Radius();
  Standard_Real R2 = C2.Radius();

//=========================================================================
//   Traitement des cas limites.                                          +
//=========================================================================

  Standard_Integer nbsol1 = 1;
  Standard_Integer nbsol2 = 0;
  Standard_Real Ron = OnCirc.Radius();
  Standard_Real distcco = OnCirc.Location().Distance(center1);
  gp_Dir2d dircc(OnCirc.Location().XY()-center1.XY());
  gp_Pnt2d pinterm(center1.XY()+(distcco-Ron)*dircc.XY());
  Standard_Real distcc2 =pinterm.Distance(center2);
  Standard_Real distcc1 =pinterm.Distance(center1);
  Standard_Real d1 = Abs(distcc2-R2-Abs(distcc1-R1));
  Standard_Real d2 = Abs(distcc2+R2-Abs(distcc1-R1));
  Standard_Real d3 = Abs(distcc2-R2-(distcc1+R1));
  Standard_Real d4 = Abs(distcc2+R2-(distcc1+R1));
  if ( d1 > Tol || d2 > Tol || d3 > Tol || d4 > Tol) {
    pinterm = gp_Pnt2d(center1.XY()+(distcco+Ron)*dircc.XY());
    distcc2 =pinterm.Distance(center2);
    distcc1 =pinterm.Distance(center1);
    d1 = Abs(distcc2-R2-Abs(distcc1-R1));
    d2 = Abs(distcc2+R2-Abs(distcc1-R1));
    d3 = Abs(distcc2-R2-(distcc1+R1));
    d4 = Abs(distcc2+R2-(distcc1+R1));
    if ( d1 > Tol || d2 > Tol || d3 > Tol || d4 > Tol) { nbsol1 = 0; }
  }
  if (nbsol1 > 0) {
    if (Qualified1.IsEnclosed() || Qualified1.IsOutside()) {
      nbsol1 = 1;
      Radius(1) = Abs(distcc1-R1);
    }
    else if (Qualified1.IsEnclosing()) {
      nbsol1 = 1;
      Radius(1) = R1+distcc1;
    }
    else if (Qualified1.IsUnqualified()) {
      nbsol1 = 2;
      Radius(1) = Abs(distcc1-R1);
      Radius(2) = R1+distcc1;
    }
    if (Qualified2.IsEnclosed() || Qualified2.IsOutside()) {
      nbsol2 = 1;
      Rradius(1) = Abs(distcc2-R2);
    }
    else if (Qualified2.IsEnclosing()) {
      nbsol2 = 1;
      Rradius(1) = R2+distcc2;
    }
    else if (Qualified2.IsUnqualified()) {
      nbsol2 = 2;
      Rradius(1) = Abs(distcc2-R2);
      Rradius(2) = R2+distcc2;
    }
    for (Standard_Integer i = 1 ; i <= nbsol1 ; i++) {
      for (Standard_Integer j = 1 ; j <= nbsol2 ; j++) {
	if (Abs(Radius(i)-Rradius(j)) <= Tol) {
	  WellDone = Standard_True;
	  NbrSol++;
	  cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(pinterm,dirx),Radius(i));
//        ===========================================================
	  gp_Dir2d dc1(center1.XY()-pinterm.XY());
	  gp_Dir2d dc2(center2.XY()-pinterm.XY());
	  distcc1 = pinterm.Distance(center1);
	  distcc2 = pinterm.Distance(center2);
	  if (!Qualified1.IsUnqualified()) { 
	    qualifier1(NbrSol) = Qualified1.Qualifier();
	  }
	  else if (Abs(distcc1+Radius(i)-R1) < Tol) {
	    qualifier1(NbrSol) = GccEnt_enclosed;
	  }
	  else if (Abs(distcc1-R1-Radius(i)) < Tol) {
	    qualifier1(NbrSol) = GccEnt_outside;
	  }
	  else { qualifier1(NbrSol) = GccEnt_enclosing; }
	  if (!Qualified2.IsUnqualified()) { 
	    qualifier2(NbrSol) = Qualified2.Qualifier();
	  }
	  else if (Abs(distcc2+Radius(i)-R2) < Tol) {
	    qualifier2(NbrSol) = GccEnt_enclosed;
	  }
	  else if (Abs(distcc2-R2-Radius(i)) < Tol) {
	    qualifier2(NbrSol) = GccEnt_outside;
	  }
	  else { qualifier2(NbrSol) = GccEnt_enclosing; }
	  pnttg1sol(NbrSol) = gp_Pnt2d(pinterm.XY()+Radius(i)*dc1.XY());
	  pnttg2sol(NbrSol) = gp_Pnt2d(pinterm.XY()+Radius(i)*dc2.XY());
	  pntcen(NbrSol) = cirsol(NbrSol).Location();
	  par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),pnttg1sol(NbrSol));
	  pararg1(NbrSol)=ElCLib::Parameter(C1,pnttg1sol(NbrSol));
	  par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),pnttg2sol(NbrSol));
	  pararg2(NbrSol)=ElCLib::Parameter(C2,pnttg2sol(NbrSol));
	  parcen3(NbrSol)=ElCLib::Parameter(OnCirc,pntcen(NbrSol));
	}
      }
    }
    if (WellDone) { return; }
  }
  
//=========================================================================
//   Cas general.                                                         +
//=========================================================================

  GccAna_Circ2dBisec Bis(C1,C2);
  if (Bis.IsDone()) {
    TColStd_Array1OfReal Rbid(1,2);
    TColStd_Array1OfReal RBid(1,2);
    Standard_Integer nbsolution = Bis.NbSolutions();
    for (Standard_Integer i = 1 ; i <=  nbsolution ; i++) {
      Handle(GccInt_Bisec) Sol = Bis.ThisSolution(i);
      GccInt_IType typ = Sol->ArcType();
      IntAna2d_AnaIntersection Intp;
      if (typ == GccInt_Cir) {
	Intp.Perform(OnCirc,Sol->Circle());
      }
      else if (typ == GccInt_Lin) {
	Intp.Perform(Sol->Line(),OnCirc);
      }
      else if (typ == GccInt_Hpr) {
	Intp.Perform(OnCirc,IntAna2d_Conic(Sol->Hyperbola()));
      }
      else if (typ == GccInt_Ell) {
	Intp.Perform(OnCirc,IntAna2d_Conic(Sol->Ellipse()));
      }
      if (Intp.IsDone()) {
	if ((!Intp.IsEmpty())&&(!Intp.ParallelElements())&&
	    (!Intp.IdenticalElements())) {
	  for (Standard_Integer j = 1 ; j <= Intp.NbPoints() ; j++) {
	    gp_Pnt2d Center(Intp.Point(j).Value());
	    Standard_Real dist1 = Center.Distance(center1);
	    Standard_Real dist2 = Center.Distance(center2);
	    Standard_Integer nbsol = 0;
	    Standard_Integer nsol = 0;
	    Standard_Integer nnsol = 0;
	    R1 = C1.Radius();
	    R2 = C2.Radius();
	    if (Qualified1.IsEnclosed()) {
	      if (dist1-R1 < Tol) { 
		nbsol = 1;
		Rbid(1) = Abs(R1-dist1);
	      }
	    }
	    else if (Qualified1.IsOutside()) {
	      if (R1-dist1 < Tol) { 
		nbsol = 1;
		Rbid(1) = Abs(dist1-R1);
	      }
	    }
	    else if (Qualified1.IsEnclosing()) {
	      nbsol = 1;
	      Rbid(1) = dist1+R1;
	    }
	    else if (Qualified1.IsUnqualified()) {
	      nbsol = 2;
	      Rbid(1) = dist1+R1;
	      Rbid(1) = Abs(dist1-R1);
	    }
	    if (Qualified2.IsEnclosed() && nbsol != 0) {
	      if (dist2-R2 < Tol) {
		nsol = 1;
		RBid(1) = Abs(R2-dist2);
	      }
	    }
	    else if (Qualified2.IsOutside() && nbsol != 0) {
	      if (R2-dist2 < Tol) {
		nsol = 1;
		RBid(1) = Abs(R2-dist2);
	      }
	    }
	    else if (Qualified2.IsEnclosing() && nbsol != 0) {
	      nsol = 1;
	      RBid(1) = dist2+R2;
	    }
	    else if (Qualified2.IsUnqualified() && nbsol != 0) {
	      nsol = 2;
	      RBid(1) = dist2+R2;
	      RBid(2) = Abs(R2-dist2);
	    }
	    for (Standard_Integer isol = 1; isol <= nbsol ; isol++) {
	      for (Standard_Integer jsol = 1; jsol <= nsol ; jsol++) {
		if (Abs(Rbid(isol)-RBid(jsol)) <= Tol) {
		  nnsol++;
		  Radius(nnsol) = (RBid(jsol)+Rbid(isol))/2.;
		}
	      }
	    }
	    if (nnsol > 0) {
	      for (Standard_Integer k = 1 ; k <= nnsol ; k++) {
		NbrSol++;
		cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius(k));
//              ==========================================================
		distcc1 = Center.Distance(center1);
		distcc2 = Center.Distance(center2);
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
		if (!Qualified2.IsUnqualified()) { 
		  qualifier2(NbrSol) = Qualified2.Qualifier();
		}
		else if (Abs(distcc2+Radius(k)-R2) < Tol) {
		  qualifier2(NbrSol) = GccEnt_enclosed;
		}
		else if (Abs(distcc2-R2-Radius(k)) < Tol) {
		  qualifier2(NbrSol) = GccEnt_outside;
		}
		else { qualifier2(NbrSol) = GccEnt_enclosing; }
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
		  pararg1(NbrSol)=ElCLib::Parameter(C1,pnttg1sol(NbrSol));
		}
		if (Center.Distance(center2) <= Tolerance &&
		    Abs(Radius(k)-C2.Radius()) <= Tolerance) {
		  TheSame2(NbrSol) = 1;
		}
		else {
		  TheSame2(NbrSol) = 0;
		  gp_Dir2d dc2(center2.XY()-Center.XY());
		  pnttg2sol(NbrSol)=gp_Pnt2d(Center.XY()+Radius(k)*dc2.XY());
		  par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
						    pnttg2sol(NbrSol));
		  pararg2(NbrSol)=ElCLib::Parameter(C2,pnttg2sol(NbrSol));
		}
		pntcen(NbrSol) = Center;
		parcen3(NbrSol)=ElCLib::Parameter(OnCirc,pntcen(NbrSol));
	      }
	    }
	  }
	}
	WellDone = Standard_True;
      }
    }
  }
}

