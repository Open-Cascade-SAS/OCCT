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

#include <StepBasic_WeekOfYearAndDayDate.ixx>


StepBasic_WeekOfYearAndDayDate::StepBasic_WeekOfYearAndDayDate ()  {}

void StepBasic_WeekOfYearAndDayDate::Init(
	const Standard_Integer aYearComponent)
{

	StepBasic_Date::Init(aYearComponent);
}

void StepBasic_WeekOfYearAndDayDate::Init(
	const Standard_Integer aYearComponent,
	const Standard_Integer aWeekComponent,
	const Standard_Boolean hasAdayComponent,
	const Standard_Integer aDayComponent)
{
	// --- classe own fields ---
	weekComponent = aWeekComponent;
	hasDayComponent = hasAdayComponent;
	dayComponent = aDayComponent;
	// --- classe inherited fields ---
	StepBasic_Date::Init(aYearComponent);
}


void StepBasic_WeekOfYearAndDayDate::SetWeekComponent(const Standard_Integer aWeekComponent)
{
	weekComponent = aWeekComponent;
}

Standard_Integer StepBasic_WeekOfYearAndDayDate::WeekComponent() const
{
	return weekComponent;
}

void StepBasic_WeekOfYearAndDayDate::SetDayComponent(const Standard_Integer aDayComponent)
{
	dayComponent = aDayComponent;
	hasDayComponent = Standard_True;
}

void StepBasic_WeekOfYearAndDayDate::UnSetDayComponent()
{
	hasDayComponent = Standard_False;
}

Standard_Integer StepBasic_WeekOfYearAndDayDate::DayComponent() const
{
	return dayComponent;
}

Standard_Boolean StepBasic_WeekOfYearAndDayDate::HasDayComponent() const
{
	return hasDayComponent;
}
