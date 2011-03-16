#include <StepVisual_PresentationLayerAssignment.ixx>


StepVisual_PresentationLayerAssignment::StepVisual_PresentationLayerAssignment ()  {}

void StepVisual_PresentationLayerAssignment::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(TCollection_HAsciiString)& aDescription,
	const Handle(StepVisual_HArray1OfLayeredItem)& aAssignedItems)
{
	// --- classe own fields ---
	name = aName;
	description = aDescription;
	assignedItems = aAssignedItems;
}


void StepVisual_PresentationLayerAssignment::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepVisual_PresentationLayerAssignment::Name() const
{
	return name;
}

void StepVisual_PresentationLayerAssignment::SetDescription(const Handle(TCollection_HAsciiString)& aDescription)
{
	description = aDescription;
}

Handle(TCollection_HAsciiString) StepVisual_PresentationLayerAssignment::Description() const
{
	return description;
}

void StepVisual_PresentationLayerAssignment::SetAssignedItems(const Handle(StepVisual_HArray1OfLayeredItem)& aAssignedItems)
{
	assignedItems = aAssignedItems;
}

Handle(StepVisual_HArray1OfLayeredItem) StepVisual_PresentationLayerAssignment::AssignedItems() const
{
	return assignedItems;
}

StepVisual_LayeredItem StepVisual_PresentationLayerAssignment::AssignedItemsValue(const Standard_Integer num) const
{
	return assignedItems->Value(num);
}

Standard_Integer StepVisual_PresentationLayerAssignment::NbAssignedItems () const
{
	return assignedItems->Length();
}
