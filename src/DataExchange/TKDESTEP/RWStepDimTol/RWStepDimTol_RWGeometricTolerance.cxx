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
#include "RWStepDimTol_RWGeometricTolerance.pxx"
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepDimTol_GeometricTolerance.hxx>
#include <StepDimTol_GeometricToleranceTarget.hxx>

//=================================================================================================

RWStepDimTol_RWGeometricTolerance::RWStepDimTol_RWGeometricTolerance() {}

//=================================================================================================

void RWStepDimTol_RWGeometricTolerance::ReadStep(
  const occ::handle<StepData_StepReaderData>&       data,
  const int                       num,
  occ::handle<Interface_Check>&                     ach,
  const occ::handle<StepDimTol_GeometricTolerance>& ent) const
{
  // Check number of parameters
  if (!data->CheckNbParams(num, 4, ach, "geometric_tolerance"))
    return;

  // Own fields of GeometricTolerance

  occ::handle<TCollection_HAsciiString> aName;
  data->ReadString(num, 1, "name", ach, aName);

  occ::handle<TCollection_HAsciiString> aDescription;
  data->ReadString(num, 2, "description", ach, aDescription);

  occ::handle<Standard_Transient> aMagnitude;
  data->ReadEntity(num, 3, "magnitude", ach, STANDARD_TYPE(Standard_Transient), aMagnitude);

  StepDimTol_GeometricToleranceTarget aTolerancedShapeAspect;
  data->ReadEntity(num, 4, "toleranced_shape_aspect", ach, aTolerancedShapeAspect);

  // Initialize entity
  ent->Init(aName, aDescription, aMagnitude, aTolerancedShapeAspect);
}

//=================================================================================================

void RWStepDimTol_RWGeometricTolerance::WriteStep(
  StepData_StepWriter&                         SW,
  const occ::handle<StepDimTol_GeometricTolerance>& ent) const
{

  // Own fields of GeometricTolerance

  SW.Send(ent->Name());

  SW.Send(ent->Description());

  SW.Send(ent->Magnitude());

  SW.Send(ent->TolerancedShapeAspect().Value());
}

//=================================================================================================

void RWStepDimTol_RWGeometricTolerance::Share(const occ::handle<StepDimTol_GeometricTolerance>& ent,
                                              Interface_EntityIterator& iter) const
{

  // Own fields of GeometricTolerance

  iter.AddItem(ent->Magnitude());

  iter.AddItem(ent->TolerancedShapeAspect().Value());
}
