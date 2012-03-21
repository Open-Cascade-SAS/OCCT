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

#include <StepData_Plex.ixx>
#include <TCollection_AsciiString.hxx>
#include <Interface_InterfaceMismatch.hxx>


static StepData_Field bid;

    StepData_Plex::StepData_Plex  (const Handle(StepData_ECDescr)& descr)
    : StepData_Described (descr)    {  }

    void  StepData_Plex::Add (const Handle(StepData_Simple)& member)
      {  themembers.Append (member);  }

    Handle(StepData_ECDescr)  StepData_Plex::ECDescr () const
      {  return Handle(StepData_ECDescr)::DownCast(Description());  }


    Standard_Boolean  StepData_Plex::IsComplex () const
      {  return Standard_True;  }

    Standard_Boolean  StepData_Plex::Matches (const Standard_CString steptype) const
{
  Standard_Integer i, nb = NbMembers();
  for (i = 1; i <= nb; i ++) {
    if (Member(i)->Matches (steptype)) return Standard_True;
  }
  return Standard_False;
}

    Handle(StepData_Simple)  StepData_Plex::As  (const Standard_CString steptype) const
{
  Handle(StepData_Simple) ent;
  Standard_Integer i, nb = NbMembers();
  for (i = 1; i <= nb; i ++) {
    ent = Member(i);
    if (ent->Matches(steptype)) return ent;
  }
  ent.Nullify();
  return ent;
}

    Standard_Boolean  StepData_Plex::HasField (const Standard_CString name) const
{
  Standard_Integer i, nb = NbMembers();
  for (i = 1; i <= nb; i ++) {
    if (Member(i)->HasField (name)) return Standard_True;
  }
  return Standard_False;
}

    const StepData_Field&  StepData_Plex::Field (const Standard_CString name) const
{
  Handle(StepData_Simple) ent;
  Standard_Integer i, nb = NbMembers();
  for (i = 1; i <= nb; i ++) {
    ent = Member(i);
    if (ent->HasField (name)) return ent->Field (name);
  }
  Interface_InterfaceMismatch::Raise("StepData_Plex : Field");
  return bid;  // pour faire plaisir au compilo
}


    StepData_Field&  StepData_Plex::CField (const Standard_CString name)
{
  Handle(StepData_Simple) ent;
  Standard_Integer i, nb = NbMembers();
  for (i = 1; i <= nb; i ++) {
    ent = Member(i);
    if (ent->HasField (name)) return ent->CField (name);
  }
  Interface_InterfaceMismatch::Raise("StepData_Plex : Field");
  return bid;  // pour faire plaisir au compilo
}


    Standard_Integer  StepData_Plex::NbMembers () const
      {  return themembers.Length();  }

    Handle(StepData_Simple)  StepData_Plex::Member (const Standard_Integer num) const
      {  return Handle(StepData_Simple)::DownCast (themembers.Value(num));  }

    Handle(TColStd_HSequenceOfAsciiString)  StepData_Plex::TypeList () const
{
  Handle(TColStd_HSequenceOfAsciiString) tl = new TColStd_HSequenceOfAsciiString();
  Standard_Integer i, nb = NbMembers();
  for (i = 1; i <= nb; i ++) {
    TCollection_AsciiString nam (Member(i)->StepType());
    tl->Append (nam);
  }
  return tl;
}


void StepData_Plex::Check(Handle(Interface_Check)& ach) const
{
  Standard_Integer i, nb = NbMembers();
  for (i = 1; i <= nb; i ++) {
    Handle(StepData_Simple) si = Member(i);
    si->Check(ach);
  }
}


    void  StepData_Plex::Shared (Interface_EntityIterator& list) const
{
  Standard_Integer i, nb = NbMembers();
  for (i = 1; i <= nb; i ++) {
    Handle(StepData_Simple) si = Member(i);
    si->Shared (list);
  }
}
