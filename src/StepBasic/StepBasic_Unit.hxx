// Created on: 1999-09-08
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
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

#ifndef _StepBasic_Unit_HeaderFile
#define _StepBasic_Unit_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepBasic_NamedUnit;
class StepBasic_DerivedUnit;

//! Implements a select type unit (NamedUnit or DerivedUnit)
class StepBasic_Unit : public StepData_SelectType
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates empty object
  Standard_EXPORT StepBasic_Unit();

  //! Recognizes a type of Unit Entity
  //! 1 -> NamedUnit
  //! 2 -> DerivedUnit
  Standard_EXPORT Standard_Integer CaseNum(const Handle(Standard_Transient)& ent) const;

  //! returns Value as a NamedUnit (Null if another type)
  Standard_EXPORT Handle(StepBasic_NamedUnit) NamedUnit() const;

  //! returns Value as a DerivedUnit (Null if another type)
  Standard_EXPORT Handle(StepBasic_DerivedUnit) DerivedUnit() const;

protected:
private:
};

#endif // _StepBasic_Unit_HeaderFile
