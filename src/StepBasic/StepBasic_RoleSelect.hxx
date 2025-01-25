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

#ifndef _StepBasic_RoleSelect_HeaderFile
#define _StepBasic_RoleSelect_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepBasic_ActionAssignment;
class StepBasic_ActionRequestAssignment;
class StepBasic_ApprovalAssignment;
class StepBasic_ApprovalDateTime;
class StepBasic_CertificationAssignment;
class StepBasic_ContractAssignment;
class StepBasic_DocumentReference;
class StepBasic_EffectivityAssignment;
class StepBasic_GroupAssignment;
class StepBasic_NameAssignment;
class StepBasic_SecurityClassificationAssignment;

//! Representation of STEP SELECT type RoleSelect
class StepBasic_RoleSelect : public StepData_SelectType
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT StepBasic_RoleSelect();

  //! Recognizes a kind of RoleSelect select type
  //! 1 -> ActionAssignment from StepBasic
  //! 2 -> ActionRequestAssignment from StepBasic
  //! 3 -> ApprovalAssignment from StepBasic
  //! 4 -> ApprovalDateTime from StepBasic
  //! 5 -> CertificationAssignment from StepBasic
  //! 6 -> ContractAssignment from StepBasic
  //! 7 -> DocumentReference from StepBasic
  //! 8 -> EffectivityAssignment from StepBasic
  //! 9 -> GroupAssignment from StepBasic
  //! 10 -> NameAssignment from StepBasic
  //! 11 -> SecurityClassificationAssignment from StepBasic
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum(const Handle(Standard_Transient)& ent) const;

  //! Returns Value as ActionAssignment (or Null if another type)
  Standard_EXPORT Handle(StepBasic_ActionAssignment) ActionAssignment() const;

  //! Returns Value as ActionRequestAssignment (or Null if another type)
  Standard_EXPORT Handle(StepBasic_ActionRequestAssignment) ActionRequestAssignment() const;

  //! Returns Value as ApprovalAssignment (or Null if another type)
  Standard_EXPORT Handle(StepBasic_ApprovalAssignment) ApprovalAssignment() const;

  //! Returns Value as ApprovalDateTime (or Null if another type)
  Standard_EXPORT Handle(StepBasic_ApprovalDateTime) ApprovalDateTime() const;

  //! Returns Value as CertificationAssignment (or Null if another type)
  Standard_EXPORT Handle(StepBasic_CertificationAssignment) CertificationAssignment() const;

  //! Returns Value as ContractAssignment (or Null if another type)
  Standard_EXPORT Handle(StepBasic_ContractAssignment) ContractAssignment() const;

  //! Returns Value as DocumentReference (or Null if another type)
  Standard_EXPORT Handle(StepBasic_DocumentReference) DocumentReference() const;

  //! Returns Value as EffectivityAssignment (or Null if another type)
  Standard_EXPORT Handle(StepBasic_EffectivityAssignment) EffectivityAssignment() const;

  //! Returns Value as GroupAssignment (or Null if another type)
  Standard_EXPORT Handle(StepBasic_GroupAssignment) GroupAssignment() const;

  //! Returns Value as NameAssignment (or Null if another type)
  Standard_EXPORT Handle(StepBasic_NameAssignment) NameAssignment() const;

  //! Returns Value as SecurityClassificationAssignment (or Null if another type)
  Standard_EXPORT Handle(StepBasic_SecurityClassificationAssignment)
    SecurityClassificationAssignment() const;

protected:
private:
};

#endif // _StepBasic_RoleSelect_HeaderFile
