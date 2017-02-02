// Created on: 1992-10-21
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


#include <GccAna_Lin2dTanObl.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dGcc_CurveTool.hxx>
#include <Geom2dGcc_IsParallel.hxx>
#include <Geom2dGcc_Lin2dTanObl.hxx>
#include <Geom2dGcc_Lin2dTanOblIter.hxx>
#include <Geom2dGcc_QCurve.hxx>
#include <Geom2dGcc_QualifiedCurve.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_NegativeValue.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

Geom2dGcc_Lin2dTanObl::
   Geom2dGcc_Lin2dTanObl (const Geom2dGcc_QualifiedCurve& Qualified1 ,
			  const gp_Lin2d&                 TheLine    ,
			  const Standard_Real             TolAng     ,
			  const Standard_Real             Angle      ):
  linsol(1,2)    ,
  qualifier1(1,2),
  pnttg1sol(1,2) ,
  pntint2sol(1,2),
  par1sol(1,2)   ,
  par2sol(1,2)   ,
  pararg1(1,2)   ,
  pararg2(1,2)   
{
  Geom2dAdaptor_Curve C1 = Qualified1.Qualified();
  Handle(Geom2d_Curve) CC1 = C1.Curve();
  GeomAbs_CurveType Type1 = C1.GetType();

//=============================================================================
//                            Appel a GccAna.                                 +
//=============================================================================

  WellDone = Standard_False;
  NbrSol = 0;
  if (Type1 == GeomAbs_Circle ) {
    Handle(Geom2d_Circle) CCC1 = Handle(Geom2d_Circle)::DownCast(CC1);
    gp_Circ2d c1(CCC1->Circ2d());
    GccEnt_QualifiedCirc Qc1=GccEnt_QualifiedCirc(c1,Qualified1.Qualifier());
    GccAna_Lin2dTanObl Lin(Qc1,TheLine,Angle);
    WellDone = Lin.IsDone();
    if(WellDone) { 
      NbrSol = Lin.NbSolutions();
      for (Standard_Integer i = 1 ; i <= NbrSol ; i++) {
	linsol(i)    = Lin.ThisSolution(i);
	Lin.Tangency1(i,par1sol(i),pararg1(i),pnttg1sol(i));
	Lin.Intersection2(i,par2sol(i),pararg2(i),pntint2sol(i));
	Lin.WhichQualifier(i,qualifier1(i));
      }
    }
  }
  else {
    Geom2dGcc_QCurve Qc1(C1,Qualified1.Qualifier());
    Standard_Real      aFirstPar  = Geom2dGcc_CurveTool::FirstParameter(C1);
    Standard_Real      aLastPar   = Geom2dGcc_CurveTool::LastParameter(C1);
    Standard_Integer   aNbSamples = Geom2dGcc_CurveTool::NbSamples(C1);
    Standard_Real      aStep      = (aLastPar - aFirstPar)/aNbSamples;
    Standard_Real      Param1     = aFirstPar;
    Standard_Integer   i;
    
    for (i = 0; i <= aNbSamples && NbrSol < 2; i++) {
      Geom2dGcc_Lin2dTanOblIter Lin(Qc1,TheLine,Param1,TolAng,Angle);

      if (Lin.IsDone()) {
	if (Add(NbrSol + 1, Lin, TolAng, C1))
	  NbrSol++;
      }

      Param1 += aStep;
    }

    WellDone = (NbrSol > 0);
  }
}

Geom2dGcc_Lin2dTanObl::
   Geom2dGcc_Lin2dTanObl (const Geom2dGcc_QualifiedCurve& Qualified1 ,
			  const gp_Lin2d&                 TheLine    ,
			  const Standard_Real             TolAng     ,
			  const Standard_Real             Param1     ,
			  const Standard_Real             Angle      ):
  linsol(1,2)    ,
  qualifier1(1,2),
  pnttg1sol(1,2) ,
  pntint2sol(1,2),
  par1sol(1,2)   ,
  par2sol(1,2)   ,
  pararg1(1,2)   ,
  pararg2(1,2)   
{
  Geom2dAdaptor_Curve C1 = Qualified1.Qualified();
  Handle(Geom2d_Curve) CC1 = C1.Curve();
  GeomAbs_CurveType Type1 = C1.GetType();

//=============================================================================
//                            Appel a GccAna.                                 +
//=============================================================================

  WellDone = Standard_False;
  NbrSol = 0;
  if (Type1 == GeomAbs_Circle ) {
    Handle(Geom2d_Circle) CCC1 = Handle(Geom2d_Circle)::DownCast(CC1);
    gp_Circ2d c1(CCC1->Circ2d());
    GccEnt_QualifiedCirc Qc1=GccEnt_QualifiedCirc(c1,Qualified1.Qualifier());
    GccAna_Lin2dTanObl Lin(Qc1,TheLine,Angle);
    WellDone = Lin.IsDone();
    if(WellDone) { 
      NbrSol = Lin.NbSolutions();
      for (Standard_Integer i = 1 ; i <= NbrSol ; i++) {
	linsol(i)    = Lin.ThisSolution(i);
	Lin.Tangency1(i,par1sol(i),pararg1(i),pnttg1sol(i));
	Lin.Intersection2(i,par2sol(i),pararg2(i),pntint2sol(i));
	Lin.WhichQualifier(i,qualifier1(i));
      }
    }
  }
  else {
    Geom2dGcc_QCurve Qc1(C1,Qualified1.Qualifier());
    Geom2dGcc_Lin2dTanOblIter Lin(Qc1,TheLine,TolAng,Param1,Angle);
    WellDone = Lin.IsDone();
    if(WellDone) { 
      linsol(1)    = Lin.ThisSolution();
      Lin.Tangency1(par1sol(1),pararg1(1),pnttg1sol(1));
      Lin.Intersection2(par2sol(1),pararg2(1),pntint2sol(1));
      Lin.WhichQualifier(qualifier1(1));
    }
  }
}

Standard_Boolean Geom2dGcc_Lin2dTanObl::
   IsDone () const { return WellDone; }

Standard_Integer Geom2dGcc_Lin2dTanObl::
   NbSolutions () const { return NbrSol; }

gp_Lin2d Geom2dGcc_Lin2dTanObl::
   ThisSolution (const Standard_Integer Index) const {

   if (Index > NbrSol || Index <= 0) { throw Standard_OutOfRange(); }
   return linsol(Index);
 }

void Geom2dGcc_Lin2dTanObl::
   WhichQualifier (const Standard_Integer Index,
		         GccEnt_Position& Qualif1) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
  else if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
  else { Qualif1 = qualifier1(Index);   }
}

void Geom2dGcc_Lin2dTanObl::
   Tangency1 (const Standard_Integer Index,
              Standard_Real& ParSol,
              Standard_Real& ParArg,
              gp_Pnt2d& PntSol) const {
   if (!WellDone) { throw StdFail_NotDone(); }
   else if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
   else {
     ParSol = par1sol(Index);
     ParArg = pararg1(Index);
     PntSol = pnttg1sol(Index);
   }
 }

void Geom2dGcc_Lin2dTanObl::
   Intersection2 (const Standard_Integer   Index  ,
                    Standard_Real&     ParSol ,
                    Standard_Real&     ParArg ,
                    gp_Pnt2d& PntSol ) const {
   if (!WellDone) { throw StdFail_NotDone(); }
   else if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
   else {
     ParSol = par2sol(Index);
     ParArg = pararg2(Index);
     PntSol = pntint2sol(Index);
   }
 }

Standard_Boolean Geom2dGcc_Lin2dTanObl::Add
                           (const Standard_Integer       theIndex,
			    const Geom2dGcc_Lin2dTanOblIter &theLin,
			    const Standard_Real          theTol,
			    const Geom2dAdaptor_Curve   &theC1)
{
  Standard_Integer i;
  Standard_Real    aPar1sol;
  Standard_Real    aPar2sol;
  Standard_Real    aPar1arg;
  Standard_Real    aPar2arg;
  gp_Pnt2d         aPnt1Sol;
  gp_Pnt2d         aPnt2Sol;
  gp_Lin2d         aLin   = theLin.ThisSolution();

  theLin.Tangency1(aPar1sol, aPar1arg, aPnt1Sol);
  theLin.Intersection2(aPar2sol, aPar2arg, aPnt2Sol);

  for(i = 1; i < theIndex; i++) {
    if (Abs(aPar1arg - pararg1(i)) <= theTol &&
	Abs(aPar2arg - pararg2(i)) <= theTol)
      return Standard_False;
  }

  gp_Dir2d aLinDir = aLin.Direction();
  gp_Vec2d aVTan;
  gp_Pnt2d aPoint;

  Geom2dGcc_CurveTool::D1(theC1, aPar1arg, aPoint, aVTan);

  if (Abs(aLinDir.Crossed(gp_Dir2d(aVTan))) > theTol)
    return Standard_False;

  linsol(theIndex)     = aLin;
  par1sol(theIndex)    = aPar1sol;
  pararg1(theIndex)    = aPar1arg;
  pnttg1sol(theIndex)  = aPnt1Sol;
  par2sol(theIndex)    = aPar2sol;
  pararg2(theIndex)    = aPar2arg;
  pntint2sol(theIndex) = aPnt2Sol;

  theLin.WhichQualifier(qualifier1(theIndex));

  return Standard_True;
}
