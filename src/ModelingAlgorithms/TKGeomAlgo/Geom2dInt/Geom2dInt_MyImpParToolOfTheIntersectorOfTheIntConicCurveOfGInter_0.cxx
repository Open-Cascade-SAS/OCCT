// Created on: 1992-06-04
// Created by: Jacques GOUSSARD
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

#include <Geom2dInt_MyImpParToolOfTheIntersectorOfTheIntConicCurveOfGInter.hxx>

#include <IntCurve_IConicTool.hxx>
#include <Adaptor2d_Curve2d.hxx>
#include <Geom2dInt_Geom2dCurveTool.hxx>

#include <gp_Vec2d.hxx>

Geom2dInt_MyImpParToolOfTheIntersectorOfTheIntConicCurveOfGInter::
  Geom2dInt_MyImpParToolOfTheIntersectorOfTheIntConicCurveOfGInter(const IntCurve_IConicTool& ITool,
                                                                   const Adaptor2d_Curve2d&   PC)
    : TheImpTool(ITool)
{
  TheParCurve = (void*)(&PC);
}

bool Geom2dInt_MyImpParToolOfTheIntersectorOfTheIntConicCurveOfGInter::Value(const double Param,
                                                                             double& ApproxDistance)
{
  ApproxDistance = TheImpTool.Distance(
    Geom2dInt_Geom2dCurveTool::Value((*((Adaptor2d_Curve2d*)(TheParCurve))), Param));
  return (true);
}

bool Geom2dInt_MyImpParToolOfTheIntersectorOfTheIntConicCurveOfGInter::Derivative(
  const double Param,
  double&      D_ApproxDistance_DV)
{
  gp_Pnt2d Pt;
  gp_Vec2d TanParCurve;
  gp_Vec2d Grad = TheImpTool.GradDistance(
    Geom2dInt_Geom2dCurveTool::Value((*((Adaptor2d_Curve2d*)(TheParCurve))), Param));
  Geom2dInt_Geom2dCurveTool::D1((*((Adaptor2d_Curve2d*)(TheParCurve))), Param, Pt, TanParCurve);
  D_ApproxDistance_DV = Grad.Dot(TanParCurve);
  return (true);
}

bool Geom2dInt_MyImpParToolOfTheIntersectorOfTheIntConicCurveOfGInter::Values(
  const double Param,
  double&      ApproxDistance,
  double&      Deriv)
{
  this->Value(Param, ApproxDistance);
  this->Derivative(Param, Deriv);
  return (true);
}
