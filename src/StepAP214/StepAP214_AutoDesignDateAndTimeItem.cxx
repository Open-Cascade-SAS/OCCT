
#include <StepAP214_AutoDesignDateAndTimeItem.ixx>
#include <Interface_Macros.hxx>

StepAP214_AutoDesignDateAndTimeItem::StepAP214_AutoDesignDateAndTimeItem () {  }

Standard_Integer StepAP214_AutoDesignDateAndTimeItem::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepBasic_ApprovalPersonOrganization))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepAP214_AutoDesignDateAndPersonAssignment))) return 2;
	if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionEffectivity))) return 3;
	return 0;
}

Handle(StepBasic_ApprovalPersonOrganization) StepAP214_AutoDesignDateAndTimeItem::ApprovalPersonOrganization () const
{
	return GetCasted(StepBasic_ApprovalPersonOrganization,Value());
}

Handle(StepAP214_AutoDesignDateAndPersonAssignment) StepAP214_AutoDesignDateAndTimeItem::AutoDesignDateAndPersonAssignment () const
{
	return GetCasted(StepAP214_AutoDesignDateAndPersonAssignment,Value());
}

Handle(StepBasic_ProductDefinitionEffectivity)  StepAP214_AutoDesignDateAndTimeItem::ProductDefinitionEffectivity () const
{  return GetCasted(StepBasic_ProductDefinitionEffectivity,Value());  }
