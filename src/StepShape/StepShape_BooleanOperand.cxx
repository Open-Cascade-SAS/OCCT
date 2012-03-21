// Copyright (c) 1999-2012 OPEN CASCADE SAS
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


#include <StepShape_BooleanOperand.ixx>
#include <Interface_Macros.hxx>

StepShape_BooleanOperand::StepShape_BooleanOperand () {  }

Handle(StepShape_SolidModel) StepShape_BooleanOperand::SolidModel () const
{
  return theSolidModel;
}

void StepShape_BooleanOperand::SetSolidModel
(const Handle(StepShape_SolidModel)& aSolidModel) 
{
  theSolidModel = aSolidModel;
}

Handle(StepShape_HalfSpaceSolid) StepShape_BooleanOperand::HalfSpaceSolid () const
{
  return theHalfSpaceSolid;
}

void StepShape_BooleanOperand::SetHalfSpaceSolid
(const Handle(StepShape_HalfSpaceSolid)& aHalfSpaceSolid)
{
  theHalfSpaceSolid = aHalfSpaceSolid;
}

StepShape_CsgPrimitive StepShape_BooleanOperand::CsgPrimitive () const
{
  return theCsgPrimitive;
}

void StepShape_BooleanOperand::SetCsgPrimitive
(const StepShape_CsgPrimitive& aCsgPrimitive)
{
  theCsgPrimitive = aCsgPrimitive;
}

Handle(StepShape_BooleanResult) StepShape_BooleanOperand::BooleanResult () const
{
  return theBooleanResult;
}

void StepShape_BooleanOperand::SetBooleanResult
(const Handle(StepShape_BooleanResult)& aBooleanResult)
{
  theBooleanResult = aBooleanResult;
}

void StepShape_BooleanOperand::SetTypeOfContent(const Standard_Integer aType)
{
  theTypeOfContent = aType;
}

Standard_Integer StepShape_BooleanOperand::TypeOfContent() const
{
  return theTypeOfContent;
}
