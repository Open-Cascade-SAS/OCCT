// Created on: 1995-12-07
// Created by: Frederic MAUPAS
// Copyright (c) 1995-1999 Matra Datavision
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
#include <StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx.hxx>
#include <StepRepr_GlobalUncertaintyAssignedContext.hxx>
#include <StepRepr_GlobalUnitAssignedContext.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx,
                           StepRepr_RepresentationContext)

//=================================================================================================

StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::
  StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx()
{
}

//=================================================================================================

void StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::Init(
  const Handle(TCollection_HAsciiString)&                  aContextIdentifier,
  const Handle(TCollection_HAsciiString)&                  aContextType,
  const Handle(StepGeom_GeometricRepresentationContext)&   aGeometricRepresentationContext,
  const Handle(StepRepr_GlobalUnitAssignedContext)&        aGlobalUnitAssignedContext,
  const Handle(StepRepr_GlobalUncertaintyAssignedContext)& aGlobalUncertaintyAssignedCtx)
{
  // --- classe own fields ---
  geometricRepresentationContext   = aGeometricRepresentationContext;
  globalUnitAssignedContext        = aGlobalUnitAssignedContext;
  globalUncertaintyAssignedContext = aGlobalUncertaintyAssignedCtx;
  // --- classe inherited fields ---
  StepRepr_RepresentationContext::Init(aContextIdentifier, aContextType);
}

//=================================================================================================

void StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::Init(
  const Handle(TCollection_HAsciiString)&                      aContextIdentifier,
  const Handle(TCollection_HAsciiString)&                      aContextType,
  const Standard_Integer                                       aCoordinateSpaceDimension,
  const Handle(StepBasic_HArray1OfNamedUnit)&                  aUnits,
  const Handle(StepBasic_HArray1OfUncertaintyMeasureWithUnit)& anUncertainty)
{
  // --- classe inherited fields ---

  StepRepr_RepresentationContext::Init(aContextIdentifier, aContextType);

  // --- ANDOR component fields : GeometricRepresentationContext ---

  geometricRepresentationContext = new StepGeom_GeometricRepresentationContext();
  geometricRepresentationContext->Init(aContextIdentifier, aContextType, aCoordinateSpaceDimension);

  // --- ANDOR component fields : GlobalUnitAssignedContext ---

  globalUnitAssignedContext = new StepRepr_GlobalUnitAssignedContext();
  globalUnitAssignedContext->Init(aContextIdentifier, aContextType, aUnits);

  // --- ANDOR component fields : GlobalUncertaintyAssignedContext ---

  globalUncertaintyAssignedContext = new StepRepr_GlobalUncertaintyAssignedContext();
  globalUncertaintyAssignedContext->Init(aContextIdentifier, aContextType, anUncertainty);
}

//=================================================================================================

void StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::
  SetGeometricRepresentationContext(
    const Handle(StepGeom_GeometricRepresentationContext)& aGeometricRepresentationContext)
{
  geometricRepresentationContext = aGeometricRepresentationContext;
}

//=================================================================================================

Handle(StepGeom_GeometricRepresentationContext)
  StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::GeometricRepresentationContext()
    const
{
  return geometricRepresentationContext;
}

//=================================================================================================

void StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::SetGlobalUnitAssignedContext(
  const Handle(StepRepr_GlobalUnitAssignedContext)& aGlobalUnitAssignedContext)
{
  globalUnitAssignedContext = aGlobalUnitAssignedContext;
}

//=================================================================================================

Handle(StepRepr_GlobalUnitAssignedContext)
  StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::GlobalUnitAssignedContext()
    const
{
  return globalUnitAssignedContext;
}

//=================================================================================================

void StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::
  SetGlobalUncertaintyAssignedContext(
    const Handle(StepRepr_GlobalUncertaintyAssignedContext)& aGlobalUncertaintyAssignedCtx)
{
  globalUncertaintyAssignedContext = aGlobalUncertaintyAssignedCtx;
}

//=================================================================================================

Handle(StepRepr_GlobalUncertaintyAssignedContext)
  StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::
    GlobalUncertaintyAssignedContext() const
{
  return globalUncertaintyAssignedContext;
}

//-------------------------------------------------------------------------------------
//--- Specific Methods for AND classe field access : GeometricRepresentationContext ---
//-------------------------------------------------------------------------------------

//=================================================================================================

void StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::SetCoordinateSpaceDimension(
  const Standard_Integer aCoordinateSpaceDimension)
{
  geometricRepresentationContext->SetCoordinateSpaceDimension(aCoordinateSpaceDimension);
}

//=================================================================================================

Standard_Integer StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::
  CoordinateSpaceDimension() const
{
  return geometricRepresentationContext->CoordinateSpaceDimension();
}

//---------------------------------------------------------------------------------
//--- Specific Methods for AND classe field access : GlobalUnitAssignedContext  ---
//---------------------------------------------------------------------------------

//=================================================================================================

void StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::SetUnits(
  const Handle(StepBasic_HArray1OfNamedUnit)& aUnits)
{
  globalUnitAssignedContext->SetUnits(aUnits);
}

//=================================================================================================

Handle(StepBasic_HArray1OfNamedUnit)
  StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::Units() const
{
  return globalUnitAssignedContext->Units();
}

//=================================================================================================

Handle(StepBasic_NamedUnit) StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::
  UnitsValue(const Standard_Integer num) const
{
  return globalUnitAssignedContext->UnitsValue(num);
}

//=================================================================================================

Standard_Integer StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::NbUnits() const
{
  return globalUnitAssignedContext->NbUnits();
}

//----------------------------------------------------------------------------------------
//--- Specific Methods for AND classe field access : GlobalUncertaintyAssignedContext  ---
//----------------------------------------------------------------------------------------

//=================================================================================================

void StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::SetUncertainty(
  const Handle(StepBasic_HArray1OfUncertaintyMeasureWithUnit)& aUncertainty)
{
  globalUncertaintyAssignedContext->SetUncertainty(aUncertainty);
}

//=================================================================================================

Handle(StepBasic_HArray1OfUncertaintyMeasureWithUnit)
  StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::Uncertainty() const
{
  return globalUncertaintyAssignedContext->Uncertainty();
}

//=================================================================================================

Handle(StepBasic_UncertaintyMeasureWithUnit)
  StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::UncertaintyValue(
    const Standard_Integer num) const
{
  return globalUncertaintyAssignedContext->UncertaintyValue(num);
}

//=================================================================================================

Standard_Integer StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::NbUncertainty()
  const
{
  return globalUncertaintyAssignedContext->NbUncertainty();
}
