// File:	StepFEA_FeaShellShearStiffness.cxx
// Created:	Thu Dec 12 17:51:06 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_FeaShellShearStiffness.ixx>

//=======================================================================
//function : StepFEA_FeaShellShearStiffness
//purpose  : 
//=======================================================================

StepFEA_FeaShellShearStiffness::StepFEA_FeaShellShearStiffness ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_FeaShellShearStiffness::Init (const Handle(TCollection_HAsciiString) &aRepresentationItem_Name,
                                           const StepFEA_SymmetricTensor22d &aFeaConstants)
{
  StepFEA_FeaMaterialPropertyRepresentationItem::Init(aRepresentationItem_Name);

  theFeaConstants = aFeaConstants;
}

//=======================================================================
//function : FeaConstants
//purpose  : 
//=======================================================================

StepFEA_SymmetricTensor22d StepFEA_FeaShellShearStiffness::FeaConstants () const
{
  return theFeaConstants;
}

//=======================================================================
//function : SetFeaConstants
//purpose  : 
//=======================================================================

void StepFEA_FeaShellShearStiffness::SetFeaConstants (const StepFEA_SymmetricTensor22d &aFeaConstants)
{
  theFeaConstants = aFeaConstants;
}
