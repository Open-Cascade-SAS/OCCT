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

#include <StepGeom_GeometricRepresentationContextAndParametricRepresentationContext.ixx>

#include <StepGeom_GeometricRepresentationContext.hxx>

#include <StepRepr_GlobalUnitAssignedContext.hxx>


StepGeom_GeometricRepresentationContextAndParametricRepresentationContext::StepGeom_GeometricRepresentationContextAndParametricRepresentationContext ()  {}

void StepGeom_GeometricRepresentationContextAndParametricRepresentationContext::Init(
	const Handle(TCollection_HAsciiString)& aContextIdentifier,
	const Handle(TCollection_HAsciiString)& aContextType)
{

	StepRepr_RepresentationContext::Init(aContextIdentifier, aContextType);
}

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
