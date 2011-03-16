
#include <StepVisual_InvisibilityContext.ixx>
#include <Interface_Macros.hxx>

StepVisual_InvisibilityContext::StepVisual_InvisibilityContext () {  }

Standard_Integer StepVisual_InvisibilityContext::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_PresentationRepresentation))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_PresentationSet))) return 2;
	return 0;
}

Handle(StepVisual_PresentationRepresentation) StepVisual_InvisibilityContext::PresentationRepresentation () const
{
	return GetCasted(StepVisual_PresentationRepresentation,Value());
}

Handle(StepVisual_PresentationSet) StepVisual_InvisibilityContext::PresentationSet () const
{
	return GetCasted(StepVisual_PresentationSet,Value());
}
