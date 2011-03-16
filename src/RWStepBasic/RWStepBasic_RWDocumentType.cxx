
#include <RWStepBasic_RWDocumentType.ixx>

#include <Interface_EntityIterator.hxx>


#include <StepBasic_DocumentType.hxx>
#include <TCollection_HAsciiString.hxx>


RWStepBasic_RWDocumentType::RWStepBasic_RWDocumentType () {}

void RWStepBasic_RWDocumentType::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_DocumentType)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"document_type")) return;

	// --- own field : product_data_type ---

	Handle(TCollection_HAsciiString) aId;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"product_data_type",ach,aId);

	//--- Initialisation of the read entity ---


	ent->Init(aId);
}


void RWStepBasic_RWDocumentType::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_DocumentType)& ent) const
{

	// --- own field : id ---

	SW.Send(ent->ProductDataType());
}


void RWStepBasic_RWDocumentType::Share(const Handle(StepBasic_DocumentType)& ent, Interface_EntityIterator& iter) const
{
}

