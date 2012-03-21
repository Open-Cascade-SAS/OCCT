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
