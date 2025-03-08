// Created on: 2015-07-20
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

#ifndef _StepDimTol_GeometricToleranceTarget_HeaderFile
#define _StepDimTol_GeometricToleranceTarget_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>

class Standard_Transient;
class StepShape_DimensionalLocation;
class StepShape_DimensionalSize;
class StepRepr_ProductDefinitionShape;
class StepRepr_ShapeAspect;

class StepDimTol_GeometricToleranceTarget : public StepData_SelectType
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns a GeometricToleranceTarget select type
  Standard_EXPORT StepDimTol_GeometricToleranceTarget();

  //! Recognizes a GeometricToleranceTarget Kind Entity that is :
  //! 1 -> DimensionalLocation
  //! 2 -> DimensionalSize
  //! 3 -> ProductDefinitionShape
  //! 4 -> ShapeAspect
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum(const Handle(Standard_Transient)& ent) const;

  //! returns Value as a DimensionalLocation (Null if another type)
  Standard_EXPORT Handle(StepShape_DimensionalLocation) DimensionalLocation() const;

  //! returns Value as a DimensionalSize (Null if another type)
  Standard_EXPORT Handle(StepShape_DimensionalSize) DimensionalSize() const;

  //! returns Value as a ProductDefinitionShape (Null if another type)
  Standard_EXPORT Handle(StepRepr_ProductDefinitionShape) ProductDefinitionShape() const;

  //! returns Value as a ShapeAspect (Null if another type)
  Standard_EXPORT Handle(StepRepr_ShapeAspect) ShapeAspect() const;
};
#endif // _StepDimTol_GeometricToleranceTarget_HeaderFile
