// Created on: 2000-05-10
// Created by: Andrey BETENEV
// Copyright (c) 2000-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <RWStepBasic_RWIdentificationAssignment.ixx>

//=======================================================================
//function : RWStepBasic_RWIdentificationAssignment
//purpose  : 
//=======================================================================

RWStepBasic_RWIdentificationAssignment::RWStepBasic_RWIdentificationAssignment ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWIdentificationAssignment::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                       const Standard_Integer num,
                                                       Handle(Interface_Check)& ach,
                                                       const Handle(StepBasic_IdentificationAssignment) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"identification_assignment") ) return;

  // Own fields of IdentificationAssignment

  Handle(TCollection_HAsciiString) aAssignedId;
  data->ReadString (num, 1, "assigned_id", ach, aAssignedId);

  Handle(StepBasic_IdentificationRole) aRole;
  data->ReadEntity (num, 2, "role", ach, STANDARD_TYPE(StepBasic_IdentificationRole), aRole);

  // Initialize entity
  ent->Init(aAssignedId,
            aRole);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWIdentificationAssignment::WriteStep (StepData_StepWriter& SW,
                                                        const Handle(StepBasic_IdentificationAssignment) &ent) const
{

  // Own fields of IdentificationAssignment

  SW.Send (ent->AssignedId());

  SW.Send (ent->Role());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWIdentificationAssignment::Share (const Handle(StepBasic_IdentificationAssignment) &ent,
                                                    Interface_EntityIterator& iter) const
{

  // Own fields of IdentificationAssignment

  iter.AddItem (ent->Role());
}
