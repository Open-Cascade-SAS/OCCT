// Created on: 2000-04-18
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _StepShape_DimensionalCharacteristic_HeaderFile
#define _StepShape_DimensionalCharacteristic_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepShape_DimensionalLocation;
class StepShape_DimensionalSize;

//! Representation of STEP SELECT type DimensionalCharacteristic
class StepShape_DimensionalCharacteristic : public StepData_SelectType
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT StepShape_DimensionalCharacteristic();

  //! Recognizes a kind of DimensionalCharacteristic select type
  //! 1 -> DimensionalLocation from StepShape
  //! 2 -> DimensionalSize from StepShape
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum(const Handle(Standard_Transient)& ent) const;

  //! Returns Value as DimensionalLocation (or Null if another type)
  Standard_EXPORT Handle(StepShape_DimensionalLocation) DimensionalLocation() const;

  //! Returns Value as DimensionalSize (or Null if another type)
  Standard_EXPORT Handle(StepShape_DimensionalSize) DimensionalSize() const;

protected:
private:
};

#endif // _StepShape_DimensionalCharacteristic_HeaderFile
