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
#include <StepAP214_SecurityClassificationItem.hxx>
#include <StepBasic_Action.hxx>
#include <StepBasic_Document.hxx>
#include <StepBasic_DocumentFile.hxx>
#include <StepBasic_Product.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_ProductDefinitionFormation.hxx>
#include <StepBasic_ProductDefinitionRelationship.hxx>
#include <StepBasic_VersionedActionRequest.hxx>
#include <StepBasic_GeneralProperty.hxx>
#include <StepRepr_ProductConcept.hxx>
#include <StepRepr_AssemblyComponentUsage.hxx>
#include <StepRepr_AssemblyComponentUsageSubstitute.hxx>
#include <StepRepr_ConfigurationDesign.hxx>
#include <StepRepr_ConfigurationEffectivity.hxx>
#include <StepRepr_MakeFromUsageOption.hxx>
#include <StepRepr_MaterialDesignation.hxx>
#include <StepRepr_ProductDefinitionUsage.hxx>
#include <StepRepr_PropertyDefinition.hxx>
#include <StepShape_ShapeRepresentation.hxx>
#include <StepVisual_DraughtingModel.hxx>
#include <StepVisual_MechanicalDesignGeometricPresentationRepresentation.hxx>
#include <StepVisual_PresentationArea.hxx>

StepAP214_SecurityClassificationItem::StepAP214_SecurityClassificationItem () {  }

Standard_Integer StepAP214_SecurityClassificationItem::CaseNum(const Handle(Standard_Transient)& ent) const
{
	
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_Action))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_AssemblyComponentUsage))) return 2;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_AssemblyComponentUsageSubstitute))) return 3;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ConfigurationDesign))) return 4;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ConfigurationEffectivity))) return 5;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_Document))) return 6;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_DocumentFile))) return 7;
  if (ent->IsKind(STANDARD_TYPE(StepVisual_DraughtingModel))) return 8;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_GeneralProperty))) return 9;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_MakeFromUsageOption))) return 10;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_MaterialDesignation))) return 11;
  if (ent->IsKind(STANDARD_TYPE(StepVisual_MechanicalDesignGeometricPresentationRepresentation))) return 12;
  if (ent->IsKind(STANDARD_TYPE(StepVisual_PresentationArea))) return 13;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_Product))) return 14;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ProductConcept))) return 15;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition))) return 16;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionFormation))) return 17;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionRelationship))) return 18;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ProductDefinitionUsage))) return 19;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_PropertyDefinition))) return 20;
  if (ent->IsKind(STANDARD_TYPE(StepShape_ShapeRepresentation))) return 21;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_VersionedActionRequest))) return 22;
  return 0;
}

Handle(StepBasic_Action) StepAP214_SecurityClassificationItem::Action() const
{
  return GetCasted(StepBasic_Action,Value());
}

Handle(StepRepr_AssemblyComponentUsage) StepAP214_SecurityClassificationItem::AssemblyComponentUsage() const
{
  return GetCasted(StepRepr_AssemblyComponentUsage,Value());
}

Handle(StepRepr_ConfigurationDesign) StepAP214_SecurityClassificationItem::ConfigurationDesign() const
{
  return GetCasted(StepRepr_ConfigurationDesign,Value());
}

Handle(StepRepr_ConfigurationEffectivity) StepAP214_SecurityClassificationItem::ConfigurationEffectivity() const
{
  return GetCasted(StepRepr_ConfigurationEffectivity,Value());
}

Handle(StepVisual_DraughtingModel) StepAP214_SecurityClassificationItem::DraughtingModel() const
{
  return GetCasted(StepVisual_DraughtingModel,Value());
}

Handle(StepBasic_GeneralProperty) StepAP214_SecurityClassificationItem::GeneralProperty() const
{
  return GetCasted(StepBasic_GeneralProperty,Value());
}

Handle(StepRepr_MakeFromUsageOption) StepAP214_SecurityClassificationItem::MakeFromUsageOption() const
{
  return GetCasted(StepRepr_MakeFromUsageOption,Value());
}

Handle(StepRepr_ProductConcept) StepAP214_SecurityClassificationItem::ProductConcept() const
{
  return GetCasted(StepRepr_ProductConcept,Value());
}

Handle(StepRepr_ProductDefinitionUsage) StepAP214_SecurityClassificationItem::ProductDefinitionUsage() const
{
  return GetCasted(StepRepr_ProductDefinitionUsage,Value());
}

Handle(StepBasic_VersionedActionRequest) StepAP214_SecurityClassificationItem::VersionedActionRequest() const
{
  return GetCasted(StepBasic_VersionedActionRequest,Value());
}
