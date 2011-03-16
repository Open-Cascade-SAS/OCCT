
#include <StepVisual_TextOrCharacter.ixx>
#include <Interface_Macros.hxx>

StepVisual_TextOrCharacter::StepVisual_TextOrCharacter () {  }

Standard_Integer StepVisual_TextOrCharacter::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_AnnotationText))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_CompositeText))) return 2;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_TextLiteral))) return 3;
	return 0;
}

Handle(StepVisual_AnnotationText) StepVisual_TextOrCharacter::AnnotationText () const
{
	return GetCasted(StepVisual_AnnotationText,Value());
}

Handle(StepVisual_CompositeText) StepVisual_TextOrCharacter::CompositeText () const
{
	return GetCasted(StepVisual_CompositeText,Value());
}

Handle(StepVisual_TextLiteral) StepVisual_TextOrCharacter::TextLiteral () const
{
	return GetCasted(StepVisual_TextLiteral,Value());
}
