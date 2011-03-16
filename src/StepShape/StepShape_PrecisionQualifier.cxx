#include <StepShape_PrecisionQualifier.ixx>

StepShape_PrecisionQualifier::StepShape_PrecisionQualifier  ()    {  }

void  StepShape_PrecisionQualifier::Init
  (const Standard_Integer precision_value)
{  thePrecisionValue = precision_value;  }

Standard_Integer  StepShape_PrecisionQualifier::PrecisionValue () const
{  return thePrecisionValue; }

void  StepShape_PrecisionQualifier::SetPrecisionValue
  (const Standard_Integer precision_value)
{  thePrecisionValue = precision_value;  }
