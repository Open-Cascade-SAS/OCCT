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

#include <StepBasic_ProductRelatedProductCategory.ixx>


StepBasic_ProductRelatedProductCategory::StepBasic_ProductRelatedProductCategory ()  {}

void StepBasic_ProductRelatedProductCategory::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Boolean hasAdescription,
	const Handle(TCollection_HAsciiString)& aDescription)
{

	StepBasic_ProductCategory::Init(aName, hasAdescription, aDescription);
}

void StepBasic_ProductRelatedProductCategory::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Boolean hasAdescription,
	const Handle(TCollection_HAsciiString)& aDescription,
	const Handle(StepBasic_HArray1OfProduct)& aProducts)
{
	// --- classe own fields ---
	products = aProducts;
	// --- classe inherited fields ---
	StepBasic_ProductCategory::Init(aName, hasAdescription, aDescription);
}


void StepBasic_ProductRelatedProductCategory::SetProducts(const Handle(StepBasic_HArray1OfProduct)& aProducts)
{
	products = aProducts;
}

Handle(StepBasic_HArray1OfProduct) StepBasic_ProductRelatedProductCategory::Products() const
{
	return products;
}

Handle(StepBasic_Product) StepBasic_ProductRelatedProductCategory::ProductsValue(const Standard_Integer num) const
{
	return products->Value(num);
}

Standard_Integer StepBasic_ProductRelatedProductCategory::NbProducts () const
{
	if (products.IsNull()) return 0;
	return products->Length();
}
