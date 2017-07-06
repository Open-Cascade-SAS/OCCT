// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <Standard_Type.hxx>
#include <StepBasic_Product.hxx>
#include <StepBasic_ProductDefinitionFormation.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_ProductDefinitionFormation,Standard_Transient)

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
