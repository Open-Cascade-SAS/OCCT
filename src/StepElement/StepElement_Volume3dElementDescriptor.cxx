// File:	StepElement_Volume3dElementDescriptor.cxx
// Created:	Thu Dec 12 17:29:05 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepElement_Volume3dElementDescriptor.ixx>

//=======================================================================
//function : StepElement_Volume3dElementDescriptor
//purpose  : 
//=======================================================================

StepElement_Volume3dElementDescriptor::StepElement_Volume3dElementDescriptor ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepElement_Volume3dElementDescriptor::Init (const StepElement_ElementOrder aElementDescriptor_TopologyOrder,
                                                  const Handle(TCollection_HAsciiString) &aElementDescriptor_Description,
                                                  const Handle(StepElement_HArray1OfVolumeElementPurposeMember) &aPurpose,
                                                  const StepElement_Volume3dElementShape aShape)
{
  StepElement_ElementDescriptor::Init(aElementDescriptor_TopologyOrder,
                                      aElementDescriptor_Description);

  thePurpose = aPurpose;

  theShape = aShape;
}

//=======================================================================
//function : Purpose
//purpose  : 
//=======================================================================

Handle(StepElement_HArray1OfVolumeElementPurposeMember) StepElement_Volume3dElementDescriptor::Purpose () const
{
  return thePurpose;
}

//=======================================================================
//function : SetPurpose
//purpose  : 
//=======================================================================

void StepElement_Volume3dElementDescriptor::SetPurpose (const Handle(StepElement_HArray1OfVolumeElementPurposeMember) &aPurpose)
{
  thePurpose = aPurpose;
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

StepElement_Volume3dElementShape StepElement_Volume3dElementDescriptor::Shape () const
{
  return theShape;
}

//=======================================================================
//function : SetShape
//purpose  : 
//=======================================================================

void StepElement_Volume3dElementDescriptor::SetShape (const StepElement_Volume3dElementShape aShape)
{
  theShape = aShape;
}
