// File:	RWStepElement_RWCurveElementEndReleasePacket.cxx
// Created:	Thu Dec 12 17:29:00 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepElement_RWCurveElementEndReleasePacket.ixx>

//=======================================================================
//function : RWStepElement_RWCurveElementEndReleasePacket
//purpose  : 
//=======================================================================

RWStepElement_RWCurveElementEndReleasePacket::RWStepElement_RWCurveElementEndReleasePacket ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepElement_RWCurveElementEndReleasePacket::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                             const Standard_Integer num,
                                                             Handle(Interface_Check)& ach,
                                                             const Handle(StepElement_CurveElementEndReleasePacket) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"curve_element_end_release_packet") ) return;

  // Own fields of CurveElementEndReleasePacket

  StepElement_CurveElementFreedom aReleaseFreedom;
  data->ReadEntity (num, 1, "release_freedom", ach, aReleaseFreedom);

  Standard_Real aReleaseStiffness;
  data->ReadReal (num, 2, "release_stiffness", ach, aReleaseStiffness);

  // Initialize entity
  ent->Init(aReleaseFreedom,
            aReleaseStiffness);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepElement_RWCurveElementEndReleasePacket::WriteStep (StepData_StepWriter& SW,
                                                              const Handle(StepElement_CurveElementEndReleasePacket) &ent) const
{

  // Own fields of CurveElementEndReleasePacket

  SW.Send (ent->ReleaseFreedom().Value());

  SW.Send (ent->ReleaseStiffness());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepElement_RWCurveElementEndReleasePacket::Share (const Handle(StepElement_CurveElementEndReleasePacket) &ent,
                                                          Interface_EntityIterator& iter) const
{

  // Own fields of CurveElementEndReleasePacket

  iter.AddItem (ent->ReleaseFreedom().Value());
}
