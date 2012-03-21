// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <StepRepr_GlobalUnitAssignedContext.ixx>


StepRepr_GlobalUnitAssignedContext::StepRepr_GlobalUnitAssignedContext ()  {}

void StepRepr_GlobalUnitAssignedContext::Init(
	const Handle(TCollection_HAsciiString)& aContextIdentifier,
	const Handle(TCollection_HAsciiString)& aContextType)
{

	StepRepr_RepresentationContext::Init(aContextIdentifier, aContextType);
}

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
