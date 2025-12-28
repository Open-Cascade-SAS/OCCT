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

#include "RWStepBasic_RWWeekOfYearAndDayDate.pxx"
#include <StepBasic_WeekOfYearAndDayDate.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

RWStepBasic_RWWeekOfYearAndDayDate::RWStepBasic_RWWeekOfYearAndDayDate() {}

void RWStepBasic_RWWeekOfYearAndDayDate::ReadStep(
  const occ::handle<StepData_StepReaderData>&        data,
  const int                                          num,
  occ::handle<Interface_Check>&                      ach,
  const occ::handle<StepBasic_WeekOfYearAndDayDate>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 3, ach, "week_of_year_and_day_date"))
    return;

  // --- inherited field : yearComponent ---

  int aYearComponent;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadInteger(num, 1, "year_component", ach, aYearComponent);

  // --- own field : weekComponent ---

  int aWeekComponent;
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->ReadInteger(num, 2, "week_component", ach, aWeekComponent);

  // --- own field : dayComponent ---

  int  aDayComponent;
  bool hasAdayComponent = true;
  if (data->IsParamDefined(num, 3))
  {
    // szv#4:S4163:12Mar99 `bool stat3 =` not needed
    data->ReadInteger(num, 3, "day_component", ach, aDayComponent);
  }
  else
  {
    hasAdayComponent = false;
    aDayComponent    = 0;
  }

  //--- Initialisation of the read entity ---

  ent->Init(aYearComponent, aWeekComponent, hasAdayComponent, aDayComponent);
}

void RWStepBasic_RWWeekOfYearAndDayDate::WriteStep(
  StepData_StepWriter&                               SW,
  const occ::handle<StepBasic_WeekOfYearAndDayDate>& ent) const
{

  // --- inherited field yearComponent ---

  SW.Send(ent->YearComponent());

  // --- own field : weekComponent ---

  SW.Send(ent->WeekComponent());

  // --- own field : dayComponent ---

  bool hasAdayComponent = ent->HasDayComponent();
  if (hasAdayComponent)
  {
    SW.Send(ent->DayComponent());
  }
  else
  {
    SW.SendUndef();
  }
}
