#include <StepShape_QualifiedRepresentationItem.ixx>

StepShape_QualifiedRepresentationItem::StepShape_QualifiedRepresentationItem  ()    {  }

void  StepShape_QualifiedRepresentationItem::Init
  (const Handle(TCollection_HAsciiString)& aName,
   const Handle(StepShape_HArray1OfValueQualifier)& qualifiers)
{
  StepRepr_RepresentationItem::Init (aName);
  theQualifiers = qualifiers;
}


Handle(StepShape_HArray1OfValueQualifier)  StepShape_QualifiedRepresentationItem::Qualifiers () const
{  return theQualifiers;  }

Standard_Integer  StepShape_QualifiedRepresentationItem::NbQualifiers () const
{  return theQualifiers->Length();  }

void  StepShape_QualifiedRepresentationItem::SetQualifiers
  (const Handle(StepShape_HArray1OfValueQualifier)& qualifiers)
{  theQualifiers = qualifiers;  }

StepShape_ValueQualifier  StepShape_QualifiedRepresentationItem::QualifiersValue
  (const Standard_Integer num) const
{  return theQualifiers->Value(num);  }

void  StepShape_QualifiedRepresentationItem::SetQualifiersValue
  (const Standard_Integer num, const StepShape_ValueQualifier& aqualifier)
{  theQualifiers->SetValue (num,aqualifier);  }
