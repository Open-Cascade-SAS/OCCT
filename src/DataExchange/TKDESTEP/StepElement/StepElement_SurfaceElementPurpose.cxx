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
#include <StepElement_SurfaceElementPurpose.hxx>
#include <StepElement_SurfaceElementPurposeMember.hxx>
#include <TCollection_HAsciiString.hxx>

//=================================================================================================

StepElement_SurfaceElementPurpose::StepElement_SurfaceElementPurpose() {}

//=================================================================================================

int StepElement_SurfaceElementPurpose::CaseNum(const occ::handle<Standard_Transient>& /*ent*/) const
{
  return 0;
}

//=================================================================================================

int StepElement_SurfaceElementPurpose::CaseMem(const occ::handle<StepData_SelectMember>& ent) const
{
  if (ent.IsNull())
    return 0;
  if (ent->Matches("EnumeratedSurfaceElementPurpose"))
    return 1;
  else if (ent->Matches("ApplicationDefinedElementPurpose"))
    return 2;
  else
    return 0;
}

//=================================================================================================

occ::handle<StepData_SelectMember> StepElement_SurfaceElementPurpose::NewMember() const
{
  return new StepElement_SurfaceElementPurposeMember;
}

//=================================================================================================

void StepElement_SurfaceElementPurpose::SetEnumeratedSurfaceElementPurpose(
  const StepElement_EnumeratedSurfaceElementPurpose val)
{
  occ::handle<StepElement_SurfaceElementPurposeMember> SelMem =
    occ::down_cast<StepElement_SurfaceElementPurposeMember>(Value());
  if (SelMem.IsNull())
    return;
  occ::handle<TCollection_HAsciiString> name =
    new TCollection_HAsciiString("EnumeratedSurfaceElementPurpose");
  SelMem->SetName(name->ToCString());
  SelMem->SetEnum((int)val);
}

//=================================================================================================

StepElement_EnumeratedSurfaceElementPurpose StepElement_SurfaceElementPurpose::
  EnumeratedSurfaceElementPurpose() const
{
  occ::handle<StepElement_SurfaceElementPurposeMember> SelMem =
    occ::down_cast<StepElement_SurfaceElementPurposeMember>(Value());
  if (SelMem.IsNull())
    return StepElement_MembraneDirect;
  occ::handle<TCollection_HAsciiString> name = new TCollection_HAsciiString;
  name->AssignCat(SelMem->Name());
  occ::handle<TCollection_HAsciiString> nameitem =
    new TCollection_HAsciiString("EnumeratedSurfaceElementPurpose");
  if (name->IsDifferent(nameitem))
    return StepElement_MembraneDirect;
  int                                         numit = SelMem->Enum();
  StepElement_EnumeratedSurfaceElementPurpose val;
  switch (numit)
  {
    case 1:
      val = StepElement_MembraneDirect;
      break;
    case 2:
      val = StepElement_MembraneShear;
      break;
    case 3:
      val = StepElement_BendingDirect;
      break;
    case 4:
      val = StepElement_BendingTorsion;
      break;
    case 5:
      val = StepElement_NormalToPlaneShear;
      break;
    default:
      return StepElement_MembraneDirect;
      break;
  }
  return val;
}

//=================================================================================================

void StepElement_SurfaceElementPurpose::SetApplicationDefinedElementPurpose(
  const occ::handle<TCollection_HAsciiString>& val)
{
  occ::handle<StepElement_SurfaceElementPurposeMember> SelMem =
    occ::down_cast<StepElement_SurfaceElementPurposeMember>(Value());
  if (SelMem.IsNull())
    return;
  occ::handle<TCollection_HAsciiString> name =
    new TCollection_HAsciiString("ApplicationDefinedElementPurpose");
  SelMem->SetName(name->ToCString());
  SelMem->SetString(val->ToCString());
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> StepElement_SurfaceElementPurpose::
  ApplicationDefinedElementPurpose() const
{
  occ::handle<StepElement_SurfaceElementPurposeMember> SelMem =
    occ::down_cast<StepElement_SurfaceElementPurposeMember>(Value());
  if (SelMem.IsNull())
    return 0;
  occ::handle<TCollection_HAsciiString> name = new TCollection_HAsciiString;
  name->AssignCat(SelMem->Name());
  occ::handle<TCollection_HAsciiString> nameitem =
    new TCollection_HAsciiString("ApplicationDefinedElementPurpose");
  if (name->IsDifferent(nameitem))
    return 0;
  occ::handle<TCollection_HAsciiString> val = new TCollection_HAsciiString;
  val->AssignCat(SelMem->String());
  return val;
}
