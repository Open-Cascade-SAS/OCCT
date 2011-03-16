#include <StepVisual_ColourRgb.ixx>


StepVisual_ColourRgb::StepVisual_ColourRgb ()  {}

void StepVisual_ColourRgb::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepVisual_ColourSpecification::Init(aName);
}

void StepVisual_ColourRgb::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Real aRed,
	const Standard_Real aGreen,
	const Standard_Real aBlue)
{
	// --- classe own fields ---
	red = aRed;
	green = aGreen;
	blue = aBlue;
	// --- classe inherited fields ---
	StepVisual_ColourSpecification::Init(aName);
}


void StepVisual_ColourRgb::SetRed(const Standard_Real aRed)
{
	red = aRed;
}

Standard_Real StepVisual_ColourRgb::Red() const
{
	return red;
}

void StepVisual_ColourRgb::SetGreen(const Standard_Real aGreen)
{
	green = aGreen;
}

Standard_Real StepVisual_ColourRgb::Green() const
{
	return green;
}

void StepVisual_ColourRgb::SetBlue(const Standard_Real aBlue)
{
	blue = aBlue;
}

Standard_Real StepVisual_ColourRgb::Blue() const
{
	return blue;
}
