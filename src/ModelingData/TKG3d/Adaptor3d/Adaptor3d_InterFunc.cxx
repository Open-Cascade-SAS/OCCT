// Created on: 1998-02-18
// Created by: Jeanine PANCIATICI
// Copyright (c) 1998-1999 Matra Datavision
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

#include <Adaptor3d_InterFunc.hxx>

#include <Adaptor2d_Curve2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Standard_ConstructionError.hxx>

Adaptor3d_InterFunc::Adaptor3d_InterFunc(const occ::handle<Adaptor2d_Curve2d>& C,
                                         const double                          FixVal,
                                         const int                             Fix)
    : myCurve2d(C),
      myFixVal(FixVal),
      myFix(Fix)
{
  if (Fix != 1 && Fix != 2)
    throw Standard_ConstructionError();
}

bool Adaptor3d_InterFunc::Value(const double X, double& F)
{
  gp_Pnt2d C;
  myCurve2d->D0(X, C);
  if (myFix == 1)
    F = C.X() - myFixVal;
  else
    F = C.Y() - myFixVal;

  return true;
}

bool Adaptor3d_InterFunc::Derivative(const double X, double& D)
{
  double F;
  return Values(X, F, D);
}

bool Adaptor3d_InterFunc::Values(const double X, double& F, double& D)
{
  gp_Pnt2d C;
  gp_Vec2d DC;
  myCurve2d->D1(X, C, DC);
  if (myFix == 1)
  {
    F = C.X() - myFixVal;
    D = DC.X();
  }
  else
  {
    F = C.Y() - myFixVal;
    D = DC.Y();
  }
  return true;
}
