//gka 05.03.99 S4134 upgrade from CD to DIS
#include <RWStepRepr_RWFunctionallyDefinedTransformation.ixx>


RWStepRepr_RWFunctionallyDefinedTransformation::RWStepRepr_RWFunctionallyDefinedTransformation () {}

void RWStepRepr_RWFunctionallyDefinedTransformation::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepRepr_FunctionallyDefinedTransformation)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"functionally_defined_transformation")) return;

	// --- own field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : description ---

	Handle(TCollection_HAsciiString) aDescription;
	if (data->IsParamDefined (num,2)) { //gka 05.03.99 S4134 upgrade from CD to DIS
	  //szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	  data->ReadString (num,2,"description",ach,aDescription);
	}
	//--- Initialisation of the read entity ---


	ent->Init(aName, aDescription);
}


void RWStepRepr_RWFunctionallyDefinedTransformation::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepRepr_FunctionallyDefinedTransformation)& ent) const
{

	// --- own field : name ---

	SW.Send(ent->Name());

	// --- own field : description ---

	SW.Send(ent->Description());
}
