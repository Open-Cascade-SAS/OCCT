// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <StepAP203_PersonOrganizationItem.ixx>

//=======================================================================
//function : StepAP203_PersonOrganizationItem
//purpose  : 
//=======================================================================

StepAP203_PersonOrganizationItem::StepAP203_PersonOrganizationItem ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepAP203_PersonOrganizationItem::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepAP203_Change))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepAP203_StartWork))) return 2;
  if (ent->IsKind(STANDARD_TYPE(StepAP203_ChangeRequest))) return 3;
  if (ent->IsKind(STANDARD_TYPE(StepAP203_StartRequest))) return 4;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ConfigurationItem))) return 5;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_Product))) return 6;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionFormation))) return 7;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition))) return 8;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_Contract))) return 9;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_SecurityClassification))) return 10;
  return 0;
}

//=======================================================================
//function : Change
//purpose  : 
//=======================================================================

Handle(StepAP203_Change) StepAP203_PersonOrganizationItem::Change () const
{
  return Handle(StepAP203_Change)::DownCast(Value());
}

//=======================================================================
//function : StartWork
//purpose  : 
//=======================================================================

Handle(StepAP203_StartWork) StepAP203_PersonOrganizationItem::StartWork () const
{
  return Handle(StepAP203_StartWork)::DownCast(Value());
}

//=======================================================================
//function : ChangeRequest
//purpose  : 
//=======================================================================

Handle(StepAP203_ChangeRequest) StepAP203_PersonOrganizationItem::ChangeRequest () const
{
  return Handle(StepAP203_ChangeRequest)::DownCast(Value());
}

//=======================================================================
//function : StartRequest
//purpose  : 
//=======================================================================

Handle(StepAP203_StartRequest) StepAP203_PersonOrganizationItem::StartRequest () const
{
  return Handle(StepAP203_StartRequest)::DownCast(Value());
}

//=======================================================================
//function : ConfigurationItem
//purpose  : 
//=======================================================================

Handle(StepRepr_ConfigurationItem) StepAP203_PersonOrganizationItem::ConfigurationItem () const
{
  return Handle(StepRepr_ConfigurationItem)::DownCast(Value());
}

//=======================================================================
//function : Product
//purpose  : 
//=======================================================================

Handle(StepBasic_Product) StepAP203_PersonOrganizationItem::Product () const
{
  return Handle(StepBasic_Product)::DownCast(Value());
}

//=======================================================================
//function : ProductDefinitionFormation
//purpose  : 
//=======================================================================

Handle(StepBasic_ProductDefinitionFormation) StepAP203_PersonOrganizationItem::ProductDefinitionFormation () const
{
  return Handle(StepBasic_ProductDefinitionFormation)::DownCast(Value());
}

//=======================================================================
//function : ProductDefinition
//purpose  : 
//=======================================================================

Handle(StepBasic_ProductDefinition) StepAP203_PersonOrganizationItem::ProductDefinition () const
{
  return Handle(StepBasic_ProductDefinition)::DownCast(Value());
}

//=======================================================================
//function : Contract
//purpose  : 
//=======================================================================

Handle(StepBasic_Contract) StepAP203_PersonOrganizationItem::Contract () const
{
  return Handle(StepBasic_Contract)::DownCast(Value());
}

//=======================================================================
//function : SecurityClassification
//purpose  : 
//=======================================================================

Handle(StepBasic_SecurityClassification) StepAP203_PersonOrganizationItem::SecurityClassification () const
{
  return Handle(StepBasic_SecurityClassification)::DownCast(Value());
}
