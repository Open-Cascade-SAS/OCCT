// Created on: 2015-08-11
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

#ifndef _StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol_HeaderFile
#define _StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepDimTol_GeoTolAndGeoTolWthDatRef.hxx>
class StepDimTol_GeometricToleranceTarget;
class StepDimTol_GeometricToleranceWithDatumReference;
class StepDimTol_UnequallyDisposedGeometricTolerance;
class TCollection_HAsciiString;
class StepRepr_ShapeAspect;

class StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol
    : public StepDimTol_GeoTolAndGeoTolWthDatRef
{

public:
  Standard_EXPORT StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol();

  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&                        theName,
    const occ::handle<TCollection_HAsciiString>&                        theDescription,
    const occ::handle<Standard_Transient>&                              theMagnitude,
    const occ::handle<StepRepr_ShapeAspect>&                            theTolerancedShapeAspect,
    const occ::handle<StepDimTol_GeometricToleranceWithDatumReference>& theGTWDR,
    const StepDimTol_GeometricToleranceType                             theType,
    const occ::handle<StepDimTol_UnequallyDisposedGeometricTolerance>&  theUDGT);

  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&                        aName,
    const occ::handle<TCollection_HAsciiString>&                        aDescription,
    const occ::handle<Standard_Transient>&                              aMagnitude,
    const StepDimTol_GeometricToleranceTarget&                          aTolerancedShapeAspect,
    const occ::handle<StepDimTol_GeometricToleranceWithDatumReference>& aGTWDR,
    const StepDimTol_GeometricToleranceType                             theType,
    const occ::handle<StepDimTol_UnequallyDisposedGeometricTolerance>&  theUDGT);

  inline void SetUnequallyDisposedGeometricTolerance(
    const occ::handle<StepDimTol_UnequallyDisposedGeometricTolerance>& theUDGT)
  {
    myUnequallyDisposedGeometricTolerance = theUDGT;
  }

  inline occ::handle<StepDimTol_UnequallyDisposedGeometricTolerance>
    GetUnequallyDisposedGeometricTolerance() const
  {
    return myUnequallyDisposedGeometricTolerance;
  }

  DEFINE_STANDARD_RTTIEXT(StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol,
                          StepDimTol_GeoTolAndGeoTolWthDatRef)

private:
  occ::handle<StepDimTol_UnequallyDisposedGeometricTolerance> myUnequallyDisposedGeometricTolerance;
};
#endif // _StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol_HeaderFile
