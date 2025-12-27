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

#include "RWStepBasic_RWCoordinatedUniversalTimeOffset.pxx"
#include <StepBasic_CoordinatedUniversalTimeOffset.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <TCollection_AsciiString.hxx>

#include "RWStepBasic_RWAheadOrBehind.pxx"

RWStepBasic_RWCoordinatedUniversalTimeOffset::RWStepBasic_RWCoordinatedUniversalTimeOffset() {}

void RWStepBasic_RWCoordinatedUniversalTimeOffset::ReadStep(
  const occ::handle<StepData_StepReaderData>&                  data,
  const int                                  num,
  occ::handle<Interface_Check>&                                ach,
  const occ::handle<StepBasic_CoordinatedUniversalTimeOffset>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 3, ach, "coordinated_universal_time_offset"))
    return;

  // --- own field : hourOffset ---

  int aHourOffset;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadInteger(num, 1, "hour_offset", ach, aHourOffset);

  // --- own field : minuteOffset ---

  int aMinuteOffset;
  bool hasAminuteOffset = true;
  if (data->IsParamDefined(num, 2))
  {
    // szv#4:S4163:12Mar99 `bool stat2 =` not needed
    data->ReadInteger(num, 2, "minute_offset", ach, aMinuteOffset);
  }
  else
  {
    hasAminuteOffset = false;
    aMinuteOffset    = 0;
  }

  // --- own field : sense ---

  StepBasic_AheadOrBehind aSense = StepBasic_aobAhead;
  if (data->ParamType(num, 3) == Interface_ParamEnum)
  {
    const char* text = data->ParamCValue(num, 3);
    if (!RWStepBasic_RWAheadOrBehind::ConvertToEnum(text, aSense))
    {
      ach->AddFail("Enumeration ahead_or_behind has not an allowed value");
    }
  }
  else
    ach->AddFail("Parameter #3 (sense) is not an enumeration");

  //--- Initialisation of the read entity ---

  ent->Init(aHourOffset, hasAminuteOffset, aMinuteOffset, aSense);
}

void RWStepBasic_RWCoordinatedUniversalTimeOffset::WriteStep(
  StepData_StepWriter&                                    SW,
  const occ::handle<StepBasic_CoordinatedUniversalTimeOffset>& ent) const
{

  // --- own field : hourOffset ---

  SW.Send(ent->HourOffset());

  // --- own field : minuteOffset ---

  bool hasAminuteOffset = ent->HasMinuteOffset();
  if (hasAminuteOffset)
  {
    SW.Send(ent->MinuteOffset());
  }
  else
  {
    SW.SendUndef();
  }

  // --- own field : sense ---

  SW.SendEnum(RWStepBasic_RWAheadOrBehind::ConvertToString(ent->Sense()));
}
