// Created on: 2000-05-11
// Created by: data exchange team
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
#include <RWStepBasic_RWCharacterizedObject.hxx>
#include <StepBasic_CharacterizedObject.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

//=======================================================================
//function : RWStepBasic_RWCharacterizedObject
//purpose  : 
//=======================================================================
RWStepBasic_RWCharacterizedObject::RWStepBasic_RWCharacterizedObject ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWCharacterizedObject::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                  const Standard_Integer num,
                                                  Handle(Interface_Check)& ach,
                                                  const Handle(StepBasic_CharacterizedObject) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"characterized_object") ) return;

  // Own fields of CharacterizedObject

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

  // Initialize entity
  ent->Init(aName,
            hasDescription,
            aDescription);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWCharacterizedObject::WriteStep (StepData_StepWriter& SW,
                                                   const Handle(StepBasic_CharacterizedObject) &ent) const
{

  // Own fields of CharacterizedObject

  SW.Send (ent->Name());

  if ( ent->HasDescription() ) {
    SW.Send (ent->Description());
  }
  else SW.SendUndef();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWCharacterizedObject::Share (const Handle(StepBasic_CharacterizedObject) &,
                                               Interface_EntityIterator&) const
{
  // Own fields of CharacterizedObject
}
