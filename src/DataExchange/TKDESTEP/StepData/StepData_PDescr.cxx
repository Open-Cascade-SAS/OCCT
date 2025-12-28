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
#include <Standard_Type.hxx>
#include <StepData_EDescr.hxx>
#include <StepData_Field.hxx>
#include <StepData_PDescr.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepData_PDescr, Standard_Transient)

#define KindInteger 1
#define KindBoolean 2
#define KindLogical 3
#define KindEnum 4
#define KindReal 5
#define KindString 6
#define KindEntity 7

StepData_PDescr::StepData_PDescr()
    : thesel(0),
      thekind(0),
      thearit(0),
      theopt(false),
      theder(false),
      thefnum(0)
{
}

void StepData_PDescr::SetName(const char* name)
{
  thename.Clear();
  thename.AssignCat(name);
}

const char* StepData_PDescr::Name() const
{
  return thename.ToCString();
}

int StepData_PDescr::Kind() const
{
  return thekind;
}

void StepData_PDescr::SetSelect()
{
  thesel = 4;
}

void StepData_PDescr::AddMember(const occ::handle<StepData_PDescr>& member)
{
  if (member.IsNull())
    return;
  if (thenext.IsNull())
    thenext = member;
  else
    thenext->AddMember(member);
  if (thesel == 3)
    return;
  if (thekind < KindEntity && thenext->Kind() >= KindEntity)
    thesel = 3;
  else if (thekind < KindEntity && (thesel == 2 || thesel == 4))
    thesel = 3;
  else if (thekind >= KindEntity && (thesel == 1 || thesel == 4))
    thesel = 2;
}

void StepData_PDescr::SetMemberName(const char* memname)
{
  thesnam.Clear();
  thesnam.AssignCat(memname);
}

void StepData_PDescr::SetInteger()
{
  thekind = KindInteger;
}

void StepData_PDescr::SetReal()
{
  thekind = KindReal;
}

void StepData_PDescr::SetString()
{
  thekind = KindString;
}

void StepData_PDescr::SetBoolean()
{
  thekind = KindBoolean;
}

void StepData_PDescr::SetLogical()
{
  thekind = KindLogical;
}

void StepData_PDescr::SetEnum()
{
  thekind = KindEnum;
}

void StepData_PDescr::AddEnumDef(const char* enumdef)
{
  theenum.AddDefinition(enumdef);
}

void StepData_PDescr::SetType(const occ::handle<Standard_Type>& atype)
{
  thekind = KindEntity;
  thetype = atype;
  thednam.Clear();
}

void StepData_PDescr::SetDescr(const char* dscnam)
{
  thekind = KindEntity;
  thetype.Nullify();
  thednam.Clear();
  thednam.AssignCat(dscnam);
}

void StepData_PDescr::AddArity(const int arity)
{
  thearit += arity;
}

void StepData_PDescr::SetArity(const int arity)
{
  thearit = arity;
}

void StepData_PDescr::SetFrom(const occ::handle<StepData_PDescr>& other)
{
  if (other.IsNull())
    return;
  thekind = other->Kind();
  int i, maxenum = other->EnumMax();
  for (i = 0; i <= maxenum; i++)
    AddEnumDef(other->EnumText(i));
  //  SELECT types are not copied
  thetype = other->Type();
  thearit = other->Arity();
  thefrom = other;
  theopt  = other->IsOptional();
  theder  = other->IsDerived();
  thefnam.Clear();
  thefnam.AssignCat(other->FieldName());
  thefnum = other->FieldRank();
}

void StepData_PDescr::SetOptional(const bool opt)
{
  theopt = opt;
}

void StepData_PDescr::SetDerived(const bool der)
{
  theder = der;
}

void StepData_PDescr::SetField(const char* name, const int rank)
{
  thefnam.Clear();
  thefnam.AssignCat(name);
  thefnum = rank;
}

//    ######  INTERRO  ######

bool StepData_PDescr::IsSelect() const
{
  if (!thefrom.IsNull())
    return thefrom->IsSelect();
  return (thesel > 0);
}

occ::handle<StepData_PDescr> StepData_PDescr::Member(const char* name) const
{
  if (!thefrom.IsNull())
    return thefrom->Member(name);
  occ::handle<StepData_PDescr> descr;
  if (thesnam.IsEqual(name))
    return this;
  if (thenext.IsNull())
    return descr; // null
  return thenext->Member(name);
}

bool StepData_PDescr::IsInteger() const
{
  return (thekind == KindInteger);
}

bool StepData_PDescr::IsReal() const
{
  return (thekind == KindReal);
}

bool StepData_PDescr::IsString() const
{
  return (thekind == KindString);
}

bool StepData_PDescr::IsBoolean() const
{
  return (thekind == KindBoolean || thekind == KindLogical);
}

bool StepData_PDescr::IsLogical() const
{
  return (thekind == KindLogical);
}

bool StepData_PDescr::IsEnum() const
{
  return (thekind == KindEnum);
}

int StepData_PDescr::EnumMax() const
{
  return theenum.MaxValue();
}

int StepData_PDescr::EnumValue(const char* name) const
{
  return theenum.Value(name);
}

const char* StepData_PDescr::EnumText(const int val) const
{
  return theenum.Text(val).ToCString();
}

bool StepData_PDescr::IsEntity() const
{
  return (thekind == KindEntity);
}

bool StepData_PDescr::IsType(const occ::handle<Standard_Type>& atype) const
{
  if (atype.IsNull())
    return false;
  if (!thetype.IsNull())
  {
    if (atype->SubType(thetype))
      return true;
  }
  if (!thenext.IsNull())
    return thenext->IsType(atype);
  if (!thefrom.IsNull())
    return thefrom->IsType(atype);
  return false;
}

occ::handle<Standard_Type> StepData_PDescr::Type() const
{
  return thetype;
}

bool StepData_PDescr::IsDescr(const occ::handle<StepData_EDescr>& descr) const
{
  if (descr.IsNull())
    return false;
  if (thednam.Length() > 0)
  {
    if (descr->Matches(thednam.ToCString()))
      return true;
  }
  if (!thenext.IsNull())
    return thenext->IsDescr(descr);
  if (!thefrom.IsNull())
    return thefrom->IsDescr(descr);
  return false;
}

const char* StepData_PDescr::DescrName() const
{
  return thednam.ToCString();
}

int StepData_PDescr::Arity() const
{
  return thearit;
}

occ::handle<StepData_PDescr> StepData_PDescr::Simple() const
{
  if (thearit == 0)
    return this;
  if (thefrom.IsNull())
    return this;
  return thefrom;
}

bool StepData_PDescr::IsOptional() const
{
  return theopt;
}

bool StepData_PDescr::IsDerived() const
{
  return theder;
}

bool StepData_PDescr::IsField() const
{
  return (thefnum > 0);
}

const char* StepData_PDescr::FieldName() const
{
  return thefnam.ToCString();
}

int StepData_PDescr::FieldRank() const
{
  return thefnum;
}

void StepData_PDescr::Check(const StepData_Field& /*afild*/,
                            occ::handle<Interface_Check>& /*ach*/) const
{
  //  For now...
}
