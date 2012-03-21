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

#include <StepData_ECDescr.ixx>
#include <TCollection_AsciiString.hxx>
#include <StepData_Plex.hxx>

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
