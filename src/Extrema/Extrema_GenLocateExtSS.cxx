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


#include <Extrema_GenLocateExtSS.ixx>

#include <Extrema_FuncExtSS.hxx>
#include <math_Vector.hxx>
#include <math_FunctionSetRoot.hxx>
#include <math_NewtonFunctionSetRoot.hxx>

//=======================================================================
//function : Extrema_GenLocateExtSS
//purpose  : 
//=======================================================================

Extrema_GenLocateExtSS::Extrema_GenLocateExtSS()
{
}

//=======================================================================
//function : Extrema_GenLocateExtSS
//purpose  : 
//=======================================================================

 Extrema_GenLocateExtSS::Extrema_GenLocateExtSS(const Adaptor3d_Surface& S1, 
						const Adaptor3d_Surface& S2, 
						const Standard_Real U1, 
						const Standard_Real V1, 
						const Standard_Real U2, 
						const Standard_Real V2, 
						const Standard_Real Tol1, 
						const Standard_Real Tol2)
{
  Perform(S1,S2,U1,V1,U2,V2,Tol1,Tol2);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void Extrema_GenLocateExtSS::Perform(const Adaptor3d_Surface& S1, 
				     const Adaptor3d_Surface& S2, 
				     const Standard_Real U1, 
				     const Standard_Real V1, 
				     const Standard_Real U2, 
				     const Standard_Real V2, 
				     const Standard_Real Tol1, 
				     const Standard_Real Tol2)
{
  myDone = Standard_False;

  Standard_Real Uinf1, Usup1, Vinf1, Vsup1;
  Uinf1 = S1.FirstUParameter();
  Usup1 = S1.LastUParameter();
  Vinf1 = S1.FirstVParameter();
  Vsup1 = S1.LastVParameter();

  Standard_Real Uinf2, Usup2, Vinf2, Vsup2;
  Uinf2 = S2.FirstUParameter();
  Usup2 = S2.LastUParameter();
  Vinf2 = S2.FirstVParameter();
  Vsup2 = S2.LastVParameter();

  Extrema_FuncExtSS F (S1,S2);
  math_Vector Tol(1, 4), Start(1, 4), BInf(1, 4), BSup(1, 4);

  Tol(1) = Tol1;
  Tol(2) = Tol1;
  Tol(3) = Tol2;
  Tol(4) = Tol2;

  Start(1) = U1;
  Start(2) = V1;
  Start(3) = U2;
  Start(4) = V2;

  BInf(1) = Uinf1;
  BInf(2) = Vinf1;
  BInf(3) = Uinf2;
  BInf(4) = Vinf2;
  BSup(1) = Usup1;
  BSup(2) = Vsup1;
  BSup(3) = Usup2;
  BSup(4) = Vsup2;

  math_FunctionSetRoot SR (F, Start,Tol, BInf, BSup);
  if (!SR.IsDone()) 
    return;

  mySqDist = F.SquareDistance(1);
  myPoint1 = F.PointOnS1(1);
  myPoint2 = F.PointOnS2(1);
  myDone = Standard_True;

}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean Extrema_GenLocateExtSS::IsDone() const 
{
  return myDone;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Real Extrema_GenLocateExtSS::SquareDistance() const 
{
  if (!IsDone()) { StdFail_NotDone::Raise(); }
  return mySqDist;
}

//=======================================================================
//function : PointOnS1
//purpose  : 
//=======================================================================

const Extrema_POnSurf& Extrema_GenLocateExtSS::PointOnS1() const 
{
  if (!IsDone()) { StdFail_NotDone::Raise(); }
  return myPoint1;
}

//=======================================================================
//function : PointOnS2
//purpose  : 
//=======================================================================

const Extrema_POnSurf& Extrema_GenLocateExtSS::PointOnS2() const 
{
  if (!IsDone()) { StdFail_NotDone::Raise(); }
  return myPoint2;
}

