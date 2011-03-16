#include <StepShape_PlusMinusTolerance.ixx>

StepShape_PlusMinusTolerance::StepShape_PlusMinusTolerance  ()    {  }

void  StepShape_PlusMinusTolerance::Init
  (const StepShape_ToleranceMethodDefinition& range,
   const StepShape_DimensionalCharacteristic& toleranced_dimension)
{
  theRange = range;
  theTolerancedDimension = toleranced_dimension;
}

StepShape_ToleranceMethodDefinition  StepShape_PlusMinusTolerance::Range () const
{  return theRange;  }

void  StepShape_PlusMinusTolerance::SetRange (const StepShape_ToleranceMethodDefinition& range)
{  theRange = range;  }

StepShape_DimensionalCharacteristic  StepShape_PlusMinusTolerance::TolerancedDimension () const
{  return theTolerancedDimension;  }

void  StepShape_PlusMinusTolerance::SetTolerancedDimension (const StepShape_DimensionalCharacteristic& toleranced_dimension)
{  theTolerancedDimension = toleranced_dimension;  }
