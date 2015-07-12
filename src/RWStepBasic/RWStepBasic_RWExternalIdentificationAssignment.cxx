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
#include <RWStepBasic_RWExternalIdentificationAssignment.hxx>
#include <StepBasic_ExternalIdentificationAssignment.hxx>
#include <StepBasic_ExternalSource.hxx>
#include <StepBasic_IdentificationRole.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

//=======================================================================
//function : RWStepBasic_RWExternalIdentificationAssignment
//purpose  : 
//=======================================================================
RWStepBasic_RWExternalIdentificationAssignment::RWStepBasic_RWExternalIdentificationAssignment ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWExternalIdentificationAssignment::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                               const Standard_Integer num,
                                                               Handle(Interface_Check)& ach,
                                                               const Handle(StepBasic_ExternalIdentificationAssignment) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"external_identification_assignment") ) return;

  // Inherited fields of IdentificationAssignment

  Handle(TCollection_HAsciiString) aIdentificationAssignment_AssignedId;
  data->ReadString (num, 1, "identification_assignment.assigned_id", ach, aIdentificationAssignment_AssignedId);

  Handle(StepBasic_IdentificationRole) aIdentificationAssignment_Role;
  data->ReadEntity (num, 2, "identification_assignment.role", ach, STANDARD_TYPE(StepBasic_IdentificationRole), aIdentificationAssignment_Role);

  // Own fields of ExternalIdentificationAssignment

  Handle(StepBasic_ExternalSource) aSource;
  data->ReadEntity (num, 3, "source", ach, STANDARD_TYPE(StepBasic_ExternalSource), aSource);

  // Initialize entity
  ent->Init(aIdentificationAssignment_AssignedId,
            aIdentificationAssignment_Role,
            aSource);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWExternalIdentificationAssignment::WriteStep (StepData_StepWriter& SW,
                                                                const Handle(StepBasic_ExternalIdentificationAssignment) &ent) const
{

  // Inherited fields of IdentificationAssignment

  SW.Send (ent->StepBasic_IdentificationAssignment::AssignedId());

  SW.Send (ent->StepBasic_IdentificationAssignment::Role());

  // Own fields of ExternalIdentificationAssignment

  SW.Send (ent->Source());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWExternalIdentificationAssignment::Share (const Handle(StepBasic_ExternalIdentificationAssignment) &ent,
                                                            Interface_EntityIterator& iter) const
{

  // Inherited fields of IdentificationAssignment

  iter.AddItem (ent->StepBasic_IdentificationAssignment::Role());

  // Own fields of ExternalIdentificationAssignment

  iter.AddItem (ent->Source());
}
