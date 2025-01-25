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
#include <StepRepr_MaterialPropertyRepresentation.hxx>
#include <StepRepr_Representation.hxx>
#include <StepRepr_RepresentedDefinition.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_MaterialPropertyRepresentation,
                           StepRepr_PropertyDefinitionRepresentation)

//=================================================================================================

StepRepr_MaterialPropertyRepresentation::StepRepr_MaterialPropertyRepresentation() {}

//=================================================================================================

void StepRepr_MaterialPropertyRepresentation::Init(
  const StepRepr_RepresentedDefinition&   aPropertyDefinitionRepresentation_Definition,
  const Handle(StepRepr_Representation)&  aPropertyDefinitionRepresentation_UsedRepresentation,
  const Handle(StepRepr_DataEnvironment)& aDependentEnvironment)
{
  StepRepr_PropertyDefinitionRepresentation::Init(
    aPropertyDefinitionRepresentation_Definition,
    aPropertyDefinitionRepresentation_UsedRepresentation);

  theDependentEnvironment = aDependentEnvironment;
}

//=================================================================================================

Handle(StepRepr_DataEnvironment) StepRepr_MaterialPropertyRepresentation::DependentEnvironment()
  const
{
  return theDependentEnvironment;
}

//=================================================================================================

void StepRepr_MaterialPropertyRepresentation::SetDependentEnvironment(
  const Handle(StepRepr_DataEnvironment)& aDependentEnvironment)
{
  theDependentEnvironment = aDependentEnvironment;
}
