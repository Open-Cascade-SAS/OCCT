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
#include <StepAP214_AppliedExternalIdentificationAssignment.hxx>
#include <StepAP214_DocumentReferenceItem.hxx>
#include <StepBasic_Approval.hxx>
#include <StepBasic_CharacterizedObject.hxx>
#include <StepBasic_ExternallyDefinedItem.hxx>
#include <StepBasic_Group.hxx>
#include <StepBasic_GroupRelationship.hxx>
#include <StepBasic_ProductCategory.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_ProductDefinitionContext.hxx>
#include <StepBasic_ProductDefinitionRelationship.hxx>
#include <StepRepr_AssemblyComponentUsage.hxx>
#include <StepRepr_DescriptiveRepresentationItem.hxx>
#include <StepRepr_MaterialDesignation.hxx>
#include <StepRepr_MeasureRepresentationItem.hxx>
#include <StepRepr_PropertyDefinition.hxx>
#include <StepRepr_Representation.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <StepRepr_ShapeAspectRelationship.hxx>
#include <StepShape_DimensionalSize.hxx>

StepAP214_DocumentReferenceItem::StepAP214_DocumentReferenceItem () {  }

Standard_Integer StepAP214_DocumentReferenceItem::CaseNum(const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_Approval))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_DescriptiveRepresentationItem))) return 2;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_MaterialDesignation))) return 3;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition))) return 4;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionRelationship))) return 5;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_PropertyDefinition))) return 6;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_Representation))) return 7;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ShapeAspect))) return 8;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ShapeAspectRelationship))) return 9;
  if (ent->IsKind(STANDARD_TYPE(StepAP214_AppliedExternalIdentificationAssignment))) return 10;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_AssemblyComponentUsage))) return 11;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_CharacterizedObject))) return 12;
  if (ent->IsKind(STANDARD_TYPE(StepShape_DimensionalSize))) return 13;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ExternallyDefinedItem))) return 14;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_Group))) return 15;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_GroupRelationship))) return 16;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_MeasureRepresentationItem))) return 17;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductCategory))) return 18;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionContext))) return 19;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_RepresentationItem))) return 20;
  return 0;
}


Handle(StepBasic_Approval) StepAP214_DocumentReferenceItem::Approval() const
{  return GetCasted(StepBasic_Approval,Value());  }

Handle(StepRepr_DescriptiveRepresentationItem)  StepAP214_DocumentReferenceItem::DescriptiveRepresentationItem() const
{  return GetCasted(StepRepr_DescriptiveRepresentationItem,Value());  }

Handle(StepRepr_MaterialDesignation) StepAP214_DocumentReferenceItem::MaterialDesignation() const
{  return GetCasted(StepRepr_MaterialDesignation,Value());  }


Handle(StepBasic_ProductDefinition) StepAP214_DocumentReferenceItem::ProductDefinition () const
{  return GetCasted(StepBasic_ProductDefinition,Value());  }


Handle(StepBasic_ProductDefinitionRelationship) StepAP214_DocumentReferenceItem::ProductDefinitionRelationship() const
{  return GetCasted(StepBasic_ProductDefinitionRelationship,Value());  }

Handle(StepRepr_PropertyDefinition) StepAP214_DocumentReferenceItem::PropertyDefinition() const
{  return GetCasted(StepRepr_PropertyDefinition,Value());  }

Handle(StepRepr_Representation)  StepAP214_DocumentReferenceItem::Representation() const
{  return GetCasted(StepRepr_Representation,Value());  }

Handle(StepRepr_ShapeAspect)  StepAP214_DocumentReferenceItem::ShapeAspect() const
{  return GetCasted(StepRepr_ShapeAspect,Value());  }

Handle(StepRepr_ShapeAspectRelationship)  StepAP214_DocumentReferenceItem::ShapeAspectRelationship() const
{  return GetCasted(StepRepr_ShapeAspectRelationship,Value());  }

Handle(StepAP214_AppliedExternalIdentificationAssignment)  StepAP214_DocumentReferenceItem::AppliedExternalIdentificationAssignment() const
{  return GetCasted(StepAP214_AppliedExternalIdentificationAssignment,Value());  }

Handle(StepRepr_AssemblyComponentUsage)  StepAP214_DocumentReferenceItem::AssemblyComponentUsage() const
{  return GetCasted(StepRepr_AssemblyComponentUsage,Value());  }

Handle(StepBasic_CharacterizedObject)  StepAP214_DocumentReferenceItem::CharacterizedObject() const
{  return GetCasted(StepBasic_CharacterizedObject,Value());  }

Handle(StepShape_DimensionalSize)  StepAP214_DocumentReferenceItem::DimensionalSize() const
{  return GetCasted(StepShape_DimensionalSize,Value());  }

Handle(StepBasic_ExternallyDefinedItem)  StepAP214_DocumentReferenceItem::ExternallyDefinedItem() const
{  return GetCasted(StepBasic_ExternallyDefinedItem,Value());  }

Handle(StepBasic_Group)  StepAP214_DocumentReferenceItem::Group() const
{  return GetCasted(StepBasic_Group,Value());  }

Handle(StepBasic_GroupRelationship)  StepAP214_DocumentReferenceItem::GroupRelationship() const
{  return GetCasted(StepBasic_GroupRelationship,Value());  }

Handle(StepRepr_MeasureRepresentationItem)  StepAP214_DocumentReferenceItem::MeasureRepresentationItem() const
{  return GetCasted(StepRepr_MeasureRepresentationItem,Value());  }

Handle(StepBasic_ProductCategory)  StepAP214_DocumentReferenceItem::ProductCategory() const
{  return GetCasted(StepBasic_ProductCategory,Value());  }

Handle(StepBasic_ProductDefinitionContext)  StepAP214_DocumentReferenceItem::ProductDefinitionContext() const
{  return GetCasted(StepBasic_ProductDefinitionContext,Value());  }

Handle(StepRepr_RepresentationItem)  StepAP214_DocumentReferenceItem::RepresentationItem() const
{  return GetCasted(StepRepr_RepresentationItem,Value());  }
