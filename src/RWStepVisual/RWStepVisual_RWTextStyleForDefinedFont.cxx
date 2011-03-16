
#include <RWStepVisual_RWTextStyleForDefinedFont.ixx>
#include <StepVisual_Colour.hxx>


#include <Interface_EntityIterator.hxx>


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

