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
     
