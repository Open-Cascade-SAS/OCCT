// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <Interface_Macros.hxx>
#include <StepShape_BooleanOperand.hxx>
#include <StepShape_BooleanResult.hxx>
#include <StepShape_CsgPrimitive.hxx>
#include <StepShape_HalfSpaceSolid.hxx>
#include <StepShape_SolidModel.hxx>

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
