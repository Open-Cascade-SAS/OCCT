// File:	StepFEA_FeaMassDensity.cxx
// Created:	Thu Dec 12 17:51:05 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_FeaMassDensity.ixx>

//=======================================================================
//function : StepFEA_FeaMassDensity
//purpose  : 
//=======================================================================

StepFEA_FeaMassDensity::StepFEA_FeaMassDensity ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_FeaMassDensity::Init (const Handle(TCollection_HAsciiString) &aRepresentationItem_Name,
                                   const Standard_Real aFeaConstant)
{
  StepFEA_FeaMaterialPropertyRepresentationItem::Init(aRepresentationItem_Name);

  theFeaConstant = aFeaConstant;
}

//=======================================================================
//function : FeaConstant
//purpose  : 
//=======================================================================

Standard_Real StepFEA_FeaMassDensity::FeaConstant () const
{
  return theFeaConstant;
}

//=======================================================================
//function : SetFeaConstant
//purpose  : 
//=======================================================================

void StepFEA_FeaMassDensity::SetFeaConstant (const Standard_Real aFeaConstant)
{
  theFeaConstant = aFeaConstant;
}
