// Created on: 2016-08-25
// Created by: Irina KRYLOVA
// Copyright (c) 2016 OPEN CASCADE SAS
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

#include <StepRepr_CharacterizedRepresentation.hxx>
#include <StepRepr_RepresentationContext.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_CharacterizedRepresentation, StepRepr_Representation)

//=================================================================================================

StepRepr_CharacterizedRepresentation::StepRepr_CharacterizedRepresentation() {}

//=================================================================================================

void StepRepr_CharacterizedRepresentation::Init(
  const Handle(TCollection_HAsciiString)&             theName,
  const Handle(TCollection_HAsciiString)&             theDescription,
  const Handle(StepRepr_HArray1OfRepresentationItem)& theItems,
  const Handle(StepRepr_RepresentationContext)&       theContextOfItems)
{
  myDescription = theDescription;
  StepRepr_Representation::Init(theName, theItems, theContextOfItems);
}
