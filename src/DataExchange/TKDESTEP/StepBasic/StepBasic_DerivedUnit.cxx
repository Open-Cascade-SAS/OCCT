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
#include <StepBasic_DerivedUnit.hxx>
#include <StepBasic_DerivedUnitElement.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_DerivedUnit, Standard_Transient)

StepBasic_DerivedUnit::StepBasic_DerivedUnit() = default;

void StepBasic_DerivedUnit::Init(
  const occ::handle<NCollection_HArray1<occ::handle<StepBasic_DerivedUnitElement>>>& elements)
{
  theElements = elements;
}

void StepBasic_DerivedUnit::SetElements(
  const occ::handle<NCollection_HArray1<occ::handle<StepBasic_DerivedUnitElement>>>& elements)
{
  theElements = elements;
}

occ::handle<NCollection_HArray1<occ::handle<StepBasic_DerivedUnitElement>>> StepBasic_DerivedUnit::
  Elements() const
{
  return theElements;
}

int StepBasic_DerivedUnit::NbElements() const
{
  return theElements->Length();
}

occ::handle<StepBasic_DerivedUnitElement> StepBasic_DerivedUnit::ElementsValue(const int num) const
{
  return theElements->Value(num);
}
