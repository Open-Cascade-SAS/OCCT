// File:	StepShape_SeamEdge.cxx
// Created:	Fri Jan  4 17:42:44 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <StepShape_SeamEdge.ixx>

//=======================================================================
//function : StepShape_SeamEdge
//purpose  : 
//=======================================================================

StepShape_SeamEdge::StepShape_SeamEdge ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepShape_SeamEdge::Init (const Handle(TCollection_HAsciiString) &aRepresentationItem_Name,
                               const Handle(StepShape_Edge) &aOrientedEdge_EdgeElement,
                               const Standard_Boolean aOrientedEdge_Orientation,
                               const Handle(StepGeom_Pcurve) &aPcurveReference)
{
  StepShape_OrientedEdge::Init(aRepresentationItem_Name,
                               aOrientedEdge_EdgeElement,
                               aOrientedEdge_Orientation);

  thePcurveReference = aPcurveReference;
}

//=======================================================================
//function : PcurveReference
//purpose  : 
//=======================================================================

Handle(StepGeom_Pcurve) StepShape_SeamEdge::PcurveReference () const
{
  return thePcurveReference;
}

//=======================================================================
//function : SetPcurveReference
//purpose  : 
//=======================================================================

void StepShape_SeamEdge::SetPcurveReference (const Handle(StepGeom_Pcurve) &aPcurveReference)
{
  thePcurveReference = aPcurveReference;
}
