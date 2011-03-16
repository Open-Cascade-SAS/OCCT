#include <StepVisual_Invisibility.ixx>


StepVisual_Invisibility::StepVisual_Invisibility ()  {}

void StepVisual_Invisibility::Init(
	const Handle(StepVisual_HArray1OfInvisibleItem)& aInvisibleItems)
{
	// --- classe own fields ---
	invisibleItems = aInvisibleItems;
}


void StepVisual_Invisibility::SetInvisibleItems(const Handle(StepVisual_HArray1OfInvisibleItem)& aInvisibleItems)
{
	invisibleItems = aInvisibleItems;
}

Handle(StepVisual_HArray1OfInvisibleItem) StepVisual_Invisibility::InvisibleItems() const
{
	return invisibleItems;
}

StepVisual_InvisibleItem StepVisual_Invisibility::InvisibleItemsValue(const Standard_Integer num) const
{
	return invisibleItems->Value(num);
}

Standard_Integer StepVisual_Invisibility::NbInvisibleItems () const
{
	return invisibleItems->Length();
}
