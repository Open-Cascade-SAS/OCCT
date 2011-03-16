#include <StepData_FieldList1.ixx>
#include <Standard_OutOfRange.hxx>


StepData_FieldList1::StepData_FieldList1  ()    {  }

Standard_Integer  StepData_FieldList1::NbFields () const
{  return 1;  }

const StepData_Field&  StepData_FieldList1::Field (const Standard_Integer num) const
{
  if (num != 1) Standard_OutOfRange::Raise("StepData_FieldList1 : Field");
  return thefield;
}

StepData_Field&  StepData_FieldList1::CField (const Standard_Integer num)
{
  if (num != 1) Standard_OutOfRange::Raise("StepData_FieldList1 : CField");
  return thefield;
}

void StepData_FieldList1::Destroy () {}
     
