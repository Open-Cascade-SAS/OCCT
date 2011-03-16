#include <StepVisual_CameraModelD3.ixx>


StepVisual_CameraModelD3::StepVisual_CameraModelD3 ()  {}

void StepVisual_CameraModelD3::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepVisual_CameraModelD3::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Axis2Placement3d)& aViewReferenceSystem,
	const Handle(StepVisual_ViewVolume)& aPerspectiveOfVolume)
{
	// --- classe own fields ---
	viewReferenceSystem = aViewReferenceSystem;
	perspectiveOfVolume = aPerspectiveOfVolume;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepVisual_CameraModelD3::SetViewReferenceSystem(const Handle(StepGeom_Axis2Placement3d)& aViewReferenceSystem)
{
	viewReferenceSystem = aViewReferenceSystem;
}

Handle(StepGeom_Axis2Placement3d) StepVisual_CameraModelD3::ViewReferenceSystem() const
{
	return viewReferenceSystem;
}

void StepVisual_CameraModelD3::SetPerspectiveOfVolume(const Handle(StepVisual_ViewVolume)& aPerspectiveOfVolume)
{
	perspectiveOfVolume = aPerspectiveOfVolume;
}

Handle(StepVisual_ViewVolume) StepVisual_CameraModelD3::PerspectiveOfVolume() const
{
	return perspectiveOfVolume;
}
