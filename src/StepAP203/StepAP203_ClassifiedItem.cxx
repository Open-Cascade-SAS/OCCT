// File:	StepAP203_ClassifiedItem.cxx
// Created:	Fri Nov 26 16:26:26 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepAP203_ClassifiedItem.ixx>

//=======================================================================
//function : StepAP203_ClassifiedItem
//purpose  : 
//=======================================================================

StepAP203_ClassifiedItem::StepAP203_ClassifiedItem ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepAP203_ClassifiedItem::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionFormation))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_AssemblyComponentUsage))) return 2;
  return 0;
}

//=======================================================================
//function : ProductDefinitionFormation
//purpose  : 
//=======================================================================

Handle(StepBasic_ProductDefinitionFormation) StepAP203_ClassifiedItem::ProductDefinitionFormation () const
{
  return Handle(StepBasic_ProductDefinitionFormation)::DownCast(Value());
}

//=======================================================================
//function : AssemblyComponentUsage
//purpose  : 
//=======================================================================

Handle(StepRepr_AssemblyComponentUsage) StepAP203_ClassifiedItem::AssemblyComponentUsage () const
{
  return Handle(StepRepr_AssemblyComponentUsage)::DownCast(Value());
}
