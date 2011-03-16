
#include <RWStepGeom_RWGeometricRepresentationItem.ixx>


RWStepGeom_RWGeometricRepresentationItem::RWStepGeom_RWGeometricRepresentationItem () {}

void RWStepGeom_RWGeometricRepresentationItem::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_GeometricRepresentationItem)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"geometric_representation_item")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	//--- Initialisation of the read entity ---


	ent->Init(aName);
}


void RWStepGeom_RWGeometricRepresentationItem::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_GeometricRepresentationItem)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());
}
