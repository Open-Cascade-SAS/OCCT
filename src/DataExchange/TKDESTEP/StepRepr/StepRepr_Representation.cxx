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

#include <StepRepr_Representation.hxx>
#include <StepRepr_RepresentationContext.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_Representation, Standard_Transient)

StepRepr_Representation::StepRepr_Representation() = default;

void StepRepr_Representation::Init(
  const occ::handle<TCollection_HAsciiString>&                                      aName,
  const occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>>& aItems,
  const occ::handle<StepRepr_RepresentationContext>&                                aContextOfItems)
{
  // --- classe own fields ---
  name           = aName;
  items          = aItems;
  contextOfItems = aContextOfItems;
}

void StepRepr_Representation::SetName(const occ::handle<TCollection_HAsciiString>& aName)
{
  name = aName;
}

occ::handle<TCollection_HAsciiString> StepRepr_Representation::Name() const
{
  return name;
}

void StepRepr_Representation::SetItems(
  const occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>>& aItems)
{
  items = aItems;
}

occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> StepRepr_Representation::
  Items() const
{
  return items;
}

occ::handle<StepRepr_RepresentationItem> StepRepr_Representation::ItemsValue(const int num) const
{
  return items->Value(num);
}

int StepRepr_Representation::NbItems() const
{
  if (items.IsNull())
    return 0;
  return items->Length();
}

void StepRepr_Representation::SetContextOfItems(
  const occ::handle<StepRepr_RepresentationContext>& aContextOfItems)
{
  contextOfItems = aContextOfItems;
}

occ::handle<StepRepr_RepresentationContext> StepRepr_Representation::ContextOfItems() const
{
  return contextOfItems;
}
