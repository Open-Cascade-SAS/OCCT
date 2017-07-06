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


#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_InterfaceMismatch.hxx>
#include <Standard_Type.hxx>
#include <StepData_Described.hxx>
#include <StepData_EDescr.hxx>
#include <StepData_Field.hxx>
#include <StepData_Simple.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepData_Described,Standard_Transient)

StepData_Described::StepData_Described  (const Handle(StepData_EDescr)& descr)
: thedescr (descr)    {  }

Handle(StepData_EDescr)  StepData_Described::Description () const
{  return thedescr;  }
