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

//=======================================================================
//function : StepVisual_TessellatedSolid
//purpose  : 
//=======================================================================

StepVisual_TessellatedSolid::StepVisual_TessellatedSolid ()
{
  myHasGeometricLink = Standard_False;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepVisual_TessellatedSolid::Init (const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                                        const Handle(StepVisual_HArray1OfTessellatedStructuredItem)& theItems,
                                        const Standard_Boolean theHasGeometricLink,
                                        const Handle(StepShape_ManifoldSolidBrep)& theGeometricLink)
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

//=======================================================================
//function : Items
//purpose  : 
//=======================================================================

Handle(StepVisual_HArray1OfTessellatedStructuredItem) StepVisual_TessellatedSolid::Items () const
{
  return myItems;
}

//=======================================================================
//function : SetItems
//purpose  : 
//=======================================================================

void StepVisual_TessellatedSolid::SetItems(const Handle(StepVisual_HArray1OfTessellatedStructuredItem)& theItems)
{
  myItems = theItems;
}


//=======================================================================
//function : NbItems
//purpose  : 
//=======================================================================

Standard_Integer StepVisual_TessellatedSolid::NbItems() const
{
  if (myItems.IsNull())
  {
    return 0;
  }
  return myItems->Length();
}


//=======================================================================
//function : ItemsValue
//purpose  : 
//=======================================================================

Handle(StepVisual_TessellatedStructuredItem) StepVisual_TessellatedSolid::ItemsValue(const Standard_Integer theNum) const
{
  return myItems->Value(theNum);
}

//=======================================================================
//function : GeometricLink
//purpose  : 
//=======================================================================

Handle(StepShape_ManifoldSolidBrep) StepVisual_TessellatedSolid::GeometricLink () const
{
  return myGeometricLink;
}

//=======================================================================
//function : SetGeometricLink
//purpose  : 
//=======================================================================

void StepVisual_TessellatedSolid::SetGeometricLink(const Handle(StepShape_ManifoldSolidBrep)& theGeometricLink)
{
  myGeometricLink = theGeometricLink;
}

//=======================================================================
//function : HasGeometricLink
//purpose  : 
//=======================================================================

Standard_Boolean StepVisual_TessellatedSolid::HasGeometricLink () const
{
  return myHasGeometricLink;
}
