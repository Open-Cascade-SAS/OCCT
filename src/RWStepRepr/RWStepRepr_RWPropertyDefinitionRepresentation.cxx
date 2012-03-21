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
