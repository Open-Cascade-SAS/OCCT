// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepElement_RWCurveElementEndReleasePacket.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepElement_CurveElementEndReleasePacket.hxx>

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
