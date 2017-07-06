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

#include <Standard_Type.hxx>
#include <StepRepr_PropertyDefinitionRepresentation.hxx>
#include <StepRepr_Representation.hxx>
#include <StepRepr_RepresentedDefinition.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_PropertyDefinitionRepresentation,Standard_Transient)

//=======================================================================
//function : StepRepr_PropertyDefinitionRepresentation
//purpose  : 
//=======================================================================
StepRepr_PropertyDefinitionRepresentation::StepRepr_PropertyDefinitionRepresentation ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepRepr_PropertyDefinitionRepresentation::Init (const StepRepr_RepresentedDefinition &aDefinition,
                                                      const Handle(StepRepr_Representation) &aUsedRepresentation)
{

  theDefinition = aDefinition;

  theUsedRepresentation = aUsedRepresentation;
}

//=======================================================================
//function : Definition
//purpose  : 
//=======================================================================

StepRepr_RepresentedDefinition StepRepr_PropertyDefinitionRepresentation::Definition () const
{
  return theDefinition;
}

//=======================================================================
//function : SetDefinition
//purpose  : 
//=======================================================================

void StepRepr_PropertyDefinitionRepresentation::SetDefinition (const StepRepr_RepresentedDefinition &aDefinition)
{
  theDefinition = aDefinition;
}

//=======================================================================
//function : UsedRepresentation
//purpose  : 
//=======================================================================

Handle(StepRepr_Representation) StepRepr_PropertyDefinitionRepresentation::UsedRepresentation () const
{
  return theUsedRepresentation;
}

//=======================================================================
//function : SetUsedRepresentation
//purpose  : 
//=======================================================================

void StepRepr_PropertyDefinitionRepresentation::SetUsedRepresentation (const Handle(StepRepr_Representation) &aUsedRepresentation)
{
  theUsedRepresentation = aUsedRepresentation;
}
