#include <StepShape_GeometricSet.ixx>


StepShape_GeometricSet::StepShape_GeometricSet ()  {}

void StepShape_GeometricSet::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepShape_GeometricSet::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_HArray1OfGeometricSetSelect)& aElements)
{
	// --- classe own fields ---
	elements = aElements;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_GeometricSet::SetElements(const Handle(StepShape_HArray1OfGeometricSetSelect)& aElements)
{
	elements = aElements;
}

Handle(StepShape_HArray1OfGeometricSetSelect) StepShape_GeometricSet::Elements() const
{
	return elements;
}

StepShape_GeometricSetSelect StepShape_GeometricSet::ElementsValue(const Standard_Integer num) const
{
	return elements->Value(num);
}

Standard_Integer StepShape_GeometricSet::NbElements () const
{
	return elements->Length();
}
