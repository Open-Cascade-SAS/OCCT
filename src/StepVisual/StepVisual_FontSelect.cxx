
#include <StepVisual_FontSelect.ixx>
#include <Interface_Macros.hxx>

StepVisual_FontSelect::StepVisual_FontSelect () {  }

Standard_Integer StepVisual_FontSelect::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_PreDefinedTextFont))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_ExternallyDefinedTextFont))) return 2;
	return 0;
}

Handle(StepVisual_PreDefinedTextFont) StepVisual_FontSelect::PreDefinedTextFont () const
{
	return GetCasted(StepVisual_PreDefinedTextFont,Value());
}

Handle(StepVisual_ExternallyDefinedTextFont) StepVisual_FontSelect::ExternallyDefinedTextFont () const
{
	return GetCasted(StepVisual_ExternallyDefinedTextFont,Value());
}
