// Created on: 1992-03-04
// Created by: Laurent BUCHARD
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

#include <IntCurve_MyImpParToolOfIntImpConicParConic.hxx>

#include <IntCurve_IConicTool.hxx>
#include <IntCurve_PConic.hxx>
#include <IntCurve_PConicTool.hxx>

#include <gp_Vec2d.hxx>

IntCurve_MyImpParToolOfIntImpConicParConic::IntCurve_MyImpParToolOfIntImpConicParConic(
  const IntCurve_IConicTool& ITool,
  const IntCurve_PConic&     PC)
    : TheImpTool(ITool)
{
  TheParCurve = (Standard_Address)(&PC);
}

Standard_Boolean IntCurve_MyImpParToolOfIntImpConicParConic::Value(const Standard_Real Param,
                                                                   Standard_Real& ApproxDistance)
{
  ApproxDistance =
    TheImpTool.Distance(IntCurve_PConicTool::Value((*((IntCurve_PConic*)(TheParCurve))), Param));
  return (Standard_True);
}

Standard_Boolean IntCurve_MyImpParToolOfIntImpConicParConic::Derivative(
  const Standard_Real Param,
  Standard_Real&      D_ApproxDistance_DV)
{
  gp_Pnt2d Pt;
  gp_Vec2d TanParCurve;
  gp_Vec2d Grad = TheImpTool.GradDistance(
    IntCurve_PConicTool::Value((*((IntCurve_PConic*)(TheParCurve))), Param));
  IntCurve_PConicTool::D1((*((IntCurve_PConic*)(TheParCurve))), Param, Pt, TanParCurve);
  D_ApproxDistance_DV = Grad.Dot(TanParCurve);
  return (Standard_True);
}

Standard_Boolean IntCurve_MyImpParToolOfIntImpConicParConic::Values(const Standard_Real Param,
                                                                    Standard_Real& ApproxDistance,
                                                                    Standard_Real& Deriv)
{
  this->Value(Param, ApproxDistance);
  this->Derivative(Param, Deriv);
  return (Standard_True);
}
