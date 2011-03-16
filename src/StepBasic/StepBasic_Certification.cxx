// File:	StepBasic_Certification.cxx
// Created:	Fri Nov 26 16:26:34 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepBasic_Certification.ixx>

//=======================================================================
//function : StepBasic_Certification
//purpose  : 
//=======================================================================

StepBasic_Certification::StepBasic_Certification ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_Certification::Init (const Handle(TCollection_HAsciiString) &aName,
                                    const Handle(TCollection_HAsciiString) &aPurpose,
                                    const Handle(StepBasic_CertificationType) &aKind)
{

  theName = aName;

  thePurpose = aPurpose;

  theKind = aKind;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_Certification::Name () const
{
  return theName;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void StepBasic_Certification::SetName (const Handle(TCollection_HAsciiString) &aName)
{
  theName = aName;
}

//=======================================================================
//function : Purpose
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_Certification::Purpose () const
{
  return thePurpose;
}

//=======================================================================
//function : SetPurpose
//purpose  : 
//=======================================================================

void StepBasic_Certification::SetPurpose (const Handle(TCollection_HAsciiString) &aPurpose)
{
  thePurpose = aPurpose;
}

//=======================================================================
//function : Kind
//purpose  : 
//=======================================================================

Handle(StepBasic_CertificationType) StepBasic_Certification::Kind () const
{
  return theKind;
}

//=======================================================================
//function : SetKind
//purpose  : 
//=======================================================================

void StepBasic_Certification::SetKind (const Handle(StepBasic_CertificationType) &aKind)
{
  theKind = aKind;
}
