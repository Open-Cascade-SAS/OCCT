// Created on: 1995-12-07
// Created by: Frederic MAUPAS
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx.ixx>

#include <StepGeom_GeometricRepresentationContext.hxx>

#include <StepRepr_GlobalUnitAssignedContext.hxx>

// --------------------------------------------------------------------------------------------------
// Method  :
// Purpose :
// --------------------------------------------------------------------------------------------------

StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx ()  {}

// --------------------------------------------------------------------------------------------------
// Method  :
// Purpose :
// --------------------------------------------------------------------------------------------------

void StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::Init
(const Handle(TCollection_HAsciiString)& aContextIdentifier,
 const Handle(TCollection_HAsciiString)& aContextType)
{
  StepRepr_RepresentationContext::Init(aContextIdentifier, aContextType);
}

// --------------------------------------------------------------------------------------------------
// Method  :
// Purpose :
// --------------------------------------------------------------------------------------------------

void StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::Init
(const Handle(TCollection_HAsciiString)& aContextIdentifier,
 const Handle(TCollection_HAsciiString)& aContextType,
 const Handle(StepGeom_GeometricRepresentationContext)& aGeometricRepresentationContext,
 const Handle(StepRepr_GlobalUnitAssignedContext)& aGlobalUnitAssignedContext,
 const Handle(StepRepr_GlobalUncertaintyAssignedContext)& aGlobalUncertaintyAssignedCtx)
{
  // --- classe own fields ---
  geometricRepresentationContext   = aGeometricRepresentationContext;
  globalUnitAssignedContext        = aGlobalUnitAssignedContext;
  globalUncertaintyAssignedContext = aGlobalUncertaintyAssignedCtx;
  // --- classe inherited fields ---
  StepRepr_RepresentationContext::Init(aContextIdentifier, aContextType);
}


// --------------------------------------------------------------------------------------------------
// Method  :
// Purpose :
// --------------------------------------------------------------------------------------------------

void StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::Init
(const Handle(TCollection_HAsciiString)& aContextIdentifier,
 const Handle(TCollection_HAsciiString)& aContextType,
 const Standard_Integer aCoordinateSpaceDimension,
 const Handle(StepBasic_HArray1OfNamedUnit)& aUnits,
 const Handle(StepBasic_HArray1OfUncertaintyMeasureWithUnit)& anUncertainty)
{
  // --- classe inherited fields ---

  StepRepr_RepresentationContext::Init(aContextIdentifier, aContextType);
  
  // --- ANDOR componant fields : GeometricRepresentationContext ---
  
  geometricRepresentationContext = new StepGeom_GeometricRepresentationContext();
  geometricRepresentationContext->Init(aContextIdentifier, aContextType, aCoordinateSpaceDimension);
  
  // --- ANDOR componant fields : GlobalUnitAssignedContext ---
  
  globalUnitAssignedContext = new StepRepr_GlobalUnitAssignedContext();
  globalUnitAssignedContext->Init(aContextIdentifier, aContextType, aUnits);

  // --- ANDOR componant fields : GlobalUncertaintyAssignedContext ---

  globalUncertaintyAssignedContext = new StepRepr_GlobalUncertaintyAssignedContext();
  globalUncertaintyAssignedContext->Init(aContextIdentifier, aContextType, anUncertainty);
  
}

// --------------------------------------------------------------------------------------------------
// Method  :
// Purpose :
// --------------------------------------------------------------------------------------------------

void StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::SetGeometricRepresentationContext(const Handle(StepGeom_GeometricRepresentationContext)& aGeometricRepresentationContext)
{
  geometricRepresentationContext = aGeometricRepresentationContext;
}

// --------------------------------------------------------------------------------------------------
// Method  :
// Purpose :
// --------------------------------------------------------------------------------------------------

Handle(StepGeom_GeometricRepresentationContext) StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::GeometricRepresentationContext() const
{
  return geometricRepresentationContext;
}

// --------------------------------------------------------------------------------------------------
// Method  :
// Purpose :
// --------------------------------------------------------------------------------------------------

void StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::SetGlobalUnitAssignedContext
(const Handle(StepRepr_GlobalUnitAssignedContext)& aGlobalUnitAssignedContext)
{
  globalUnitAssignedContext = aGlobalUnitAssignedContext;
}

// --------------------------------------------------------------------------------------------------
// Method  :
// Purpose :
// --------------------------------------------------------------------------------------------------

Handle(StepRepr_GlobalUnitAssignedContext) StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::GlobalUnitAssignedContext() const
{
  return globalUnitAssignedContext;
}

// --------------------------------------------------------------------------------------------------
// Method  :
// Purpose :
// --------------------------------------------------------------------------------------------------

void StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::SetGlobalUncertaintyAssignedContext
(const Handle(StepRepr_GlobalUncertaintyAssignedContext)& aGlobalUncertaintyAssignedCtx)
{
  globalUncertaintyAssignedContext = aGlobalUncertaintyAssignedCtx;
}


// --------------------------------------------------------------------------------------------------
// Method  :
// Purpose :
// --------------------------------------------------------------------------------------------------

Handle(StepRepr_GlobalUncertaintyAssignedContext) StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::GlobalUncertaintyAssignedContext() const
{
  return globalUncertaintyAssignedContext;
}

//-------------------------------------------------------------------------------------
//--- Specific Methods for AND classe field access : GeometricRepresentationContext ---
//-------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
// Method  :
// Purpose :
// --------------------------------------------------------------------------------------------------

void StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::SetCoordinateSpaceDimension(const Standard_Integer aCoordinateSpaceDimension)
{
  geometricRepresentationContext->SetCoordinateSpaceDimension(aCoordinateSpaceDimension);
}

// --------------------------------------------------------------------------------------------------
// Method  :
// Purpose :
// --------------------------------------------------------------------------------------------------

Standard_Integer StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::CoordinateSpaceDimension() const
{
  return geometricRepresentationContext->CoordinateSpaceDimension();
}

//---------------------------------------------------------------------------------
//--- Specific Methods for AND classe field access : GlobalUnitAssignedContext  ---
//---------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
// Method  :
// Purpose :
// --------------------------------------------------------------------------------------------------

void StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::SetUnits(const Handle(StepBasic_HArray1OfNamedUnit)& aUnits)
{
  globalUnitAssignedContext->SetUnits(aUnits);
}

// --------------------------------------------------------------------------------------------------
// Method  :
// Purpose :
// --------------------------------------------------------------------------------------------------

Handle(StepBasic_HArray1OfNamedUnit) StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::Units() const
{
  return globalUnitAssignedContext->Units();
}

// --------------------------------------------------------------------------------------------------
// Method  :
// Purpose :
// --------------------------------------------------------------------------------------------------

Handle(StepBasic_NamedUnit) StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::UnitsValue(const Standard_Integer num) const
{
  return globalUnitAssignedContext->UnitsValue(num);
}

// --------------------------------------------------------------------------------------------------
// Method  :
// Purpose :
// --------------------------------------------------------------------------------------------------

Standard_Integer StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::NbUnits () const
{
  return globalUnitAssignedContext->NbUnits();
}


//----------------------------------------------------------------------------------------
//--- Specific Methods for AND classe field access : GlobalUncertaintyAssignedContext  ---
//----------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
// Method  :
// Purpose :
// --------------------------------------------------------------------------------------------------

void StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::SetUncertainty(const Handle(StepBasic_HArray1OfUncertaintyMeasureWithUnit)& aUncertainty)
{
  globalUncertaintyAssignedContext->SetUncertainty(aUncertainty);
}

// --------------------------------------------------------------------------------------------------
// Method  :
// Purpose :
// --------------------------------------------------------------------------------------------------

Handle(StepBasic_HArray1OfUncertaintyMeasureWithUnit) StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::Uncertainty() const
{
  return globalUncertaintyAssignedContext->Uncertainty();
}

// --------------------------------------------------------------------------------------------------
// Method  :
// Purpose :
// --------------------------------------------------------------------------------------------------

Handle(StepBasic_UncertaintyMeasureWithUnit) StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::UncertaintyValue(const Standard_Integer num) const 
{
  return globalUncertaintyAssignedContext->UncertaintyValue(num);
}

// --------------------------------------------------------------------------------------------------
// Method  :
// Purpose :
// --------------------------------------------------------------------------------------------------

Standard_Integer StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx::NbUncertainty() const
{
  return globalUncertaintyAssignedContext->NbUncertainty();
}
