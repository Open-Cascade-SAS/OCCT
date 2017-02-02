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
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <Standard_NegativeValue.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

// circulare tangent to a circle a point and a given radius
//=============================================================
//========================================================================
// Initialize WellDone to false.                                       +
// Return circle C1.                                             +
// Leave with error if the construction is impossible.     +
// Create parallel to C1 in the proper direction.                           +
// Create circle with center in Point1 of radius Radius.                   +
// Intersect the parallel and the circle.                              +
//                              ==> The center point of the solution.   +
// Create the solution that will be added to already found solutions.         +
// Fill the fields.                                                +
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
    throw GccEnt_BadQualifier();
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
  if (Radius < 0.0) { throw Standard_NegativeValue(); }
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
#ifdef OCCT_DEBUG
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





