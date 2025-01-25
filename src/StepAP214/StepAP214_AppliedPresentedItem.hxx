// Created on: 1999-03-10
// Created by: data exchange team
// Copyright (c) 1999 Matra Datavision
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

#ifndef _StepAP214_AppliedPresentedItem_HeaderFile
#define _StepAP214_AppliedPresentedItem_HeaderFile

#include <Standard.hxx>

#include <StepAP214_HArray1OfPresentedItemSelect.hxx>
#include <StepVisual_PresentedItem.hxx>
#include <Standard_Integer.hxx>
class StepAP214_PresentedItemSelect;

class StepAP214_AppliedPresentedItem;
DEFINE_STANDARD_HANDLE(StepAP214_AppliedPresentedItem, StepVisual_PresentedItem)

class StepAP214_AppliedPresentedItem : public StepVisual_PresentedItem
{

public:
  //! Returns a AutoDesignPresentedItem
  Standard_EXPORT StepAP214_AppliedPresentedItem();

  Standard_EXPORT void Init(const Handle(StepAP214_HArray1OfPresentedItemSelect)& aItems);

  Standard_EXPORT void SetItems(const Handle(StepAP214_HArray1OfPresentedItemSelect)& aItems);

  Standard_EXPORT Handle(StepAP214_HArray1OfPresentedItemSelect) Items() const;

  Standard_EXPORT StepAP214_PresentedItemSelect ItemsValue(const Standard_Integer num) const;

  Standard_EXPORT Standard_Integer NbItems() const;

  DEFINE_STANDARD_RTTIEXT(StepAP214_AppliedPresentedItem, StepVisual_PresentedItem)

protected:
private:
  Handle(StepAP214_HArray1OfPresentedItemSelect) items;
};

#endif // _StepAP214_AppliedPresentedItem_HeaderFile
