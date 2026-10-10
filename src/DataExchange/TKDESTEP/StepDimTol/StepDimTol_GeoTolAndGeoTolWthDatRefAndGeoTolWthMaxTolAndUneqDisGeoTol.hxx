// Copyright (c) 2026 OPEN CASCADE SAS
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

#ifndef _StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTolAndUneqDisGeoTol_HeaderFile
#define _StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTolAndUneqDisGeoTol_HeaderFile

#include <StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol.hxx>
#include <StepDimTol_GeometricToleranceType.hxx>
class StepDimTol_UnequallyDisposedGeometricTolerance;

//! Complex geometric tolerance with datum references, modifiers, maximum tolerance, unequal
//! displacement and one concrete geometric tolerance subtype. All named constituents are required.
class StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTolAndUneqDisGeoTol
    : public StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol
{
public:
  Standard_EXPORT StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTolAndUneqDisGeoTol();

  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&                        theName,
    const occ::handle<TCollection_HAsciiString>&                        theDescription,
    const occ::handle<Standard_Transient>&                              theMagnitude,
    const occ::handle<StepRepr_ShapeAspect>&                            theTarget,
    const occ::handle<StepDimTol_GeometricToleranceWithDatumReference>& theDatum,
    const occ::handle<StepDimTol_GeometricToleranceWithModifiers>&      theModifiers,
    const occ::handle<Standard_Transient>&                              theMaximum,
    const StepDimTol_GeometricToleranceType                             theType,
    const occ::handle<StepDimTol_UnequallyDisposedGeometricTolerance>&  theUnequal);

  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&                        theName,
    const occ::handle<TCollection_HAsciiString>&                        theDescription,
    const occ::handle<Standard_Transient>&                              theMagnitude,
    const StepDimTol_GeometricToleranceTarget&                          theTarget,
    const occ::handle<StepDimTol_GeometricToleranceWithDatumReference>& theDatum,
    const occ::handle<StepDimTol_GeometricToleranceWithModifiers>&      theModifiers,
    const occ::handle<Standard_Transient>&                              theMaximum,
    const StepDimTol_GeometricToleranceType                             theType,
    const occ::handle<StepDimTol_UnequallyDisposedGeometricTolerance>&  theUnequal);

  void SetUnequallyDisposedGeometricTolerance(
    const occ::handle<StepDimTol_UnequallyDisposedGeometricTolerance>& theUnequal)
  {
    myUnequal = theUnequal;
  }

  const occ::handle<StepDimTol_UnequallyDisposedGeometricTolerance>&
    GetUnequallyDisposedGeometricTolerance() const
  {
    return myUnequal;
  }

  DEFINE_STANDARD_RTTIEXT(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTolAndUneqDisGeoTol,
                          StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol)

private:
  occ::handle<StepDimTol_UnequallyDisposedGeometricTolerance> myUnequal;
};
#endif
