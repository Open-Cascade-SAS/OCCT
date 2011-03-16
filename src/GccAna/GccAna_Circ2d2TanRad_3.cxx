// File:	GccAna_Circ2d2TanRad_3.cxx
// Created:	Tue Sep 24 09:14:08 1991
// Author:	Remi GILET
//		<reg@topsn2>
// Modified:	Thu Jun 18 15:45:00 1998
// Author:	Joelle CHAUVET
//		PRO10310 : cas ou le point est sur la droite

#include <GccAna_Circ2d2TanRad.jxx>

#include <ElCLib.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Ax2d.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Standard_NegativeValue.hxx>
#include <GccEnt_BadQualifier.hxx>

// circulaire tangent a une ligne et un point et de rayon donne
//=============================================================
//========================================================================
// On initialise WellDone a false.                                       +
// On recupere la ligne L1.                                              +
// On sort en erreur dans les cas ou la construction est impossible.     +
// On fait la parallele a L1 dans le bon sens.                           +
// On fait le cercle centre en Point1 de rayon Radius.                   +
// On intersecte la parallele et le cercle.                              +
//                              ==> Le point de centre de la solution.   +
// On cree la solution qu on ajoute aux solutions deja trouvees.         +
// On remplit les champs.                                                +
//========================================================================

GccAna_Circ2d2TanRad::
   GccAna_Circ2d2TanRad (const GccEnt_QualifiedLin&  Qualified1 ,
                         const gp_Pnt2d&             Point2     ,
                         const Standard_Real         Radius     ,
                         const Standard_Real         Tolerance  ):
   qualifier1(1,2) ,
   qualifier2(1,2),
   TheSame1(1,2)   ,
   TheSame2(1,2)   ,
   cirsol(1,2)     ,
   pnttg1sol(1,2)  ,
   pnttg2sol(1,2)  ,
   par1sol(1,2)    ,
   par2sol(1,2)    ,
   pararg1(1,2)    ,
   pararg2(1,2)    
{

  gp_Dir2d dirx(1.0,0.0);
  Standard_Real Tol = Abs(Tolerance);
  NbrSol = 0;
  WellDone = Standard_False;
  if (!(Qualified1.IsEnclosed() || Qualified1.IsOutside() || 
	Qualified1.IsUnqualified())) {
    GccEnt_BadQualifier::Raise();
    return;
  }
  Standard_Integer nbsol = 0;
  Standard_Integer nbcote=0;
  TColStd_Array1OfReal cote(1,2);
  gp_Lin2d L1 = Qualified1.Qualified();
  Standard_Real displ1 = L1.Distance(Point2);
  Standard_Real xdir = (L1.Direction()).X();
  Standard_Real ydir = (L1.Direction()).Y();
  Standard_Real lxloc = (L1.Location()).X();
  Standard_Real lyloc = (L1.Location()).Y();
  gp_Pnt2d origin1(lxloc,lyloc);
  gp_Dir2d normL1(-ydir,xdir);
  Standard_Real cxloc = Point2.X();
  Standard_Real cyloc = Point2.Y();

  if (Radius < 0.0) { Standard_NegativeValue::Raise(); }

  else {
    if ( displ1-Radius*2.0 > Tol) { WellDone = Standard_True; }
    else if (Qualified1.IsEnclosed()) {
//  =================================
      if ((-ydir*(cxloc-lxloc)+xdir*(cyloc-lyloc)<0.0)) { 
	WellDone = Standard_True; 
      }
      else {
	if ( displ1-Radius*2.0 > 0.0) {
	  nbsol = 2;
	  NbrSol = 1;
	  nbcote = 1;
	  cote(1) = 1.0;
	}
	else {
	  nbsol = 1;
	  nbcote = 1;
	  cote(1) = 1.0;
	} 
      }
    }
    else if (Qualified1.IsOutside()) {
//  ================================
      if ((-ydir*(cxloc-lxloc)+xdir*(cyloc-lyloc)>0.0)) { 
	WellDone = Standard_True; 
      }
      else {
	if ( displ1-Radius*2.0 > 0.0) {
	  nbsol = 2;
	  nbcote = 1;
	  cote(1) = -1.0;
	}
	else {
	  nbsol = 1;
	  nbcote = 1;
	  cote(1) = -1.0;
	}
      }
    }
    else if (Qualified1.IsUnqualified()) {
//  ====================================
      if ( displ1-Radius*2.0 > 0.0) {
	if ((-ydir*(cxloc-lxloc)+xdir*(cyloc-lyloc)>0.0)) {
	  nbsol = 2;
	  nbcote = 1;
	  cote(1) = 1.0;
	}
	else if ((-ydir*(cxloc-lxloc)+xdir*(cyloc-lyloc)<0.0)) {
	  nbsol = 2;
	  nbcote = 1;
	  cote(1) = -1.0;
	}
      }
      else {
	nbsol = 1;
	nbcote = 2;
	cote(1) = 1.0;
	cote(2) = -1.0;
      }
    }

    if (nbsol == 1) {
      if (displ1<1.e-10) {
	// cas particulier ou Point2 est sur la ligne
	// pas la peine de passer par les intersections
	// on construit les deux solutions directement
	for (Standard_Integer jcote = 1 ; jcote <= nbcote ; jcote++) {
	  NbrSol++;
	  gp_Pnt2d Center(cxloc-cote(jcote)*ydir*Radius,
			  cyloc+cote(jcote)*xdir*Radius);
	  cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius);
//        =======================================================
	  qualifier2(NbrSol) = GccEnt_noqualifier;
	  if (!Qualified1.IsUnqualified()) { 
	    qualifier1(NbrSol) = Qualified1.Qualifier();
	  }
	  else if (cote(jcote) > 0.0) {
	    qualifier1(NbrSol) = GccEnt_outside;
	  }
	  else { 
	    qualifier1(NbrSol) = GccEnt_enclosed; 
	  }
	  TheSame1(NbrSol) = 0;
	  TheSame2(NbrSol) = 0;
	  pnttg1sol(NbrSol) = Point2;
	  pnttg2sol(NbrSol) = Point2;
	}
	WellDone = Standard_True;
      }
      else {
	gp_Circ2d cirint(gp_Ax2d(Point2,dirx),Radius);
	for (Standard_Integer jcote = 1 ; jcote <= nbcote ; jcote++) {
	  gp_Lin2d  linint(gp_Pnt2d(lxloc-cote(jcote)*ydir*Radius,
				    lyloc+cote(jcote)*xdir*Radius),
			   L1.Direction());
	  IntAna2d_AnaIntersection Intp(linint,cirint);
	  if (Intp.IsDone()) {
	    if (!Intp.IsEmpty()) {
	      for (Standard_Integer i = 1 ; i <= Intp.NbPoints() && NbrSol<2; i++) {
		NbrSol++;
		gp_Pnt2d Center(Intp.Point(i).Value());
		cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius);
  //            =======================================================
		gp_Dir2d dc1(origin1.XY()-Center.XY());
		qualifier2(NbrSol) = GccEnt_noqualifier;
		if (!Qualified1.IsUnqualified()) { 
		  qualifier1(NbrSol) = Qualified1.Qualifier();
		}
		else if (dc1.Dot(normL1) > 0.0) {
		  qualifier1(NbrSol) = GccEnt_outside;
		}
		else { qualifier1(NbrSol) = GccEnt_enclosed; }
		TheSame1(NbrSol) = 0;
		TheSame2(NbrSol) = 0;
		pnttg1sol(NbrSol)=gp_Pnt2d(Center.XY()+
					   cote(jcote)*Radius*gp_XY(ydir,-xdir));
		pnttg2sol(NbrSol) = Point2;
	      }
	    }
	    WellDone = Standard_True;
	  }
	}
      }
    }

    else if (nbsol == 2) {
      gp_Pnt2d Center(Point2.XY()+cote(1)*Radius*gp_XY(-ydir,xdir));
      WellDone = Standard_True;
      NbrSol = 1;
      cirsol(1) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius);
//    ==================================================
      qualifier2(1) = GccEnt_noqualifier;
      TheSame1(1) = 0;
      TheSame2(1) = 0;
      pnttg1sol(1)=gp_Pnt2d(Center.XY()+cote(1)*Radius*gp_XY(ydir,-xdir));
      pnttg2sol(1) = Point2;
    }
  }

  for (Standard_Integer i = 1 ; i <= NbrSol ; i++) {
    par1sol(i)=ElCLib::Parameter(cirsol(i),pnttg1sol(i));
    pararg1(i)=ElCLib::Parameter(L1,pnttg1sol(i));
    par2sol(i)=ElCLib::Parameter(cirsol(i),pnttg2sol(i));
    pararg2(i)=0.;
  }

}




