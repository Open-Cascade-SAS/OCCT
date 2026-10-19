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
  //! Construct an uninitialized complex tolerance.
  Standard_EXPORT StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTolAndUneqDisGeoTol();

  //! Initialize the geometric tolerance and all required complex constituents.
  //! Entity handles are retained; the caller must provide schema-compatible constituents.
  //! @param[in] theName geometric tolerance name
  //! @param[in] theDescription authored description
  //! @param[in] theMagnitude magnitude measure entity; complex constituents are retained
  //! @param[in] theTarget toleranced shape aspect or geometric tolerance target
  //! @param[in] theDatum required datum-reference constituent
  //! @param[in] theModifiers required non-empty modifiers constituent
  //! @param[in] theMaximum maximum length measure, greater than magnitude with matching units
  //! @param[in] theType concrete subtype compatible with the datum constituents
  //! @param[in] theUnequal required unequal-displacement constituent
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

  //! Initialize the geometric tolerance and all required complex constituents.
  //! Entity handles are retained; the caller must provide schema-compatible constituents.
  //! @param[in] theName geometric tolerance name
  //! @param[in] theDescription authored description
  //! @param[in] theMagnitude magnitude measure entity; complex constituents are retained
  //! @param[in] theTarget toleranced shape aspect or geometric tolerance target
  //! @param[in] theDatum required datum-reference constituent
  //! @param[in] theModifiers required non-empty modifiers constituent
  //! @param[in] theMaximum maximum length measure, greater than magnitude with matching units
  //! @param[in] theType concrete subtype compatible with the datum constituents
  //! @param[in] theUnequal required unequal-displacement constituent
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

  //! Set the required unequal-displacement constituent; retain the supplied handle.
  void SetUnequallyDisposedGeometricTolerance(
    const occ::handle<StepDimTol_UnequallyDisposedGeometricTolerance>& theUnequal)
  {
    myUnequallyDisposedGeometricTolerance = theUnequal;
  }

  //! Return the unequal-displacement constituent retained by this entity.
  const occ::handle<StepDimTol_UnequallyDisposedGeometricTolerance>&
    GetUnequallyDisposedGeometricTolerance() const
  {
    return myUnequallyDisposedGeometricTolerance;
  }

  DEFINE_STANDARD_RTTIEXT(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTolAndUneqDisGeoTol,
                          StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol)

private:
  occ::handle<StepDimTol_UnequallyDisposedGeometricTolerance>
    myUnequallyDisposedGeometricTolerance; //!< Required unequal-displacement constituent.
};
#endif // _StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTolAndUneqDisGeoTol_HeaderFile
