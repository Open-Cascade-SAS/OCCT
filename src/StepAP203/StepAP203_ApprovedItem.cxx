// File:	StepAP203_ApprovedItem.cxx
// Created:	Fri Nov 26 16:26:26 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepAP203_ApprovedItem.ixx>

//=======================================================================
//function : StepAP203_ApprovedItem
//purpose  : 
//=======================================================================

StepAP203_ApprovedItem::StepAP203_ApprovedItem ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepAP203_ApprovedItem::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionFormation))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition))) return 2;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ConfigurationEffectivity))) return 3;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ConfigurationItem))) return 4;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_SecurityClassification))) return 5;
  if (ent->IsKind(STANDARD_TYPE(StepAP203_ChangeRequest))) return 6;
  if (ent->IsKind(STANDARD_TYPE(StepAP203_Change))) return 7;
  if (ent->IsKind(STANDARD_TYPE(StepAP203_StartRequest))) return 8;
  if (ent->IsKind(STANDARD_TYPE(StepAP203_StartWork))) return 9;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_Certification))) return 10;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_Contract))) return 11;
  return 0;
}

//=======================================================================
//function : ProductDefinitionFormation
//purpose  : 
//=======================================================================

Handle(StepBasic_ProductDefinitionFormation) StepAP203_ApprovedItem::ProductDefinitionFormation () const
{
  return Handle(StepBasic_ProductDefinitionFormation)::DownCast(Value());
}

//=======================================================================
//function : ProductDefinition
//purpose  : 
//=======================================================================

Handle(StepBasic_ProductDefinition) StepAP203_ApprovedItem::ProductDefinition () const
{
  return Handle(StepBasic_ProductDefinition)::DownCast(Value());
}

//=======================================================================
//function : ConfigurationEffectivity
//purpose  : 
//=======================================================================

Handle(StepRepr_ConfigurationEffectivity) StepAP203_ApprovedItem::ConfigurationEffectivity () const
{
  return Handle(StepRepr_ConfigurationEffectivity)::DownCast(Value());
}

//=======================================================================
//function : ConfigurationItem
//purpose  : 
//=======================================================================

Handle(StepRepr_ConfigurationItem) StepAP203_ApprovedItem::ConfigurationItem () const
{
  return Handle(StepRepr_ConfigurationItem)::DownCast(Value());
}

//=======================================================================
//function : SecurityClassification
//purpose  : 
//=======================================================================

Handle(StepBasic_SecurityClassification) StepAP203_ApprovedItem::SecurityClassification () const
{
  return Handle(StepBasic_SecurityClassification)::DownCast(Value());
}

//=======================================================================
//function : ChangeRequest
//purpose  : 
//=======================================================================

Handle(StepAP203_ChangeRequest) StepAP203_ApprovedItem::ChangeRequest () const
{
  return Handle(StepAP203_ChangeRequest)::DownCast(Value());
}

//=======================================================================
//function : Change
//purpose  : 
//=======================================================================

Handle(StepAP203_Change) StepAP203_ApprovedItem::Change () const
{
  return Handle(StepAP203_Change)::DownCast(Value());
}

//=======================================================================
//function : StartRequest
//purpose  : 
//=======================================================================

Handle(StepAP203_StartRequest) StepAP203_ApprovedItem::StartRequest () const
{
  return Handle(StepAP203_StartRequest)::DownCast(Value());
}

//=======================================================================
//function : StartWork
//purpose  : 
//=======================================================================

Handle(StepAP203_StartWork) StepAP203_ApprovedItem::StartWork () const
{
  return Handle(StepAP203_StartWork)::DownCast(Value());
}

//=======================================================================
//function : Certification
//purpose  : 
//=======================================================================

Handle(StepBasic_Certification) StepAP203_ApprovedItem::Certification () const
{
  return Handle(StepBasic_Certification)::DownCast(Value());
}

//=======================================================================
//function : Contract
//purpose  : 
//=======================================================================

Handle(StepBasic_Contract) StepAP203_ApprovedItem::Contract () const
{
  return Handle(StepBasic_Contract)::DownCast(Value());
}
