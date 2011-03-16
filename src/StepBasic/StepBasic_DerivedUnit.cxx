#include <StepBasic_DerivedUnit.ixx>

StepBasic_DerivedUnit::StepBasic_DerivedUnit  ()    {  }

void  StepBasic_DerivedUnit::Init (const Handle(StepBasic_HArray1OfDerivedUnitElement)& elements)
{  theElements = elements;  }

void  StepBasic_DerivedUnit::SetElements (const Handle(StepBasic_HArray1OfDerivedUnitElement)& elements)
{  theElements = elements;  }

Handle(StepBasic_HArray1OfDerivedUnitElement)  StepBasic_DerivedUnit::Elements () const
{  return theElements;  }

Standard_Integer  StepBasic_DerivedUnit::NbElements () const
{  return theElements->Length();  }

Handle(StepBasic_DerivedUnitElement)  StepBasic_DerivedUnit::ElementsValue (const Standard_Integer num) const
{  return theElements->Value(num);  }
