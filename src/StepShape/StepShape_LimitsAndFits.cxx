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

#include <StepShape_LimitsAndFits.ixx>

StepShape_LimitsAndFits::StepShape_LimitsAndFits ()    {  }

void  StepShape_LimitsAndFits::Init
  (const Handle(TCollection_HAsciiString)& form_variance,
   const Handle(TCollection_HAsciiString)& zone_variance,
   const Handle(TCollection_HAsciiString)& grade,
   const Handle(TCollection_HAsciiString)& source)
{
  theFormVariance = form_variance;
  theZoneVariance = zone_variance;
  theGrade = grade;
  theSource = source;
}

Handle(TCollection_HAsciiString)  StepShape_LimitsAndFits::FormVariance () const
{  return theFormVariance;  }

void  StepShape_LimitsAndFits::SetFormVariance
  (const Handle(TCollection_HAsciiString)& form_variance)
{  theFormVariance = form_variance;  }

Handle(TCollection_HAsciiString)  StepShape_LimitsAndFits::ZoneVariance () const
{  return theZoneVariance;  }

void  StepShape_LimitsAndFits::SetZoneVariance
  (const Handle(TCollection_HAsciiString)& zone_variance)
{  theZoneVariance = zone_variance;  }

Handle(TCollection_HAsciiString)  StepShape_LimitsAndFits::Grade () const
{  return theGrade;  }

void  StepShape_LimitsAndFits::SetGrade
  (const Handle(TCollection_HAsciiString)& grade)
{  theGrade = grade;  }

Handle(TCollection_HAsciiString)  StepShape_LimitsAndFits::Source () const
{  return theSource;  }

void  StepShape_LimitsAndFits::SetSource
  (const Handle(TCollection_HAsciiString)& source)
{  theSource = source;  }
