#include <StepData_Described.ixx>

StepData_Described::StepData_Described  (const Handle(StepData_EDescr)& descr)
: thedescr (descr)    {  }

Handle(StepData_EDescr)  StepData_Described::Description () const
{  return thedescr;  }
