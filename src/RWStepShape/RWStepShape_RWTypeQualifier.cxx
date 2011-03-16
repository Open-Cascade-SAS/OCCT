#include <RWStepShape_RWTypeQualifier.ixx>


RWStepShape_RWTypeQualifier::RWStepShape_RWTypeQualifier () {}

void RWStepShape_RWTypeQualifier::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_TypeQualifier)& ent) const
{
	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"type_qualifier")) return;

	// --- own field : name ---

	Handle(TCollection_HAsciiString) aName;
	data->ReadString (num,1,"name",ach,aName);

	//--- Initialisation of the read entity ---

	ent->Init(aName);
}


void RWStepShape_RWTypeQualifier::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_TypeQualifier)& ent) const
{
  SW.Send(ent->Name());
}
