// Created on: 2015-07-16
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

#include <RWStepDimTol_RWDatumReferenceModifierWithValue.hxx>

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepBasic_LengthMeasureWithUnit.hxx>
#include <StepDimTol_DatumReferenceModifierType.hxx>
#include <StepDimTol_DatumReferenceModifierWithValue.hxx>

//=======================================================================
//function : RWStepDimTol_RWGeometricTolerance
//purpose  : 
//=======================================================================

RWStepDimTol_RWDatumReferenceModifierWithValue::RWStepDimTol_RWDatumReferenceModifierWithValue ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWDatumReferenceModifierWithValue::
  ReadStep (const Handle(StepData_StepReaderData)& data,
            const Standard_Integer num,
            Handle(Interface_Check)& ach,
            const Handle(StepDimTol_DatumReferenceModifierWithValue) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num, 2, ach, "datum_reference_modifier_with_value") ) return;

  // own fields of DatumReferenceModifierWithValue

  StepDimTol_DatumReferenceModifierType aModifierType = StepDimTol_CircularOrCylindrical;
  if (data->ParamType (num, 1) == Interface_ParamEnum) {
    Standard_CString text = data->ParamCValue(num, 1);
    if      (strcmp(text, ".CIRCULAR_OR_CYLINDRICAL.")==0) aModifierType = StepDimTol_CircularOrCylindrical;
    else if (strcmp(text, ".DISTANCE.")==0) aModifierType = StepDimTol_Distance;
    else if (strcmp(text, ".PROJECTED.")==0) aModifierType = StepDimTol_Projected;
    else if (strcmp(text, ".SPHERICAL.")==0) aModifierType = StepDimTol_Spherical;
    else ach->AddFail("Parameter #1 (modifier_type) has not allowed value");
    }
  else ach->AddFail("Parameter #1 (modifier_type) is not an enumeration");

  Handle(StepBasic_LengthMeasureWithUnit) aModifierValue;
  data->ReadEntity (num, 2, "modifier_value", ach, STANDARD_TYPE(StepBasic_LengthMeasureWithUnit), aModifierValue);

  // Initialize entity
  ent->Init(aModifierType,
            aModifierValue);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWDatumReferenceModifierWithValue::
  WriteStep (StepData_StepWriter& SW,
  const Handle(StepDimTol_DatumReferenceModifierWithValue) &ent) const
{
  // own fields of DatumReferenceModifierWithValue
  
  switch (ent->ModifierType()) {
    case StepDimTol_CircularOrCylindrical: SW.SendEnum (".CIRCULAR_OR_CYLINDRICAL."); break;
    case StepDimTol_Distance: SW.SendEnum (".DISTANCE."); break;
    case StepDimTol_Projected: SW.SendEnum (".PROJECTED."); break;
    case StepDimTol_Spherical: SW.SendEnum (".SPHERICAL."); break;
  }

  SW.Send (ent->ModifierValue());  
}
