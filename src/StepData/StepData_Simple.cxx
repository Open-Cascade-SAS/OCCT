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

#include <StepData_Simple.ixx>
#include <Interface_InterfaceMismatch.hxx>

StepData_Simple::StepData_Simple (const Handle(StepData_ESDescr)& descr)
    : StepData_Described (descr) , thefields (descr->NbFields())    {  }

    Handle(StepData_ESDescr)  StepData_Simple::ESDescr () const
      {  return Handle(StepData_ESDescr)::DownCast (Description());  }

    Standard_CString  StepData_Simple::StepType () const
      {  return ESDescr()->TypeName();  }


    Standard_Boolean  StepData_Simple::IsComplex () const
      {  return Standard_False;  }

    Standard_Boolean  StepData_Simple::Matches (const Standard_CString steptype) const
      {  return ESDescr()->Matches (steptype);  }

    Handle(StepData_Simple)  StepData_Simple::As (const Standard_CString steptype) const
{
  Handle(StepData_Simple) nulent;
  if (Matches(steptype)) return this;
  return nulent;
}


    Standard_Boolean  StepData_Simple::HasField (const Standard_CString name) const
{
  Standard_Integer num = ESDescr()->Rank (name);
  return (num > 0);
}

    const StepData_Field&  StepData_Simple::Field (const Standard_CString name) const
{
  Standard_Integer num = ESDescr()->Rank (name);
  if (num == 0) Interface_InterfaceMismatch::Raise("StepData_Simple : Field");
  return FieldNum (num);
}

    StepData_Field&  StepData_Simple::CField (const Standard_CString name)
{
  Standard_Integer num = ESDescr()->Rank (name);
  if (num == 0) Interface_InterfaceMismatch::Raise("StepData_Simple : Field");
  return CFieldNum (num);
}

    Standard_Integer  StepData_Simple::NbFields () const
      {  return thefields.NbFields();  }

    const StepData_Field&  StepData_Simple::FieldNum (const Standard_Integer num) const
      {  return thefields.Field(num);  }

    StepData_Field&  StepData_Simple::CFieldNum (const Standard_Integer num)
      {  return thefields.CField(num);  }

    const StepData_FieldListN&  StepData_Simple::Fields () const
      {  return thefields;  }

    StepData_FieldListN&  StepData_Simple::CFields ()
      {  return thefields;  }


void StepData_Simple::Check(Handle(Interface_Check)& ach) const
{
}  // qq chose ? cf la description


    void  StepData_Simple::Shared (Interface_EntityIterator& list) const
{
  Standard_Integer i, nb = thefields.NbFields();
  for (i = 1; i <= nb; i ++) {
    const StepData_Field& fi = thefields.Field(i);
    Standard_Integer j1,j2,l1,l2;  l1 = l2 = 1;
    if (fi.Arity() >= 1) l1 = fi.Length(1);
    if (fi.Arity() >  1) l2 = fi.Length(2);
    for (j1 = 1; j1 <= l1; j1 ++) {
      for (j2 = 1; j2 <= l2; j2 ++) {
	Handle(Standard_Transient) ent = fi.Entity(j1,j2);
	if (!ent.IsNull()) list.AddItem(ent);
      }
    }
  }
}
