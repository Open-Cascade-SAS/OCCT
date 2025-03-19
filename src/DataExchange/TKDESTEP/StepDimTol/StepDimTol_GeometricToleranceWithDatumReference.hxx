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

#include <StepDimTol_HArray1OfDatumReference.hxx>
#include <StepDimTol_HArray1OfDatumSystemOrReference.hxx>
#include <StepDimTol_GeometricTolerance.hxx>
class TCollection_HAsciiString;
class StepBasic_MeasureWithUnit;
class StepDimTol_GeometricToleranceTarget;
class StepRepr_ShapeAspect;

class StepDimTol_GeometricToleranceWithDatumReference;
DEFINE_STANDARD_HANDLE(StepDimTol_GeometricToleranceWithDatumReference,
                       StepDimTol_GeometricTolerance)

//! Representation of STEP entity GeometricToleranceWithDatumReference
class StepDimTol_GeometricToleranceWithDatumReference : public StepDimTol_GeometricTolerance
{

public:
  //! Empty constructor
  Standard_EXPORT StepDimTol_GeometricToleranceWithDatumReference();

  //! Initialize all fields (own and inherited) AP214
  Standard_EXPORT void Init(
    const Handle(TCollection_HAsciiString)&           theGeometricTolerance_Name,
    const Handle(TCollection_HAsciiString)&           theGeometricTolerance_Description,
    const Handle(StepBasic_MeasureWithUnit)&          theGeometricTolerance_Magnitude,
    const Handle(StepRepr_ShapeAspect)&               theGeometricTolerance_TolerancedShapeAspect,
    const Handle(StepDimTol_HArray1OfDatumReference)& theDatumSystem);

  //! Initialize all fields (own and inherited) AP242
  Standard_EXPORT void Init(
    const Handle(TCollection_HAsciiString)&    theGeometricTolerance_Name,
    const Handle(TCollection_HAsciiString)&    theGeometricTolerance_Description,
    const Handle(StepBasic_MeasureWithUnit)&   theGeometricTolerance_Magnitude,
    const StepDimTol_GeometricToleranceTarget& theGeometricTolerance_TolerancedShapeAspect,
    const Handle(StepDimTol_HArray1OfDatumSystemOrReference)& theDatumSystem);

  //! Returns field DatumSystem AP214
  Standard_EXPORT Handle(StepDimTol_HArray1OfDatumReference) DatumSystem() const;

  //! Returns field DatumSystem AP242
  Standard_EXPORT Handle(StepDimTol_HArray1OfDatumSystemOrReference) DatumSystemAP242() const;

  //! Set field DatumSystem AP214
  Standard_EXPORT void SetDatumSystem(
    const Handle(StepDimTol_HArray1OfDatumReference)& theDatumSystem);

  //! Set field DatumSystem AP242
  Standard_EXPORT void SetDatumSystem(
    const Handle(StepDimTol_HArray1OfDatumSystemOrReference)& theDatumSystem);

  DEFINE_STANDARD_RTTIEXT(StepDimTol_GeometricToleranceWithDatumReference,
                          StepDimTol_GeometricTolerance)

protected:
private:
  Handle(StepDimTol_HArray1OfDatumSystemOrReference) myDatumSystem;
};

#endif // _StepDimTol_GeometricToleranceWithDatumReference_HeaderFile
