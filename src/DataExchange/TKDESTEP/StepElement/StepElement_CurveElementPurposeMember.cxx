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

#include <StepElement_CurveElementPurposeMember.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepElement_CurveElementPurposeMember, StepData_SelectNamed)

static const char* ECEP = "ENUMERATED_CURVE_ELEMENT_PURPOSE";
static const char* ADEP = "APPLICATION_DEFINED_ELEMENT_PURPOSE";

//=================================================================================================

StepElement_CurveElementPurposeMember::StepElement_CurveElementPurposeMember()
    : mycase(0)
{
}

//=================================================================================================

bool StepElement_CurveElementPurposeMember::HasName() const
{
  return mycase > 0;
}

//=================================================================================================

const char* StepElement_CurveElementPurposeMember::Name() const
{
  switch (mycase)
  {
    case 1:
      return ECEP;
    case 2:
      return ADEP;
    default:
      break;
  }
  return "";
}

//=================================================================================================

static int CompareNames(const char* name, int& /*numen*/)
{
  int thecase = 0;
  if (!name || name[0] == '\0')
    thecase = 0;
  else if (!strcmp(name, ECEP))
  {
    thecase = 1;
  }
  else if (!strcmp(name, ADEP))
  {
    thecase = 2;
  }
  return thecase;
}

//=================================================================================================

bool StepElement_CurveElementPurposeMember::SetName(const char* name)
{
  int numit = 0;
  mycase                 = CompareNames(name, numit);
  if (numit)
    SetInteger(numit);
  return (mycase > 0);
}

//=================================================================================================

bool StepElement_CurveElementPurposeMember::Matches(const char* name) const
{
  int numit   = 0;
  int thecase = CompareNames(name, numit);
  return (mycase == thecase);
}
