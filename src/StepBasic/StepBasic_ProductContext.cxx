#include <StepBasic_ProductContext.ixx>


StepBasic_ProductContext::StepBasic_ProductContext ()  {}

void StepBasic_ProductContext::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepBasic_ApplicationContext)& aFrameOfReference)
{

	StepBasic_ApplicationContextElement::Init(aName, aFrameOfReference);
}

void StepBasic_ProductContext::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepBasic_ApplicationContext)& aFrameOfReference,
	const Handle(TCollection_HAsciiString)& aDisciplineType)
{
	// --- classe own fields ---
	disciplineType = aDisciplineType;
	// --- classe inherited fields ---
	StepBasic_ApplicationContextElement::Init(aName, aFrameOfReference);
}


void StepBasic_ProductContext::SetDisciplineType(const Handle(TCollection_HAsciiString)& aDisciplineType)
{
	disciplineType = aDisciplineType;
}

Handle(TCollection_HAsciiString) StepBasic_ProductContext::DisciplineType() const
{
	return disciplineType;
}
