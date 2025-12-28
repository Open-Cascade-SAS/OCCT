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
#include <StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext.hxx>
#include <StepRepr_GlobalUnitAssignedContext.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext,
                           StepRepr_RepresentationContext)

StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext::
  StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext()
= default;

void StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext::Init(
  const occ::handle<TCollection_HAsciiString>&                aContextIdentifier,
  const occ::handle<TCollection_HAsciiString>&                aContextType,
  const occ::handle<StepGeom_GeometricRepresentationContext>& aGeometricRepresentationContext,
  const occ::handle<StepRepr_GlobalUnitAssignedContext>&      aGlobalUnitAssignedContext)
{
  // --- classe own fields ---
  geometricRepresentationContext = aGeometricRepresentationContext;
  globalUnitAssignedContext      = aGlobalUnitAssignedContext;
  // --- classe inherited fields ---
  StepRepr_RepresentationContext::Init(aContextIdentifier, aContextType);
}

void StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext::Init(
  const occ::handle<TCollection_HAsciiString>& aContextIdentifier,
  const occ::handle<TCollection_HAsciiString>& aContextType,
  const int                                    aCoordinateSpaceDimension,
  const occ::handle<NCollection_HArray1<occ::handle<StepBasic_NamedUnit>>>& aUnits)
{
  // --- classe inherited fields ---

  StepRepr_RepresentationContext::Init(aContextIdentifier, aContextType);

  // --- ANDOR component fields ---

  geometricRepresentationContext = new StepGeom_GeometricRepresentationContext();
  geometricRepresentationContext->Init(aContextIdentifier, aContextType, aCoordinateSpaceDimension);

  // --- ANDOR component fields ---

  globalUnitAssignedContext = new StepRepr_GlobalUnitAssignedContext();
  globalUnitAssignedContext->Init(aContextIdentifier, aContextType, aUnits);
}

void StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext::
  SetGeometricRepresentationContext(
    const occ::handle<StepGeom_GeometricRepresentationContext>& aGeometricRepresentationContext)
{
  geometricRepresentationContext = aGeometricRepresentationContext;
}

occ::handle<StepGeom_GeometricRepresentationContext>
  StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext::
    GeometricRepresentationContext() const
{
  return geometricRepresentationContext;
}

void StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext::
  SetGlobalUnitAssignedContext(
    const occ::handle<StepRepr_GlobalUnitAssignedContext>& aGlobalUnitAssignedContext)
{
  globalUnitAssignedContext = aGlobalUnitAssignedContext;
}

occ::handle<StepRepr_GlobalUnitAssignedContext>
  StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext::GlobalUnitAssignedContext()
    const
{
  return globalUnitAssignedContext;
}

//--- Specific Methods for AND classe field access ---

void StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext::
  SetCoordinateSpaceDimension(const int aCoordinateSpaceDimension)
{
  geometricRepresentationContext->SetCoordinateSpaceDimension(aCoordinateSpaceDimension);
}

int StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext::CoordinateSpaceDimension()
  const
{
  return geometricRepresentationContext->CoordinateSpaceDimension();
}

//--- Specific Methods for AND classe field access ---

void StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext::SetUnits(
  const occ::handle<NCollection_HArray1<occ::handle<StepBasic_NamedUnit>>>& aUnits)
{
  globalUnitAssignedContext->SetUnits(aUnits);
}

occ::handle<NCollection_HArray1<occ::handle<StepBasic_NamedUnit>>>
  StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext::Units() const
{
  return globalUnitAssignedContext->Units();
}

occ::handle<StepBasic_NamedUnit>
  StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext::UnitsValue(
    const int num) const
{
  return globalUnitAssignedContext->UnitsValue(num);
}

int StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext::NbUnits() const
{
  return globalUnitAssignedContext->NbUnits();
}
