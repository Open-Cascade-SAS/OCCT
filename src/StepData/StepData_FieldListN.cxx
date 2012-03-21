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
     
