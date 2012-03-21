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

#include <StepBasic_Product.ixx>


StepBasic_Product::StepBasic_Product ()  {}

void StepBasic_Product::Init(
	const Handle(TCollection_HAsciiString)& aId,
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(TCollection_HAsciiString)& aDescription,
	const Handle(StepBasic_HArray1OfProductContext)& aFrameOfReference)
{
	// --- classe own fields ---
	id = aId;
	name = aName;
	description = aDescription;
	frameOfReference = aFrameOfReference;
}


void StepBasic_Product::SetId(const Handle(TCollection_HAsciiString)& aId)
{
	id = aId;
}

Handle(TCollection_HAsciiString) StepBasic_Product::Id() const
{
	return id;
}

void StepBasic_Product::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepBasic_Product::Name() const
{
	return name;
}

void StepBasic_Product::SetDescription(const Handle(TCollection_HAsciiString)& aDescription)
{
	description = aDescription;
}

Handle(TCollection_HAsciiString) StepBasic_Product::Description() const
{
	return description;
}

void StepBasic_Product::SetFrameOfReference(const Handle(StepBasic_HArray1OfProductContext)& aFrameOfReference)
{
	frameOfReference = aFrameOfReference;
}

Handle(StepBasic_HArray1OfProductContext) StepBasic_Product::FrameOfReference() const
{
	return frameOfReference;
}

Handle(StepBasic_ProductContext) StepBasic_Product::FrameOfReferenceValue(const Standard_Integer num) const
{
	return frameOfReference->Value(num);
}

Standard_Integer StepBasic_Product::NbFrameOfReference () const
{
	if (frameOfReference.IsNull()) return 0;
	return frameOfReference->Length();
}
