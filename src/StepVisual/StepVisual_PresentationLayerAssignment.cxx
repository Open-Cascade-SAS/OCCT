// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
