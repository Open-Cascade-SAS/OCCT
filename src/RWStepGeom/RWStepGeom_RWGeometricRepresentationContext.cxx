
#include <RWStepGeom_RWGeometricRepresentationContext.ixx>


RWStepGeom_RWGeometricRepresentationContext::RWStepGeom_RWGeometricRepresentationContext () {}

void RWStepGeom_RWGeometricRepresentationContext::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_GeometricRepresentationContext)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"geometric_representation_context")) return;

	// --- inherited field : contextIdentifier ---

	Handle(TCollection_HAsciiString) aContextIdentifier;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"context_identifier",ach,aContextIdentifier);

	// --- inherited field : contextType ---

	Handle(TCollection_HAsciiString) aContextType;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadString (num,2,"context_type",ach,aContextType);

	// --- own field : coordinateSpaceDimension ---

	Standard_Integer aCoordinateSpaceDimension;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadInteger (num,3,"coordinate_space_dimension",ach,aCoordinateSpaceDimension);

	//--- Initialisation of the read entity ---


	ent->Init(aContextIdentifier, aContextType, aCoordinateSpaceDimension);
}


void RWStepGeom_RWGeometricRepresentationContext::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_GeometricRepresentationContext)& ent) const
{

	// --- inherited field contextIdentifier ---

	SW.Send(ent->ContextIdentifier());

	// --- inherited field contextType ---

	SW.Send(ent->ContextType());

	// --- own field : coordinateSpaceDimension ---

	SW.Send(ent->CoordinateSpaceDimension());
}
