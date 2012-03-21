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


#include <RWStepGeom_RWEvaluatedDegeneratePcurve.ixx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_Surface.hxx>
#include <StepRepr_DefinitionalRepresentation.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepGeom_EvaluatedDegeneratePcurve.hxx>


RWStepGeom_RWEvaluatedDegeneratePcurve::RWStepGeom_RWEvaluatedDegeneratePcurve () {}

void RWStepGeom_RWEvaluatedDegeneratePcurve::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_EvaluatedDegeneratePcurve)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,4,ach,"evaluated_degenerate_pcurve")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : basisSurface ---

	Handle(StepGeom_Surface) aBasisSurface;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"basis_surface", ach, STANDARD_TYPE(StepGeom_Surface), aBasisSurface);

	// --- inherited field : referenceToCurve ---

	Handle(StepRepr_DefinitionalRepresentation) aReferenceToCurve;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadEntity(num, 3,"reference_to_curve", ach, STANDARD_TYPE(StepRepr_DefinitionalRepresentation), aReferenceToCurve);

	// --- own field : equivalentPoint ---

	Handle(StepGeom_CartesianPoint) aEquivalentPoint;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadEntity(num, 4,"equivalent_point", ach, STANDARD_TYPE(StepGeom_CartesianPoint), aEquivalentPoint);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aBasisSurface, aReferenceToCurve, aEquivalentPoint);
}


void RWStepGeom_RWEvaluatedDegeneratePcurve::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_EvaluatedDegeneratePcurve)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field basisSurface ---

	SW.Send(ent->BasisSurface());

	// --- inherited field referenceToCurve ---

	SW.Send(ent->ReferenceToCurve());

	// --- own field : equivalentPoint ---

	SW.Send(ent->EquivalentPoint());
}


void RWStepGeom_RWEvaluatedDegeneratePcurve::Share(const Handle(StepGeom_EvaluatedDegeneratePcurve)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->BasisSurface());


	iter.GetOneItem(ent->ReferenceToCurve());


	iter.GetOneItem(ent->EquivalentPoint());
}

