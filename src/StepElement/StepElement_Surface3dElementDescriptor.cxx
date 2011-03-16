// File:	StepElement_Surface3dElementDescriptor.cxx
// Created:	Thu Dec 12 17:29:04 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepElement_Surface3dElementDescriptor.ixx>

//=======================================================================
//function : StepElement_Surface3dElementDescriptor
//purpose  : 
//=======================================================================

StepElement_Surface3dElementDescriptor::StepElement_Surface3dElementDescriptor ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepElement_Surface3dElementDescriptor::Init (const StepElement_ElementOrder aElementDescriptor_TopologyOrder,
                                                   const Handle(TCollection_HAsciiString) &aElementDescriptor_Description,
                                                   const Handle(StepElement_HArray1OfHSequenceOfSurfaceElementPurposeMember) &aPurpose,
                                                   const StepElement_Element2dShape aShape)
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

Handle(StepElement_HArray1OfHSequenceOfSurfaceElementPurposeMember) StepElement_Surface3dElementDescriptor::Purpose () const
{
  return thePurpose;
}

//=======================================================================
//function : SetPurpose
//purpose  : 
//=======================================================================

void StepElement_Surface3dElementDescriptor::SetPurpose (const Handle(StepElement_HArray1OfHSequenceOfSurfaceElementPurposeMember) &aPurpose)
{
  thePurpose = aPurpose;
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

StepElement_Element2dShape StepElement_Surface3dElementDescriptor::Shape () const
{
  return theShape;
}

//=======================================================================
//function : SetShape
//purpose  : 
//=======================================================================

void StepElement_Surface3dElementDescriptor::SetShape (const StepElement_Element2dShape aShape)
{
  theShape = aShape;
}
