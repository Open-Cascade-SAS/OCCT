// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _StepAP203_DateTimeItem_HeaderFile
#define _StepAP203_DateTimeItem_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepBasic_ProductDefinition;
class StepAP203_ChangeRequest;
class StepAP203_StartRequest;
class StepAP203_Change;
class StepAP203_StartWork;
class StepBasic_ApprovalPersonOrganization;
class StepBasic_Contract;
class StepBasic_SecurityClassification;
class StepBasic_Certification;

//! Representation of STEP SELECT type DateTimeItem
class StepAP203_DateTimeItem : public StepData_SelectType
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT StepAP203_DateTimeItem();

  //! Recognizes a kind of DateTimeItem select type
  //! 1 -> ProductDefinition from StepBasic
  //! 2 -> ChangeRequest from StepAP203
  //! 3 -> StartRequest from StepAP203
  //! 4 -> Change from StepAP203
  //! 5 -> StartWork from StepAP203
  //! 6 -> ApprovalPersonOrganization from StepBasic
  //! 7 -> Contract from StepBasic
  //! 8 -> SecurityClassification from StepBasic
  //! 9 -> Certification from StepBasic
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum(const Handle(Standard_Transient)& ent) const;

  //! Returns Value as ProductDefinition (or Null if another type)
  Standard_EXPORT Handle(StepBasic_ProductDefinition) ProductDefinition() const;

  //! Returns Value as ChangeRequest (or Null if another type)
  Standard_EXPORT Handle(StepAP203_ChangeRequest) ChangeRequest() const;

  //! Returns Value as StartRequest (or Null if another type)
  Standard_EXPORT Handle(StepAP203_StartRequest) StartRequest() const;

  //! Returns Value as Change (or Null if another type)
  Standard_EXPORT Handle(StepAP203_Change) Change() const;

  //! Returns Value as StartWork (or Null if another type)
  Standard_EXPORT Handle(StepAP203_StartWork) StartWork() const;

  //! Returns Value as ApprovalPersonOrganization (or Null if another type)
  Standard_EXPORT Handle(StepBasic_ApprovalPersonOrganization) ApprovalPersonOrganization() const;

  //! Returns Value as Contract (or Null if another type)
  Standard_EXPORT Handle(StepBasic_Contract) Contract() const;

  //! Returns Value as SecurityClassification (or Null if another type)
  Standard_EXPORT Handle(StepBasic_SecurityClassification) SecurityClassification() const;

  //! Returns Value as Certification (or Null if another type)
  Standard_EXPORT Handle(StepBasic_Certification) Certification() const;

protected:
private:
};

#endif // _StepAP203_DateTimeItem_HeaderFile
