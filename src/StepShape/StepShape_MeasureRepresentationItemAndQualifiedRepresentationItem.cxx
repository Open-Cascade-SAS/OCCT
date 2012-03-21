// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
