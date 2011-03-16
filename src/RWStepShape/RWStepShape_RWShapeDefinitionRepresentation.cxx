// File:	RWStepShape_RWShapeDefinitionRepresentation.cxx
// Created:	Fri Nov 26 16:26:39 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <RWStepShape_RWShapeDefinitionRepresentation.ixx>

//=======================================================================
//function : RWStepShape_RWShapeDefinitionRepresentation
//purpose  : 
//=======================================================================

RWStepShape_RWShapeDefinitionRepresentation::RWStepShape_RWShapeDefinitionRepresentation ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepShape_RWShapeDefinitionRepresentation::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                            const Standard_Integer num,
                                                            Handle(Interface_Check)& ach,
                                                            const Handle(StepShape_ShapeDefinitionRepresentation) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"shape_definition_representation") ) return;

  // Inherited fields of PropertyDefinitionRepresentation

  StepRepr_RepresentedDefinition aPropertyDefinitionRepresentation_Definition;
  data->ReadEntity (num, 1, "property_definition_representation.definition", ach, aPropertyDefinitionRepresentation_Definition);

  Handle(StepRepr_Representation) aPropertyDefinitionRepresentation_UsedRepresentation;
  data->ReadEntity (num, 2, "property_definition_representation.used_representation", ach, STANDARD_TYPE(StepRepr_Representation), aPropertyDefinitionRepresentation_UsedRepresentation);

  // Initialize entity
  ent->Init(aPropertyDefinitionRepresentation_Definition,
            aPropertyDefinitionRepresentation_UsedRepresentation);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepShape_RWShapeDefinitionRepresentation::WriteStep (StepData_StepWriter& SW,
                                                             const Handle(StepShape_ShapeDefinitionRepresentation) &ent) const
{

  // Inherited fields of PropertyDefinitionRepresentation

  SW.Send (ent->StepRepr_PropertyDefinitionRepresentation::Definition().Value());

  SW.Send (ent->StepRepr_PropertyDefinitionRepresentation::UsedRepresentation());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepShape_RWShapeDefinitionRepresentation::Share (const Handle(StepShape_ShapeDefinitionRepresentation) &ent,
                                                         Interface_EntityIterator& iter) const
{

  // Inherited fields of PropertyDefinitionRepresentation

  iter.AddItem (ent->StepRepr_PropertyDefinitionRepresentation::Definition().Value());

  iter.AddItem (ent->StepRepr_PropertyDefinitionRepresentation::UsedRepresentation());
}
