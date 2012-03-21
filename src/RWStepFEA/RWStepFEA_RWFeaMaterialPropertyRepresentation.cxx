// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <RWStepFEA_RWFeaMaterialPropertyRepresentation.ixx>

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
