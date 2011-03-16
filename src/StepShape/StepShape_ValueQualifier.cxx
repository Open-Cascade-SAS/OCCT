#include <StepShape_ValueQualifier.ixx>

StepShape_ValueQualifier::StepShape_ValueQualifier  ()    {  }

Standard_Integer  StepShape_ValueQualifier::CaseNum
  (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepShape_PrecisionQualifier))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepShape_TypeQualifier))) return 2;
  return 0;
}

Handle(StepShape_PrecisionQualifier)  StepShape_ValueQualifier::PrecisionQualifier () const
{  return Handle(StepShape_PrecisionQualifier)::DownCast(Value());  }

Handle(StepShape_TypeQualifier)  StepShape_ValueQualifier::TypeQualifier () const
{  return Handle(StepShape_TypeQualifier)::DownCast(Value());  }
