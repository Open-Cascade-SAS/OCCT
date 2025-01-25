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

#ifndef _StepShape_ValueQualifier_HeaderFile
#define _StepShape_ValueQualifier_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepShape_PrecisionQualifier;
class StepShape_TypeQualifier;
class StepShape_ValueFormatTypeQualifier;

//! Added for Dimensional Tolerances
class StepShape_ValueQualifier : public StepData_SelectType
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT StepShape_ValueQualifier();

  //! Recognizes a kind of ValueQualifier Select Type :
  //! 1 -> PrecisionQualifier from StepShape
  //! 2 -> TypeQualifier from StepShape
  //! 3 -> UnceraintyQualifier .. not yet implemented
  //! 4 -> ValueFormatTypeQualifier
  Standard_EXPORT Standard_Integer CaseNum(const Handle(Standard_Transient)& ent) const;

  //! Returns Value as PrecisionQualifier
  Standard_EXPORT Handle(StepShape_PrecisionQualifier) PrecisionQualifier() const;

  //! Returns Value as TypeQualifier
  Standard_EXPORT Handle(StepShape_TypeQualifier) TypeQualifier() const;

  //! Returns Value as ValueFormatTypeQualifier
  Standard_EXPORT Handle(StepShape_ValueFormatTypeQualifier) ValueFormatTypeQualifier() const;

protected:
private:
};

#endif // _StepShape_ValueQualifier_HeaderFile
