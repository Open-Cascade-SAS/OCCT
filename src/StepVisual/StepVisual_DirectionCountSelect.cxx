#include <StepVisual_DirectionCountSelect.hxx>

StepVisual_DirectionCountSelect::StepVisual_DirectionCountSelect() {}

void StepVisual_DirectionCountSelect::SetTypeOfContent(const Standard_Integer aType)
{
  theTypeOfContent = aType;
}

Standard_Integer StepVisual_DirectionCountSelect::TypeOfContent() const 
{
  return theTypeOfContent;
}

Standard_Integer StepVisual_DirectionCountSelect::UDirectionCount() const
{
  return theUDirectionCount;
}

void StepVisual_DirectionCountSelect::SetUDirectionCount(const Standard_Integer aUDirectionCount)
{
  theUDirectionCount = aUDirectionCount;
  theTypeOfContent   = 1;
}


Standard_Integer StepVisual_DirectionCountSelect::VDirectionCount() const
{
  return theUDirectionCount;
}

void StepVisual_DirectionCountSelect::SetVDirectionCount(const Standard_Integer aVDirectionCount)
{
  theVDirectionCount = aVDirectionCount;
  theTypeOfContent   = 2;
}


