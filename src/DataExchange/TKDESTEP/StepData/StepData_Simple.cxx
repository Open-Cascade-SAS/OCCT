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
#include <StepData_Simple.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepData_Simple, StepData_Described)

StepData_Simple::StepData_Simple(const occ::handle<StepData_ESDescr>& descr)
    : StepData_Described(descr),
      thefields(descr->NbFields())
{
}

occ::handle<StepData_ESDescr> StepData_Simple::ESDescr() const
{
  return occ::down_cast<StepData_ESDescr>(Description());
}

const char* StepData_Simple::StepType() const
{
  return ESDescr()->TypeName();
}

bool StepData_Simple::IsComplex() const
{
  return false;
}

bool StepData_Simple::Matches(const char* const steptype) const
{
  return ESDescr()->Matches(steptype);
}

occ::handle<StepData_Simple> StepData_Simple::As(const char* const steptype) const
{
  occ::handle<StepData_Simple> nulent;
  if (Matches(steptype))
    return this;
  return nulent;
}

bool StepData_Simple::HasField(const char* const name) const
{
  int num = ESDescr()->Rank(name);
  return (num > 0);
}

const StepData_Field& StepData_Simple::Field(const char* const name) const
{
  int num = ESDescr()->Rank(name);
  if (num == 0)
    throw Interface_InterfaceMismatch("StepData_Simple : Field");
  return FieldNum(num);
}

StepData_Field& StepData_Simple::CField(const char* const name)
{
  int num = ESDescr()->Rank(name);
  if (num == 0)
    throw Interface_InterfaceMismatch("StepData_Simple : Field");
  return CFieldNum(num);
}

int StepData_Simple::NbFields() const
{
  return thefields.NbFields();
}

const StepData_Field& StepData_Simple::FieldNum(const int num) const
{
  return thefields.Field(num);
}

StepData_Field& StepData_Simple::CFieldNum(const int num)
{
  return thefields.CField(num);
}

const StepData_FieldListN& StepData_Simple::Fields() const
{
  return thefields;
}

StepData_FieldListN& StepData_Simple::CFields()
{
  return thefields;
}

void StepData_Simple::Check(occ::handle<Interface_Check>& /*ach*/) const {
} // something? see the description

void StepData_Simple::Shared(Interface_EntityIterator& list) const
{
  int i, nb = thefields.NbFields();
  for (i = 1; i <= nb; i++)
  {
    const StepData_Field& fi = thefields.Field(i);
    int      j1, j2, l1, l2;
    l1 = l2 = 1;
    if (fi.Arity() >= 1)
      l1 = fi.Length(1);
    if (fi.Arity() > 1)
      l2 = fi.Length(2);
    for (j1 = 1; j1 <= l1; j1++)
    {
      for (j2 = 1; j2 <= l2; j2++)
      {
        occ::handle<Standard_Transient> ent = fi.Entity(j1, j2);
        if (!ent.IsNull())
          list.AddItem(ent);
      }
    }
  }
}
