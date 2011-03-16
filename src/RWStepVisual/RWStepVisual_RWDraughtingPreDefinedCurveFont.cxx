
#include <RWStepVisual_RWDraughtingPreDefinedCurveFont.ixx>


RWStepVisual_RWDraughtingPreDefinedCurveFont::RWStepVisual_RWDraughtingPreDefinedCurveFont () {}

void RWStepVisual_RWDraughtingPreDefinedCurveFont::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_DraughtingPreDefinedCurveFont)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"draughting_pre_defined_curve_font has not 1 parameter(s)")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	//--- Initialisation of the read entity ---


	ent->Init(aName);
}


void RWStepVisual_RWDraughtingPreDefinedCurveFont::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_DraughtingPreDefinedCurveFont)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());
}
