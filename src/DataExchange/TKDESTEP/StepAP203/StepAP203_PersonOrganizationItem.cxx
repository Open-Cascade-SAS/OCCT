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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <Standard_Transient.hxx>
#include <StepAP203_Change.hxx>
#include <StepAP203_ChangeRequest.hxx>
#include <StepAP203_PersonOrganizationItem.hxx>
#include <StepAP203_StartRequest.hxx>
#include <StepAP203_StartWork.hxx>
#include <StepBasic_Contract.hxx>
#include <StepBasic_Product.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_ProductDefinitionFormation.hxx>
#include <StepBasic_SecurityClassification.hxx>
#include <StepRepr_ConfigurationItem.hxx>

//=================================================================================================

StepAP203_PersonOrganizationItem::StepAP203_PersonOrganizationItem() {}

//=================================================================================================

Standard_Integer StepAP203_PersonOrganizationItem::CaseNum(
  const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull())
    return 0;
  if (ent->IsKind(STANDARD_TYPE(StepAP203_Change)))
    return 1;
  if (ent->IsKind(STANDARD_TYPE(StepAP203_StartWork)))
    return 2;
  if (ent->IsKind(STANDARD_TYPE(StepAP203_ChangeRequest)))
    return 3;
  if (ent->IsKind(STANDARD_TYPE(StepAP203_StartRequest)))
    return 4;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ConfigurationItem)))
    return 5;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_Product)))
    return 6;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionFormation)))
    return 7;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition)))
    return 8;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_Contract)))
    return 9;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_SecurityClassification)))
    return 10;
  return 0;
}

//=================================================================================================

Handle(StepAP203_Change) StepAP203_PersonOrganizationItem::Change() const
{
  return Handle(StepAP203_Change)::DownCast(Value());
}

//=================================================================================================

Handle(StepAP203_StartWork) StepAP203_PersonOrganizationItem::StartWork() const
{
  return Handle(StepAP203_StartWork)::DownCast(Value());
}

//=================================================================================================

Handle(StepAP203_ChangeRequest) StepAP203_PersonOrganizationItem::ChangeRequest() const
{
  return Handle(StepAP203_ChangeRequest)::DownCast(Value());
}

//=================================================================================================

Handle(StepAP203_StartRequest) StepAP203_PersonOrganizationItem::StartRequest() const
{
  return Handle(StepAP203_StartRequest)::DownCast(Value());
}

//=================================================================================================

Handle(StepRepr_ConfigurationItem) StepAP203_PersonOrganizationItem::ConfigurationItem() const
{
  return Handle(StepRepr_ConfigurationItem)::DownCast(Value());
}

//=================================================================================================

Handle(StepBasic_Product) StepAP203_PersonOrganizationItem::Product() const
{
  return Handle(StepBasic_Product)::DownCast(Value());
}

//=================================================================================================

Handle(StepBasic_ProductDefinitionFormation) StepAP203_PersonOrganizationItem::
  ProductDefinitionFormation() const
{
  return Handle(StepBasic_ProductDefinitionFormation)::DownCast(Value());
}

//=================================================================================================

Handle(StepBasic_ProductDefinition) StepAP203_PersonOrganizationItem::ProductDefinition() const
{
  return Handle(StepBasic_ProductDefinition)::DownCast(Value());
}

//=================================================================================================

Handle(StepBasic_Contract) StepAP203_PersonOrganizationItem::Contract() const
{
  return Handle(StepBasic_Contract)::DownCast(Value());
}

//=================================================================================================

Handle(StepBasic_SecurityClassification) StepAP203_PersonOrganizationItem::SecurityClassification()
  const
{
  return Handle(StepBasic_SecurityClassification)::DownCast(Value());
}
