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
#include <RWStepBasic_RWOrganization.ixx>


RWStepBasic_RWOrganization::RWStepBasic_RWOrganization () {}

void RWStepBasic_RWOrganization::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_Organization)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"organization")) return;

	// --- own field : id ---

	Handle(TCollection_HAsciiString) aId;
	Standard_Boolean hasAid = Standard_True;
	if (data->IsParamDefined(num,1)) {
	  //szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	  data->ReadString (num,1,"id",ach,aId);
	}
	else {
	  hasAid = Standard_False;
	  aId.Nullify();
	}

	// --- own field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadString (num,2,"name",ach,aName);

	// --- own field : description ---

	Handle(TCollection_HAsciiString) aDescription;
	if (data->IsParamDefined (num,3)) { //gka 05.03.99 S4134 upgrade from CD to DIS
	  //szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	  data->ReadString (num,3,"description",ach,aDescription);
	}
	//--- Initialisation of the read entity ---


	ent->Init(hasAid, aId, aName, aDescription);
}


void RWStepBasic_RWOrganization::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_Organization)& ent) const
{

	// --- own field : id ---

	Standard_Boolean hasAid = ent->HasId();
	if (hasAid) {
	  SW.Send(ent->Id());
	}
	else {
	  SW.SendUndef();
	}

	// --- own field : name ---

	SW.Send(ent->Name());

	// --- own field : description ---

	SW.Send(ent->Description());
}
