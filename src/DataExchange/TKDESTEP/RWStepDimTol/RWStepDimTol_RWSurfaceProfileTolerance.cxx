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

#include <Interface_EntityIterator.hxx>
#include "RWStepDimTol_RWSurfaceProfileTolerance.pxx"
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepDimTol_SurfaceProfileTolerance.hxx>

//=================================================================================================

RWStepDimTol_RWSurfaceProfileTolerance::RWStepDimTol_RWSurfaceProfileTolerance() {}

//=================================================================================================

void RWStepDimTol_RWSurfaceProfileTolerance::ReadStep(
  const Handle(StepData_StepReaderData)&            data,
  const Standard_Integer                            num,
  Handle(Interface_Check)&                          ach,
  const Handle(StepDimTol_SurfaceProfileTolerance)& ent) const
{
  // Check number of parameters
  if (!data->CheckNbParams(num, 4, ach, "surface_profile_tolerance"))
    return;

  // Inherited fields of GeometricTolerance

  Handle(TCollection_HAsciiString) aGeometricTolerance_Name;
  data->ReadString(num, 1, "geometric_tolerance.name", ach, aGeometricTolerance_Name);

  Handle(TCollection_HAsciiString) aGeometricTolerance_Description;
  data->ReadString(num, 2, "geometric_tolerance.description", ach, aGeometricTolerance_Description);

  Handle(StepBasic_MeasureWithUnit) aGeometricTolerance_Magnitude;
  data->ReadEntity(num,
                   3,
                   "geometric_tolerance.magnitude",
                   ach,
                   STANDARD_TYPE(StepBasic_MeasureWithUnit),
                   aGeometricTolerance_Magnitude);

  StepDimTol_GeometricToleranceTarget aGeometricTolerance_TolerancedShapeAspect;
  data->ReadEntity(num,
                   4,
                   "geometric_tolerance.toleranced_shape_aspect",
                   ach,
                   aGeometricTolerance_TolerancedShapeAspect);

  // Initialize entity
  ent->Init(aGeometricTolerance_Name,
            aGeometricTolerance_Description,
            aGeometricTolerance_Magnitude,
            aGeometricTolerance_TolerancedShapeAspect);
}

//=================================================================================================

void RWStepDimTol_RWSurfaceProfileTolerance::WriteStep(
  StepData_StepWriter&                              SW,
  const Handle(StepDimTol_SurfaceProfileTolerance)& ent) const
{

  // Inherited fields of GeometricTolerance

  SW.Send(ent->StepDimTol_GeometricTolerance::Name());

  SW.Send(ent->StepDimTol_GeometricTolerance::Description());

  SW.Send(ent->StepDimTol_GeometricTolerance::Magnitude());

  SW.Send(ent->StepDimTol_GeometricTolerance::TolerancedShapeAspect().Value());
}

//=================================================================================================

void RWStepDimTol_RWSurfaceProfileTolerance::Share(
  const Handle(StepDimTol_SurfaceProfileTolerance)& ent,
  Interface_EntityIterator&                         iter) const
{

  // Inherited fields of GeometricTolerance

  iter.AddItem(ent->StepDimTol_GeometricTolerance::Magnitude());

  iter.AddItem(ent->StepDimTol_GeometricTolerance::TolerancedShapeAspect().Value());
}
