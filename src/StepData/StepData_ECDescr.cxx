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


#include <Standard_Type.hxx>
#include <StepData_Described.hxx>
#include <StepData_ECDescr.hxx>
#include <StepData_ESDescr.hxx>
#include <StepData_Plex.hxx>
#include <StepData_Simple.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepData_ECDescr,StepData_EDescr)

StepData_ECDescr::StepData_ECDescr  ()    {  }

    void  StepData_ECDescr::Add (const Handle(StepData_ESDescr)& member)
{
  if (member.IsNull()) return;
  Standard_CString name = member->TypeName();
  TCollection_AsciiString nam (name);
  for (Standard_Integer i = NbMembers(); i > 0; i --) {
    Handle(StepData_ESDescr) mem = Member(i);
    if (nam.IsLess (mem->TypeName())) { thelist.InsertBefore (i,member); return; }
  }
  thelist.Append (member);
}

    Standard_Integer  StepData_ECDescr::NbMembers () const
      {  return thelist.Length();  }

    Handle(StepData_ESDescr)  StepData_ECDescr::Member
  (const Standard_Integer num) const
      {  return Handle(StepData_ESDescr)::DownCast (thelist.Value(num));  }

    Handle(TColStd_HSequenceOfAsciiString)  StepData_ECDescr::TypeList () const
{
  Handle(TColStd_HSequenceOfAsciiString) tl = new TColStd_HSequenceOfAsciiString();
  Standard_Integer i, nb = NbMembers();
  for (i = 1; i <= nb; i ++) {
    TCollection_AsciiString nam (Member(i)->TypeName());
    tl->Append(nam);
  }
  return tl;
}


    Standard_Boolean  StepData_ECDescr::Matches (const Standard_CString name) const
{
  Standard_Integer i, nb = NbMembers();
  for (i = 1; i <= nb; i ++) {
    Handle(StepData_ESDescr) member = Member(i);
    if (member->Matches(name)) return Standard_True;
  }
  return Standard_False;
}

    Standard_Boolean  StepData_ECDescr::IsComplex () const
      {  return Standard_True;  }

    Handle(StepData_Described)  StepData_ECDescr::NewEntity () const
{
  Handle(StepData_Plex) ent = new StepData_Plex (this);
  Standard_Integer i, nb = NbMembers();
  for (i = 1; i <= nb; i ++) {
    Handle(StepData_ESDescr) member = Member(i);
    Handle(StepData_Simple) mem = Handle(StepData_Simple)::DownCast(member->NewEntity());
    if (!mem.IsNull()) ent->Add (mem);
  }
  return ent;
}
