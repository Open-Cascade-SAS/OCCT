// Created on: 2015-07-13
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

#ifndef _StepDimTol_ToleranceZoneTarget_HeaderFile
#define _StepDimTol_ToleranceZoneTarget_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>

class Standard_Transient;
class StepDimTol_GeometricTolerance;
class StepDimTol_GeneralDatumReference;
class StepShape_DimensionalLocation;
class StepShape_DimensionalSize;

class StepDimTol_ToleranceZoneTarget : public StepData_SelectType
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns a ToleranceZoneTarget select type
  Standard_EXPORT StepDimTol_ToleranceZoneTarget();

  //! Recognizes a ToleranceZoneTarget Kind Entity that is :
  //! 1 -> DimensionalLocation
  //! 2 -> DimensionalSize
  //! 3 -> GeometricTolerance
  //! 4 -> GeneralDatumReference
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum(const Handle(Standard_Transient)& ent) const;

  //! returns Value as a DimensionalLocation (Null if another type)
  Standard_EXPORT Handle(StepShape_DimensionalLocation) DimensionalLocation() const;

  //! returns Value as a DimensionalSize (Null if another type)
  Standard_EXPORT Handle(StepShape_DimensionalSize) DimensionalSize() const;

  //! returns Value as a GeometricTolerance (Null if another type)
  Standard_EXPORT Handle(StepDimTol_GeometricTolerance) GeometricTolerance() const;

  //! returns Value as a GeneralDatumReference (Null if another type)
  Standard_EXPORT Handle(StepDimTol_GeneralDatumReference) GeneralDatumReference() const;
};
#endif // _StepDimTol_ToleranceZoneTarget_HeaderFile
