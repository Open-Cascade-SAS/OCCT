#include <StepData_FieldListD.ixx>
#include <Standard_OutOfRange.hxx>


StepData_FieldListD::StepData_FieldListD  (const Standard_Integer nb)
{  if (nb > 0) thefields = new StepData_HArray1OfField (1,nb);  }

void  StepData_FieldListD::SetNb (const Standard_Integer nb)
{
  thefields.Nullify();
  if (nb > 0) thefields = new StepData_HArray1OfField (1,nb);
}

Standard_Integer  StepData_FieldListD::NbFields () const
{  return (thefields.IsNull() ? 0 : thefields->Length());  }

const StepData_Field&  StepData_FieldListD::Field (const Standard_Integer num) const
{
  if (thefields.IsNull()) Standard_OutOfRange::Raise("StepData_FieldListD::Field");
  return thefields->Value(num);
}

StepData_Field&  StepData_FieldListD::CField (const Standard_Integer num)
{
  if (thefields.IsNull()) Standard_OutOfRange::Raise("StepData_FieldListD::Field");
  return thefields->ChangeValue(num);
}

void StepData_FieldListD::Destroy () {}
     
