// Created on: 2000-07-03
// Created by: Andrey BETENEV
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <RWStepRepr_RWPropertyDefinition.ixx>

//=======================================================================
//function : RWStepRepr_RWPropertyDefinition
//purpose  : 
//=======================================================================

RWStepRepr_RWPropertyDefinition::RWStepRepr_RWPropertyDefinition ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWPropertyDefinition::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                const Standard_Integer num,
                                                Handle(Interface_Check)& ach,
                                                const Handle(StepRepr_PropertyDefinition) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"property_definition") ) return;

  // Own fields of PropertyDefinition

  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 1, "name", ach, aName);

  Handle(TCollection_HAsciiString) aDescription;
  Standard_Boolean hasDescription = Standard_True;
  if ( data->IsParamDefined (num,2) ) {
    data->ReadString (num, 2, "description", ach, aDescription);
  }
  else {
    hasDescription = Standard_False;
  }

  StepRepr_CharacterizedDefinition aDefinition;
  data->ReadEntity (num, 3, "definition", ach, aDefinition);

  // Initialize entity
  ent->Init(aName,
            hasDescription,
            aDescription,
            aDefinition);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWPropertyDefinition::WriteStep (StepData_StepWriter& SW,
                                                 const Handle(StepRepr_PropertyDefinition) &ent) const
{

  // Own fields of PropertyDefinition

  SW.Send (ent->Name());

  if ( ent->HasDescription() ) {
    SW.Send (ent->Description());
  }
  else SW.SendUndef();

  SW.Send (ent->Definition().Value());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepRepr_RWPropertyDefinition::Share (const Handle(StepRepr_PropertyDefinition) &ent,
                                             Interface_EntityIterator& iter) const
{

  // Own fields of PropertyDefinition

  iter.AddItem (ent->Definition().Value());
}
