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

#include <RWStepDimTol_RWGeometricToleranceWithDefinedAreaUnit.hxx>

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <StepBasic_LengthMeasureWithUnit.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepDimTol_AreaUnitType.hxx>
#include <StepDimTol_GeometricToleranceTarget.hxx>
#include <StepDimTol_GeometricToleranceWithDefinedAreaUnit.hxx>

//=======================================================================
//function : RWStepDimTol_RWGeometricToleranceWithDefinedAreaUnit
//purpose  : 
//=======================================================================

RWStepDimTol_RWGeometricToleranceWithDefinedAreaUnit::RWStepDimTol_RWGeometricToleranceWithDefinedAreaUnit ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWGeometricToleranceWithDefinedAreaUnit::
  ReadStep (const Handle(StepData_StepReaderData)& data,
            const Standard_Integer num,
            Handle(Interface_Check)& ach,
            const Handle(StepDimTol_GeometricToleranceWithDefinedAreaUnit) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num, 7, ach, "geometric_tolerance_with_defined_area_unit") ) return;

  // inherited fields from GeometricTolerance

  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 1, "geometric_tolerance.name", ach, aName);

  Handle(TCollection_HAsciiString) aDescription;
  data->ReadString (num, 2, "geometric_tolerance.description", ach, aDescription);

  Handle(StepBasic_MeasureWithUnit) aMagnitude;
  data->ReadEntity (num, 3, "geometric_tolerance.magnitude", ach, STANDARD_TYPE(StepBasic_MeasureWithUnit), aMagnitude);

  StepDimTol_GeometricToleranceTarget aTolerancedShapeAspect;
  data->ReadEntity (num, 4, "geometric_tolerance.toleranced_shape_aspect", ach, aTolerancedShapeAspect);

  // inherited fields from GeometricToleranceWithDefinedUnit

  Handle(StepBasic_LengthMeasureWithUnit) anUnitSize;
  data->ReadEntity (num, 5, "geometric_tolerance_with_defined_unit.unit_size", ach, STANDARD_TYPE(StepBasic_LengthMeasureWithUnit), anUnitSize);

  // own fields of GeometricToleranceWithDefinedAreaUnit
  StepDimTol_AreaUnitType aType = StepDimTol_Circular;
  if (data->ParamType (num, 6) == Interface_ParamEnum) {
    Standard_CString text = data->ParamCValue(num, 6);
    if      (strcmp(text, ".CIRCULAR.")==0) aType = StepDimTol_Circular;
    else if (strcmp(text, ".RECTANGULAR.")==0) aType = StepDimTol_Rectangular;
    else if (strcmp(text, ".SQUARE.")==0) aType = StepDimTol_Square;
    else ach->AddFail("Parameter #6 (area_type) has not allowed value");
  }
  else ach->AddFail("Parameter #6 (area_type) is not enumerations");

  Handle(StepBasic_LengthMeasureWithUnit) aSecondUnitSize;
  Standard_Boolean hasSecondUnitSize = data->IsParamDefined(num, 7);
  if (hasSecondUnitSize)
    data->ReadEntity(num, 7, "second_unit_size", ach, STANDARD_TYPE(StepBasic_LengthMeasureWithUnit), aSecondUnitSize);

  // Initialize entity
  ent->Init(aName,
            aDescription,
            aMagnitude,
            aTolerancedShapeAspect,
            anUnitSize,
            aType,
            hasSecondUnitSize,
            aSecondUnitSize);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWGeometricToleranceWithDefinedAreaUnit::
  WriteStep (StepData_StepWriter& SW,
             const Handle(StepDimTol_GeometricToleranceWithDefinedAreaUnit) &ent) const
{

  // inherited fields from GeometricTolerance

  SW.Send (ent->Name());

  SW.Send (ent->Description());

  SW.Send (ent->Magnitude());

  SW.Send (ent->TolerancedShapeAspect().Value());

  // inherited fields from GeometricToleranceWithDefinedUnit

  SW.Send (ent->UnitSize());

  // own fields of GeometricToleranceWithDefinedAreaUnit

  switch (ent->AreaType()) {
    case StepDimTol_Circular: SW.SendEnum (".CIRCULAR."); break;
    case StepDimTol_Rectangular: SW.SendEnum (".RECTANGULAR."); break;
    case StepDimTol_Square: SW.SendEnum (".SQUARE."); break;
  }

  if (ent->HasSecondUnitSize())
    SW.Send (ent->SecondUnitSize());
  else
    SW.SendUndef();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepDimTol_RWGeometricToleranceWithDefinedAreaUnit::
  Share (const Handle(StepDimTol_GeometricToleranceWithDefinedAreaUnit) &ent,
         Interface_EntityIterator& iter) const
{

  // inherited fields from GeometricTolerance

  iter.AddItem (ent->Magnitude());

  iter.AddItem (ent->TolerancedShapeAspect().Value());
}
