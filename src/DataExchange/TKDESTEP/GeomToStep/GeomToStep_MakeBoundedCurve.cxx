// Created on: 1993-06-21
// Created by: Martine LANGLOIS
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

#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BoundedCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2dConvert.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BoundedCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomConvert.hxx>
#include <GeomToStep_MakeBoundedCurve.hxx>
#include <GeomToStep_MakeBSplineCurveWithKnots.hxx>
#include <GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve.hxx>
#include <StdFail_NotDone.hxx>
#include <StepData_Factors.hxx>
#include <StepGeom_BoundedCurve.hxx>
#include <StepGeom_BSplineCurveWithKnots.hxx>
#include <StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve.hxx>

//=============================================================================
// Creation d' une BoundedCurve de prostep a partir d' une BoundedCurve de Geom
//=============================================================================
GeomToStep_MakeBoundedCurve::GeomToStep_MakeBoundedCurve(const occ::handle<Geom_BoundedCurve>& C,
                                                         const StepData_Factors& theLocalFactors)
{
  done = true;

  if (C->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
  {
    occ::handle<Geom_BSplineCurve> Bspli = occ::down_cast<Geom_BSplineCurve>(C);
    // UPDATE FMA 1-04-96
    if (C->IsPeriodic())
    {
      occ::handle<Geom_BSplineCurve> newBspli = occ::down_cast<Geom_BSplineCurve>(Bspli->Copy());
      newBspli->SetNotPeriodic();
      Bspli = newBspli;
    }
    if (Bspli->IsRational())
    {
      GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve MkRatBSplineC(Bspli,
                                                                                theLocalFactors);
      theBoundedCurve = MkRatBSplineC.Value();
    }
    else
    {
      GeomToStep_MakeBSplineCurveWithKnots MkBSplineC(Bspli, theLocalFactors);
      theBoundedCurve = MkBSplineC.Value();
    }
  }
  else if (C->IsKind(STANDARD_TYPE(Geom_BezierCurve)))
  {
    occ::handle<Geom_BezierCurve>  Cur   = occ::down_cast<Geom_BezierCurve>(C);
    occ::handle<Geom_BSplineCurve> Bspli = GeomConvert::CurveToBSplineCurve(Cur);
    if (Bspli->IsRational())
    {
      GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve MkRatBSplineC(Bspli,
                                                                                theLocalFactors);
      theBoundedCurve = MkRatBSplineC.Value();
    }
    else
    {
      GeomToStep_MakeBSplineCurveWithKnots MkBSplineC(Bspli, theLocalFactors);
      theBoundedCurve = MkBSplineC.Value();
    }
  }
  else
  {
#ifdef OCCT_DEBUG
    std::cout << "MakeBoundedCurve, type : " << C->DynamicType()->Name() << " not processed"
              << std::endl;
#endif
    done = false;
  }
}

//=============================================================================
// Creation d' une BoundedCurve de prostep a partir d' une BoundedCurve de
// Geom2d
//=============================================================================

GeomToStep_MakeBoundedCurve::GeomToStep_MakeBoundedCurve(const occ::handle<Geom2d_BoundedCurve>& C,
                                                         const StepData_Factors& theLocalFactors)
{
  done = true;
  if (C->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)))
  {
    occ::handle<Geom2d_BSplineCurve> Bspli = occ::down_cast<Geom2d_BSplineCurve>(C);
    // UPDATE FMA 1-04-96
    if (C->IsPeriodic())
    {
      occ::handle<Geom2d_BSplineCurve> newBspli =
        occ::down_cast<Geom2d_BSplineCurve>(Bspli->Copy());
      newBspli->SetNotPeriodic();
      Bspli = newBspli;
    }
    if (Bspli->IsRational())
    {
      GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve MkRatBSplineC(Bspli,
                                                                                theLocalFactors);
      theBoundedCurve = MkRatBSplineC.Value();
    }
    else
    {
      GeomToStep_MakeBSplineCurveWithKnots MkBSplineC(Bspli, theLocalFactors);
      theBoundedCurve = MkBSplineC.Value();
    }
  }
  else if (C->IsKind(STANDARD_TYPE(Geom2d_BezierCurve)))
  {
    occ::handle<Geom2d_BezierCurve>      Cur   = occ::down_cast<Geom2d_BezierCurve>(C);
    occ::handle<Geom2d_BSplineCurve>     Bspli = Geom2dConvert::CurveToBSplineCurve(Cur);
    GeomToStep_MakeBSplineCurveWithKnots MkBSplineC(Bspli, theLocalFactors);
    theBoundedCurve = MkBSplineC.Value();
  }
  else
    done = false;
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const occ::handle<StepGeom_BoundedCurve>& GeomToStep_MakeBoundedCurve::Value() const
{
  StdFail_NotDone_Raise_if(!done, "GeomToStep_MakeBoundedCurve::Value() - no result");
  return theBoundedCurve;
}
