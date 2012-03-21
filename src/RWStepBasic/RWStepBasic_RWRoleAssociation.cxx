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

#include <RWStepBasic_RWRoleAssociation.ixx>

//=======================================================================
//function : RWStepBasic_RWRoleAssociation
//purpose  : 
//=======================================================================

RWStepBasic_RWRoleAssociation::RWStepBasic_RWRoleAssociation ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWRoleAssociation::ReadStep (const Handle(StepData_StepReaderData)& data,
                                              const Standard_Integer num,
                                              Handle(Interface_Check)& ach,
                                              const Handle(StepBasic_RoleAssociation) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"role_association") ) return;

  // Own fields of RoleAssociation

  Handle(StepBasic_ObjectRole) aRole;
  data->ReadEntity (num, 1, "role", ach, STANDARD_TYPE(StepBasic_ObjectRole), aRole);

  StepBasic_RoleSelect aItemWithRole;
  data->ReadEntity (num, 2, "item_with_role", ach, aItemWithRole);

  // Initialize entity
  ent->Init(aRole,
            aItemWithRole);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWRoleAssociation::WriteStep (StepData_StepWriter& SW,
                                               const Handle(StepBasic_RoleAssociation) &ent) const
{

  // Own fields of RoleAssociation

  SW.Send (ent->Role());

  SW.Send (ent->ItemWithRole().Value());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWRoleAssociation::Share (const Handle(StepBasic_RoleAssociation) &ent,
                                           Interface_EntityIterator& iter) const
{

  // Own fields of RoleAssociation

  iter.AddItem (ent->Role());

  iter.AddItem (ent->ItemWithRole().Value());
}
