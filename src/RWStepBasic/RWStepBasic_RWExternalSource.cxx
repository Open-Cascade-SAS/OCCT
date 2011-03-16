// File:	RWStepBasic_RWExternalSource.cxx
// Created:	Wed May 10 15:09:06 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <RWStepBasic_RWExternalSource.ixx>

//=======================================================================
//function : RWStepBasic_RWExternalSource
//purpose  : 
//=======================================================================

RWStepBasic_RWExternalSource::RWStepBasic_RWExternalSource ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWExternalSource::ReadStep (const Handle(StepData_StepReaderData)& data,
                                             const Standard_Integer num,
                                             Handle(Interface_Check)& ach,
                                             const Handle(StepBasic_ExternalSource) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,1,ach,"external_source") ) return;

  // Own fields of ExternalSource

  StepBasic_SourceItem aSourceId;
  data->ReadEntity (num, 1, "source_id", ach, aSourceId);

  // Initialize entity
  ent->Init(aSourceId);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWExternalSource::WriteStep (StepData_StepWriter& SW,
                                              const Handle(StepBasic_ExternalSource) &ent) const
{

  // Own fields of ExternalSource

  SW.Send (ent->SourceId().Value());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWExternalSource::Share (const Handle(StepBasic_ExternalSource) &ent,
                                          Interface_EntityIterator& iter) const
{

  // Own fields of ExternalSource

  iter.AddItem (ent->SourceId().Value());
}
