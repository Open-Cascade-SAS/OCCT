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

#include <StepAP214_AutoDesignPresentedItemSelect.ixx>
#include <Interface_Macros.hxx>

StepAP214_AutoDesignPresentedItemSelect::StepAP214_AutoDesignPresentedItemSelect ()    {  }

Standard_Integer  StepAP214_AutoDesignPresentedItemSelect::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionRelationship))) return 2;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ProductDefinitionShape))) return 3;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_RepresentationRelationship))) return 4;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ShapeAspect))) return 5;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_DocumentRelationship))) return 6;
  return 0;
}

Handle(StepBasic_ProductDefinition)  StepAP214_AutoDesignPresentedItemSelect::ProductDefinition () const
{
  return GetCasted(StepBasic_ProductDefinition,Value());
}

Handle(StepBasic_ProductDefinitionRelationship)  StepAP214_AutoDesignPresentedItemSelect::ProductDefinitionRelationship () const
{
  return GetCasted(StepBasic_ProductDefinitionRelationship,Value());
}

Handle(StepRepr_ProductDefinitionShape)  StepAP214_AutoDesignPresentedItemSelect::ProductDefinitionShape () const
{
  return GetCasted(StepRepr_ProductDefinitionShape,Value());
}

Handle(StepRepr_RepresentationRelationship)  StepAP214_AutoDesignPresentedItemSelect::RepresentationRelationship () const
{
  return GetCasted(StepRepr_RepresentationRelationship,Value());
}

Handle(StepRepr_ShapeAspect)  StepAP214_AutoDesignPresentedItemSelect::ShapeAspect () const
{
  return GetCasted(StepRepr_ShapeAspect,Value());
}

Handle(StepBasic_DocumentRelationship)  StepAP214_AutoDesignPresentedItemSelect::DocumentRelationship () const
{
  return GetCasted(StepBasic_DocumentRelationship,Value());
}
