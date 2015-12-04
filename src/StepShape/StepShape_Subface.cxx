// Created on: 2002-01-04
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <Standard_Type.hxx>
#include <StepShape_Face.hxx>
#include <StepShape_Subface.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_Subface,StepShape_Face)

//=======================================================================
//function : StepShape_Subface
//purpose  : 
//=======================================================================
StepShape_Subface::StepShape_Subface ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepShape_Subface::Init (const Handle(TCollection_HAsciiString) &aRepresentationItem_Name,
                              const Handle(StepShape_HArray1OfFaceBound) &aFace_Bounds,
                              const Handle(StepShape_Face) &aParentFace)
{
  StepShape_Face::Init(aRepresentationItem_Name,
                       aFace_Bounds);

  theParentFace = aParentFace;
}

//=======================================================================
//function : ParentFace
//purpose  : 
//=======================================================================

Handle(StepShape_Face) StepShape_Subface::ParentFace () const
{
  return theParentFace;
}

//=======================================================================
//function : SetParentFace
//purpose  : 
//=======================================================================

void StepShape_Subface::SetParentFace (const Handle(StepShape_Face) &aParentFace)
{
  theParentFace = aParentFace;
}
