// File:	RWStepBasic_RWContract.cxx
// Created:	Fri Nov 26 16:26:37 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <RWStepBasic_RWContract.ixx>

//=======================================================================
//function : RWStepBasic_RWContract
//purpose  : 
//=======================================================================

RWStepBasic_RWContract::RWStepBasic_RWContract ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWContract::ReadStep (const Handle(StepData_StepReaderData)& data,
                                       const Standard_Integer num,
                                       Handle(Interface_Check)& ach,
                                       const Handle(StepBasic_Contract) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"contract") ) return;

  // Own fields of Contract

  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 1, "name", ach, aName);

  Handle(TCollection_HAsciiString) aPurpose;
  data->ReadString (num, 2, "purpose", ach, aPurpose);

  Handle(StepBasic_ContractType) aKind;
  data->ReadEntity (num, 3, "kind", ach, STANDARD_TYPE(StepBasic_ContractType), aKind);

  // Initialize entity
  ent->Init(aName,
            aPurpose,
            aKind);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWContract::WriteStep (StepData_StepWriter& SW,
                                        const Handle(StepBasic_Contract) &ent) const
{

  // Own fields of Contract

  SW.Send (ent->Name());

  SW.Send (ent->Purpose());

  SW.Send (ent->Kind());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWContract::Share (const Handle(StepBasic_Contract) &ent,
                                    Interface_EntityIterator& iter) const
{

  // Own fields of Contract

  iter.AddItem (ent->Kind());
}
