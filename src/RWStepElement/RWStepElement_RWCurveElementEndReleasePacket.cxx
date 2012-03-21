// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

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
