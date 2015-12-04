// Created on: 2003-08-22
// Created by: Sergey KUUL
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

#ifndef _StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol_HeaderFile
#define _StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepDimTol_GeometricTolerance.hxx>
class StepDimTol_GeometricToleranceTarget;
class StepDimTol_GeometricToleranceWithDatumReference;
class StepDimTol_ModifiedGeometricTolerance;
class StepDimTol_PositionTolerance;
class TCollection_HAsciiString;
class StepBasic_MeasureWithUnit;
class StepRepr_ShapeAspect;


class StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol;
DEFINE_STANDARD_HANDLE(StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol, StepDimTol_GeometricTolerance)


class StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol : public StepDimTol_GeometricTolerance
{

public:

  
  Standard_EXPORT StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(TCollection_HAsciiString)& aDescription, const Handle(StepBasic_MeasureWithUnit)& aMagnitude, const Handle(StepRepr_ShapeAspect)& aTolerancedShapeAspect, const Handle(StepDimTol_GeometricToleranceWithDatumReference)& aGTWDR, const Handle(StepDimTol_ModifiedGeometricTolerance)& aMGT);

  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(TCollection_HAsciiString)& aDescription, const Handle(StepBasic_MeasureWithUnit)& aMagnitude, const StepDimTol_GeometricToleranceTarget& aTolerancedShapeAspect, const Handle(StepDimTol_GeometricToleranceWithDatumReference)& aGTWDR, const Handle(StepDimTol_ModifiedGeometricTolerance)& aMGT);

  Standard_EXPORT void SetGeometricToleranceWithDatumReference (const Handle(StepDimTol_GeometricToleranceWithDatumReference)& aGTWDR);
  
  Standard_EXPORT Handle(StepDimTol_GeometricToleranceWithDatumReference) GetGeometricToleranceWithDatumReference() const;
  
  Standard_EXPORT void SetModifiedGeometricTolerance (const Handle(StepDimTol_ModifiedGeometricTolerance)& aMGT);
  
  Standard_EXPORT Handle(StepDimTol_ModifiedGeometricTolerance) GetModifiedGeometricTolerance() const;
  
  Standard_EXPORT void SetPositionTolerance (const Handle(StepDimTol_PositionTolerance)& aPT);
  
  Standard_EXPORT Handle(StepDimTol_PositionTolerance) GetPositionTolerance() const;




  DEFINE_STANDARD_RTTIEXT(StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol,StepDimTol_GeometricTolerance)

protected:




private:


  Handle(StepDimTol_GeometricToleranceWithDatumReference) myGeometricToleranceWithDatumReference;
  Handle(StepDimTol_ModifiedGeometricTolerance) myModifiedGeometricTolerance;
  Handle(StepDimTol_PositionTolerance) myPositionTolerance;


};







#endif // _StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol_HeaderFile
