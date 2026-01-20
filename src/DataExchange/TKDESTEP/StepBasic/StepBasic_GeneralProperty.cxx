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

#include <StepBasic_GeneralProperty.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_GeneralProperty, Standard_Transient)

//=================================================================================================

StepBasic_GeneralProperty::StepBasic_GeneralProperty()
{
  defDescription = false;
}

//=================================================================================================

void StepBasic_GeneralProperty::Init(const occ::handle<TCollection_HAsciiString>& aId,
                                     const occ::handle<TCollection_HAsciiString>& aName,
                                     const bool                  hasDescription,
                                     const occ::handle<TCollection_HAsciiString>& aDescription)
{

  theId = aId;

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

occ::handle<TCollection_HAsciiString> StepBasic_GeneralProperty::Id() const
{
  return theId;
}

//=================================================================================================

void StepBasic_GeneralProperty::SetId(const occ::handle<TCollection_HAsciiString>& aId)
{
  theId = aId;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> StepBasic_GeneralProperty::Name() const
{
  return theName;
}

//=================================================================================================

void StepBasic_GeneralProperty::SetName(const occ::handle<TCollection_HAsciiString>& aName)
{
  theName = aName;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> StepBasic_GeneralProperty::Description() const
{
  return theDescription;
}

//=================================================================================================

void StepBasic_GeneralProperty::SetDescription(const occ::handle<TCollection_HAsciiString>& aDescription)
{
  theDescription = aDescription;
}

//=================================================================================================

bool StepBasic_GeneralProperty::HasDescription() const
{
  return defDescription;
}
