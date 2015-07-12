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
#include <RWStepBasic_RWCoordinatedUniversalTimeOffset.hxx>
#include <StepBasic_AheadOrBehind.hxx>
#include <StepBasic_CoordinatedUniversalTimeOffset.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <TCollection_AsciiString.hxx>

// --- Enum : AheadOrBehind ---
static TCollection_AsciiString aobAhead(".AHEAD.");
static TCollection_AsciiString aobExact(".EXACT.");
static TCollection_AsciiString aobBehind(".BEHIND.");

RWStepBasic_RWCoordinatedUniversalTimeOffset::RWStepBasic_RWCoordinatedUniversalTimeOffset () {}

void RWStepBasic_RWCoordinatedUniversalTimeOffset::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_CoordinatedUniversalTimeOffset)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"coordinated_universal_time_offset")) return;

	// --- own field : hourOffset ---

	Standard_Integer aHourOffset;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadInteger (num,1,"hour_offset",ach,aHourOffset);

	// --- own field : minuteOffset ---

	Standard_Integer aMinuteOffset;
	Standard_Boolean hasAminuteOffset = Standard_True;
	if (data->IsParamDefined(num,2)) {
	  //szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	  data->ReadInteger (num,2,"minute_offset",ach,aMinuteOffset);
	}
	else {
	  hasAminuteOffset = Standard_False;
	  aMinuteOffset = 0;
	}

	// --- own field : sense ---

	StepBasic_AheadOrBehind aSense = StepBasic_aobAhead;
	if (data->ParamType(num,3) == Interface_ParamEnum) {
	  Standard_CString text = data->ParamCValue(num,3);
	  if      (aobAhead.IsEqual(text)) aSense = StepBasic_aobAhead;
	  else if (aobExact.IsEqual(text)) aSense = StepBasic_aobExact;
	  else if (aobBehind.IsEqual(text)) aSense = StepBasic_aobBehind;
	  else ach->AddFail("Enumeration ahead_or_behind has not an allowed value");
	}
	else ach->AddFail("Parameter #3 (sense) is not an enumeration");

	//--- Initialisation of the read entity ---


	ent->Init(aHourOffset, hasAminuteOffset, aMinuteOffset, aSense);
}


void RWStepBasic_RWCoordinatedUniversalTimeOffset::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_CoordinatedUniversalTimeOffset)& ent) const
{

	// --- own field : hourOffset ---

	SW.Send(ent->HourOffset());

	// --- own field : minuteOffset ---

	Standard_Boolean hasAminuteOffset = ent->HasMinuteOffset();
	if (hasAminuteOffset) {
	  SW.Send(ent->MinuteOffset());
	}
	else {
	  SW.SendUndef();
	}

	// --- own field : sense ---

	switch(ent->Sense()) {
	  case StepBasic_aobAhead : SW.SendEnum (aobAhead); break;
	  case StepBasic_aobExact : SW.SendEnum (aobExact); break;
	  case StepBasic_aobBehind : SW.SendEnum (aobBehind); break;
	}
}
