// File:	RWStepFEA_RWFeaMaterialPropertyRepresentation.cxx
// Created:	Thu Dec 12 17:51:05 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

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
