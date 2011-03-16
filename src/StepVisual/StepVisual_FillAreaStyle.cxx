#include <StepVisual_FillAreaStyle.ixx>


StepVisual_FillAreaStyle::StepVisual_FillAreaStyle ()  {}

void StepVisual_FillAreaStyle::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepVisual_HArray1OfFillStyleSelect)& aFillStyles)
{
	// --- classe own fields ---
	name = aName;
	fillStyles = aFillStyles;
}


void StepVisual_FillAreaStyle::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepVisual_FillAreaStyle::Name() const
{
	return name;
}

void StepVisual_FillAreaStyle::SetFillStyles(const Handle(StepVisual_HArray1OfFillStyleSelect)& aFillStyles)
{
	fillStyles = aFillStyles;
}

Handle(StepVisual_HArray1OfFillStyleSelect) StepVisual_FillAreaStyle::FillStyles() const
{
	return fillStyles;
}

StepVisual_FillStyleSelect StepVisual_FillAreaStyle::FillStylesValue(const Standard_Integer num) const
{
	return fillStyles->Value(num);
}

Standard_Integer StepVisual_FillAreaStyle::NbFillStyles () const
{
	return fillStyles->Length();
}
