
#include <StepVisual_StyleContextSelect.ixx>
#include <Interface_Macros.hxx>

StepVisual_StyleContextSelect::StepVisual_StyleContextSelect () {  }

Standard_Integer StepVisual_StyleContextSelect::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepRepr_Representation))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepRepr_RepresentationItem))) return 2;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_PresentationSet))) return 3;
	return 0;
}

Handle(StepRepr_Representation) StepVisual_StyleContextSelect::Representation () const
{
	return GetCasted(StepRepr_Representation,Value());
}

Handle(StepRepr_RepresentationItem) StepVisual_StyleContextSelect::RepresentationItem () const
{
	return GetCasted(StepRepr_RepresentationItem,Value());
}

Handle(StepVisual_PresentationSet) StepVisual_StyleContextSelect::PresentationSet () const
{
	return GetCasted(StepVisual_PresentationSet,Value());
}
