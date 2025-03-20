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

#ifndef _StepAP214_ExternalIdentificationItem_HeaderFile
#define _StepAP214_ExternalIdentificationItem_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepBasic_DocumentFile;
class StepAP214_AppliedOrganizationAssignment;
class StepAP214_AppliedPersonAndOrganizationAssignment;
class StepAP214_ExternallyDefinedClass;
class StepAP214_ExternallyDefinedGeneralProperty;
class StepBasic_Approval;
class StepBasic_ApprovalStatus;
class StepBasic_ExternalSource;
class StepBasic_OrganizationalAddress;
class StepBasic_ProductDefinition;
class StepBasic_SecurityClassification;
class StepBasic_VersionedActionRequest;
class StepGeom_TrimmedCurve;
class StepBasic_DateAndTimeAssignment;
class StepBasic_DateAssignment;

//! Representation of STEP SELECT type ExternalIdentificationItem
class StepAP214_ExternalIdentificationItem : public StepData_SelectType
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT StepAP214_ExternalIdentificationItem();

  //! Recognizes a kind of ExternalIdentificationItem select type
  //! 1 -> DocumentFile from StepBasic
  //! 2 -> ExternallyDefinedClass from StepAP214
  //! 3 -> ExternallyDefinedGeneralProperty from StepAP214
  //! 4 -> ProductDefinition from StepBasic
  //! 5 -> AppliedOrganizationAssignment from AP214
  //! 6 -> AppliedPersonAndOrganizationAssignment from AP214
  //! 7 -> Approval from StepBasic
  //! 8 -> ApprovalStatus from StepBasic
  //! 9 -> ExternalSource from StepBasic
  //! 10 -> OrganizationalAddress from StepBasic
  //! 11 -> SecurityClassification from StepBasic
  //! 12 -> TrimmedCurve from StepGeom
  //! 13 -> VersionedActionRequest from StepBasic
  //! 14 -> DateAndTimeAssignment from StepBasic
  //! 15 -> DateAssignment from StepBasic
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum(const Handle(Standard_Transient)& ent) const;

  //! Returns Value as DocumentFile (or Null if another type)
  Standard_EXPORT Handle(StepBasic_DocumentFile) DocumentFile() const;

  //! Returns Value as ExternallyDefinedClass (or Null if another type)
  Standard_EXPORT Handle(StepAP214_ExternallyDefinedClass) ExternallyDefinedClass() const;

  //! Returns Value as ExternallyDefinedGeneralProperty (or Null if another type)
  Standard_EXPORT Handle(StepAP214_ExternallyDefinedGeneralProperty)
    ExternallyDefinedGeneralProperty() const;

  //! Returns Value as ProductDefinition (or Null if another type)
  Standard_EXPORT Handle(StepBasic_ProductDefinition) ProductDefinition() const;

  //! Returns Value as AppliedOrganizationAssignment (or Null if another type)
  Standard_EXPORT Handle(StepAP214_AppliedOrganizationAssignment) AppliedOrganizationAssignment()
    const;

  //! Returns Value as AppliedPersonAndOrganizationAssignment (or Null if another type)
  Standard_EXPORT Handle(StepAP214_AppliedPersonAndOrganizationAssignment)
    AppliedPersonAndOrganizationAssignment() const;

  //! Returns Value as Approval (or Null if another type)
  Standard_EXPORT Handle(StepBasic_Approval) Approval() const;

  //! Returns Value as ApprovalStatus (or Null if another type)
  Standard_EXPORT Handle(StepBasic_ApprovalStatus) ApprovalStatus() const;

  //! Returns Value as ExternalSource (or Null if another type)
  Standard_EXPORT Handle(StepBasic_ExternalSource) ExternalSource() const;

  //! Returns Value as OrganizationalAddress (or Null if another type)
  Standard_EXPORT Handle(StepBasic_OrganizationalAddress) OrganizationalAddress() const;

  //! Returns Value as SecurityClassification (or Null if another type)
  Standard_EXPORT Handle(StepBasic_SecurityClassification) SecurityClassification() const;

  //! Returns Value as TrimmedCurve (or Null if another type)
  Standard_EXPORT Handle(StepGeom_TrimmedCurve) TrimmedCurve() const;

  //! Returns Value as VersionedActionRequest (or Null if another type)
  Standard_EXPORT Handle(StepBasic_VersionedActionRequest) VersionedActionRequest() const;

  //! Returns Value as DateAndTimeAssignment (or Null if another type)
  Standard_EXPORT Handle(StepBasic_DateAndTimeAssignment) DateAndTimeAssignment() const;

  //! Returns Value as DateAssignment (or Null if another type)
  Standard_EXPORT Handle(StepBasic_DateAssignment) DateAssignment() const;

protected:
private:
};

#endif // _StepAP214_ExternalIdentificationItem_HeaderFile
