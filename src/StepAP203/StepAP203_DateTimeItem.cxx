// File:	StepAP203_DateTimeItem.cxx
// Created:	Fri Nov 26 16:26:27 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepAP203_DateTimeItem.ixx>

//=======================================================================
//function : StepAP203_DateTimeItem
//purpose  : 
//=======================================================================

StepAP203_DateTimeItem::StepAP203_DateTimeItem ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepAP203_DateTimeItem::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepAP203_ChangeRequest))) return 2;
  if (ent->IsKind(STANDARD_TYPE(StepAP203_StartRequest))) return 3;
  if (ent->IsKind(STANDARD_TYPE(StepAP203_Change))) return 4;
  if (ent->IsKind(STANDARD_TYPE(StepAP203_StartWork))) return 5;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ApprovalPersonOrganization))) return 6;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_Contract))) return 7;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_SecurityClassification))) return 8;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_Certification))) return 9;
  return 0;
}

//=======================================================================
//function : ProductDefinition
//purpose  : 
//=======================================================================

Handle(StepBasic_ProductDefinition) StepAP203_DateTimeItem::ProductDefinition () const
{
  return Handle(StepBasic_ProductDefinition)::DownCast(Value());
}

//=======================================================================
//function : ChangeRequest
//purpose  : 
//=======================================================================

Handle(StepAP203_ChangeRequest) StepAP203_DateTimeItem::ChangeRequest () const
{
  return Handle(StepAP203_ChangeRequest)::DownCast(Value());
}

//=======================================================================
//function : StartRequest
//purpose  : 
//=======================================================================

Handle(StepAP203_StartRequest) StepAP203_DateTimeItem::StartRequest () const
{
  return Handle(StepAP203_StartRequest)::DownCast(Value());
}

//=======================================================================
//function : Change
//purpose  : 
//=======================================================================

Handle(StepAP203_Change) StepAP203_DateTimeItem::Change () const
{
  return Handle(StepAP203_Change)::DownCast(Value());
}

//=======================================================================
//function : StartWork
//purpose  : 
//=======================================================================

Handle(StepAP203_StartWork) StepAP203_DateTimeItem::StartWork () const
{
  return Handle(StepAP203_StartWork)::DownCast(Value());
}

//=======================================================================
//function : ApprovalPersonOrganization
//purpose  : 
//=======================================================================

Handle(StepBasic_ApprovalPersonOrganization) StepAP203_DateTimeItem::ApprovalPersonOrganization () const
{
  return Handle(StepBasic_ApprovalPersonOrganization)::DownCast(Value());
}

//=======================================================================
//function : Contract
//purpose  : 
//=======================================================================

Handle(StepBasic_Contract) StepAP203_DateTimeItem::Contract () const
{
  return Handle(StepBasic_Contract)::DownCast(Value());
}

//=======================================================================
//function : SecurityClassification
//purpose  : 
//=======================================================================

Handle(StepBasic_SecurityClassification) StepAP203_DateTimeItem::SecurityClassification () const
{
  return Handle(StepBasic_SecurityClassification)::DownCast(Value());
}

//=======================================================================
//function : Certification
//purpose  : 
//=======================================================================

Handle(StepBasic_Certification) StepAP203_DateTimeItem::Certification () const
{
  return Handle(StepBasic_Certification)::DownCast(Value());
}
