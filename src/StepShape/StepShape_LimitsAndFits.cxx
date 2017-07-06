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
#include <StepShape_LimitsAndFits.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_LimitsAndFits,Standard_Transient)

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
