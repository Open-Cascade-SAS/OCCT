// File:	StepFEA_FeaMoistureAbsorption.cxx
// Created:	Thu Dec 12 17:51:05 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_FeaMoistureAbsorption.ixx>

//=======================================================================
//function : StepFEA_FeaMoistureAbsorption
//purpose  : 
//=======================================================================

StepFEA_FeaMoistureAbsorption::StepFEA_FeaMoistureAbsorption ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_FeaMoistureAbsorption::Init (const Handle(TCollection_HAsciiString) &aRepresentationItem_Name,
                                          const StepFEA_SymmetricTensor23d &aFeaConstants)
{
  StepFEA_FeaMaterialPropertyRepresentationItem::Init(aRepresentationItem_Name);

  theFeaConstants = aFeaConstants;
}

//=======================================================================
//function : FeaConstants
//purpose  : 
//=======================================================================

StepFEA_SymmetricTensor23d StepFEA_FeaMoistureAbsorption::FeaConstants () const
{
  return theFeaConstants;
}

//=======================================================================
//function : SetFeaConstants
//purpose  : 
//=======================================================================

void StepFEA_FeaMoistureAbsorption::SetFeaConstants (const StepFEA_SymmetricTensor23d &aFeaConstants)
{
  theFeaConstants = aFeaConstants;
}
