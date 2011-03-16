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
