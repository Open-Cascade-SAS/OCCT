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

#include <StepBasic_CoordinatedUniversalTimeOffset.ixx>


StepBasic_CoordinatedUniversalTimeOffset::StepBasic_CoordinatedUniversalTimeOffset ()  {}

void StepBasic_CoordinatedUniversalTimeOffset::Init(
	const Standard_Integer aHourOffset,
	const Standard_Boolean hasAminuteOffset,
	const Standard_Integer aMinuteOffset,
	const StepBasic_AheadOrBehind aSense)
{
	// --- classe own fields ---
	hourOffset = aHourOffset;
	hasMinuteOffset = hasAminuteOffset;
	minuteOffset = aMinuteOffset;
	sense = aSense;
}


void StepBasic_CoordinatedUniversalTimeOffset::SetHourOffset(const Standard_Integer aHourOffset)
{
	hourOffset = aHourOffset;
}

Standard_Integer StepBasic_CoordinatedUniversalTimeOffset::HourOffset() const
{
	return hourOffset;
}

void StepBasic_CoordinatedUniversalTimeOffset::SetMinuteOffset(const Standard_Integer aMinuteOffset)
{
	minuteOffset = aMinuteOffset;
	hasMinuteOffset = Standard_True;
}

void StepBasic_CoordinatedUniversalTimeOffset::UnSetMinuteOffset()
{
	hasMinuteOffset = Standard_False;
}

Standard_Integer StepBasic_CoordinatedUniversalTimeOffset::MinuteOffset() const
{
	return minuteOffset;
}

Standard_Boolean StepBasic_CoordinatedUniversalTimeOffset::HasMinuteOffset() const
{
	return hasMinuteOffset;
}

void StepBasic_CoordinatedUniversalTimeOffset::SetSense(const StepBasic_AheadOrBehind aSense)
{
	sense = aSense;
}

StepBasic_AheadOrBehind StepBasic_CoordinatedUniversalTimeOffset::Sense() const
{
	return sense;
}
