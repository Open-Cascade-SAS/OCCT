// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <Standard_Type.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepShape_MeasureQualification.hxx>
#include <StepShape_ValueQualifier.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_MeasureQualification,Standard_Transient)

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
