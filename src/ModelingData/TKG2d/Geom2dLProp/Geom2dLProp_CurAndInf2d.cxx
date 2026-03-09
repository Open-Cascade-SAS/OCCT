// Created on: 1994-09-06
// Created by: Yves FRICAUD
// Copyright (c) 1994-1999 Matra Datavision
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

#include <Geom2dLProp_CurAndInf2d.hxx>

#include <Geom2dProp_Curve.hxx>

//=================================================================================================

Geom2dLProp_CurAndInf2d::Geom2dLProp_CurAndInf2d()
    : isDone(false)
{
}

//=================================================================================================

void Geom2dLProp_CurAndInf2d::Perform(const occ::handle<Geom2d_Curve>& C)
{
  Clear();
  isDone = true;
  performCurExt(C);
  performInf(C);
}

//=================================================================================================

void Geom2dLProp_CurAndInf2d::PerformCurExt(const occ::handle<Geom2d_Curve>& C)
{
  Clear();
  isDone = true;
  performCurExt(C);
}

//=================================================================================================

void Geom2dLProp_CurAndInf2d::performCurExt(const occ::handle<Geom2d_Curve>& theCurve)
{
  Geom2dProp_Curve             aProp(theCurve);
  Geom2dProp::CurveAnalysis    aResult = aProp.FindCurvatureExtrema();

  if (!aResult.IsDone)
  {
    isDone = false;
    return;
  }

  for (const Geom2dProp::CurveSpecialPoint& aPt : aResult.Points)
  {
    if (aPt.Type == Geom2dProp::CIType::MinCurvature)
    {
      AddExtCur(aPt.Parameter, true);
    }
    else if (aPt.Type == Geom2dProp::CIType::MaxCurvature)
    {
      AddExtCur(aPt.Parameter, false);
    }
  }
}

//=================================================================================================

void Geom2dLProp_CurAndInf2d::PerformInf(const occ::handle<Geom2d_Curve>& C)
{
  Clear();
  isDone = true;
  performInf(C);
}

//=================================================================================================

void Geom2dLProp_CurAndInf2d::performInf(const occ::handle<Geom2d_Curve>& theCurve)
{
  Geom2dProp_Curve             aProp(theCurve);
  Geom2dProp::CurveAnalysis    aResult = aProp.FindInflections();

  if (!aResult.IsDone)
  {
    isDone = false;
    return;
  }

  for (const Geom2dProp::CurveSpecialPoint& aPt : aResult.Points)
  {
    if (aPt.Type == Geom2dProp::CIType::Inflection)
    {
      AddInflection(aPt.Parameter);
    }
  }
}

//=================================================================================================

bool Geom2dLProp_CurAndInf2d::IsDone() const
{
  return isDone;
}
