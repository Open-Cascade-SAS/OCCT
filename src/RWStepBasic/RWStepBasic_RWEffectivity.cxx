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


#include <RWStepBasic_RWEffectivity.ixx>

#include <Interface_EntityIterator.hxx>


#include <StepBasic_Effectivity.hxx>
#include <TCollection_HAsciiString.hxx>


RWStepBasic_RWEffectivity::RWStepBasic_RWEffectivity () {}

void RWStepBasic_RWEffectivity::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_Effectivity)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"effectivity")) return;

	// --- own field : id ---

	Handle(TCollection_HAsciiString) aId;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"id",ach,aId);

	//--- Initialisation of the read entity ---


	ent->Init(aId);
}


void RWStepBasic_RWEffectivity::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_Effectivity)& ent) const
{

	// --- own field : id ---

	SW.Send(ent->Id());
}


void RWStepBasic_RWEffectivity::Share(const Handle(StepBasic_Effectivity)& ent, Interface_EntityIterator& iter) const
{
}

