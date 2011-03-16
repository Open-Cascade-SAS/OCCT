
#include <RWStepBasic_RWApproval.ixx>
#include <StepBasic_ApprovalStatus.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepBasic_Approval.hxx>


RWStepBasic_RWApproval::RWStepBasic_RWApproval () {}

void RWStepBasic_RWApproval::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_Approval)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"approval")) return;

	// --- own field : status ---

	Handle(StepBasic_ApprovalStatus) aStatus;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadEntity(num, 1,"status", ach, STANDARD_TYPE(StepBasic_ApprovalStatus), aStatus);

	// --- own field : level ---

	Handle(TCollection_HAsciiString) aLevel;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadString (num,2,"level",ach,aLevel);

	//--- Initialisation of the read entity ---


	ent->Init(aStatus, aLevel);
}


void RWStepBasic_RWApproval::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_Approval)& ent) const
{

	// --- own field : status ---

	SW.Send(ent->Status());

	// --- own field : level ---

	SW.Send(ent->Level());
}


void RWStepBasic_RWApproval::Share(const Handle(StepBasic_Approval)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Status());
}

