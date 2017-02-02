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
#include <Extrema_FuncExtSS.hxx>
#include <Extrema_GenLocateExtSS.hxx>
#include <Extrema_POnSurf.hxx>
#include <math_FunctionSetRoot.hxx>
#include <math_NewtonFunctionSetRoot.hxx>
#include <math_Vector.hxx>
#include <Standard_DomainError.hxx>
#include <StdFail_NotDone.hxx>

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

  math_FunctionSetRoot SR (F, Tol);
  SR.Perform(F, Start, BInf, BSup);
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
  if (!IsDone()) { throw StdFail_NotDone(); }
  return mySqDist;
}

//=======================================================================
//function : PointOnS1
//purpose  : 
//=======================================================================

const Extrema_POnSurf& Extrema_GenLocateExtSS::PointOnS1() const 
{
  if (!IsDone()) { throw StdFail_NotDone(); }
  return myPoint1;
}

//=======================================================================
//function : PointOnS2
//purpose  : 
//=======================================================================

const Extrema_POnSurf& Extrema_GenLocateExtSS::PointOnS2() const 
{
  if (!IsDone()) { throw StdFail_NotDone(); }
  return myPoint2;
}

