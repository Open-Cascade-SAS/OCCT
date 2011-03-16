// File:	RWStepRepr_RWPropertyDefinitionRelationship.cxx
// Created:	Thu Dec 12 17:15:59 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepRepr_RWPropertyDefinitionRelationship.ixx>

//=======================================================================
//function : RWStepRepr_RWPropertyDefinitionRelationship
//purpose  : 
//=======================================================================

RWStepRepr_RWPropertyDefinitionRelationship::RWStepRepr_RWPropertyDefinitionRelationship ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWPropertyDefinitionRelationship::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                            const Standard_Integer num,
                                                            Handle(Interface_Check)& ach,
                                                            const Handle(StepRepr_PropertyDefinitionRelationship) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,4,ach,"property_definition_relationship") ) return;

  // Own fields of PropertyDefinitionRelationship

  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 1, "name", ach, aName);

  Handle(TCollection_HAsciiString) aDescription;
  data->ReadString (num, 2, "description", ach, aDescription);

  Handle(StepRepr_PropertyDefinition) aRelatingPropertyDefinition;
  data->ReadEntity (num, 3, "relating_property_definition", ach, STANDARD_TYPE(StepRepr_PropertyDefinition), aRelatingPropertyDefinition);

  Handle(StepRepr_PropertyDefinition) aRelatedPropertyDefinition;
  data->ReadEntity (num, 4, "related_property_definition", ach, STANDARD_TYPE(StepRepr_PropertyDefinition), aRelatedPropertyDefinition);

  // Initialize entity
  ent->Init(aName,
            aDescription,
            aRelatingPropertyDefinition,
            aRelatedPropertyDefinition);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWPropertyDefinitionRelationship::WriteStep (StepData_StepWriter& SW,
                                                             const Handle(StepRepr_PropertyDefinitionRelationship) &ent) const
{

  // Own fields of PropertyDefinitionRelationship

  SW.Send (ent->Name());

  SW.Send (ent->Description());

  SW.Send (ent->RelatingPropertyDefinition());

  SW.Send (ent->RelatedPropertyDefinition());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepRepr_RWPropertyDefinitionRelationship::Share (const Handle(StepRepr_PropertyDefinitionRelationship) &ent,
                                                         Interface_EntityIterator& iter) const
{

  // Own fields of PropertyDefinitionRelationship

  iter.AddItem (ent->RelatingPropertyDefinition());

  iter.AddItem (ent->RelatedPropertyDefinition());
}
