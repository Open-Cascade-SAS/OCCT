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
#include <StepData_Plex.hxx>
#include <StepData_Simple.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepData_Plex, StepData_Described)

static StepData_Field bid;

StepData_Plex::StepData_Plex(const occ::handle<StepData_ECDescr>& descr)
    : StepData_Described(descr)
{
}

void StepData_Plex::Add(const occ::handle<StepData_Simple>& member)
{
  themembers.Append(member);
}

occ::handle<StepData_ECDescr> StepData_Plex::ECDescr() const
{
  return occ::down_cast<StepData_ECDescr>(Description());
}

bool StepData_Plex::IsComplex() const
{
  return true;
}

bool StepData_Plex::Matches(const char* const steptype) const
{
  int i, nb = NbMembers();
  for (i = 1; i <= nb; i++)
  {
    if (Member(i)->Matches(steptype))
      return true;
  }
  return false;
}

occ::handle<StepData_Simple> StepData_Plex::As(const char* const steptype) const
{
  occ::handle<StepData_Simple> ent;
  int                          i, nb = NbMembers();
  for (i = 1; i <= nb; i++)
  {
    ent = Member(i);
    if (ent->Matches(steptype))
      return ent;
  }
  ent.Nullify();
  return ent;
}

bool StepData_Plex::HasField(const char* const name) const
{
  int i, nb = NbMembers();
  for (i = 1; i <= nb; i++)
  {
    if (Member(i)->HasField(name))
      return true;
  }
  return false;
}

const StepData_Field& StepData_Plex::Field(const char* const name) const
{
  occ::handle<StepData_Simple> ent;
  int                          i, nb = NbMembers();
  for (i = 1; i <= nb; i++)
  {
    ent = Member(i);
    if (ent->HasField(name))
      return ent->Field(name);
  }
  throw Interface_InterfaceMismatch("StepData_Plex : Field");
}

StepData_Field& StepData_Plex::CField(const char* const name)
{
  occ::handle<StepData_Simple> ent;
  int                          i, nb = NbMembers();
  for (i = 1; i <= nb; i++)
  {
    ent = Member(i);
    if (ent->HasField(name))
      return ent->CField(name);
  }
  throw Interface_InterfaceMismatch("StepData_Plex : Field");
}

int StepData_Plex::NbMembers() const
{
  return themembers.Length();
}

occ::handle<StepData_Simple> StepData_Plex::Member(const int num) const
{
  return occ::down_cast<StepData_Simple>(themembers.Value(num));
}

occ::handle<NCollection_HSequence<TCollection_AsciiString>> StepData_Plex::TypeList() const
{
  occ::handle<NCollection_HSequence<TCollection_AsciiString>> tl =
    new NCollection_HSequence<TCollection_AsciiString>();
  int i, nb = NbMembers();
  for (i = 1; i <= nb; i++)
  {
    TCollection_AsciiString nam(Member(i)->StepType());
    tl->Append(nam);
  }
  return tl;
}

void StepData_Plex::Check(occ::handle<Interface_Check>& ach) const
{
  int i, nb = NbMembers();
  for (i = 1; i <= nb; i++)
  {
    occ::handle<StepData_Simple> si = Member(i);
    si->Check(ach);
  }
}

void StepData_Plex::Shared(Interface_EntityIterator& list) const
{
  int i, nb = NbMembers();
  for (i = 1; i <= nb; i++)
  {
    occ::handle<StepData_Simple> si = Member(i);
    si->Shared(list);
  }
}
