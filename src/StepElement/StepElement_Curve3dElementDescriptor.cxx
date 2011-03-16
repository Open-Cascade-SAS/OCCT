// File:	StepElement_Curve3dElementDescriptor.cxx
// Created:	Thu Dec 12 17:28:59 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepElement_Curve3dElementDescriptor.ixx>

//=======================================================================
//function : StepElement_Curve3dElementDescriptor
//purpose  : 
//=======================================================================

StepElement_Curve3dElementDescriptor::StepElement_Curve3dElementDescriptor ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepElement_Curve3dElementDescriptor::Init (const StepElement_ElementOrder aElementDescriptor_TopologyOrder,
                                                 const Handle(TCollection_HAsciiString) &aElementDescriptor_Description,
                                                 const Handle(StepElement_HArray1OfHSequenceOfCurveElementPurposeMember) &aPurpose)
{
  StepElement_ElementDescriptor::Init(aElementDescriptor_TopologyOrder,
                                      aElementDescriptor_Description);

  thePurpose = aPurpose;
}

//=======================================================================
//function : Purpose
//purpose  : 
//=======================================================================

Handle(StepElement_HArray1OfHSequenceOfCurveElementPurposeMember) StepElement_Curve3dElementDescriptor::Purpose () const
{
  return thePurpose;
}

//=======================================================================
//function : SetPurpose
//purpose  : 
//=======================================================================

void StepElement_Curve3dElementDescriptor::SetPurpose (const Handle(StepElement_HArray1OfHSequenceOfCurveElementPurposeMember) &aPurpose)
{
  thePurpose = aPurpose;
}
