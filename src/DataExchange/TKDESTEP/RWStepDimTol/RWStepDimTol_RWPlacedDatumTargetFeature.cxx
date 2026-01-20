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
#include "RWStepDimTol_RWPlacedDatumTargetFeature.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepDimTol_PlacedDatumTargetFeature.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>

//=================================================================================================

RWStepDimTol_RWPlacedDatumTargetFeature::RWStepDimTol_RWPlacedDatumTargetFeature() {}

//=================================================================================================

void RWStepDimTol_RWPlacedDatumTargetFeature::ReadStep(
  const occ::handle<StepData_StepReaderData>&             data,
  const int                             num,
  occ::handle<Interface_Check>&                           ach,
  const occ::handle<StepDimTol_PlacedDatumTargetFeature>& ent) const
{
  // Check number of parameters
  if (!data->CheckNbParams(num, 5, ach, "placed_datum_target_feature"))
    return;

  // Inherited fields of ShapeAspect

  occ::handle<TCollection_HAsciiString> aShapeAspect_Name;
  data->ReadString(num, 1, "shape_aspect.name", ach, aShapeAspect_Name);

  occ::handle<TCollection_HAsciiString> aShapeAspect_Description;
  if (data->IsParamDefined(num, 2))
  {
    data->ReadString(num, 2, "shape_aspect.description", ach, aShapeAspect_Description);
  }

  occ::handle<StepRepr_ProductDefinitionShape> aShapeAspect_OfShape;
  data->ReadEntity(num,
                   3,
                   "shape_aspect.of_shape",
                   ach,
                   STANDARD_TYPE(StepRepr_ProductDefinitionShape),
                   aShapeAspect_OfShape);

  StepData_Logical aShapeAspect_ProductDefinitional;
  data->ReadLogical(num,
                    4,
                    "shape_aspect.product_definitional",
                    ach,
                    aShapeAspect_ProductDefinitional);

  // Inherited fields of DatumTarget

  occ::handle<TCollection_HAsciiString> aDatumTarget_TargetId;
  data->ReadString(num, 5, "datum_target.target_id", ach, aDatumTarget_TargetId);

  // Initialize entity
  ent->Init(aShapeAspect_Name,
            aShapeAspect_Description,
            aShapeAspect_OfShape,
            aShapeAspect_ProductDefinitional,
            aDatumTarget_TargetId);
}

//=================================================================================================

void RWStepDimTol_RWPlacedDatumTargetFeature::WriteStep(
  StepData_StepWriter&                               SW,
  const occ::handle<StepDimTol_PlacedDatumTargetFeature>& ent) const
{

  // Inherited fields of ShapeAspect

  SW.Send(ent->StepRepr_ShapeAspect::Name());

  SW.Send(ent->StepRepr_ShapeAspect::Description());

  SW.Send(ent->StepRepr_ShapeAspect::OfShape());

  SW.SendLogical(ent->StepRepr_ShapeAspect::ProductDefinitional());

  // Inherited fields of DatumTarget

  SW.Send(ent->StepDimTol_DatumTarget::TargetId());
}

//=================================================================================================

void RWStepDimTol_RWPlacedDatumTargetFeature::Share(
  const occ::handle<StepDimTol_PlacedDatumTargetFeature>& ent,
  Interface_EntityIterator&                          iter) const
{

  // Inherited fields of ShapeAspect

  iter.AddItem(ent->StepRepr_ShapeAspect::OfShape());

  // Inherited fields of DatumTarget
}
