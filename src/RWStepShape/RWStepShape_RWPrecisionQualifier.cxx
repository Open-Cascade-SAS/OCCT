#include <RWStepShape_RWPrecisionQualifier.ixx>


RWStepShape_RWPrecisionQualifier::RWStepShape_RWPrecisionQualifier () {}

void RWStepShape_RWPrecisionQualifier::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_PrecisionQualifier)& ent) const
{
	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"precision_qualifier")) return;

	// --- own field : precision_value ---

	Standard_Integer PV;
	data->ReadInteger (num,1,"precision_value",ach,PV);

	//--- Initialisation of the read entity ---

	ent->Init(PV);
}


void RWStepShape_RWPrecisionQualifier::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_PrecisionQualifier)& ent) const
{
  SW.Send (ent->PrecisionValue());
}
