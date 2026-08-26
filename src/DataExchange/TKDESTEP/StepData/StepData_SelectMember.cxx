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
#include <StepData_Field.hxx>
#include <StepData_SelectMember.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepData_SelectMember, Standard_Transient)

StepData_SelectMember::StepData_SelectMember() = default;

bool StepData_SelectMember::HasName() const
{
  return false;
}

const char* StepData_SelectMember::Name() const
{
  return "";
}

bool StepData_SelectMember::SetName(const char* const /*bid*/)
{
  return false;
}

bool StepData_SelectMember::Matches(const char* const name) const
{
  return !strcmp(name, Name());
}

int StepData_SelectMember::Kind() const
{
  return 0;
}

void StepData_SelectMember::SetKind(const int) {}

Interface_ParamType StepData_SelectMember::ParamType() const
{
  const int aKind = Kind();
  if (aKind == static_cast<int>(StepData_Field::FieldKind::Undefined))
  {
    return Interface_ParamVoid;
  }
  if (aKind == static_cast<int>(StepData_Field::FieldKind::Integer))
  {
    return Interface_ParamInteger;
  }
  if (aKind == static_cast<int>(StepData_Field::FieldKind::Boolean)
      || aKind == static_cast<int>(StepData_Field::FieldKind::Logical))
  {
    return Interface_ParamLogical;
  }
  if (aKind == static_cast<int>(StepData_Field::FieldKind::Enum))
  {
    return Interface_ParamEnum;
  }
  if (aKind == static_cast<int>(StepData_Field::FieldKind::Real))
  {
    return Interface_ParamReal;
  }
  if (aKind == static_cast<int>(StepData_Field::FieldKind::String))
  {
    return Interface_ParamText;
  }
  return Interface_ParamMisc;
}

int StepData_SelectMember::Int() const
{
  return 0;
}

void StepData_SelectMember::SetInt(const int) {}

int StepData_SelectMember::Integer() const
{
  return Int();
}

void StepData_SelectMember::SetInteger(const int val)
{
  SetKind(static_cast<int>(StepData_Field::FieldKind::Integer));
  SetInt(val);
}

bool StepData_SelectMember::Boolean() const
{
  return (Int() > 0);
}

void StepData_SelectMember::SetBoolean(const bool val)
{
  SetKind(static_cast<int>(StepData_Field::FieldKind::Boolean));
  SetInt((val ? 1 : 0));
}

StepData_Logical StepData_SelectMember::Logical() const
{
  int ival = Int();
  if (ival == 0)
  {
    return StepData_LFalse;
  }
  if (ival == 1)
  {
    return StepData_LTrue;
  }
  return StepData_LUnknown;
}

void StepData_SelectMember::SetLogical(const StepData_Logical val)
{
  SetKind(static_cast<int>(StepData_Field::FieldKind::Logical));
  SetInt(static_cast<int>(val));
}

double StepData_SelectMember::Real() const
{
  return 0.0;
}

void StepData_SelectMember::SetReal(const double) {}

const char* StepData_SelectMember::String() const
{
  return "";
}

void StepData_SelectMember::SetString(const char* const) {}

int StepData_SelectMember::Enum() const
{
  return Int();
}

const char* StepData_SelectMember::EnumText() const
{
  return String();
}

void StepData_SelectMember::SetEnum(const int val, const char* const text)
{
  SetKind(static_cast<int>(StepData_Field::FieldKind::Enum));
  SetInt(val);
  if (text && text[0] != '\0')
  {
    SetEnumText(val, text);
  }
}

void StepData_SelectMember::SetEnumText(const int /*val*/, const char* const text)
{
  SetString(text);
}
