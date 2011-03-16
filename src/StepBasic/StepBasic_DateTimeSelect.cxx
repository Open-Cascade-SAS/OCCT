
#include <StepBasic_DateTimeSelect.ixx>
#include <Interface_Macros.hxx>

StepBasic_DateTimeSelect::StepBasic_DateTimeSelect () {  }

Standard_Integer StepBasic_DateTimeSelect::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepBasic_Date))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepBasic_LocalTime))) return 2;
	if (ent->IsKind(STANDARD_TYPE(StepBasic_DateAndTime))) return 3;
	return 0;
}

Handle(StepBasic_Date) StepBasic_DateTimeSelect::Date () const
{
	return GetCasted(StepBasic_Date,Value());
}

Handle(StepBasic_LocalTime) StepBasic_DateTimeSelect::LocalTime () const
{
	return GetCasted(StepBasic_LocalTime,Value());
}

Handle(StepBasic_DateAndTime) StepBasic_DateTimeSelect::DateAndTime () const
{
	return GetCasted(StepBasic_DateAndTime,Value());
}
