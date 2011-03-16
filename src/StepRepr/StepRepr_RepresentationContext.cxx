#include <StepRepr_RepresentationContext.ixx>


StepRepr_RepresentationContext::StepRepr_RepresentationContext ()  {}

void StepRepr_RepresentationContext::Init(
	const Handle(TCollection_HAsciiString)& aContextIdentifier,
	const Handle(TCollection_HAsciiString)& aContextType)
{
	// --- classe own fields ---
	contextIdentifier = aContextIdentifier;
	contextType = aContextType;
}


void StepRepr_RepresentationContext::SetContextIdentifier(const Handle(TCollection_HAsciiString)& aContextIdentifier)
{
	contextIdentifier = aContextIdentifier;
}

Handle(TCollection_HAsciiString) StepRepr_RepresentationContext::ContextIdentifier() const
{
	return contextIdentifier;
}

void StepRepr_RepresentationContext::SetContextType(const Handle(TCollection_HAsciiString)& aContextType)
{
	contextType = aContextType;
}

Handle(TCollection_HAsciiString) StepRepr_RepresentationContext::ContextType() const
{
	return contextType;
}
