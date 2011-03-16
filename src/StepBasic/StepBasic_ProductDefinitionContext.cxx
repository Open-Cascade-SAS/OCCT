#include <StepBasic_ProductDefinitionContext.ixx>


StepBasic_ProductDefinitionContext::StepBasic_ProductDefinitionContext ()  {}

void StepBasic_ProductDefinitionContext::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepBasic_ApplicationContext)& aFrameOfReference)
{

	StepBasic_ApplicationContextElement::Init(aName, aFrameOfReference);
}

void StepBasic_ProductDefinitionContext::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepBasic_ApplicationContext)& aFrameOfReference,
	const Handle(TCollection_HAsciiString)& aLifeCycleStage)
{
	// --- classe own fields ---
	lifeCycleStage = aLifeCycleStage;
	// --- classe inherited fields ---
	StepBasic_ApplicationContextElement::Init(aName, aFrameOfReference);
}


void StepBasic_ProductDefinitionContext::SetLifeCycleStage(const Handle(TCollection_HAsciiString)& aLifeCycleStage)
{
	lifeCycleStage = aLifeCycleStage;
}

Handle(TCollection_HAsciiString) StepBasic_ProductDefinitionContext::LifeCycleStage() const
{
	return lifeCycleStage;
}
