
#include <StepVisual_PresentationSizeAssignmentSelect.ixx>
#include <Interface_Macros.hxx>

StepVisual_PresentationSizeAssignmentSelect::StepVisual_PresentationSizeAssignmentSelect () {  }

Standard_Integer StepVisual_PresentationSizeAssignmentSelect::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_PresentationView))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_PresentationArea))) return 2;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_AreaInSet))) return 3;
	return 0;
}

Handle(StepVisual_PresentationView) StepVisual_PresentationSizeAssignmentSelect::PresentationView () const
{
	return GetCasted(StepVisual_PresentationView,Value());
}

Handle(StepVisual_PresentationArea) StepVisual_PresentationSizeAssignmentSelect::PresentationArea () const
{
	return GetCasted(StepVisual_PresentationArea,Value());
}

Handle(StepVisual_AreaInSet) StepVisual_PresentationSizeAssignmentSelect::AreaInSet () const
{
	return GetCasted(StepVisual_AreaInSet,Value());
}
