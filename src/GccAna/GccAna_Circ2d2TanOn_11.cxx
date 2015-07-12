// Created on: 1992-01-02
// Created by: Remi GILET
// Copyright (c) 1992-1999 Matra Datavision
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

//=========================================================================
//   Creation of a circle tangent to two elements : Straight line.        +
//                                                  Circle.               +
//                                                  Point.                +
//                              center on a third : Straight line.        +
//                                                  Circle.               +
//=========================================================================

#include <ElCLib.hxx>
#include <GccAna_Circ2d2TanOn.hxx>
#include <GccAna_Lin2dBisec.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <GccEnt_QualifiedLin.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

//=========================================================================
//   Creation of a circle passing by  : 2 points Point1 and Point2.        +
//                        Center on   : 1 circle OnCirc.                  +
//   with a Tolerance of precision    : Tolerance.                        +
//                                                                        +
//   Create L1 the straight line of points equidistant to Point1 and Point2.     +
//   Create then solutions cirsol so that :                      +
//   cirsol is the set of circles with center in one of inter-    +
//   sections of L1 with OnCirc and the radius the distance between Point1 and   +
//   the point calculated below.                                          +
//=========================================================================
GccAna_Circ2d2TanOn::
   GccAna_Circ2d2TanOn (const gp_Pnt2d&         Point1     ,
                        const gp_Pnt2d&         Point2     ,
                        const gp_Circ2d&        OnCirc     ,
                        const Standard_Real     Tolerance  ):
   cirsol(1,2)      ,
   qualifier1(1,2)  ,
   qualifier2(1,2)  ,
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
  Standard_Real Tol = Abs(Tolerance);
  Standard_Real dist   = Point1.Distance(Point2);
  Standard_Real dp1cen = Point1.Distance(OnCirc.Location());
  Standard_Real dp2cen = Point2.Distance(OnCirc.Location());
  Standard_Real R      = OnCirc.Radius();
  gp_Circ2d C1 = OnCirc;
  if (dist<Tol||Abs(dp1cen+2*R-dp2cen)<Tol||Abs(dp2cen+2*R-dp1cen)<Tol){
    WellDone = Standard_True;
    return;
  }
  gp_Lin2d L1(gp_Pnt2d((Point1.XY()+Point2.XY())/2.0),
	      gp_Dir2d(Point1.Y()-Point2.Y(),Point2.X()-Point1.X()));
  if (Abs(dp1cen+2*R-dp2cen)<Tol || Abs(dp2cen+2*R-dp1cen)<Tol) {
    if (Abs(dp1cen+2*R-dp2cen)<Tol) {
      C1 = gp_Circ2d(gp_Ax2d(OnCirc.Location(),dirx),
		     OnCirc.Radius()+Abs(dp2cen-dp1cen-2.0*R));
    }
    else if (Abs(dp1cen+2*R-dp2cen)<Tol) {
      C1 = gp_Circ2d(gp_Ax2d(OnCirc.Location(),dirx),
		     OnCirc.Radius()+Abs(dp2cen-dp1cen-2.0*R));
    }
  }
  IntAna2d_AnaIntersection Intp(L1,C1);
  if (Intp.IsDone()) {
    if (!Intp.IsEmpty()) {
      for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
	NbrSol++;
	gp_Ax2d axe(Intp.Point(i).Value(),dirx);
	cirsol(NbrSol) = gp_Circ2d(axe,Point1.Distance(Intp.Point(i).Value()));
//      ======================================================================
	qualifier1(NbrSol) = GccEnt_noqualifier;
	qualifier2(NbrSol) = GccEnt_noqualifier;
	pnttg1sol(NbrSol) = Point1;
	pararg1(NbrSol) = 0.;
	par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),pnttg1sol(NbrSol));
	pnttg2sol(NbrSol) = Point2;
	pararg2(NbrSol) = 0.;
	par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),pnttg2sol(NbrSol));
	pntcen(NbrSol) = cirsol(NbrSol).Location();
	parcen3(NbrSol)=ElCLib::Parameter(OnCirc,pntcen(NbrSol));
      }
     }
    WellDone = Standard_True;
  }
}


