// File:	StepFEA_FreedomAndCoefficient.cxx
// Created:	Sat Dec 14 11:02:05 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_FreedomAndCoefficient.ixx>

//=======================================================================
//function : StepFEA_FreedomAndCoefficient
//purpose  : 
//=======================================================================

StepFEA_FreedomAndCoefficient::StepFEA_FreedomAndCoefficient ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_FreedomAndCoefficient::Init (const StepFEA_DegreeOfFreedom &aFreedom,
                                          const StepElement_MeasureOrUnspecifiedValue &aA)
{

  theFreedom = aFreedom;

  theA = aA;
}

//=======================================================================
//function : Freedom
//purpose  : 
//=======================================================================

StepFEA_DegreeOfFreedom StepFEA_FreedomAndCoefficient::Freedom () const
{
  return theFreedom;
}

//=======================================================================
//function : SetFreedom
//purpose  : 
//=======================================================================

void StepFEA_FreedomAndCoefficient::SetFreedom (const StepFEA_DegreeOfFreedom &aFreedom)
{
  theFreedom = aFreedom;
}

//=======================================================================
//function : A
//purpose  : 
//=======================================================================

StepElement_MeasureOrUnspecifiedValue StepFEA_FreedomAndCoefficient::A () const
{
  return theA;
}

//=======================================================================
//function : SetA
//purpose  : 
//=======================================================================

void StepFEA_FreedomAndCoefficient::SetA (const StepElement_MeasureOrUnspecifiedValue &aA)
{
  theA = aA;
}
