
#include <RWStepVisual_RWCameraModel.ixx>


RWStepVisual_RWCameraModel::RWStepVisual_RWCameraModel () {}

void RWStepVisual_RWCameraModel::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_CameraModel)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"camera_model has not 1 parameter(s)")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	//--- Initialisation of the read entity ---


	ent->Init(aName);
}


void RWStepVisual_RWCameraModel::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_CameraModel)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());
}
