
#include <RWStepBasic_RWDocumentUsageConstraint.ixx>

#include <Interface_EntityIterator.hxx>


#include <StepBasic_Document.hxx>
#include <TCollection_HAsciiString.hxx>


RWStepBasic_RWDocumentUsageConstraint::RWStepBasic_RWDocumentUsageConstraint () {}

void RWStepBasic_RWDocumentUsageConstraint::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_DocumentUsageConstraint)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"document_usage_constraint")) return;

	// --- own field : source ---

	Handle(StepBasic_Document) aSource;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadEntity (num, 1,"source", ach, STANDARD_TYPE(StepBasic_Document), aSource);

	// --- own field : label ---

	Handle(TCollection_HAsciiString) aLabel;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadString (num,2,"subject_element",ach,aLabel);

	// --- own field : description ---

	Handle(TCollection_HAsciiString) aDescription;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadString (num,3,"subject_element_value",ach,aDescription);

	//--- Initialisation of the read entity ---


	ent->Init(aSource, aLabel, aDescription);
}


void RWStepBasic_RWDocumentUsageConstraint::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_DocumentUsageConstraint)& ent) const
{

	// --- own field : id ---

	SW.Send(ent->Source());

	// --- own field : subject_element ---

	SW.Send(ent->SubjectElement());

	// --- own field : subject_element_value ---

	SW.Send(ent->SubjectElementValue());
}


void RWStepBasic_RWDocumentUsageConstraint::Share(const Handle(StepBasic_DocumentUsageConstraint)& ent, Interface_EntityIterator& iter) const
{
  iter.AddItem (ent->Source());
}
