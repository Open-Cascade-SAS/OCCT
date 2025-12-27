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
#include <StepData_SelectReal.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepData_SelectReal, StepData_SelectMember)

//  Definitions : cf Field
#define KindReal 5

StepData_SelectReal::StepData_SelectReal()
{
  theval = 0.0;
}

int StepData_SelectReal::Kind() const
{
  return KindReal;
}

double StepData_SelectReal::Real() const
{
  return theval;
}

void StepData_SelectReal::SetReal(const double val)
{
  theval = val;
}
