// File:	RWStepRepr_RWPropertyDefinition.cxx
// Created:	Mon Jul  3 16:29:03 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

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
