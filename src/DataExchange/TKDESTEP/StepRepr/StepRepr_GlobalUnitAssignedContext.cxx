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

#include <StepRepr_GlobalUnitAssignedContext.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_GlobalUnitAssignedContext, StepRepr_RepresentationContext)

StepRepr_GlobalUnitAssignedContext::StepRepr_GlobalUnitAssignedContext() {}

void StepRepr_GlobalUnitAssignedContext::Init(
  const occ::handle<TCollection_HAsciiString>&     aContextIdentifier,
  const occ::handle<TCollection_HAsciiString>&     aContextType,
  const occ::handle<NCollection_HArray1<occ::handle<StepBasic_NamedUnit>>>& aUnits)
{
  // --- classe own fields ---
  units = aUnits;
  // --- classe inherited fields ---
  StepRepr_RepresentationContext::Init(aContextIdentifier, aContextType);
}

void StepRepr_GlobalUnitAssignedContext::SetUnits(
  const occ::handle<NCollection_HArray1<occ::handle<StepBasic_NamedUnit>>>& aUnits)
{
  units = aUnits;
}

occ::handle<NCollection_HArray1<occ::handle<StepBasic_NamedUnit>>> StepRepr_GlobalUnitAssignedContext::Units() const
{
  return units;
}

occ::handle<StepBasic_NamedUnit> StepRepr_GlobalUnitAssignedContext::UnitsValue(
  const int num) const
{
  return units->Value(num);
}

int StepRepr_GlobalUnitAssignedContext::NbUnits() const
{
  if (units.IsNull())
    return 0;
  return units->Length();
}
