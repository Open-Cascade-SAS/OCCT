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



#include <StepAP214_DateItem.ixx>
#include <Interface_Macros.hxx>

StepAP214_DateItem::StepAP214_DateItem () {  }

Standard_Integer StepAP214_DateItem::CaseNum(const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ApprovalPersonOrganization))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepAP214_AppliedPersonAndOrganizationAssignment))) return 2;
  if (ent->IsKind(STANDARD_TYPE(StepAP214_AppliedOrganizationAssignment))) return 3;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_AssemblyComponentUsageSubstitute))) return 4;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_DocumentFile))) return 5;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_Effectivity))) return 6;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_MaterialDesignation))) return 7;
  if (ent->IsKind(STANDARD_TYPE(StepVisual_MechanicalDesignGeometricPresentationRepresentation))) return 8;
  if (ent->IsKind(STANDARD_TYPE(StepVisual_PresentationArea))) return 9;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_Product))) return 10;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition))) return 11;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionFormation))) return 12;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionRelationship))) return 13;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_PropertyDefinition))) return 14;
  if (ent->IsKind(STANDARD_TYPE(StepShape_ShapeRepresentation))) return 15;
  return 0;
}

Handle(StepBasic_ApprovalPersonOrganization) StepAP214_DateItem::ApprovalPersonOrganization () const
{
	return GetCasted(StepBasic_ApprovalPersonOrganization,Value());
}

Handle(StepAP214_AppliedPersonAndOrganizationAssignment) StepAP214_DateItem::AppliedPersonAndOrganizationAssignment () const
{
	return GetCasted(StepAP214_AppliedPersonAndOrganizationAssignment,Value());
}

Handle(StepAP214_AppliedOrganizationAssignment) StepAP214_DateItem::AppliedOrganizationAssignment() const
{  return GetCasted(StepAP214_AppliedOrganizationAssignment,Value());  }


Handle(StepBasic_Effectivity) StepAP214_DateItem::Effectivity() const
{  return GetCasted(StepBasic_Effectivity,Value());  }


