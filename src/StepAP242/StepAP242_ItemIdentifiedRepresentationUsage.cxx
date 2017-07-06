// Created on: 2015-07-10
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <StepAP242_ItemIdentifiedRepresentationUsage.hxx>

#include <StepAP242_ItemIdentifiedRepresentationUsageDefinition.hxx>
#include <StepRepr_HArray1OfRepresentationItem.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepAP242_ItemIdentifiedRepresentationUsage,Standard_Transient)

//=======================================================================
//function : StepAP242_ItemIdentifiedRepresentationUsage
//purpose  : 
//=======================================================================

StepAP242_ItemIdentifiedRepresentationUsage::StepAP242_ItemIdentifiedRepresentationUsage ()  {}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepAP242_ItemIdentifiedRepresentationUsage::Init(
  const Handle(TCollection_HAsciiString)& theName,
  const Handle(TCollection_HAsciiString)& theDescription,
  const StepAP242_ItemIdentifiedRepresentationUsageDefinition& theDefinition,
  const Handle(StepRepr_Representation)& theUsedRepresentation,
  const Handle(StepRepr_HArray1OfRepresentationItem)& theIdentifiedItem)
{
  // --- classe own fields ---
  name = theName;
  description = theDescription;
  definition = theDefinition;
  usedRepresentation = theUsedRepresentation;
  identifiedItem = theIdentifiedItem;
}
