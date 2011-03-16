#include <StepShape_BooleanResult.ixx>


StepShape_BooleanResult::StepShape_BooleanResult ()  {}

void StepShape_BooleanResult::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepShape_BooleanResult::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const StepShape_BooleanOperator aOperator,
	const StepShape_BooleanOperand& aFirstOperand,
	const StepShape_BooleanOperand& aSecondOperand)
{
	// --- classe own fields ---
	anOperator = aOperator;
	firstOperand = aFirstOperand;
	secondOperand = aSecondOperand;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_BooleanResult::SetOperator(const StepShape_BooleanOperator aOperator)
{
	anOperator = aOperator;
}

StepShape_BooleanOperator StepShape_BooleanResult::Operator() const
{
	return anOperator;
}

void StepShape_BooleanResult::SetFirstOperand(const StepShape_BooleanOperand& aFirstOperand)
{
	firstOperand = aFirstOperand;
}

StepShape_BooleanOperand StepShape_BooleanResult::FirstOperand() const
{
	return firstOperand;
}

void StepShape_BooleanResult::SetSecondOperand(const StepShape_BooleanOperand& aSecondOperand)
{
	secondOperand = aSecondOperand;
}

StepShape_BooleanOperand StepShape_BooleanResult::SecondOperand() const
{
	return secondOperand;
}
