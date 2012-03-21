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


#include <RWStepBasic_RWPerson.ixx>
#include <Interface_HArray1OfHAsciiString.hxx>


RWStepBasic_RWPerson::RWStepBasic_RWPerson () {}

void RWStepBasic_RWPerson::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_Person)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,6,ach,"person")) return;

	// --- own field : id ---

	Handle(TCollection_HAsciiString) aId;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"id",ach,aId);

	// --- own field : lastName ---

	Handle(TCollection_HAsciiString) aLastName;
	Standard_Boolean hasAlastName = Standard_True;
	if (data->IsParamDefined(num,2)) {
	  //szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	  data->ReadString (num,2,"last_name",ach,aLastName);
	}
	else {
	  hasAlastName = Standard_False;
	  aLastName.Nullify();
	}

	// --- own field : firstName ---

	Handle(TCollection_HAsciiString) aFirstName;
	Standard_Boolean hasAfirstName = Standard_True;
	if (data->IsParamDefined(num,3)) {
	  //szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	  data->ReadString (num,3,"first_name",ach,aFirstName);
	}
	else {
	  hasAfirstName = Standard_False;
	  aFirstName.Nullify();
	}

	// --- own field : middleNames ---

	Handle(Interface_HArray1OfHAsciiString) aMiddleNames;
	Standard_Boolean hasAmiddleNames = Standard_True;
	if (data->IsParamDefined(num,4)) {
	  Handle(TCollection_HAsciiString) aMiddleNamesItem;
	  Standard_Integer nsub4;
	  if (data->ReadSubList (num,4,"middle_names",ach,nsub4)) {
	    Standard_Integer nb4 = data->NbParams(nsub4);
	    aMiddleNames = new Interface_HArray1OfHAsciiString (1, nb4);
	    for (Standard_Integer i4 = 1; i4 <= nb4; i4 ++) {
	      //szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	      if (data->ReadString (nsub4,i4,"middle_names",ach,aMiddleNamesItem))
		aMiddleNames->SetValue(i4,aMiddleNamesItem);
	    }
	  }
	}
	else {
	  hasAmiddleNames = Standard_False;
	  aMiddleNames.Nullify();
	}

	// --- own field : prefixTitles ---

	Handle(Interface_HArray1OfHAsciiString) aPrefixTitles;
	Standard_Boolean hasAprefixTitles = Standard_True;
	if (data->IsParamDefined(num,5)) {
	  Handle(TCollection_HAsciiString) aPrefixTitlesItem;
	  Standard_Integer nsub5;
	  if (data->ReadSubList (num,5,"prefix_titles",ach,nsub5)) {
	    Standard_Integer nb5 = data->NbParams(nsub5);
	    aPrefixTitles = new Interface_HArray1OfHAsciiString (1, nb5);
	    for (Standard_Integer i5 = 1; i5 <= nb5; i5 ++) {
	      //szv#4:S4163:12Mar99 `Standard_Boolean stat5 =` not needed
	      if (data->ReadString (nsub5,i5,"prefix_titles",ach,aPrefixTitlesItem))
		aPrefixTitles->SetValue(i5,aPrefixTitlesItem);
	    }
	  }
	}
	else {
	  hasAprefixTitles = Standard_False;
	  aPrefixTitles.Nullify();
	}

	// --- own field : suffixTitles ---

	Handle(Interface_HArray1OfHAsciiString) aSuffixTitles;
	Standard_Boolean hasAsuffixTitles = Standard_True;
	if (data->IsParamDefined(num,6)) {
	  Handle(TCollection_HAsciiString) aSuffixTitlesItem;
	  Standard_Integer nsub6;
	  if (data->ReadSubList (num,6,"suffix_titles",ach,nsub6)) {
	    Standard_Integer nb6 = data->NbParams(nsub6);
	    aSuffixTitles = new Interface_HArray1OfHAsciiString (1, nb6);
	    for (Standard_Integer i6 = 1; i6 <= nb6; i6 ++) {
	      //szv#4:S4163:12Mar99 `Standard_Boolean stat6 =` not needed
	      if (data->ReadString (nsub6,i6,"suffix_titles",ach,aSuffixTitlesItem))
		aSuffixTitles->SetValue(i6,aSuffixTitlesItem);
	    }
	  }
	}
	else {
	  hasAsuffixTitles = Standard_False;
	  aSuffixTitles.Nullify();
	}

	//--- Initialisation of the read entity ---


	ent->Init(aId, hasAlastName, aLastName, hasAfirstName, aFirstName, hasAmiddleNames, aMiddleNames, hasAprefixTitles, aPrefixTitles, hasAsuffixTitles, aSuffixTitles);
}


void RWStepBasic_RWPerson::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_Person)& ent) const
{

	// --- own field : id ---

	SW.Send(ent->Id());

	// --- own field : lastName ---

	Standard_Boolean hasAlastName = ent->HasLastName();
	if (hasAlastName) {
	  SW.Send(ent->LastName());
	}
	else {
	  SW.SendUndef();
	}

	// --- own field : firstName ---

	Standard_Boolean hasAfirstName = ent->HasFirstName();
	if (hasAfirstName) {
	  SW.Send(ent->FirstName());
	}
	else {
	  SW.SendUndef();
	}

	// --- own field : middleNames ---

	Standard_Boolean hasAmiddleNames = ent->HasMiddleNames();
	if (hasAmiddleNames) {
	  SW.OpenSub();
	  for (Standard_Integer i4 = 1;  i4 <= ent->NbMiddleNames();  i4 ++) {
	    SW.Send(ent->MiddleNamesValue(i4));
	  }
	  SW.CloseSub();
	}
	else {
	  SW.SendUndef();
	}

	// --- own field : prefixTitles ---

	Standard_Boolean hasAprefixTitles = ent->HasPrefixTitles();
	if (hasAprefixTitles) {
	  SW.OpenSub();
	  for (Standard_Integer i5 = 1;  i5 <= ent->NbPrefixTitles();  i5 ++) {
	    SW.Send(ent->PrefixTitlesValue(i5));
	  }
	  SW.CloseSub();
	}
	else {
	  SW.SendUndef();
	}

	// --- own field : suffixTitles ---

	Standard_Boolean hasAsuffixTitles = ent->HasSuffixTitles();
	if (hasAsuffixTitles) {
	  SW.OpenSub();
	  for (Standard_Integer i6 = 1;  i6 <= ent->NbSuffixTitles();  i6 ++) {
	    SW.Send(ent->SuffixTitlesValue(i6));
	  }
	  SW.CloseSub();
	}
	else {
	  SW.SendUndef();
	}
}
