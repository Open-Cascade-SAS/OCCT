// File:	StepFEA_ElementGroup.cxx
// Created:	Thu Dec 12 17:51:05 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_ElementGroup.ixx>

//=======================================================================
//function : StepFEA_ElementGroup
//purpose  : 
//=======================================================================

StepFEA_ElementGroup::StepFEA_ElementGroup ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_ElementGroup::Init (const Handle(TCollection_HAsciiString) &aGroup_Name,
                                 const Handle(TCollection_HAsciiString) &aGroup_Description,
                                 const Handle(StepFEA_FeaModel) &aFeaGroup_ModelRef,
                                 const Handle(StepFEA_HArray1OfElementRepresentation) &aElements)
{
  StepFEA_FeaGroup::Init(aGroup_Name,
                         aGroup_Description,
                         aFeaGroup_ModelRef);

  theElements = aElements;
}

//=======================================================================
//function : Elements
//purpose  : 
//=======================================================================

Handle(StepFEA_HArray1OfElementRepresentation) StepFEA_ElementGroup::Elements () const
{
  return theElements;
}

//=======================================================================
//function : SetElements
//purpose  : 
//=======================================================================

void StepFEA_ElementGroup::SetElements (const Handle(StepFEA_HArray1OfElementRepresentation) &aElements)
{
  theElements = aElements;
}
