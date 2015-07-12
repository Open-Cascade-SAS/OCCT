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

//gka 05.03.99 S4134 upgrade from CD to DIS

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepBasic_RWDocumentRelationship.hxx>
#include <StepBasic_Document.hxx>
#include <StepBasic_DocumentRelationship.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
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

