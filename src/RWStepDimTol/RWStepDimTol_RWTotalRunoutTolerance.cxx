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

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepDimTol_RWTotalRunoutTolerance.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepDimTol_DatumReference.hxx>
#include <StepDimTol_GeometricToleranceTarget.hxx>
#include <StepDimTol_HArray1OfDatumSystemOrReference.hxx>
#include <StepDimTol_TotalRunoutTolerance.hxx>

//=======================================================================
//function : RWStepDimTol_RWTotalRunoutTolerance
//purpose  : 
//=======================================================================
RWStepDimTol_RWTotalRunoutTolerance::RWStepDimTol_RWTotalRunoutTolerance ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWTotalRunoutTolerance::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                    const Standard_Integer num,
                                                    Handle(Interface_Check)& ach,
                                                    const Handle(StepDimTol_TotalRunoutTolerance) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,5,ach,"total_runout_tolerance") ) return;

  // Inherited fields of GeometricTolerance

  Handle(TCollection_HAsciiString) aGeometricTolerance_Name;
  data->ReadString (num, 1, "geometric_tolerance.name", ach, aGeometricTolerance_Name);

  Handle(TCollection_HAsciiString) aGeometricTolerance_Description;
  data->ReadString (num, 2, "geometric_tolerance.description", ach, aGeometricTolerance_Description);

  Handle(StepBasic_MeasureWithUnit) aGeometricTolerance_Magnitude;
  data->ReadEntity (num, 3, "geometric_tolerance.magnitude", ach, STANDARD_TYPE(StepBasic_MeasureWithUnit), aGeometricTolerance_Magnitude);

  StepDimTol_GeometricToleranceTarget aGeometricTolerance_TolerancedShapeAspect;
  data->ReadEntity (num, 4, "geometric_tolerance.toleranced_shape_aspect", ach, aGeometricTolerance_TolerancedShapeAspect);

  // Inherited fields of GeometricToleranceWithDatumReference

  Handle(StepDimTol_HArray1OfDatumSystemOrReference) aGeometricToleranceWithDatumReference_DatumSystem;
  Standard_Integer sub5 = 0;
  if ( data->ReadSubList (num, 5, "geometric_tolerance_with_datum_reference.datum_system", ach, sub5) ) {
    Standard_Integer nb0 = data->NbParams(sub5);
    aGeometricToleranceWithDatumReference_DatumSystem = new StepDimTol_HArray1OfDatumSystemOrReference (1, nb0);
    Standard_Integer num2 = sub5;
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      StepDimTol_DatumSystemOrReference anIt0;
      data->ReadEntity (num2, i0, "datum_system_or_reference", ach, anIt0);
      aGeometricToleranceWithDatumReference_DatumSystem->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  ent->Init(aGeometricTolerance_Name,
            aGeometricTolerance_Description,
            aGeometricTolerance_Magnitude,
            aGeometricTolerance_TolerancedShapeAspect,
            aGeometricToleranceWithDatumReference_DatumSystem);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWTotalRunoutTolerance::WriteStep (StepData_StepWriter& SW,
                                                     const Handle(StepDimTol_TotalRunoutTolerance) &ent) const
{

  // Inherited fields of GeometricTolerance

  SW.Send (ent->StepDimTol_GeometricTolerance::Name());

  SW.Send (ent->StepDimTol_GeometricTolerance::Description());

  SW.Send (ent->StepDimTol_GeometricTolerance::Magnitude());

  SW.Send (ent->StepDimTol_GeometricTolerance::TolerancedShapeAspect().Value());

  // Inherited fields of GeometricToleranceWithDatumReference

  SW.OpenSub();
  for (Standard_Integer i4=1; i4 <= ent->StepDimTol_GeometricToleranceWithDatumReference::DatumSystemAP242()->Length(); i4++ ) {
    StepDimTol_DatumSystemOrReference Var0 = ent->StepDimTol_GeometricToleranceWithDatumReference::DatumSystemAP242()->Value(i4);
    SW.Send (Var0.Value());
  }
  SW.CloseSub();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepDimTol_RWTotalRunoutTolerance::Share (const Handle(StepDimTol_TotalRunoutTolerance) &ent,
                                                 Interface_EntityIterator& iter) const
{

  // Inherited fields of GeometricTolerance

  iter.AddItem (ent->StepDimTol_GeometricTolerance::Magnitude());

  iter.AddItem (ent->StepDimTol_GeometricTolerance::TolerancedShapeAspect().Value());

  // Inherited fields of GeometricToleranceWithDatumReference

  for (Standard_Integer i3=1; i3 <= ent->StepDimTol_GeometricToleranceWithDatumReference::DatumSystemAP242()->Length(); i3++ ) {
    StepDimTol_DatumSystemOrReference Var0 = ent->StepDimTol_GeometricToleranceWithDatumReference::DatumSystemAP242()->Value(i3);
    iter.AddItem (Var0.Value());
  }
}
