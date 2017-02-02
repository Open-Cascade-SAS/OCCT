// Copyright (c) 1995-1999 Matra Datavision
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
#include <GccAna_Circ2dTanOnRad.hxx>
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

//=========================================================================
//   Circle tangent to a point   Point1.                                  +
//          center on circle     OnCirc.                                  +
//          radius               Radius.                                  +
//                                                                        +
//  Initialize the table of solutions cirsol and all fields.              +
//  Eliminate cases not being the solution.                               +
//  Create the circle with center in Point1 of radius Radius.             +
//  Intersect this circle with OnCirc and obtain the center points        +
//  of found solutions.                                                   +
//  Create solutions cirsol.                                              +
//=========================================================================
GccAna_Circ2dTanOnRad::
   GccAna_Circ2dTanOnRad (const gp_Pnt2d&     Point1    ,
                          const gp_Circ2d&    OnCirc    ,
                          const Standard_Real Radius    ,
                          const Standard_Real Tolerance ):
   cirsol(1,2)   ,
   qualifier1(1,2) ,
   TheSame1(1,2) ,
   pnttg1sol(1,2),
   pntcen3(1,2)  ,
   par1sol(1,2)  ,
   pararg1(1,2)  ,
   parcen3(1,2)  
{

   gp_Dir2d dirx(1.0,0.0);
   Standard_Real Tol = Abs(Tolerance);
   WellDone = Standard_False;
   NbrSol = 0;
   Standard_Real Roncirc = OnCirc.Radius();
   Standard_Real dist1 = Point1.Distance(OnCirc.Location())-Roncirc;
   Standard_Real dist2 = Point1.Distance(OnCirc.Location())+Roncirc;

   if (Radius < 0.0) { throw Standard_NegativeValue(); }
   else if ((dist1-Radius > Tol) || (Tol < Radius-dist2)) { 
     WellDone = Standard_True; 
   }
   else {
     Standard_Integer signe = 0;
     if (Abs(dist1-Radius) < Tol) { signe = 1; }
     else if (Abs(dist2-Radius) < Tol) { signe = -1; }
     if (signe != 0) {
       gp_Ax2d axe(gp_Pnt2d(OnCirc.Location().XY()-Roncirc*
			 gp_Dir2d(OnCirc.Location().X()-signe*Point1.X(),
			   OnCirc.Location().Y()-signe*Point1.Y()).XY()),dirx);
       cirsol(1) = gp_Circ2d(axe,Radius);
//      ================================
       qualifier1(1) = GccEnt_noqualifier;
       TheSame1(1) = 0;
       pnttg1sol(1) = Point1;
       pntcen3(1) = cirsol(1).Location();
       pararg1(1) = 0.0;
       par1sol(1)=ElCLib::Parameter(cirsol(1),pnttg1sol(1));
       parcen3(1) = ElCLib::Parameter(OnCirc,pntcen3(1));
       WellDone = Standard_True;
       NbrSol = 1;
     }
     else {
       IntAna2d_AnaIntersection Intp(OnCirc,gp_Circ2d(gp_Ax2d(Point1,dirx),
						      Radius));
       if (Intp.IsDone()) {
	 if (!Intp.IsEmpty()) {
	   for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
	     NbrSol++;
	     gp_Pnt2d Center(Intp.Point(i).Value());
	     cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius);
//           =======================================================
       qualifier1(1) = GccEnt_noqualifier;
	     TheSame1(1) = 0;
	     pnttg1sol(1) = Point1;
	     pntcen3(1) = cirsol(1).Location();
	     par1sol(1)=ElCLib::Parameter(cirsol(1),pnttg1sol(1));
	     parcen3(1) = ElCLib::Parameter(OnCirc,pntcen3(1));
	     pararg1(1) = 0.0;
	   }
	 }
	 WellDone = Standard_True;
       }
     }
   }
 }
