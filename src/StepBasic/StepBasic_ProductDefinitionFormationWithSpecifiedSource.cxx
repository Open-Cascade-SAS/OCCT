#include <StepBasic_ProductDefinitionFormationWithSpecifiedSource.ixx>


StepBasic_ProductDefinitionFormationWithSpecifiedSource::StepBasic_ProductDefinitionFormationWithSpecifiedSource ()  {}

void StepBasic_ProductDefinitionFormationWithSpecifiedSource::Init(
	const Handle(TCollection_HAsciiString)& aId,
	const Handle(TCollection_HAsciiString)& aDescription,
	const Handle(StepBasic_Product)& aOfProduct)
{

	StepBasic_ProductDefinitionFormation::Init(aId, aDescription, aOfProduct);
}

void StepBasic_ProductDefinitionFormationWithSpecifiedSource::Init(
	const Handle(TCollection_HAsciiString)& aId,
	const Handle(TCollection_HAsciiString)& aDescription,
	const Handle(StepBasic_Product)& aOfProduct,
	const StepBasic_Source aMakeOrBuy)
{
	// --- classe own fields ---
	makeOrBuy = aMakeOrBuy;
	// --- classe inherited fields ---
	StepBasic_ProductDefinitionFormation::Init(aId, aDescription, aOfProduct);
}


void StepBasic_ProductDefinitionFormationWithSpecifiedSource::SetMakeOrBuy(const StepBasic_Source aMakeOrBuy)
{
	makeOrBuy = aMakeOrBuy;
}

StepBasic_Source StepBasic_ProductDefinitionFormationWithSpecifiedSource::MakeOrBuy() const
{
	return makeOrBuy;
}
