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
#include <RWStepDimTol_RWModifiedGeometricTolerance.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepDimTol_GeometricToleranceTarget.hxx>
#include <StepDimTol_ModifiedGeometricTolerance.hxx>

//=======================================================================
//function : RWStepDimTol_RWModifiedGeometricTolerance
//purpose  : 
//=======================================================================
RWStepDimTol_RWModifiedGeometricTolerance::RWStepDimTol_RWModifiedGeometricTolerance ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWModifiedGeometricTolerance::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                          const Standard_Integer num,
                                                          Handle(Interface_Check)& ach,
                                                          const Handle(StepDimTol_ModifiedGeometricTolerance) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,5,ach,"modified_geometric_tolerance") ) return;

  // Inherited fields of GeometricTolerance

  Handle(TCollection_HAsciiString) aGeometricTolerance_Name;
  data->ReadString (num, 1, "geometric_tolerance.name", ach, aGeometricTolerance_Name);

  Handle(TCollection_HAsciiString) aGeometricTolerance_Description;
  data->ReadString (num, 2, "geometric_tolerance.description", ach, aGeometricTolerance_Description);

  Handle(StepBasic_MeasureWithUnit) aGeometricTolerance_Magnitude;
  data->ReadEntity (num, 3, "geometric_tolerance.magnitude", ach, STANDARD_TYPE(StepBasic_MeasureWithUnit), aGeometricTolerance_Magnitude);

  StepDimTol_GeometricToleranceTarget aGeometricTolerance_TolerancedShapeAspect;
  data->ReadEntity (num, 4, "geometric_tolerance.toleranced_shape_aspect", ach, aGeometricTolerance_TolerancedShapeAspect);

  // Own fields of ModifiedGeometricTolerance

  StepDimTol_LimitCondition aModifier = StepDimTol_MaximumMaterialCondition;
  if (data->ParamType (num, 5) == Interface_ParamEnum) {
    Standard_CString text = data->ParamCValue(num, 5);
    if      (strcmp(text, ".MAXIMUM_MATERIAL_CONDITION.")==0) aModifier = StepDimTol_MaximumMaterialCondition;
    else if (strcmp(text, ".LEAST_MATERIAL_CONDITION.")==0) aModifier = StepDimTol_LeastMaterialCondition;
    else if (strcmp(text, ".REGARDLESS_OF_FEATURE_SIZE.")==0) aModifier = StepDimTol_RegardlessOfFeatureSize;
    else ach->AddFail("Parameter #5 (modifier) has not allowed value");
  }
  else ach->AddFail("Parameter #5 (modifier) is not enumeration");

  // Initialize entity
  ent->Init(aGeometricTolerance_Name,
            aGeometricTolerance_Description,
            aGeometricTolerance_Magnitude,
            aGeometricTolerance_TolerancedShapeAspect,
            aModifier);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWModifiedGeometricTolerance::WriteStep (StepData_StepWriter& SW,
                                                           const Handle(StepDimTol_ModifiedGeometricTolerance) &ent) const
{

  // Inherited fields of GeometricTolerance

  SW.Send (ent->StepDimTol_GeometricTolerance::Name());

  SW.Send (ent->StepDimTol_GeometricTolerance::Description());

  SW.Send (ent->StepDimTol_GeometricTolerance::Magnitude());

  SW.Send (ent->StepDimTol_GeometricTolerance::TolerancedShapeAspect().Value());

  // Own fields of ModifiedGeometricTolerance

  switch (ent->Modifier()) {
    case StepDimTol_MaximumMaterialCondition: SW.SendEnum (".MAXIMUM_MATERIAL_CONDITION."); break;
    case StepDimTol_LeastMaterialCondition: SW.SendEnum (".LEAST_MATERIAL_CONDITION."); break;
    case StepDimTol_RegardlessOfFeatureSize: SW.SendEnum (".REGARDLESS_OF_FEATURE_SIZE."); break;
  }
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepDimTol_RWModifiedGeometricTolerance::Share (const Handle(StepDimTol_ModifiedGeometricTolerance) &ent,
                                                       Interface_EntityIterator& iter) const
{

  // Inherited fields of GeometricTolerance

  iter.AddItem (ent->StepDimTol_GeometricTolerance::Magnitude());

  iter.AddItem (ent->StepDimTol_GeometricTolerance::TolerancedShapeAspect().Value());

  // Own fields of ModifiedGeometricTolerance
}
