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
#include <StepBasic_MeasureValueMember.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepBasic_Unit.hxx>
#include <StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem.hxx>
#include <StepShape_ValueQualifier.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem,
                           StepRepr_RepresentationItem)

StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem::
  StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem()
{
  myMeasure = new StepBasic_MeasureWithUnit;
}

void StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem::Init(
  const occ::handle<TCollection_HAsciiString>&          aName,
  const occ::handle<StepBasic_MeasureValueMember>&      aValueComponent,
  const StepBasic_Unit&                            aUnitComponent,
  const occ::handle<NCollection_HArray1<StepShape_ValueQualifier>>& qualifiers)
{
  StepRepr_RepresentationItem::Init(aName);
  myMeasure->Init(aValueComponent, aUnitComponent);
  theQualifiers = qualifiers;
}

void StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem::SetMeasure(
  const occ::handle<StepBasic_MeasureWithUnit>& Measure)
{
  myMeasure = Measure;
}

occ::handle<StepBasic_MeasureWithUnit>
  StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem::Measure() const
{
  return myMeasure;
}

occ::handle<NCollection_HArray1<StepShape_ValueQualifier>>
  StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem::Qualifiers() const
{
  return theQualifiers;
}

int StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem::NbQualifiers()
  const
{
  return theQualifiers->Length();
}

void StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem::SetQualifiers(
  const occ::handle<NCollection_HArray1<StepShape_ValueQualifier>>& qualifiers)
{
  theQualifiers = qualifiers;
}

StepShape_ValueQualifier StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem::
  QualifiersValue(const int num) const
{
  return theQualifiers->Value(num);
}

void StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem::SetQualifiersValue(
  const int          num,
  const StepShape_ValueQualifier& aqualifier)
{
  theQualifiers->SetValue(num, aqualifier);
}
