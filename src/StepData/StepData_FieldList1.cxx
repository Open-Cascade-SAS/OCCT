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
#include <StepData_Field.hxx>
#include <StepData_FieldList1.hxx>

StepData_FieldList1::StepData_FieldList1  ()    {  }

Standard_Integer  StepData_FieldList1::NbFields () const
{  return 1;  }

const StepData_Field&  StepData_FieldList1::Field (const Standard_Integer num) const
{
  if (num != 1) throw Standard_OutOfRange("StepData_FieldList1 : Field");
  return thefield;
}

StepData_Field&  StepData_FieldList1::CField (const Standard_Integer num)
{
  if (num != 1) throw Standard_OutOfRange("StepData_FieldList1 : CField");
  return thefield;
}
