#include <StepAP214_PresentedItemSelect.ixx>
#include <Interface_Macros.hxx>

StepAP214_PresentedItemSelect::StepAP214_PresentedItemSelect ()    {  }

Standard_Integer  StepAP214_PresentedItemSelect::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionRelationship))) return 2;
  return 0;
}

Handle(StepBasic_ProductDefinition)  StepAP214_PresentedItemSelect::ProductDefinition () const
{
  return GetCasted(StepBasic_ProductDefinition,Value());
}

Handle(StepBasic_ProductDefinitionRelationship)  StepAP214_PresentedItemSelect::ProductDefinitionRelationship () const
{
  return GetCasted(StepBasic_ProductDefinitionRelationship,Value());
}

