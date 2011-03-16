// File:	RWStepBasic_RWAction.cxx
// Created:	Fri Nov 26 16:26:28 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <RWStepBasic_RWAction.ixx>

//=======================================================================
//function : RWStepBasic_RWAction
//purpose  : 
//=======================================================================

RWStepBasic_RWAction::RWStepBasic_RWAction ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWAction::ReadStep (const Handle(StepData_StepReaderData)& data,
                                     const Standard_Integer num,
                                     Handle(Interface_Check)& ach,
                                     const Handle(StepBasic_Action) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"action") ) return;

  // Own fields of Action

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

  Handle(StepBasic_ActionMethod) aChosenMethod;
  data->ReadEntity (num, 3, "chosen_method", ach, STANDARD_TYPE(StepBasic_ActionMethod), aChosenMethod);

  // Initialize entity
  ent->Init(aName,
            hasDescription,
            aDescription,
            aChosenMethod);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWAction::WriteStep (StepData_StepWriter& SW,
                                      const Handle(StepBasic_Action) &ent) const
{

  // Own fields of Action

  SW.Send (ent->Name());

  if ( ent->HasDescription() ) {
    SW.Send (ent->Description());
  }
  else SW.SendUndef();

  SW.Send (ent->ChosenMethod());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWAction::Share (const Handle(StepBasic_Action) &ent,
                                  Interface_EntityIterator& iter) const
{

  // Own fields of Action

  iter.AddItem (ent->ChosenMethod());
}
