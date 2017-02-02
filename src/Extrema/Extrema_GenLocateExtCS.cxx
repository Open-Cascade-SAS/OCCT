// Created on: 1996-01-25
// Created by: Laurent PAINNOT
// Copyright (c) 1996-1999 Matra Datavision
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


#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_Surface.hxx>
#include <Extrema_FuncExtCS.hxx>
#include <Extrema_GenLocateExtCS.hxx>
#include <Extrema_POnCurv.hxx>
#include <Extrema_POnSurf.hxx>
#include <math_FunctionSetRoot.hxx>
#include <math_NewtonFunctionSetRoot.hxx>
#include <math_Vector.hxx>
#include <Standard_DomainError.hxx>
#include <StdFail_NotDone.hxx>

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

  math_FunctionSetRoot SR (F, Tol);
  SR.Perform(F, Start, BInf, BSup);
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
  if (!IsDone()) { throw StdFail_NotDone(); }
  return mySqDist;
}

//=======================================================================
//function : PointOnCurve
//purpose  : 
//=======================================================================

const Extrema_POnCurv& Extrema_GenLocateExtCS::PointOnCurve() const 
{
  if (!IsDone()) { throw StdFail_NotDone(); }
  return myPoint1;
}

//=======================================================================
//function : PointOnSurface
//purpose  : 
//=======================================================================

const Extrema_POnSurf& Extrema_GenLocateExtCS::PointOnSurface() const 
{
  if (!IsDone()) { throw StdFail_NotDone(); }
  return myPoint2;
}

