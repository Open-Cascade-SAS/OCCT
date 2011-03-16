#include <StepShape_TypeQualifier.ixx>

StepShape_TypeQualifier::StepShape_TypeQualifier  ()    {  }

void  StepShape_TypeQualifier::Init
  (const Handle(TCollection_HAsciiString)& name)
{  theName = name;  }

Handle(TCollection_HAsciiString)  StepShape_TypeQualifier::Name () const
{  return theName; }

void  StepShape_TypeQualifier::SetName
  (const Handle(TCollection_HAsciiString)& name)
{  theName = name;  }
