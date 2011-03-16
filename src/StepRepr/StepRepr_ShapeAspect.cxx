#include <StepRepr_ShapeAspect.ixx>


StepRepr_ShapeAspect::StepRepr_ShapeAspect ()  {}

void StepRepr_ShapeAspect::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(TCollection_HAsciiString)& aDescription,
	const Handle(StepRepr_ProductDefinitionShape)& aOfShape,
	const StepData_Logical aProductDefinitional)
{
	// --- classe own fields ---
	name = aName;
	description = aDescription;
	ofShape = aOfShape;
	productDefinitional = aProductDefinitional;
}


void StepRepr_ShapeAspect::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepRepr_ShapeAspect::Name() const
{
	return name;
}

void StepRepr_ShapeAspect::SetDescription(const Handle(TCollection_HAsciiString)& aDescription)
{
	description = aDescription;
}

Handle(TCollection_HAsciiString) StepRepr_ShapeAspect::Description() const
{
	return description;
}

void StepRepr_ShapeAspect::SetOfShape(const Handle(StepRepr_ProductDefinitionShape)& aOfShape)
{
	ofShape = aOfShape;
}

Handle(StepRepr_ProductDefinitionShape) StepRepr_ShapeAspect::OfShape() const
{
	return ofShape;
}

void StepRepr_ShapeAspect::SetProductDefinitional(const StepData_Logical aProductDefinitional)
{
	productDefinitional = aProductDefinitional;
}

StepData_Logical StepRepr_ShapeAspect::ProductDefinitional() const
{
	return productDefinitional;
}
