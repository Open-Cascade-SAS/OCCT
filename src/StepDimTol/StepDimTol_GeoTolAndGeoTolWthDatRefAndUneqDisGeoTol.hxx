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
class StepBasic_MeasureWithUnit;
class StepRepr_ShapeAspect;


class StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol;
DEFINE_STANDARD_HANDLE(StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol, StepDimTol_GeoTolAndGeoTolWthDatRef)

class StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol : public StepDimTol_GeoTolAndGeoTolWthDatRef
{

public:

  
  Standard_EXPORT StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& theName, 
                             const Handle(TCollection_HAsciiString)& theDescription, 
                             const Handle(StepBasic_MeasureWithUnit)& theMagnitude, 
                             const Handle(StepRepr_ShapeAspect)& theTolerancedShapeAspect, 
                             const Handle(StepDimTol_GeometricToleranceWithDatumReference)& theGTWDR, 
                             const StepDimTol_GeometricToleranceType theType,
                             const Handle(StepDimTol_UnequallyDisposedGeometricTolerance)& theUDGT);

  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, 
                             const Handle(TCollection_HAsciiString)& aDescription, 
                             const Handle(StepBasic_MeasureWithUnit)& aMagnitude, 
                             const StepDimTol_GeometricToleranceTarget& aTolerancedShapeAspect, 
                             const Handle(StepDimTol_GeometricToleranceWithDatumReference)& aGTWDR, 
                             const StepDimTol_GeometricToleranceType theType,
                             const Handle(StepDimTol_UnequallyDisposedGeometricTolerance)& theUDGT);

  inline void SetUnequallyDisposedGeometricTolerance (const Handle(StepDimTol_UnequallyDisposedGeometricTolerance)& theUDGT){
    myUnequallyDisposedGeometricTolerance = theUDGT;
  }
  
  inline Handle(StepDimTol_UnequallyDisposedGeometricTolerance) GetUnequallyDisposedGeometricTolerance() const {
    return myUnequallyDisposedGeometricTolerance;
  }
  
  DEFINE_STANDARD_RTTIEXT(StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol,StepDimTol_GeoTolAndGeoTolWthDatRef)

private:

  Handle(StepDimTol_UnequallyDisposedGeometricTolerance) myUnequallyDisposedGeometricTolerance;
};
#endif // _StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol_HeaderFile
