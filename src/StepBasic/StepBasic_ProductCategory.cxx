#include <StepBasic_ProductCategory.ixx>


StepBasic_ProductCategory::StepBasic_ProductCategory ()  {}

void StepBasic_ProductCategory::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Boolean hasAdescription,
	const Handle(TCollection_HAsciiString)& aDescription)
{
	// --- classe own fields ---
	name = aName;
	hasDescription = hasAdescription;
	description = aDescription;
}


void StepBasic_ProductCategory::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepBasic_ProductCategory::Name() const
{
	return name;
}

void StepBasic_ProductCategory::SetDescription(const Handle(TCollection_HAsciiString)& aDescription)
{
	description = aDescription;
	hasDescription = Standard_True;
}

void StepBasic_ProductCategory::UnSetDescription()
{
	hasDescription = Standard_False;
	description.Nullify();
}

Handle(TCollection_HAsciiString) StepBasic_ProductCategory::Description() const
{
	return description;
}

Standard_Boolean StepBasic_ProductCategory::HasDescription() const
{
	return hasDescription;
}
