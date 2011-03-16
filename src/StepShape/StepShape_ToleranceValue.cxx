#include <StepShape_ToleranceValue.ixx>

StepShape_ToleranceValue::StepShape_ToleranceValue  ()    {  }

void  StepShape_ToleranceValue::Init
  (const Handle(StepBasic_MeasureWithUnit)& lower_bound,
   const Handle(StepBasic_MeasureWithUnit)& upper_bound)
{
  theLowerBound = lower_bound;
  theUpperBound = upper_bound;
}

Handle(StepBasic_MeasureWithUnit)  StepShape_ToleranceValue::LowerBound () const
{  return theLowerBound;  }

void  StepShape_ToleranceValue::SetLowerBound (const Handle(StepBasic_MeasureWithUnit)& lower_bound)
{  theLowerBound = lower_bound;  }

Handle(StepBasic_MeasureWithUnit)  StepShape_ToleranceValue::UpperBound () const
{  return theUpperBound;  }

void  StepShape_ToleranceValue::SetUpperBound (const Handle(StepBasic_MeasureWithUnit)& upper_bound)
{  theUpperBound = upper_bound;  }
