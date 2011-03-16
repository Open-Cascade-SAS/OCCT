// File:	StepAP203_SpecifiedItem.cxx
// Created:	Fri Nov 26 16:26:27 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepAP203_SpecifiedItem.ixx>

//=======================================================================
//function : StepAP203_SpecifiedItem
//purpose  : 
//=======================================================================

StepAP203_SpecifiedItem::StepAP203_SpecifiedItem ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepAP203_SpecifiedItem::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ShapeAspect))) return 2;
  return 0;
}

//=======================================================================
//function : ProductDefinition
//purpose  : 
//=======================================================================

Handle(StepBasic_ProductDefinition) StepAP203_SpecifiedItem::ProductDefinition () const
{
  return Handle(StepBasic_ProductDefinition)::DownCast(Value());
}

//=======================================================================
//function : ShapeAspect
//purpose  : 
//=======================================================================

Handle(StepRepr_ShapeAspect) StepAP203_SpecifiedItem::ShapeAspect () const
{
  return Handle(StepRepr_ShapeAspect)::DownCast(Value());
}
