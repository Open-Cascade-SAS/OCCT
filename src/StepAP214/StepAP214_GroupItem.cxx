
#include <StepAP214_GroupItem.ixx>
#include <Interface_Macros.hxx>

StepAP214_GroupItem::StepAP214_GroupItem () {  }

Standard_Integer StepAP214_GroupItem::CaseNum(const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepGeom_GeometricRepresentationItem))) return 1;
  
  return 0;
}

Handle(StepGeom_GeometricRepresentationItem) StepAP214_GroupItem::GeometricRepresentationItem() const
{
  return GetCasted(StepGeom_GeometricRepresentationItem,Value());
}

