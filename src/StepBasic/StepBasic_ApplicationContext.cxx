#include <StepBasic_ApplicationContext.ixx>


StepBasic_ApplicationContext::StepBasic_ApplicationContext ()  {}

void StepBasic_ApplicationContext::Init(
	const Handle(TCollection_HAsciiString)& aApplication)
{
	// --- classe own fields ---
	application = aApplication;
}


void StepBasic_ApplicationContext::SetApplication(const Handle(TCollection_HAsciiString)& aApplication)
{
	application = aApplication;
}

Handle(TCollection_HAsciiString) StepBasic_ApplicationContext::Application() const
{
	return application;
}
