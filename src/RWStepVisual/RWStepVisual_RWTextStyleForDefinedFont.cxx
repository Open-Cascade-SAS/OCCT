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
#include <RWStepVisual_RWTextStyleForDefinedFont.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_Colour.hxx>
#include <StepVisual_TextStyleForDefinedFont.hxx>

RWStepVisual_RWTextStyleForDefinedFont::RWStepVisual_RWTextStyleForDefinedFont () {}

void RWStepVisual_RWTextStyleForDefinedFont::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_TextStyleForDefinedFont)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"text_style_for_defined_font has not 1 parameter(s)")) return;

	// --- own field : textColour ---

	Handle(StepVisual_Colour) aTextColour;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadEntity(num, 1,"text_colour", ach, STANDARD_TYPE(StepVisual_Colour), aTextColour);

	//--- Initialisation of the read entity ---


	ent->Init(aTextColour);
}


void RWStepVisual_RWTextStyleForDefinedFont::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_TextStyleForDefinedFont)& ent) const
{

	// --- own field : textColour ---

	SW.Send(ent->TextColour());
}


void RWStepVisual_RWTextStyleForDefinedFont::Share(const Handle(StepVisual_TextStyleForDefinedFont)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->TextColour());
}

