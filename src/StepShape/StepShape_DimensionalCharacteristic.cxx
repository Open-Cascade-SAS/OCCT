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

#include <StepShape_DimensionalCharacteristic.ixx>

//=======================================================================
//function : StepShape_DimensionalCharacteristic
//purpose  : 
//=======================================================================

StepShape_DimensionalCharacteristic::StepShape_DimensionalCharacteristic ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepShape_DimensionalCharacteristic::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepShape_DimensionalLocation))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepShape_DimensionalSize))) return 2;
  return 0;
}

//=======================================================================
//function : DimensionalLocation
//purpose  : 
//=======================================================================

Handle(StepShape_DimensionalLocation) StepShape_DimensionalCharacteristic::DimensionalLocation () const
{
  return Handle(StepShape_DimensionalLocation)::DownCast(Value());
}

//=======================================================================
//function : DimensionalSize
//purpose  : 
//=======================================================================

Handle(StepShape_DimensionalSize) StepShape_DimensionalCharacteristic::DimensionalSize () const
{
  return Handle(StepShape_DimensionalSize)::DownCast(Value());
}
