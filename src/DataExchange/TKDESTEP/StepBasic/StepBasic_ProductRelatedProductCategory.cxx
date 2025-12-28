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

#include <StepBasic_ProductRelatedProductCategory.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_ProductRelatedProductCategory, StepBasic_ProductCategory)

StepBasic_ProductRelatedProductCategory::StepBasic_ProductRelatedProductCategory() {}

void StepBasic_ProductRelatedProductCategory::Init(
  const occ::handle<TCollection_HAsciiString>&                            aName,
  const bool                                                              hasAdescription,
  const occ::handle<TCollection_HAsciiString>&                            aDescription,
  const occ::handle<NCollection_HArray1<occ::handle<StepBasic_Product>>>& aProducts)
{
  // --- classe own fields ---
  products = aProducts;
  // --- classe inherited fields ---
  StepBasic_ProductCategory::Init(aName, hasAdescription, aDescription);
}

void StepBasic_ProductRelatedProductCategory::SetProducts(
  const occ::handle<NCollection_HArray1<occ::handle<StepBasic_Product>>>& aProducts)
{
  products = aProducts;
}

occ::handle<NCollection_HArray1<occ::handle<StepBasic_Product>>>
  StepBasic_ProductRelatedProductCategory::Products() const
{
  return products;
}

occ::handle<StepBasic_Product> StepBasic_ProductRelatedProductCategory::ProductsValue(
  const int num) const
{
  return products->Value(num);
}

int StepBasic_ProductRelatedProductCategory::NbProducts() const
{
  if (products.IsNull())
    return 0;
  return products->Length();
}
