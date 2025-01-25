// Created on: 2015-08-06
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

#ifndef _StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod_HeaderFile
#define _StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepDimTol_GeometricTolerance.hxx>
#include <StepDimTol_GeometricToleranceType.hxx>
class StepDimTol_GeometricToleranceTarget;
class StepDimTol_GeometricToleranceWithDatumReference;
class StepDimTol_GeometricToleranceWithModifiers;
class TCollection_HAsciiString;
class StepBasic_MeasureWithUnit;
class StepRepr_ShapeAspect;

class StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod;
DEFINE_STANDARD_HANDLE(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod,
                       StepDimTol_GeometricTolerance)

class StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod : public StepDimTol_GeometricTolerance
{

public:
  Standard_EXPORT StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)&  theName,
                            const Handle(TCollection_HAsciiString)&  theDescription,
                            const Handle(StepBasic_MeasureWithUnit)& theMagnitude,
                            const Handle(StepRepr_ShapeAspect)&      theTolerancedShapeAspect,
                            const Handle(StepDimTol_GeometricToleranceWithDatumReference)& theGTWDR,
                            const Handle(StepDimTol_GeometricToleranceWithModifiers)&      theGTWM,
                            const StepDimTol_GeometricToleranceType                        theType);

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)&    aName,
                            const Handle(TCollection_HAsciiString)&    aDescription,
                            const Handle(StepBasic_MeasureWithUnit)&   aMagnitude,
                            const StepDimTol_GeometricToleranceTarget& aTolerancedShapeAspect,
                            const Handle(StepDimTol_GeometricToleranceWithDatumReference)& aGTWDR,
                            const Handle(StepDimTol_GeometricToleranceWithModifiers)&      aGTWM,
                            const StepDimTol_GeometricToleranceType                        theType);

  inline void SetGeometricToleranceWithDatumReference(
    const Handle(StepDimTol_GeometricToleranceWithDatumReference)& theGTWDR)
  {
    myGeometricToleranceWithDatumReference = theGTWDR;
  }

  inline Handle(StepDimTol_GeometricToleranceWithDatumReference)
    GetGeometricToleranceWithDatumReference() const
  {
    return myGeometricToleranceWithDatumReference;
  }

  inline void SetGeometricToleranceWithModifiers(
    const Handle(StepDimTol_GeometricToleranceWithModifiers)& theGTWM)
  {
    myGeometricToleranceWithModifiers = theGTWM;
  }

  inline Handle(StepDimTol_GeometricToleranceWithModifiers) GetGeometricToleranceWithModifiers()
    const
  {
    return myGeometricToleranceWithModifiers;
  }

  inline void SetGeometricToleranceType(const StepDimTol_GeometricToleranceType theType)
  {
    myToleranceType = theType;
  }

  StepDimTol_GeometricToleranceType GetToleranceType() const { return myToleranceType; }

  DEFINE_STANDARD_RTTIEXT(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod,
                          StepDimTol_GeometricTolerance)

private:
  Handle(StepDimTol_GeometricToleranceWithDatumReference) myGeometricToleranceWithDatumReference;
  Handle(StepDimTol_GeometricToleranceWithModifiers)      myGeometricToleranceWithModifiers;
  StepDimTol_GeometricToleranceType                       myToleranceType;
};
#endif // _StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod_HeaderFile
