// File:	StepFEA_FreedomsList.cxx
// Created:	Sat Dec 14 11:02:05 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_FreedomsList.ixx>

//=======================================================================
//function : StepFEA_FreedomsList
//purpose  : 
//=======================================================================

StepFEA_FreedomsList::StepFEA_FreedomsList ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_FreedomsList::Init (const Handle(StepFEA_HArray1OfDegreeOfFreedom) &aFreedoms)
{

  theFreedoms = aFreedoms;
}

//=======================================================================
//function : Freedoms
//purpose  : 
//=======================================================================

Handle(StepFEA_HArray1OfDegreeOfFreedom) StepFEA_FreedomsList::Freedoms () const
{
  return theFreedoms;
}

//=======================================================================
//function : SetFreedoms
//purpose  : 
//=======================================================================

void StepFEA_FreedomsList::SetFreedoms (const Handle(StepFEA_HArray1OfDegreeOfFreedom) &aFreedoms)
{
  theFreedoms = aFreedoms;
}
