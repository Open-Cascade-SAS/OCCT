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
#include <StepBasic_LocalTime.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_LocalTime, Standard_Transient)

StepBasic_LocalTime::StepBasic_LocalTime() {}

void StepBasic_LocalTime::Init(const int    aHourComponent,
                               const bool   hasAminuteComponent,
                               const int    aMinuteComponent,
                               const bool   hasAsecondComponent,
                               const double aSecondComponent,
                               const occ::handle<StepBasic_CoordinatedUniversalTimeOffset>& aZone)
{
  // --- classe own fields ---
  hourComponent      = aHourComponent;
  hasMinuteComponent = hasAminuteComponent;
  minuteComponent    = aMinuteComponent;
  hasSecondComponent = hasAsecondComponent;
  secondComponent    = aSecondComponent;
  zone               = aZone;
}

void StepBasic_LocalTime::SetHourComponent(const int aHourComponent)
{
  hourComponent = aHourComponent;
}

int StepBasic_LocalTime::HourComponent() const
{
  return hourComponent;
}

void StepBasic_LocalTime::SetMinuteComponent(const int aMinuteComponent)
{
  minuteComponent    = aMinuteComponent;
  hasMinuteComponent = true;
}

void StepBasic_LocalTime::UnSetMinuteComponent()
{
  hasMinuteComponent = false;
}

int StepBasic_LocalTime::MinuteComponent() const
{
  return minuteComponent;
}

bool StepBasic_LocalTime::HasMinuteComponent() const
{
  return hasMinuteComponent;
}

void StepBasic_LocalTime::SetSecondComponent(const double aSecondComponent)
{
  secondComponent    = aSecondComponent;
  hasSecondComponent = true;
}

void StepBasic_LocalTime::UnSetSecondComponent()
{
  hasSecondComponent = false;
}

double StepBasic_LocalTime::SecondComponent() const
{
  return secondComponent;
}

bool StepBasic_LocalTime::HasSecondComponent() const
{
  return hasSecondComponent;
}

void StepBasic_LocalTime::SetZone(
  const occ::handle<StepBasic_CoordinatedUniversalTimeOffset>& aZone)
{
  zone = aZone;
}

occ::handle<StepBasic_CoordinatedUniversalTimeOffset> StepBasic_LocalTime::Zone() const
{
  return zone;
}
