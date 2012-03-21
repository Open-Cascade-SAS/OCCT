// Created on: 2000-04-18
// Created by: Andrey BETENEV
// Copyright (c) 2000-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <StepShape_DimensionalSizeWithPath.ixx>

//=======================================================================
//function : StepShape_DimensionalSizeWithPath
//purpose  : 
//=======================================================================

StepShape_DimensionalSizeWithPath::StepShape_DimensionalSizeWithPath ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepShape_DimensionalSizeWithPath::Init (const Handle(StepRepr_ShapeAspect) &aDimensionalSize_AppliesTo,
                                              const Handle(TCollection_HAsciiString) &aDimensionalSize_Name,
                                              const Handle(StepRepr_ShapeAspect) &aPath)
{
  StepShape_DimensionalSize::Init(aDimensionalSize_AppliesTo,
                                  aDimensionalSize_Name);

  thePath = aPath;
}

//=======================================================================
//function : Path
//purpose  : 
//=======================================================================

Handle(StepRepr_ShapeAspect) StepShape_DimensionalSizeWithPath::Path () const
{
  return thePath;
}

//=======================================================================
//function : SetPath
//purpose  : 
//=======================================================================

void StepShape_DimensionalSizeWithPath::SetPath (const Handle(StepRepr_ShapeAspect) &aPath)
{
  thePath = aPath;
}
