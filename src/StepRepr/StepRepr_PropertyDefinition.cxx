// Created on: 2000-07-03
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

#include <StepRepr_PropertyDefinition.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_PropertyDefinition, Standard_Transient)

//=================================================================================================

StepRepr_PropertyDefinition::StepRepr_PropertyDefinition()
{
  defDescription = Standard_False;
}

//=================================================================================================

void StepRepr_PropertyDefinition::Init(const Handle(TCollection_HAsciiString)& aName,
                                       const Standard_Boolean                  hasDescription,
                                       const Handle(TCollection_HAsciiString)& aDescription,
                                       const StepRepr_CharacterizedDefinition& aDefinition)
{

  theName = aName;

  defDescription = hasDescription;
  if (defDescription)
  {
    theDescription = aDescription;
  }
  else
    theDescription.Nullify();

  theDefinition = aDefinition;
}

//=================================================================================================

Handle(TCollection_HAsciiString) StepRepr_PropertyDefinition::Name() const
{
  return theName;
}

//=================================================================================================

void StepRepr_PropertyDefinition::SetName(const Handle(TCollection_HAsciiString)& aName)
{
  theName = aName;
}

//=================================================================================================

Handle(TCollection_HAsciiString) StepRepr_PropertyDefinition::Description() const
{
  return theDescription;
}

//=================================================================================================

void StepRepr_PropertyDefinition::SetDescription(
  const Handle(TCollection_HAsciiString)& aDescription)
{
  theDescription = aDescription;
}

//=================================================================================================

Standard_Boolean StepRepr_PropertyDefinition::HasDescription() const
{
  return defDescription;
}

//=================================================================================================

StepRepr_CharacterizedDefinition StepRepr_PropertyDefinition::Definition() const
{
  return theDefinition;
}

//=================================================================================================

void StepRepr_PropertyDefinition::SetDefinition(const StepRepr_CharacterizedDefinition& aDefinition)
{
  theDefinition = aDefinition;
}
