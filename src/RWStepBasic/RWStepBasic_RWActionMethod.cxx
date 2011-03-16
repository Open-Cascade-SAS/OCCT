// File:	RWStepBasic_RWActionMethod.cxx
// Created:	Fri Nov 26 16:26:30 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <RWStepBasic_RWActionMethod.ixx>

//=======================================================================
//function : RWStepBasic_RWActionMethod
//purpose  : 
//=======================================================================

RWStepBasic_RWActionMethod::RWStepBasic_RWActionMethod ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWActionMethod::ReadStep (const Handle(StepData_StepReaderData)& data,
                                           const Standard_Integer num,
                                           Handle(Interface_Check)& ach,
                                           const Handle(StepBasic_ActionMethod) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,4,ach,"action_method") ) return;

  // Own fields of ActionMethod

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

  Handle(TCollection_HAsciiString) aConsequence;
  data->ReadString (num, 3, "consequence", ach, aConsequence);

  Handle(TCollection_HAsciiString) aPurpose;
  data->ReadString (num, 4, "purpose", ach, aPurpose);

  // Initialize entity
  ent->Init(aName,
            hasDescription,
            aDescription,
            aConsequence,
            aPurpose);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWActionMethod::WriteStep (StepData_StepWriter& SW,
                                            const Handle(StepBasic_ActionMethod) &ent) const
{

  // Own fields of ActionMethod

  SW.Send (ent->Name());

  if ( ent->HasDescription() ) {
    SW.Send (ent->Description());
  }
  else SW.SendUndef();

  SW.Send (ent->Consequence());

  SW.Send (ent->Purpose());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWActionMethod::Share (const Handle(StepBasic_ActionMethod) &ent,
                                        Interface_EntityIterator& iter) const
{

  // Own fields of ActionMethod
}
