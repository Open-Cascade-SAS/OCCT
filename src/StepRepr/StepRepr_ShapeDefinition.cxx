
#include <StepRepr_ShapeDefinition.ixx>
#include <Interface_Macros.hxx>

StepRepr_ShapeDefinition::StepRepr_ShapeDefinition () {  }

Standard_Integer StepRepr_ShapeDefinition::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepRepr_ProductDefinitionShape))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepRepr_ShapeAspect))) return 2;
	if (ent->IsKind(STANDARD_TYPE(StepRepr_ShapeAspectRelationship))) return 3;
	return 0;
}

Handle(StepRepr_ProductDefinitionShape) StepRepr_ShapeDefinition::ProductDefinitionShape () const
{
	return GetCasted(StepRepr_ProductDefinitionShape,Value());
}

Handle(StepRepr_ShapeAspect) StepRepr_ShapeDefinition::ShapeAspect () const
{
	return GetCasted(StepRepr_ShapeAspect,Value());
}

Handle(StepRepr_ShapeAspectRelationship) StepRepr_ShapeDefinition::ShapeAspectRelationship () const
{
	return GetCasted(StepRepr_ShapeAspectRelationship,Value());
}
