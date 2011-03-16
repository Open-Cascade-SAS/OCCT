// File:	StepFEA_FeaAreaDensity.cxx
// Created:	Thu Dec 12 17:51:05 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_FeaAreaDensity.ixx>

//=======================================================================
//function : StepFEA_FeaAreaDensity
//purpose  : 
//=======================================================================

StepFEA_FeaAreaDensity::StepFEA_FeaAreaDensity ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_FeaAreaDensity::Init (const Handle(TCollection_HAsciiString) &aRepresentationItem_Name,
                                   const Standard_Real aFeaConstant)
{
  StepFEA_FeaMaterialPropertyRepresentationItem::Init(aRepresentationItem_Name);

  theFeaConstant = aFeaConstant;
}

//=======================================================================
//function : FeaConstant
//purpose  : 
//=======================================================================

Standard_Real StepFEA_FeaAreaDensity::FeaConstant () const
{
  return theFeaConstant;
}

//=======================================================================
//function : SetFeaConstant
//purpose  : 
//=======================================================================

void StepFEA_FeaAreaDensity::SetFeaConstant (const Standard_Real aFeaConstant)
{
  theFeaConstant = aFeaConstant;
}
