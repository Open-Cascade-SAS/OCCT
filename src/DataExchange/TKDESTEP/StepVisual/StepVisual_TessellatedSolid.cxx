// Created on : Thu Mar 24 18:30:12 2022
// Created by: snn
// Generator: Express (EXPRESS -> CASCADE/XSTEP Translator) V2.0
// Copyright (c) Open CASCADE 2022
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

#include <StepVisual_TessellatedSolid.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_TessellatedSolid, StepVisual_TessellatedItem)

//=================================================================================================

StepVisual_TessellatedSolid::StepVisual_TessellatedSolid()
{
  myHasGeometricLink = false;
}

//=================================================================================================

void StepVisual_TessellatedSolid::Init(
  const occ::handle<TCollection_HAsciiString>& theRepresentationItem_Name,
  const occ::handle<NCollection_HArray1<occ::handle<StepVisual_TessellatedStructuredItem>>>&
                                                  theItems,
  const bool                                      theHasGeometricLink,
  const occ::handle<StepShape_ManifoldSolidBrep>& theGeometricLink)
{
  StepVisual_TessellatedItem::Init(theRepresentationItem_Name);

  myItems = theItems;

  myHasGeometricLink = theHasGeometricLink;
  if (myHasGeometricLink)
  {
    myGeometricLink = theGeometricLink;
  }
  else
  {
    myGeometricLink.Nullify();
  }
}

//=================================================================================================

occ::handle<NCollection_HArray1<occ::handle<StepVisual_TessellatedStructuredItem>>>
  StepVisual_TessellatedSolid::Items() const
{
  return myItems;
}

//=================================================================================================

void StepVisual_TessellatedSolid::SetItems(
  const occ::handle<NCollection_HArray1<occ::handle<StepVisual_TessellatedStructuredItem>>>&
    theItems)
{
  myItems = theItems;
}

//=================================================================================================

int StepVisual_TessellatedSolid::NbItems() const
{
  if (myItems.IsNull())
  {
    return 0;
  }
  return myItems->Length();
}

//=================================================================================================

occ::handle<StepVisual_TessellatedStructuredItem> StepVisual_TessellatedSolid::ItemsValue(
  const int theNum) const
{
  return myItems->Value(theNum);
}

//=================================================================================================

occ::handle<StepShape_ManifoldSolidBrep> StepVisual_TessellatedSolid::GeometricLink() const
{
  return myGeometricLink;
}

//=================================================================================================

void StepVisual_TessellatedSolid::SetGeometricLink(
  const occ::handle<StepShape_ManifoldSolidBrep>& theGeometricLink)
{
  myGeometricLink = theGeometricLink;
}

//=================================================================================================

bool StepVisual_TessellatedSolid::HasGeometricLink() const
{
  return myHasGeometricLink;
}
