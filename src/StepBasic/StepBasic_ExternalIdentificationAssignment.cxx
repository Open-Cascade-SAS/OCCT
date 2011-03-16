// File:	StepBasic_ExternalIdentificationAssignment.cxx
// Created:	Wed May 10 15:09:06 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <StepBasic_ExternalIdentificationAssignment.ixx>

//=======================================================================
//function : StepBasic_ExternalIdentificationAssignment
//purpose  : 
//=======================================================================

StepBasic_ExternalIdentificationAssignment::StepBasic_ExternalIdentificationAssignment ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_ExternalIdentificationAssignment::Init (const Handle(TCollection_HAsciiString) &aIdentificationAssignment_AssignedId,
                                                       const Handle(StepBasic_IdentificationRole) &aIdentificationAssignment_Role,
                                                       const Handle(StepBasic_ExternalSource) &aSource)
{
  StepBasic_IdentificationAssignment::Init(aIdentificationAssignment_AssignedId,
                                           aIdentificationAssignment_Role);

  theSource = aSource;
}

//=======================================================================
//function : Source
//purpose  : 
//=======================================================================

Handle(StepBasic_ExternalSource) StepBasic_ExternalIdentificationAssignment::Source () const
{
  return theSource;
}

//=======================================================================
//function : SetSource
//purpose  : 
//=======================================================================

void StepBasic_ExternalIdentificationAssignment::SetSource (const Handle(StepBasic_ExternalSource) &aSource)
{
  theSource = aSource;
}
