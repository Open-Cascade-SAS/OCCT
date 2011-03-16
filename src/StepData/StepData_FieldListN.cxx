#include <StepData_FieldListN.ixx>
#include <Standard_OutOfRange.hxx>


StepData_FieldListN::StepData_FieldListN  (const Standard_Integer nb)
:  thefields ( (nb == 0 ? 0 : 1),nb)    {  }

Standard_Integer  StepData_FieldListN::NbFields () const
{  return thefields.Upper();  }

const StepData_Field&  StepData_FieldListN::Field (const Standard_Integer num) const
{
  return thefields.Value(num);
}

StepData_Field&  StepData_FieldListN::CField (const Standard_Integer num)
{
  return thefields.ChangeValue(num);
}

void StepData_FieldListN::Destroy() {}
     
