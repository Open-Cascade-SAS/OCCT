
#include <StepAP214_PersonAndOrganizationItem.ixx>
#include <Interface_Macros.hxx>

StepAP214_PersonAndOrganizationItem::StepAP214_PersonAndOrganizationItem () {  }

Standard_Integer StepAP214_PersonAndOrganizationItem::CaseNum(const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepAP214_AppliedOrganizationAssignment))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_AssemblyComponentUsageSubstitute))) return 2;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_DocumentFile))) return 3;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_MaterialDesignation))) return 4;
  if (ent->IsKind(STANDARD_TYPE(StepVisual_MechanicalDesignGeometricPresentationRepresentation))) return 5;
  if (ent->IsKind(STANDARD_TYPE(StepVisual_PresentationArea))) return 6;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_Product))) return 7;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition))) return 8;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionFormation))) return 9;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionRelationship))) return 10;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_PropertyDefinition))) return 11;
  if (ent->IsKind(STANDARD_TYPE(StepShape_ShapeRepresentation))) return 12;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_SecurityClassification))) return 13;
  
  return 0;
}


Handle(StepAP214_AppliedOrganizationAssignment) StepAP214_PersonAndOrganizationItem::AppliedOrganizationAssignment() const
{
  return GetCasted(StepAP214_AppliedOrganizationAssignment,Value());
}


