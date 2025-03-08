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

#include <StepElement_VolumeElementPurposeMember.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepElement_VolumeElementPurposeMember, StepData_SelectNamed)

static Standard_CString EVEP = "ENUMERATED_VOLUME_ELEMENT_PURPOSE";
static Standard_CString ADEP = "APPLICATION_DEFINED_ELEMENT_PURPOSE";

//=================================================================================================

StepElement_VolumeElementPurposeMember::StepElement_VolumeElementPurposeMember()
    : mycase(0)
{
}

//=================================================================================================

Standard_Boolean StepElement_VolumeElementPurposeMember::HasName() const
{
  return mycase > 0;
}

//=================================================================================================

Standard_CString StepElement_VolumeElementPurposeMember::Name() const
{
  switch (mycase)
  {
    case 1:
      return EVEP;
    case 2:
      return ADEP;
    default:
      break;
  }
  return "";
}

//=================================================================================================

static Standard_Integer CompareNames(const Standard_CString name, Standard_Integer& /*numen*/)
{
  Standard_Integer thecase = 0;
  if (!name || name[0] == '\0')
    thecase = 0;
  else if (!strcmp(name, EVEP))
  {
    thecase = 1;
  }
  else if (!strcmp(name, ADEP))
  {
    thecase = 1;
  }
  return thecase;
}

//=================================================================================================

Standard_Boolean StepElement_VolumeElementPurposeMember::SetName(const Standard_CString name)
{
  Standard_Integer numit = 0;
  mycase                 = CompareNames(name, numit);
  if (numit)
    SetInteger(numit);
  return (mycase > 0);
}

//=================================================================================================

Standard_Boolean StepElement_VolumeElementPurposeMember::Matches(const Standard_CString name) const
{
  Standard_Integer numit   = 0;
  Standard_Integer thecase = CompareNames(name, numit);
  return (mycase == thecase);
}
