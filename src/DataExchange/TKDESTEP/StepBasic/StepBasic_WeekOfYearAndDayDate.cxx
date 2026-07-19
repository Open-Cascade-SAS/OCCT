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
#include <StepBasic_WeekOfYearAndDayDate.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_WeekOfYearAndDayDate, StepBasic_Date)

StepBasic_WeekOfYearAndDayDate::StepBasic_WeekOfYearAndDayDate() = default;

void StepBasic_WeekOfYearAndDayDate::Init(const int  aYearComponent,
                                          const int  aWeekComponent,
                                          const bool hasAdayComponent,
                                          const int  aDayComponent)
{
  // --- classe own fields ---
  weekComponent   = aWeekComponent;
  hasDayComponent = hasAdayComponent;
  dayComponent    = aDayComponent;
  // --- classe inherited fields ---
  StepBasic_Date::Init(aYearComponent);
}

void StepBasic_WeekOfYearAndDayDate::SetWeekComponent(const int aWeekComponent)
{
  weekComponent = aWeekComponent;
}

int StepBasic_WeekOfYearAndDayDate::WeekComponent() const
{
  return weekComponent;
}

void StepBasic_WeekOfYearAndDayDate::SetDayComponent(const int aDayComponent)
{
  dayComponent    = aDayComponent;
  hasDayComponent = true;
}

void StepBasic_WeekOfYearAndDayDate::UnSetDayComponent()
{
  hasDayComponent = false;
}

int StepBasic_WeekOfYearAndDayDate::DayComponent() const
{
  return dayComponent;
}

bool StepBasic_WeekOfYearAndDayDate::HasDayComponent() const
{
  return hasDayComponent;
}
