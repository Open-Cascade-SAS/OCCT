#include <StepShape_MeasureQualification.ixx>

StepShape_MeasureQualification::StepShape_MeasureQualification  ()    {  }

void  StepShape_MeasureQualification::Init
  (const Handle(TCollection_HAsciiString)& name,
   const Handle(TCollection_HAsciiString)& description,
   const Handle(StepBasic_MeasureWithUnit)& qualified_measure,
   const Handle(StepShape_HArray1OfValueQualifier)& qualifiers)
{
  theName = name;
  theDescription = description;
  theQualifiedMeasure = qualified_measure;
  theQualifiers = qualifiers;
}

Handle(TCollection_HAsciiString)  StepShape_MeasureQualification::Name () const
{  return theName;  }

void  StepShape_MeasureQualification::SetName
  (const Handle(TCollection_HAsciiString)& name)
{  theName = name;  }

Handle(TCollection_HAsciiString)  StepShape_MeasureQualification::Description () const
{  return theDescription;  }

void  StepShape_MeasureQualification::SetDescription
  (const Handle(TCollection_HAsciiString)& description)
{  theDescription = description;  }

Handle(StepBasic_MeasureWithUnit)  StepShape_MeasureQualification::QualifiedMeasure () const
{  return theQualifiedMeasure;  }

void  StepShape_MeasureQualification::SetQualifiedMeasure
  (const Handle(StepBasic_MeasureWithUnit)& qualified_measure)
{  theQualifiedMeasure = qualified_measure;  }

Handle(StepShape_HArray1OfValueQualifier)  StepShape_MeasureQualification::Qualifiers () const
{  return theQualifiers;  }

Standard_Integer  StepShape_MeasureQualification::NbQualifiers () const
{  return theQualifiers->Length();  }

void  StepShape_MeasureQualification::SetQualifiers
  (const Handle(StepShape_HArray1OfValueQualifier)& qualifiers)
{  theQualifiers = qualifiers;  }

StepShape_ValueQualifier  StepShape_MeasureQualification::QualifiersValue
  (const Standard_Integer num) const
{  return theQualifiers->Value(num);  }

void  StepShape_MeasureQualification::SetQualifiersValue
  (const Standard_Integer num, const StepShape_ValueQualifier& aqualifier)
{  theQualifiers->SetValue (num,aqualifier);  }
