// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

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
