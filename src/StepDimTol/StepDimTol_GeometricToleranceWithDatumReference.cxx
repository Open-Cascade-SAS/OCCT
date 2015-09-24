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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <Standard_Type.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepDimTol_GeometricToleranceTarget.hxx>
#include <StepDimTol_GeometricToleranceWithDatumReference.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <TCollection_HAsciiString.hxx>

//=======================================================================
//function : StepDimTol_GeometricToleranceWithDatumReference
//purpose  : 
//=======================================================================
StepDimTol_GeometricToleranceWithDatumReference::StepDimTol_GeometricToleranceWithDatumReference ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepDimTol_GeometricToleranceWithDatumReference::Init (const Handle(TCollection_HAsciiString) &aGeometricTolerance_Name,
                                                            const Handle(TCollection_HAsciiString) &aGeometricTolerance_Description,
                                                            const Handle(StepBasic_MeasureWithUnit) &aGeometricTolerance_Magnitude,
                                                            const Handle(StepRepr_ShapeAspect) &aGeometricTolerance_TolerancedShapeAspect,
                                                            const Handle(StepDimTol_HArray1OfDatumReference) &aDatumSystem)
{
  StepDimTol_GeometricTolerance::Init(aGeometricTolerance_Name,
                                      aGeometricTolerance_Description,
                                      aGeometricTolerance_Magnitude,
                                      aGeometricTolerance_TolerancedShapeAspect);

  theDatumSystem = new StepDimTol_HArray1OfDatumSystemOrReference(aDatumSystem->Lower(), aDatumSystem->Upper());
  StepDimTol_DatumSystemOrReference anAux;
  for (Standard_Integer i = aDatumSystem->Lower(); i <= aDatumSystem->Upper(); i++) {
    anAux.SetValue(aDatumSystem->Value(i));
    theDatumSystem->SetValue(i, anAux);
  }
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepDimTol_GeometricToleranceWithDatumReference::Init (const Handle(TCollection_HAsciiString) &aGeometricTolerance_Name,
                                                            const Handle(TCollection_HAsciiString) &aGeometricTolerance_Description,
                                                            const Handle(StepBasic_MeasureWithUnit) &aGeometricTolerance_Magnitude,
                                                            const StepDimTol_GeometricToleranceTarget &aGeometricTolerance_TolerancedShapeAspect,
                                                            const Handle(StepDimTol_HArray1OfDatumSystemOrReference) &aDatumSystem)
{
  StepDimTol_GeometricTolerance::Init(aGeometricTolerance_Name,
                                      aGeometricTolerance_Description,
                                      aGeometricTolerance_Magnitude,
                                      aGeometricTolerance_TolerancedShapeAspect);

  theDatumSystem = aDatumSystem;
}

//=======================================================================
//function : DatumSystem
//purpose  : 
//=======================================================================

Handle(StepDimTol_HArray1OfDatumReference) StepDimTol_GeometricToleranceWithDatumReference::DatumSystem () const
{
  Handle(StepDimTol_HArray1OfDatumReference) aDatumSystem;
  aDatumSystem = new StepDimTol_HArray1OfDatumReference(theDatumSystem->Lower(), theDatumSystem->Upper());
  for (Standard_Integer i = aDatumSystem->Lower(); i <= aDatumSystem->Upper(); i++) {
    aDatumSystem->SetValue(i, theDatumSystem->Value(i).DatumReference());
  }
  return aDatumSystem;
}

//=======================================================================
//function : DatumSystemAP242
//purpose  : 
//=======================================================================

Handle(StepDimTol_HArray1OfDatumSystemOrReference) StepDimTol_GeometricToleranceWithDatumReference::DatumSystemAP242 () const
{
  return theDatumSystem;
}

//=======================================================================
//function : SetDatumSystem
//purpose  : 
//=======================================================================

void StepDimTol_GeometricToleranceWithDatumReference::SetDatumSystem (const Handle(StepDimTol_HArray1OfDatumReference) &aDatumSystem)
{
  theDatumSystem = new StepDimTol_HArray1OfDatumSystemOrReference(aDatumSystem->Lower(), aDatumSystem->Upper());
  StepDimTol_DatumSystemOrReference anAux;
  for (Standard_Integer i = aDatumSystem->Lower(); i <= aDatumSystem->Upper(); i++) {
    anAux.SetValue(aDatumSystem->Value(i));
    theDatumSystem->SetValue(i, anAux);
  }
}

//=======================================================================
//function : SetDatumSystem
//purpose  : 
//=======================================================================

void StepDimTol_GeometricToleranceWithDatumReference::SetDatumSystem (const Handle(StepDimTol_HArray1OfDatumSystemOrReference) &aDatumSystem)
{
  theDatumSystem = aDatumSystem;
}
