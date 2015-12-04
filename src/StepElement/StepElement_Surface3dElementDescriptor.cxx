// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <Standard_Type.hxx>
#include <StepElement_Surface3dElementDescriptor.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepElement_Surface3dElementDescriptor,StepElement_ElementDescriptor)

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
