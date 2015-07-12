// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepBasic_RWContract.hxx>
#include <StepBasic_Contract.hxx>
#include <StepBasic_ContractType.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

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
