#include <StepShape_CsgSolid.ixx>


StepShape_CsgSolid::StepShape_CsgSolid ()  {}

void StepShape_CsgSolid::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepShape_CsgSolid::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const StepShape_CsgSelect& aTreeRootExpression)
{
	// --- classe own fields ---
	treeRootExpression = aTreeRootExpression;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_CsgSolid::SetTreeRootExpression(const StepShape_CsgSelect& aTreeRootExpression)
{
	treeRootExpression = aTreeRootExpression;
}

StepShape_CsgSelect StepShape_CsgSolid::TreeRootExpression() const
{
	return treeRootExpression;
}
