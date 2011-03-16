// File:	RWStepBasic_RWGeneralProperty.cxx
// Created:	Wed May 10 15:09:07 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <RWStepBasic_RWGeneralProperty.ixx>

//=======================================================================
//function : RWStepBasic_RWGeneralProperty
//purpose  : 
//=======================================================================

RWStepBasic_RWGeneralProperty::RWStepBasic_RWGeneralProperty ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWGeneralProperty::ReadStep (const Handle(StepData_StepReaderData)& data,
                                              const Standard_Integer num,
                                              Handle(Interface_Check)& ach,
                                              const Handle(StepBasic_GeneralProperty) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"general_property") ) return;

  // Own fields of GeneralProperty

  Handle(TCollection_HAsciiString) aId;
  data->ReadString (num, 1, "id", ach, aId);

  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 2, "name", ach, aName);

  Handle(TCollection_HAsciiString) aDescription;
  Standard_Boolean hasDescription = Standard_True;
  if ( data->IsParamDefined (num,3) ) {
    data->ReadString (num, 3, "description", ach, aDescription);
  }
  else {
    hasDescription = Standard_False;
  }

  // Initialize entity
  ent->Init(aId,
            aName,
            hasDescription,
            aDescription);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWGeneralProperty::WriteStep (StepData_StepWriter& SW,
                                               const Handle(StepBasic_GeneralProperty) &ent) const
{

  // Own fields of GeneralProperty

  SW.Send (ent->Id());

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

void RWStepBasic_RWGeneralProperty::Share (const Handle(StepBasic_GeneralProperty) &ent,
                                           Interface_EntityIterator& iter) const
{

  // Own fields of GeneralProperty
}
