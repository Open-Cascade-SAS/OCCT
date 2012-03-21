// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <RWStepBasic_RWVersionedActionRequest.ixx>

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

void RWStepBasic_RWVersionedActionRequest::Share (const Handle(StepBasic_VersionedActionRequest) &ent,
                                                  Interface_EntityIterator& iter) const
{

  // Own fields of VersionedActionRequest
}
