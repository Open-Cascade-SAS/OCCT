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

#include <StepGeom_TrimmedCurve.ixx>


StepGeom_TrimmedCurve::StepGeom_TrimmedCurve ()  {}

void StepGeom_TrimmedCurve::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_TrimmedCurve::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Curve)& aBasisCurve,
	const Handle(StepGeom_HArray1OfTrimmingSelect)& aTrim1,
	const Handle(StepGeom_HArray1OfTrimmingSelect)& aTrim2,
	const Standard_Boolean aSenseAgreement,
	const StepGeom_TrimmingPreference aMasterRepresentation)
{
	// --- classe own fields ---
	basisCurve = aBasisCurve;
	trim1 = aTrim1;
	trim2 = aTrim2;
	senseAgreement = aSenseAgreement;
	masterRepresentation = aMasterRepresentation;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_TrimmedCurve::SetBasisCurve(const Handle(StepGeom_Curve)& aBasisCurve)
{
	basisCurve = aBasisCurve;
}

Handle(StepGeom_Curve) StepGeom_TrimmedCurve::BasisCurve() const
{
	return basisCurve;
}

void StepGeom_TrimmedCurve::SetTrim1(const Handle(StepGeom_HArray1OfTrimmingSelect)& aTrim1)
{
	trim1 = aTrim1;
}

Handle(StepGeom_HArray1OfTrimmingSelect) StepGeom_TrimmedCurve::Trim1() const
{
	return trim1;
}

StepGeom_TrimmingSelect StepGeom_TrimmedCurve::Trim1Value(const Standard_Integer num) const
{
	return trim1->Value(num);
}

Standard_Integer StepGeom_TrimmedCurve::NbTrim1 () const
{
	return trim1->Length();
}

void StepGeom_TrimmedCurve::SetTrim2(const Handle(StepGeom_HArray1OfTrimmingSelect)& aTrim2)
{
	trim2 = aTrim2;
}

Handle(StepGeom_HArray1OfTrimmingSelect) StepGeom_TrimmedCurve::Trim2() const
{
	return trim2;
}

StepGeom_TrimmingSelect StepGeom_TrimmedCurve::Trim2Value(const Standard_Integer num) const
{
	return trim2->Value(num);
}

Standard_Integer StepGeom_TrimmedCurve::NbTrim2 () const
{
	return trim2->Length();
}

void StepGeom_TrimmedCurve::SetSenseAgreement(const Standard_Boolean aSenseAgreement)
{
	senseAgreement = aSenseAgreement;
}

Standard_Boolean StepGeom_TrimmedCurve::SenseAgreement() const
{
	return senseAgreement;
}

void StepGeom_TrimmedCurve::SetMasterRepresentation(const StepGeom_TrimmingPreference aMasterRepresentation)
{
	masterRepresentation = aMasterRepresentation;
}

StepGeom_TrimmingPreference StepGeom_TrimmedCurve::MasterRepresentation() const
{
	return masterRepresentation;
}
