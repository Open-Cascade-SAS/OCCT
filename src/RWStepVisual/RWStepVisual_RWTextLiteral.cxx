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
#include <RWStepVisual_RWTextLiteral.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_Axis2Placement.hxx>
#include <StepVisual_FontSelect.hxx>
#include <StepVisual_TextLiteral.hxx>
#include <StepVisual_TextPath.hxx>
#include <TCollection_AsciiString.hxx>

// --- Enum : TextPath ---
static TCollection_AsciiString tpUp(".UP.");
static TCollection_AsciiString tpRight(".RIGHT.");
static TCollection_AsciiString tpDown(".DOWN.");
static TCollection_AsciiString tpLeft(".LEFT.");

RWStepVisual_RWTextLiteral::RWStepVisual_RWTextLiteral () {}

void RWStepVisual_RWTextLiteral::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_TextLiteral)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,6,ach,"text_literal has not 6 parameter(s)")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : literal ---

	Handle(TCollection_HAsciiString) aLiteral;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadString (num,2,"literal",ach,aLiteral);

	// --- own field : placement ---

	StepGeom_Axis2Placement aPlacement;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadEntity(num,3,"placement",ach,aPlacement);

	// --- own field : alignment ---

	Handle(TCollection_HAsciiString) aAlignment;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadString (num,4,"alignment",ach,aAlignment);

	// --- own field : path ---

	StepVisual_TextPath aPath = StepVisual_tpUp;
	if (data->ParamType(num,5) == Interface_ParamEnum) {
	  Standard_CString text = data->ParamCValue(num,5);
	  if      (tpUp.IsEqual(text)) aPath = StepVisual_tpUp;
	  else if (tpRight.IsEqual(text)) aPath = StepVisual_tpRight;
	  else if (tpDown.IsEqual(text)) aPath = StepVisual_tpDown;
	  else if (tpLeft.IsEqual(text)) aPath = StepVisual_tpLeft;
	  else ach->AddFail("Enumeration text_path has not an allowed value");
	}
	else ach->AddFail("Parameter #5 (path) is not an enumeration");

	// --- own field : font ---

	StepVisual_FontSelect aFont;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat6 =` not needed
	data->ReadEntity(num,6,"font",ach,aFont);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aLiteral, aPlacement, aAlignment, aPath, aFont);
}


void RWStepVisual_RWTextLiteral::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_TextLiteral)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : literal ---

	SW.Send(ent->Literal());

	// --- own field : placement ---

	SW.Send(ent->Placement().Value());

	// --- own field : alignment ---

	SW.Send(ent->Alignment());

	// --- own field : path ---

	switch(ent->Path()) {
	  case StepVisual_tpUp : SW.SendEnum (tpUp); break;
	  case StepVisual_tpRight : SW.SendEnum (tpRight); break;
	  case StepVisual_tpDown : SW.SendEnum (tpDown); break;
	  case StepVisual_tpLeft : SW.SendEnum (tpLeft); break;
	}

	// --- own field : font ---

	SW.Send(ent->Font().Value());
}


void RWStepVisual_RWTextLiteral::Share(const Handle(StepVisual_TextLiteral)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Placement().Value());


	iter.GetOneItem(ent->Font().Value());
}

