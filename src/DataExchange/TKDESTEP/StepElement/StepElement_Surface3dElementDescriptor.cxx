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

#include <StepElement_Surface3dElementDescriptor.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepElement_Surface3dElementDescriptor, StepElement_ElementDescriptor)

//=================================================================================================

StepElement_Surface3dElementDescriptor::StepElement_Surface3dElementDescriptor() {}

//=================================================================================================

void StepElement_Surface3dElementDescriptor::Init(
  const StepElement_ElementOrder          aElementDescriptor_TopologyOrder,
  const occ::handle<TCollection_HAsciiString>& aElementDescriptor_Description,
  const occ::handle<NCollection_HArray1<occ::handle<NCollection_HSequence<occ::handle<StepElement_SurfaceElementPurposeMember>>>>>& aPurpose,
  const StepElement_Element2dShape                                           aShape)
{
  StepElement_ElementDescriptor::Init(aElementDescriptor_TopologyOrder,
                                      aElementDescriptor_Description);

  thePurpose = aPurpose;

  theShape = aShape;
}

//=================================================================================================

occ::handle<NCollection_HArray1<occ::handle<NCollection_HSequence<occ::handle<StepElement_SurfaceElementPurposeMember>>>>>
  StepElement_Surface3dElementDescriptor::Purpose() const
{
  return thePurpose;
}

//=================================================================================================

void StepElement_Surface3dElementDescriptor::SetPurpose(
  const occ::handle<NCollection_HArray1<occ::handle<NCollection_HSequence<occ::handle<StepElement_SurfaceElementPurposeMember>>>>>& aPurpose)
{
  thePurpose = aPurpose;
}

//=================================================================================================

StepElement_Element2dShape StepElement_Surface3dElementDescriptor::Shape() const
{
  return theShape;
}

//=================================================================================================

void StepElement_Surface3dElementDescriptor::SetShape(const StepElement_Element2dShape aShape)
{
  theShape = aShape;
}
