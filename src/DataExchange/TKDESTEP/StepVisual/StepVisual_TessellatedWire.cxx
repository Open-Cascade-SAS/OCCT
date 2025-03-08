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

#include <StepVisual_TessellatedWire.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_TessellatedWire, StepVisual_TessellatedItem)

//=================================================================================================

StepVisual_TessellatedWire::StepVisual_TessellatedWire()
{
  myHasGeometricModelLink = Standard_False;
}

//=================================================================================================

void StepVisual_TessellatedWire::Init(
  const Handle(TCollection_HAsciiString)&                    theRepresentationItem_Name,
  const Handle(StepVisual_HArray1OfTessellatedEdgeOrVertex)& theItems,
  const Standard_Boolean                                     theHasGeometricModelLink,
  const StepVisual_PathOrCompositeCurve&                     theGeometricModelLink)
{
  StepVisual_TessellatedItem::Init(theRepresentationItem_Name);

  myItems = theItems;

  myHasGeometricModelLink = theHasGeometricModelLink;
  if (myHasGeometricModelLink)
  {
    myGeometricModelLink = theGeometricModelLink;
  }
  else
  {
    myGeometricModelLink = StepVisual_PathOrCompositeCurve();
  }
}

//=================================================================================================

Handle(StepVisual_HArray1OfTessellatedEdgeOrVertex) StepVisual_TessellatedWire::Items() const
{
  return myItems;
}

//=================================================================================================

void StepVisual_TessellatedWire::SetItems(
  const Handle(StepVisual_HArray1OfTessellatedEdgeOrVertex)& theItems)
{
  myItems = theItems;
}

//=================================================================================================

Standard_Integer StepVisual_TessellatedWire::NbItems() const
{
  if (myItems.IsNull())
  {
    return 0;
  }
  return myItems->Length();
}

//=================================================================================================

const StepVisual_TessellatedEdgeOrVertex& StepVisual_TessellatedWire::ItemsValue(
  const Standard_Integer theNum) const
{
  return myItems->Value(theNum);
}

//=================================================================================================

StepVisual_PathOrCompositeCurve StepVisual_TessellatedWire::GeometricModelLink() const
{
  return myGeometricModelLink;
}

//=================================================================================================

void StepVisual_TessellatedWire::SetGeometricModelLink(
  const StepVisual_PathOrCompositeCurve& theGeometricModelLink)
{
  myGeometricModelLink = theGeometricModelLink;
}

//=================================================================================================

Standard_Boolean StepVisual_TessellatedWire::HasGeometricModelLink() const
{
  return myHasGeometricModelLink;
}
