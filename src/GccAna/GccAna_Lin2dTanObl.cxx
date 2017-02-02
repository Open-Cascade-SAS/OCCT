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
//   CREATION of a STRAIGHT LINE TANGENT to a CIRCLE or PASSING by a POINT +
//                and MAKING ANGLE A with a STRAIGHT LINE.                   +
//=========================================================================

#include <ElCLib.hxx>
#include <GccAna_Lin2dTanObl.hxx>
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
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

//=========================================================================
//   Creation of a straight line passing by a point : ThePoint                +
//                         making an angle     : TheAngle                +
//                         with straight line  : TheLine.                +
//   Subject the straight line (ThePoint,TheLine.Location()) to a rotation +
//   by angle TheAngle ==> D1.                                            +
//   create straight line passing through ThePoint of direction D1.              +
//=========================================================================
GccAna_Lin2dTanObl::
   GccAna_Lin2dTanObl (const gp_Pnt2d&          ThePoint  ,
                       const gp_Lin2d&          TheLine   ,
                       const Standard_Real      TheAngle  ):
   linsol(1,1)    ,
   qualifier1(1,1),
   pnttg1sol(1,1) ,
   pntint2sol(1,1),
   par1sol(1,1)   ,
   par2sol(1,1)   ,
   pararg1(1,1)   ,
   pararg2(1,1)   
{

   Standard_Real Cosa = TheLine.Direction().X();
   Standard_Real Sina = TheLine.Direction().Y();
   linsol(1) = gp_Lin2d(ThePoint,
// ==============================
                        gp_Dir2d(Cosa*Cos(TheAngle)-Sina*Sin(TheAngle),
//                      ===============================================
				 Sina*Cos(TheAngle)+Sin(TheAngle)*Cosa));
//                               =======================================
   qualifier1(1) = GccEnt_noqualifier;
   pnttg1sol(1) = ThePoint;
   IntAna2d_AnaIntersection Intp(linsol(1),TheLine);
   if (Intp.IsDone()) {
     if (!Intp.IsEmpty()) {
       for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
	 pntint2sol(1) = Intp.Point(i).Value();
       }
     }
     par1sol(1)=ElCLib::Parameter(linsol(1),pnttg1sol(1));
     par2sol(1)=ElCLib::Parameter(linsol(1),pntint2sol(1));
     pararg1(1)=0.;
     pararg2(1)=ElCLib::Parameter(TheLine,pntint2sol(1));
     NbrSol = 1;
     WellDone = Standard_True;
   }
   else { 
     WellDone = Standard_False; 
     NbrSol=0;
   }
 }

//=========================================================================
//   Creation of a straight line tangent to a circle  : Qualified1 (C1)         +
//                         making angle           : TheAngle                +
//                         with a straight line   : TheLine.                +
//   Subject the straight line  (C1.Location,TheLine.Location()) to a       +
//   rotation by angle TheAngle or -TheAngle ==> D1.                       +
//   create the straight line passing by C1 of direction D1.                    +
//=========================================================================

GccAna_Lin2dTanObl::
   GccAna_Lin2dTanObl (const GccEnt_QualifiedCirc& Qualified1 ,
                       const gp_Lin2d&             TheLine    ,
                       const Standard_Real         TheAngle   ):
   linsol(1,2)   ,
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
   if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
	 Qualified1.IsOutside() || Qualified1.IsUnqualified())) {
     throw GccEnt_BadQualifier();
     return;
   }
   Standard_Real Cosa = TheLine.Direction().X();
   Standard_Real Sina = TheLine.Direction().Y();
   if (Qualified1.IsEnclosed()) {
// ============================
     throw GccEnt_BadQualifier();
   }
   else {
     gp_Circ2d C1 = Qualified1.Qualified();
     Standard_Real R1 = C1.Radius();
     if (Qualified1.IsEnclosing()) {
//   =============================
       gp_XY xy(Cos(TheAngle)*Cosa-Sin(TheAngle)*Sina,
	        Cos(TheAngle)*Sina+Sin(TheAngle)*Cosa);
       pnttg1sol(1) = gp_Pnt2d(C1.Location().XY()+R1*gp_XY(xy.Y(),-xy.X()));
       linsol(1) = gp_Lin2d(pnttg1sol(1),gp_Dir2d(xy));
//     ===============================================
       qualifier1(1) = Qualified1.Qualifier();
       IntAna2d_AnaIntersection Intp(linsol(1),TheLine);
       NbrSol = 1;
       WellDone = Standard_True;
       if (Intp.IsDone()) {
	 if (!Intp.IsEmpty()) {
	   for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
	     pntint2sol(1) = Intp.Point(i).Value();
	   }
	 }
       }
     }
     else if (Qualified1.IsOutside()) {
//   ================================
       gp_XY xy(Cos(TheAngle)*Cosa-Sin(TheAngle)*Sina,
		Cos(TheAngle)*Sina+Sin(TheAngle)*Cosa);
       pnttg1sol(1) = gp_Pnt2d(C1.Location().XY()+R1*gp_XY(-xy.Y(),xy.X()));
       linsol(1) = gp_Lin2d(pnttg1sol(1),gp_Dir2d(xy));
//     ===============================================
       qualifier1(1) = Qualified1.Qualifier();
       IntAna2d_AnaIntersection Intp(linsol(1),TheLine);
       WellDone = Standard_True;
       NbrSol = 1;
       if (Intp.IsDone()) {
	 if (!Intp.IsEmpty()) {
	   for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
	     pntint2sol(1) = Intp.Point(i).Value();
	   }
	 }
       }
     }
     else if (Qualified1.IsUnqualified()) {
//   ====================================
       gp_XY xy(Cos(TheAngle)*Cosa-Sin(TheAngle)*Sina,
		Cos(TheAngle)*Sina+Sin(TheAngle)*Cosa);
       pnttg1sol(1) = gp_Pnt2d(C1.Location().XY()+R1*gp_XY(xy.Y(),-xy.X()));
       linsol(1) = gp_Lin2d(pnttg1sol(1),gp_Dir2d(xy));
//     ===============================================
       qualifier1(1) = GccEnt_enclosing;
       IntAna2d_AnaIntersection Intp(linsol(1),TheLine);
       WellDone = Standard_True;
       NbrSol=1;
       if (Intp.IsDone()) {
	 if (!Intp.IsEmpty()) {
	   for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
	     pntint2sol(1) = Intp.Point(i).Value();
	   }
	 }
       }
       pnttg1sol(2) = gp_Pnt2d(C1.Location().XY()+R1*gp_XY(-xy.Y(),xy.X()));
       linsol(2) = gp_Lin2d(pnttg1sol(2),gp_Dir2d(xy));
//     ===============================================
       qualifier1(2) = GccEnt_outside;
       Intp = IntAna2d_AnaIntersection(linsol(1),TheLine);
       NbrSol++;
       WellDone = Standard_True;
       if (Intp.IsDone()) {
	 if (!Intp.IsEmpty()) {
	   for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
	     pntint2sol(2) = Intp.Point(i).Value();
	   }
	 }
       }
     }
     for (Standard_Integer index = 1; index <= NbrSol; index++) {
       par1sol(index)=ElCLib::Parameter(linsol(index),pnttg1sol(index));
       pararg1(index)=ElCLib::Parameter(C1,pnttg1sol(index));
       par2sol(index)=ElCLib::Parameter(linsol(index),pntint2sol(index));
       pararg2(index)=ElCLib::Parameter(TheLine,pntint2sol(index));
     }
   }
 }

Standard_Boolean GccAna_Lin2dTanObl::
   IsDone () const { return WellDone; }

Standard_Integer GccAna_Lin2dTanObl::
   NbSolutions () const 
{ 
  if (!WellDone) throw StdFail_NotDone();
  return NbrSol;
}

gp_Lin2d GccAna_Lin2dTanObl::
   ThisSolution (const Standard_Integer Index) const 
{
  if (!WellDone) 
    throw StdFail_NotDone();
  if (Index <= 0 || Index > NbrSol) 
    throw Standard_OutOfRange();

  return linsol(Index);
}

void GccAna_Lin2dTanObl::
  WhichQualifier(const Standard_Integer Index   ,
		       GccEnt_Position& Qualif1 ) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
   else if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
   else {
     Qualif1 = qualifier1(Index);
   }
}

void GccAna_Lin2dTanObl::
   Tangency1 (const Standard_Integer   Index,
                    Standard_Real&     ParSol,
                    Standard_Real&     ParArg,
                    gp_Pnt2d& PntSol) const{
   if (!WellDone) { throw StdFail_NotDone(); }
   else if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
   else {
     ParSol = par1sol(Index);
     ParArg = pararg1(Index);
     PntSol = gp_Pnt2d(pnttg1sol(Index));
   }
 }

void GccAna_Lin2dTanObl::
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


