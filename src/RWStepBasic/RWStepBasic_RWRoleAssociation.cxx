// File:	RWStepBasic_RWRoleAssociation.cxx
// Created:	Wed May 10 15:09:09 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

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
