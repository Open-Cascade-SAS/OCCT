// File:	StepFEA_FeaSecantCoefficientOfLinearThermalExpansion.cxx
// Created:	Thu Dec 12 17:51:06 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_FeaSecantCoefficientOfLinearThermalExpansion.ixx>

//=======================================================================
//function : StepFEA_FeaSecantCoefficientOfLinearThermalExpansion
//purpose  : 
//=======================================================================

StepFEA_FeaSecantCoefficientOfLinearThermalExpansion::StepFEA_FeaSecantCoefficientOfLinearThermalExpansion ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_FeaSecantCoefficientOfLinearThermalExpansion::Init (const Handle(TCollection_HAsciiString) &aRepresentationItem_Name,
                                                                 const StepFEA_SymmetricTensor23d &aFeaConstants,
                                                                 const Standard_Real aReferenceTemperature)
{
  StepFEA_FeaMaterialPropertyRepresentationItem::Init(aRepresentationItem_Name);

  theFeaConstants = aFeaConstants;

  theReferenceTemperature = aReferenceTemperature;
}

//=======================================================================
//function : FeaConstants
//purpose  : 
//=======================================================================

StepFEA_SymmetricTensor23d StepFEA_FeaSecantCoefficientOfLinearThermalExpansion::FeaConstants () const
{
  return theFeaConstants;
}

//=======================================================================
//function : SetFeaConstants
//purpose  : 
//=======================================================================

void StepFEA_FeaSecantCoefficientOfLinearThermalExpansion::SetFeaConstants (const StepFEA_SymmetricTensor23d &aFeaConstants)
{
  theFeaConstants = aFeaConstants;
}

//=======================================================================
//function : ReferenceTemperature
//purpose  : 
//=======================================================================

Standard_Real StepFEA_FeaSecantCoefficientOfLinearThermalExpansion::ReferenceTemperature () const
{
  return theReferenceTemperature;
}

//=======================================================================
//function : SetReferenceTemperature
//purpose  : 
//=======================================================================

void StepFEA_FeaSecantCoefficientOfLinearThermalExpansion::SetReferenceTemperature (const Standard_Real aReferenceTemperature)
{
  theReferenceTemperature = aReferenceTemperature;
}
