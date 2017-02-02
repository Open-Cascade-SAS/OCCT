// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_InterfaceMismatch.hxx>
#include <Standard_Type.hxx>
#include <StepData_ESDescr.hxx>
#include <StepData_Field.hxx>
#include <StepData_FieldListN.hxx>
#include <StepData_Simple.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepData_Simple,StepData_Described)

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
  if (num == 0) throw Interface_InterfaceMismatch("StepData_Simple : Field");
  return FieldNum (num);
}

    StepData_Field&  StepData_Simple::CField (const Standard_CString name)
{
  Standard_Integer num = ESDescr()->Rank (name);
  if (num == 0) throw Interface_InterfaceMismatch("StepData_Simple : Field");
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


void StepData_Simple::Check(Handle(Interface_Check)& /*ach*/) const
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
