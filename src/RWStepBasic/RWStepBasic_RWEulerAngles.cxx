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

#include <RWStepBasic_RWEulerAngles.ixx>
#include <TColStd_HArray1OfReal.hxx>
#include <Standard_Real.hxx>

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

void RWStepBasic_RWEulerAngles::Share (const Handle(StepBasic_EulerAngles) &ent,
                                       Interface_EntityIterator& iter) const
{

  // Own fields of EulerAngles
}
