// File:	StepBasic_CertificationType.cxx
// Created:	Fri Nov 26 16:26:35 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepBasic_CertificationType.ixx>

//=======================================================================
//function : StepBasic_CertificationType
//purpose  : 
//=======================================================================

StepBasic_CertificationType::StepBasic_CertificationType ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_CertificationType::Init (const Handle(TCollection_HAsciiString) &aDescription)
{

  theDescription = aDescription;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_CertificationType::Description () const
{
  return theDescription;
}

//=======================================================================
//function : SetDescription
//purpose  : 
//=======================================================================

void StepBasic_CertificationType::SetDescription (const Handle(TCollection_HAsciiString) &aDescription)
{
  theDescription = aDescription;
}
