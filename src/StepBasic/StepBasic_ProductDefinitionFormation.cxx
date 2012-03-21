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

#include <StepBasic_ProductDefinitionFormation.ixx>


StepBasic_ProductDefinitionFormation::StepBasic_ProductDefinitionFormation ()  {}

void StepBasic_ProductDefinitionFormation::Init(
	const Handle(TCollection_HAsciiString)& aId,
	const Handle(TCollection_HAsciiString)& aDescription,
	const Handle(StepBasic_Product)& aOfProduct)
{
	// --- classe own fields ---
	id = aId;
	description = aDescription;
	ofProduct = aOfProduct;
}


void StepBasic_ProductDefinitionFormation::SetId(const Handle(TCollection_HAsciiString)& aId)
{
	id = aId;
}

Handle(TCollection_HAsciiString) StepBasic_ProductDefinitionFormation::Id() const
{
	return id;
}

void StepBasic_ProductDefinitionFormation::SetDescription(const Handle(TCollection_HAsciiString)& aDescription)
{
	description = aDescription;
}

Handle(TCollection_HAsciiString) StepBasic_ProductDefinitionFormation::Description() const
{
	return description;
}

void StepBasic_ProductDefinitionFormation::SetOfProduct(const Handle(StepBasic_Product)& aOfProduct)
{
	ofProduct = aOfProduct;
}

Handle(StepBasic_Product) StepBasic_ProductDefinitionFormation::OfProduct() const
{
	return ofProduct;
}
