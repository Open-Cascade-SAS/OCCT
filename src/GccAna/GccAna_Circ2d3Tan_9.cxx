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
#include <GccAna_Circ2d3Tan.hxx>
#include <GccAna_Lin2dBisec.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <GccEnt_QualifiedLin.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

//=========================================================================
//   Creation of a circle passing by three points.                       +
//   Three cases  :                                               +
//      1/ Three points coincide.                               +
//      -----------------------------------                               +
//      The result is the circle with center in Point1 with zero radius.  +
//      2/ Two of three points coincide.                          +
//      ----------------------------------------                          +
//      Create the medium line between two non-coinciding points and      +
//      the straight line passing by these two points.                               +
//      The center of the solution is the intersection of two straight lines and the   +
//      radius is the distance between this center and one of three points.  +
//      3/ The three points are distinct.                                +
//      ----------------------------------                                +
//=========================================================================
GccAna_Circ2d3Tan::
   GccAna_Circ2d3Tan (const gp_Pnt2d& Point1    ,
                      const gp_Pnt2d& Point2    ,
                      const gp_Pnt2d& Point3    ,
		      const Standard_Real      Tolerance ):

//=========================================================================
//   Initialization of fields.                                           +
//=========================================================================

   cirsol(1,1)     ,
   qualifier1(1,1) ,
   qualifier2(1,1) ,
   qualifier3(1,1) ,
   TheSame1(1,1)   ,
   TheSame2(1,1)   ,
   TheSame3(1,1)   ,
   pnttg1sol(1,1)  ,
   pnttg2sol(1,1)  ,
   pnttg3sol(1,1)  ,
   par1sol(1,1)    ,
   par2sol(1,1)    ,
   par3sol(1,1)    , 
   pararg1(1,1)    , 
   pararg2(1,1)    ,
   pararg3(1,1)    
{

   gp_Dir2d dirx(1.0,0.0);
   WellDone = Standard_False;
   NbrSol = 0;

//=========================================================================
//   Processing.                                                          +
//=========================================================================

   Standard_Real dist1 = Point1.Distance(Point2);
   Standard_Real dist2 = Point1.Distance(Point3);
   Standard_Real dist3 = Point2.Distance(Point3);

   qualifier1(1) = GccEnt_noqualifier;
   qualifier2(1) = GccEnt_noqualifier;
   qualifier3(1) = GccEnt_noqualifier;

   if ((dist1 < Tolerance) && (dist2 < Tolerance) && (dist3 < Tolerance)) {
     NbrSol++;
     WellDone = Standard_True;
     cirsol(1) = gp_Circ2d(gp_Ax2d(Point1,dirx),0.0);
//   ===============================================
     TheSame1(1) = 0;
     TheSame2(1) = 0;
     TheSame3(1) = 0;
     pnttg1sol(1) = Point1;
     pnttg2sol(1) = Point2;
     pnttg3sol(1) = Point3;
     par1sol(1) =0.0;
     par2sol(1) =0.0;
     par3sol(1) =0.0;
     pararg1(1) =0.0;
     pararg2(1) =0.0;
     pararg3(1) =0.0;
   }
   else {
     gp_Lin2d L1;
     gp_Lin2d L2;
     if (dist1 >= Tolerance) {
       L1 = gp_Lin2d(gp_Pnt2d((Point1.XY()+Point2.XY())/2.0),
		     gp_Dir2d(Point1.Y()-Point2.Y(),Point2.X()-Point1.X()));
     }
     if (dist2 >= Tolerance) {
       L2 = gp_Lin2d(gp_Pnt2d((Point1.XY()+Point3.XY())/2.0),
		     gp_Dir2d(Point1.Y()-Point3.Y(),Point3.X()-Point1.X()));
     }
     if (dist2 <= Tolerance) {
       L2 = gp_Lin2d(Point1,
		     gp_Dir2d(Point1.Y()-Point2.Y(),Point2.X()-Point1.X()));
     }
     else if (dist1 <= Tolerance) {
       L1 = gp_Lin2d(Point1,
		     gp_Dir2d(Point1.Y()-Point3.Y(),Point3.X()-Point1.X()));
     }
     else if (dist3 <= Tolerance) {
       L2 = gp_Lin2d(Point1,
		     gp_Dir2d(Point1.Y()-Point2.Y(),Point2.X()-Point1.X()));
     }
     IntAna2d_AnaIntersection Intp(L1,L2);
     if (Intp.IsDone()) {
       if (!Intp.IsEmpty()) {
	 for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
	   NbrSol++;
	   cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Intp.Point(i).Value(),dirx),
//         ===============================================================
				      Point1.Distance(Intp.Point(i).Value()));
//                                    =======================================

	   TheSame1(NbrSol) = 0;
	   TheSame2(NbrSol) = 0;
	   TheSame3(NbrSol) = 0;
	   pnttg1sol(NbrSol) = Point1;
	   pnttg2sol(NbrSol) = Point2;
	   pnttg3sol(NbrSol) = Point3;
	   par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),pnttg1sol(NbrSol));
	   par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),pnttg2sol(NbrSol));
	   par3sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),pnttg3sol(NbrSol));
	   pararg1(NbrSol) =0.0;
	   pararg2(NbrSol) =0.0;
	   pararg3(NbrSol) =0.0;
	 }
       }
       WellDone = Standard_True;
     }
   }
 }

