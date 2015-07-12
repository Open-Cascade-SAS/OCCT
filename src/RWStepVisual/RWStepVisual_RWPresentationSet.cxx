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


#include <Interface_Check.hxx>
#include <RWStepVisual_RWPresentationSet.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_PresentationSet.hxx>

RWStepVisual_RWPresentationSet::RWStepVisual_RWPresentationSet () {}

void RWStepVisual_RWPresentationSet::ReadStep
(const Handle(StepData_StepReaderData)& data,
 const Standard_Integer num,
 Handle(Interface_Check)& ach,
 const Handle(StepVisual_PresentationSet)& /*ent*/) const
{
  
  
  // --- Number of Parameter Control ---
  
  if (!data->CheckNbParams(num,0,ach,"presentation_set")) return;
  
  //--- Initialisation of the read entity ---
  
}
void RWStepVisual_RWPresentationSet::WriteStep
(StepData_StepWriter& /*SW*/,
 const Handle(StepVisual_PresentationSet)& /*ent*/) const
{
}

