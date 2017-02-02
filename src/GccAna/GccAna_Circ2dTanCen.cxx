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

//================================================================================
//    Creation of a circle tangent to an element and having center in a point    +
//================================================================================

#include <ElCLib.hxx>
#include <GccAna_Circ2dTanCen.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <gp.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_NegativeValue.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

//========================================================================
//     Creation of a circle tangent to a circle with center in a point.      +
//   - Calculate the distance between the center of the circle and the point of  +
//     center : dist                                                     +
//   - Check that this distance is compatible with the qualifier of the circle.                                                        +
//       Si yes, the radius of the solution will be :                           +
//          C1.Radius()-dist if the qualifier is Enclosed.              +
//          C1.Radius()+dist if the qualifier is Enclosing.             +
//          dist-C1.Radius() if the qualifier is Outside.               +
//          a mix of these values if the qualifier is Unqualified.  +
//========================================================================
GccAna_Circ2dTanCen::
   GccAna_Circ2dTanCen (const GccEnt_QualifiedCirc& Qualified1,
                        const gp_Pnt2d&             Pcenter   ,
			const Standard_Real                  Tolerance ):

//========================================================================
//   Initialization of fields.                                          +
//========================================================================

   cirsol(1,2)   ,
   qualifier1(1,2) ,
   TheSame1(1,2) ,
   pnttg1sol(1,2),
   par1sol(1,2),
   pararg1(1,2)
{

   NbrSol = 0;
   Standard_Real Radius = 0.0;
   WellDone = Standard_False;
   if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
	 Qualified1.IsOutside() || Qualified1.IsUnqualified())) {
     throw GccEnt_BadQualifier();
     return;
   }
   gp_Dir2d dirx(1.0,0.0);
   Standard_Real Tol = Abs(Tolerance);
   gp_Circ2d C1 = Qualified1.Qualified();
   Standard_Real R1 = C1.Radius();
   gp_Pnt2d center1(C1.Location());
   Standard_Real dist;
   Standard_Integer signe = 0;
   Standard_Integer signe1 = 0;

   if (!Qualified1.IsUnqualified()) {
     dist = Pcenter.Distance(center1);
     if (Qualified1.IsEnclosed()) {
//   ============================
       if (dist-R1 <= Tol) {
	 Radius = Abs(R1-dist);
	 signe = 1;
       }
       else { WellDone = Standard_True; }
     }
     else if (Qualified1.IsEnclosing()) {
//   =================================
       Radius = R1+dist;
       signe = -1;
     }
     else if (Qualified1.IsOutside()) {
//   ===============================
       if (dist < R1-Tol) { WellDone = Standard_True; }
       else {
	 Radius = Abs(R1-dist);
	 signe = -1;
       }
     }
     if (signe != 0) {
       NbrSol++;
       cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Pcenter,dirx),Radius);
//     ========================================================
       qualifier1(NbrSol) = Qualified1.Qualifier();
       if (dist <= gp::Resolution()) { TheSame1(NbrSol) = 1; }
       else {
	 TheSame1(NbrSol) = 0;
	 gp_Dir2d d(Pcenter.X()-center1.X(),Pcenter.Y()-center1.Y());
	 pnttg1sol(NbrSol) = gp_Pnt2d(Pcenter.XY()+signe*Radius*d.XY());
	 par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),pnttg1sol(NbrSol));
	 pararg1(NbrSol)=ElCLib::Parameter(C1,pnttg1sol(NbrSol));
       }
       WellDone = Standard_True;
     }
   }
   else {
// ====
     dist = Pcenter.Distance(center1);
     if (dist >= gp::Resolution()) {
       signe = 1;
       for (Standard_Integer i = 1; i <= 2 ; i++) {
	 signe = -signe;
	 if (R1-dist <= 0.) {
	   signe1 = -1;
	 }
	 else {
	   signe1 = -signe;
	 }
	 Radius = Abs(R1+signe*dist);
	 NbrSol++;
	 cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Pcenter,dirx),Radius);
//       ========================================================
	 Standard_Real distcc1 = Pcenter.Distance(center1);
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
	 TheSame1(NbrSol) = 0;
	 WellDone = Standard_True;
	 gp_Dir2d d(Pcenter.X()-center1.X(),Pcenter.Y()-center1.Y());
	 pnttg1sol(NbrSol) = gp_Pnt2d(Pcenter.XY()+signe1*Radius*d.XY());
	 par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),pnttg1sol(NbrSol));
	 pararg1(NbrSol)=ElCLib::Parameter(C1,pnttg1sol(NbrSol));
       }
     }
     else {
       NbrSol++;
       cirsol(NbrSol) = gp_Circ2d(C1);
//     ==============================
       qualifier1(1) = Qualified1.Qualifier();
       TheSame1(NbrSol) = 1;
       WellDone = Standard_True;
     }
   }
 }

//=========================================================================
//   Circle tangent to line Linetan and center in a point Pcenter.    +
//   Calculate the distance from the point to the line ==> Radius.                +
//   Create the circle with center Pcenter of radius Radius.                  +
//=========================================================================

GccAna_Circ2dTanCen::
   GccAna_Circ2dTanCen (const gp_Lin2d&            Linetan    ,
                        const gp_Pnt2d&            Pcenter    ):

//=========================================================================
//   Initialisation of fields.                                           +
//=========================================================================

   cirsol(1,1)   ,
   qualifier1(1,1),
   TheSame1(1,1) ,
   pnttg1sol(1,1),
   par1sol(1,1),
   pararg1(1,1) 
{

   gp_Dir2d dirx(1.0,0.0);
   Standard_Real rayon = Linetan.Distance(Pcenter);
   cirsol(1) = gp_Circ2d(gp_Ax2d(Pcenter,dirx),rayon);
// ==================================================
   qualifier1(1) = GccEnt_noqualifier;
   TheSame1(1) = 0;
   Standard_Real xloc = Linetan.Location().X();
   Standard_Real yloc = Linetan.Location().Y();
   Standard_Real xdir = Linetan.Direction().X();
   Standard_Real ydir = Linetan.Direction().Y();

   if (gp_Dir2d(xloc-Pcenter.X(),yloc-Pcenter.Y())
               .Dot(gp_Dir2d(-ydir,xdir)) > 0.0) {
     pnttg1sol(1) = gp_Pnt2d(Pcenter.XY()+rayon*gp_XY(-ydir,xdir));
     par1sol(1)=ElCLib::Parameter(cirsol(1),pnttg1sol(1));
     pararg1(1)=ElCLib::Parameter(Linetan,pnttg1sol(1));
   }
   else {
     pnttg1sol(1) = gp_Pnt2d(Pcenter.XY()+rayon*gp_XY(ydir,-xdir));
     par1sol(1)=ElCLib::Parameter(cirsol(1),pnttg1sol(1));
     pararg1(1)=ElCLib::Parameter(Linetan,pnttg1sol(1));
   }
   NbrSol = 1;
   WellDone = Standard_True;
 }

//=========================================================================
//   Circle tangent to point Point1 and centered in a point Pcenter.      +
//   Calculate the distance from Pcenter to Point1 ==> Radius.                +
//   Create the circle with center Pcenter of radius Radius.                  +
//=========================================================================

GccAna_Circ2dTanCen::
   GccAna_Circ2dTanCen (const gp_Pnt2d&            Point1     ,
                        const gp_Pnt2d&            Pcenter    ):

//=========================================================================
//   Initialisation of fields.                                           +
//=========================================================================

   cirsol(1,1)   ,
   qualifier1(1,1),
   TheSame1(1,1) ,
   pnttg1sol(1,1),
   par1sol(1,1)  ,
   pararg1(1,1)  
{

   gp_Dir2d dirx(1.0,0.0);
   Standard_Real rayon = Point1.Distance(Pcenter);
   cirsol(1) = gp_Circ2d(gp_Ax2d(Pcenter,dirx),rayon);
// =================================================
   qualifier1(1) = GccEnt_noqualifier;
   TheSame1(1) = 0;
   pnttg1sol(1) = Point1;
   par1sol(1)=ElCLib::Parameter(cirsol(1),pnttg1sol(1));
   pararg1(1) = 0.0;
   NbrSol = 1;
   WellDone = Standard_True;
 }

//=========================================================================

Standard_Boolean GccAna_Circ2dTanCen::
   IsDone () const { return WellDone; }

Standard_Integer GccAna_Circ2dTanCen::
   NbSolutions () const { return NbrSol; }

gp_Circ2d GccAna_Circ2dTanCen::
   ThisSolution (const Standard_Integer Index) const 
{
  if (Index > NbrSol || Index <= 0) { throw Standard_OutOfRange(); }
  return cirsol(Index);
}

void GccAna_Circ2dTanCen::
  WhichQualifier(const Standard_Integer Index   ,
		       GccEnt_Position& Qualif1 ) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
   else if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
   else {
     Qualif1 = qualifier1(Index);
   }
}

void GccAna_Circ2dTanCen::
   Tangency1 (const Standard_Integer Index,
              Standard_Real& ParSol,
              Standard_Real& ParArg,
              gp_Pnt2d& PntSol) const{
   if (!WellDone) { throw StdFail_NotDone(); }
   else if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
   else {
     if (TheSame1(Index) == 0) {
       PntSol = gp_Pnt2d(pnttg1sol(Index));
       ParSol = par1sol(Index);
       ParArg = pararg1(Index);
     }
     else { throw StdFail_NotDone(); }
   }
 }

Standard_Boolean GccAna_Circ2dTanCen::
   IsTheSame1 (const Standard_Integer Index) const
{
  if (!WellDone) 
    throw StdFail_NotDone();
  if (Index <= 0 ||Index > NbrSol) 
    throw Standard_OutOfRange();
 
  if (TheSame1(Index) == 0) 
    return Standard_False;

  return Standard_True;
}
