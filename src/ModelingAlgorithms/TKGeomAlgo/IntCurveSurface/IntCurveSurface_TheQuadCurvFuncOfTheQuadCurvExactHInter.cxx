// Created on: 1993-04-07
// Created by: Laurent BUCHARD
// Copyright (c) 1993-1999 Matra Datavision
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

#include <IntCurveSurface_TheQuadCurvFuncOfTheQuadCurvExactHInter.hxx>

#include <Adaptor3d_Curve.hxx>
#include <IntSurf_Quadric.hxx>
#include <IntCurveSurface_TheHCurveTool.hxx>
#include <gp_Vec.hxx>

IntCurveSurface_TheQuadCurvFuncOfTheQuadCurvExactHInter::
  IntCurveSurface_TheQuadCurvFuncOfTheQuadCurvExactHInter(const IntSurf_Quadric&         Q,
                                                          const occ::handle<Adaptor3d_Curve>& C)
    : myQuadric(Q),
      myCurve(C)
{
}

bool IntCurveSurface_TheQuadCurvFuncOfTheQuadCurvExactHInter::Value(
  const double Param,
  double&      F)
{
  F = myQuadric.Distance(IntCurveSurface_TheHCurveTool::Value(myCurve, Param));
  return (true);
}

bool IntCurveSurface_TheQuadCurvFuncOfTheQuadCurvExactHInter::Derivative(
  const double Param,
  double&      D)
{
  gp_Pnt P;
  gp_Vec T;
  IntCurveSurface_TheHCurveTool::D1(myCurve, Param, P, T);
  D = T.Dot(myQuadric.Gradient(P));
  return (true);
}

bool IntCurveSurface_TheQuadCurvFuncOfTheQuadCurvExactHInter::Values(
  const double Param,
  double&      F,
  double&      D)
{
  gp_Pnt P;
  gp_Vec T, Grad;
  IntCurveSurface_TheHCurveTool::D1(myCurve, Param, P, T);
  myQuadric.ValAndGrad(P, F, Grad);
  D = T.Dot(Grad);
  return (true);
}
