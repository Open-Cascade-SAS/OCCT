// File:	StepFEA_NodeGroup.cxx
// Created:	Thu Dec 12 17:51:06 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_NodeGroup.ixx>

//=======================================================================
//function : StepFEA_NodeGroup
//purpose  : 
//=======================================================================

StepFEA_NodeGroup::StepFEA_NodeGroup ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_NodeGroup::Init (const Handle(TCollection_HAsciiString) &aGroup_Name,
                              const Handle(TCollection_HAsciiString) &aGroup_Description,
                              const Handle(StepFEA_FeaModel) &aFeaGroup_ModelRef,
                              const Handle(StepFEA_HArray1OfNodeRepresentation) &aNodes)
{
  StepFEA_FeaGroup::Init(aGroup_Name,
                         aGroup_Description,
                         aFeaGroup_ModelRef);

  theNodes = aNodes;
}

//=======================================================================
//function : Nodes
//purpose  : 
//=======================================================================

Handle(StepFEA_HArray1OfNodeRepresentation) StepFEA_NodeGroup::Nodes () const
{
  return theNodes;
}

//=======================================================================
//function : SetNodes
//purpose  : 
//=======================================================================

void StepFEA_NodeGroup::SetNodes (const Handle(StepFEA_HArray1OfNodeRepresentation) &aNodes)
{
  theNodes = aNodes;
}
