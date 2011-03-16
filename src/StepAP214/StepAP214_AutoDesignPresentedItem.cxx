#include <StepAP214_AutoDesignPresentedItem.ixx>


StepAP214_AutoDesignPresentedItem::StepAP214_AutoDesignPresentedItem ()  {}

void StepAP214_AutoDesignPresentedItem::Init(
	const Handle(StepAP214_HArray1OfAutoDesignPresentedItemSelect)& aItems)
{
	// --- classe own fields ---
	items = aItems;
}


void StepAP214_AutoDesignPresentedItem::SetItems(const Handle(StepAP214_HArray1OfAutoDesignPresentedItemSelect)& aItems)
{
	items = aItems;
}

Handle(StepAP214_HArray1OfAutoDesignPresentedItemSelect) StepAP214_AutoDesignPresentedItem::Items() const
{
	return items;
}

StepAP214_AutoDesignPresentedItemSelect  StepAP214_AutoDesignPresentedItem::ItemsValue(const Standard_Integer num) const
{
	return items->Value(num);
}

Standard_Integer StepAP214_AutoDesignPresentedItem::NbItems () const
{
	if (items.IsNull()) return 0;
	return items->Length();
}
