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


#include <HeaderSection_FileSchema.hxx>
#include <Interface_Check.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <RWHeaderSection_RWFileSchema.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

RWHeaderSection_RWFileSchema::RWHeaderSection_RWFileSchema () {}

void RWHeaderSection_RWFileSchema::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(HeaderSection_FileSchema)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"file_schema has not 1 parameter(s)")) return;

	// --- own field : schemaIdentifiers ---

	Handle(Interface_HArray1OfHAsciiString) aSchemaIdentifiers;
	Handle(TCollection_HAsciiString) aSchemaIdentifiersItem;
	Standard_Integer nsub1;
	nsub1 = data->SubListNumber(num, 1, Standard_False);
	if (nsub1 !=0) {
	  Standard_Integer nb1 = data->NbParams(nsub1);
	  aSchemaIdentifiers = new Interface_HArray1OfHAsciiString (1, nb1);
	  for (Standard_Integer i1 = 1; i1 <= nb1; i1 ++) {
	    Standard_Boolean stat1 = data->ReadString
	         (nsub1,i1,"schema_identifiers",ach,aSchemaIdentifiersItem);
	    if (stat1) aSchemaIdentifiers->SetValue(i1,aSchemaIdentifiersItem);
	  }
	}
	else {
	  ach->AddFail("Parameter #1 (schema_identifiers) is not a LIST");
	}

	//--- Initialisation of the read entity ---


	if (!ach->HasFailed()) ent->Init(aSchemaIdentifiers);
}


void RWHeaderSection_RWFileSchema::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(HeaderSection_FileSchema)& ent) const
{

	// --- own field : schemaIdentifiers ---

	SW.OpenSub();
	for (Standard_Integer i1 = 1;  i1 <= ent->NbSchemaIdentifiers();  i1 ++) {
	  SW.Send(ent->SchemaIdentifiersValue(i1));
	}
	SW.CloseSub();
}
