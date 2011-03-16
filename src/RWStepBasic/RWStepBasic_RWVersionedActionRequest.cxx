// File:	RWStepBasic_RWVersionedActionRequest.cxx
// Created:	Fri Nov 26 16:26:41 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

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
