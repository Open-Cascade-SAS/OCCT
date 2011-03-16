#include <StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem.ixx>

StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem::StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem  ()
{
  myMeasure = new StepBasic_MeasureWithUnit;
}

void  StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem::Init
  (const Handle(TCollection_HAsciiString)& aName,
   const Handle(StepBasic_MeasureValueMember)& aValueComponent,
   const StepBasic_Unit& aUnitComponent,
   const Handle(StepShape_HArray1OfValueQualifier)& qualifiers)
{
  StepRepr_RepresentationItem::Init (aName);
  myMeasure->Init ( aValueComponent, aUnitComponent );
  theQualifiers = qualifiers;
}


void StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem::SetMeasure (const Handle(StepBasic_MeasureWithUnit)& Measure)
{  myMeasure = Measure;  }

Handle(StepBasic_MeasureWithUnit) StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem::Measure () const
{  return myMeasure;  }



Handle(StepShape_HArray1OfValueQualifier)  StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem::Qualifiers () const
{  return theQualifiers;  }

Standard_Integer  StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem::NbQualifiers () const
{  return theQualifiers->Length();  }

void  StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem::SetQualifiers
  (const Handle(StepShape_HArray1OfValueQualifier)& qualifiers)
{  theQualifiers = qualifiers;  }

StepShape_ValueQualifier  StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem::QualifiersValue
  (const Standard_Integer num) const
{  return theQualifiers->Value(num);  }

void  StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem::SetQualifiersValue
  (const Standard_Integer num, const StepShape_ValueQualifier& aqualifier)
{  theQualifiers->SetValue (num,aqualifier);  }
