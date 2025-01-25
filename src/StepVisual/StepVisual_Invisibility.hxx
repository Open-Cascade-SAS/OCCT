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

#ifndef _StepVisual_Invisibility_HeaderFile
#define _StepVisual_Invisibility_HeaderFile

#include <Standard.hxx>

#include <StepVisual_HArray1OfInvisibleItem.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
class StepVisual_InvisibleItem;

class StepVisual_Invisibility;
DEFINE_STANDARD_HANDLE(StepVisual_Invisibility, Standard_Transient)

class StepVisual_Invisibility : public Standard_Transient
{

public:
  //! Returns a Invisibility
  Standard_EXPORT StepVisual_Invisibility();

  Standard_EXPORT void Init(const Handle(StepVisual_HArray1OfInvisibleItem)& aInvisibleItems);

  Standard_EXPORT void SetInvisibleItems(
    const Handle(StepVisual_HArray1OfInvisibleItem)& aInvisibleItems);

  Standard_EXPORT Handle(StepVisual_HArray1OfInvisibleItem) InvisibleItems() const;

  Standard_EXPORT StepVisual_InvisibleItem InvisibleItemsValue(const Standard_Integer num) const;

  Standard_EXPORT Standard_Integer NbInvisibleItems() const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_Invisibility, Standard_Transient)

protected:
private:
  Handle(StepVisual_HArray1OfInvisibleItem) invisibleItems;
};

#endif // _StepVisual_Invisibility_HeaderFile
