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
#include <StepRepr_RepresentationItem.hxx>
#include <StepVisual_PresentationStyleAssignment.hxx>
#include <StepVisual_StyledItem.hxx>
#include <StepVisual_StyledItemTarget.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_StyledItem,StepRepr_RepresentationItem)

void StepVisual_StyledItem::Init(
  const Handle(TCollection_HAsciiString)& aName,
  const Handle(StepVisual_HArray1OfPresentationStyleAssignment)& aStyles,
  const Handle(Standard_Transient)& aItem)
{
  // --- classe own fields ---
  myStyles = aStyles;
  myItem = aItem;
  myReprItem = Handle(StepRepr_RepresentationItem)::DownCast(aItem);
  // --- classe inherited fields ---
  StepRepr_RepresentationItem::Init(aName);
}

void StepVisual_StyledItem::SetStyles(const Handle(StepVisual_HArray1OfPresentationStyleAssignment)& aStyles)
{
  myStyles = aStyles;
}

void StepVisual_StyledItem::SetItem(const Handle(StepRepr_RepresentationItem)& aItem)
{
  myItem = aItem;
  myReprItem = aItem;
}

void StepVisual_StyledItem::SetItem(const StepVisual_StyledItemTarget& theItem)
{
  myItem = theItem.Value();
  myReprItem = Handle(StepRepr_RepresentationItem)::DownCast(myItem);
}

StepVisual_StyledItemTarget StepVisual_StyledItem::ItemAP242() const
{
  StepVisual_StyledItemTarget anItem;
  if (anItem.CaseNum(myItem) > 0)
    anItem.SetValue(myItem);
  return anItem;
}
