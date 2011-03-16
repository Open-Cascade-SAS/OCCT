// File:	RWStepFEA_RWFreedomsList.cxx
// Created:	Sat Dec 14 11:02:06 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepFEA_RWFreedomsList.ixx>
#include <StepFEA_HArray1OfDegreeOfFreedom.hxx>
#include <StepFEA_DegreeOfFreedom.hxx>

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
