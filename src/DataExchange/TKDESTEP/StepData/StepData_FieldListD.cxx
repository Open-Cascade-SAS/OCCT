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

#include <Standard_OutOfRange.hxx>
#include <StepData_FieldListD.hxx>

StepData_FieldListD::StepData_FieldListD(const int nb)
{
  if (nb > 0)
    thefields = new NCollection_HArray1<StepData_Field>(1, nb);
}

void StepData_FieldListD::SetNb(const int nb)
{
  thefields.Nullify();
  if (nb > 0)
    thefields = new NCollection_HArray1<StepData_Field>(1, nb);
}

int StepData_FieldListD::NbFields() const
{
  return (thefields.IsNull() ? 0 : thefields->Length());
}

const StepData_Field& StepData_FieldListD::Field(const int num) const
{
  if (thefields.IsNull())
    throw Standard_OutOfRange("StepData_FieldListD::Field");
  return thefields->Value(num);
}

StepData_Field& StepData_FieldListD::CField(const int num)
{
  if (thefields.IsNull())
    throw Standard_OutOfRange("StepData_FieldListD::Field");
  return thefields->ChangeValue(num);
}
