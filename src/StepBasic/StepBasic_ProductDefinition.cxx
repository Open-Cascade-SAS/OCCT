#include <StepBasic_ProductDefinition.ixx>


StepBasic_ProductDefinition::StepBasic_ProductDefinition ()  {}

void StepBasic_ProductDefinition::Init(
	const Handle(TCollection_HAsciiString)& aId,
	const Handle(TCollection_HAsciiString)& aDescription,
	const Handle(StepBasic_ProductDefinitionFormation)& aFormation,
	const Handle(StepBasic_ProductDefinitionContext)& aFrameOfReference)
{
	// --- classe own fields ---
	id = aId;
	description = aDescription;
	formation = aFormation;
	frameOfReference = aFrameOfReference;
}


void StepBasic_ProductDefinition::SetId(const Handle(TCollection_HAsciiString)& aId)
{
	id = aId;
}

Handle(TCollection_HAsciiString) StepBasic_ProductDefinition::Id() const
{
	return id;
}

void StepBasic_ProductDefinition::SetDescription(const Handle(TCollection_HAsciiString)& aDescription)
{
	description = aDescription;
}

Handle(TCollection_HAsciiString) StepBasic_ProductDefinition::Description() const
{
	return description;
}

void StepBasic_ProductDefinition::SetFormation(const Handle(StepBasic_ProductDefinitionFormation)& aFormation)
{
	formation = aFormation;
}

Handle(StepBasic_ProductDefinitionFormation) StepBasic_ProductDefinition::Formation() const
{
	return formation;
}

void StepBasic_ProductDefinition::SetFrameOfReference(const Handle(StepBasic_ProductDefinitionContext)& aFrameOfReference)
{
	frameOfReference = aFrameOfReference;
}

Handle(StepBasic_ProductDefinitionContext) StepBasic_ProductDefinition::FrameOfReference() const
{
	return frameOfReference;
}
