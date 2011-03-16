// File:	StepFEA_NodeSet.cxx
// Created:	Thu Dec 12 17:51:06 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_NodeSet.ixx>

//=======================================================================
//function : StepFEA_NodeSet
//purpose  : 
//=======================================================================

StepFEA_NodeSet::StepFEA_NodeSet ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_NodeSet::Init (const Handle(TCollection_HAsciiString) &aRepresentationItem_Name,
                            const Handle(StepFEA_HArray1OfNodeRepresentation) &aNodes)
{
  StepGeom_GeometricRepresentationItem::Init(aRepresentationItem_Name);

  theNodes = aNodes;
}

//=======================================================================
//function : Nodes
//purpose  : 
//=======================================================================

Handle(StepFEA_HArray1OfNodeRepresentation) StepFEA_NodeSet::Nodes () const
{
  return theNodes;
}

//=======================================================================
//function : SetNodes
//purpose  : 
//=======================================================================

void StepFEA_NodeSet::SetNodes (const Handle(StepFEA_HArray1OfNodeRepresentation) &aNodes)
{
  theNodes = aNodes;
}
