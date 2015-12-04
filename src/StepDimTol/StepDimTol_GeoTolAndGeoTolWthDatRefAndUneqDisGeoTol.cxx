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


#include <Standard_Type.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepDimTol_GeometricToleranceTarget.hxx>
#include <StepDimTol_GeometricToleranceWithDatumReference.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol.hxx>
#include <StepDimTol_UnequallyDisposedGeometricTolerance.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol,StepDimTol_GeoTolAndGeoTolWthDatRef)

//=======================================================================
//function : StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol
//purpose  : 
//=======================================================================
StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol::StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol()
{
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol::Init
  (const Handle(TCollection_HAsciiString)& theName,
   const Handle(TCollection_HAsciiString)& theDescription,
   const Handle(StepBasic_MeasureWithUnit)& theMagnitude,
   const Handle(StepRepr_ShapeAspect)& theTolerancedShapeAspect,
   const Handle(StepDimTol_GeometricToleranceWithDatumReference)& theGTWDR,
   const StepDimTol_GeometricToleranceType theType,
   const Handle(StepDimTol_UnequallyDisposedGeometricTolerance)& theUDGT)
{
  StepDimTol_GeoTolAndGeoTolWthDatRef::Init(theName, theDescription, theMagnitude,
    theTolerancedShapeAspect, theGTWDR, theType);
  myUnequallyDisposedGeometricTolerance = theUDGT;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol::Init
  (const Handle(TCollection_HAsciiString)& theName,
   const Handle(TCollection_HAsciiString)& theDescription,
   const Handle(StepBasic_MeasureWithUnit)& theMagnitude,
   const StepDimTol_GeometricToleranceTarget& theTolerancedShapeAspect,
   const Handle(StepDimTol_GeometricToleranceWithDatumReference)& theGTWDR,
   const StepDimTol_GeometricToleranceType theType,
   const Handle(StepDimTol_UnequallyDisposedGeometricTolerance)& theUDGT)
{
  StepDimTol_GeoTolAndGeoTolWthDatRef::Init(theName, theDescription, theMagnitude,
    theTolerancedShapeAspect, theGTWDR, theType);
  myUnequallyDisposedGeometricTolerance = theUDGT;
}
