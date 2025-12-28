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

#include <StepShape_QualifiedRepresentationItem.hxx>
#include <StepShape_ValueQualifier.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_QualifiedRepresentationItem, StepRepr_RepresentationItem)

StepShape_QualifiedRepresentationItem::StepShape_QualifiedRepresentationItem() {}

void StepShape_QualifiedRepresentationItem::Init(
  const occ::handle<TCollection_HAsciiString>&          aName,
  const occ::handle<NCollection_HArray1<StepShape_ValueQualifier>>& qualifiers)
{
  StepRepr_RepresentationItem::Init(aName);
  theQualifiers = qualifiers;
}

occ::handle<NCollection_HArray1<StepShape_ValueQualifier>> StepShape_QualifiedRepresentationItem::Qualifiers() const
{
  return theQualifiers;
}

int StepShape_QualifiedRepresentationItem::NbQualifiers() const
{
  return theQualifiers->Length();
}

void StepShape_QualifiedRepresentationItem::SetQualifiers(
  const occ::handle<NCollection_HArray1<StepShape_ValueQualifier>>& qualifiers)
{
  theQualifiers = qualifiers;
}

StepShape_ValueQualifier StepShape_QualifiedRepresentationItem::QualifiersValue(
  const int num) const
{
  return theQualifiers->Value(num);
}

void StepShape_QualifiedRepresentationItem::SetQualifiersValue(
  const int          num,
  const StepShape_ValueQualifier& aqualifier)
{
  theQualifiers->SetValue(num, aqualifier);
}
