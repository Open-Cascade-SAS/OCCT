#include <StepVisual_PointStyle.ixx>


StepVisual_PointStyle::StepVisual_PointStyle ()  {}

void StepVisual_PointStyle::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const StepVisual_MarkerSelect& aMarker,
	const StepBasic_SizeSelect& aMarkerSize,
	const Handle(StepVisual_Colour)& aMarkerColour)
{
	// --- classe own fields ---
	name = aName;
	marker = aMarker;
	markerSize = aMarkerSize;
	markerColour = aMarkerColour;
}


void StepVisual_PointStyle::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepVisual_PointStyle::Name() const
{
	return name;
}

void StepVisual_PointStyle::SetMarker(const StepVisual_MarkerSelect& aMarker)
{
	marker = aMarker;
}

StepVisual_MarkerSelect StepVisual_PointStyle::Marker() const
{
	return marker;
}

void StepVisual_PointStyle::SetMarkerSize(const StepBasic_SizeSelect& aMarkerSize)
{
	markerSize = aMarkerSize;
}

StepBasic_SizeSelect StepVisual_PointStyle::MarkerSize() const
{
	return markerSize;
}

void StepVisual_PointStyle::SetMarkerColour(const Handle(StepVisual_Colour)& aMarkerColour)
{
	markerColour = aMarkerColour;
}

Handle(StepVisual_Colour) StepVisual_PointStyle::MarkerColour() const
{
	return markerColour;
}
