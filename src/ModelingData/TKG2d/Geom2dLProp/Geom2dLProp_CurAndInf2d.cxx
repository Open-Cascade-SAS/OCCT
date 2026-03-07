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
#include <Geom2dProp.hxx>
#include <Geom2dProp_Curve.hxx>

namespace
{
// Maps the new explicit Geom2dProp analysis result to the legacy LProp enum.
static LProp_CIType mapPointType(const Geom2dProp::CIType theType)
{
  switch (theType)
  {
    case Geom2dProp::CIType::Inflection:
      return LProp_Inflection;
    case Geom2dProp::CIType::MinCurvature:
      return LProp_MinCur;
    case Geom2dProp::CIType::MaxCurvature:
      return LProp_MaxCur;
  }

  return LProp_Inflection;
}

static void appendAnalysis(const Geom2dProp::CurveAnalysis& theAnalysis,
                           const bool                       theInflectionsOnly,
                           LProp_CurAndInf&                 theResult)
{
  if (!theAnalysis.IsDone)
  {
    return;
  }

  for (int anIndex = 0; anIndex < theAnalysis.Points.Length(); ++anIndex)
  {
    const Geom2dProp::CurveSpecialPoint& aPoint = theAnalysis.Points.Value(anIndex);
    if (theInflectionsOnly)
    {
      if (aPoint.Type == Geom2dProp::CIType::Inflection)
      {
        theResult.AddInflection(aPoint.Parameter);
      }
      continue;
    }

    if (aPoint.Type == Geom2dProp::CIType::Inflection)
    {
      continue;
    }
    theResult.AddExtCur(aPoint.Parameter, mapPointType(aPoint.Type) == LProp_MinCur);
  }
}
} // namespace

//=================================================================================================

Geom2dLProp_CurAndInf2d::Geom2dLProp_CurAndInf2d()
    : isDone(false)
{
}

//=================================================================================================

void Geom2dLProp_CurAndInf2d::Perform(const occ::handle<Geom2d_Curve>& C)
{
  Clear();

  const Geom2dProp_Curve          aProp(C);
  const Geom2dProp::CurveAnalysis anExtrema = aProp.FindCurvatureExtrema();
  const Geom2dProp::CurveAnalysis anInflect = aProp.FindInflections();

  isDone = anExtrema.IsDone && anInflect.IsDone;
  appendAnalysis(anExtrema, false, *this);
  appendAnalysis(anInflect, true, *this);
}

//=================================================================================================

void Geom2dLProp_CurAndInf2d::PerformCurExt(const occ::handle<Geom2d_Curve>& C)
{
  Clear();

  const Geom2dProp_Curve          aProp(C);
  const Geom2dProp::CurveAnalysis anAnalysis = aProp.FindCurvatureExtrema();
  isDone                                     = anAnalysis.IsDone;
  appendAnalysis(anAnalysis, false, *this);
}

//=================================================================================================

void Geom2dLProp_CurAndInf2d::PerformInf(const occ::handle<Geom2d_Curve>& C)
{
  Clear();

  const Geom2dProp_Curve          aProp(C);
  const Geom2dProp::CurveAnalysis anAnalysis = aProp.FindInflections();
  isDone                                     = anAnalysis.IsDone;
  appendAnalysis(anAnalysis, true, *this);
}

//=================================================================================================

bool Geom2dLProp_CurAndInf2d::IsDone() const
{
  return isDone;
}
