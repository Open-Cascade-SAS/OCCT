// File:	StepFEA_FeaTangentialCoefficientOfLinearThermalExpansion.cxx
// Created:	Thu Dec 12 17:51:06 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_FeaTangentialCoefficientOfLinearThermalExpansion.ixx>

//=======================================================================
//function : StepFEA_FeaTangentialCoefficientOfLinearThermalExpansion
//purpose  : 
//=======================================================================

StepFEA_FeaTangentialCoefficientOfLinearThermalExpansion::StepFEA_FeaTangentialCoefficientOfLinearThermalExpansion ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_FeaTangentialCoefficientOfLinearThermalExpansion::Init (const Handle(TCollection_HAsciiString) &aRepresentationItem_Name,
                                                                     const StepFEA_SymmetricTensor23d &aFeaConstants)
{
  StepFEA_FeaMaterialPropertyRepresentationItem::Init(aRepresentationItem_Name);

  theFeaConstants = aFeaConstants;
}

//=======================================================================
//function : FeaConstants
//purpose  : 
//=======================================================================

StepFEA_SymmetricTensor23d StepFEA_FeaTangentialCoefficientOfLinearThermalExpansion::FeaConstants () const
{
  return theFeaConstants;
}

//=======================================================================
//function : SetFeaConstants
//purpose  : 
//=======================================================================

void StepFEA_FeaTangentialCoefficientOfLinearThermalExpansion::SetFeaConstants (const StepFEA_SymmetricTensor23d &aFeaConstants)
{
  theFeaConstants = aFeaConstants;
}
