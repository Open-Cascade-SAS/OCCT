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

#include <Standard_Type.hxx>
#include <StepGeom_GeometricRepresentationContext.hxx>
#include <StepGeom_GeometricRepresentationContextAndParametricRepresentationContext.hxx>
#include <StepRepr_GlobalUnitAssignedContext.hxx>
#include <StepRepr_ParametricRepresentationContext.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(
  StepGeom_GeometricRepresentationContextAndParametricRepresentationContext,
  StepRepr_RepresentationContext)

StepGeom_GeometricRepresentationContextAndParametricRepresentationContext::
  StepGeom_GeometricRepresentationContextAndParametricRepresentationContext() = default;

void StepGeom_GeometricRepresentationContextAndParametricRepresentationContext::Init(
  const occ::handle<TCollection_HAsciiString>&                 aContextIdentifier,
  const occ::handle<TCollection_HAsciiString>&                 aContextType,
  const occ::handle<StepGeom_GeometricRepresentationContext>&  aGeometricRepresentationContext,
  const occ::handle<StepRepr_ParametricRepresentationContext>& aParametricRepresentationContext)
{
  // --- classe own fields ---
  geometricRepresentationContext  = aGeometricRepresentationContext;
  parametricRepresentationContext = aParametricRepresentationContext;
  // --- classe inherited fields ---
  StepRepr_RepresentationContext::Init(aContextIdentifier, aContextType);
}

void StepGeom_GeometricRepresentationContextAndParametricRepresentationContext::Init(
  const occ::handle<TCollection_HAsciiString>& aContextIdentifier,
  const occ::handle<TCollection_HAsciiString>& aContextType,
  const int                                    aCoordinateSpaceDimension)
{
  // --- classe inherited fields ---

  StepRepr_RepresentationContext::Init(aContextIdentifier, aContextType);

  // --- ANDOR component fields ---

  geometricRepresentationContext = new StepGeom_GeometricRepresentationContext();
  geometricRepresentationContext->Init(aContextIdentifier, aContextType, aCoordinateSpaceDimension);

  // --- ANDOR component fields ---

  parametricRepresentationContext = new StepRepr_ParametricRepresentationContext();
  parametricRepresentationContext->Init(aContextIdentifier, aContextType);
}

void StepGeom_GeometricRepresentationContextAndParametricRepresentationContext::
  SetGeometricRepresentationContext(
    const occ::handle<StepGeom_GeometricRepresentationContext>& aGeometricRepresentationContext)
{
  geometricRepresentationContext = aGeometricRepresentationContext;
}

occ::handle<StepGeom_GeometricRepresentationContext>
  StepGeom_GeometricRepresentationContextAndParametricRepresentationContext::
    GeometricRepresentationContext() const
{
  return geometricRepresentationContext;
}

void StepGeom_GeometricRepresentationContextAndParametricRepresentationContext::
  SetParametricRepresentationContext(
    const occ::handle<StepRepr_ParametricRepresentationContext>& aParametricRepresentationContext)
{
  parametricRepresentationContext = aParametricRepresentationContext;
}

occ::handle<StepRepr_ParametricRepresentationContext>
  StepGeom_GeometricRepresentationContextAndParametricRepresentationContext::
    ParametricRepresentationContext() const
{
  return parametricRepresentationContext;
}

//--- Specific Methods for AND classe field access ---

void StepGeom_GeometricRepresentationContextAndParametricRepresentationContext::
  SetCoordinateSpaceDimension(const int aCoordinateSpaceDimension)
{
  geometricRepresentationContext->SetCoordinateSpaceDimension(aCoordinateSpaceDimension);
}

int StepGeom_GeometricRepresentationContextAndParametricRepresentationContext::
  CoordinateSpaceDimension() const
{
  return geometricRepresentationContext->CoordinateSpaceDimension();
}
