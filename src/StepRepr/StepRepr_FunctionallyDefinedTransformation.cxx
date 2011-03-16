#include <StepRepr_FunctionallyDefinedTransformation.ixx>


StepRepr_FunctionallyDefinedTransformation::StepRepr_FunctionallyDefinedTransformation ()  {}

void StepRepr_FunctionallyDefinedTransformation::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(TCollection_HAsciiString)& aDescription)
{
	// --- classe own fields ---
	name = aName;
	description = aDescription;
}


void StepRepr_FunctionallyDefinedTransformation::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepRepr_FunctionallyDefinedTransformation::Name() const
{
	return name;
}

void StepRepr_FunctionallyDefinedTransformation::SetDescription(const Handle(TCollection_HAsciiString)& aDescription)
{
	description = aDescription;
}

Handle(TCollection_HAsciiString) StepRepr_FunctionallyDefinedTransformation::Description() const
{
	return description;
}
