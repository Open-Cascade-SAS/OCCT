// File:	GccAna_Circ2d2TanRad_2.cxx
// Created:	Tue Sep 24 09:12:49 1991
// Author:	Remi GILET
//		<reg@topsn2>

#include <GccAna_Circ2d2TanRad.jxx>

#include <gp_Circ2d.hxx>
#include <ElCLib.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <Standard_NegativeValue.hxx>
#include <GccEnt_BadQualifier.hxx>

// circulaire tangent a un cercle et un point et de rayon donne
//=============================================================

//========================================================================
// On initialise WellDone a false.                                       +
// On recupere le cercle C1.                                             +
// On sort en erreur dans les cas ou la construction est impossible.     +
// On fait la parallele a C1 dans le bon sens.                           +
// On fait le cercle centre en Point1 de rayon Radius.                   +
// On intersecte la parallele et le cercle.                              +
//                              ==> Le point de centre de la solution.   +
// On cree la solution qu on ajoute aux solutions deja trouvees.         +
// On remplit les champs.                                                +
//========================================================================

GccAna_Circ2d2TanRad::
   GccAna_Circ2d2TanRad (const GccEnt_QualifiedCirc& Qualified1 ,
                         const gp_Pnt2d&             Point2     ,
                         const Standard_Real         Radius     ,
                         const Standard_Real         Tolerance  ):
   qualifier1(1,4) ,
   qualifier2(1,4),
   TheSame1(1,4)   ,
   TheSame2(1,4)   ,
   cirsol(1,4)     ,
   pnttg1sol(1,4)  ,
   pnttg2sol(1,4)  ,
   par1sol(1,4)    ,
   par2sol(1,4)    ,
   pararg1(1,4)    ,
   pararg2(1,4)    
{
     
  gp_Dir2d dirx(1.0,0.0);
  Standard_Real Tol = Abs(Tolerance);
  NbrSol = 0;
  WellDone = Standard_False;
  if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
	Qualified1.IsOutside() || Qualified1.IsUnqualified())) {
    GccEnt_BadQualifier::Raise();
    return;
  }
  Standard_Integer i ;
  for ( i = 1 ; i <= 4 ; i++) {
    TheSame1(i) = 0;
    TheSame2(i) = 0;
  }
  Standard_Real deport = 0.;
  Standard_Integer signe = 0;
  Standard_Integer nbsol = 0;
  gp_Circ2d C1 = Qualified1.Qualified();
  TColgp_Array1OfCirc2d C(1,4);
  Standard_Real R1 = C1.Radius();
  Standard_Real distance = (C1.Location()).Distance(Point2);
  Standard_Real dispc1 = C1.Distance(Point2);
  gp_Dir2d dir1(Point2.XY()-(C1.Location().XY()));
  gp_Pnt2d center1(C1.Location());
  if (Radius < 0.0) { Standard_NegativeValue::Raise(); }
  else {
    if ( dispc1-Radius*2.0 > Tol) { WellDone = Standard_True; }
    else if (Qualified1.IsEnclosed()) {
//  =================================
      if ((distance-R1>Tol)||(Radius-R1>Tol)) { WellDone = Standard_True; }
      else {
	if (Abs(distance-R1) < Tol) {
	  nbsol = -1;
	  deport = R1-Radius;
	  signe = 1;
	}
	else {
	  C(1) = gp_Circ2d(C1.XAxis(),Abs(Radius-R1));
	  C(2) = gp_Circ2d(gp_Ax2d(Point2,dirx),Radius);
	  nbsol = 1;
	}
      }
    }
    else if (Qualified1.IsEnclosing()) {
//  ==================================
      if ((Tol<R1-distance)||(Tol<R1-Radius)) { WellDone = Standard_True; }
      else {
	if (Abs(distance-R1) < Tol) {
	  nbsol = -1;
	  deport = R1-Radius;
	  signe = 1;
	}
	else {
	  C(1) = gp_Circ2d(C1.XAxis(),Abs(Radius-R1));
	  C(2) = gp_Circ2d(gp_Ax2d(Point2,dirx),Radius);
	  nbsol = 1;
	}
      }
    }
    else if (Qualified1.IsOutside()) {
//  ================================
      if (Tol < R1-distance) { WellDone = Standard_True; }
      else if ((Abs(distance-R1) < Tol) || (Abs(dispc1-Radius*2.0) < Tol)) {
	nbsol = -1;
	deport = R1+Radius;
	signe = -1;
      }
      else {
	C(1) = gp_Circ2d(C1.XAxis(),Radius+R1);
	C(2) = gp_Circ2d(gp_Ax2d(Point2,dirx),Radius);
	nbsol = 1;
      }
    }
    else if (Qualified1.IsUnqualified()) {
//  ====================================
      if (Abs(dispc1-Radius*2.0) < Tol) {
	WellDone = Standard_True;
	gp_Pnt2d Center(center1.XY()+(distance-Radius)*dir1.XY());
	cirsol(1) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius);
//      ==================================================
	if (Abs(Center.Distance(center1)-R1) < Tol) {
	  qualifier1(1) = GccEnt_enclosed;
	}
	else { qualifier1(1) = GccEnt_outside; }
	qualifier2(1) = GccEnt_noqualifier;
	pnttg1sol(1) = gp_Pnt2d(Center.XY()-Radius*dir1.XY());
	pnttg2sol(1) = Point2;
	WellDone = Standard_True;
	NbrSol = 1;
      }
      else if ((Abs(R1-Radius)<Tol) && (Abs(distance-R1)<Tol)){
	cirsol(1) = gp_Circ2d(C1);
//      =========================
	qualifier1(1) = GccEnt_unqualified;
	qualifier2(1) = GccEnt_noqualifier;
	TheSame1(1) = 1;
	pnttg2sol(1) = Point2;
	WellDone = Standard_True;
	NbrSol = 1;
	C(1) = gp_Circ2d(C1.XAxis(),Radius+R1);
	C(2) = gp_Circ2d(gp_Ax2d(Point2,dirx),Radius);
	nbsol = 1;
      }
      else {
	C(1) = gp_Circ2d(C1.XAxis(),Abs(Radius-R1));
	C(2) = gp_Circ2d(gp_Ax2d(Point2,dirx),Radius);
	C(3) = gp_Circ2d(C1.XAxis(),Radius+R1);
	C(4) = gp_Circ2d(gp_Ax2d(Point2,dirx),Radius);
	nbsol = 2;
      }
    }
    if (nbsol > 0) {
      for (Standard_Integer j = 1 ; j <= nbsol ; j++) {
	IntAna2d_AnaIntersection Intp(C(2*j-1),C(2*j));
	if (Intp.IsDone()) {
	  if (!Intp.IsEmpty()) {
	    for (i = 1 ; i <= Intp.NbPoints() ; i++) {
	      NbrSol++;
	      gp_Pnt2d Center(Intp.Point(i).Value());
	      cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius);
//            =======================================================
	      Standard_Real distcc1 = center1.Distance(Center);
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
	      qualifier2(NbrSol) = GccEnt_noqualifier;
	      dir1 = gp_Dir2d(Center.XY()-center1.XY());
#ifdef DEB
	      gp_Dir2d dir2(Center.XY()-Point2.XY());
#endif
	      if ((Center.Distance(center1) > C1.Radius()) &&
		  (Radius < Center.Distance(center1)+C1.Radius())) {
		pnttg1sol(NbrSol) = gp_Pnt2d(Center.XY()-Radius*dir1.XY());
	      }
	      else if ((Center.Distance(center1) < C1.Radius()) &&
		       (Radius < C1.Radius())) {
		pnttg1sol(NbrSol) = gp_Pnt2d(Center.XY()+Radius*dir1.XY());
	      }
	      else {
		pnttg1sol(NbrSol) = gp_Pnt2d(Center.XY()-Radius*dir1.XY());
	      }
	      pnttg2sol(NbrSol) = Point2;
	    }
	  }
	  WellDone = Standard_True;
	}
      }
    }
    else if (nbsol < 0) {
      gp_Pnt2d Center(center1.XY()+deport*dir1.XY());
      cirsol(1) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius);
//    ==================================================
      qualifier1(1) = Qualified1.Qualifier();
      qualifier2(1) = GccEnt_noqualifier;
      if (Abs(deport) <= Tol && Abs(Radius-R1) <= Tol) {
	TheSame1(1) = 1;
      }
      else {
	pnttg1sol(1) = gp_Pnt2d(Center.XY()+signe*Radius*dir1.XY());
      }
      pnttg2sol(1) = Point2;
      WellDone = Standard_True;
      NbrSol = 1;
    }
  }
  for (i = 1 ; i <= NbrSol ; i++) {
    par1sol(i)=ElCLib::Parameter(cirsol(i),pnttg1sol(i));
    if (TheSame1(i) == 0) {
      pararg1(i)=ElCLib::Parameter(C1,pnttg1sol(i));
    }
    par2sol(i) = ElCLib::Parameter(cirsol(i),pnttg2sol(i));
    pararg2(i) = 0.;
  }
}





