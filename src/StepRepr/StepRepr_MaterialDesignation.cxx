// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <Standard_Type.hxx>
#include <StepRepr_CharacterizedDefinition.hxx>
#include <StepRepr_MaterialDesignation.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_MaterialDesignation,Standard_Transient)

StepRepr_MaterialDesignation::StepRepr_MaterialDesignation  ()    {  }

void  StepRepr_MaterialDesignation::Init
(const Handle(TCollection_HAsciiString)& aName,
 const StepRepr_CharacterizedDefinition& aOfDefinition)
{
  name = aName;
  ofDefinition = aOfDefinition;
}

void  StepRepr_MaterialDesignation::SetName
  (const Handle(TCollection_HAsciiString)& aName)
{
  name = aName;
}

Handle(TCollection_HAsciiString)  StepRepr_MaterialDesignation::Name () const
{
  return name;
}

void  StepRepr_MaterialDesignation::SetOfDefinition
  (const StepRepr_CharacterizedDefinition& aOfDefinition)
{
  ofDefinition = aOfDefinition;
}

StepRepr_CharacterizedDefinition  StepRepr_MaterialDesignation::OfDefinition () const
{
  return ofDefinition;
}
