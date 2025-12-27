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
class StepRepr_ShapeAspect;

class StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol
    : public StepDimTol_GeometricTolerance
{

public:
  Standard_EXPORT StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol();

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>& aName,
                            const occ::handle<TCollection_HAsciiString>& aDescription,
                            const occ::handle<Standard_Transient>&       aMagnitude,
                            const occ::handle<StepRepr_ShapeAspect>&     aTolerancedShapeAspect,
                            const occ::handle<StepDimTol_GeometricToleranceWithDatumReference>& aGTWDR,
                            const occ::handle<StepDimTol_ModifiedGeometricTolerance>&           aMGT);

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>&    aName,
                            const occ::handle<TCollection_HAsciiString>&    aDescription,
                            const occ::handle<Standard_Transient>&          aMagnitude,
                            const StepDimTol_GeometricToleranceTarget& aTolerancedShapeAspect,
                            const occ::handle<StepDimTol_GeometricToleranceWithDatumReference>& aGTWDR,
                            const occ::handle<StepDimTol_ModifiedGeometricTolerance>&           aMGT);

  Standard_EXPORT void SetGeometricToleranceWithDatumReference(
    const occ::handle<StepDimTol_GeometricToleranceWithDatumReference>& aGTWDR);

  Standard_EXPORT occ::handle<StepDimTol_GeometricToleranceWithDatumReference>
    GetGeometricToleranceWithDatumReference() const;

  Standard_EXPORT void SetModifiedGeometricTolerance(
    const occ::handle<StepDimTol_ModifiedGeometricTolerance>& aMGT);

  Standard_EXPORT occ::handle<StepDimTol_ModifiedGeometricTolerance> GetModifiedGeometricTolerance()
    const;

  Standard_EXPORT void SetPositionTolerance(const occ::handle<StepDimTol_PositionTolerance>& aPT);

  Standard_EXPORT occ::handle<StepDimTol_PositionTolerance> GetPositionTolerance() const;

  DEFINE_STANDARD_RTTIEXT(StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol,
                          StepDimTol_GeometricTolerance)

private:
  occ::handle<StepDimTol_GeometricToleranceWithDatumReference> myGeometricToleranceWithDatumReference;
  occ::handle<StepDimTol_ModifiedGeometricTolerance>           myModifiedGeometricTolerance;
  occ::handle<StepDimTol_PositionTolerance>                    myPositionTolerance;
};

#endif // _StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol_HeaderFile
