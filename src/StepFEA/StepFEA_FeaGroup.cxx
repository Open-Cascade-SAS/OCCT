// File:	StepFEA_FeaGroup.cxx
// Created:	Thu Dec 12 17:51:05 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_FeaGroup.ixx>

//=======================================================================
//function : StepFEA_FeaGroup
//purpose  : 
//=======================================================================

StepFEA_FeaGroup::StepFEA_FeaGroup ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_FeaGroup::Init (const Handle(TCollection_HAsciiString) &aGroup_Name,
                             const Handle(TCollection_HAsciiString) &aGroup_Description,
                             const Handle(StepFEA_FeaModel) &aModelRef)
{
  StepBasic_Group::Init(aGroup_Name,
                        Standard_True,
                        aGroup_Description);

  theModelRef = aModelRef;
}

//=======================================================================
//function : ModelRef
//purpose  : 
//=======================================================================

Handle(StepFEA_FeaModel) StepFEA_FeaGroup::ModelRef () const
{
  return theModelRef;
}

//=======================================================================
//function : SetModelRef
//purpose  : 
//=======================================================================

void StepFEA_FeaGroup::SetModelRef (const Handle(StepFEA_FeaModel) &aModelRef)
{
  theModelRef = aModelRef;
}
