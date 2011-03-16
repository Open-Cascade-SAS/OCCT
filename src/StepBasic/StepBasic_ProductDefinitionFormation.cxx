#include <StepBasic_ProductDefinitionFormation.ixx>


StepBasic_ProductDefinitionFormation::StepBasic_ProductDefinitionFormation ()  {}

void StepBasic_ProductDefinitionFormation::Init(
	const Handle(TCollection_HAsciiString)& aId,
	const Handle(TCollection_HAsciiString)& aDescription,
	const Handle(StepBasic_Product)& aOfProduct)
{
	// --- classe own fields ---
	id = aId;
	description = aDescription;
	ofProduct = aOfProduct;
}


void StepBasic_ProductDefinitionFormation::SetId(const Handle(TCollection_HAsciiString)& aId)
{
	id = aId;
}

Handle(TCollection_HAsciiString) StepBasic_ProductDefinitionFormation::Id() const
{
	return id;
}

void StepBasic_ProductDefinitionFormation::SetDescription(const Handle(TCollection_HAsciiString)& aDescription)
{
	description = aDescription;
}

Handle(TCollection_HAsciiString) StepBasic_ProductDefinitionFormation::Description() const
{
	return description;
}

void StepBasic_ProductDefinitionFormation::SetOfProduct(const Handle(StepBasic_Product)& aOfProduct)
{
	ofProduct = aOfProduct;
}

Handle(StepBasic_Product) StepBasic_ProductDefinitionFormation::OfProduct() const
{
	return ofProduct;
}
