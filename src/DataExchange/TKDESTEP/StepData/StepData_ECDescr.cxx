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
#include <StepData_ECDescr.hxx>
#include <StepData_ESDescr.hxx>
#include <StepData_Plex.hxx>
#include <StepData_Simple.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepData_ECDescr, StepData_EDescr)

StepData_ECDescr::StepData_ECDescr() {}

void StepData_ECDescr::Add(const occ::handle<StepData_ESDescr>& member)
{
  if (member.IsNull())
    return;
  const char*             name = member->TypeName();
  TCollection_AsciiString nam(name);
  for (int i = NbMembers(); i > 0; i--)
  {
    occ::handle<StepData_ESDescr> mem = Member(i);
    if (nam.IsLess(mem->TypeName()))
    {
      thelist.InsertBefore(i, member);
      return;
    }
  }
  thelist.Append(member);
}

int StepData_ECDescr::NbMembers() const
{
  return thelist.Length();
}

occ::handle<StepData_ESDescr> StepData_ECDescr::Member(const int num) const
{
  return occ::down_cast<StepData_ESDescr>(thelist.Value(num));
}

occ::handle<NCollection_HSequence<TCollection_AsciiString>> StepData_ECDescr::TypeList() const
{
  occ::handle<NCollection_HSequence<TCollection_AsciiString>> tl =
    new NCollection_HSequence<TCollection_AsciiString>();
  int i, nb = NbMembers();
  for (i = 1; i <= nb; i++)
  {
    TCollection_AsciiString nam(Member(i)->TypeName());
    tl->Append(nam);
  }
  return tl;
}

bool StepData_ECDescr::Matches(const char* name) const
{
  int i, nb = NbMembers();
  for (i = 1; i <= nb; i++)
  {
    occ::handle<StepData_ESDescr> member = Member(i);
    if (member->Matches(name))
      return true;
  }
  return false;
}

bool StepData_ECDescr::IsComplex() const
{
  return true;
}

occ::handle<StepData_Described> StepData_ECDescr::NewEntity() const
{
  occ::handle<StepData_Plex> ent = new StepData_Plex(this);
  int                        i, nb = NbMembers();
  for (i = 1; i <= nb; i++)
  {
    occ::handle<StepData_ESDescr> member = Member(i);
    occ::handle<StepData_Simple>  mem    = occ::down_cast<StepData_Simple>(member->NewEntity());
    if (!mem.IsNull())
      ent->Add(mem);
  }
  return ent;
}
