#include <StepBasic_Effectivity.ixx>

StepBasic_Effectivity::StepBasic_Effectivity  ()    {  }

void  StepBasic_Effectivity::Init (const Handle(TCollection_HAsciiString)& aid)
{  theid = aid;  }

Handle(TCollection_HAsciiString)  StepBasic_Effectivity::Id () const
{  return theid;  }

void  StepBasic_Effectivity::SetId (const Handle(TCollection_HAsciiString)& aid)
{  theid = aid;  }
