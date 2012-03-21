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

//gka 05.03.99 S4134 upgrade from CD to DIS
#include <RWStepBasic_RWDocumentRelationship.ixx>

#include <Interface_EntityIterator.hxx>


#include <StepBasic_DocumentRelationship.hxx>
#include <StepBasic_Document.hxx>
#include <TCollection_HAsciiString.hxx>


RWStepBasic_RWDocumentRelationship::RWStepBasic_RWDocumentRelationship () {}

void RWStepBasic_RWDocumentRelationship::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_DocumentRelationship)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,4,ach,"document_relationship")) return;

	// --- own field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : description ---

	Handle(TCollection_HAsciiString) aDescr;
	if (data->IsParamDefined (num,2)) { //gka 05.03.99 S4134 upgrade from CD to DIS
	  //szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	  data->ReadString (num,2,"description",ach,aDescr);
	}
	// --- own field : relating ---

	Handle(StepBasic_Document) aRelating;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadEntity (num, 3,"relating_document", ach, STANDARD_TYPE(StepBasic_Document), aRelating);

	// --- own field : related ---

	Handle(StepBasic_Document) aRelated;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadEntity (num, 4,"related_document", ach, STANDARD_TYPE(StepBasic_Document), aRelated);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aDescr, aRelating,aRelated);
}


void RWStepBasic_RWDocumentRelationship::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_DocumentRelationship)& ent) const
{

	// --- own field : name ---

	SW.Send(ent->Name());

	// --- own field : description ---

	SW.Send(ent->Description());

	// --- own field : kind ---

	SW.Send(ent->RelatingDocument());
	SW.Send(ent->RelatedDocument());
}


void RWStepBasic_RWDocumentRelationship::Share(const Handle(StepBasic_DocumentRelationship)& ent, Interface_EntityIterator& iter) const
{

	iter.AddItem(ent->RelatingDocument());
	iter.AddItem(ent->RelatedDocument());

}

