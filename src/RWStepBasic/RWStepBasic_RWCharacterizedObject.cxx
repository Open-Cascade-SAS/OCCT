// File:	RWStepBasic_RWCharacterizedObject.cxx
// Created:	Thu May 11 16:38:00 2000 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <RWStepBasic_RWCharacterizedObject.ixx>

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

void RWStepBasic_RWCharacterizedObject::Share (const Handle(StepBasic_CharacterizedObject) &ent,
                                               Interface_EntityIterator& iter) const
{

  // Own fields of CharacterizedObject
}
