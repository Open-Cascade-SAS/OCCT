// Created on: 2002-12-15
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

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepRepr_RWStructuralResponsePropertyDefinitionRepresentation.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_PropertyDefinition.hxx>
#include <StepRepr_Representation.hxx>
#include <StepRepr_StructuralResponsePropertyDefinitionRepresentation.hxx>

//=======================================================================
//function : RWStepRepr_RWStructuralResponsePropertyDefinitionRepresentation
//purpose  : 
//=======================================================================
RWStepRepr_RWStructuralResponsePropertyDefinitionRepresentation::RWStepRepr_RWStructuralResponsePropertyDefinitionRepresentation ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWStructuralResponsePropertyDefinitionRepresentation::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                                                const Standard_Integer num,
                                                                                Handle(Interface_Check)& ach,
                                                                                const Handle(StepRepr_StructuralResponsePropertyDefinitionRepresentation) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"structural_response_property_definition_representation") ) return;

  // Inherited fields of PropertyDefinitionRepresentation

  Handle(StepRepr_PropertyDefinition) aPropertyDefinitionRepresentation_Definition;
  data->ReadEntity (num, 1, "property_definition_representation.definition", ach, STANDARD_TYPE(StepRepr_PropertyDefinition), aPropertyDefinitionRepresentation_Definition);

  Handle(StepRepr_Representation) aPropertyDefinitionRepresentation_UsedRepresentation;
  data->ReadEntity (num, 2, "property_definition_representation.used_representation", ach, STANDARD_TYPE(StepRepr_Representation), aPropertyDefinitionRepresentation_UsedRepresentation);

  //skl 15.12.2002
  StepRepr_RepresentedDefinition aRepresentedDefinition;
  aRepresentedDefinition.SetValue(aPropertyDefinitionRepresentation_Definition);

  // Initialize entity
  ent->Init(/*aPropertyDefinitionRepresentation_Definition*/aRepresentedDefinition,
            aPropertyDefinitionRepresentation_UsedRepresentation);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWStructuralResponsePropertyDefinitionRepresentation::WriteStep (StepData_StepWriter& SW,
                                                                                 const Handle(StepRepr_StructuralResponsePropertyDefinitionRepresentation) &ent) const
{

  // Inherited fields of PropertyDefinitionRepresentation

  SW.Send (ent->StepRepr_PropertyDefinitionRepresentation::Definition().Value());

  SW.Send (ent->StepRepr_PropertyDefinitionRepresentation::UsedRepresentation());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepRepr_RWStructuralResponsePropertyDefinitionRepresentation::Share (const Handle(StepRepr_StructuralResponsePropertyDefinitionRepresentation) &ent,
                                                                             Interface_EntityIterator& iter) const
{

  // Inherited fields of PropertyDefinitionRepresentation

  iter.AddItem (ent->StepRepr_PropertyDefinitionRepresentation::Definition().Value());

  iter.AddItem (ent->StepRepr_PropertyDefinitionRepresentation::UsedRepresentation());
}
