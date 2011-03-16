// File:	RWStepRepr_RWMaterialPropertyRepresentation.cxx
// Created:	Thu Dec 12 17:15:59 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepRepr_RWMaterialPropertyRepresentation.ixx>

//=======================================================================
//function : RWStepRepr_RWMaterialPropertyRepresentation
//purpose  : 
//=======================================================================

RWStepRepr_RWMaterialPropertyRepresentation::RWStepRepr_RWMaterialPropertyRepresentation ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWMaterialPropertyRepresentation::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                            const Standard_Integer num,
                                                            Handle(Interface_Check)& ach,
                                                            const Handle(StepRepr_MaterialPropertyRepresentation) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"material_property_representation") ) return;

  // Inherited fields of PropertyDefinitionRepresentation

  StepRepr_RepresentedDefinition aPropertyDefinitionRepresentation_Definition;
  data->ReadEntity (num, 1, "property_definition_representation.definition", ach, aPropertyDefinitionRepresentation_Definition);

  Handle(StepRepr_Representation) aPropertyDefinitionRepresentation_UsedRepresentation;
  data->ReadEntity (num, 2, "property_definition_representation.used_representation", ach, STANDARD_TYPE(StepRepr_Representation), aPropertyDefinitionRepresentation_UsedRepresentation);

  // Own fields of MaterialPropertyRepresentation

  Handle(StepRepr_DataEnvironment) aDependentEnvironment;
  data->ReadEntity (num, 3, "dependent_environment", ach, STANDARD_TYPE(StepRepr_DataEnvironment), aDependentEnvironment);

  // Initialize entity
  ent->Init(aPropertyDefinitionRepresentation_Definition,
            aPropertyDefinitionRepresentation_UsedRepresentation,
            aDependentEnvironment);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWMaterialPropertyRepresentation::WriteStep (StepData_StepWriter& SW,
                                                             const Handle(StepRepr_MaterialPropertyRepresentation) &ent) const
{

  // Inherited fields of PropertyDefinitionRepresentation

  SW.Send (ent->StepRepr_PropertyDefinitionRepresentation::Definition().Value());

  SW.Send (ent->StepRepr_PropertyDefinitionRepresentation::UsedRepresentation());

  // Own fields of MaterialPropertyRepresentation

  SW.Send (ent->DependentEnvironment());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepRepr_RWMaterialPropertyRepresentation::Share (const Handle(StepRepr_MaterialPropertyRepresentation) &ent,
                                                         Interface_EntityIterator& iter) const
{

  // Inherited fields of PropertyDefinitionRepresentation

  iter.AddItem (ent->StepRepr_PropertyDefinitionRepresentation::Definition().Value());

  iter.AddItem (ent->StepRepr_PropertyDefinitionRepresentation::UsedRepresentation());

  // Own fields of MaterialPropertyRepresentation

  iter.AddItem (ent->DependentEnvironment());
}
