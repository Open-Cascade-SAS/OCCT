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
#include <StepBasic_Date.hxx>
#include <StepBasic_DateAndTime.hxx>
#include <StepBasic_LocalTime.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_DateAndTime,Standard_Transient)

StepBasic_DateAndTime::StepBasic_DateAndTime ()  {}

void StepBasic_DateAndTime::Init(
	const Handle(StepBasic_Date)& aDateComponent,
	const Handle(StepBasic_LocalTime)& aTimeComponent)
{
	// --- classe own fields ---
	dateComponent = aDateComponent;
	timeComponent = aTimeComponent;
}


void StepBasic_DateAndTime::SetDateComponent(const Handle(StepBasic_Date)& aDateComponent)
{
	dateComponent = aDateComponent;
}

Handle(StepBasic_Date) StepBasic_DateAndTime::DateComponent() const
{
	return dateComponent;
}

void StepBasic_DateAndTime::SetTimeComponent(const Handle(StepBasic_LocalTime)& aTimeComponent)
{
	timeComponent = aTimeComponent;
}

Handle(StepBasic_LocalTime) StepBasic_DateAndTime::TimeComponent() const
{
	return timeComponent;
}
