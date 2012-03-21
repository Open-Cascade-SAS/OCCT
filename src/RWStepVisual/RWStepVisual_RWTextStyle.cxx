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


#include <RWStepVisual_RWTextStyle.ixx>
#include <StepVisual_TextStyleForDefinedFont.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepVisual_TextStyle.hxx>


RWStepVisual_RWTextStyle::RWStepVisual_RWTextStyle () {}

void RWStepVisual_RWTextStyle::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_TextStyle)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"text_style has not 2 parameter(s)")) return;

	// --- own field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : characterAppearance ---

	Handle(StepVisual_TextStyleForDefinedFont) aCharacterAppearance;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"character_appearance", ach, STANDARD_TYPE(StepVisual_TextStyleForDefinedFont), aCharacterAppearance);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aCharacterAppearance);
}


void RWStepVisual_RWTextStyle::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_TextStyle)& ent) const
{

	// --- own field : name ---

	SW.Send(ent->Name());

	// --- own field : characterAppearance ---

	SW.Send(ent->CharacterAppearance());
}


void RWStepVisual_RWTextStyle::Share(const Handle(StepVisual_TextStyle)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->CharacterAppearance());
}

