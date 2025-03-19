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

#include <StepGeom_Curve.hxx>
#include <StepGeom_PointOnCurve.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_PointOnCurve, StepGeom_Point)

StepGeom_PointOnCurve::StepGeom_PointOnCurve() {}

void StepGeom_PointOnCurve::Init(const Handle(TCollection_HAsciiString)& aName,
                                 const Handle(StepGeom_Curve)&           aBasisCurve,
                                 const Standard_Real                     aPointParameter)
{
  // --- classe own fields ---
  basisCurve     = aBasisCurve;
  pointParameter = aPointParameter;
  // --- classe inherited fields ---
  StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_PointOnCurve::SetBasisCurve(const Handle(StepGeom_Curve)& aBasisCurve)
{
  basisCurve = aBasisCurve;
}

Handle(StepGeom_Curve) StepGeom_PointOnCurve::BasisCurve() const
{
  return basisCurve;
}

void StepGeom_PointOnCurve::SetPointParameter(const Standard_Real aPointParameter)
{
  pointParameter = aPointParameter;
}

Standard_Real StepGeom_PointOnCurve::PointParameter() const
{
  return pointParameter;
}
