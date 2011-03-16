
#include <RWStepRepr_RWRepresentationContext.ixx>


RWStepRepr_RWRepresentationContext::RWStepRepr_RWRepresentationContext () {}

void RWStepRepr_RWRepresentationContext::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepRepr_RepresentationContext)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"representation_context")) return;

	// --- own field : contextIdentifier ---

	Handle(TCollection_HAsciiString) aContextIdentifier;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"context_identifier",ach,aContextIdentifier);

	// --- own field : contextType ---

	Handle(TCollection_HAsciiString) aContextType;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadString (num,2,"context_type",ach,aContextType);

	//--- Initialisation of the read entity ---


	ent->Init(aContextIdentifier, aContextType);
}


void RWStepRepr_RWRepresentationContext::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepRepr_RepresentationContext)& ent) const
{

	// --- own field : contextIdentifier ---

	SW.Send(ent->ContextIdentifier());

	// --- own field : contextType ---

	SW.Send(ent->ContextType());
}
