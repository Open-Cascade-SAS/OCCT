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

#include <StepShape_DimensionalSize.ixx>

//=======================================================================
//function : StepShape_DimensionalSize
//purpose  : 
//=======================================================================

StepShape_DimensionalSize::StepShape_DimensionalSize ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepShape_DimensionalSize::Init (const Handle(StepRepr_ShapeAspect) &aAppliesTo,
                                      const Handle(TCollection_HAsciiString) &aName)
{

  theAppliesTo = aAppliesTo;

  theName = aName;
}

//=======================================================================
//function : AppliesTo
//purpose  : 
//=======================================================================

Handle(StepRepr_ShapeAspect) StepShape_DimensionalSize::AppliesTo () const
{
  return theAppliesTo;
}

//=======================================================================
//function : SetAppliesTo
//purpose  : 
//=======================================================================

void StepShape_DimensionalSize::SetAppliesTo (const Handle(StepRepr_ShapeAspect) &aAppliesTo)
{
  theAppliesTo = aAppliesTo;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepShape_DimensionalSize::Name () const
{
  return theName;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void StepShape_DimensionalSize::SetName (const Handle(TCollection_HAsciiString) &aName)
{
  theName = aName;
}
