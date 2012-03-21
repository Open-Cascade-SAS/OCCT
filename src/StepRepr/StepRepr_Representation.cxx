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
