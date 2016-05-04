// Created on: 2016-05-10
// Created by: Alexander MALYSHEV
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2016 OPEN CASCADE SAS
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


#include <Extrema_FuncPSDist.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <math_Vector.hxx>

//=======================================================================
//function : Extrema_FuncPSDist
//purpose  : 
//=======================================================================
Extrema_FuncPSDist::Extrema_FuncPSDist(const Adaptor3d_Surface& theS,
                                       const gp_Pnt& theP)
: mySurf(theS),
  myP(theP)
{
}

//=======================================================================
//function : NbVariables
//purpose  : 
//=======================================================================
Standard_Integer Extrema_FuncPSDist::NbVariables () const
{
  return 2;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================
Standard_Boolean Extrema_FuncPSDist::Value(const math_Vector& X,Standard_Real& F)
{
  if (!IsInside(X))
    return Standard_False;

  F = mySurf.Value(X(1), X(2)).SquareDistance(myP);

  return Standard_True;
}

//=======================================================================
//function : Gradient
//purpose  : 
//=======================================================================
Standard_Boolean Extrema_FuncPSDist::Gradient(const math_Vector& X,math_Vector& G)

{
  if (!IsInside(X))
    return Standard_False;

  gp_Pnt aP;
  gp_Vec Du1s, Dv1s;
  mySurf.D1(X(1),X(2),aP,Du1s,Dv1s);

  gp_Vec P1P2 (aP, myP);

  G(1) = P1P2.Dot(Du1s);
  G(2) = P1P2.Dot(Dv1s);

  return Standard_True;
}

//=======================================================================
//function : Values
//purpose  : 
//=======================================================================
Standard_Boolean Extrema_FuncPSDist::Values(const math_Vector& X,Standard_Real& F,math_Vector& G)
{
  if (!IsInside(X))
    return Standard_False;

  gp_Pnt aP;
  gp_Vec Du1s, Dv1s;
  mySurf.D1(X(1),X(2),aP,Du1s,Dv1s);

  gp_Vec P1P2 (aP, myP);

  G(1) = P1P2.Dot(Du1s);
  G(2) = P1P2.Dot(Dv1s);

  F = mySurf.Value(X(1), X(2)).SquareDistance(myP);

  return true;
}

//=======================================================================
//function : IsInside
//purpose  : 
//=======================================================================
Standard_Boolean Extrema_FuncPSDist::IsInside(const math_Vector& X)
{
    if (X(1) < mySurf.FirstUParameter() ||
        X(1) > mySurf.LastUParameter() ||
        X(2) < mySurf.FirstUParameter() ||
        X(2) > mySurf.LastUParameter() )
    {
      // Point out of borders.
      return Standard_False;
    }

  // Point is inside.
  return Standard_True;
}
