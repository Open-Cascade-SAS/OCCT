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

#include <StepShape_AngularSize.ixx>

//=======================================================================
//function : StepShape_AngularSize
//purpose  : 
//=======================================================================

StepShape_AngularSize::StepShape_AngularSize ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepShape_AngularSize::Init (const Handle(StepRepr_ShapeAspect) &aDimensionalSize_AppliesTo,
                                  const Handle(TCollection_HAsciiString) &aDimensionalSize_Name,
                                  const StepShape_AngleRelator aAngleSelection)
{
  StepShape_DimensionalSize::Init(aDimensionalSize_AppliesTo,
                                  aDimensionalSize_Name);

  theAngleSelection = aAngleSelection;
}

//=======================================================================
//function : AngleSelection
//purpose  : 
//=======================================================================

StepShape_AngleRelator StepShape_AngularSize::AngleSelection () const
{
  return theAngleSelection;
}

//=======================================================================
//function : SetAngleSelection
//purpose  : 
//=======================================================================

void StepShape_AngularSize::SetAngleSelection (const StepShape_AngleRelator aAngleSelection)
{
  theAngleSelection = aAngleSelection;
}
