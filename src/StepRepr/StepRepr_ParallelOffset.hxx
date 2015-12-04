// Created on: 2015-07-10
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _StepRepr_ParallelOffset_HeaderFile
#define _StepRepr_ParallelOffset_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_DerivedShapeAspect.hxx>
#include <StepData_Logical.hxx>
class StepBasic_MeasureWithUnit;
class TCollection_HAsciiString;
class StepRepr_ProductDefinitionShape;

class StepRepr_ParallelOffset;
DEFINE_STANDARD_HANDLE(StepRepr_ParallelOffset, StepRepr_DerivedShapeAspect)
//! Added for Dimensional Tolerances
class StepRepr_ParallelOffset : public StepRepr_DerivedShapeAspect
{

public:
  
  Standard_EXPORT StepRepr_ParallelOffset();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT   void Init (const Handle(TCollection_HAsciiString)& theName, const Handle(TCollection_HAsciiString)& theDescription, const Handle(StepRepr_ProductDefinitionShape)& theOfShape, const StepData_Logical theProductDefinitional, const Handle(StepBasic_MeasureWithUnit)& theOffset) ;
  
  //! Returns field Offset  
  inline Handle(StepBasic_MeasureWithUnit) Offset () const
  {
    return offset;
  }
  
  //! Set field Offset  
  inline void SetOffset (const Handle(StepBasic_MeasureWithUnit)& theOffset)
  {
    offset = theOffset;
  }

  DEFINE_STANDARD_RTTIEXT(StepRepr_ParallelOffset,StepRepr_DerivedShapeAspect)

private:
  Handle(StepBasic_MeasureWithUnit) offset;
};
#endif // _StepRepr_ParallelOffset_HeaderFile
