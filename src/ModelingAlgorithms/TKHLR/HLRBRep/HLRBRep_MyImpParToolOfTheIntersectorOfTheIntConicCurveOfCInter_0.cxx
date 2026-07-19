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

#include <HLRBRep_MyImpParToolOfTheIntersectorOfTheIntConicCurveOfCInter.hxx>

#include <IntCurve_IConicTool.hxx>
#include <HLRBRep_CurveTool.hxx>

#include <gp_Vec2d.hxx>

HLRBRep_MyImpParToolOfTheIntersectorOfTheIntConicCurveOfCInter::
  HLRBRep_MyImpParToolOfTheIntersectorOfTheIntConicCurveOfCInter(const IntCurve_IConicTool& ITool,
                                                                 const HLRBRep_CurvePtr&    PC)
    : TheImpTool(ITool)
{
  TheParCurve = (HLRBRep_CurvePtr)(&PC);
}

bool HLRBRep_MyImpParToolOfTheIntersectorOfTheIntConicCurveOfCInter::Value(const double Param,
                                                                           double& ApproxDistance)
{
  ApproxDistance =
    TheImpTool.Distance(HLRBRep_CurveTool::Value((*((HLRBRep_CurvePtr*)(TheParCurve))), Param));
  return (true);
}

bool HLRBRep_MyImpParToolOfTheIntersectorOfTheIntConicCurveOfCInter::Derivative(
  const double Param,
  double&      D_ApproxDistance_DV)
{
  gp_Pnt2d Pt;
  gp_Vec2d TanParCurve;
  gp_Vec2d Grad =
    TheImpTool.GradDistance(HLRBRep_CurveTool::Value((*((HLRBRep_CurvePtr*)(TheParCurve))), Param));
  HLRBRep_CurveTool::D1((*((HLRBRep_CurvePtr*)(TheParCurve))), Param, Pt, TanParCurve);
  D_ApproxDistance_DV = Grad.Dot(TanParCurve);
  return (true);
}

bool HLRBRep_MyImpParToolOfTheIntersectorOfTheIntConicCurveOfCInter::Values(const double Param,
                                                                            double& ApproxDistance,
                                                                            double& Deriv)
{
  this->Value(Param, ApproxDistance);
  this->Derivative(Param, Deriv);
  return (true);
}
