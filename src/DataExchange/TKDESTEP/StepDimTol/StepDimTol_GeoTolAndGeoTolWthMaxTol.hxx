// Created on: 2015-11-13
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

#ifndef _StepDimTol_GeoTolAndGeoTolWthMaxTol_HeaderFile
#define _StepDimTol_GeoTolAndGeoTolWthMaxTol_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepDimTol_GeoTolAndGeoTolWthMod.hxx>
#include <StepDimTol_GeometricToleranceType.hxx>
class StepDimTol_GeometricToleranceTarget;
class StepDimTol_GeometricToleranceWithModifiers;
class TCollection_HAsciiString;
class StepBasic_LengthMeasureWithUnit;
class StepBasic_MeasureWithUnit;
class StepRepr_ShapeAspect;

class StepDimTol_GeoTolAndGeoTolWthMaxTol;
DEFINE_STANDARD_HANDLE(StepDimTol_GeoTolAndGeoTolWthMaxTol, StepDimTol_GeoTolAndGeoTolWthMod)

class StepDimTol_GeoTolAndGeoTolWthMaxTol : public StepDimTol_GeoTolAndGeoTolWthMod
{

public:
  Standard_EXPORT StepDimTol_GeoTolAndGeoTolWthMaxTol();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)&  theName,
                            const Handle(TCollection_HAsciiString)&  theDescription,
                            const Handle(StepBasic_MeasureWithUnit)& theMagnitude,
                            const Handle(StepRepr_ShapeAspect)&      theTolerancedShapeAspect,
                            const Handle(StepDimTol_GeometricToleranceWithModifiers)& theGTWM,
                            const Handle(StepBasic_LengthMeasureWithUnit)&            theMaxTol,
                            const StepDimTol_GeometricToleranceType                   theType);

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)&    aName,
                            const Handle(TCollection_HAsciiString)&    aDescription,
                            const Handle(StepBasic_MeasureWithUnit)&   aMagnitude,
                            const StepDimTol_GeometricToleranceTarget& aTolerancedShapeAspect,
                            const Handle(StepDimTol_GeometricToleranceWithModifiers)& aGTWM,
                            const Handle(StepBasic_LengthMeasureWithUnit)&            theMaxTol,
                            const StepDimTol_GeometricToleranceType                   theType);

  inline void SetMaxTolerance(Handle(StepBasic_LengthMeasureWithUnit)& theMaxTol)
  {
    myMaxTol = theMaxTol;
  }

  inline Handle(StepBasic_LengthMeasureWithUnit) GetMaxTolerance() { return myMaxTol; }

  DEFINE_STANDARD_RTTIEXT(StepDimTol_GeoTolAndGeoTolWthMaxTol, StepDimTol_GeoTolAndGeoTolWthMod)

private:
  Handle(StepBasic_LengthMeasureWithUnit) myMaxTol;
};
#endif // _StepDimTol_GeoTolAndGeoTolWthMaxTol_HeaderFile
