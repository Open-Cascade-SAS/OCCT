// Created on: 2000-05-11
// Created by: data exchange team
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <StepRepr_CharacterizedDefinition.ixx>
//=======================================================================
//function : StepRepr_CharacterizedDefinition
//purpose  : 
//=======================================================================

StepRepr_CharacterizedDefinition::StepRepr_CharacterizedDefinition ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepRepr_CharacterizedDefinition::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_CharacterizedObject))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition))) return 2;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionRelationship))) return 3;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ProductDefinitionShape))) return 4;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ShapeAspect))) return 5;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ShapeAspectRelationship))) return 6;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_DocumentFile))) return 7;
  return 0;
}

//=======================================================================
//function : CharacterizedObject
//purpose  : 
//=======================================================================

Handle(StepBasic_CharacterizedObject) StepRepr_CharacterizedDefinition::CharacterizedObject () const
{
  return Handle(StepBasic_CharacterizedObject)::DownCast(Value());
}

//=======================================================================
//function : ProductDefinition
//purpose  : 
//=======================================================================

Handle(StepBasic_ProductDefinition) StepRepr_CharacterizedDefinition::ProductDefinition () const
{
  return Handle(StepBasic_ProductDefinition)::DownCast(Value());
}

//=======================================================================
//function : ProductDefinitionRelationship
//purpose  : 
//=======================================================================

Handle(StepBasic_ProductDefinitionRelationship) StepRepr_CharacterizedDefinition::ProductDefinitionRelationship () const
{
  return Handle(StepBasic_ProductDefinitionRelationship)::DownCast(Value());
}

//=======================================================================
//function : ProductDefinitionShape
//purpose  : 
//=======================================================================

Handle(StepRepr_ProductDefinitionShape) StepRepr_CharacterizedDefinition::ProductDefinitionShape () const
{
  return Handle(StepRepr_ProductDefinitionShape)::DownCast(Value());
}

//=======================================================================
//function : ShapeAspect
//purpose  : 
//=======================================================================

Handle(StepRepr_ShapeAspect) StepRepr_CharacterizedDefinition::ShapeAspect () const
{
  return Handle(StepRepr_ShapeAspect)::DownCast(Value());
}

//=======================================================================
//function : ShapeAspectRelationship
//purpose  : 
//=======================================================================

Handle(StepRepr_ShapeAspectRelationship) StepRepr_CharacterizedDefinition::ShapeAspectRelationship () const
{
  return Handle(StepRepr_ShapeAspectRelationship)::DownCast(Value());
}

//=======================================================================
//function : DocumentFile
//purpose  : 
//=======================================================================

Handle(StepBasic_DocumentFile) StepRepr_CharacterizedDefinition::DocumentFile () const
{
  return Handle(StepBasic_DocumentFile)::DownCast(Value());
}
