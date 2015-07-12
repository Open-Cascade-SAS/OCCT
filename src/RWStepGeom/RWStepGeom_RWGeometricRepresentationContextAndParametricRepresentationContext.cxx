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


#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepGeom_RWGeometricRepresentationContextAndParametricRepresentationContext.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_GeometricRepresentationContext.hxx>
#include <StepGeom_GeometricRepresentationContextAndParametricRepresentationContext.hxx>

RWStepGeom_RWGeometricRepresentationContextAndParametricRepresentationContext::RWStepGeom_RWGeometricRepresentationContextAndParametricRepresentationContext () {}

void RWStepGeom_RWGeometricRepresentationContextAndParametricRepresentationContext::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num0,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_GeometricRepresentationContextAndParametricRepresentationContext)& ent) const
{

	Standard_Integer num = num0;


	// --- Instance of plex componant GeometricRepresentationContext ---

	if (!data->CheckNbParams(num,1,ach,"geometric_representation_context")) return;

	// --- field : coordinateSpaceDimension ---

	Standard_Integer aCoordinateSpaceDimension;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadInteger (num,1,"coordinate_space_dimension",ach,aCoordinateSpaceDimension);

	num = data->NextForComplex(num);

	// --- Instance of plex componant ParametricRepresentationContext ---

	if (!data->CheckNbParams(num,0,ach,"parametric_representation_context")) return;

	num = data->NextForComplex(num);

	// --- Instance of common supertype RepresentationContext ---

	if (!data->CheckNbParams(num,2,ach,"representation_context")) return;

	// --- field : contextIdentifier ---


	Handle(TCollection_HAsciiString) aContextIdentifier;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadString (num,1,"context_identifier",ach,aContextIdentifier);
	// --- field : contextType ---


	Handle(TCollection_HAsciiString) aContextType;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadString (num,2,"context_type",ach,aContextType);

	//--- Initialisation of the red entity ---

	ent->Init(aContextIdentifier,aContextType,aCoordinateSpaceDimension);
}


void RWStepGeom_RWGeometricRepresentationContextAndParametricRepresentationContext::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_GeometricRepresentationContextAndParametricRepresentationContext)& ent) const
{

	// --- Instance of plex componant GeometricRepresentationContext ---

	SW.StartEntity("GEOMETRIC_REPRESENTATION_CONTEXT");
	// --- field : coordinateSpaceDimension ---

	SW.Send(ent->CoordinateSpaceDimension());

	// --- Instance of plex componant ParametricRepresentationContext ---

	SW.StartEntity("PARAMETRIC_REPRESENTATION_CONTEXT");

	// --- Instance of common supertype RepresentationContext ---

	SW.StartEntity("REPRESENTATION_CONTEXT");
	// --- field : contextIdentifier ---

	SW.Send(ent->ContextIdentifier());
	// --- field : contextType ---

	SW.Send(ent->ContextType());
}


void RWStepGeom_RWGeometricRepresentationContextAndParametricRepresentationContext::Share(const Handle(StepGeom_GeometricRepresentationContextAndParametricRepresentationContext)& , Interface_EntityIterator& ) const
{
}

