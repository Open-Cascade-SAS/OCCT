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


#include <RWStepShape_RWBoxedHalfSpace.ixx>
#include <StepShape_BoxDomain.hxx>
#include <StepGeom_Surface.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepShape_BoxedHalfSpace.hxx>


RWStepShape_RWBoxedHalfSpace::RWStepShape_RWBoxedHalfSpace () {}

void RWStepShape_RWBoxedHalfSpace::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_BoxedHalfSpace)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,4,ach,"boxed_half_space")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : baseSurface ---

	Handle(StepGeom_Surface) aBaseSurface;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"base_surface", ach, STANDARD_TYPE(StepGeom_Surface), aBaseSurface);

	// --- inherited field : agreementFlag ---

	Standard_Boolean aAgreementFlag;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadBoolean (num,3,"agreement_flag",ach,aAgreementFlag);

	// --- own field : enclosure ---

	Handle(StepShape_BoxDomain) aEnclosure;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadEntity(num, 4,"enclosure", ach, STANDARD_TYPE(StepShape_BoxDomain), aEnclosure);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aBaseSurface, aAgreementFlag, aEnclosure);
}


void RWStepShape_RWBoxedHalfSpace::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_BoxedHalfSpace)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field baseSurface ---

	SW.Send(ent->BaseSurface());

	// --- inherited field agreementFlag ---

	SW.SendBoolean(ent->AgreementFlag());

	// --- own field : enclosure ---

	SW.Send(ent->Enclosure());
}


void RWStepShape_RWBoxedHalfSpace::Share(const Handle(StepShape_BoxedHalfSpace)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->BaseSurface());


	iter.GetOneItem(ent->Enclosure());
}

