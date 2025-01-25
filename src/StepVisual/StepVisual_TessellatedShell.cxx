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

#include <StepVisual_TessellatedShell.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_TessellatedShell, StepVisual_TessellatedItem)

//=================================================================================================

StepVisual_TessellatedShell::StepVisual_TessellatedShell()
{
  myHasTopologicalLink = Standard_False;
}

//=================================================================================================

void StepVisual_TessellatedShell::Init(
  const Handle(TCollection_HAsciiString)&                      theRepresentationItem_Name,
  const Handle(StepVisual_HArray1OfTessellatedStructuredItem)& theItems,
  const Standard_Boolean                                       theHasTopologicalLink,
  const Handle(StepShape_ConnectedFaceSet)&                    theTopologicalLink)
{
  StepVisual_TessellatedItem::Init(theRepresentationItem_Name);

  myItems = theItems;

  myHasTopologicalLink = theHasTopologicalLink;
  if (myHasTopologicalLink)
  {
    myTopologicalLink = theTopologicalLink;
  }
  else
  {
    myTopologicalLink.Nullify();
  }
}

//=================================================================================================

Handle(StepVisual_HArray1OfTessellatedStructuredItem) StepVisual_TessellatedShell::Items() const
{
  return myItems;
}

//=================================================================================================

void StepVisual_TessellatedShell::SetItems(
  const Handle(StepVisual_HArray1OfTessellatedStructuredItem)& theItems)
{
  myItems = theItems;
}

//=================================================================================================

Standard_Integer StepVisual_TessellatedShell::NbItems() const
{
  if (myItems.IsNull())
  {
    return 0;
  }
  return myItems->Length();
}

//=================================================================================================

Handle(StepVisual_TessellatedStructuredItem) StepVisual_TessellatedShell::ItemsValue(
  const Standard_Integer theNum) const
{
  return myItems->Value(theNum);
}

//=================================================================================================

Handle(StepShape_ConnectedFaceSet) StepVisual_TessellatedShell::TopologicalLink() const
{
  return myTopologicalLink;
}

//=================================================================================================

void StepVisual_TessellatedShell::SetTopologicalLink(
  const Handle(StepShape_ConnectedFaceSet)& theTopologicalLink)
{
  myTopologicalLink = theTopologicalLink;
}

//=================================================================================================

Standard_Boolean StepVisual_TessellatedShell::HasTopologicalLink() const
{
  return myHasTopologicalLink;
}
