
#include <RWStepBasic_RWSecurityClassificationLevel.ixx>


RWStepBasic_RWSecurityClassificationLevel::RWStepBasic_RWSecurityClassificationLevel () {}

void RWStepBasic_RWSecurityClassificationLevel::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_SecurityClassificationLevel)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"security_classification_level")) return;

	// --- own field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	//--- Initialisation of the read entity ---


	ent->Init(aName);
}


void RWStepBasic_RWSecurityClassificationLevel::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_SecurityClassificationLevel)& ent) const
{

	// --- own field : name ---

	SW.Send(ent->Name());
}
