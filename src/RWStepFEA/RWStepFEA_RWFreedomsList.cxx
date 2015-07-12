// Created on: 2002-12-14
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
#include <RWStepFEA_RWFreedomsList.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepFEA_DegreeOfFreedom.hxx>
#include <StepFEA_FreedomsList.hxx>
#include <StepFEA_HArray1OfDegreeOfFreedom.hxx>

//=======================================================================
//function : RWStepFEA_RWFreedomsList
//purpose  : 
//=======================================================================
RWStepFEA_RWFreedomsList::RWStepFEA_RWFreedomsList ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFreedomsList::ReadStep (const Handle(StepData_StepReaderData)& data,
                                         const Standard_Integer num,
                                         Handle(Interface_Check)& ach,
                                         const Handle(StepFEA_FreedomsList) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,1,ach,"freedoms_list") ) return;

  // Own fields of FreedomsList

  Handle(StepFEA_HArray1OfDegreeOfFreedom) aFreedoms;
  Standard_Integer sub1 = 0;
  if ( data->ReadSubList (num, 1, "freedoms", ach, sub1) ) {
    Standard_Integer nb0 = data->NbParams(sub1);
    aFreedoms = new StepFEA_HArray1OfDegreeOfFreedom (1, nb0);
    Standard_Integer num2 = sub1;
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      StepFEA_DegreeOfFreedom anIt0;
      data->ReadEntity (num2, i0, "degree_of_freedom", ach, anIt0);
      aFreedoms->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  ent->Init(aFreedoms);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFreedomsList::WriteStep (StepData_StepWriter& SW,
                                          const Handle(StepFEA_FreedomsList) &ent) const
{

  // Own fields of FreedomsList

  SW.OpenSub();
  for (Standard_Integer i0=1; i0 <= ent->Freedoms()->Length(); i0++ ) {
    StepFEA_DegreeOfFreedom Var0 = ent->Freedoms()->Value(i0);
    SW.Send (Var0.Value());
  }
  SW.CloseSub();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWFreedomsList::Share (const Handle(StepFEA_FreedomsList) &ent,
                                      Interface_EntityIterator& iter) const
{

  // Own fields of FreedomsList

  for (Standard_Integer i1=1; i1 <= ent->Freedoms()->Length(); i1++ ) {
    StepFEA_DegreeOfFreedom Var0 = ent->Freedoms()->Value(i1);
    iter.AddItem (Var0.Value());
  }
}
