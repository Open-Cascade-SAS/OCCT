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


#include <HeaderSection_FileName.hxx>
#include <Interface_Check.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <RWHeaderSection_RWFileName.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

RWHeaderSection_RWFileName::RWHeaderSection_RWFileName () {}

void RWHeaderSection_RWFileName::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(HeaderSection_FileName)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,7,ach,"file_name has not 7 parameter(s)")) return;

	// --- own field : name ---

	Handle(TCollection_HAsciiString) aName;
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : timeStamp ---

	Handle(TCollection_HAsciiString) aTimeStamp;
	data->ReadString (num,2,"time_stamp",ach,aTimeStamp);

	// --- own field : author ---

	Handle(Interface_HArray1OfHAsciiString) aAuthor;
	Handle(TCollection_HAsciiString) aAuthorItem;
	Standard_Integer nsub3;
	nsub3 = data->SubListNumber(num, 3, Standard_False);
	if (nsub3 !=0) {
	  Standard_Integer nb3 = data->NbParams(nsub3);
	  aAuthor = new Interface_HArray1OfHAsciiString (1, nb3);
	  for (Standard_Integer i3 = 1; i3 <= nb3; i3 ++) {
	    Standard_Boolean stat3 = data->ReadString
	         (nsub3,i3,"author",ach,aAuthorItem);
	    if (stat3) aAuthor->SetValue(i3,aAuthorItem);
	  }
	}
	else {
	  ach->AddFail("Parameter #3 (author) is not a LIST");
	}

	// --- own field : organization ---

	Handle(Interface_HArray1OfHAsciiString) aOrganization;
	Handle(TCollection_HAsciiString) aOrganizationItem;
	Standard_Integer nsub4;
	nsub4 = data->SubListNumber(num, 4, Standard_False);
	if (nsub4 !=0) {
	  Standard_Integer nb4 = data->NbParams(nsub4);
	  aOrganization = new Interface_HArray1OfHAsciiString (1, nb4);
	  for (Standard_Integer i4 = 1; i4 <= nb4; i4 ++) {
	    Standard_Boolean stat4 = data->ReadString
	         (nsub4,i4,"organization",ach,aOrganizationItem);
	    if (stat4) aOrganization->SetValue(i4,aOrganizationItem);
	  }
	}
	else {
	  ach->AddFail("Parameter #4 (organization) is not a LIST");
	}

	// --- own field : preprocessorVersion ---

	Handle(TCollection_HAsciiString) aPreprocessorVersion;
	data->ReadString (num,5,"preprocessor_version",ach,aPreprocessorVersion);

	// --- own field : originatingSystem ---

	Handle(TCollection_HAsciiString) aOriginatingSystem;
	data->ReadString (num,6,"originating_system",ach,aOriginatingSystem);

	// --- own field : authorisation ---

	Handle(TCollection_HAsciiString) aAuthorisation;
	data->ReadString (num,7,"authorisation",ach,aAuthorisation);

	//--- Initialisation of the read entity ---


	if (!ach->HasFailed()) ent->Init(aName, aTimeStamp, aAuthor, aOrganization, aPreprocessorVersion, aOriginatingSystem, aAuthorisation);
}


void RWHeaderSection_RWFileName::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(HeaderSection_FileName)& ent) const
{

	// --- own field : name ---

	SW.Send(ent->Name());

	// --- own field : timeStamp ---

	SW.Send(ent->TimeStamp());

	// --- own field : author ---

	SW.OpenSub();
	for (Standard_Integer i3 = 1;  i3 <= ent->NbAuthor();  i3 ++) {
	  SW.Send(ent->AuthorValue(i3));
	}
	SW.CloseSub();

	// --- own field : organization ---

	SW.OpenSub();
	for (Standard_Integer i4 = 1;  i4 <= ent->NbOrganization();  i4 ++) {
	  SW.Send(ent->OrganizationValue(i4));
	}
	SW.CloseSub();

	// --- own field : preprocessorVersion ---

	SW.Send(ent->PreprocessorVersion());

	// --- own field : originatingSystem ---

	SW.Send(ent->OriginatingSystem());

	// --- own field : authorisation ---

	SW.Send(ent->Authorisation());
}
