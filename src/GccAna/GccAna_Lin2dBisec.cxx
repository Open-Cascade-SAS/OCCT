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

//=========================================================================
//   CREATION of the BISSECTICE between two STRAIGHT LINES.                        +
//=========================================================================

#include <ElCLib.hxx>
#include <GccAna_Lin2dBisec.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <gp.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_XY.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

//=========================================================================
//   The first calculated bissectrice is the interior bisectrice, the     +
//   second is the exterior bissectrice.                                  +
//   the direction of the first bissectrice is such that its scalar product +
//   with direction of Lin1 is always positive.             +
//   The second bissectrice is turned in the positive direction.             +
//=========================================================================
GccAna_Lin2dBisec::
   GccAna_Lin2dBisec (const gp_Lin2d& Lin1,
		      const gp_Lin2d& Lin2):
   linsol(1,2)    ,
   pntint1sol(1,2),
   pntint2sol(1,2),
   par1sol(1,2),
   par2sol(1,2),
   pararg1(1,2),
   pararg2(1,2) {

   WellDone = Standard_False;
   NbrSol = 0;

   IntAna2d_AnaIntersection Intp(Lin1,Lin2);
   if (Intp.IsDone()) {
     
     if (Intp.ParallelElements()) {
       if (Intp.IdenticalElements()) {
	 NbrSol = 1;
	 WellDone = Standard_True;
	 linsol(NbrSol) = gp_Lin2d(Lin1);
       }
       else {
	 // Attention : do not use dist = Lin1.Distance(Lin2);
	 // as straight lines can be concurrent for gp_Lin2d
	 // so dist = 0.0 (test of the angle too strict ?)
	 Standard_Real dist = Lin1.Distance(Lin2.Location())/2.0;
	 Standard_Real cross = 
	   gp_Vec2d ( -Lin2.Direction().Y() , Lin2.Direction().X() )
	   .Dot ( gp_Vec2d ( Lin2.Location() , Lin1.Location() ) );
	 if (cross < 0) dist = -dist;
	 NbrSol++;
	 WellDone = Standard_True;
	 linsol(NbrSol) = gp_Lin2d(gp_Pnt2d(Lin2.Location().XY()+
//       ========================================================
		 gp_XY(-Lin2.Direction().Y()*dist,Lin2.Direction().X()*dist)),
//               =============================================================
		 Lin2.Direction());
//               =================
       }
     }
     
     else {
       if (!Intp.IsEmpty()) {
	 for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
	   NbrSol++;
	   linsol(NbrSol) = gp_Lin2d(Intp.Point(i).Value(),
//	   ================================================
		       gp_Dir2d(Lin1.Direction().XY()+Lin2.Direction().XY()));
//	               ======================================================
	   NbrSol++;
	   linsol(NbrSol) = gp_Lin2d(Intp.Point(i).Value(),
//	   ===============================================
		       gp_Dir2d(Lin1.Direction().XY()-Lin2.Direction().XY()));
//	               ======================================================
	   if (Lin1.Angle(Lin2) >= 0.) { linsol(NbrSol).Reverse(); }
	   WellDone = Standard_True;
	 }
       }
     }
   }

   for (Standard_Integer i = 1 ; i <= NbrSol ; i++) {
     pntint1sol(i) = linsol(i).Location();
     pntint2sol(i) = pntint1sol(i);
     par1sol(i)=ElCLib::Parameter(linsol(i),pntint1sol(i));
     par2sol(i)=ElCLib::Parameter(linsol(i),pntint2sol(i));
     pararg1(i)=ElCLib::Parameter(Lin1,pntint1sol(i));
     pararg2(i)=ElCLib::Parameter(Lin2,pntint2sol(i));
   }

 }

//=========================================================================

Standard_Boolean GccAna_Lin2dBisec::
   IsDone () const { return WellDone; }


Standard_Integer GccAna_Lin2dBisec::
   NbSolutions () const 
{
  if (!WellDone) 
    throw StdFail_NotDone();
  return NbrSol;
}

gp_Lin2d GccAna_Lin2dBisec::
   ThisSolution (const Standard_Integer Index) const 
{
  if (!WellDone) 
    throw StdFail_NotDone();
  if (Index <= 0 || Index > NbrSol) 
    throw Standard_OutOfRange();
  return linsol(Index);
}

void GccAna_Lin2dBisec::
   Intersection1 (const Standard_Integer Index,
                  Standard_Real& ParSol,
                  Standard_Real& ParArg,
                  gp_Pnt2d& PntSol) const{
   if (!WellDone) { throw StdFail_NotDone(); }
   else if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
   else {
     ParSol = par1sol(Index);
     ParArg = pararg1(Index);
     PntSol = gp_Pnt2d(pntint1sol(Index));
   }
 }


void GccAna_Lin2dBisec::
   Intersection2 (const Standard_Integer Index,
                  Standard_Real& ParSol,
                  Standard_Real& ParArg,
                  gp_Pnt2d& PntSol) const{
   if (!WellDone) { throw StdFail_NotDone(); }
   else if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
   else {
     ParSol = par2sol(Index);
     ParArg = pararg2(Index);
     PntSol = gp_Pnt2d(pntint2sol(Index));
   }
 }

