// Created on: 1991-09-24
// Created by: Joelle CHAUVET
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

// Modified:	Thu Jun 18 15:45:00 1998
//		PRO10310 : cas ou le point est sur la droite

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

// circular tangent to a line and a point and a given radius
//=============================================================
//========================================================================
// Initialize WellDone to false.                                         +
// Return line L1.                                                       +
// Leave with error if the construction is impossible.                   +
// Create parallel to L1 in the proper direction.                        +
// Create the circle with center at Point1 of radius Radius.             +
// Intersect the parallel and the circle.                                +
//                              ==> The center point of the  solution.   +
// Create the solution to be added to already found solutions.           +
// Fill the fields.                                                      +
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
    throw GccEnt_BadQualifier();
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

  if (Radius < 0.0) { throw Standard_NegativeValue(); }

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
	// particular case when Point2 is on the line
	// construct two solutions directly
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




