// File:	RWStepBasic_RWMechanicalContext.cxx
// Created:	Wed Jul 24 14:37:55 1996
// Author:	Frederic MAUPAS
//		<fma@pronox.paris1.matra-dtv.fr>

#include <RWStepBasic_RWMechanicalContext.ixx>
#include <StepBasic_ApplicationContext.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepBasic_MechanicalContext.hxx>


RWStepBasic_RWMechanicalContext::RWStepBasic_RWMechanicalContext () {}

void RWStepBasic_RWMechanicalContext::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_MechanicalContext)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"mechanical_context")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : frameOfReference ---

	Handle(StepBasic_ApplicationContext) aFrameOfReference;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"frame_of_reference", ach, STANDARD_TYPE(StepBasic_ApplicationContext), aFrameOfReference);

	// --- own field : disciplineType ---

	Handle(TCollection_HAsciiString) aDisciplineType;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadString (num,3,"discipline_type",ach,aDisciplineType);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aFrameOfReference, aDisciplineType);
}


void RWStepBasic_RWMechanicalContext::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_MechanicalContext)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field frameOfReference ---

	SW.Send(ent->FrameOfReference());

	// --- own field : disciplineType ---

	SW.Send(ent->DisciplineType());
}


void RWStepBasic_RWMechanicalContext::Share(const Handle(StepBasic_MechanicalContext)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->FrameOfReference());
}

