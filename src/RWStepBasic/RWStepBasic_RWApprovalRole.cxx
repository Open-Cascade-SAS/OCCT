
#include <RWStepBasic_RWApprovalRole.ixx>


RWStepBasic_RWApprovalRole::RWStepBasic_RWApprovalRole () {}

void RWStepBasic_RWApprovalRole::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_ApprovalRole)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"approval_role")) return;

	// --- own field : role ---

	Handle(TCollection_HAsciiString) aRole;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"role",ach,aRole);

	//--- Initialisation of the read entity ---


	ent->Init(aRole);
}


void RWStepBasic_RWApprovalRole::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_ApprovalRole)& ent) const
{

	// --- own field : role ---

	SW.Send(ent->Role());
}
