
#include <StepVisual_PresentationRepresentationSelect.ixx>
#include <Interface_Macros.hxx>

StepVisual_PresentationRepresentationSelect::StepVisual_PresentationRepresentationSelect () {  }

Standard_Integer StepVisual_PresentationRepresentationSelect::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_PresentationRepresentation))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_PresentationSet))) return 2;
	return 0;
}

Handle(StepVisual_PresentationRepresentation) StepVisual_PresentationRepresentationSelect::PresentationRepresentation () const
{
	return GetCasted(StepVisual_PresentationRepresentation,Value());
}

Handle(StepVisual_PresentationSet) StepVisual_PresentationRepresentationSelect::PresentationSet () const
{
	return GetCasted(StepVisual_PresentationSet,Value());
}
