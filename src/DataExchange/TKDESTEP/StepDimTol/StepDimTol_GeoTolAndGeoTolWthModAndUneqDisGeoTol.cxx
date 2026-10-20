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

#include <StepDimTol_GeoTolAndGeoTolWthModAndUneqDisGeoTol.hxx>
#include <StepDimTol_UnequallyDisposedGeometricTolerance.hxx>
#include <StepRepr_ShapeAspect.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepDimTol_GeoTolAndGeoTolWthModAndUneqDisGeoTol,
                           StepDimTol_GeoTolAndGeoTolWthMod)

//==================================================================================================

StepDimTol_GeoTolAndGeoTolWthModAndUneqDisGeoTol::
  StepDimTol_GeoTolAndGeoTolWthModAndUneqDisGeoTol() = default;

//==================================================================================================

void StepDimTol_GeoTolAndGeoTolWthModAndUneqDisGeoTol::Init(
  const occ::handle<TCollection_HAsciiString>&                       theName,
  const occ::handle<TCollection_HAsciiString>&                       theDescription,
  const occ::handle<Standard_Transient>&                             theMagnitude,
  const StepDimTol_GeometricToleranceTarget&                         theTarget,
  const occ::handle<StepDimTol_GeometricToleranceWithModifiers>&     theModifiers,
  const StepDimTol_GeometricToleranceType                            theType,
  const occ::handle<StepDimTol_UnequallyDisposedGeometricTolerance>& theUnequal)
{
  StepDimTol_GeoTolAndGeoTolWthMod::Init(theName,
                                         theDescription,
                                         theMagnitude,
                                         theTarget,
                                         theModifiers,
                                         theType);
  myUnequallyDisposedGeometricTolerance = theUnequal;
}

//==================================================================================================

void StepDimTol_GeoTolAndGeoTolWthModAndUneqDisGeoTol::Init(
  const occ::handle<TCollection_HAsciiString>&                       theName,
  const occ::handle<TCollection_HAsciiString>&                       theDescription,
  const occ::handle<Standard_Transient>&                             theMagnitude,
  const occ::handle<StepRepr_ShapeAspect>&                           theTarget,
  const occ::handle<StepDimTol_GeometricToleranceWithModifiers>&     theModifiers,
  const StepDimTol_GeometricToleranceType                            theType,
  const occ::handle<StepDimTol_UnequallyDisposedGeometricTolerance>& theUnequal)
{
  StepDimTol_GeometricToleranceTarget aTarget;
  aTarget.SetValue(theTarget);
  Init(theName, theDescription, theMagnitude, aTarget, theModifiers, theType, theUnequal);
}
