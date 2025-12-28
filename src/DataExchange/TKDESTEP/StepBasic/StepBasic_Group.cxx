// Created on: 2000-05-10
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <StepBasic_Group.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_Group, Standard_Transient)

//=================================================================================================

StepBasic_Group::StepBasic_Group()
{
  defDescription = false;
}

//=================================================================================================

void StepBasic_Group::Init(const occ::handle<TCollection_HAsciiString>& aName,
                           const bool                  hasDescription,
                           const occ::handle<TCollection_HAsciiString>& aDescription)
{

  theName = aName;

  defDescription = hasDescription;
  if (defDescription)
  {
    theDescription = aDescription;
  }
  else
    theDescription.Nullify();
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> StepBasic_Group::Name() const
{
  return theName;
}

//=================================================================================================

void StepBasic_Group::SetName(const occ::handle<TCollection_HAsciiString>& aName)
{
  theName = aName;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> StepBasic_Group::Description() const
{
  return theDescription;
}

//=================================================================================================

void StepBasic_Group::SetDescription(const occ::handle<TCollection_HAsciiString>& aDescription)
{
  theDescription = aDescription;
}

//=================================================================================================

bool StepBasic_Group::HasDescription() const
{
  return defDescription;
}
