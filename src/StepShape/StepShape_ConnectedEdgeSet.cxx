// File:	StepShape_ConnectedEdgeSet.cxx
// Created:	Fri Dec 28 16:02:00 2001 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <StepShape_ConnectedEdgeSet.ixx>

//=======================================================================
//function : StepShape_ConnectedEdgeSet
//purpose  : 
//=======================================================================

StepShape_ConnectedEdgeSet::StepShape_ConnectedEdgeSet ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepShape_ConnectedEdgeSet::Init (const Handle(TCollection_HAsciiString) &aRepresentationItem_Name,
                                       const Handle(StepShape_HArray1OfEdge) &aCesEdges)
{
  StepShape_TopologicalRepresentationItem::Init(aRepresentationItem_Name);

  theCesEdges = aCesEdges;
}

//=======================================================================
//function : CesEdges
//purpose  : 
//=======================================================================

Handle(StepShape_HArray1OfEdge) StepShape_ConnectedEdgeSet::CesEdges () const
{
  return theCesEdges;
}

//=======================================================================
//function : SetCesEdges
//purpose  : 
//=======================================================================

void StepShape_ConnectedEdgeSet::SetCesEdges (const Handle(StepShape_HArray1OfEdge) &aCesEdges)
{
  theCesEdges = aCesEdges;
}
