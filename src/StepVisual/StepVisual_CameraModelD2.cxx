#include <StepVisual_CameraModelD2.ixx>


StepVisual_CameraModelD2::StepVisual_CameraModelD2 ()  {}

void StepVisual_CameraModelD2::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepVisual_CameraModelD2::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepVisual_PlanarBox)& aViewWindow,
	const Standard_Boolean aViewWindowClipping)
{
	// --- classe own fields ---
	viewWindow = aViewWindow;
	viewWindowClipping = aViewWindowClipping;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepVisual_CameraModelD2::SetViewWindow(const Handle(StepVisual_PlanarBox)& aViewWindow)
{
	viewWindow = aViewWindow;
}

Handle(StepVisual_PlanarBox) StepVisual_CameraModelD2::ViewWindow() const
{
	return viewWindow;
}

void StepVisual_CameraModelD2::SetViewWindowClipping(const Standard_Boolean aViewWindowClipping)
{
	viewWindowClipping = aViewWindowClipping;
}

Standard_Boolean StepVisual_CameraModelD2::ViewWindowClipping() const
{
	return viewWindowClipping;
}
