// File:	StepBasic_ContractType.cxx
// Created:	Fri Nov 26 16:26:37 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepBasic_ContractType.ixx>

//=======================================================================
//function : StepBasic_ContractType
//purpose  : 
//=======================================================================

StepBasic_ContractType::StepBasic_ContractType ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_ContractType::Init (const Handle(TCollection_HAsciiString) &aDescription)
{

  theDescription = aDescription;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_ContractType::Description () const
{
  return theDescription;
}

//=======================================================================
//function : SetDescription
//purpose  : 
//=======================================================================

void StepBasic_ContractType::SetDescription (const Handle(TCollection_HAsciiString) &aDescription)
{
  theDescription = aDescription;
}
