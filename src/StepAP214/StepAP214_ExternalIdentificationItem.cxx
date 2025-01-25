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
#include <StepAP214_AppliedOrganizationAssignment.hxx>
#include <StepAP214_AppliedPersonAndOrganizationAssignment.hxx>
#include <StepAP214_ExternalIdentificationItem.hxx>
#include <StepAP214_ExternallyDefinedClass.hxx>
#include <StepAP214_ExternallyDefinedGeneralProperty.hxx>
#include <StepBasic_Approval.hxx>
#include <StepBasic_ApprovalStatus.hxx>
#include <StepBasic_DateAndTimeAssignment.hxx>
#include <StepBasic_DateAssignment.hxx>
#include <StepBasic_DocumentFile.hxx>
#include <StepBasic_ExternalSource.hxx>
#include <StepBasic_OrganizationalAddress.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_SecurityClassification.hxx>
#include <StepBasic_VersionedActionRequest.hxx>
#include <StepGeom_TrimmedCurve.hxx>

//=================================================================================================

StepAP214_ExternalIdentificationItem::StepAP214_ExternalIdentificationItem() {}

//=================================================================================================

Standard_Integer StepAP214_ExternalIdentificationItem::CaseNum(
  const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull())
    return 0;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_DocumentFile)))
    return 1;
  if (ent->IsKind(STANDARD_TYPE(StepAP214_ExternallyDefinedClass)))
    return 2;
  if (ent->IsKind(STANDARD_TYPE(StepAP214_ExternallyDefinedGeneralProperty)))
    return 3;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition)))
    return 4;
  if (ent->IsKind(STANDARD_TYPE(StepAP214_AppliedOrganizationAssignment)))
    return 5;
  if (ent->IsKind(STANDARD_TYPE(StepAP214_AppliedPersonAndOrganizationAssignment)))
    return 6;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_Approval)))
    return 7;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ApprovalStatus)))
    return 8;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ExternalSource)))
    return 9;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_OrganizationalAddress)))
    return 10;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_SecurityClassification)))
    return 11;
  if (ent->IsKind(STANDARD_TYPE(StepGeom_TrimmedCurve)))
    return 12;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_VersionedActionRequest)))
    return 13;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_DateAndTimeAssignment)))
    return 14;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_DateAssignment)))
    return 15;
  return 0;
}

//=================================================================================================

Handle(StepBasic_DocumentFile) StepAP214_ExternalIdentificationItem::DocumentFile() const
{
  return Handle(StepBasic_DocumentFile)::DownCast(Value());
}

//=================================================================================================

Handle(StepAP214_ExternallyDefinedClass) StepAP214_ExternalIdentificationItem::
  ExternallyDefinedClass() const
{
  return Handle(StepAP214_ExternallyDefinedClass)::DownCast(Value());
}

//=================================================================================================

Handle(StepAP214_ExternallyDefinedGeneralProperty) StepAP214_ExternalIdentificationItem::
  ExternallyDefinedGeneralProperty() const
{
  return Handle(StepAP214_ExternallyDefinedGeneralProperty)::DownCast(Value());
}

//=================================================================================================

Handle(StepBasic_ProductDefinition) StepAP214_ExternalIdentificationItem::ProductDefinition() const
{
  return Handle(StepBasic_ProductDefinition)::DownCast(Value());
}

//=================================================================================================

Handle(StepAP214_AppliedOrganizationAssignment) StepAP214_ExternalIdentificationItem::
  AppliedOrganizationAssignment() const
{
  return Handle(StepAP214_AppliedOrganizationAssignment)::DownCast(Value());
}

//=================================================================================================

Handle(StepAP214_AppliedPersonAndOrganizationAssignment) StepAP214_ExternalIdentificationItem::
  AppliedPersonAndOrganizationAssignment() const
{
  return Handle(StepAP214_AppliedPersonAndOrganizationAssignment)::DownCast(Value());
}

//=================================================================================================

Handle(StepBasic_Approval) StepAP214_ExternalIdentificationItem::Approval() const
{
  return Handle(StepBasic_Approval)::DownCast(Value());
}

//=================================================================================================

Handle(StepBasic_ApprovalStatus) StepAP214_ExternalIdentificationItem::ApprovalStatus() const
{
  return Handle(StepBasic_ApprovalStatus)::DownCast(Value());
}

//=================================================================================================

Handle(StepBasic_ExternalSource) StepAP214_ExternalIdentificationItem::ExternalSource() const
{
  return Handle(StepBasic_ExternalSource)::DownCast(Value());
}

//=================================================================================================

Handle(StepBasic_OrganizationalAddress) StepAP214_ExternalIdentificationItem::
  OrganizationalAddress() const
{
  return Handle(StepBasic_OrganizationalAddress)::DownCast(Value());
}

//=================================================================================================

Handle(StepBasic_SecurityClassification) StepAP214_ExternalIdentificationItem::
  SecurityClassification() const
{
  return Handle(StepBasic_SecurityClassification)::DownCast(Value());
}

//=================================================================================================

Handle(StepGeom_TrimmedCurve) StepAP214_ExternalIdentificationItem::TrimmedCurve() const
{
  return Handle(StepGeom_TrimmedCurve)::DownCast(Value());
}

//=================================================================================================

Handle(StepBasic_VersionedActionRequest) StepAP214_ExternalIdentificationItem::
  VersionedActionRequest() const
{
  return Handle(StepBasic_VersionedActionRequest)::DownCast(Value());
}

//=================================================================================================

Handle(StepBasic_DateAndTimeAssignment) StepAP214_ExternalIdentificationItem::
  DateAndTimeAssignment() const
{
  return Handle(StepBasic_DateAndTimeAssignment)::DownCast(Value());
}

//=================================================================================================

Handle(StepBasic_DateAssignment) StepAP214_ExternalIdentificationItem::DateAssignment() const
{
  return Handle(StepBasic_DateAssignment)::DownCast(Value());
}
