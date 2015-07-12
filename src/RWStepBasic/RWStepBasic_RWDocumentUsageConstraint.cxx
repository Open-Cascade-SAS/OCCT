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
#include <RWStepBasic_RWDocumentUsageConstraint.hxx>
#include <StepBasic_Document.hxx>
#include <StepBasic_DocumentUsageConstraint.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <TCollection_HAsciiString.hxx>

RWStepBasic_RWDocumentUsageConstraint::RWStepBasic_RWDocumentUsageConstraint () {}

void RWStepBasic_RWDocumentUsageConstraint::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_DocumentUsageConstraint)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"document_usage_constraint")) return;

	// --- own field : source ---

	Handle(StepBasic_Document) aSource;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadEntity (num, 1,"source", ach, STANDARD_TYPE(StepBasic_Document), aSource);

	// --- own field : label ---

	Handle(TCollection_HAsciiString) aLabel;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadString (num,2,"subject_element",ach,aLabel);

	// --- own field : description ---

	Handle(TCollection_HAsciiString) aDescription;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadString (num,3,"subject_element_value",ach,aDescription);

	//--- Initialisation of the read entity ---


	ent->Init(aSource, aLabel, aDescription);
}


void RWStepBasic_RWDocumentUsageConstraint::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_DocumentUsageConstraint)& ent) const
{

	// --- own field : id ---

	SW.Send(ent->Source());

	// --- own field : subject_element ---

	SW.Send(ent->SubjectElement());

	// --- own field : subject_element_value ---

	SW.Send(ent->SubjectElementValue());
}


void RWStepBasic_RWDocumentUsageConstraint::Share(const Handle(StepBasic_DocumentUsageConstraint)& ent, Interface_EntityIterator& iter) const
{
  iter.AddItem (ent->Source());
}
