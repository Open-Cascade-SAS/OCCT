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
#include <StepRepr_RepresentationItem.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepAP242_ItemIdentifiedRepresentationUsage, Standard_Transient)

//=================================================================================================

StepAP242_ItemIdentifiedRepresentationUsage::StepAP242_ItemIdentifiedRepresentationUsage() {}

//=================================================================================================

void StepAP242_ItemIdentifiedRepresentationUsage::Init(
  const occ::handle<TCollection_HAsciiString>&                 theName,
  const occ::handle<TCollection_HAsciiString>&                 theDescription,
  const StepAP242_ItemIdentifiedRepresentationUsageDefinition& theDefinition,
  const occ::handle<StepRepr_Representation>&                  theUsedRepresentation,
  const occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>>&
    theIdentifiedItem)
{
  // --- classe own fields ---
  name               = theName;
  description        = theDescription;
  definition         = theDefinition;
  usedRepresentation = theUsedRepresentation;
  identifiedItem     = theIdentifiedItem;
}
