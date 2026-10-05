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

#include <Interface_EntityIterator.hxx>
#include <Standard_OutOfRange.hxx>
#include <StepData_Field.hxx>
#include <StepData_FieldList.hxx>

static StepData_Field nulfild;

StepData_FieldList::~StepData_FieldList() = default;

StepData_FieldList::StepData_FieldList() = default;

int StepData_FieldList::NbFields() const
{
  return 0;
}

const StepData_Field& StepData_FieldList::Field(const int) const
{
  throw Standard_OutOfRange("StepData_FieldList : Field");
}

StepData_Field& StepData_FieldList::CField(const int)
{
  throw Standard_OutOfRange("StepData_FieldList : CField");
}

void StepData_FieldList::FillShared(Interface_EntityIterator& theIterator) const
{
  for (int aFieldIndex = 1; aFieldIndex <= NbFields(); ++aFieldIndex)
  {
    const StepData_Field& aField = Field(aFieldIndex);
    if (aField.Kind() != StepData_Field::FieldKind::Entity)
    {
      continue;
    }

    const int anArity  = aField.Arity();
    int       aLower1  = 1;
    int       aLength1 = 1;
    int       aLower2  = 1;
    int       aLength2 = 1;
    if (anArity == 1)
    {
      aLower1  = aField.Lower(1);
      aLength1 = aField.Length(1);
    }
    else if (anArity == 2)
    {
      aLower1  = aField.Lower(1);
      aLength1 = aField.Length(1);
      aLower2  = aField.Lower(2);
      aLength2 = aField.Length(2);
    }

    for (int anOffset1 = 0; anOffset1 < aLength1; ++anOffset1)
    {
      for (int anOffset2 = 0; anOffset2 < aLength2; ++anOffset2)
      {
        theIterator.AddItem(aField.Entity(aLower1 + anOffset1, aLower2 + anOffset2));
      }
    }
  }
}
