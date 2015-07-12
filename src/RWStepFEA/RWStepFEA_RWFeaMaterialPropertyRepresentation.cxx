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

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepFEA_RWFeaMaterialPropertyRepresentation.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepFEA_FeaMaterialPropertyRepresentation.hxx>
#include <StepRepr_DataEnvironment.hxx>
#include <StepRepr_PropertyDefinition.hxx>
#include <StepRepr_Representation.hxx>

//=======================================================================
//function : RWStepFEA_RWFeaMaterialPropertyRepresentation
//purpose  : 
//=======================================================================
RWStepFEA_RWFeaMaterialPropertyRepresentation::RWStepFEA_RWFeaMaterialPropertyRepresentation ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaMaterialPropertyRepresentation::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                              const Standard_Integer num,
                                                              Handle(Interface_Check)& ach,
                                                              const Handle(StepFEA_FeaMaterialPropertyRepresentation) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"fea_material_property_representation") ) return;

  // Inherited fields of PropertyDefinitionRepresentation

  Handle(StepRepr_PropertyDefinition) aPropertyDefinitionRepresentation_Definition;
  data->ReadEntity (num, 1, "property_definition_representation.definition", ach, STANDARD_TYPE(StepRepr_PropertyDefinition), aPropertyDefinitionRepresentation_Definition);
  StepRepr_RepresentedDefinition aDefinition;
  aDefinition.SetValue(aPropertyDefinitionRepresentation_Definition);

  Handle(StepRepr_Representation) aPropertyDefinitionRepresentation_UsedRepresentation;
  data->ReadEntity (num, 2, "property_definition_representation.used_representation", ach, STANDARD_TYPE(StepRepr_Representation), aPropertyDefinitionRepresentation_UsedRepresentation);

  // Inherited fields of MaterialPropertyRepresentation

  Handle(StepRepr_DataEnvironment) aMaterialPropertyRepresentation_DependentEnvironment;
  data->ReadEntity (num, 3, "material_property_representation.dependent_environment", ach, STANDARD_TYPE(StepRepr_DataEnvironment), aMaterialPropertyRepresentation_DependentEnvironment);

  // Initialize entity
  ent->Init(aDefinition,
            aPropertyDefinitionRepresentation_UsedRepresentation,
            aMaterialPropertyRepresentation_DependentEnvironment);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaMaterialPropertyRepresentation::WriteStep (StepData_StepWriter& SW,
                                                               const Handle(StepFEA_FeaMaterialPropertyRepresentation) &ent) const
{

  // Inherited fields of PropertyDefinitionRepresentation

  SW.Send (ent->StepRepr_PropertyDefinitionRepresentation::Definition().PropertyDefinition());

  SW.Send (ent->StepRepr_PropertyDefinitionRepresentation::UsedRepresentation());

  // Inherited fields of MaterialPropertyRepresentation

  SW.Send (ent->StepRepr_MaterialPropertyRepresentation::DependentEnvironment());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaMaterialPropertyRepresentation::Share (const Handle(StepFEA_FeaMaterialPropertyRepresentation) &ent,
                                                           Interface_EntityIterator& iter) const
{

  // Inherited fields of PropertyDefinitionRepresentation

  iter.AddItem (ent->StepRepr_PropertyDefinitionRepresentation::Definition().PropertyDefinition());

  iter.AddItem (ent->StepRepr_PropertyDefinitionRepresentation::UsedRepresentation());

  // Inherited fields of MaterialPropertyRepresentation

  iter.AddItem (ent->StepRepr_MaterialPropertyRepresentation::DependentEnvironment());
}
