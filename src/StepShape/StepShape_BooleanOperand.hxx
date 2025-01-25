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

#ifndef _StepShape_BooleanOperand_HeaderFile
#define _StepShape_BooleanOperand_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepShape_CsgPrimitive.hxx>
#include <Standard_Integer.hxx>
class StepShape_SolidModel;
class StepShape_HalfSpaceSolid;
class StepShape_BooleanResult;

class StepShape_BooleanOperand
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns a BooleanOperand SelectType
  Standard_EXPORT StepShape_BooleanOperand();

  Standard_EXPORT void SetTypeOfContent(const Standard_Integer aTypeOfContent);

  Standard_EXPORT Standard_Integer TypeOfContent() const;

  //! returns Value as a SolidModel (Null if another
  //! type)
  Standard_EXPORT Handle(StepShape_SolidModel) SolidModel() const;

  Standard_EXPORT void SetSolidModel(const Handle(StepShape_SolidModel)& aSolidModel);

  //! returns Value as a HalfSpaceSolid (Null if
  //! another type)
  Standard_EXPORT Handle(StepShape_HalfSpaceSolid) HalfSpaceSolid() const;

  Standard_EXPORT void SetHalfSpaceSolid(const Handle(StepShape_HalfSpaceSolid)& aHalfSpaceSolid);

  //! returns Value as a CsgPrimitive (Null if another
  //! type)
  //! CsgPrimitive is another Select Type
  Standard_EXPORT StepShape_CsgPrimitive CsgPrimitive() const;

  Standard_EXPORT void SetCsgPrimitive(const StepShape_CsgPrimitive& aCsgPrimitive);

  //! returns Value as a BooleanResult (Null if another
  //! type)
  Standard_EXPORT Handle(StepShape_BooleanResult) BooleanResult() const;

  Standard_EXPORT void SetBooleanResult(const Handle(StepShape_BooleanResult)& aBooleanResult);

protected:
private:
  Handle(StepShape_SolidModel)     theSolidModel;
  Handle(StepShape_HalfSpaceSolid) theHalfSpaceSolid;
  StepShape_CsgPrimitive           theCsgPrimitive;
  Handle(StepShape_BooleanResult)  theBooleanResult;
  Standard_Integer                 theTypeOfContent;
};

#endif // _StepShape_BooleanOperand_HeaderFile
