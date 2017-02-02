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

//========================================================================
//    CREATION of a LINE TANGENT to a CIRCLE or PASSING by a POINT  +
//                     and PARALLEL to a STRAIGHT DROITE.                        +
//========================================================================

#include <ElCLib.hxx>
#include <GccAna_Lin2dTanPar.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_XY.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

//========================================================================
//   Passing by a point :                                              +
//   Create the straight line with origin ThePoint and                             + 
//                          direction Lin1.Direction().                    +
//========================================================================
GccAna_Lin2dTanPar::
   GccAna_Lin2dTanPar (const gp_Pnt2d& ThePoint  ,
                       const gp_Lin2d& Lin1      ):
   linsol(1,1),
   qualifier1(1,1) ,
   pnttg1sol(1,1),
   par1sol(1,1),
   pararg1(1,1)
{

   linsol(1) = gp_Lin2d(ThePoint,Lin1.Direction());
// ===============================================
   qualifier1(1) = GccEnt_noqualifier;
   pnttg1sol(1) = ThePoint;
   par1sol(1) = 0.;
   pararg1(1) = 0.;
   NbrSol = 1;
   WellDone = Standard_True;
}

//========================================================================
//   Tangent to a circle :                                               +
//   Create following the qualifier the straight line                    +
//          - with origin P1 (P1 is a point of intersection between C1 and +
//                       a straight line passing by the center of C1 and  +
//                         direction the normal to Lin1).                 +
//                         the choice of the point of intersection depends +
//                         on the qualifier.                            +
//          - with direction the direction of Lin1.                         +
//========================================================================

GccAna_Lin2dTanPar::
   GccAna_Lin2dTanPar (const GccEnt_QualifiedCirc& Qualified1,
                       const gp_Lin2d&             Lin1      ):
   linsol(1,2),
   qualifier1(1,2) ,
   pnttg1sol(1,2),
   par1sol(1,2),
   pararg1(1,2)
{

   WellDone = Standard_False;
   Standard_Integer signe = 0;
   Standard_Integer nbsol = 0;
   NbrSol = 0;
   if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
	 Qualified1.IsOutside() || Qualified1.IsUnqualified())) {
     throw GccEnt_BadQualifier();
     return;
   }
   gp_Circ2d C1 = Qualified1.Qualified();
   Standard_Real xdir = (Lin1.Direction()).X();
   Standard_Real ydir = (Lin1.Direction()).Y();

   if (Qualified1.IsEnclosed()) { throw GccEnt_BadQualifier(); }
// ============================
   else if (Qualified1.IsEnclosing()) {
// ==================================
     nbsol = 1;
     signe = 1;
     qualifier1(1) = GccEnt_enclosing;
   }
   else if (Qualified1.IsOutside()) {
// ===================================
     nbsol = 1;
     signe = -1;
     qualifier1(1) = GccEnt_outside;
   }
   else {
     nbsol = 2;
     signe = -1;
     qualifier1(1) = GccEnt_outside;
     qualifier1(2) = GccEnt_enclosing;
   }
   gp_XY xy(-C1.Radius()*ydir,C1.Radius()*xdir);
   for (Standard_Integer j = 1 ; j <= nbsol ; j++) {
     signe = -signe;
     NbrSol++;
     linsol(NbrSol) = gp_Lin2d(gp_Pnt2d((C1.Location().XY()).Added(signe*xy)),
//   =========================================================================
			       Lin1.Direction());
//                             =================
     pnttg1sol(NbrSol) = gp_Pnt2d((C1.Location().XY()).Added(signe*xy));
     par1sol(NbrSol) = 0.;
     pararg1(NbrSol)=ElCLib::Parameter(C1,pnttg1sol(NbrSol));
     WellDone = Standard_True;
   }
 }

Standard_Boolean GccAna_Lin2dTanPar::
   IsDone () const { return WellDone; }

Standard_Integer GccAna_Lin2dTanPar::NbSolutions () const 
{
  if (!WellDone) 
    throw StdFail_NotDone();
  return NbrSol;
}

gp_Lin2d GccAna_Lin2dTanPar::ThisSolution (const Standard_Integer Index) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
  else if (Index <= 0 || Index > NbrSol) { throw Standard_OutOfRange(); }
  return linsol(Index);
}

void GccAna_Lin2dTanPar::
  WhichQualifier(const Standard_Integer Index   ,
		       GccEnt_Position& Qualif1 ) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
   else if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
   else {
     Qualif1 = qualifier1(Index);
   }
}

void GccAna_Lin2dTanPar::
   Tangency1 (const Standard_Integer Index,
              Standard_Real& ParSol,
              Standard_Real& ParArg,
              gp_Pnt2d& Pnt) const {
   if (!WellDone) { throw StdFail_NotDone(); }
   else if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
   else {
     ParSol = par1sol(Index);
     ParArg = pararg1(Index);
     Pnt    = gp_Pnt2d(pnttg1sol(Index));
   }
 }
