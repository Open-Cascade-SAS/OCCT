#include <StepData_SelectInt.ixx>

StepData_SelectInt::StepData_SelectInt  ()    {  thekind = 0;  theval = 0;  }

Standard_Integer  StepData_SelectInt::Kind () const    {  return thekind;  }
void  StepData_SelectInt::SetKind (const Standard_Integer kind)    {  thekind = kind;  }

Standard_Integer  StepData_SelectInt::Int () const    {  return theval;  }
void  StepData_SelectInt::SetInt (const Standard_Integer val)    {  theval = val;  }
