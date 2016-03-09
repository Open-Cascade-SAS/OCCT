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
#include <StepVisual_TessellatedGeometricSet.hxx>


IMPLEMENT_STANDARD_RTTIEXT(StepVisual_TessellatedGeometricSet,StepGeom_TessellatedItem)


StepVisual_TessellatedGeometricSet::StepVisual_TessellatedGeometricSet ()  {}

void StepVisual_TessellatedGeometricSet::Init(const Handle(TCollection_HAsciiString)& theName, const NCollection_Handle<StepVisual_Array1OfTessellatedItem>& theItems)
{
  StepRepr_RepresentationItem::Init(theName);
  myItems = theItems;
}

NCollection_Handle<StepVisual_Array1OfTessellatedItem> StepVisual_TessellatedGeometricSet::Items() const
{
  return myItems;
}
