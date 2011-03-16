// File:	RWStepRepr_RWPropertyDefinitionRepresentation.cxx
// Created:	Thu Dec 12 17:15:59 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepRepr_RWPropertyDefinitionRepresentation.ixx>

//=======================================================================
//function : RWStepRepr_RWPropertyDefinitionRepresentation
//purpose  : 
//=======================================================================

RWStepRepr_RWPropertyDefinitionRepresentation::RWStepRepr_RWPropertyDefinitionRepresentation ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWPropertyDefinitionRepresentation::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                              const Standard_Integer num,
                                                              Handle(Interface_Check)& ach,
                                                              const Handle(StepRepr_PropertyDefinitionRepresentation) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"property_definition_representation") ) return;

  // Own fields of PropertyDefinitionRepresentation

  StepRepr_RepresentedDefinition aDefinition;
  data->ReadEntity (num, 1, "definition", ach, aDefinition);

  Handle(StepRepr_Representation) aUsedRepresentation;
  data->ReadEntity (num, 2, "used_representation", ach, STANDARD_TYPE(StepRepr_Representation), aUsedRepresentation);

  // Initialize entity
  ent->Init(aDefinition,
            aUsedRepresentation);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWPropertyDefinitionRepresentation::WriteStep (StepData_StepWriter& SW,
                                                               const Handle(StepRepr_PropertyDefinitionRepresentation) &ent) const
{

  // Own fields of PropertyDefinitionRepresentation

  SW.Send (ent->Definition().Value());

  SW.Send (ent->UsedRepresentation());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepRepr_RWPropertyDefinitionRepresentation::Share (const Handle(StepRepr_PropertyDefinitionRepresentation) &ent,
                                                           Interface_EntityIterator& iter) const
{

  // Own fields of PropertyDefinitionRepresentation

  iter.AddItem (ent->Definition().Value());

  iter.AddItem (ent->UsedRepresentation());
}
