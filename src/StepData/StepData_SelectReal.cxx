#include <StepData_SelectReal.ixx>

//  Definitions : cf Field
#define KindReal 5

StepData_SelectReal::StepData_SelectReal  ()    {  theval = 0.0;  }

Standard_Integer  StepData_SelectReal::Kind () const    {  return KindReal;  }

Standard_Real  StepData_SelectReal::Real () const    { return theval;  }
void  StepData_SelectReal::SetReal (const Standard_Real val)    {  theval = val;  }
