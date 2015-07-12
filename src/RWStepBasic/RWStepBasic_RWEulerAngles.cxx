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
#include <RWStepBasic_RWEulerAngles.hxx>
#include <Standard_Real.hxx>
#include <StepBasic_EulerAngles.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <TColStd_HArray1OfReal.hxx>

//=======================================================================
//function : RWStepBasic_RWEulerAngles
//purpose  : 
//=======================================================================
RWStepBasic_RWEulerAngles::RWStepBasic_RWEulerAngles ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWEulerAngles::ReadStep (const Handle(StepData_StepReaderData)& data,
                                          const Standard_Integer num,
                                          Handle(Interface_Check)& ach,
                                          const Handle(StepBasic_EulerAngles) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,1,ach,"euler_angles") ) return;

  // Own fields of EulerAngles

  Handle(TColStd_HArray1OfReal) aAngles;
  Standard_Integer sub1 = 0;
  if ( data->ReadSubList (num, 1, "angles", ach, sub1) ) {
    Standard_Integer nb0 = data->NbParams(sub1);
    aAngles = new TColStd_HArray1OfReal (1, nb0);
    Standard_Integer num2 = sub1;
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      Standard_Real anIt0;
      data->ReadReal (num2, i0, "real", ach, anIt0);
      aAngles->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  ent->Init(aAngles);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWEulerAngles::WriteStep (StepData_StepWriter& SW,
                                           const Handle(StepBasic_EulerAngles) &ent) const
{

  // Own fields of EulerAngles

  SW.OpenSub();
  for (Standard_Integer i0=1; i0 <= ent->Angles()->Length(); i0++ ) {
    Standard_Real Var0 = ent->Angles()->Value(i0);
    SW.Send (Var0);
  }
  SW.CloseSub();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWEulerAngles::Share (const Handle(StepBasic_EulerAngles) &,
                                       Interface_EntityIterator&) const
{
  // Own fields of EulerAngles
}
