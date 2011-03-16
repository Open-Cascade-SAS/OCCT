// File:	StepFEA_FeaLinearElasticity.cxx
// Created:	Thu Dec 12 17:51:05 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_FeaLinearElasticity.ixx>

//=======================================================================
//function : StepFEA_FeaLinearElasticity
//purpose  : 
//=======================================================================

StepFEA_FeaLinearElasticity::StepFEA_FeaLinearElasticity ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_FeaLinearElasticity::Init (const Handle(TCollection_HAsciiString) &aRepresentationItem_Name,
                                        const StepFEA_SymmetricTensor43d &aFeaConstants)
{
  StepFEA_FeaMaterialPropertyRepresentationItem::Init(aRepresentationItem_Name);

  theFeaConstants = aFeaConstants;
}

//=======================================================================
//function : FeaConstants
//purpose  : 
//=======================================================================

StepFEA_SymmetricTensor43d StepFEA_FeaLinearElasticity::FeaConstants () const
{
  return theFeaConstants;
}

//=======================================================================
//function : SetFeaConstants
//purpose  : 
//=======================================================================

void StepFEA_FeaLinearElasticity::SetFeaConstants (const StepFEA_SymmetricTensor43d &aFeaConstants)
{
  theFeaConstants = aFeaConstants;
}
