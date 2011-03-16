// File:	StepBasic_Contract.cxx
// Created:	Fri Nov 26 16:26:37 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepBasic_Contract.ixx>

//=======================================================================
//function : StepBasic_Contract
//purpose  : 
//=======================================================================

StepBasic_Contract::StepBasic_Contract ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_Contract::Init (const Handle(TCollection_HAsciiString) &aName,
                               const Handle(TCollection_HAsciiString) &aPurpose,
                               const Handle(StepBasic_ContractType) &aKind)
{

  theName = aName;

  thePurpose = aPurpose;

  theKind = aKind;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_Contract::Name () const
{
  return theName;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void StepBasic_Contract::SetName (const Handle(TCollection_HAsciiString) &aName)
{
  theName = aName;
}

//=======================================================================
//function : Purpose
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_Contract::Purpose () const
{
  return thePurpose;
}

//=======================================================================
//function : SetPurpose
//purpose  : 
//=======================================================================

void StepBasic_Contract::SetPurpose (const Handle(TCollection_HAsciiString) &aPurpose)
{
  thePurpose = aPurpose;
}

//=======================================================================
//function : Kind
//purpose  : 
//=======================================================================

Handle(StepBasic_ContractType) StepBasic_Contract::Kind () const
{
  return theKind;
}

//=======================================================================
//function : SetKind
//purpose  : 
//=======================================================================

void StepBasic_Contract::SetKind (const Handle(StepBasic_ContractType) &aKind)
{
  theKind = aKind;
}
