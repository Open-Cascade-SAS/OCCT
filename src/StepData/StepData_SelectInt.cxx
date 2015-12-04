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
#include <StepData_SelectInt.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepData_SelectInt,StepData_SelectMember)

StepData_SelectInt::StepData_SelectInt  ()    {  thekind = 0;  theval = 0;  }

Standard_Integer  StepData_SelectInt::Kind () const    {  return thekind;  }
void  StepData_SelectInt::SetKind (const Standard_Integer kind)    {  thekind = kind;  }

Standard_Integer  StepData_SelectInt::Int () const    {  return theval;  }
void  StepData_SelectInt::SetInt (const Standard_Integer val)    {  theval = val;  }
