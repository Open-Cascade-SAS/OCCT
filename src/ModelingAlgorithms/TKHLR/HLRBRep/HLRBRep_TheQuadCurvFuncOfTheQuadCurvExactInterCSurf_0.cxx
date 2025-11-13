// Created on: 1992-10-14
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
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

#include <HLRBRep_TheQuadCurvFuncOfTheQuadCurvExactInterCSurf.hxx>

#include <IntSurf_Quadric.hxx>
#include <gp_Lin.hxx>
#include <HLRBRep_LineTool.hxx>
#include <gp_Vec.hxx>

HLRBRep_TheQuadCurvFuncOfTheQuadCurvExactInterCSurf::
  HLRBRep_TheQuadCurvFuncOfTheQuadCurvExactInterCSurf(const IntSurf_Quadric& Q, const gp_Lin& C)
    : myQuadric(Q),
      myCurve(C)
{
}

Standard_Boolean HLRBRep_TheQuadCurvFuncOfTheQuadCurvExactInterCSurf::Value(
  const Standard_Real Param,
  Standard_Real&      F)
{
  F = myQuadric.Distance(HLRBRep_LineTool::Value(myCurve, Param));
  return (Standard_True);
}

Standard_Boolean HLRBRep_TheQuadCurvFuncOfTheQuadCurvExactInterCSurf::Derivative(
  const Standard_Real Param,
  Standard_Real&      D)
{
  gp_Pnt P;
  gp_Vec T;
  HLRBRep_LineTool::D1(myCurve, Param, P, T);
  D = T.Dot(myQuadric.Gradient(P));
  return (Standard_True);
}

Standard_Boolean HLRBRep_TheQuadCurvFuncOfTheQuadCurvExactInterCSurf::Values(
  const Standard_Real Param,
  Standard_Real&      F,
  Standard_Real&      D)
{
  gp_Pnt P;
  gp_Vec T, Grad;
  HLRBRep_LineTool::D1(myCurve, Param, P, T);
  myQuadric.ValAndGrad(P, F, Grad);
  D = T.Dot(Grad);
  return (Standard_True);
}
