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


#include <Interface_Macros.hxx>
#include <Standard_Transient.hxx>
#include <StepBasic_Date.hxx>
#include <StepBasic_DateAndTime.hxx>
#include <StepBasic_DateTimeSelect.hxx>
#include <StepBasic_LocalTime.hxx>

StepBasic_DateTimeSelect::StepBasic_DateTimeSelect () {  }

Standard_Integer StepBasic_DateTimeSelect::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepBasic_Date))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepBasic_LocalTime))) return 2;
	if (ent->IsKind(STANDARD_TYPE(StepBasic_DateAndTime))) return 3;
	return 0;
}

Handle(StepBasic_Date) StepBasic_DateTimeSelect::Date () const
{
	return GetCasted(StepBasic_Date,Value());
}

Handle(StepBasic_LocalTime) StepBasic_DateTimeSelect::LocalTime () const
{
	return GetCasted(StepBasic_LocalTime,Value());
}

Handle(StepBasic_DateAndTime) StepBasic_DateTimeSelect::DateAndTime () const
{
	return GetCasted(StepBasic_DateAndTime,Value());
}
