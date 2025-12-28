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
#include <StepElement_CurveElementFreedom.hxx>
#include <StepElement_CurveElementFreedomMember.hxx>
#include <TCollection_HAsciiString.hxx>

static const char* anEnum = "ENUMERATED_CURVE_ELEMENT_FREEDOM";
static const char* anApp  = "APPLICATION_DEFINED_DEGREE_OF_FREEDOM";

//=================================================================================================

StepElement_CurveElementFreedom::StepElement_CurveElementFreedom() {}

//=================================================================================================

int StepElement_CurveElementFreedom::CaseNum(const occ::handle<Standard_Transient>& /*ent*/) const
{
  return 0;
}

//=================================================================================================

int StepElement_CurveElementFreedom::CaseMem(const occ::handle<StepData_SelectMember>& ent) const
{
  if (ent.IsNull())
    return 0;
  if (ent->Matches(anEnum))
    return 1;
  else if (ent->Matches(anApp))
    return 2;
  else
    return 0;
}

//=================================================================================================

occ::handle<StepData_SelectMember> StepElement_CurveElementFreedom::NewMember() const
{
  return new StepElement_CurveElementFreedomMember;
}

//=================================================================================================

void StepElement_CurveElementFreedom::SetEnumeratedCurveElementFreedom(
  const StepElement_EnumeratedCurveElementFreedom val)
{
  occ::handle<StepElement_CurveElementFreedomMember> SelMem =
    occ::down_cast<StepElement_CurveElementFreedomMember>(Value());
  if (SelMem.IsNull())
    return;
  occ::handle<TCollection_HAsciiString> name =
    new TCollection_HAsciiString("ENUMERATED_CURVE_ELEMENT_FREEDOM");
  SelMem->SetName(name->ToCString());
  SelMem->SetEnum((int)val);
}

//=================================================================================================

StepElement_EnumeratedCurveElementFreedom StepElement_CurveElementFreedom::
  EnumeratedCurveElementFreedom() const
{
  occ::handle<StepElement_CurveElementFreedomMember> SelMem =
    occ::down_cast<StepElement_CurveElementFreedomMember>(Value());
  if (SelMem.IsNull())
    return StepElement_None;
  occ::handle<TCollection_HAsciiString> name = new TCollection_HAsciiString;
  name->AssignCat(SelMem->Name());
  occ::handle<TCollection_HAsciiString> nameitem =
    new TCollection_HAsciiString("ENUMERATED_CURVE_ELEMENT_FREEDOM");
  if (name->IsDifferent(nameitem))
    return StepElement_None;
  int                                       numit = SelMem->Enum();
  StepElement_EnumeratedCurveElementFreedom val;
  switch (numit)
  {
    case 1:
      val = StepElement_XTranslation;
      break;
    case 2:
      val = StepElement_YTranslation;
      break;
    case 3:
      val = StepElement_ZTranslation;
      break;
    case 4:
      val = StepElement_XRotation;
      break;
    case 5:
      val = StepElement_YRotation;
      break;
    case 6:
      val = StepElement_ZRotation;
      break;
    case 7:
      val = StepElement_Warp;
      break;
    case 8:
      val = StepElement_None;
      break;
    default:
      return StepElement_None;
      break;
  }
  return val;
}

//=================================================================================================

void StepElement_CurveElementFreedom::SetApplicationDefinedDegreeOfFreedom(
  const occ::handle<TCollection_HAsciiString>& val)
{
  occ::handle<StepElement_CurveElementFreedomMember> SelMem =
    occ::down_cast<StepElement_CurveElementFreedomMember>(Value());
  if (SelMem.IsNull())
    return;
  occ::handle<TCollection_HAsciiString> name =
    new TCollection_HAsciiString("APPLICATION_DEFINED_DEGREE_OF_FREEDOM");
  SelMem->SetName(name->ToCString());
  SelMem->SetString(val->ToCString());
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> StepElement_CurveElementFreedom::
  ApplicationDefinedDegreeOfFreedom() const
{
  occ::handle<StepElement_CurveElementFreedomMember> SelMem =
    occ::down_cast<StepElement_CurveElementFreedomMember>(Value());
  if (SelMem.IsNull())
    return 0;
  occ::handle<TCollection_HAsciiString> name = new TCollection_HAsciiString;
  name->AssignCat(SelMem->Name());
  occ::handle<TCollection_HAsciiString> nameitem =
    new TCollection_HAsciiString("APPLICATION_DEFINED_DEGREE_OF_FREEDOM");
  if (name->IsDifferent(nameitem))
    return 0;
  occ::handle<TCollection_HAsciiString> val = new TCollection_HAsciiString;
  val->AssignCat(SelMem->String());
  return val;
}
