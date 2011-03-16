// File:	RWStepFEA_RWFreedomAndCoefficient.cxx
// Created:	Sat Dec 14 11:02:05 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepFEA_RWFreedomAndCoefficient.ixx>

//=======================================================================
//function : RWStepFEA_RWFreedomAndCoefficient
//purpose  : 
//=======================================================================

RWStepFEA_RWFreedomAndCoefficient::RWStepFEA_RWFreedomAndCoefficient ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFreedomAndCoefficient::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                  const Standard_Integer num,
                                                  Handle(Interface_Check)& ach,
                                                  const Handle(StepFEA_FreedomAndCoefficient) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"freedom_and_coefficient") ) return;

  // Own fields of FreedomAndCoefficient

  StepFEA_DegreeOfFreedom aFreedom;
  data->ReadEntity (num, 1, "freedom", ach, aFreedom);

  StepElement_MeasureOrUnspecifiedValue aA;
  data->ReadEntity (num, 2, "a", ach, aA);

  // Initialize entity
  ent->Init(aFreedom,
            aA);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFreedomAndCoefficient::WriteStep (StepData_StepWriter& SW,
                                                   const Handle(StepFEA_FreedomAndCoefficient) &ent) const
{

  // Own fields of FreedomAndCoefficient

  SW.Send (ent->Freedom().Value());

  SW.Send (ent->A().Value());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWFreedomAndCoefficient::Share (const Handle(StepFEA_FreedomAndCoefficient) &ent,
                                               Interface_EntityIterator& iter) const
{

  // Own fields of FreedomAndCoefficient

  iter.AddItem (ent->Freedom().Value());

  iter.AddItem (ent->A().Value());
}
