// Created on: 1995-07-18
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
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



#include <Extrema_GenLocateExtPS.ixx>
#include <Extrema_FuncExtPS.hxx>
#include <StdFail_NotDone.hxx>
#include <gp.hxx>
#include <math_FunctionSetRoot.hxx>
#include <math_NewtonFunctionSetRoot.hxx>
#include <math_Vector.hxx>

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

  math_FunctionSetRoot SR (F, Start,Tol, BInf, BSup);
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

Extrema_POnSurf Extrema_GenLocateExtPS::Point () const
{
  if (!IsDone()) { StdFail_NotDone::Raise(); }
  return myPoint;
}
//=============================================================================
