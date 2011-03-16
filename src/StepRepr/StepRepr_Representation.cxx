#include <StepRepr_Representation.ixx>


StepRepr_Representation::StepRepr_Representation ()  {}

void StepRepr_Representation::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepRepr_HArray1OfRepresentationItem)& aItems,
	const Handle(StepRepr_RepresentationContext)& aContextOfItems)
{
	// --- classe own fields ---
	name = aName;
	items = aItems;
	contextOfItems = aContextOfItems;
}


void StepRepr_Representation::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepRepr_Representation::Name() const
{
	return name;
}

void StepRepr_Representation::SetItems(const Handle(StepRepr_HArray1OfRepresentationItem)& aItems)
{
	items = aItems;
}

Handle(StepRepr_HArray1OfRepresentationItem) StepRepr_Representation::Items() const
{
	return items;
}

Handle(StepRepr_RepresentationItem) StepRepr_Representation::ItemsValue(const Standard_Integer num) const
{
	return items->Value(num);
}

Standard_Integer StepRepr_Representation::NbItems () const
{
	if (items.IsNull()) return 0;
	return items->Length();
}

void StepRepr_Representation::SetContextOfItems(const Handle(StepRepr_RepresentationContext)& aContextOfItems)
{
	contextOfItems = aContextOfItems;
}

Handle(StepRepr_RepresentationContext) StepRepr_Representation::ContextOfItems() const
{
	return contextOfItems;
}
