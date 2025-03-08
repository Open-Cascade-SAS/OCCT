// Created on: 2015-06-22
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

#include "RWStepRepr_RWPerpendicularTo.pxx"

#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_PerpendicularTo.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>

//=================================================================================================

RWStepRepr_RWPerpendicularTo::RWStepRepr_RWPerpendicularTo() {}

//=================================================================================================

void RWStepRepr_RWPerpendicularTo::ReadStep(const Handle(StepData_StepReaderData)&  data,
                                            const Standard_Integer                  num,
                                            Handle(Interface_Check)&                ach,
                                            const Handle(StepRepr_PerpendicularTo)& ent) const
{
  // Check number of parameters
  if (!data->CheckNbParams(num, 4, ach, "perpendicular_to"))
    return;

  // Inherited fields of ShapeAspect

  Handle(TCollection_HAsciiString) aShapeAspect_Name;
  data->ReadString(num, 1, "shape_aspect.name", ach, aShapeAspect_Name);

  Handle(TCollection_HAsciiString) aShapeAspect_Description;
  if (data->IsParamDefined(num, 2))
  {
    data->ReadString(num, 2, "shape_aspect.description", ach, aShapeAspect_Description);
  }

  Handle(StepRepr_ProductDefinitionShape) aShapeAspect_OfShape;
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

  // Initialize entity
  ent->Init(aShapeAspect_Name,
            aShapeAspect_Description,
            aShapeAspect_OfShape,
            aShapeAspect_ProductDefinitional);
}

//=================================================================================================

void RWStepRepr_RWPerpendicularTo::WriteStep(StepData_StepWriter&                    SW,
                                             const Handle(StepRepr_PerpendicularTo)& ent) const
{

  // Inherited fields of ShapeAspect

  SW.Send(ent->Name());

  SW.Send(ent->Description());

  SW.Send(ent->OfShape());

  SW.SendLogical(ent->ProductDefinitional());
}

//=================================================================================================

void RWStepRepr_RWPerpendicularTo::Share(const Handle(StepRepr_PerpendicularTo)& ent,
                                         Interface_EntityIterator&               iter) const
{

  // Inherited fields of ShapeAspect

  iter.AddItem(ent->OfShape());
}
