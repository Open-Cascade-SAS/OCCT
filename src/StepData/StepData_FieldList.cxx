#include <StepData_FieldList.ixx>
#include <Standard_OutOfRange.hxx>


static StepData_Field nulfild;

StepData_FieldList::StepData_FieldList  ()    {  }

Standard_Integer  StepData_FieldList::NbFields () const
{  return 0;  }

const StepData_Field&  StepData_FieldList::Field (const Standard_Integer num) const
{
  Standard_OutOfRange::Raise("StepData_FieldList : Field");
  return nulfild;
}

StepData_Field&  StepData_FieldList::CField (const Standard_Integer num)
{
  Standard_OutOfRange::Raise("StepData_FieldList : CField");
  return nulfild;
}


void  StepData_FieldList::FillShared (Interface_EntityIterator& iter) const
{
  Standard_Integer i, nb = NbFields();
  for (i = 1; i <= nb; i ++) {
    const StepData_Field& fi = Field(i);
    if (fi.Kind() != 7) continue;  // KindEntity
    Standard_Integer i1,i2, nb1 = 1, nb2 = 1, ari = fi.Arity();
    if (ari == 1)   nb1 = fi.Length();
    if (ari == 2) { nb1 = fi.Length(1); nb2 = fi.Length(2); }
    for (i1 = 1; i1 <= nb1; i1 ++)
      for (i2 = 1; i2 <= nb2; i2 ++)
	iter.AddItem (fi.Entity(i1,i2));
  }
}
