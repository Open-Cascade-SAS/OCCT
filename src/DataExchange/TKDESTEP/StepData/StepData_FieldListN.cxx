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

#include <StepData_FieldListN.hxx>
#include <Standard_RangeError.hxx>

StepData_FieldListN::StepData_FieldListN(const int nb)
{
  if (nb < 0)
  {
    throw Standard_RangeError("StepData_FieldListN: negative field count");
  }
  if (nb > 0)
  {
    thefields.Resize(1, nb, false);
  }
}

int StepData_FieldListN::NbFields() const
{
  return thefields.Length();
}

const StepData_Field& StepData_FieldListN::Field(const int num) const
{
  return thefields.Value(num);
}

StepData_Field& StepData_FieldListN::CField(const int num)
{
  return thefields.ChangeValue(num);
}
