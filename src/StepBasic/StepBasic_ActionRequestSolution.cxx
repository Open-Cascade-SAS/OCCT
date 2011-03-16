// File:	StepBasic_ActionRequestSolution.cxx
// Created:	Fri Nov 26 16:26:30 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepBasic_ActionRequestSolution.ixx>

//=======================================================================
//function : StepBasic_ActionRequestSolution
//purpose  : 
//=======================================================================

StepBasic_ActionRequestSolution::StepBasic_ActionRequestSolution ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_ActionRequestSolution::Init (const Handle(StepBasic_ActionMethod) &aMethod,
                                            const Handle(StepBasic_VersionedActionRequest) &aRequest)
{

  theMethod = aMethod;

  theRequest = aRequest;
}

//=======================================================================
//function : Method
//purpose  : 
//=======================================================================

Handle(StepBasic_ActionMethod) StepBasic_ActionRequestSolution::Method () const
{
  return theMethod;
}

//=======================================================================
//function : SetMethod
//purpose  : 
//=======================================================================

void StepBasic_ActionRequestSolution::SetMethod (const Handle(StepBasic_ActionMethod) &aMethod)
{
  theMethod = aMethod;
}

//=======================================================================
//function : Request
//purpose  : 
//=======================================================================

Handle(StepBasic_VersionedActionRequest) StepBasic_ActionRequestSolution::Request () const
{
  return theRequest;
}

//=======================================================================
//function : SetRequest
//purpose  : 
//=======================================================================

void StepBasic_ActionRequestSolution::SetRequest (const Handle(StepBasic_VersionedActionRequest) &aRequest)
{
  theRequest = aRequest;
}
