// File:	StepAP203_CertifiedItem.cxx
// Created:	Fri Nov 26 16:26:26 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepAP203_CertifiedItem.ixx>

//=======================================================================
//function : StepAP203_CertifiedItem
//purpose  : 
//=======================================================================

StepAP203_CertifiedItem::StepAP203_CertifiedItem ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepAP203_CertifiedItem::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_SuppliedPartRelationship))) return 1;
  return 0;
}

//=======================================================================
//function : SuppliedPartRelationship
//purpose  : 
//=======================================================================

Handle(StepRepr_SuppliedPartRelationship) StepAP203_CertifiedItem::SuppliedPartRelationship () const
{
  return Handle(StepRepr_SuppliedPartRelationship)::DownCast(Value());
}
