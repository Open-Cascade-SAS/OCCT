// Created on: 2015-07-07
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
// commercial license or contractual agreement. to be the "object code" form of the original source.

#ifndef _StepDimTol_GeometricToleranceWithMaximumTolerance_HeaderFile
#define _StepDimTol_GeometricToleranceWithMaximumTolerance_HeaderFile

#include <Standard.hxx>

#include <StepBasic_LengthMeasureWithUnit.hxx>
#include <StepDimTol_GeometricToleranceWithModifiers.hxx>
#include <StepDimTol_GeometricToleranceModifier.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

class TCollection_HAsciiString;
class StepBasic_MeasureWithUnit;
class StepDimTol_GeometricToleranceTarget;

//! Representation of STEP entity GeometricToleranceWithMaximumTolerance
class StepDimTol_GeometricToleranceWithMaximumTolerance
    : public StepDimTol_GeometricToleranceWithModifiers
{

public:
  //! Empty constructor
  Standard_EXPORT StepDimTol_GeometricToleranceWithMaximumTolerance();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>& theName,
    const occ::handle<TCollection_HAsciiString>& theDescription,
    const occ::handle<Standard_Transient>&       theMagnitude,
    const StepDimTol_GeometricToleranceTarget&   theTolerancedShapeAspect,
    const occ::handle<NCollection_HArray1<StepDimTol_GeometricToleranceModifier>>& theModifiers,
    const occ::handle<StepBasic_LengthMeasureWithUnit>&                            theUnitSize);

  //! Returns field MaximumUpperTolerance
  inline occ::handle<StepBasic_LengthMeasureWithUnit> MaximumUpperTolerance() const
  {
    return myMaximumUpperTolerance;
  }

  //! Set field MaximumUpperTolerance
  inline void SetMaximumUpperTolerance(
    const occ::handle<StepBasic_LengthMeasureWithUnit>& theMaximumUpperTolerance)
  {
    myMaximumUpperTolerance = theMaximumUpperTolerance;
  }

  DEFINE_STANDARD_RTTIEXT(StepDimTol_GeometricToleranceWithMaximumTolerance,
                          StepDimTol_GeometricToleranceWithModifiers)

private:
  occ::handle<StepBasic_LengthMeasureWithUnit> myMaximumUpperTolerance;
};
#endif // _StepDimTol_GeometricToleranceWithMaximumTolerance_HeaderFile
