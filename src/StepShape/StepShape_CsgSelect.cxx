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
#include <StepShape_BooleanResult.hxx>
#include <StepShape_CsgPrimitive.hxx>
#include <StepShape_CsgSelect.hxx>

StepShape_CsgSelect::StepShape_CsgSelect () {  }

void StepShape_CsgSelect::SetTypeOfContent(const Standard_Integer aType) 
{
  theTypeOfContent = aType;
}

Standard_Integer StepShape_CsgSelect::TypeOfContent() const 
{
  return theTypeOfContent;
}

Handle(StepShape_BooleanResult) StepShape_CsgSelect::BooleanResult () const
{
	return theBooleanResult;
}

void StepShape_CsgSelect::SetBooleanResult(const Handle(StepShape_BooleanResult)& aBooleanResult)
{
  theBooleanResult = aBooleanResult;
  theTypeOfContent = 1;
}

StepShape_CsgPrimitive StepShape_CsgSelect::CsgPrimitive () const
{
	return theCsgPrimitive;
}

void StepShape_CsgSelect::SetCsgPrimitive (const StepShape_CsgPrimitive& aCsgPrimitive)
{
  theCsgPrimitive  = aCsgPrimitive;
  theTypeOfContent = 2;
}
