// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _StepBasic_ProductRelatedProductCategory_HeaderFile
#define _StepBasic_ProductRelatedProductCategory_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_Product.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepBasic_ProductCategory.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class StepBasic_Product;

class StepBasic_ProductRelatedProductCategory : public StepBasic_ProductCategory
{

public:
  //! Returns a ProductRelatedProductCategory
  Standard_EXPORT StepBasic_ProductRelatedProductCategory();

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>&   aName,
                            const bool                    hasAdescription,
                            const occ::handle<TCollection_HAsciiString>&   aDescription,
                            const occ::handle<NCollection_HArray1<occ::handle<StepBasic_Product>>>& aProducts);

  Standard_EXPORT void SetProducts(const occ::handle<NCollection_HArray1<occ::handle<StepBasic_Product>>>& aProducts);

  Standard_EXPORT occ::handle<NCollection_HArray1<occ::handle<StepBasic_Product>>> Products() const;

  Standard_EXPORT occ::handle<StepBasic_Product> ProductsValue(const int num) const;

  Standard_EXPORT int NbProducts() const;

  DEFINE_STANDARD_RTTIEXT(StepBasic_ProductRelatedProductCategory, StepBasic_ProductCategory)

private:
  occ::handle<NCollection_HArray1<occ::handle<StepBasic_Product>>> products;
};

#endif // _StepBasic_ProductRelatedProductCategory_HeaderFile
