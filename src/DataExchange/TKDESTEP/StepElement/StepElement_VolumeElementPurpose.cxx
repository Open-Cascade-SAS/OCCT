// Created on: 2002-12-10
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V2.0

#include <Standard_Transient.hxx>
#include <StepData_SelectMember.hxx>
#include <StepElement_VolumeElementPurpose.hxx>
#include <StepElement_VolumeElementPurposeMember.hxx>
#include <TCollection_HAsciiString.hxx>

//=================================================================================================

StepElement_VolumeElementPurpose::StepElement_VolumeElementPurpose() = default;

//=================================================================================================

int StepElement_VolumeElementPurpose::CaseNum(const occ::handle<Standard_Transient>& /*ent*/) const
{
  return 0;
}

//=================================================================================================

int StepElement_VolumeElementPurpose::CaseMem(const occ::handle<StepData_SelectMember>& ent) const
{
  if (ent.IsNull())
    return 0;
  if (ent->Matches("EnumeratedVolumeElementPurpose"))
    return 1;
  else if (ent->Matches("ApplicationDefinedElementPurpose"))
    return 2;
  else
    return 0;
}

//=================================================================================================

occ::handle<StepData_SelectMember> StepElement_VolumeElementPurpose::NewMember() const
{
  return new StepElement_VolumeElementPurposeMember;
}

//=================================================================================================

void StepElement_VolumeElementPurpose::SetEnumeratedVolumeElementPurpose(
  const StepElement_EnumeratedVolumeElementPurpose val)
{
  occ::handle<StepElement_VolumeElementPurposeMember> SelMem =
    occ::down_cast<StepElement_VolumeElementPurposeMember>(Value());
  if (SelMem.IsNull())
    return;
  occ::handle<TCollection_HAsciiString> name =
    new TCollection_HAsciiString("EnumeratedVolumeElementPurpose");
  SelMem->SetName(name->ToCString());
  SelMem->SetEnum((int)val);
}

//=================================================================================================

StepElement_EnumeratedVolumeElementPurpose StepElement_VolumeElementPurpose::
  EnumeratedVolumeElementPurpose() const
{
  occ::handle<StepElement_VolumeElementPurposeMember> SelMem =
    occ::down_cast<StepElement_VolumeElementPurposeMember>(Value());
  if (SelMem.IsNull())
    return StepElement_StressDisplacement;
  occ::handle<TCollection_HAsciiString> name = new TCollection_HAsciiString;
  name->AssignCat(SelMem->Name());
  occ::handle<TCollection_HAsciiString> nameitem =
    new TCollection_HAsciiString("EnumeratedVolumeElementPurpose");
  if (name->IsDifferent(nameitem))
    return StepElement_StressDisplacement;
  int                                        numit = SelMem->Enum();
  StepElement_EnumeratedVolumeElementPurpose val;
  switch (numit)
  {
    case 1:
      val = StepElement_StressDisplacement;
      break;
    default:
      return StepElement_StressDisplacement;
      break;
  }
  return val;
}

//=================================================================================================

void StepElement_VolumeElementPurpose::SetApplicationDefinedElementPurpose(
  const occ::handle<TCollection_HAsciiString>& val)
{
  occ::handle<StepElement_VolumeElementPurposeMember> SelMem =
    occ::down_cast<StepElement_VolumeElementPurposeMember>(Value());
  if (SelMem.IsNull())
    return;
  occ::handle<TCollection_HAsciiString> name =
    new TCollection_HAsciiString("ApplicationDefinedElementPurpose");
  SelMem->SetName(name->ToCString());
  SelMem->SetString(val->ToCString());
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> StepElement_VolumeElementPurpose::
  ApplicationDefinedElementPurpose() const
{
  occ::handle<StepElement_VolumeElementPurposeMember> SelMem =
    occ::down_cast<StepElement_VolumeElementPurposeMember>(Value());
  if (SelMem.IsNull())
    return nullptr;
  occ::handle<TCollection_HAsciiString> name = new TCollection_HAsciiString;
  name->AssignCat(SelMem->Name());
  occ::handle<TCollection_HAsciiString> nameitem =
    new TCollection_HAsciiString("ApplicationDefinedElementPurpose");
  if (name->IsDifferent(nameitem))
    return nullptr;
  occ::handle<TCollection_HAsciiString> val = new TCollection_HAsciiString;
  val->AssignCat(SelMem->String());
  return val;
}
