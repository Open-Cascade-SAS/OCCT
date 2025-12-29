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

#include <StepRepr_GlobalUncertaintyAssignedContext.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_GlobalUncertaintyAssignedContext,
                           StepRepr_RepresentationContext)

StepRepr_GlobalUncertaintyAssignedContext::StepRepr_GlobalUncertaintyAssignedContext() = default;

void StepRepr_GlobalUncertaintyAssignedContext::Init(
  const occ::handle<TCollection_HAsciiString>& aContextIdentifier,
  const occ::handle<TCollection_HAsciiString>& aContextType,
  const occ::handle<NCollection_HArray1<occ::handle<StepBasic_UncertaintyMeasureWithUnit>>>&
    aUncertainty)
{
  // --- classe own fields ---
  uncertainty = aUncertainty;
  // --- classe inherited fields ---
  StepRepr_RepresentationContext::Init(aContextIdentifier, aContextType);
}

void StepRepr_GlobalUncertaintyAssignedContext::SetUncertainty(
  const occ::handle<NCollection_HArray1<occ::handle<StepBasic_UncertaintyMeasureWithUnit>>>&
    aUncertainty)
{
  uncertainty = aUncertainty;
}

occ::handle<NCollection_HArray1<occ::handle<StepBasic_UncertaintyMeasureWithUnit>>>
  StepRepr_GlobalUncertaintyAssignedContext::Uncertainty() const
{
  return uncertainty;
}

occ::handle<StepBasic_UncertaintyMeasureWithUnit> StepRepr_GlobalUncertaintyAssignedContext::
  UncertaintyValue(const int num) const
{
  return uncertainty->Value(num);
}

int StepRepr_GlobalUncertaintyAssignedContext::NbUncertainty() const
{
  if (uncertainty.IsNull())
    return 0;
  return uncertainty->Length();
}
