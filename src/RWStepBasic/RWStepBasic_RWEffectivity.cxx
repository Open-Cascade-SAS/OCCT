
#include <RWStepBasic_RWEffectivity.ixx>

#include <Interface_EntityIterator.hxx>


#include <StepBasic_Effectivity.hxx>
#include <TCollection_HAsciiString.hxx>


RWStepBasic_RWEffectivity::RWStepBasic_RWEffectivity () {}

void RWStepBasic_RWEffectivity::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_Effectivity)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"effectivity")) return;

	// --- own field : id ---

	Handle(TCollection_HAsciiString) aId;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"id",ach,aId);

	//--- Initialisation of the read entity ---


	ent->Init(aId);
}


void RWStepBasic_RWEffectivity::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_Effectivity)& ent) const
{

	// --- own field : id ---

	SW.Send(ent->Id());
}


void RWStepBasic_RWEffectivity::Share(const Handle(StepBasic_Effectivity)& ent, Interface_EntityIterator& iter) const
{
}

