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

#include <GeomConvert_FuncSphereLSDist.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <math_Vector.hxx>

//=================================================================================================

GeomConvert_FuncSphereLSDist::GeomConvert_FuncSphereLSDist(
  const Handle(TColgp_HArray1OfXYZ)& thePoints)
    : myPoints(thePoints)
{
}

//=================================================================================================

Standard_Integer GeomConvert_FuncSphereLSDist::NbVariables() const
{
  return 4;
}

//=================================================================================================

Standard_Boolean GeomConvert_FuncSphereLSDist::Value(const math_Vector& X, Standard_Real& F)
{
  gp_XYZ        aLoc(X(1), X(2), X(3));
  Standard_Real anR2 = X(4) * X(4);

  F = 0.;
  Standard_Integer i;
  for (i = myPoints->Lower(); i <= myPoints->Upper(); ++i)
  {
    Standard_Real d = (myPoints->Value(i) - aLoc).SquareModulus() - anR2;
    F += d * d;
  }

  return Standard_True;
}

//=================================================================================================

Standard_Boolean GeomConvert_FuncSphereLSDist::Gradient(const math_Vector& X, math_Vector& G)

{
  gp_XYZ        aLoc(X(1), X(2), X(3));
  Standard_Real anR = X(4), anR2 = anR * anR;

  G.Init(0.);
  Standard_Integer i;
  for (i = myPoints->Lower(); i <= myPoints->Upper(); ++i)
  {
    gp_XYZ        dLoc = myPoints->Value(i) - aLoc;
    Standard_Real d    = dLoc.SquareModulus() - anR2;
    G(1) += d * dLoc.X();
    G(2) += d * dLoc.Y();
    G(3) += d * dLoc.Z();
    G(4) += d;
  }
  G *= -4;
  G(4) *= anR;

  return Standard_True;
}

//=================================================================================================

Standard_Boolean GeomConvert_FuncSphereLSDist::Values(const math_Vector& X,
                                                      Standard_Real&     F,
                                                      math_Vector&       G)
{
  gp_XYZ        aLoc(X(1), X(2), X(3));
  Standard_Real anR = X(4), anR2 = anR * anR;

  G.Init(0.);
  F = 0.;
  Standard_Integer i;
  for (i = myPoints->Lower(); i <= myPoints->Upper(); ++i)
  {
    gp_XYZ        dLoc = myPoints->Value(i) - aLoc;
    Standard_Real d    = dLoc.SquareModulus() - anR2;
    G(1) += d * dLoc.X();
    G(2) += d * dLoc.Y();
    G(3) += d * dLoc.Z();
    G(4) += d;
    F += d * d;
  }
  G *= -4;
  G(4) *= anR;

  return true;
}
