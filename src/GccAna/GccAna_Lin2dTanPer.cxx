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
//   CREATION of a STRAIGHT LINE TANGENT to a CIRCLE or PASSING by a POINT   +
//                      and ORTHOGONAL to a STRAIGHT LINE.                      +
//=========================================================================

#include <ElCLib.hxx>
#include <GccAna_Lin2dTanPer.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_XY.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

//=========================================================================
//   Straight line passing by point  : ThePoint and                       +
//       orthogonal to straight line : TheLin.                            +
//   Create the straight line of origin     : ThePoint                           +
//                  and direction  : TheLin.Direction() turned by 90   +
//=========================================================================
GccAna_Lin2dTanPer::
   GccAna_Lin2dTanPer (const gp_Pnt2d& ThePnt    ,
                       const gp_Lin2d& TheLin    ):
   linsol(1,1),
   qualifier1(1,1) ,
   pnttg1sol(1,1),
   pntint2sol(1,1),
   par1sol(1,1),
   par2sol(1,1),
   pararg1(1,1),
   pararg2(1,1)
{

   linsol(1) = gp_Lin2d(ThePnt,gp_Dir2d(-(TheLin.Direction().Y()),
// ===============================================================
					TheLin.Direction().X()));
//                                      ========================
   pnttg1sol(1) = ThePnt;
   IntAna2d_AnaIntersection Intp(linsol(1),TheLin);
   if (Intp.IsDone()) {
     if (!Intp.IsEmpty()) {
       for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
	 pntint2sol(1) = Intp.Point(i).Value();
       }
     }
   }
   par1sol(1) = ElCLib::Parameter(linsol(1),pnttg1sol(1));
   par2sol(1) = ElCLib::Parameter(linsol(1),pntint2sol(1));
   pararg1(1) = 0.;
   pararg2(1) = ElCLib::Parameter(TheLin,pntint2sol(1));
   NbrSol = 1;
   WellDone = Standard_True;
 }

//=========================================================================
//   Straight line passing by point      : ThePnt                             +
//       and orthogonal to circle        : TheCircle.                         +
//   Create the straight line of origin  : ThePoint                           +
//                  and direction        : (TheCircle.Location(),ThePnt).     +
//=========================================================================

GccAna_Lin2dTanPer::
   GccAna_Lin2dTanPer (const gp_Pnt2d&  ThePnt    ,
                       const gp_Circ2d& TheCircle ):
   linsol(1,1),
   qualifier1(1,1) ,
   pnttg1sol(1,1),
   pntint2sol(1,1),
   par1sol(1,1),
   par2sol(1,1),
   pararg1(1,1),
   pararg2(1,1)
{

   linsol(1) = gp_Lin2d(ThePnt,
// ============================
                        gp_Dir2d(TheCircle.Location().XY()-ThePnt.XY()));
//                      ================================================
   pnttg1sol(1) = ThePnt;
   IntAna2d_AnaIntersection Intp(linsol(1),TheCircle);
   if (Intp.IsDone()) {
     if (!Intp.IsEmpty()) {
       Standard_Real maxdist = RealLast();
       for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
	 if (Intp.Point(i).Value().Distance(ThePnt) < maxdist) {
	   pntint2sol(1) = Intp.Point(i).Value();
	 }
       }
     }
   }
   par1sol(1) = ElCLib::Parameter(linsol(1),pnttg1sol(1));
   par2sol(1) = ElCLib::Parameter(linsol(1),pntint2sol(1));
   pararg1(1) = 0.;
   pararg2(1) = ElCLib::Parameter(TheCircle,pntint2sol(1));
   NbrSol = 1;
   WellDone = Standard_True;
 }

//=========================================================================
//   Straight line tangent to circle     : Qualified1 (C1)                    +
//   and orthogonal to straight line     : TheLin.                            +
//   Create straight line of origin      : P1 (on C1)                         +
//                  and direction        : TheLin.Direction() turned by 90`   +
//=========================================================================

GccAna_Lin2dTanPer::
   GccAna_Lin2dTanPer (const GccEnt_QualifiedCirc& Qualified1,
                       const gp_Lin2d&             TheLin    ):
   linsol(1,2),
   qualifier1(1,2) ,
   pnttg1sol(1,2),
   pntint2sol(1,2),
   par1sol(1,2),
   par2sol(1,2),
   pararg1(1,2),
   pararg2(1,2)
{

   WellDone = Standard_False;
   Standard_Integer nbsol = 0;
   Standard_Integer signe = 0;
   NbrSol = 0;
   gp_Circ2d C1 = Qualified1.Qualified();

   if (Qualified1.IsEnclosed()) {
// ============================
     throw GccEnt_BadQualifier();
   }
   else if (Qualified1.IsEnclosing()) {
// ==================================
     nbsol = 1;
     signe = -1;
   }
   else if (Qualified1.IsOutside()) {
// ================================
     nbsol = 1;
     signe = 1;
   }
   else {
     nbsol = 2;
     signe = -1;
   }
   gp_XY xy(C1.Radius()*TheLin.Direction().XY());
   for (Standard_Integer j = 1 ; j <= nbsol ; j++) {
     signe = -signe;
     NbrSol++;
     linsol(NbrSol)=gp_Lin2d(gp_Pnt2d((C1.Location().XY()).Added(signe*xy)),
//   =======================================================================
			     gp_Dir2d(-TheLin.Direction().Y(),
//                           =================================
				      TheLin.Direction().X()));
//                                    ========================
     pnttg1sol(NbrSol) = gp_Pnt2d((C1.Location().XY()).Added(signe*xy));
     IntAna2d_AnaIntersection Intp(linsol(NbrSol),TheLin);
     if (Intp.IsDone()) {
       if (!Intp.IsEmpty()) {
	 for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
	   pntint2sol(NbrSol) = Intp.Point(i).Value();
	 }
       }
     }
     par1sol(NbrSol) = ElCLib::Parameter(linsol(NbrSol),pnttg1sol(NbrSol));
     par2sol(NbrSol) = ElCLib::Parameter(linsol(NbrSol),pntint2sol(NbrSol));
     pararg1(NbrSol) = ElCLib::Parameter(C1,pnttg1sol(NbrSol));
     pararg2(NbrSol) = ElCLib::Parameter(TheLin,pntint2sol(NbrSol));
     WellDone = Standard_True;
   }
 }

//=========================================================================
//   Straight line tangent to circle     : Qualified1 (C1)                    +
//       and orthogonal to circle        : TheCircle.                         +
//   Create straight line of origin      : P1 (on C1)                         +
//                  and direction        : TheLin.Direction() turned by 90`   +
//=========================================================================

GccAna_Lin2dTanPer::
   GccAna_Lin2dTanPer (const GccEnt_QualifiedCirc& Qualified1,
                       const gp_Circ2d&            TheCircle ):
   linsol(1,2),
   qualifier1(1,2) ,
   pnttg1sol(1,2),
   pntint2sol(1,2),
   par1sol(1,2),
   par2sol(1,2),
   pararg1(1,2),
   pararg2(1,2)
{

   WellDone = Standard_False;
   NbrSol = 0;
   Standard_Integer signe = 0;
   gp_Circ2d C1 = Qualified1.Qualified();

   if (Qualified1.IsEnclosed()) {
// ============================
     throw GccEnt_BadQualifier();
   }
   else if (Qualified1.IsEnclosing()) {
// ==================================
     signe = -1;
     qualifier1(1) = GccEnt_enclosing;
   }
   else if (Qualified1.IsOutside()) {
// ================================
     signe = 1;
     qualifier1(1) = GccEnt_outside;
   }
   else if (Qualified1.IsUnqualified()) {
// ====================================
     signe = -1;
     qualifier1(1) = GccEnt_enclosing;
     qualifier1(2) = GccEnt_outside;
   }
   for (Standard_Integer j = 1 ; j <= 2 ; j++) {
     NbrSol++;
     signe = -signe;
     gp_Dir2d D1(TheCircle.Location().XY()-C1.Location().XY());
     linsol(NbrSol) = gp_Lin2d(gp_Pnt2d((C1.Location().XY())+
//   ===================================================
		       signe*(D1.XY()*C1.Radius())),gp_Dir2d(-D1.Y(),D1.X()));
//                     ======================================================
     pnttg1sol(NbrSol) = gp_Pnt2d((C1.Location().XY())+
				  signe*(D1.XY()*C1.Radius()));
     IntAna2d_AnaIntersection Intp(linsol(NbrSol),TheCircle);
     if (Intp.IsDone()) {
       if (!Intp.IsEmpty()) {
	 Standard_Real maxdist = RealLast();
	 for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
	   if (Intp.Point(i).Value().Distance(pnttg1sol(NbrSol)) < maxdist) {
	     pntint2sol(NbrSol) = Intp.Point(i).Value();
	   }
	 }
       }
     }
     par1sol(NbrSol) = ElCLib::Parameter(linsol(NbrSol),pnttg1sol(NbrSol));
     par2sol(NbrSol) = ElCLib::Parameter(linsol(NbrSol),pntint2sol(NbrSol));
     pararg1(NbrSol) = ElCLib::Parameter(C1,pnttg1sol(NbrSol));
     pararg2(NbrSol) = ElCLib::Parameter(TheCircle,pntint2sol(NbrSol));
     WellDone = Standard_True;
   }
 }

Standard_Boolean GccAna_Lin2dTanPer::
   IsDone () const { return WellDone; }

Standard_Integer GccAna_Lin2dTanPer::
   NbSolutions () const 
{
  if (!WellDone) { throw StdFail_NotDone(); } 
  return NbrSol;
}

gp_Lin2d GccAna_Lin2dTanPer::
   ThisSolution (const Standard_Integer Index) const 
{
  if (!WellDone) { throw StdFail_NotDone(); } 
  if (Index <= 0 || Index > NbrSol) { throw Standard_RangeError(); }
  return linsol(Index);
}

void GccAna_Lin2dTanPer::
  WhichQualifier(const Standard_Integer Index   ,
		       GccEnt_Position& Qualif1 ) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
  if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
  else {
    Qualif1 = qualifier1(Index);
  }
}

void GccAna_Lin2dTanPer::
   Tangency1 (const Standard_Integer Index,
              Standard_Real& ParSol,
              Standard_Real& ParArg,
              gp_Pnt2d& Pnt) const{
   if (!WellDone) { throw StdFail_NotDone(); }
   else if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
   else {
     ParSol = par1sol(Index);
     ParArg = pararg1(Index);
     Pnt    = gp_Pnt2d(pnttg1sol(Index));
   }
 }

void GccAna_Lin2dTanPer::
   Intersection2 (const Standard_Integer Index,
                  Standard_Real& ParSol,
                  Standard_Real& ParArg,
                  gp_Pnt2d& PntSol) const {
   if (!WellDone) { throw StdFail_NotDone(); }
   else if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
   else {
     ParSol = par2sol(Index);
     ParArg = pararg2(Index);
     PntSol = gp_Pnt2d(pntint2sol(Index));
   }
 }

