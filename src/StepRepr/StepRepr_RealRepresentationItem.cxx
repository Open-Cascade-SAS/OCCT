// Copyright (c) 2024 OPEN CASCADE SAS
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

#include <StepRepr_RealRepresentationItem.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_RealRepresentationItem, StepRepr_RepresentationItem)

StepRepr_RealRepresentationItem::StepRepr_RealRepresentationItem() {}

void StepRepr_RealRepresentationItem::Init(const Handle(TCollection_HAsciiString)& theName,
                                           const Standard_Real                     theValue)
{
  value = theValue;
  StepRepr_RepresentationItem::Init(theName);
}
