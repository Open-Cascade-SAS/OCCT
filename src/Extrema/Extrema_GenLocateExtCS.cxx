// Created on: 1996-01-25
// Created by: Laurent PAINNOT
// Copyright (c) 1996-1999 Matra Datavision
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



#include <Extrema_GenLocateExtCS.ixx>

#include <Extrema_FuncExtCS.hxx>
#include <math_Vector.hxx>
#include <math_FunctionSetRoot.hxx>
#include <math_NewtonFunctionSetRoot.hxx>

//=======================================================================
//function : Extrema_GenLocateExtCS
//purpose  : 
//=======================================================================

Extrema_GenLocateExtCS::Extrema_GenLocateExtCS()
{
}

//=======================================================================
//function : Extrema_GenLocateExtCS
//purpose  : 
//=======================================================================

 Extrema_GenLocateExtCS::Extrema_GenLocateExtCS(const Adaptor3d_Curve& C, 
						const Adaptor3d_Surface& S, 
						const Standard_Real T, 
						const Standard_Real U, 
						const Standard_Real V, 
						const Standard_Real Tol1, 
						const Standard_Real Tol2)
{
  Perform(C,S,T,U,V,Tol1,Tol2);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void Extrema_GenLocateExtCS::Perform(const Adaptor3d_Curve& C, 
				     const Adaptor3d_Surface& S, 
				     const Standard_Real T, 
				     const Standard_Real U, 
				     const Standard_Real V, 
				     const Standard_Real Tol1, 
				     const Standard_Real Tol2)
{
  myDone = Standard_False;

  Standard_Real Tinf, Tsup;
  Tinf = C.FirstParameter();
  Tsup = C.LastParameter();

  Standard_Real Uinf, Usup, Vinf, Vsup;
  Uinf = S.FirstUParameter();
  Usup = S.LastUParameter();
  Vinf = S.FirstVParameter();
  Vsup = S.LastVParameter();

  Extrema_FuncExtCS F (C,S);
  math_Vector Tol(1, 3), Start(1, 3), BInf(1, 3), BSup(1, 3);
  Tol(1) = Tol1;
  Tol(2) = Tol2;
  Tol(3) = Tol2;

  Start(1) = T;
  Start(2) = U;
  Start(3) = V;

  BInf(1) = Tinf;
  BInf(2) = Uinf;
  BInf(3) = Vinf;

  BSup(1) = Tsup;
  BSup(2) = Usup;
  BSup(3) = Vsup;

  math_FunctionSetRoot SR (F, Start,Tol, BInf, BSup);
  if (!SR.IsDone()) 
    return;

  mySqDist = F.SquareDistance(1);
  myPoint1 = F.PointOnCurve(1);
  myPoint2 = F.PointOnSurface(1);
  myDone = Standard_True;

}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean Extrema_GenLocateExtCS::IsDone() const 
{
  return myDone;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Real Extrema_GenLocateExtCS::SquareDistance() const 
{
  if (!IsDone()) { StdFail_NotDone::Raise(); }
  return mySqDist;
}

//=======================================================================
//function : PointOnCurve
//purpose  : 
//=======================================================================

const Extrema_POnCurv& Extrema_GenLocateExtCS::PointOnCurve() const 
{
  if (!IsDone()) { StdFail_NotDone::Raise(); }
  return myPoint1;
}

//=======================================================================
//function : PointOnSurface
//purpose  : 
//=======================================================================

const Extrema_POnSurf& Extrema_GenLocateExtCS::PointOnSurface() const 
{
  if (!IsDone()) { StdFail_NotDone::Raise(); }
  return myPoint2;
}

