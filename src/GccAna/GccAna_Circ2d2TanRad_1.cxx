// Created on: 1991-09-24
// Created by: Remi GILET
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <ElCLib.hxx>
#include <GccAna_Circ2d2TanRad.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <GccEnt_QualifiedLin.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <Standard_NegativeValue.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <TColStd_Array1OfReal.hxx>

// circular tangent to a circle, a line and a given radius
//==============================================================
//========================================================================
// Initialize WellDone to false.                                         +
// Return circle C1 and straight line L2.                                +
// Leave with error if the construction is impossible.                   +
// Create parallel to C1 in the proper direction.                        +
// Create parallel to L2 in the proper direction.                        +
// Intersect parallels ==> center point of the solution.                 +
// Create the solution and add it to already found ones.                 +
// Fill the fields.                                                      +
//========================================================================
GccAna_Circ2d2TanRad::
   GccAna_Circ2d2TanRad (const GccEnt_QualifiedCirc& Qualified1 ,
                         const GccEnt_QualifiedLin&  Qualified2 ,
                         const Standard_Real         Radius     ,
                         const Standard_Real         Tolerance  ):
   qualifier1(1,8) ,
   qualifier2(1,8),
   TheSame1(1,8)   ,
   TheSame2(1,8)   ,
   cirsol(1,8)     ,
   pnttg1sol(1,8)  ,
   pnttg2sol(1,8)  ,
   par1sol(1,8)    ,
   par2sol(1,8)    ,
   pararg1(1,8)    ,
   pararg2(1,8)    
{

  Standard_Real Tol = Abs(Tolerance);
  gp_Dir2d dirx(1.,0.);
  NbrSol = 0;
  WellDone = Standard_False;
  if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
	Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
      !(Qualified2.IsEnclosed() || Qualified2.IsOutside() || 
	Qualified2.IsUnqualified())) {
    throw GccEnt_BadQualifier();
    return;
  }
  Standard_Integer i ;
  for ( i = 1 ; i <= 8 ; i++) { TheSame2(i) = 0; }
  Standard_Integer ncote1=0;
  Standard_Integer ncote2=0;
  Standard_Integer nbsol = 0;
  Standard_Real cote = 0.0;
  Standard_Real ccote = 0.0;
  TColStd_Array1OfReal cote1(1,3);
  TColStd_Array1OfReal cote2(1,2);
  gp_Circ2d C1 = Qualified1.Qualified();
  gp_Lin2d  L2 = Qualified2.Qualified();
  Standard_Real xdir = (L2.Direction()).X();
  Standard_Real ydir = (L2.Direction()).Y();
  gp_Dir2d normL2(-ydir,xdir);
  Standard_Real lxloc = (L2.Location()).X();
  Standard_Real lyloc = (L2.Location()).Y();
  Standard_Real cxloc = (C1.Location()).X();
  Standard_Real cyloc = (C1.Location()).Y();
  Standard_Real R1 = C1.Radius();
  gp_Pnt2d center1(cxloc,cyloc);
  gp_Pnt2d origin2(lxloc,lyloc);
  Standard_Real dist = Radius*2.0 + R1;
  Standard_Real distance = L2.Distance(center1);
  
  if (Radius < 0.0) { throw Standard_NegativeValue(); }
  else {
    if ( distance-dist >Tol) { WellDone = Standard_True; }
    else if (Qualified1.IsEnclosed()) {
//  =================================
      if (Qualified2.IsEnclosed()) {
//    ============================
	if (distance-R1 > Tol) { WellDone = Standard_True; }
	else if (((-ydir*(cxloc-lxloc)+xdir*(cyloc-lyloc)<0.0) && 
		  (Radius*2.0-R1+distance > Tol)) ||
		 ((-ydir*(cxloc-lxloc)+xdir*(cyloc-lyloc)>0.0) &&
		  (Radius*2.0-R1-distance > Tol))||
		 (Radius-R1 > Tol)) { WellDone = Standard_True; }
	else {
	  if (((-ydir*(cxloc-lxloc)+xdir*(cyloc-lyloc)<0.0) && 
	       (Radius*2.0>R1-distance)) || (Abs(distance-R1)<Tol) ||
	      ((-ydir*(cxloc-lxloc)+xdir*(cyloc-lyloc)>0.0) &&
	       (Radius*2.0 > (R1+distance+Tol)))) {
	    cote = 1.0;
	    nbsol = 3;
	    ccote = 1.0;
	  }
	  else {
	    ncote1 = 1;
	    ncote2 = 1;
	    cote1(1) = R1-Radius;
	    cote2(1) = 1.0;
	    nbsol = 1;
	  }
	}
      }
      else if (Qualified2.IsOutside()) {
//    ================================
	if (distance > R1+Tol) { WellDone = Standard_True; }
	else if ((((-ydir*(cxloc-lxloc)+xdir*(cyloc-lyloc)) > 0.0) &&
		  (Radius*2.0-R1+distance > Tol)) ||
		 (((-ydir*(cxloc-lxloc)+xdir*(cyloc-lyloc)) < 0.0) &&
		  (Radius*2.0-R1-distance > Tol)) ||
		 (Radius-R1 >Tol)) { WellDone = Standard_True; }
	else {
	  if (((-ydir*(cxloc-lxloc)+xdir*(cyloc-lyloc) > 0.0) && 
	       (Radius*2.0 > R1-distance)) || (Abs(R1-Radius) < Tol) ||
	      ((-ydir*(cxloc-lxloc)+xdir*(cyloc-lyloc) < 0.0) &&
	       (Radius*2.0>(R1+distance))) || (Abs(distance-R1)<Tol)) {
	    cote = -1.0;
	    nbsol = 3;
	    ccote = 1.0;
	  }
	  else {
	    ncote1 = 1;
	    ncote2 = 1;
	    cote1(1) = R1-Radius;
	    cote2(1) = -1.0;
	    nbsol = 1;
	  }
	}
      }
      else if (Qualified2.IsUnqualified()) {
//    ====================================
	if ((distance-R1>Tol) || (Radius*2.0-R1-distance>Tol) ||
	    (Radius-R1 > Tol)) { WellDone = Standard_True; }
	else if (distance > R1) {
	  if ((-ydir*(cxloc-lxloc)+xdir*(cyloc-lyloc) > 0.0)) {
	    cote = 1.0;
	  }
	  else {
	    cote = -1.0;
	    nbsol = 3;
	  }
	  nbsol = 3;
	  ccote = 1;
	}
	else {
	  ncote1 = 1;
	  ncote2 = 2;
	  cote1(1) = R1-Radius;
	  cote2(1) = 1.0;
	  cote2(2) = -1.0;
	  nbsol = 1;
	}
      }
      if (nbsol == 3) {
      }
    }
    else if (Qualified1.IsEnclosing()) {
//  ==================================
      if (Qualified2.IsEnclosed()) {
//    =================================
	if ((distance<R1-Tol)||(Radius<R1-Tol)||(Radius*2<distance+R1-Tol) ||
	    (Tol<R1-Radius)||(-ydir*(cxloc-lxloc)+xdir*(cyloc-lyloc)<0.0)){
	  WellDone = Standard_True;
	}
	else if ((distance<R1) || (Radius<R1) || (Radius*2.0<distance+R1)) {
	  cote = 1.0;
	  ccote =1.0;
	  nbsol = 3;
	}
	else {
	  ncote1 = 1;
	  ncote2 = 1;
	  cote1(1) = Radius-R1;
	  cote2(1) = 1.0;
	  nbsol = 1;
	}
      }
      else if (Qualified2.IsOutside()) {
//    ================================
	if ((Tol<R1-distance) || (Tol<distance+R1-Radius*2.0) ||
	    (Tol<R1-Radius)||(-ydir*(cxloc-lxloc)+xdir*(cyloc-lyloc)>0.0)){
	  WellDone = Standard_True;
	}
	else if ((distance<R1) || (Radius*2.0<distance+R1) || (Radius<R1)) {
	  cote = -1.0;
	  ccote = -1.0;
	  nbsol = 3;
	}
	else {
	  ncote1 = 1;
	  ncote2 = 1;
	  cote1(1) = Radius-R1;
	  cote2(1) = -1.0;
	  nbsol = 1;
	}
      }
      else if (Qualified2.IsUnqualified()) {
//    ====================================
	if ((distance<R1-Tol) || (Radius*2.0<distance+R1-Tol) ||
	    (Radius < R1-Tol)) { WellDone = Standard_True; }
	else if ((distance < R1) || (Radius*2.0 < distance+R1)) {
	  if ((-ydir*(cxloc-lxloc)+xdir*(cyloc-lyloc) > 0.0)) {
	    cote = 1.0;
	    ccote = 1.0;
	    nbsol = 3;
	  }
	  else {
	    ccote = -1.0;
	    cote = -1.0;
	    nbsol = 3;
	  }
	}
	else {
	  ncote1 = 1;
	  ncote2 = 2;
	  cote1(1) = Radius-R1;
	  cote2(1) = 1.0;
	  cote2(2) = -1.0;
	  nbsol = 1;
	}
      }
    }
    else if ((Qualified1.IsOutside()) && (Qualified2.IsEnclosed())) {
//  ===============================================================
      if (((-ydir*(cxloc-lxloc)+xdir*(cyloc-lyloc)<0.0) &&
	   (distance>R1+Tol)) || (distance > R1+Radius*2.0+Tol)) {
	WellDone = Standard_True;
      }
      else {
	if ((-ydir*(cxloc-lxloc)+xdir*(cyloc-lyloc)<0.0) &&
	    (distance>R1)) {
	  cote = 1.0;
	  nbsol = 2;
	}
	else if ((-ydir*(cxloc-lxloc)+xdir*(cyloc-lyloc)>0.0) &&
		 (distance>R1+2.0*Radius)) {
	  cote = -1.0;
	  nbsol = 2;
	}
	else {
	  ncote1 = 1;
	  ncote2 = 1;
	  cote1(1) = Radius+R1;
	  cote2(1) = 1.0;
	  nbsol = 1;
	}
      }
    }
    else if ((Qualified1.IsOutside()) && (Qualified2.IsOutside())) {
//  ==============================================================
      if (((-ydir*(cxloc-lxloc)+xdir*(cyloc-lyloc)>0.0) &&
	   (distance>R1+Tol)) || (distance > R1+Radius*2.0+Tol)) {
	WellDone = Standard_True;
      }
      else {
	if ((-ydir*(cxloc-lxloc)+xdir*(cyloc-lyloc)>0.0) && (distance>R1)) {
	  cote = -1.0;
	  nbsol = 2;
	}
	else if ((-ydir*(cxloc-lxloc)+xdir*(cyloc-lyloc)<0.0) &&
		 (distance>R1+2.0*Radius)) {
	  cote = 1.0;
	  nbsol = 2;
	}
	else {
	  ncote1 = 1;
	  ncote2 = 1;
	  cote1(1) = Radius+R1;
	  cote2(1) = -1.0;
	  nbsol = 1;
	}
      }
    }
    else if ((Qualified1.IsOutside()) && (Qualified2.IsUnqualified())) {
//  ==================================================================
      if (distance > Radius*2.0+R1+Tol) { WellDone = Standard_True; }
      else if (distance > Radius*2.0+R1) {
	if (-ydir*(cxloc-lxloc)+xdir*(cyloc-lyloc)>0.0) { cote = 1.0; }
	else { cote = -1.0; }
	nbsol = 4;
      }
      else {
	ncote1 = 1;
	ncote2 = 2;
	cote1(1) = Radius+R1;
	cote2(1) = 1.0;
	cote2(2) = -1.0;
	nbsol = 1;
      }
    }
    else if ((Qualified1.IsUnqualified()) && (Qualified2.IsEnclosed())) {
//  ===================================================================
      if ((distance>R1+Radius*2.0+Tol) ||
	  ((-ydir*(cxloc-lxloc)+xdir*(cyloc-lyloc)>0.0) && 
	   (distance > R1+Tol))){ WellDone = Standard_True; }
      else {
	if ((-ydir*(cxloc-lxloc)+xdir*(cyloc-lyloc)>0.0) && (distance > R1)){
	  cote = 1.0;
	  nbsol = 2;
	}
	else if ((-ydir*(cxloc-lxloc)+xdir*(cyloc-lyloc)<0.0) &&
		 (distance>R1+2.0*Radius)) {
	  cote = -1.0;
	  nbsol = 2;
	}
	else {
	  ncote1 = 2;
	  ncote2 = 1;
	  cote1(1) = Abs(Radius-R1);
	  cote1(2) = Radius+R1;
	  cote2(1) = 1.0;
	  nbsol = 1;
	}
      }
    }
    else if ((Qualified1.IsUnqualified()) && (Qualified2.IsOutside())) {
//  ==================================================================
      if ((distance>R1+Radius*2.0+Tol) ||
	  ((-ydir*(cxloc-lxloc)+xdir*(cyloc-lyloc)<0.0) && 
	   (distance > R1+Tol))){ WellDone = Standard_True; }
      else {
	if ((-ydir*(cxloc-lxloc)+xdir*(cyloc-lyloc)<0.0) && (distance > R1)){
	  cote = -1.0;
	  nbsol = 2;
	}
	else if ((-ydir*(cxloc-lxloc)+xdir*(cyloc-lyloc)>0.0) &&
		 (distance>R1+2.0*Radius)) {
	  cote = 1.0;
	  nbsol = 2;
	}
	else {
	  ncote1 = 2;
	  ncote2 = 1;
	  cote1(1) = Abs(Radius-R1);
	  cote1(2) = Radius+R1;
	  cote2(1) = -1.0;
	  nbsol = 1;
	}
      }
    }
    else if ((Qualified1.IsUnqualified()) && (Qualified2.IsUnqualified())) {
//  ======================================================================
      if (distance>R1+Radius*2.0+Tol) { WellDone = Standard_True; }
      else if (distance>R1+Radius*2.0) {
	if (-ydir*(cxloc-lxloc)+xdir*(cyloc-lyloc)>0.0) { cote = -1.0; }
	else { cote = 1.0; }
	nbsol = 4;
      }
      else {
	ncote1 = 2;
	ncote2 = 2;
	cote1(1) = Abs(Radius-R1);
	cote1(2) = Radius+R1;
	cote2(1) = 1.0;
	cote2(2) = -1.0;
	nbsol = 1;
      }
    }
    if (nbsol == 1) {
      for (Standard_Integer jcote1 = 1 ; jcote1 <= ncote1 ; jcote1++) {
	for (Standard_Integer jcote2 = 1 ; jcote2 <= ncote2 ; jcote2++) {
	  if (cote1(jcote1)<Tol) continue;
	  gp_Circ2d cirint(gp_Ax2d(center1,dirx),cote1(jcote1));
	  gp_Lin2d  linint(gp_Pnt2d(lxloc-cote2(jcote2)*ydir*Radius,
			     lyloc+cote2(jcote2)*xdir*Radius),L2.Direction());
	  IntAna2d_AnaIntersection Intp(linint,cirint);
	  if (Intp.IsDone()) {
	    if (!Intp.IsEmpty()) {
	      for (i = 1 ; i <= Intp.NbPoints() ; i++) {
		NbrSol++;
		gp_Pnt2d Center(Intp.Point(i).Value());
		gp_Ax2d axe(Center,dirx);
		cirsol(NbrSol) = gp_Circ2d(axe,Radius);
//              ======================================
#ifdef OCCT_DEBUG
		gp_Dir2d dc1(center1.XY()-Center.XY());
#endif
		gp_Dir2d dc2(origin2.XY()-Center.XY());
		Standard_Real distcc1 = Center.Distance(center1);
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
		if (!Qualified2.IsUnqualified()) { 
		  qualifier2(NbrSol) = Qualified2.Qualifier();
		}
		else if (dc2.Dot(normL2) > 0.0) {
		  qualifier2(NbrSol) = GccEnt_outside;
		}
		else { qualifier2(NbrSol) = GccEnt_enclosed; }
		TheSame1(NbrSol) = 0;
		if ((Radius < R1) && Center.Distance(center1) <= R1) {
		  pnttg1sol(NbrSol) = gp_Pnt2d(Center.XY()+Radius*(gp_Dir2d(
					Center.XY()-center1.XY()).XY()));
		}
		else {
		  pnttg1sol(NbrSol) = gp_Pnt2d(Center.XY()-Radius*(gp_Dir2d(
					Center.XY()-center1.XY()).XY()));
		}
		pnttg2sol(NbrSol) = gp_Pnt2d(Center.XY()+
				       cote2(jcote2)*Radius*gp_XY(ydir,-xdir));
	      }
	    }
	    WellDone = Standard_True;
	  }
	}
      }
    }
    else if (nbsol == 2) {
      gp_Pnt2d Cen(center1.XY()+cote*(R1+Radius)*gp_XY(-ydir,xdir));
      gp_Ax2d axe(Cen,dirx);
      cirsol(1) = gp_Circ2d(axe,Radius);
//    =================================
      WellDone = Standard_True;
      NbrSol = 1;
      TheSame1(1) = 0;
      qualifier1(1) = Qualified1.Qualifier();;
      qualifier2(1) = Qualified2.Qualifier();
      pnttg1sol(1)=gp_Pnt2d(Cen.XY()+cote*Radius*gp_XY(ydir,-xdir));
      pnttg2sol(1)=gp_Pnt2d(Cen.XY()+Radius*gp_XY(ydir,-xdir));
    }
    else if (nbsol == 3) {
      WellDone = Standard_True;
      NbrSol = 1;
      gp_Pnt2d Cen(center1.XY()+cote*(R1-Radius)*gp_XY(ydir,-xdir));
      gp_Ax2d axe(Cen,dirx);
      cirsol(1) = gp_Circ2d(axe,Radius);
//    =================================
      qualifier1(1) = Qualified1.Qualifier();
      qualifier2(1) = Qualified2.Qualifier();
      pnttg2sol(1) = gp_Pnt2d(Cen.XY()+cote*Radius*gp_XY(ydir,-xdir));
      if (Abs(R1-Radius) > 0.0) {
	pnttg1sol(1) = gp_Pnt2d(Cen.XY()+ccote*Radius*gp_XY(ydir,-xdir));
      }
      else { TheSame1(1) = 1; }
    }
    else if (nbsol == 4) {
      gp_Pnt2d Cent(center1.XY()+cote*(R1+Radius)*gp_XY(-ydir,xdir));
      gp_Ax2d axe(Cent,dirx);
      cirsol(1) = gp_Circ2d(axe,Radius);
//    =================================
      qualifier1(1) = Qualified1.Qualifier();
      qualifier2(1) = Qualified2.Qualifier();
      WellDone = Standard_True;
      NbrSol = 1;
      TheSame1(1) = 0;
      pnttg1sol(1)=gp_Pnt2d(Cent.XY()+cote*Radius*gp_XY(ydir,-xdir));
      pnttg2sol(1)=gp_Pnt2d(Cent.XY()+cote*Radius*gp_XY(-ydir,xdir));
    }
  }
  for (i = 1 ; i <= NbrSol ; i++) {
    par1sol(i)=ElCLib::Parameter(cirsol(i),pnttg1sol(i));
    if (TheSame1(i) == 0) {
      pararg1(i)=ElCLib::Parameter(C1,pnttg1sol(i));
    }
    par2sol(i)=ElCLib::Parameter(cirsol(i),pnttg2sol(i));
    pararg2(i)=ElCLib::Parameter(L2,pnttg2sol(i));
  }
}


