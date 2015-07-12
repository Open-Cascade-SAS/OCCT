// Created on: 2000-07-03
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepRepr_RWPropertyDefinition.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_PropertyDefinition.hxx>

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
