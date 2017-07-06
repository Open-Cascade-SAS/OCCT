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
#include <StepBasic_CoordinatedUniversalTimeOffset.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_CoordinatedUniversalTimeOffset,Standard_Transient)

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
