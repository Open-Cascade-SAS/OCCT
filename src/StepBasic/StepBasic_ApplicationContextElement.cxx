#include <StepBasic_ApplicationContextElement.ixx>


StepBasic_ApplicationContextElement::StepBasic_ApplicationContextElement ()  {}

void StepBasic_ApplicationContextElement::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepBasic_ApplicationContext)& aFrameOfReference)
{
	// --- classe own fields ---
	name = aName;
	frameOfReference = aFrameOfReference;
}


void StepBasic_ApplicationContextElement::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepBasic_ApplicationContextElement::Name() const
{
	return name;
}

void StepBasic_ApplicationContextElement::SetFrameOfReference(const Handle(StepBasic_ApplicationContext)& aFrameOfReference)
{
	frameOfReference = aFrameOfReference;
}

Handle(StepBasic_ApplicationContext) StepBasic_ApplicationContextElement::FrameOfReference() const
{
	return frameOfReference;
}
