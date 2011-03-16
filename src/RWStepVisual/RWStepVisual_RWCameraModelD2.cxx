
#include <RWStepVisual_RWCameraModelD2.ixx>
#include <StepVisual_PlanarBox.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepVisual_CameraModelD2.hxx>


RWStepVisual_RWCameraModelD2::RWStepVisual_RWCameraModelD2 () {}

void RWStepVisual_RWCameraModelD2::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_CameraModelD2)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"camera_model_d2")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : viewWindow ---

	Handle(StepVisual_PlanarBox) aViewWindow;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"view_window", ach, STANDARD_TYPE(StepVisual_PlanarBox), aViewWindow);

	// --- own field : viewWindowClipping ---

	Standard_Boolean aViewWindowClipping;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadBoolean (num,3,"view_window_clipping",ach,aViewWindowClipping);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aViewWindow, aViewWindowClipping);
}


void RWStepVisual_RWCameraModelD2::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_CameraModelD2)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : viewWindow ---

	SW.Send(ent->ViewWindow());

	// --- own field : viewWindowClipping ---

	SW.SendBoolean(ent->ViewWindowClipping());
}


void RWStepVisual_RWCameraModelD2::Share(const Handle(StepVisual_CameraModelD2)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->ViewWindow());
}

