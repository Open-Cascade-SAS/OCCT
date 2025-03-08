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

#include <StepBasic_IdentificationRole.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_IdentificationRole, Standard_Transient)

//=================================================================================================

StepBasic_IdentificationRole::StepBasic_IdentificationRole()
{
  defDescription = Standard_False;
}

//=================================================================================================

void StepBasic_IdentificationRole::Init(const Handle(TCollection_HAsciiString)& aName,
                                        const Standard_Boolean                  hasDescription,
                                        const Handle(TCollection_HAsciiString)& aDescription)
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

Handle(TCollection_HAsciiString) StepBasic_IdentificationRole::Name() const
{
  return theName;
}

//=================================================================================================

void StepBasic_IdentificationRole::SetName(const Handle(TCollection_HAsciiString)& aName)
{
  theName = aName;
}

//=================================================================================================

Handle(TCollection_HAsciiString) StepBasic_IdentificationRole::Description() const
{
  return theDescription;
}

//=================================================================================================

void StepBasic_IdentificationRole::SetDescription(
  const Handle(TCollection_HAsciiString)& aDescription)
{
  theDescription = aDescription;
}

//=================================================================================================

Standard_Boolean StepBasic_IdentificationRole::HasDescription() const
{
  return defDescription;
}
