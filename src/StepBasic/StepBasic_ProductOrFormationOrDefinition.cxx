// File:	StepBasic_ProductOrFormationOrDefinition.cxx
// Created:	Tue Jan 28 12:40:35 2003 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepBasic_ProductOrFormationOrDefinition.ixx>

//=======================================================================
//function : StepBasic_ProductOrFormationOrDefinition
//purpose  : 
//=======================================================================

StepBasic_ProductOrFormationOrDefinition::StepBasic_ProductOrFormationOrDefinition ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepBasic_ProductOrFormationOrDefinition::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_Product))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionFormation))) return 2;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition))) return 3;
  return 0;
}

//=======================================================================
//function : Product
//purpose  : 
//=======================================================================

Handle(StepBasic_Product) StepBasic_ProductOrFormationOrDefinition::Product () const
{
  return Handle(StepBasic_Product)::DownCast(Value());
}

//=======================================================================
//function : ProductDefinitionFormation
//purpose  : 
//=======================================================================

Handle(StepBasic_ProductDefinitionFormation) StepBasic_ProductOrFormationOrDefinition::ProductDefinitionFormation () const
{
  return Handle(StepBasic_ProductDefinitionFormation)::DownCast(Value());
}

//=======================================================================
//function : ProductDefinition
//purpose  : 
//=======================================================================

Handle(StepBasic_ProductDefinition) StepBasic_ProductOrFormationOrDefinition::ProductDefinition () const
{
  return Handle(StepBasic_ProductDefinition)::DownCast(Value());
}
