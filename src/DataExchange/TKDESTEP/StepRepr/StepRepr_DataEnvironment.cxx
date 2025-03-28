// Created on: 2002-12-12
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <StepRepr_DataEnvironment.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_DataEnvironment, Standard_Transient)

//=================================================================================================

StepRepr_DataEnvironment::StepRepr_DataEnvironment() {}

//=================================================================================================

void StepRepr_DataEnvironment::Init(
  const Handle(TCollection_HAsciiString)&                           aName,
  const Handle(TCollection_HAsciiString)&                           aDescription,
  const Handle(StepRepr_HArray1OfPropertyDefinitionRepresentation)& aElements)
{

  theName = aName;

  theDescription = aDescription;

  theElements = aElements;
}

//=================================================================================================

Handle(TCollection_HAsciiString) StepRepr_DataEnvironment::Name() const
{
  return theName;
}

//=================================================================================================

void StepRepr_DataEnvironment::SetName(const Handle(TCollection_HAsciiString)& aName)
{
  theName = aName;
}

//=================================================================================================

Handle(TCollection_HAsciiString) StepRepr_DataEnvironment::Description() const
{
  return theDescription;
}

//=================================================================================================

void StepRepr_DataEnvironment::SetDescription(const Handle(TCollection_HAsciiString)& aDescription)
{
  theDescription = aDescription;
}

//=================================================================================================

Handle(StepRepr_HArray1OfPropertyDefinitionRepresentation) StepRepr_DataEnvironment::Elements()
  const
{
  return theElements;
}

//=================================================================================================

void StepRepr_DataEnvironment::SetElements(
  const Handle(StepRepr_HArray1OfPropertyDefinitionRepresentation)& aElements)
{
  theElements = aElements;
}
