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

#include "RWStepBasic_RWDate.pxx"
#include <StepBasic_Date.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

RWStepBasic_RWDate::RWStepBasic_RWDate() {}

void RWStepBasic_RWDate::ReadStep(const Handle(StepData_StepReaderData)& data,
                                  const Standard_Integer                 num,
                                  Handle(Interface_Check)&               ach,
                                  const Handle(StepBasic_Date)&          ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 1, ach, "date"))
    return;

  // --- own field : yearComponent ---

  Standard_Integer aYearComponent;
  // szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
  data->ReadInteger(num, 1, "year_component", ach, aYearComponent);

  //--- Initialisation of the read entity ---

  ent->Init(aYearComponent);
}

void RWStepBasic_RWDate::WriteStep(StepData_StepWriter& SW, const Handle(StepBasic_Date)& ent) const
{

  // --- own field : yearComponent ---

  SW.Send(ent->YearComponent());
}
