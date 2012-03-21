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


#include <RWStepGeom_RWGeometricRepresentationContextAndParametricRepresentationContext.ixx>
#include <StepGeom_GeometricRepresentationContext.hxx>

#include <Interface_EntityIterator.hxx>


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


void RWStepGeom_RWGeometricRepresentationContextAndParametricRepresentationContext::Share(const Handle(StepGeom_GeometricRepresentationContextAndParametricRepresentationContext)& ent, Interface_EntityIterator& iter) const
{
}

