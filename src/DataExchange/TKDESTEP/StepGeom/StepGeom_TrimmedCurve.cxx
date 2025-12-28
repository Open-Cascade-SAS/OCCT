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

#include <StepGeom_TrimmedCurve.hxx>
#include <StepGeom_TrimmingSelect.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_TrimmedCurve, StepGeom_BoundedCurve)

StepGeom_TrimmedCurve::StepGeom_TrimmedCurve() {}

void StepGeom_TrimmedCurve::Init(
  const occ::handle<TCollection_HAsciiString>&                     aName,
  const occ::handle<StepGeom_Curve>&                               aBasisCurve,
  const occ::handle<NCollection_HArray1<StepGeom_TrimmingSelect>>& aTrim1,
  const occ::handle<NCollection_HArray1<StepGeom_TrimmingSelect>>& aTrim2,
  const bool                                                       aSenseAgreement,
  const StepGeom_TrimmingPreference                                aMasterRepresentation)
{
  // --- classe own fields ---
  basisCurve           = aBasisCurve;
  trim1                = aTrim1;
  trim2                = aTrim2;
  senseAgreement       = aSenseAgreement;
  masterRepresentation = aMasterRepresentation;
  // --- classe inherited fields ---
  StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_TrimmedCurve::SetBasisCurve(const occ::handle<StepGeom_Curve>& aBasisCurve)
{
  basisCurve = aBasisCurve;
}

occ::handle<StepGeom_Curve> StepGeom_TrimmedCurve::BasisCurve() const
{
  return basisCurve;
}

void StepGeom_TrimmedCurve::SetTrim1(
  const occ::handle<NCollection_HArray1<StepGeom_TrimmingSelect>>& aTrim1)
{
  trim1 = aTrim1;
}

occ::handle<NCollection_HArray1<StepGeom_TrimmingSelect>> StepGeom_TrimmedCurve::Trim1() const
{
  return trim1;
}

StepGeom_TrimmingSelect StepGeom_TrimmedCurve::Trim1Value(const int num) const
{
  return trim1->Value(num);
}

int StepGeom_TrimmedCurve::NbTrim1() const
{
  return trim1->Length();
}

void StepGeom_TrimmedCurve::SetTrim2(
  const occ::handle<NCollection_HArray1<StepGeom_TrimmingSelect>>& aTrim2)
{
  trim2 = aTrim2;
}

occ::handle<NCollection_HArray1<StepGeom_TrimmingSelect>> StepGeom_TrimmedCurve::Trim2() const
{
  return trim2;
}

StepGeom_TrimmingSelect StepGeom_TrimmedCurve::Trim2Value(const int num) const
{
  return trim2->Value(num);
}

int StepGeom_TrimmedCurve::NbTrim2() const
{
  return trim2->Length();
}

void StepGeom_TrimmedCurve::SetSenseAgreement(const bool aSenseAgreement)
{
  senseAgreement = aSenseAgreement;
}

bool StepGeom_TrimmedCurve::SenseAgreement() const
{
  return senseAgreement;
}

void StepGeom_TrimmedCurve::SetMasterRepresentation(
  const StepGeom_TrimmingPreference aMasterRepresentation)
{
  masterRepresentation = aMasterRepresentation;
}

StepGeom_TrimmingPreference StepGeom_TrimmedCurve::MasterRepresentation() const
{
  return masterRepresentation;
}
