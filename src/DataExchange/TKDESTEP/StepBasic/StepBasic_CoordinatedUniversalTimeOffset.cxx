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

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_CoordinatedUniversalTimeOffset, Standard_Transient)

StepBasic_CoordinatedUniversalTimeOffset::StepBasic_CoordinatedUniversalTimeOffset() {}

void StepBasic_CoordinatedUniversalTimeOffset::Init(const int        aHourOffset,
                                                    const bool        hasAminuteOffset,
                                                    const int        aMinuteOffset,
                                                    const StepBasic_AheadOrBehind aSense)
{
  // --- classe own fields ---
  hourOffset      = aHourOffset;
  hasMinuteOffset = hasAminuteOffset;
  minuteOffset    = aMinuteOffset;
  sense           = aSense;
}

void StepBasic_CoordinatedUniversalTimeOffset::SetHourOffset(const int aHourOffset)
{
  hourOffset = aHourOffset;
}

int StepBasic_CoordinatedUniversalTimeOffset::HourOffset() const
{
  return hourOffset;
}

void StepBasic_CoordinatedUniversalTimeOffset::SetMinuteOffset(const int aMinuteOffset)
{
  minuteOffset    = aMinuteOffset;
  hasMinuteOffset = true;
}

void StepBasic_CoordinatedUniversalTimeOffset::UnSetMinuteOffset()
{
  hasMinuteOffset = false;
}

int StepBasic_CoordinatedUniversalTimeOffset::MinuteOffset() const
{
  return minuteOffset;
}

bool StepBasic_CoordinatedUniversalTimeOffset::HasMinuteOffset() const
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
