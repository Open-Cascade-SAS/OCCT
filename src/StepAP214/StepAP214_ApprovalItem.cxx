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
#include <StepAP214_ApprovalItem.hxx>
#include <StepBasic_Date.hxx>
#include <StepBasic_Document.hxx>
#include <StepBasic_DocumentFile.hxx>
#include <StepBasic_Effectivity.hxx>
#include <StepBasic_Group.hxx>
#include <StepBasic_GroupRelationship.hxx>
#include <StepBasic_Product.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_ProductDefinitionFormation.hxx>
#include <StepBasic_ProductDefinitionFormationRelationship.hxx>
#include <StepBasic_ProductDefinitionRelationship.hxx>
#include <StepBasic_SecurityClassification.hxx>
#include <StepRepr_AssemblyComponentUsageSubstitute.hxx>
#include <StepRepr_ConfigurationItem.hxx>
#include <StepRepr_MaterialDesignation.hxx>
#include <StepRepr_PropertyDefinition.hxx>
#include <StepRepr_Representation.hxx>
#include <StepRepr_ShapeAspectRelationship.hxx>
#include <StepShape_ShapeRepresentation.hxx>
#include <StepVisual_MechanicalDesignGeometricPresentationRepresentation.hxx>
#include <StepVisual_PresentationArea.hxx>

StepAP214_ApprovalItem::StepAP214_ApprovalItem () {  }

Standard_Integer StepAP214_ApprovalItem::CaseNum(const Handle(Standard_Transient)& ent) const
{
	
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_AssemblyComponentUsageSubstitute))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_DocumentFile))) return 2;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_MaterialDesignation))) return 3;
  if (ent->IsKind(STANDARD_TYPE(StepVisual_MechanicalDesignGeometricPresentationRepresentation))) return 4;
  if (ent->IsKind(STANDARD_TYPE(StepVisual_PresentationArea))) return 5;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_Product))) return 6;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition))) return 7;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionFormation))) return 8;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionRelationship))) return 9;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_PropertyDefinition))) return 10;
  if (ent->IsKind(STANDARD_TYPE(StepShape_ShapeRepresentation))) return 11;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_SecurityClassification))) return 12;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ConfigurationItem))) return 13;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_Date))) return 14;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_Document))) return 15;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_Effectivity))) return 16;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_Group))) return 17;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_GroupRelationship))) return 18;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionFormationRelationship))) return 19;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_Representation))) return 20;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ShapeAspectRelationship))) return 21;
  return 0;
}


Handle(StepRepr_AssemblyComponentUsageSubstitute)  StepAP214_ApprovalItem::AssemblyComponentUsageSubstitute() const
{  return GetCasted(StepRepr_AssemblyComponentUsageSubstitute,Value());  }

Handle(StepBasic_DocumentFile) StepAP214_ApprovalItem::DocumentFile() const
{  return GetCasted(StepBasic_DocumentFile,Value());  }


Handle(StepRepr_MaterialDesignation) StepAP214_ApprovalItem::MaterialDesignation() const
{  return GetCasted(StepRepr_MaterialDesignation,Value());  }

Handle(StepVisual_MechanicalDesignGeometricPresentationRepresentation) StepAP214_ApprovalItem::MechanicalDesignGeometricPresentationRepresentation() const
{  return GetCasted(StepVisual_MechanicalDesignGeometricPresentationRepresentation,Value());  }

Handle(StepVisual_PresentationArea) StepAP214_ApprovalItem::PresentationArea() const
{  return GetCasted(StepVisual_PresentationArea,Value());  }

Handle(StepBasic_Product) StepAP214_ApprovalItem::Product() const
{  return GetCasted(StepBasic_Product,Value());  }

Handle(StepBasic_ProductDefinition) StepAP214_ApprovalItem::ProductDefinition () const
{  return GetCasted(StepBasic_ProductDefinition,Value());  }

Handle(StepBasic_ProductDefinitionFormation) StepAP214_ApprovalItem::ProductDefinitionFormation() const
{  return GetCasted(StepBasic_ProductDefinitionFormation,Value());  }

Handle(StepBasic_ProductDefinitionRelationship) StepAP214_ApprovalItem::ProductDefinitionRelationship() const
{  return GetCasted(StepBasic_ProductDefinitionRelationship,Value());  }

Handle(StepRepr_PropertyDefinition) StepAP214_ApprovalItem::PropertyDefinition() const
{  return GetCasted(StepRepr_PropertyDefinition,Value());  }

Handle(StepShape_ShapeRepresentation) StepAP214_ApprovalItem::ShapeRepresentation() const
{  return GetCasted(StepShape_ShapeRepresentation,Value());  }

Handle(StepBasic_SecurityClassification) StepAP214_ApprovalItem::SecurityClassification() const
{  return GetCasted(StepBasic_SecurityClassification,Value());  }

Handle(StepRepr_ConfigurationItem) StepAP214_ApprovalItem::ConfigurationItem() const
{  return GetCasted(StepRepr_ConfigurationItem,Value());  }

Handle(StepBasic_Date) StepAP214_ApprovalItem::Date() const
{  return GetCasted(StepBasic_Date,Value());  }

Handle(StepBasic_Document) StepAP214_ApprovalItem::Document() const
{  return GetCasted(StepBasic_Document,Value());  }

Handle(StepBasic_Effectivity) StepAP214_ApprovalItem::Effectivity() const
{  return GetCasted(StepBasic_Effectivity,Value());  }

Handle(StepBasic_Group) StepAP214_ApprovalItem::Group() const
{  return GetCasted(StepBasic_Group,Value());  }

Handle(StepBasic_GroupRelationship) StepAP214_ApprovalItem::GroupRelationship() const
{  return GetCasted(StepBasic_GroupRelationship,Value());  }

Handle(StepBasic_ProductDefinitionFormationRelationship) StepAP214_ApprovalItem::ProductDefinitionFormationRelationship() const
{  return GetCasted(StepBasic_ProductDefinitionFormationRelationship,Value());  }

Handle(StepRepr_Representation) StepAP214_ApprovalItem::Representation() const
{  return GetCasted(StepRepr_Representation,Value());  }

Handle(StepRepr_ShapeAspectRelationship) StepAP214_ApprovalItem::ShapeAspectRelationship() const
{  return GetCasted(StepRepr_ShapeAspectRelationship,Value());  }
