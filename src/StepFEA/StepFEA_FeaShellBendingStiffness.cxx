// File:	StepFEA_FeaShellBendingStiffness.cxx
// Created:	Thu Dec 12 17:51:06 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_FeaShellBendingStiffness.ixx>

//=======================================================================
//function : StepFEA_FeaShellBendingStiffness
//purpose  : 
//=======================================================================

StepFEA_FeaShellBendingStiffness::StepFEA_FeaShellBendingStiffness ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_FeaShellBendingStiffness::Init (const Handle(TCollection_HAsciiString) &aRepresentationItem_Name,
                                             const StepFEA_SymmetricTensor42d &aFeaConstants)
{
  StepFEA_FeaMaterialPropertyRepresentationItem::Init(aRepresentationItem_Name);

  theFeaConstants = aFeaConstants;
}

//=======================================================================
//function : FeaConstants
//purpose  : 
//=======================================================================

StepFEA_SymmetricTensor42d StepFEA_FeaShellBendingStiffness::FeaConstants () const
{
  return theFeaConstants;
}

//=======================================================================
//function : SetFeaConstants
//purpose  : 
//=======================================================================

void StepFEA_FeaShellBendingStiffness::SetFeaConstants (const StepFEA_SymmetricTensor42d &aFeaConstants)
{
  theFeaConstants = aFeaConstants;
}
