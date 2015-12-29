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
#include <StepBasic_NamedUnit.hxx>
#include <StepRepr_GlobalUnitAssignedContext.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_GlobalUnitAssignedContext,StepRepr_RepresentationContext)

StepRepr_GlobalUnitAssignedContext::StepRepr_GlobalUnitAssignedContext ()  {}

void StepRepr_GlobalUnitAssignedContext::Init(
	const Handle(TCollection_HAsciiString)& aContextIdentifier,
	const Handle(TCollection_HAsciiString)& aContextType,
	const Handle(StepBasic_HArray1OfNamedUnit)& aUnits)
{
	// --- classe own fields ---
	units = aUnits;
	// --- classe inherited fields ---
	StepRepr_RepresentationContext::Init(aContextIdentifier, aContextType);
}


void StepRepr_GlobalUnitAssignedContext::SetUnits(const Handle(StepBasic_HArray1OfNamedUnit)& aUnits)
{
	units = aUnits;
}

Handle(StepBasic_HArray1OfNamedUnit) StepRepr_GlobalUnitAssignedContext::Units() const
{
	return units;
}

Handle(StepBasic_NamedUnit) StepRepr_GlobalUnitAssignedContext::UnitsValue(const Standard_Integer num) const
{
	return units->Value(num);
}

Standard_Integer StepRepr_GlobalUnitAssignedContext::NbUnits () const
{
	if (units.IsNull()) return 0;
	return units->Length();
}
