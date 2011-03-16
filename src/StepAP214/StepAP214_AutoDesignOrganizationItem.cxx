#include <StepAP214_AutoDesignOrganizationItem.ixx>
#include <Interface_Macros.hxx>

StepAP214_AutoDesignOrganizationItem::StepAP214_AutoDesignOrganizationItem ()  {  }

Standard_Integer  StepAP214_AutoDesignOrganizationItem::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  Standard_Integer num = StepAP214_AutoDesignGeneralOrgItem::CaseNum(ent);
  if (num > 0) return num;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_Document))) return 9;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_PhysicallyModeledProductDefinition))) return 10;
  return 0;
}

Handle(StepBasic_Document)  StepAP214_AutoDesignOrganizationItem::Document () const
{  return GetCasted(StepBasic_Document,Value());  }

Handle(StepBasic_PhysicallyModeledProductDefinition)  StepAP214_AutoDesignOrganizationItem::PhysicallyModeledProductDefinition () const
{  return GetCasted(StepBasic_PhysicallyModeledProductDefinition,Value());  }
