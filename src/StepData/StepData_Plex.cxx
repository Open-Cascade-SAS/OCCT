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
#include <StepData_ECDescr.hxx>
#include <StepData_Field.hxx>
#include <StepData_Plex.hxx>
#include <StepData_Simple.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepData_Plex,StepData_Described)

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
  throw Interface_InterfaceMismatch("StepData_Plex : Field");
}


    StepData_Field&  StepData_Plex::CField (const Standard_CString name)
{
  Handle(StepData_Simple) ent;
  Standard_Integer i, nb = NbMembers();
  for (i = 1; i <= nb; i ++) {
    ent = Member(i);
    if (ent->HasField (name)) return ent->CField (name);
  }
  throw Interface_InterfaceMismatch("StepData_Plex : Field");
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
