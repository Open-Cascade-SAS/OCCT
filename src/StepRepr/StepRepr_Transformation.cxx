
#include <StepRepr_Transformation.ixx>
#include <Interface_Macros.hxx>

StepRepr_Transformation::StepRepr_Transformation () {  }

Standard_Integer StepRepr_Transformation::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepRepr_ItemDefinedTransformation))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepRepr_FunctionallyDefinedTransformation))) return 2;
	return 0;
}

Handle(StepRepr_ItemDefinedTransformation) StepRepr_Transformation::ItemDefinedTransformation () const
{
	return GetCasted(StepRepr_ItemDefinedTransformation,Value());
}

Handle(StepRepr_FunctionallyDefinedTransformation) StepRepr_Transformation::FunctionallyDefinedTransformation () const
{
	return GetCasted(StepRepr_FunctionallyDefinedTransformation,Value());
}
