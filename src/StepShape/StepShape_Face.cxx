#include <StepShape_Face.ixx>


StepShape_Face::StepShape_Face ()  {}

void StepShape_Face::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepShape_Face::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_HArray1OfFaceBound)& aBounds)
{
	// --- classe own fields ---
	bounds = aBounds;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_Face::SetBounds(const Handle(StepShape_HArray1OfFaceBound)& aBounds)
{
	bounds = aBounds;
}

Handle(StepShape_HArray1OfFaceBound) StepShape_Face::Bounds() const
{
	return bounds;
}

Handle(StepShape_FaceBound) StepShape_Face::BoundsValue(const Standard_Integer num) const
{
	return bounds->Value(num);
}

Standard_Integer StepShape_Face::NbBounds () const
{
	if (bounds.IsNull()) return 0;
	return bounds->Length();
}
