// Copyright (c) 1999-2014 OPEN CASCADE SAS
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
#include <StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol.hxx>
#include <StepDimTol_ModifiedGeometricTolerance.hxx>
#include <StepDimTol_PositionTolerance.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol,StepDimTol_GeometricTolerance)

//=======================================================================
//function : StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol
//purpose  : 
//=======================================================================
StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol::StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol()
{
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol::Init
  (const Handle(TCollection_HAsciiString)& aName,
   const Handle(TCollection_HAsciiString)& aDescription,
   const Handle(StepBasic_MeasureWithUnit)& aMagnitude,
   const Handle(StepRepr_ShapeAspect)& aTolerancedShapeAspect,
   const Handle(StepDimTol_GeometricToleranceWithDatumReference)& aGTWDR,
   const Handle(StepDimTol_ModifiedGeometricTolerance)& aMGT)
{
  SetName(aName);
  SetDescription(aDescription);
  SetMagnitude(aMagnitude);
  SetTolerancedShapeAspect(aTolerancedShapeAspect);
  myGeometricToleranceWithDatumReference = aGTWDR;
  myModifiedGeometricTolerance = aMGT;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol::Init
  (const Handle(TCollection_HAsciiString)& aName,
   const Handle(TCollection_HAsciiString)& aDescription,
   const Handle(StepBasic_MeasureWithUnit)& aMagnitude,
   const StepDimTol_GeometricToleranceTarget& aTolerancedShapeAspect,
   const Handle(StepDimTol_GeometricToleranceWithDatumReference)& aGTWDR,
   const Handle(StepDimTol_ModifiedGeometricTolerance)& aMGT)
{
  SetName(aName);
  SetDescription(aDescription);
  SetMagnitude(aMagnitude);
  SetTolerancedShapeAspect(aTolerancedShapeAspect);
  myGeometricToleranceWithDatumReference = aGTWDR;
  myModifiedGeometricTolerance = aMGT;
}


//=======================================================================
//function : SetGeometricToleranceWithDatumReference
//purpose  : 
//=======================================================================

void StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol::SetGeometricToleranceWithDatumReference
  (const Handle(StepDimTol_GeometricToleranceWithDatumReference)& aGTWDR) 
{
  myGeometricToleranceWithDatumReference = aGTWDR;
}


//=======================================================================
//function : GetGeometricToleranceWithDatumReference
//purpose  : 
//=======================================================================

Handle(StepDimTol_GeometricToleranceWithDatumReference) StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol::GetGeometricToleranceWithDatumReference() const
{
  return myGeometricToleranceWithDatumReference;
}


//=======================================================================
//function : SetModifiedGeometricTolerance
//purpose  : 
//=======================================================================

void StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol::SetModifiedGeometricTolerance
  (const Handle(StepDimTol_ModifiedGeometricTolerance)& aMGT) 
{
  myModifiedGeometricTolerance = aMGT;
}


//=======================================================================
//function : GetModifiedGeometricTolerance
//purpose  : 
//=======================================================================

Handle(StepDimTol_ModifiedGeometricTolerance) StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol::GetModifiedGeometricTolerance() const
{
  return myModifiedGeometricTolerance;
}


//=======================================================================
//function : SetPositionTolerance
//purpose  : 
//=======================================================================

void StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol::SetPositionTolerance
  (const Handle(StepDimTol_PositionTolerance)& aPT) 
{
  myPositionTolerance = aPT;
}


//=======================================================================
//function : GetPositionTolerance
//purpose  : 
//=======================================================================

Handle(StepDimTol_PositionTolerance) StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol::GetPositionTolerance() const
{
  return myPositionTolerance;
}

