// Created on: 2015-07-07
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

#include <RWStepDimTol_RWGeometricToleranceWithDefinedUnit.hxx>

#include <Interface_EntityIterator.hxx>
#include <StepBasic_LengthMeasureWithUnit.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepDimTol_GeometricToleranceTarget.hxx>
#include <StepDimTol_GeometricToleranceWithDefinedUnit.hxx>

//=======================================================================
//function : RWStepDimTol_RWGeometricToleranceWithDefinedUnit
//purpose  : 
//=======================================================================

RWStepDimTol_RWGeometricToleranceWithDefinedUnit::RWStepDimTol_RWGeometricToleranceWithDefinedUnit ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWGeometricToleranceWithDefinedUnit::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                  const Standard_Integer num,
                                                  Handle(Interface_Check)& ach,
                                                  const Handle(StepDimTol_GeometricToleranceWithDefinedUnit) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num, 5, ach, "geometric_tolerance_with_defined_unit") ) return;

  // inherited fields from GeometricTolerance

  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 1, "geometric_tolerance.name", ach, aName);

  Handle(TCollection_HAsciiString) aDescription;
  data->ReadString (num, 2, "geometric_tolerance.description", ach, aDescription);

  Handle(StepBasic_MeasureWithUnit) aMagnitude;
  data->ReadEntity (num, 3, "geometric_tolerance.magnitude", ach, STANDARD_TYPE(StepBasic_MeasureWithUnit), aMagnitude);

  StepDimTol_GeometricToleranceTarget aTolerancedShapeAspect;
  data->ReadEntity (num, 4, "geometric_tolerance.toleranced_shape_aspect", ach, aTolerancedShapeAspect);

  // own fields of GeometricToleranceWithDefinedUnit

  Handle(StepBasic_LengthMeasureWithUnit) anUnitSize;
  data->ReadEntity (num, 5, "unit_size", ach, STANDARD_TYPE(StepBasic_LengthMeasureWithUnit), anUnitSize);

  // Initialize entity
  ent->Init(aName,
            aDescription,
            aMagnitude,
            aTolerancedShapeAspect,
            anUnitSize);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWGeometricToleranceWithDefinedUnit::
  WriteStep (StepData_StepWriter& SW,
             const Handle(StepDimTol_GeometricToleranceWithDefinedUnit) &ent) const
{

  // inherited fields from GeometricTolerance

  SW.Send (ent->Name());

  SW.Send (ent->Description());

  SW.Send (ent->Magnitude());

  SW.Send (ent->TolerancedShapeAspect().Value());

  // own fields of GeometricToleranceWithDefinedUnit

  SW.Send (ent->UnitSize());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepDimTol_RWGeometricToleranceWithDefinedUnit::Share (const Handle(StepDimTol_GeometricToleranceWithDefinedUnit) &ent,
                                                              Interface_EntityIterator& iter) const
{

  // inherited fields from GeometricTolerance

  iter.AddItem (ent->Magnitude());

  iter.AddItem (ent->TolerancedShapeAspect().Value());
}
