// File:	StepBasic_RoleAssociation.cxx
// Created:	Wed May 10 15:09:08 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <StepBasic_RoleAssociation.ixx>

//=======================================================================
//function : StepBasic_RoleAssociation
//purpose  : 
//=======================================================================

StepBasic_RoleAssociation::StepBasic_RoleAssociation ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_RoleAssociation::Init (const Handle(StepBasic_ObjectRole) &aRole,
                                      const StepBasic_RoleSelect &aItemWithRole)
{

  theRole = aRole;

  theItemWithRole = aItemWithRole;
}

//=======================================================================
//function : Role
//purpose  : 
//=======================================================================

Handle(StepBasic_ObjectRole) StepBasic_RoleAssociation::Role () const
{
  return theRole;
}

//=======================================================================
//function : SetRole
//purpose  : 
//=======================================================================

void StepBasic_RoleAssociation::SetRole (const Handle(StepBasic_ObjectRole) &aRole)
{
  theRole = aRole;
}

//=======================================================================
//function : ItemWithRole
//purpose  : 
//=======================================================================

StepBasic_RoleSelect StepBasic_RoleAssociation::ItemWithRole () const
{
  return theItemWithRole;
}

//=======================================================================
//function : SetItemWithRole
//purpose  : 
//=======================================================================

void StepBasic_RoleAssociation::SetItemWithRole (const StepBasic_RoleSelect &aItemWithRole)
{
  theItemWithRole = aItemWithRole;
}
