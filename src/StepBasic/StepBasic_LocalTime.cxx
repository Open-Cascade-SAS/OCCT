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

#include <StepBasic_LocalTime.ixx>


StepBasic_LocalTime::StepBasic_LocalTime ()  {}

void StepBasic_LocalTime::Init(
	const Standard_Integer aHourComponent,
	const Standard_Boolean hasAminuteComponent,
	const Standard_Integer aMinuteComponent,
	const Standard_Boolean hasAsecondComponent,
	const Standard_Real aSecondComponent,
	const Handle(StepBasic_CoordinatedUniversalTimeOffset)& aZone)
{
	// --- classe own fields ---
	hourComponent = aHourComponent;
	hasMinuteComponent = hasAminuteComponent;
	minuteComponent = aMinuteComponent;
	hasSecondComponent = hasAsecondComponent;
	secondComponent = aSecondComponent;
	zone = aZone;
}


void StepBasic_LocalTime::SetHourComponent(const Standard_Integer aHourComponent)
{
	hourComponent = aHourComponent;
}

Standard_Integer StepBasic_LocalTime::HourComponent() const
{
	return hourComponent;
}

void StepBasic_LocalTime::SetMinuteComponent(const Standard_Integer aMinuteComponent)
{
	minuteComponent = aMinuteComponent;
	hasMinuteComponent = Standard_True;
}

void StepBasic_LocalTime::UnSetMinuteComponent()
{
	hasMinuteComponent = Standard_False;
}

Standard_Integer StepBasic_LocalTime::MinuteComponent() const
{
	return minuteComponent;
}

Standard_Boolean StepBasic_LocalTime::HasMinuteComponent() const
{
	return hasMinuteComponent;
}

void StepBasic_LocalTime::SetSecondComponent(const Standard_Real aSecondComponent)
{
	secondComponent = aSecondComponent;
	hasSecondComponent = Standard_True;
}

void StepBasic_LocalTime::UnSetSecondComponent()
{
	hasSecondComponent = Standard_False;
}

Standard_Real StepBasic_LocalTime::SecondComponent() const
{
	return secondComponent;
}

Standard_Boolean StepBasic_LocalTime::HasSecondComponent() const
{
	return hasSecondComponent;
}

void StepBasic_LocalTime::SetZone(const Handle(StepBasic_CoordinatedUniversalTimeOffset)& aZone)
{
	zone = aZone;
}

Handle(StepBasic_CoordinatedUniversalTimeOffset) StepBasic_LocalTime::Zone() const
{
	return zone;
}
