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

#include <Standard_Transient.hxx>
#include <StepBasic_ActionAssignment.hxx>
#include <StepBasic_ActionRequestAssignment.hxx>
#include <StepBasic_ApprovalAssignment.hxx>
#include <StepBasic_ApprovalDateTime.hxx>
#include <StepBasic_CertificationAssignment.hxx>
#include <StepBasic_ContractAssignment.hxx>
#include <StepBasic_DocumentReference.hxx>
#include <StepBasic_EffectivityAssignment.hxx>
#include <StepBasic_GroupAssignment.hxx>
#include <StepBasic_NameAssignment.hxx>
#include <StepBasic_RoleSelect.hxx>
#include <StepBasic_SecurityClassificationAssignment.hxx>

//=======================================================================
//function : StepBasic_RoleSelect
//purpose  : 
//=======================================================================
StepBasic_RoleSelect::StepBasic_RoleSelect ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepBasic_RoleSelect::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ActionAssignment))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ActionRequestAssignment))) return 2;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ApprovalAssignment))) return 3;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ApprovalDateTime))) return 4;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_CertificationAssignment))) return 5;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ContractAssignment))) return 6;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_DocumentReference))) return 7;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_EffectivityAssignment))) return 8;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_GroupAssignment))) return 9;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_NameAssignment))) return 10;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_SecurityClassificationAssignment))) return 11;
  return 0;
}

//=======================================================================
//function : ActionAssignment
//purpose  : 
//=======================================================================

Handle(StepBasic_ActionAssignment) StepBasic_RoleSelect::ActionAssignment () const
{
  return Handle(StepBasic_ActionAssignment)::DownCast(Value());
}

//=======================================================================
//function : ActionRequestAssignment
//purpose  : 
//=======================================================================

Handle(StepBasic_ActionRequestAssignment) StepBasic_RoleSelect::ActionRequestAssignment () const
{
  return Handle(StepBasic_ActionRequestAssignment)::DownCast(Value());
}

//=======================================================================
//function : ApprovalAssignment
//purpose  : 
//=======================================================================

Handle(StepBasic_ApprovalAssignment) StepBasic_RoleSelect::ApprovalAssignment () const
{
  return Handle(StepBasic_ApprovalAssignment)::DownCast(Value());
}

//=======================================================================
//function : ApprovalDateTime
//purpose  : 
//=======================================================================

Handle(StepBasic_ApprovalDateTime) StepBasic_RoleSelect::ApprovalDateTime () const
{
  return Handle(StepBasic_ApprovalDateTime)::DownCast(Value());
}

//=======================================================================
//function : CertificationAssignment
//purpose  : 
//=======================================================================

Handle(StepBasic_CertificationAssignment) StepBasic_RoleSelect::CertificationAssignment () const
{
  return Handle(StepBasic_CertificationAssignment)::DownCast(Value());
}

//=======================================================================
//function : ContractAssignment
//purpose  : 
//=======================================================================

Handle(StepBasic_ContractAssignment) StepBasic_RoleSelect::ContractAssignment () const
{
  return Handle(StepBasic_ContractAssignment)::DownCast(Value());
}

//=======================================================================
//function : DocumentReference
//purpose  : 
//=======================================================================

Handle(StepBasic_DocumentReference) StepBasic_RoleSelect::DocumentReference () const
{
  return Handle(StepBasic_DocumentReference)::DownCast(Value());
}

//=======================================================================
//function : EffectivityAssignment
//purpose  : 
//=======================================================================

Handle(StepBasic_EffectivityAssignment) StepBasic_RoleSelect::EffectivityAssignment () const
{
  return Handle(StepBasic_EffectivityAssignment)::DownCast(Value());
}

//=======================================================================
//function : GroupAssignment
//purpose  : 
//=======================================================================

Handle(StepBasic_GroupAssignment) StepBasic_RoleSelect::GroupAssignment () const
{
  return Handle(StepBasic_GroupAssignment)::DownCast(Value());
}

//=======================================================================
//function : NameAssignment
//purpose  : 
//=======================================================================

Handle(StepBasic_NameAssignment) StepBasic_RoleSelect::NameAssignment () const
{
  return Handle(StepBasic_NameAssignment)::DownCast(Value());
}

//=======================================================================
//function : SecurityClassificationAssignment
//purpose  : 
//=======================================================================

Handle(StepBasic_SecurityClassificationAssignment) StepBasic_RoleSelect::SecurityClassificationAssignment () const
{
  return Handle(StepBasic_SecurityClassificationAssignment)::DownCast(Value());
}
