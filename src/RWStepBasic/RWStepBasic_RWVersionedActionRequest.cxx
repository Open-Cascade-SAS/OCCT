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
#include <RWStepBasic_RWVersionedActionRequest.hxx>
#include <StepBasic_VersionedActionRequest.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

//=======================================================================
//function : RWStepBasic_RWVersionedActionRequest
//purpose  : 
//=======================================================================
RWStepBasic_RWVersionedActionRequest::RWStepBasic_RWVersionedActionRequest ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWVersionedActionRequest::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                     const Standard_Integer num,
                                                     Handle(Interface_Check)& ach,
                                                     const Handle(StepBasic_VersionedActionRequest) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,4,ach,"versioned_action_request") ) return;

  // Own fields of VersionedActionRequest

  Handle(TCollection_HAsciiString) aId;
  data->ReadString (num, 1, "id", ach, aId);

  Handle(TCollection_HAsciiString) aVersion;
  data->ReadString (num, 2, "version", ach, aVersion);

  Handle(TCollection_HAsciiString) aPurpose;
  data->ReadString (num, 3, "purpose", ach, aPurpose);

  Handle(TCollection_HAsciiString) aDescription;
  Standard_Boolean hasDescription = Standard_True;
  if ( data->IsParamDefined (num,4) ) {
    data->ReadString (num, 4, "description", ach, aDescription);
  }
  else {
    hasDescription = Standard_False;
  }

  // Initialize entity
  ent->Init(aId,
            aVersion,
            aPurpose,
            hasDescription,
            aDescription);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWVersionedActionRequest::WriteStep (StepData_StepWriter& SW,
                                                      const Handle(StepBasic_VersionedActionRequest) &ent) const
{

  // Own fields of VersionedActionRequest

  SW.Send (ent->Id());

  SW.Send (ent->Version());

  SW.Send (ent->Purpose());

  if ( ent->HasDescription() ) {
    SW.Send (ent->Description());
  }
  else SW.SendUndef();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWVersionedActionRequest::Share (const Handle(StepBasic_VersionedActionRequest) &,
                                                  Interface_EntityIterator&) const
{

  // Own fields of VersionedActionRequest
}
