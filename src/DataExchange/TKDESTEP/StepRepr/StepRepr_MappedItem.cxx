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

#include <StepRepr_MappedItem.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepRepr_RepresentationMap.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_MappedItem, StepRepr_RepresentationItem)

StepRepr_MappedItem::StepRepr_MappedItem() = default;

void StepRepr_MappedItem::Init(const occ::handle<TCollection_HAsciiString>&    aName,
                               const occ::handle<StepRepr_RepresentationMap>&  aMappingSource,
                               const occ::handle<StepRepr_RepresentationItem>& aMappingTarget)
{
  // --- classe own fields ---
  mappingSource = aMappingSource;
  mappingTarget = aMappingTarget;
  // --- classe inherited fields ---
  StepRepr_RepresentationItem::Init(aName);
}

void StepRepr_MappedItem::SetMappingSource(
  const occ::handle<StepRepr_RepresentationMap>& aMappingSource)
{
  mappingSource = aMappingSource;
}

occ::handle<StepRepr_RepresentationMap> StepRepr_MappedItem::MappingSource() const
{
  return mappingSource;
}

void StepRepr_MappedItem::SetMappingTarget(
  const occ::handle<StepRepr_RepresentationItem>& aMappingTarget)
{
  mappingTarget = aMappingTarget;
}

occ::handle<StepRepr_RepresentationItem> StepRepr_MappedItem::MappingTarget() const
{
  return mappingTarget;
}
