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

#ifndef _StepDimTol_GeometricToleranceWithDatumReference_HeaderFile
#define _StepDimTol_GeometricToleranceWithDatumReference_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepDimTol_DatumReference.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepDimTol_DatumSystemOrReference.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepDimTol_GeometricTolerance.hxx>
class TCollection_HAsciiString;
class StepDimTol_GeometricToleranceTarget;
class StepRepr_ShapeAspect;

//! Representation of STEP entity GeometricToleranceWithDatumReference
class StepDimTol_GeometricToleranceWithDatumReference : public StepDimTol_GeometricTolerance
{

public:
  //! Empty constructor
  Standard_EXPORT StepDimTol_GeometricToleranceWithDatumReference();

  //! Initialize all fields (own and inherited) AP214
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&           theGeometricTolerance_Name,
    const occ::handle<TCollection_HAsciiString>&           theGeometricTolerance_Description,
    const occ::handle<Standard_Transient>&                 theGeometricTolerance_Magnitude,
    const occ::handle<StepRepr_ShapeAspect>&               theGeometricTolerance_TolerancedShapeAspect,
    const occ::handle<NCollection_HArray1<occ::handle<StepDimTol_DatumReference>>>& theDatumSystem);

  //! Initialize all fields (own and inherited) AP242
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&    theGeometricTolerance_Name,
    const occ::handle<TCollection_HAsciiString>&    theGeometricTolerance_Description,
    const occ::handle<Standard_Transient>&          theGeometricTolerance_Magnitude,
    const StepDimTol_GeometricToleranceTarget& theGeometricTolerance_TolerancedShapeAspect,
    const occ::handle<NCollection_HArray1<StepDimTol_DatumSystemOrReference>>& theDatumSystem);

  //! Returns field DatumSystem AP214
  Standard_EXPORT occ::handle<NCollection_HArray1<occ::handle<StepDimTol_DatumReference>>> DatumSystem() const;

  //! Returns field DatumSystem AP242
  Standard_EXPORT occ::handle<NCollection_HArray1<StepDimTol_DatumSystemOrReference>> DatumSystemAP242() const;

  //! Set field DatumSystem AP214
  Standard_EXPORT void SetDatumSystem(
    const occ::handle<NCollection_HArray1<occ::handle<StepDimTol_DatumReference>>>& theDatumSystem);

  //! Set field DatumSystem AP242
  Standard_EXPORT void SetDatumSystem(
    const occ::handle<NCollection_HArray1<StepDimTol_DatumSystemOrReference>>& theDatumSystem);

  DEFINE_STANDARD_RTTIEXT(StepDimTol_GeometricToleranceWithDatumReference,
                          StepDimTol_GeometricTolerance)

private:
  occ::handle<NCollection_HArray1<StepDimTol_DatumSystemOrReference>> myDatumSystem;
};

#endif // _StepDimTol_GeometricToleranceWithDatumReference_HeaderFile
