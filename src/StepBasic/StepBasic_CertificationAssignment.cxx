// File:	StepBasic_CertificationAssignment.cxx
// Created:	Fri Nov 26 16:26:34 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepBasic_CertificationAssignment.ixx>

//=======================================================================
//function : StepBasic_CertificationAssignment
//purpose  : 
//=======================================================================

StepBasic_CertificationAssignment::StepBasic_CertificationAssignment ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_CertificationAssignment::Init (const Handle(StepBasic_Certification) &aAssignedCertification)
{

  theAssignedCertification = aAssignedCertification;
}

//=======================================================================
//function : AssignedCertification
//purpose  : 
//=======================================================================

Handle(StepBasic_Certification) StepBasic_CertificationAssignment::AssignedCertification () const
{
  return theAssignedCertification;
}

//=======================================================================
//function : SetAssignedCertification
//purpose  : 
//=======================================================================

void StepBasic_CertificationAssignment::SetAssignedCertification (const Handle(StepBasic_Certification) &aAssignedCertification)
{
  theAssignedCertification = aAssignedCertification;
}
