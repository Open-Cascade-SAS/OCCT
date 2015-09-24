// Created on: 2015-07-10
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <StepAP242_ItemIdentifiedRepresentationUsageDefinition.hxx>
#include <Interface_Macros.hxx>
#include <StepAP214_AppliedApprovalAssignment.hxx>
#include <StepAP214_AppliedDateAndTimeAssignment.hxx>
#include <StepAP214_AppliedDateAssignment.hxx>
#include <StepAP214_AppliedDocumentReference.hxx>
#include <StepAP214_AppliedExternalIdentificationAssignment.hxx>
#include <StepAP214_AppliedGroupAssignment.hxx>
#include <StepAP214_AppliedOrganizationAssignment.hxx>
#include <StepAP214_AppliedPersonAndOrganizationAssignment.hxx>
#include <StepAP214_AppliedSecurityClassificationAssignment.hxx>
#include <StepBasic_GeneralProperty.hxx>
#include <StepBasic_ProductDefinitionRelationship.hxx>
#include <StepDimTol_GeometricTolerance.hxx>
#include <StepRepr_PropertyDefinition.hxx>
#include <StepRepr_PropertyDefinitionRelationship.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <StepRepr_ShapeAspectRelationship.hxx>
#include <StepShape_DimensionalSize.hxx>

//=======================================================================
//function : StepAP242_ItemIdentifiedRepresentationUsageDefinition
//purpose  : 
//=======================================================================

StepAP242_ItemIdentifiedRepresentationUsageDefinition::
  StepAP242_ItemIdentifiedRepresentationUsageDefinition () {  }

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepAP242_ItemIdentifiedRepresentationUsageDefinition::
  CaseNum(const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepAP214_AppliedApprovalAssignment))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepAP214_AppliedDateAndTimeAssignment))) return 2;
  if (ent->IsKind(STANDARD_TYPE(StepAP214_AppliedDateAssignment))) return 3;
  if (ent->IsKind(STANDARD_TYPE(StepAP214_AppliedDocumentReference))) return 4;
  if (ent->IsKind(STANDARD_TYPE(StepAP214_AppliedExternalIdentificationAssignment))) return 5;
  if (ent->IsKind(STANDARD_TYPE(StepAP214_AppliedGroupAssignment))) return 6;
  if (ent->IsKind(STANDARD_TYPE(StepAP214_AppliedOrganizationAssignment))) return 7;
  if (ent->IsKind(STANDARD_TYPE(StepAP214_AppliedPersonAndOrganizationAssignment))) return 8;
  if (ent->IsKind(STANDARD_TYPE(StepAP214_AppliedSecurityClassificationAssignment))) return 9;
  if (ent->IsKind(STANDARD_TYPE(StepShape_DimensionalSize))) return 10;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_GeneralProperty))) return 11;
  if (ent->IsKind(STANDARD_TYPE(StepDimTol_GeometricTolerance))) return 12;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionRelationship))) return 13;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_PropertyDefinition))) return 14;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_PropertyDefinitionRelationship))) return 15;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ShapeAspect))) return 16;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ShapeAspectRelationship))) return 17;
  return 0;
}

Handle(StepAP214_AppliedApprovalAssignment) StepAP242_ItemIdentifiedRepresentationUsageDefinition::AppliedApprovalAssignment() const
{  return GetCasted(StepAP214_AppliedApprovalAssignment,Value());  }

Handle(StepAP214_AppliedDateAndTimeAssignment) StepAP242_ItemIdentifiedRepresentationUsageDefinition::AppliedDateAndTimeAssignment() const
{  return GetCasted(StepAP214_AppliedDateAndTimeAssignment,Value());  }

Handle(StepAP214_AppliedDateAssignment) StepAP242_ItemIdentifiedRepresentationUsageDefinition::AppliedDateAssignment() const
{  return GetCasted(StepAP214_AppliedDateAssignment,Value());  }

Handle(StepAP214_AppliedDocumentReference) StepAP242_ItemIdentifiedRepresentationUsageDefinition::AppliedDocumentReference() const
{  return GetCasted(StepAP214_AppliedDocumentReference,Value());  }

Handle(StepAP214_AppliedExternalIdentificationAssignment) StepAP242_ItemIdentifiedRepresentationUsageDefinition::AppliedExternalIdentificationAssignment() const
{  return GetCasted(StepAP214_AppliedExternalIdentificationAssignment,Value());  }

Handle(StepAP214_AppliedGroupAssignment) StepAP242_ItemIdentifiedRepresentationUsageDefinition::AppliedGroupAssignment() const
{  return GetCasted(StepAP214_AppliedGroupAssignment,Value());  }

Handle(StepAP214_AppliedOrganizationAssignment) StepAP242_ItemIdentifiedRepresentationUsageDefinition::AppliedOrganizationAssignment() const
{  return GetCasted(StepAP214_AppliedOrganizationAssignment,Value());  }

Handle(StepAP214_AppliedPersonAndOrganizationAssignment) StepAP242_ItemIdentifiedRepresentationUsageDefinition::AppliedPersonAndOrganizationAssignment() const
{  return GetCasted(StepAP214_AppliedPersonAndOrganizationAssignment,Value());  }

Handle(StepAP214_AppliedSecurityClassificationAssignment) StepAP242_ItemIdentifiedRepresentationUsageDefinition::AppliedSecurityClassificationAssignment() const
{  return GetCasted(StepAP214_AppliedSecurityClassificationAssignment,Value());  }

Handle(StepShape_DimensionalSize) StepAP242_ItemIdentifiedRepresentationUsageDefinition::DimensionalSize() const
{  return GetCasted(StepShape_DimensionalSize,Value());  }

Handle(StepBasic_GeneralProperty) StepAP242_ItemIdentifiedRepresentationUsageDefinition::GeneralProperty() const
{  return GetCasted(StepBasic_GeneralProperty,Value());  }

Handle(StepDimTol_GeometricTolerance) StepAP242_ItemIdentifiedRepresentationUsageDefinition::GeometricTolerance() const
{  return GetCasted(StepDimTol_GeometricTolerance,Value());  }

Handle(StepBasic_ProductDefinitionRelationship) StepAP242_ItemIdentifiedRepresentationUsageDefinition::ProductDefinitionRelationship() const
{  return GetCasted(StepBasic_ProductDefinitionRelationship,Value());  }

Handle(StepRepr_PropertyDefinition) StepAP242_ItemIdentifiedRepresentationUsageDefinition::PropertyDefinition() const
{  return GetCasted(StepRepr_PropertyDefinition,Value());  }

Handle(StepRepr_PropertyDefinitionRelationship) StepAP242_ItemIdentifiedRepresentationUsageDefinition::PropertyDefinitionRelationship() const
{  return GetCasted(StepRepr_PropertyDefinitionRelationship,Value());  }

Handle(StepRepr_ShapeAspect) StepAP242_ItemIdentifiedRepresentationUsageDefinition::ShapeAspect() const
{  return GetCasted(StepRepr_ShapeAspect,Value());  }

Handle(StepRepr_ShapeAspectRelationship) StepAP242_ItemIdentifiedRepresentationUsageDefinition::ShapeAspectRelationship() const
{  return GetCasted(StepRepr_ShapeAspectRelationship,Value());  }

