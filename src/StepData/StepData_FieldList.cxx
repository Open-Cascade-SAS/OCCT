// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
