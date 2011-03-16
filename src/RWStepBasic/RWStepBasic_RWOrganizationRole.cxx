
#include <RWStepBasic_RWOrganizationRole.ixx>


RWStepBasic_RWOrganizationRole::RWStepBasic_RWOrganizationRole () {}

void RWStepBasic_RWOrganizationRole::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_OrganizationRole)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"organization_role")) return;

	// --- own field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	//--- Initialisation of the read entity ---


	ent->Init(aName);
}


void RWStepBasic_RWOrganizationRole::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_OrganizationRole)& ent) const
{

	// --- own field : name ---

	SW.Send(ent->Name());
}
