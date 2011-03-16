#include <StepVisual_SurfaceSideStyle.ixx>


StepVisual_SurfaceSideStyle::StepVisual_SurfaceSideStyle ()  {}

void StepVisual_SurfaceSideStyle::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepVisual_HArray1OfSurfaceStyleElementSelect)& aStyles)
{
	// --- classe own fields ---
	name = aName;
	styles = aStyles;
}


void StepVisual_SurfaceSideStyle::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepVisual_SurfaceSideStyle::Name() const
{
	return name;
}

void StepVisual_SurfaceSideStyle::SetStyles(const Handle(StepVisual_HArray1OfSurfaceStyleElementSelect)& aStyles)
{
	styles = aStyles;
}

Handle(StepVisual_HArray1OfSurfaceStyleElementSelect) StepVisual_SurfaceSideStyle::Styles() const
{
	return styles;
}

StepVisual_SurfaceStyleElementSelect StepVisual_SurfaceSideStyle::StylesValue(const Standard_Integer num) const
{
	return styles->Value(num);
}

Standard_Integer StepVisual_SurfaceSideStyle::NbStyles () const
{
	return styles->Length();
}
