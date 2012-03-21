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
