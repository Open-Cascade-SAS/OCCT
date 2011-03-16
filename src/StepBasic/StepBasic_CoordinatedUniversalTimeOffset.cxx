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
