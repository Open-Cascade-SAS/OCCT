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
#include <RWStepBasic_RWRoleAssociation.hxx>
#include <StepBasic_ObjectRole.hxx>
#include <StepBasic_RoleAssociation.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

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
