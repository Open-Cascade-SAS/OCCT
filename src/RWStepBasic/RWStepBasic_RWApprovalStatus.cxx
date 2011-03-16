
#include <RWStepBasic_RWApprovalStatus.ixx>


RWStepBasic_RWApprovalStatus::RWStepBasic_RWApprovalStatus () {}

void RWStepBasic_RWApprovalStatus::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_ApprovalStatus)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"approval_status")) return;

	// --- own field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	//--- Initialisation of the read entity ---


	ent->Init(aName);
}


void RWStepBasic_RWApprovalStatus::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_ApprovalStatus)& ent) const
{

	// --- own field : name ---

	SW.Send(ent->Name());
}
