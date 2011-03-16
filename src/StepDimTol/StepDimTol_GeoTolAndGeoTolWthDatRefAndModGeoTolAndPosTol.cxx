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

