// Created on: 2001-04-24
// Created by: Christian CAILLET
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _StepShape_ToleranceMethodDefinition_HeaderFile
#define _StepShape_ToleranceMethodDefinition_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepShape_ToleranceValue;
class StepShape_LimitsAndFits;

//! Added for Dimensional Tolerances
class StepShape_ToleranceMethodDefinition : public StepData_SelectType
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT StepShape_ToleranceMethodDefinition();

  //! Recognizes a kind of ValueQualifier Select Type :
  //! 1 -> ToleranceValue from StepShape
  //! 2 -> LimitsAndFits from StepShape
  Standard_EXPORT Standard_Integer CaseNum(const Handle(Standard_Transient)& ent) const;

  //! Returns Value as ToleranceValue
  Standard_EXPORT Handle(StepShape_ToleranceValue) ToleranceValue() const;

  //! Returns Value as LimitsAndFits
  Standard_EXPORT Handle(StepShape_LimitsAndFits) LimitsAndFits() const;

protected:
private:
};

#endif // _StepShape_ToleranceMethodDefinition_HeaderFile
