// File:	StepRepr_RepresentedDefinition.cxx
// Created:	Thu Dec 12 16:02:14 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepRepr_RepresentedDefinition.ixx>

//=======================================================================
//function : StepRepr_RepresentedDefinition
//purpose  : 
//=======================================================================

StepRepr_RepresentedDefinition::StepRepr_RepresentedDefinition ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepRepr_RepresentedDefinition::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_GeneralProperty))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_PropertyDefinition))) return 2;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_PropertyDefinitionRelationship))) return 3;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ShapeAspect))) return 4;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ShapeAspectRelationship))) return 5;
  return 0;
}

//=======================================================================
//function : GeneralProperty
//purpose  : 
//=======================================================================

Handle(StepBasic_GeneralProperty) StepRepr_RepresentedDefinition::GeneralProperty () const
{
  return Handle(StepBasic_GeneralProperty)::DownCast(Value());
}

//=======================================================================
//function : PropertyDefinition
//purpose  : 
//=======================================================================

Handle(StepRepr_PropertyDefinition) StepRepr_RepresentedDefinition::PropertyDefinition () const
{
  return Handle(StepRepr_PropertyDefinition)::DownCast(Value());
}

//=======================================================================
//function : PropertyDefinitionRelationship
//purpose  : 
//=======================================================================

Handle(StepRepr_PropertyDefinitionRelationship) StepRepr_RepresentedDefinition::PropertyDefinitionRelationship () const
{
  return Handle(StepRepr_PropertyDefinitionRelationship)::DownCast(Value());
}

//=======================================================================
//function : ShapeAspect
//purpose  : 
//=======================================================================

Handle(StepRepr_ShapeAspect) StepRepr_RepresentedDefinition::ShapeAspect () const
{
  return Handle(StepRepr_ShapeAspect)::DownCast(Value());
}

//=======================================================================
//function : ShapeAspectRelationship
//purpose  : 
//=======================================================================

Handle(StepRepr_ShapeAspectRelationship) StepRepr_RepresentedDefinition::ShapeAspectRelationship () const
{
  return Handle(StepRepr_ShapeAspectRelationship)::DownCast(Value());
}
