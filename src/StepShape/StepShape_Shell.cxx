
#include <StepShape_Shell.ixx>
#include <Interface_Macros.hxx>

StepShape_Shell::StepShape_Shell () {  }

Standard_Integer StepShape_Shell::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepShape_OpenShell))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepShape_ClosedShell))) return 2;
	return 0;
}

Handle(StepShape_OpenShell) StepShape_Shell::OpenShell () const
{
	return GetCasted(StepShape_OpenShell,Value());
}

Handle(StepShape_ClosedShell) StepShape_Shell::ClosedShell () const
{
	return GetCasted(StepShape_ClosedShell,Value());
}
