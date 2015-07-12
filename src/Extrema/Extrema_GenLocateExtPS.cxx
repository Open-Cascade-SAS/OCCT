// Created on: 1995-07-18
// Created by: Modelistation
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


#include <Adaptor3d_Surface.hxx>
#include <Extrema_FuncExtPS.hxx>
#include <Extrema_GenLocateExtPS.hxx>
#include <Extrema_POnSurf.hxx>
#include <gp.hxx>
#include <gp_Pnt.hxx>
#include <math_FunctionSetRoot.hxx>
#include <math_NewtonFunctionSetRoot.hxx>
#include <math_Vector.hxx>
#include <Standard_DomainError.hxx>
#include <StdFail_NotDone.hxx>

//=============================================================================
Extrema_GenLocateExtPS::Extrema_GenLocateExtPS () { myDone = Standard_False; }
//=============================================================================

Extrema_GenLocateExtPS::Extrema_GenLocateExtPS (const gp_Pnt&          P,
						const Adaptor3d_Surface& S, 
						const Standard_Real    U0, 
						const Standard_Real    V0,
						const Standard_Real    TolU, 
						const Standard_Real    TolV)
/*-----------------------------------------------------------------------------
Function:
  Find (U,V) close to (U0,V0) so that dist(S(U,V),P) was extreme.

Method:
  If (u,v) is a solution, it is possible to write:
   { F1(u,v) = (S(u,v)-P).dS/du(u,v) = 0.
   { F2(u,v) = (S(u,v)-P).dS/dv(u,v) = 0.
  The problem consists in finding, in the interval of surface definition,
  the root of the system closest to (U0,V0).
  Use class math_FunctionSetRoot with the following construction arguments:
  - F: Extrema_FuncExtPS created from P and S,
  - U0V0: math_Vector (U0,V0),
  - Tol: Min(TolU,TolV),            
				    
  - math_Vector (Uinf,Usup),
  - math_Vector (Vinf,Vsup),
  - 100. .
---------------------------------------------------------------------------*/  
{
  myDone = Standard_False;

  Standard_Real Uinf, Usup, Vinf, Vsup;
  Uinf = S.FirstUParameter();
  Usup = S.LastUParameter();
  Vinf = S.FirstVParameter();
  Vsup = S.LastVParameter();

  Extrema_FuncExtPS F (P,S);
  math_Vector Tol(1, 2), Start(1, 2), BInf(1, 2), BSup(1, 2);

  Tol(1) = TolU;
  Tol(2) = TolV;

  Start(1) = U0;
  Start(2) = V0;

  BInf(1) = Uinf;
  BInf(2) = Vinf;
  BSup(1) = Usup;
  BSup(2) = Vsup;

  math_FunctionSetRoot SR (F, Tol);
  SR.Perform(F, Start, BInf, BSup);
  if (!SR.IsDone()) 
    return;

  mySqDist = F.SquareDistance(1);
  myPoint = F.Point(1);
  myDone = Standard_True;
}
//=============================================================================

Standard_Boolean Extrema_GenLocateExtPS::IsDone () const { return myDone; }
//=============================================================================

Standard_Real Extrema_GenLocateExtPS::SquareDistance () const
{
  if (!IsDone()) { StdFail_NotDone::Raise(); }
  return mySqDist;
}
//=============================================================================

const Extrema_POnSurf& Extrema_GenLocateExtPS::Point () const
{
  if (!IsDone()) { StdFail_NotDone::Raise(); }
  return myPoint;
}
//=============================================================================
