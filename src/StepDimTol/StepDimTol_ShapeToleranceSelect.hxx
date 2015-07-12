// Created on: 2003-06-04
// Created by: Galina KULIKOVA
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#ifndef _StepDimTol_ShapeToleranceSelect_HeaderFile
#define _StepDimTol_ShapeToleranceSelect_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepDimTol_GeometricTolerance;
class StepShape_PlusMinusTolerance;


//! Representation of STEP SELECT type ShapeToleranceSelect
class StepDimTol_ShapeToleranceSelect  : public StepData_SelectType
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor
  Standard_EXPORT StepDimTol_ShapeToleranceSelect();
  
  //! Recognizes a kind of ShapeToleranceSelect select type
  //! 1 -> GeometricTolerance from StepDimTol
  //! 2 -> PlusMinusTolerance from StepShape
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum (const Handle(Standard_Transient)& ent) const;
  
  //! Returns Value as GeometricTolerance (or Null if another type)
  Standard_EXPORT Handle(StepDimTol_GeometricTolerance) GeometricTolerance() const;
  
  //! Returns Value as PlusMinusTolerance (or Null if another type)
  Standard_EXPORT Handle(StepShape_PlusMinusTolerance) PlusMinusTolerance() const;




protected:





private:





};







#endif // _StepDimTol_ShapeToleranceSelect_HeaderFile
