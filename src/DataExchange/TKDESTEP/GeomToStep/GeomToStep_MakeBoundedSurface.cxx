// Created on: 1993-06-22
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

#include <Geom_BezierSurface.hxx>
#include <Geom_BoundedSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <GeomConvert.hxx>
#include <GeomToStep_MakeBoundedSurface.hxx>
#include <GeomToStep_MakeBSplineSurfaceWithKnots.hxx>
#include <GeomToStep_MakeBSplineSurfaceWithKnotsAndRationalBSplineSurface.hxx>
#include <GeomToStep_MakeRectangularTrimmedSurface.hxx>
#include <StdFail_NotDone.hxx>
#include <StepData_Factors.hxx>
#include <StepGeom_BoundedSurface.hxx>
#include <StepGeom_BSplineSurfaceWithKnots.hxx>
#include <StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface.hxx>
#include <StepGeom_RectangularTrimmedSurface.hxx>

//=============================================================================
// Creation d' une BoundedSurface de prostep a partir d' une BoundedSurface
// de Geom
//=============================================================================
GeomToStep_MakeBoundedSurface::GeomToStep_MakeBoundedSurface(
  const occ::handle<Geom_BoundedSurface>& S,
  const StepData_Factors&                 theLocalFactors)
{
  done = true;
  if (S->IsKind(STANDARD_TYPE(Geom_BSplineSurface)))
  {
    occ::handle<Geom_BSplineSurface> BS = occ::down_cast<Geom_BSplineSurface>(S);
    // UPDATE FMA 1-04-96
    if (S->IsUPeriodic() || S->IsVPeriodic())
    {
      occ::handle<Geom_BSplineSurface> newBS = occ::down_cast<Geom_BSplineSurface>(BS->Copy());
      newBS->SetUNotPeriodic();
      newBS->SetVNotPeriodic();
      BS = newBS;
    }
    if (BS->IsURational() || BS->IsVRational())
    {
      GeomToStep_MakeBSplineSurfaceWithKnotsAndRationalBSplineSurface MkRatBSplineS(
        BS,
        theLocalFactors);
      theBoundedSurface = MkRatBSplineS.Value();
    }
    else
    {
      GeomToStep_MakeBSplineSurfaceWithKnots MkBSplineS(BS, theLocalFactors);
      theBoundedSurface = MkBSplineS.Value();
    }
  }
  else if (S->IsKind(STANDARD_TYPE(Geom_BezierSurface)))
  {
    occ::handle<Geom_BezierSurface>  Sur = occ::down_cast<Geom_BezierSurface>(S);
    occ::handle<Geom_BSplineSurface> BS  = GeomConvert::SurfaceToBSplineSurface(Sur);
    if (BS->IsURational() || BS->IsVRational())
    {
      GeomToStep_MakeBSplineSurfaceWithKnotsAndRationalBSplineSurface MkRatBSplineS(
        BS,
        theLocalFactors);
      theBoundedSurface = MkRatBSplineS.Value();
    }
    else
    {
      GeomToStep_MakeBSplineSurfaceWithKnots MkBSplineS(BS, theLocalFactors);
      theBoundedSurface = MkBSplineS.Value();
    }
  }
  else if (S->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
  {
    occ::handle<Geom_RectangularTrimmedSurface> Sur =
      occ::down_cast<Geom_RectangularTrimmedSurface>(S);
    GeomToStep_MakeRectangularTrimmedSurface MkRTSurf(Sur, theLocalFactors);
    theBoundedSurface = MkRTSurf.Value();
  }
  else
    done = false;
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const occ::handle<StepGeom_BoundedSurface>& GeomToStep_MakeBoundedSurface::Value() const
{
  StdFail_NotDone_Raise_if(!done, "GeomToStep_MakeBoundedSurface::Value() - no result");
  return theBoundedSurface;
}
