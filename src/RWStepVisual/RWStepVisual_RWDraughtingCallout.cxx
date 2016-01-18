// Created on: 2015-10-29
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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
#include <Interface_EntityIterator.hxx>
#include <RWStepVisual_RWDraughtingCallout.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_DraughtingCallout.hxx>

//=======================================================================
//function : RWStepVisual_RWDraughtingCallout
//purpose  : 
//=======================================================================
RWStepVisual_RWDraughtingCallout::RWStepVisual_RWDraughtingCallout () {}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================
void RWStepVisual_RWDraughtingCallout::ReadStep(const Handle(StepData_StepReaderData)& data,
                                                const Standard_Integer num,
                                                Handle(Interface_Check)& ach,
                                                const Handle(StepVisual_DraughtingCallout)& ent) const
{
  if (!data->CheckNbParams(num, 2, ach, "draughting_callout")) return;

  // Inherited field : name
  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 1, "name" ,ach, aName);
    
  // Own field: contents
  Handle(StepVisual_HArray1OfDraughtingCalloutElement) aContents;
  StepVisual_DraughtingCalloutElement anEnt;
  Standard_Integer nbSub;
  if (data->ReadSubList (num, 2, "contents", ach, nbSub)) {
    Standard_Integer nbElements = data->NbParams(nbSub);
    aContents = new StepVisual_HArray1OfDraughtingCalloutElement (1, nbElements);
    for (Standard_Integer i = 1; i <= nbElements; i++) {
      if (data->ReadEntity(nbSub, i,"content", ach, anEnt))
        aContents->SetValue(i, anEnt);
    }
  }

  // Initialisation of the read entity
  ent->Init(aName, aContents);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================
void RWStepVisual_RWDraughtingCallout::WriteStep
  (StepData_StepWriter& SW,
   const Handle(StepVisual_DraughtingCallout)& ent) const
{
  // Inherited field: name
  SW.Send(ent->Name());
    
  // Own field: contents
  SW.OpenSub();
  for (Standard_Integer i = 1;  i <= ent->NbContents();  i++) {
    SW.Send(ent->ContentsValue(i).Value());
  }
  SW.CloseSub();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepVisual_RWDraughtingCallout::Share (const Handle(StepVisual_DraughtingCallout) &ent,
                                              Interface_EntityIterator& iter) const
{
  // Own field: contents
  Standard_Integer i, nb = ent->NbContents();
  for (i = 1; i <= nb; i++)  
    iter.AddItem (ent->ContentsValue(i).Value());
}

