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
