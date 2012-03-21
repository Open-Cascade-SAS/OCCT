// Created on: 1991-09-24
// Created by: Remi GILET
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <GccAna_Circ2d2TanRad.jxx>

#include <ElCLib.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Ax2d.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <Standard_NegativeValue.hxx>
#include <GccEnt_BadQualifier.hxx>

//   Circle passing by two points of given radius.
//   ==============================================

//==============================================================================
//   Solution of the equation of second degree showing that the center of the  +
//   circle is equidistant from two points.                                    +
//==============================================================================

GccAna_Circ2d2TanRad::
   GccAna_Circ2d2TanRad (const gp_Pnt2d&             Point1     ,
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
  if (Radius < 0.0) { Standard_NegativeValue::Raise(); }
  else {
    if (Point1.Distance(Point2)-Radius*2.0 > Tol) { WellDone = Standard_True; }
    else {
      if (Point1.Distance(Point2)-Radius*2.0 >0.0) {
	WellDone = Standard_True;
	NbrSol = 1;
	gp_Ax2d axe(gp_Pnt2d(0.5*(Point1.XY()+Point2.XY())),dirx);
	cirsol(1) = gp_Circ2d(axe,Radius);
//      =================================
	qualifier1(1) = GccEnt_noqualifier;
	qualifier2(1) = GccEnt_noqualifier;
	TheSame1(1) = 0;
	TheSame2(1) = 0;
	pnttg1sol(1) = Point1;
	pnttg2sol(1) = Point2;
      }
      else {
	gp_Circ2d C1(gp_Ax2d(Point1,dirx),Radius);
	gp_Circ2d C2(gp_Ax2d(Point2,dirx),Radius);
	IntAna2d_AnaIntersection Intp(C1,C2);
	if (Intp.IsDone()) {
	  if (!Intp.IsEmpty()) {
	    for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
	      NbrSol++;
	      gp_Pnt2d Center(Intp.Point(i).Value());
	      cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius);
//            =======================================================
	      qualifier1(NbrSol) = GccEnt_noqualifier;
	      qualifier2(NbrSol) = GccEnt_noqualifier;
	      TheSame1(NbrSol) = 0;
	      TheSame2(NbrSol) = 0;
	      pnttg1sol(NbrSol) = Point1;
	      pnttg2sol(NbrSol) = Point2;
	    }
	  }
	  WellDone = Standard_True;
	}
      }
    }
  }
  for (Standard_Integer i = 1 ; i <= NbrSol ; i++) {
    pararg1(i) = 0.;
    pararg2(i) = 0.;
    par1sol(i)=ElCLib::Parameter(cirsol(i),pnttg1sol(i));
    par2sol(i)=ElCLib::Parameter(cirsol(i),pnttg2sol(i));
  }
}

