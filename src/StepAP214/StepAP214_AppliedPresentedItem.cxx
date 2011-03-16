#include <StepAP214_AppliedPresentedItem.ixx>


StepAP214_AppliedPresentedItem::StepAP214_AppliedPresentedItem ()  {}

void StepAP214_AppliedPresentedItem::Init(
	const Handle(StepAP214_HArray1OfPresentedItemSelect)& aItems)
{
  // --- classe own fields ---
  items = aItems;
}


void StepAP214_AppliedPresentedItem::SetItems(const Handle(StepAP214_HArray1OfPresentedItemSelect)& aItems)
{
  items = aItems;
}

Handle(StepAP214_HArray1OfPresentedItemSelect) StepAP214_AppliedPresentedItem::Items() const
{
  return items;
}

StepAP214_PresentedItemSelect  StepAP214_AppliedPresentedItem::ItemsValue(const Standard_Integer num) const
{
  return items->Value(num);
}

Standard_Integer StepAP214_AppliedPresentedItem::NbItems () const
{
  if (items.IsNull()) return 0;
  return items->Length();
}
