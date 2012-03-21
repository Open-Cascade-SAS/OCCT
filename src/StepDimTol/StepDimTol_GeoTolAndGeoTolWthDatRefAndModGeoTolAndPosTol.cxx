// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol.ixx>


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

