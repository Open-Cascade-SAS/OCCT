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

#include <StepRepr_GlobalUncertaintyAssignedContext.ixx>


StepRepr_GlobalUncertaintyAssignedContext::StepRepr_GlobalUncertaintyAssignedContext ()  {}

void StepRepr_GlobalUncertaintyAssignedContext::Init(
	const Handle(TCollection_HAsciiString)& aContextIdentifier,
	const Handle(TCollection_HAsciiString)& aContextType)
{

	StepRepr_RepresentationContext::Init(aContextIdentifier, aContextType);
}

void StepRepr_GlobalUncertaintyAssignedContext::Init(
	const Handle(TCollection_HAsciiString)& aContextIdentifier,
	const Handle(TCollection_HAsciiString)& aContextType,
	const Handle(StepBasic_HArray1OfUncertaintyMeasureWithUnit)& aUncertainty)
{
	// --- classe own fields ---
	uncertainty = aUncertainty;
	// --- classe inherited fields ---
	StepRepr_RepresentationContext::Init(aContextIdentifier, aContextType);
}


void StepRepr_GlobalUncertaintyAssignedContext::SetUncertainty(const Handle(StepBasic_HArray1OfUncertaintyMeasureWithUnit)& aUncertainty)
{
	uncertainty = aUncertainty;
}

Handle(StepBasic_HArray1OfUncertaintyMeasureWithUnit) StepRepr_GlobalUncertaintyAssignedContext::Uncertainty() const
{
	return uncertainty;
}

Handle(StepBasic_UncertaintyMeasureWithUnit) StepRepr_GlobalUncertaintyAssignedContext::UncertaintyValue(const Standard_Integer num) const
{
	return uncertainty->Value(num);
}

Standard_Integer StepRepr_GlobalUncertaintyAssignedContext::NbUncertainty () const
{
	if (uncertainty.IsNull()) return 0;
	return uncertainty->Length();
}
