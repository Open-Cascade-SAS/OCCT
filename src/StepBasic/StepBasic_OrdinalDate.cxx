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
#include <StepBasic_OrdinalDate.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_OrdinalDate,StepBasic_Date)

StepBasic_OrdinalDate::StepBasic_OrdinalDate ()  {}

void StepBasic_OrdinalDate::Init(
	const Standard_Integer aYearComponent,
	const Standard_Integer aDayComponent)
{
	// --- classe own fields ---
	dayComponent = aDayComponent;
	// --- classe inherited fields ---
	StepBasic_Date::Init(aYearComponent);
}


void StepBasic_OrdinalDate::SetDayComponent(const Standard_Integer aDayComponent)
{
	dayComponent = aDayComponent;
}

Standard_Integer StepBasic_OrdinalDate::DayComponent() const
{
	return dayComponent;
}
