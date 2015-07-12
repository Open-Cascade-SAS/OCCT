// Created on: 2000-05-10
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepAP214_RWAppliedGroupAssignment.hxx>
#include <StepAP214_AppliedGroupAssignment.hxx>
#include <StepAP214_GroupItem.hxx>
#include <StepAP214_HArray1OfGroupItem.hxx>
#include <StepBasic_Group.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

//=======================================================================
//function : RWStepAP214_RWAppliedGroupAssignment
//purpose  : 
//=======================================================================
RWStepAP214_RWAppliedGroupAssignment::RWStepAP214_RWAppliedGroupAssignment ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepAP214_RWAppliedGroupAssignment::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                     const Standard_Integer num,
                                                     Handle(Interface_Check)& ach,
                                                     const Handle(StepAP214_AppliedGroupAssignment) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"applied_group_assignment") ) return;

  // Inherited fields of GroupAssignment

  Handle(StepBasic_Group) aGroupAssignment_AssignedGroup;
  data->ReadEntity (num, 1, "group_assignment.assigned_group", ach, STANDARD_TYPE(StepBasic_Group), aGroupAssignment_AssignedGroup);

  // Own fields of AppliedGroupAssignment

  Handle(StepAP214_HArray1OfGroupItem) anItems;
  Standard_Integer sub2 = 0;
  if ( data->ReadSubList (num, 2, "items", ach, sub2) ) {
    Standard_Integer num2 = sub2;
    Standard_Integer nb0 = data->NbParams(num2);
    if (nb0)
    {
      anItems = new StepAP214_HArray1OfGroupItem (1, nb0);
      for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
        StepAP214_GroupItem anIt0;
        data->ReadEntity (num2, i0, "items", ach, anIt0);
        anItems->SetValue(i0, anIt0);
      }
    }
  }
  // Initialize entity
  ent->Init(aGroupAssignment_AssignedGroup, anItems);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepAP214_RWAppliedGroupAssignment::WriteStep (StepData_StepWriter& SW,
                                                      const Handle(StepAP214_AppliedGroupAssignment) &ent) const
{

  // Inherited fields of GroupAssignment

  SW.Send (ent->StepBasic_GroupAssignment::AssignedGroup());

  // Own fields of AppliedGroupAssignment

  SW.OpenSub();
  if (!ent->Items().IsNull())
  {
    for (Standard_Integer i1=1; i1 <= ent->Items()->Length(); i1++ ) {
      StepAP214_GroupItem Var0 = ent->Items()->Value(i1);
      SW.Send (Var0.Value());
    }
  }
  SW.CloseSub();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepAP214_RWAppliedGroupAssignment::Share (const Handle(StepAP214_AppliedGroupAssignment) &ent,
                                                  Interface_EntityIterator& iter) const
{

  // Inherited fields of GroupAssignment

  iter.AddItem (ent->StepBasic_GroupAssignment::AssignedGroup());

  // Own fields of AppliedGroupAssignment
  if (!ent->Items().IsNull())
  {
    for (Standard_Integer i2=1; i2 <= ent->Items()->Length(); i2++ ) {
      StepAP214_GroupItem Var0 = ent->Items()->Value(i2);
      iter.AddItem (Var0.Value());
    }
  }
}
