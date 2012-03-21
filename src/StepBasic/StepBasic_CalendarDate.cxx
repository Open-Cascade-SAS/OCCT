// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <StepBasic_CalendarDate.ixx>


StepBasic_CalendarDate::StepBasic_CalendarDate ()  {}

void StepBasic_CalendarDate::Init(
	const Standard_Integer aYearComponent)
{

	StepBasic_Date::Init(aYearComponent);
}

void StepBasic_CalendarDate::Init(
	const Standard_Integer aYearComponent,
	const Standard_Integer aDayComponent,
	const Standard_Integer aMonthComponent)
{
	// --- classe own fields ---
	dayComponent = aDayComponent;
	monthComponent = aMonthComponent;
	// --- classe inherited fields ---
	StepBasic_Date::Init(aYearComponent);
}


void StepBasic_CalendarDate::SetDayComponent(const Standard_Integer aDayComponent)
{
	dayComponent = aDayComponent;
}

Standard_Integer StepBasic_CalendarDate::DayComponent() const
{
	return dayComponent;
}

void StepBasic_CalendarDate::SetMonthComponent(const Standard_Integer aMonthComponent)
{
	monthComponent = aMonthComponent;
}

Standard_Integer StepBasic_CalendarDate::MonthComponent() const
{
	return monthComponent;
}
