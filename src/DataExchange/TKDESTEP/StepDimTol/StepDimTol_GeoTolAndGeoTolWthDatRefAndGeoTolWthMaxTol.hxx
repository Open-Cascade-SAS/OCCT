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

#ifndef _StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol_HeaderFile
#define _StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod.hxx>
#include <StepDimTol_GeometricToleranceType.hxx>
class StepDimTol_GeometricToleranceTarget;
class StepDimTol_GeometricToleranceWithDatumReference;
class StepDimTol_GeometricToleranceWithModifiers;
class TCollection_HAsciiString;
class StepBasic_LengthMeasureWithUnit;
class StepRepr_ShapeAspect;

class StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol
    : public StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod
{

public:
  Standard_EXPORT StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol();

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>& theName,
                            const occ::handle<TCollection_HAsciiString>& theDescription,
                            const occ::handle<Standard_Transient>&       theMagnitude,
                            const occ::handle<StepRepr_ShapeAspect>&     theTolerancedShapeAspect,
                            const occ::handle<StepDimTol_GeometricToleranceWithDatumReference>& theGTWDR,
                            const occ::handle<StepDimTol_GeometricToleranceWithModifiers>&      theGTWM,
                            const occ::handle<StepBasic_LengthMeasureWithUnit>& theMaxTol,
                            const StepDimTol_GeometricToleranceType        theType);

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>&    aName,
                            const occ::handle<TCollection_HAsciiString>&    aDescription,
                            const occ::handle<Standard_Transient>&          aMagnitude,
                            const StepDimTol_GeometricToleranceTarget& aTolerancedShapeAspect,
                            const occ::handle<StepDimTol_GeometricToleranceWithDatumReference>& aGTWDR,
                            const occ::handle<StepDimTol_GeometricToleranceWithModifiers>&      aGTWM,
                            const occ::handle<StepBasic_LengthMeasureWithUnit>& theMaxTol,
                            const StepDimTol_GeometricToleranceType        theType);

  inline void SetMaxTolerance(occ::handle<StepBasic_LengthMeasureWithUnit>& theMaxTol)
  {
    myMaxTol = theMaxTol;
  }

  inline occ::handle<StepBasic_LengthMeasureWithUnit> GetMaxTolerance() { return myMaxTol; }

  DEFINE_STANDARD_RTTIEXT(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol,
                          StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod)

private:
  occ::handle<StepBasic_LengthMeasureWithUnit> myMaxTol;
};
#endif // _StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod_HeaderFile
