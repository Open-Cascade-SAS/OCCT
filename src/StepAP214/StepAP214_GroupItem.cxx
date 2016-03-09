// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <Interface_Macros.hxx>
#include <Standard_Transient.hxx>
#include <StepAP214_GroupItem.hxx>
#include <StepBasic_GroupRelationship.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_ProductDefinitionFormation.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>
#include <StepRepr_MappedItem.hxx>
#include <StepRepr_PropertyDefinitionRepresentation.hxx>
#include <StepRepr_Representation.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepRepr_RepresentationRelationshipWithTransformation.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <StepRepr_ShapeAspectRelationship.hxx>
#include <StepRepr_ShapeRepresentationRelationship.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>
#include <StepVisual_StyledItem.hxx>


StepAP214_GroupItem::StepAP214_GroupItem () {  }

Standard_Integer StepAP214_GroupItem::CaseNum(const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepGeom_GeometricRepresentationItem))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_GroupRelationship))) return 2;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_MappedItem))) return 3;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition))) return 4;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionFormation))) return 5;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_PropertyDefinitionRepresentation))) return 6;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_Representation))) return 7;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_RepresentationItem))) return 8;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_RepresentationRelationshipWithTransformation))) return 9;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ShapeAspect))) return 10;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ShapeAspectRelationship))) return 11;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ShapeRepresentationRelationship))) return 12;
  if (ent->IsKind(STANDARD_TYPE(StepVisual_StyledItem))) return 13;
  if (ent->IsKind(STANDARD_TYPE(StepShape_TopologicalRepresentationItem))) return 14;
  return 0;
}

Handle(StepGeom_GeometricRepresentationItem) StepAP214_GroupItem::GeometricRepresentationItem() const
{
  return GetCasted(StepGeom_GeometricRepresentationItem,Value());
}

Handle(StepBasic_GroupRelationship) StepAP214_GroupItem::GroupRelationship() const
{
  return GetCasted(StepBasic_GroupRelationship,Value());
}

Handle(StepRepr_MappedItem) StepAP214_GroupItem::MappedItem() const
{
  return GetCasted(StepRepr_MappedItem,Value());
}

Handle(StepBasic_ProductDefinition) StepAP214_GroupItem::ProductDefinition() const
{
  return GetCasted(StepBasic_ProductDefinition,Value());
}

Handle(StepBasic_ProductDefinitionFormation) StepAP214_GroupItem::ProductDefinitionFormation() const
{
  return GetCasted(StepBasic_ProductDefinitionFormation,Value());
}

Handle(StepRepr_PropertyDefinitionRepresentation) StepAP214_GroupItem::PropertyDefinitionRepresentation() const
{
  return GetCasted(StepRepr_PropertyDefinitionRepresentation,Value());
}

Handle(StepRepr_Representation) StepAP214_GroupItem::Representation() const
{
  return GetCasted(StepRepr_Representation,Value());
}

Handle(StepRepr_RepresentationItem) StepAP214_GroupItem::RepresentationItem() const
{
  return GetCasted(StepRepr_RepresentationItem,Value());
}

Handle(StepRepr_RepresentationRelationshipWithTransformation) StepAP214_GroupItem::RepresentationRelationshipWithTransformation() const
{
  return GetCasted(StepRepr_RepresentationRelationshipWithTransformation,Value());
}

Handle(StepRepr_ShapeAspect) StepAP214_GroupItem::ShapeAspect() const
{
  return GetCasted(StepRepr_ShapeAspect,Value());
}

Handle(StepRepr_ShapeAspectRelationship) StepAP214_GroupItem::ShapeAspectRelationship() const
{
  return GetCasted(StepRepr_ShapeAspectRelationship,Value());
}

Handle(StepRepr_ShapeRepresentationRelationship) StepAP214_GroupItem::ShapeRepresentationRelationship() const
{
  return GetCasted(StepRepr_ShapeRepresentationRelationship,Value());
}

Handle(StepVisual_StyledItem) StepAP214_GroupItem::StyledItem() const
{
  return GetCasted(StepVisual_StyledItem,Value());
}

Handle(StepShape_TopologicalRepresentationItem) StepAP214_GroupItem::TopologicalRepresentationItem() const
{
  return GetCasted(StepShape_TopologicalRepresentationItem,Value());
}

