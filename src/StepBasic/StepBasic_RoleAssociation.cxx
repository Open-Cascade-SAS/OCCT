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

#include <Standard_Type.hxx>
#include <StepBasic_ObjectRole.hxx>
#include <StepBasic_RoleAssociation.hxx>
#include <StepBasic_RoleSelect.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_RoleAssociation,Standard_Transient)

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
