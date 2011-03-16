// File:	StepShape_EdgeBasedWireframeModel.cxx
// Created:	Fri Dec 28 16:02:01 2001 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <StepShape_EdgeBasedWireframeModel.ixx>

//=======================================================================
//function : StepShape_EdgeBasedWireframeModel
//purpose  : 
//=======================================================================

StepShape_EdgeBasedWireframeModel::StepShape_EdgeBasedWireframeModel ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepShape_EdgeBasedWireframeModel::Init (const Handle(TCollection_HAsciiString) &aRepresentationItem_Name,
                                              const Handle(StepShape_HArray1OfConnectedEdgeSet) &aEbwmBoundary)
{
  StepGeom_GeometricRepresentationItem::Init(aRepresentationItem_Name);

  theEbwmBoundary = aEbwmBoundary;
}

//=======================================================================
//function : EbwmBoundary
//purpose  : 
//=======================================================================

Handle(StepShape_HArray1OfConnectedEdgeSet) StepShape_EdgeBasedWireframeModel::EbwmBoundary () const
{
  return theEbwmBoundary;
}

//=======================================================================
//function : SetEbwmBoundary
//purpose  : 
//=======================================================================

void StepShape_EdgeBasedWireframeModel::SetEbwmBoundary (const Handle(StepShape_HArray1OfConnectedEdgeSet) &aEbwmBoundary)
{
  theEbwmBoundary = aEbwmBoundary;
}
