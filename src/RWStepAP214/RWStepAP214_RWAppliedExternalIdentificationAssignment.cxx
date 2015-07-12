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
#include <RWStepAP214_RWAppliedExternalIdentificationAssignment.hxx>
#include <StepAP214_AppliedExternalIdentificationAssignment.hxx>
#include <StepAP214_ExternalIdentificationItem.hxx>
#include <StepAP214_HArray1OfExternalIdentificationItem.hxx>
#include <StepBasic_ExternalSource.hxx>
#include <StepBasic_IdentificationRole.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

//=======================================================================
//function : RWStepAP214_RWAppliedExternalIdentificationAssignment
//purpose  : 
//=======================================================================
RWStepAP214_RWAppliedExternalIdentificationAssignment::RWStepAP214_RWAppliedExternalIdentificationAssignment ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepAP214_RWAppliedExternalIdentificationAssignment::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                                      const Standard_Integer num,
                                                                      Handle(Interface_Check)& ach,
                                                                      const Handle(StepAP214_AppliedExternalIdentificationAssignment) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,4,ach,"applied_external_identification_assignment") ) return;

  // Inherited fields of IdentificationAssignment

  Handle(TCollection_HAsciiString) aIdentificationAssignment_AssignedId;
  data->ReadString (num, 1, "identification_assignment.assigned_id", ach, aIdentificationAssignment_AssignedId);

  Handle(StepBasic_IdentificationRole) aIdentificationAssignment_Role;
  data->ReadEntity (num, 2, "identification_assignment.role", ach, STANDARD_TYPE(StepBasic_IdentificationRole), aIdentificationAssignment_Role);

  // Inherited fields of ExternalIdentificationAssignment

  Handle(StepBasic_ExternalSource) aExternalIdentificationAssignment_Source;
  data->ReadEntity (num, 3, "external_identification_assignment.source", ach, STANDARD_TYPE(StepBasic_ExternalSource), aExternalIdentificationAssignment_Source);

  // Own fields of AppliedExternalIdentificationAssignment

  Handle(StepAP214_HArray1OfExternalIdentificationItem) aItems;
  Standard_Integer sub4 = 0;
  if ( data->ReadSubList (num, 4, "items", ach, sub4) ) {
    Standard_Integer num2 = sub4;
    Standard_Integer nb0 = data->NbParams(num2);
    aItems = new StepAP214_HArray1OfExternalIdentificationItem (1, nb0);
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      StepAP214_ExternalIdentificationItem anIt0;
      data->ReadEntity (num2, i0, "items", ach, anIt0);
      aItems->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  ent->Init(aIdentificationAssignment_AssignedId,
            aIdentificationAssignment_Role,
            aExternalIdentificationAssignment_Source,
            aItems);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepAP214_RWAppliedExternalIdentificationAssignment::WriteStep (StepData_StepWriter& SW,
                                                                       const Handle(StepAP214_AppliedExternalIdentificationAssignment) &ent) const
{

  // Inherited fields of IdentificationAssignment

  SW.Send (ent->StepBasic_IdentificationAssignment::AssignedId());

  SW.Send (ent->StepBasic_IdentificationAssignment::Role());

  // Inherited fields of ExternalIdentificationAssignment

  SW.Send (ent->StepBasic_ExternalIdentificationAssignment::Source());

  // Own fields of AppliedExternalIdentificationAssignment

  SW.OpenSub();
  for (Standard_Integer i3=1; i3 <= ent->Items()->Length(); i3++ ) {
    StepAP214_ExternalIdentificationItem Var0 = ent->Items()->Value(i3);
    SW.Send (Var0.Value());
  }
  SW.CloseSub();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepAP214_RWAppliedExternalIdentificationAssignment::Share (const Handle(StepAP214_AppliedExternalIdentificationAssignment) &ent,
                                                                   Interface_EntityIterator& iter) const
{

  // Inherited fields of IdentificationAssignment

  iter.AddItem (ent->StepBasic_IdentificationAssignment::Role());

  // Inherited fields of ExternalIdentificationAssignment

  iter.AddItem (ent->StepBasic_ExternalIdentificationAssignment::Source());

  // Own fields of AppliedExternalIdentificationAssignment

  for (Standard_Integer i3=1; i3 <= ent->Items()->Length(); i3++ ) {
    StepAP214_ExternalIdentificationItem Var0 = ent->Items()->Value(i3);
    iter.AddItem (Var0.Value());
  }
}
