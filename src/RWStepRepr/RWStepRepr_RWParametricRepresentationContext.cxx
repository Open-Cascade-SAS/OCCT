
#include <RWStepRepr_RWParametricRepresentationContext.ixx>


RWStepRepr_RWParametricRepresentationContext::RWStepRepr_RWParametricRepresentationContext () {}

void RWStepRepr_RWParametricRepresentationContext::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepRepr_ParametricRepresentationContext)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"parametric_representation_context")) return;

	// --- inherited field : contextIdentifier ---

	Handle(TCollection_HAsciiString) aContextIdentifier;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"context_identifier",ach,aContextIdentifier);

	// --- inherited field : contextType ---

	Handle(TCollection_HAsciiString) aContextType;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadString (num,2,"context_type",ach,aContextType);

	//--- Initialisation of the read entity ---


	ent->Init(aContextIdentifier, aContextType);
}


void RWStepRepr_RWParametricRepresentationContext::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepRepr_ParametricRepresentationContext)& ent) const
{

	// --- inherited field contextIdentifier ---

	SW.Send(ent->ContextIdentifier());

	// --- inherited field contextType ---

	SW.Send(ent->ContextType());
}
