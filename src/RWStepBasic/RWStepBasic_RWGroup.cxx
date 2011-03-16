// File:	RWStepBasic_RWGroup.cxx
// Created:	Wed May 10 15:09:07 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <RWStepBasic_RWGroup.ixx>

//=======================================================================
//function : RWStepBasic_RWGroup
//purpose  : 
//=======================================================================

RWStepBasic_RWGroup::RWStepBasic_RWGroup ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWGroup::ReadStep (const Handle(StepData_StepReaderData)& data,
                                    const Standard_Integer num,
                                    Handle(Interface_Check)& ach,
                                    const Handle(StepBasic_Group) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"group") ) return;

  // Own fields of Group

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

void RWStepBasic_RWGroup::WriteStep (StepData_StepWriter& SW,
                                     const Handle(StepBasic_Group) &ent) const
{

  // Own fields of Group

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

void RWStepBasic_RWGroup::Share (const Handle(StepBasic_Group) &ent,
                                 Interface_EntityIterator& iter) const
{

  // Own fields of Group
}
