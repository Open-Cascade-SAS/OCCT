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

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_GeometricRepresentationContextAndParametricRepresentationContext,StepRepr_RepresentationContext)

StepGeom_GeometricRepresentationContextAndParametricRepresentationContext::StepGeom_GeometricRepresentationContextAndParametricRepresentationContext ()  {}

void StepGeom_GeometricRepresentationContextAndParametricRepresentationContext::Init
(const Handle(TCollection_HAsciiString)& aContextIdentifier,
 const Handle(TCollection_HAsciiString)& aContextType,
 const Handle(StepGeom_GeometricRepresentationContext)& aGeometricRepresentationContext,
 const Handle(StepRepr_ParametricRepresentationContext)& aParametricRepresentationContext)
{
  // --- classe own fields ---
  geometricRepresentationContext = aGeometricRepresentationContext;
  parametricRepresentationContext= aParametricRepresentationContext;
  // --- classe inherited fields ---
  StepRepr_RepresentationContext::Init(aContextIdentifier, aContextType);
}


void StepGeom_GeometricRepresentationContextAndParametricRepresentationContext::Init
(const Handle(TCollection_HAsciiString)& aContextIdentifier,
 const Handle(TCollection_HAsciiString)& aContextType,
 const Standard_Integer aCoordinateSpaceDimension)
{
  // --- classe inherited fields ---
  
  StepRepr_RepresentationContext::Init(aContextIdentifier, aContextType);
  
  // --- ANDOR componant fields ---
  
  geometricRepresentationContext = new StepGeom_GeometricRepresentationContext();
  geometricRepresentationContext->Init(aContextIdentifier, aContextType, aCoordinateSpaceDimension);
  
  // --- ANDOR componant fields ---
  
  parametricRepresentationContext = new StepRepr_ParametricRepresentationContext();
  parametricRepresentationContext->Init(aContextIdentifier, aContextType);
}


void StepGeom_GeometricRepresentationContextAndParametricRepresentationContext::SetGeometricRepresentationContext(const Handle(StepGeom_GeometricRepresentationContext)& aGeometricRepresentationContext)
{
  geometricRepresentationContext = aGeometricRepresentationContext;
}

Handle(StepGeom_GeometricRepresentationContext) StepGeom_GeometricRepresentationContextAndParametricRepresentationContext::GeometricRepresentationContext() const
{
  return geometricRepresentationContext;
}

void StepGeom_GeometricRepresentationContextAndParametricRepresentationContext::SetParametricRepresentationContext(const Handle(StepRepr_ParametricRepresentationContext)& aParametricRepresentationContext)
{
  parametricRepresentationContext = aParametricRepresentationContext;
}

Handle(StepRepr_ParametricRepresentationContext) StepGeom_GeometricRepresentationContextAndParametricRepresentationContext::ParametricRepresentationContext() const
{
	return parametricRepresentationContext;
}

//--- Specific Methods for AND classe field access ---


void StepGeom_GeometricRepresentationContextAndParametricRepresentationContext::SetCoordinateSpaceDimension(const Standard_Integer aCoordinateSpaceDimension)
{
  geometricRepresentationContext->SetCoordinateSpaceDimension(aCoordinateSpaceDimension);
}

Standard_Integer StepGeom_GeometricRepresentationContextAndParametricRepresentationContext::CoordinateSpaceDimension() const
{
  return geometricRepresentationContext->CoordinateSpaceDimension();
}
