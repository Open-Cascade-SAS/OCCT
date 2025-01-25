// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _StepShape_CsgSelect_HeaderFile
#define _StepShape_CsgSelect_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepShape_CsgPrimitive.hxx>
#include <Standard_Integer.hxx>
class StepShape_BooleanResult;

class StepShape_CsgSelect
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns a CsgSelect SelectType
  Standard_EXPORT StepShape_CsgSelect();

  Standard_EXPORT void SetTypeOfContent(const Standard_Integer aTypeOfContent);

  Standard_EXPORT Standard_Integer TypeOfContent() const;

  //! returns Value as a BooleanResult (Null if another type)
  Standard_EXPORT Handle(StepShape_BooleanResult) BooleanResult() const;

  Standard_EXPORT void SetBooleanResult(const Handle(StepShape_BooleanResult)& aBooleanResult);

  //! returns Value as a CsgPrimitive (Null if another type)
  Standard_EXPORT StepShape_CsgPrimitive CsgPrimitive() const;

  Standard_EXPORT void SetCsgPrimitive(const StepShape_CsgPrimitive& aCsgPrimitive);

protected:
private:
  Handle(StepShape_BooleanResult) theBooleanResult;
  StepShape_CsgPrimitive          theCsgPrimitive;
  Standard_Integer                theTypeOfContent;
};

#endif // _StepShape_CsgSelect_HeaderFile
